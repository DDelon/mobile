#include "servant/AdapterProxy.h"
#include "servant/Communicator.h"
#include "servant/StatReport.h"
#include "servant/Application.h"
#include "servant/AdminF.h"
#include "servant/AppCache.h"
#include "log/taf_logger.h"
#include "jce/wup.h"

namespace taf
{

AdapterProxy::AdapterProxy(Communicator* comm, const EndpointInfo &ep, ObjectProxy* op)
: _bUseBus(false)
, _comm(comm)
, _endpoint(ep)
, _objectProxy(op)
, _reactor(comm->fdReactor())
, _timeoutQueue(NULL)
, _trans(NULL)
, _closeTrans(false)
, _checkTransInterval(2)    //Ŀǰʵ��checkActive�������첽����, ���ҽ������ӵ�����Ӱ�쵱ǰ��η���,��˿��԰�Ƶ����ߵ�2s
, _lastCheckTransTime(0)
, _currentGridGroup(0)
, _timeoutInvoke(0)
, _bConnTimeout(false)
, _totalInvoke(0)
, _frequenceFailInvoke(0)
, _lastFinishInvokeTime(TC_TimeProvider::getInstance()->getNow())
, _lastRetryTime(0)
, _activeStatus(true)
,_activeStateInReg(true)
{
   _currentGridGroup = ep.grid();

   _timeoutQueue = ObjectProxy::createTimeoutQueue(_comm, _objectProxy->timeout());

   _trans = Connector::create(_objectProxy, _endpoint);

   if(_endpoint.type() == EndpointInfo::UDP)
   {
       _checkTransInterval = 10;    //udp�˿�10����һ��, ����Ӱ���û�����
   }
}

AdapterProxy::~AdapterProxy()
{
    delete _timeoutQueue;

    _timeoutQueue = NULL;

    if(_trans != NULL)
    {
        delete _trans;

        _trans = NULL;
    }
}

const EndpointInfo& AdapterProxy::endpoint()
{
    return _endpoint;
}

void AdapterProxy::setActive(bool value)
{
    _activeStatus = value;
}

int AdapterProxy::invoke(ReqMessagePtr& req)
{
    assert(_trans != NULL);

    //�������û�г�ʼ������������Ч
    if(!_trans->hasConnected() && !_trans->isConnecting() && !_trans->isBusConnecting())
    {
        LOG->error() << "[TAF][invoke," << _objectProxy->name() << ",select trans and retry fail,ep:" << _endpoint.desc() << "]" << endl;
        return -1;
    }

    if (req->fromRpc == false)
    {
        req->request.iRequestId = _objectProxy->getTimeoutQueue()->generateId();
    }

    //1.���û��·��״̬�������ObjectProxy������
    //2.�����HASH����·��״̬�������Adapter�Լ��Ķ�����
    //3.ע�⣬ʹ�õĶ���ObjectProxy��TimeoutQueue������id
    if (IS_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEHASH)
        || IS_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID))
    {
        _timeoutQueue->push(req, req->request.iRequestId);
    }
    else
    {
        _objectProxy->getTimeoutQueue()->push(req, req->request.iRequestId);
    }

    _reactor->notify(this);

    //����ͳ��
    _comm->sampleToStat(req, _objectProxy->name(), _endpoint.host());

    return 0;
}

bool AdapterProxy::finishConnect()
{
    int val = 0;

    socklen_t len = static_cast<socklen_t>(sizeof(int));

    if (::getsockopt(_trans->fd(), SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&val), &len) == -1 || val)
    {
        //����ʧ�ܣ����¼���Ӧ����ɾ�����¼�
        _reactor->unregisterAdapter(this, EPOLLIN | EPOLLOUT);

        _trans->close();

        _trans->setConnectFailed();

        LOG->error() << "[TAF][AdapterProxy finishConnect, exception:" << _endpoint.desc() << "," << strerror(val) << endl;

        return false;
    }
    else
    {
        _trans->setConnected();
		if(_objectProxy->getBusCommuEnable() && ClientConfig::SetLocalIp.count(_endpoint.host()) == 1)
		{
			if(!doBusReconnect())
            {
                _reactor->unregisterAdapter(this, EPOLLIN | EPOLLOUT);

				_trans->close();

				_trans->setConnectFailed();

				LOG->error() << "[TAF][AdapterProxy finishConnect, doBusReconnect error:" << _endpoint.desc() << endl;

				return false;
            }
		}

        _reactor->notify(this);

        LOGINFO("[TAF][AdapterProxy finishConnect, connect " << _endpoint.desc() << " succ" << endl);

        return true;
    }

    return false;
}

bool AdapterProxy::finishBusConnect()
{
	try
	{
		list<ResponsePacket> done;

		if (_trans->doResponse(done) > 0)
		{
			list<ResponsePacket>::iterator lit = done.begin();

			BusCommuData sKey;

			taf::JceInputStream<taf::BufferReader> is;
			is.setBuffer(lit->sBuffer);
			taf::Int32 iRet;
			is.read(iRet, 0, true);
			is.read(sKey, 2, true);

			if(iRet == 0)
			{
				BusCommuKey cKey;
				cKey.IdName      = ClientConfig::ModuleName;
				cKey.CommuKey    = AppCache::getInstance()->get(_objectProxy->name(), "BusCommu");
				cKey.ServantName = _objectProxy->name();

				if (cKey.CommuKey != sKey.CommuKey)
				{
					AppCache::getInstance()->set(_objectProxy->name(), sKey.CommuKey, "BusCommu");
				}

				_reactor->unregisterAdapter(this, EPOLLIN | EPOLLOUT);
				Transceiver *p = _trans;

				_bUseBus = true;
				_trans = new BusTransceiver(_objectProxy, _endpoint);
				delete p;

				((BusTransceiver *)_trans)->setc2s(sKey.c2sFifoName, sKey.c2sMmapName, sKey.c2sMmapSize);
				((BusTransceiver *)_trans)->sets2c(sKey.s2cFifoName, sKey.s2cMmapName, sKey.s2cMmapSize);
				((BusTransceiver *)_trans)->init();
				((BusTransceiver *)_trans)->setConnected();

				_reactor->registerAdapter(this, EPOLLIN | EPOLLOUT);
				//_comm->reportToStat(req, _objectProxy->name(), StatReport::STAT_SUCC);
				//return _trans;
			}

		}
	}
	catch (TC_Exception & ex)
    {
		_reactor->unregisterAdapter(this, EPOLLIN | EPOLLOUT);
        _trans->close();
        _trans->setConnectFailed();
        LOG->error() << "[TAF][doShmReconnect]Exception:" << ex.what() << endl;
		return false;
    }
	return true;
}

bool AdapterProxy::doBusReconnect()
{
    try
    {
		BusCommuKey cKey;
        cKey.IdName      = ClientConfig::ModuleName;
        cKey.CommuKey    = AppCache::getInstance()->get(_objectProxy->name(), "BusCommu");
        cKey.ServantName = _objectProxy->name();

		taf::JceOutputStream<taf::BufferWriter> os;
		os.write(cKey, 1);


		ReqMessagePtr req = new ReqMessage(_comm);
		req->proxy                  = _comm->getServantProxy(_objectProxy->name()).get();
        req->fromRpc                = false;
        req->request.iRequestId     = _objectProxy->getTimeoutQueue()->generateId();
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = taf::JCENORMAL;
        req->request.sServantName   = _objectProxy->name();
        req->request.sFuncName      = "taf_bus";
        req->request.sBuffer        = os.getByteBuffer();
        //req->request.context        = context;
        //req->request.status         = status;
        req->request.iTimeout       = 3000;
        req->type                   = ReqMessage::SYNC_CALL;
        req->response.iVersion      = -1; //ʹ�ø��ֶ��ж��Ƿ���response����
        req->response.iRet          = JCESERVERUNKNOWNERR;
		req->callback 				= NULL;
		TC_TimeProvider::getInstance()->getNow(&req->begtime);

        //req->monitor = new ReqMonitor;
		string s = "";

        _objectProxy->getProxyProtocol().requestFunc(req->request, s);

        _trans->writeToSendBuffer(s);

		_trans->setBusConnecting();

		_reactor->notify(this);
    }
    catch (TC_Exception & ex)
    {
        LOG->error() << "[TAF][doShmReconnect]Exception:" << ex.what() << endl;
		return false;
    }

    return true;
}

Transceiver* AdapterProxy::doReconnect()
{
    assert(_trans !=  NULL);

    try
    {
        if (_trans->isValid() && (_trans->hasConnected() == false))
        {
            //��ǰAdapter�������ӷ����У�û�п��õ�����
            LOGINFO("[TAF][doReconnect," << _objectProxy->name() << ",connecting :" << _endpoint.desc() << "]" << endl);
            return NULL;
        }


		if(hasBusCommu())
		{
			Transceiver *p = _trans;

			_bUseBus = false;
			_trans = Connector::create(_objectProxy, _endpoint);
			delete p;
		}

        //��δ��뷢��connect���󣬵���һ���ἰʱ������
        Connector::connect(_trans);

		LOGINFO("[TAF][doReconnect," << _objectProxy->name() << ",connect:" << _endpoint.desc() << ",fd:" << _trans->fd() << "]" << endl);

		//�����׽ӿ�ѡ��
		vector<SocketOpt> &socketOpts = _objectProxy->getSocketOpt();

		for(size_t i=0; i<socketOpts.size(); ++i)
		{
			int ret = setsockopt(_trans->fd(), socketOpts[i].level, socketOpts[i].optname, socketOpts[i].optval, socketOpts[i].optlen);

			if (-1 == ret)
			{
				LOG->error() << "[TAF][setsockopt error:" << NetworkUtil::errorToString(errno) << ","	<< _objectProxy->name() << "," << _endpoint.desc()
				              << ",fd:" << _trans->fd() << ", level:"<<  socketOpts[i].level<<",optname:" << socketOpts[i].optname << ",optval:" << socketOpts[i].optval <<"	]"<< endl;
			}
		}

        _reactor->registerAdapter(this, EPOLLIN | EPOLLOUT);

		if(_trans->hasConnected() && _objectProxy->getBusCommuEnable() && ClientConfig::SetLocalIp.count(_endpoint.host()) == 1)
		{
			if(!doBusReconnect())
			{
				_reactor->unregisterAdapter(this, EPOLLIN | EPOLLOUT);
				_trans->close();
				_trans->setConnectFailed();
				LOG->error() << "[TAF][AdapterProxy doReconnect, doBusReconnect error, " << _endpoint.desc() << endl;
			}
		}

        return _trans->hasConnected()?_trans:NULL;
    }
    catch (TafException& e)
    {
        LOG->error() << "[TAF][doReconnect," << _objectProxy->name() << ",connect fail:" << _endpoint.desc() << ",ex:" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][doReconnect," << _objectProxy->name() << ",connect fail,ep:" << _endpoint.desc() << "]" << endl;
    }

    return NULL;
}

bool AdapterProxy::popRequest(ReqMessagePtr& req)
{
    assert(_timeoutQueue != NULL);

    return _timeoutQueue->pop(req);
}

/**
 * ��Transceiver��Ӧ��EpollOut����ã���ȡ�����͵���Ϣ
 */
bool AdapterProxy::sendRequest()
{
	if(!_trans->hasConnected())
	{
		return false;
	}
    int len =  0;

    ReqMessagePtr req;

    //Adapter�Լ��������״̬����Ϣ����
    while (this->popRequest(req) || _objectProxy->popRequest(req))
    {
        //�����������ݵ�adapter
        req->adapter  = this;

        string s = "";

        _objectProxy->getProxyProtocol().requestFunc(req->request, s);

        _trans->writeToSendBuffer(s);

        LOGINFO("[TAF][sendRequest, " << _objectProxy->name() << ", " << _endpoint.desc() << ", id:" << req->request.iRequestId << ", len:" << s.length() << "]" << endl);

        len += s.length();
        //�ϲ�һ�η��͵İ� ���ϲ���8k ����첽ʱ�ͻ���Ч��
        if (_endpoint.type() == EndpointInfo::UDP || len > 8192)
        {
            break;
        }
    }
    return len>0;
}

void AdapterProxy::finishInvoke(bool bTimeout)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    time_t now = TC_TimeProvider::getInstance()->getNow();

    _frequenceFailInvoke = (bTimeout? (_frequenceFailInvoke + 1) : 0);

    //�����쳣״̬�����Գɹ�,�ָ�����״̬
    if (!_activeStatus && !bTimeout)
    {
        _activeStatus = true;

        _lastFinishInvokeTime = now;

        _frequenceFailInvoke = 0;

        _totalInvoke = 1;

        _timeoutInvoke = 0;

		_bConnTimeout = false;

        LOGINFO("[TAF][finishInvoke, " << _objectProxy->name() << ", " << _endpoint.desc() << ", retry ok]" << endl);

        return;
    }

    //�����쳣״̬������ʧ��
    //if (!_activeStatus && (now - _lastFinishInvokeTime >= _lastRetryTime))
	if (!_activeStatus)
    {
        LOGINFO("[TAF][finishInvoke, " << _objectProxy->name() << ", " << _endpoint.desc() << ", retry fail]" << endl);
        return;
    }

    ++_totalInvoke;

    if (bTimeout)
    {
        ++_timeoutInvoke;
		if(_trans->isConnecting() && !_bConnTimeout)
		{
			_bConnTimeout = true;
		}
    }

    CheckTimeoutInfo& info = _objectProxy->checkTimeoutInfo();

    //�ж��Ƿ���뵽��һ������ʱ��Ƭ
    uint32_t interval = uint32_t(now - _lastFinishInvokeTime);
    if (interval >= info.checkTimeoutInterval || (_frequenceFailInvoke >= info.frequenceFailInvoke && interval > 5)) //���ٴ���5��
    {
        _lastFinishInvokeTime = now;

        //�����Ƿ�ʱ��������������
        if (_timeoutInvoke >= info.minTimeoutInvoke && (_timeoutInvoke >= info.radio * _totalInvoke))
        {
            _activeStatus = false;

            LOG->error() << "[TAF][finishInvoke, "
                         << _objectProxy->name() << "," << _endpoint.desc()
                         << ",disable,freqtimeout:" << _frequenceFailInvoke
                         << ",timeout:"<< _timeoutInvoke
                         << ",total:" << _totalInvoke << "] " << endl;

            //�������� ���ε�
            //TAF_NOTIFY_ERROR(_endpoint.desc() + ": disabled because of too many timeout.");

            resetInvoke();
        }
        else
        {
            _frequenceFailInvoke = 0;

            _totalInvoke = 0;

            _timeoutInvoke = 0;
        }
    }
}

bool AdapterProxy::checkActive(bool bForceConnect)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    time_t now = TC_TimeProvider::getInstance()->getNow();

    LOGINFO("[TAF][checkActive,"
                << _objectProxy->name() << "," << _endpoint.desc() << ","
                << (_activeStatus ? "enable" : "disable")
                << (bForceConnect? ",forceConnect" : "")
                << ",freqtimeout:" << _frequenceFailInvoke
                << ",timeout:" << _timeoutInvoke
                << ",total:" << _totalInvoke << "]" << endl);

    if(bForceConnect)
    {
		if (_trans->isConnecting() || _trans->hasConnected())
		{
			return true;
		}

		_lastRetryTime = now;

		_lastCheckTransTime = now;

    	doReconnect();

		return _trans->hasConnected() | _trans->isConnecting() | _trans->isBusConnecting();
    }

    //ʧЧ��û�е��´�����ʱ��, ֱ�ӷ��ز�����
    if(!_activeStatus && (now - _lastRetryTime < (int)_objectProxy->checkTimeoutInfo().tryTimeInterval))
    {
        return false;
    }

    _lastRetryTime = now;

	//����û�н�������������Ч, ���½�������
    if( _lastCheckTransTime + _checkTransInterval < now && !_trans->isValid())
    {
        _lastCheckTransTime = now;

        doReconnect();
    }

	return _trans->hasConnected() | _trans->isConnecting() | _trans->isBusConnecting();
}

void AdapterProxy::resetInvoke()
{
    _lastFinishInvokeTime = TC_TimeProvider::getInstance()->getNow();

    _lastRetryTime = TC_TimeProvider::getInstance()->getNow();

    _frequenceFailInvoke = 0;

    _totalInvoke = 0;

    _timeoutInvoke = 0;

    //������Ҫ�ر�����
    setCloseTrans(true);

    //֪ͨ�����̹߳ر�����
    _reactor->notify(this);

    LOGINFO("[TAF][resetInvoke, " << _objectProxy->name() << ", " << _endpoint.desc() << ", close]" << endl);
}

bool AdapterProxy::dispatchPushCallback(ResponsePacket &rsp)
{
    ServantProxyCallback* cb = _objectProxy->getOneRandomPushCallback();

    if (cb)
    {
        ReqMessagePtr req = new ReqMessage(_comm);

        req->response = rsp;

        req->adapter  = this;

        cb->onDispatch(req);

        return true;
    }
    return false;
}

int AdapterProxy::finished(ResponsePacket &rsp)
{
    //����Ҷ�·��ѡȡ�ķ����״̬��ƥ�䣬�Է����Ϊ׼
    //reset,��Adapter������group��ת�Ƶ�����˷��ص�group��
    //���޸ı���Adapter::Endpoint��grid����
    if (rsp.iRet == JCESERVERRESETGRID)
    {
        const map<string, string>& respStatus = rsp.status;

        map<string, string>::const_iterator sit = respStatus.find(ServantProxy::STATUS_GRID_CODE);

        if (sit != respStatus.end())
        {
            int32_t newGrid = TC_Common::strto<int32_t>(sit->second);

            LOGINFO("[TAF][finished," << _objectProxy->name() << ",server reset grid msg:" << _currentGridGroup << "->" << newGrid << "]" << endl);

            if (_objectProxy->resetAdapterGrid(_currentGridGroup, newGrid, this))
            {
                _currentGridGroup = newGrid;
            }
        }
    }

    ReqMessagePtr ptr = NULL;

    //1.�����·��״̬�����AdapterProxy�Լ��Ķ����в���request
    //2.���û��·��״̬�����ObjectProxy�����в���request
    if (IS_MSG_TYPE(rsp.iMessageType, taf::JCEMESSAGETYPEHASH)
        || IS_MSG_TYPE(rsp.iMessageType, taf::JCEMESSAGETYPEGRID))
    {
        ptr = _timeoutQueue->get(rsp.iRequestId);
    }
    else
    {
        ptr = _objectProxy->getTimeoutQueue()->get(rsp.iRequestId);

        //����ǵ�����Э��������Ӧ���ᶪʧHASH״̬
        //����Ҫ���Դ�AdpaterProxy�Ķ�������getһ��
        if (!ptr && _timeoutQueue)
        {
            ptr = _timeoutQueue->get(rsp.iRequestId);
        }
    }

    if (!ptr)
    {
        if (!dispatchPushCallback(rsp))
        {
            LOG->error() << "[TAF][finished," << _objectProxy->name() << ",get req-ptr NULL,may be timeout,id:" << rsp.iRequestId << "]" << endl;
        }
        return -1;
    }

    ptr->response = rsp;

    //ͬ�����ã�����ServantProxy�߳�
    if (ptr->type == ReqMessage::SYNC_CALL)
    {
        return ptr->proxy->finished(ptr);
    }

    //�첽�ص�������ص������߳���
    if (ptr->callback)
    {
        _comm->asyncProcThread()->put(ptr);

        return 0;
    }

    LOG->error() << "[TAF][finished," << _objectProxy->name() << ",adapter proxy finish fail,id:" << rsp.iRequestId << ",ret:" << rsp.iRet << "]" << endl;

    return -1;
}

//////////////////////////////////////////////////////////////////////////////////
}
