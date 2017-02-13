﻿#include "servant/ServantProxy.h"
#include "servant/ServantHandle.h"
#include "servant/StatReport.h"
#include "servant/Application.h"
#include "servant/BaseF.h"
#include "log/taf_logger.h"

namespace taf
{

////////////////////////////////////////////////////////////////////////////

int32_t ThreadPrivateData::INVALID_HASH_CODE = -1;

int32_t ThreadPrivateData::INVALID_GRID_CODE = -1;

////////////////////////////////////////////////////////////////////////////

pthread_key_t ServantProxyThreadData::g_key = 0;

TC_ThreadMutex ServantProxyThreadData::mutex;

void ServantProxyThreadData::destructor(void* p)
{
    ServantProxyThreadData* td = (ServantProxyThreadData*)p;

    if (td)
    {
        delete td;

        td = NULL;
    }
}

ServantProxyThreadData* ServantProxyThreadData::getData()
{
    if(ServantProxyThreadData::g_key == 0)
    {
        TC_LockT<TC_ThreadMutex> lock(ServantProxyThreadData::mutex);

        //g_key 为0表示未初始化，因此可能会浪费掉第一个线程数据key
        if(ServantProxyThreadData::g_key == 0)
        {
            int ret = ::pthread_key_create(&ServantProxyThreadData::g_key, ServantProxyThreadData::destructor);

            if (ret != 0)
            {
                LOG->error() << "[TAF][ServantProxyThreadData pthread_key_create fail:" << errno << ":" << strerror(errno) << "]" << endl;

                return NULL;
            }
        }
    }
    ServantProxyThreadData* td = (ServantProxyThreadData*)TC_ThreadPool::getThreadData(ServantProxyThreadData::g_key);

    if (td == NULL)
    {
        TC_LockT<TC_ThreadMutex> lock(ServantProxyThreadData::mutex);

        if(td == NULL)
        {
            td              = new ServantProxyThreadData();
            td->_dataPtr    = new ThreadPrivateData();

            TC_ThreadPool::setThreadData(ServantProxyThreadData::g_key, td);
        }
    }
    return td;
}

ServantProxyThreadData::ServantProxyThreadData():_p(NULL)
{
}

const ThreadPrivateDataPtr & ServantProxyThreadData::data()
{
    return _dataPtr;
}

void ServantProxyThreadData::data(const ThreadPrivateDataPtr & dataPtr)
{
    _dataPtr = dataPtr;
}

void ServantProxyThreadData::setPrivatePointer(void *p)
{
    _p =  p;
}

void* ServantProxyThreadData::getPrivatePointer()
{
    return _p;
}

ServantProxyCallback::ServantProxyCallback()
{
    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    _dataPtr = new ThreadPrivateData(*(td->data()));
}

///////////////////////////////////////////////////////////////////////////////
//
string ServantProxy::STATUS_GRID_KEY    = "STATUS_GRID_KEY";
string ServantProxy::STATUS_DYED_KEY    = "STATUS_DYED_KEY";
string ServantProxy::STATUS_GRID_CODE   = "STATUS_GRID_CODE";
string ServantProxy::STATUS_SAMPLE_KEY  = "STATUS_SAMPLE_KEY";
string ServantProxy::STATUS_RESULT_CODE = "STATUS_RESULT_CODE";
string ServantProxy::STATUS_RESULT_DESC = "STATUS_RESULT_DESC";
//string ServantProxy::STATUS_TUNNEL_OBJ  = "STATUS_TUNNEL_OBJ";
string ServantProxy::STATUS_HASH_CODE  = "STATUS_HASH_CODE";

ServantProxy::ServantProxy()
: _comm(NULL)
, _objectProxy(NULL)
, _router(NULL)
, _timeout(DEFAULT_TIMEOUT)
{
}

ServantProxy::ServantProxy(Communicator* comm, ObjectProxy* op)
: _comm(comm)
, _objectProxy(op)
, _router(NULL)
, _timeout(DEFAULT_TIMEOUT)
{
}

ServantProxy::~ServantProxy()
{
}

void ServantProxy::taf_max_trans_num(int num)
{
    /*
    if (_objectProxy)
    {
        _objectProxy->setAdapterMaxTransNum(num);
    }
    */
}

int ServantProxy::taf_max_trans_num() const
{
    /*
    if (_objectProxy)
    {
        return _objectProxy->getAdapterMaxTransNum();
    }
    return 0;
    */
    return 1;
}


void ServantProxy::taf_name(const string &name)
{
    string s = name;
    string::size_type n = name.find_first_of('@');
    //直连
    if (n != string::npos)
    {
        s = name.substr(0, n);
    }
    _name = s;
}

string ServantProxy::taf_name() const
{
    if(!_name.empty())
    {
        return _name;
    }
    return "NULL";

    /*
    if (_objectProxy)
    {
        return _objectProxy->name();
    }
    return "NULL";
    */
}

bool ServantProxy::taf_tunnel_enabled() const
{
    if (_objectProxy)
    {
        return _objectProxy->getTunnelEnabled();
    }
    return false;
}


TC_Endpoint ServantProxy::taf_invoke_endpoint()
{
    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    if (td)
    {
        td->data()->_szHost[sizeof(td->data()->_szHost) - 1] = '\0';//防止被误操作，导致没有结束符

        return TC_Endpoint(td->data()->_szHost);
    }
    return TC_Endpoint();
}

//通信器取Endpoint与objectProxy直接取不一样，object可能是tunnel隧道obj

vector<TC_Endpoint> ServantProxy::getEndpoint()
{
    return _comm->getEndpoint(_name);
    /*
    if (_objectProxy)
    {
        return _objectProxy->getEndpoint();
    }
    return vector<TC_Endpoint>();
    */
}

vector<TC_Endpoint> ServantProxy::getEndpoint4All()
{
    return _comm->getEndpoint4All(_name);
    /*
    if (_objectProxy)
    {
        return _objectProxy->getEndpoint4All();
    }
    return vector<TC_Endpoint>();
    */
}

void ServantProxy::taf_endpoints(vector < EndpointInfo > & activeEndPoints, vector < EndpointInfo > & inactiveEndPoints)
{
    if (_objectProxy)
    {
        return _objectProxy->getEndpoint4All(activeEndPoints, inactiveEndPoints);
    }
}

vector<TC_Endpoint> ServantProxy::taf_endpoints(const std::string & sStation)
{
    if (_objectProxy)
    {
        vector<TC_Endpoint> vecActive;

        vector<TC_Endpoint> vecInactive;

        _objectProxy->getEndpoint4All(sStation, vecActive, vecInactive);

        return vecActive;
    }
    return vector<TC_Endpoint>();
}

void ServantProxy::taf_endpoints(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive)
{
    if (_objectProxy)
    {
        _objectProxy->getEndpoint4All(sStation, vecActive, vecInactive);
    }
}

void ServantProxy::taf_timeout(int msecond)
{
    _timeout = msecond;

    if (_timeout > _objectProxy->timeout())
    {
        _timeout = _objectProxy->timeout();
    }
}

int ServantProxy::taf_timeout() const
{
    return _timeout;
}

void ServantProxy::taf_set_protocol(const ProxyProtocol& protocol)
{
    _objectProxy->setProxyProtocol(protocol);
}

void ServantProxy::taf_set_sockopt(int level, int optname, const void * optval, socklen_t optlen)
{
   _objectProxy->setSocketOpt(level, optname, optval, optlen);
}

CheckTimeoutInfo& ServantProxy::taf_check_timeout()
{
    return _objectProxy->checkTimeoutInfo();
}

void ServantProxy::taf_bus_enable(bool enable)
{
    _objectProxy->setBusCommuEnable(enable);
}

void ServantProxy::taf_ping()
{
    vector<char> v;

    map<string, string> m;

    map<string, string> s;

    ResponsePacket rsp;

    taf_invoke(taf::JCENORMAL, "taf_ping", v, m, s, rsp);
}

void ServantProxy::taf_clear_hash()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    if (td)
    {
        td->data()->_hashCode = ThreadPrivateData::INVALID_HASH_CODE;
    }
}

ServantProxy* ServantProxy::taf_hash(int64_t key)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    if (td)
    {
        td->data()->_hashCode = key;
    }
    return this;
}

void ServantProxy::taf_set_router(const ServantProxyRouterPtr& router)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    _router = router;
}

void ServantProxy::taf_del_router()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    if (_router)
    {
        _router = NULL;
    }
}

uint32_t ServantProxy::taf_gen_requestid()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    return _objectProxy->getTimeoutQueue()->generateId();
}

void ServantProxy::taf_set_push_callback(const ServantProxyCallbackPtr& cb)
{
    _objectProxy->setPushCallbacks(cb);
}

/////////////////////////////////////////////////////////////////////////////////////
int ServantProxy::invoke(ReqMessagePtr& req)
{
    ServantProxyThreadData* td = ServantProxyThreadData::getData();

    assert(td != NULL);

    //如果是需要HASH的消息
    if (td->data()->_hashCode != ThreadPrivateData::INVALID_HASH_CODE)
    {
        req->hashCode = td->data()->_hashCode;

        SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEHASH);

        LOGINFO("[TAF][ServantProxy::invoke, " << _objectProxy->name() << ", hash:" << td->data()->_hashCode << "]" << endl);
    }

    //如果是需要染色的消息
    if (td->data()->_bDyeing)
    {
        LOGINFO("[TAF][" << __FILE__ << "|" << __LINE__ << "] set dyeing, key=" << td->data()->_dyeingKey << endl);

        SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEDYED);

        req->request.status[ServantProxy::STATUS_DYED_KEY] = td->data()->_dyeingKey;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    map<string, string>::iterator keyIt = req->request.status.find(ServantProxy::STATUS_GRID_KEY);

    //如果是需要灰度路由的接口
    if (keyIt != req->request.status.end())
    {
        string routeKey = keyIt->second;

        //如果业务设置了路由器，则首选使用路由器
        int32_t grid = ThreadPrivateData::INVALID_GRID_CODE;

        if (_router)
        {
            grid = _router->getGridByKey(routeKey);
        }

        //没有路由器或者路由器的getGridByKey返回无效状态(-1)
        if (grid == ThreadPrivateData::INVALID_GRID_CODE)
        {
            //如果之前的调用有路由状态(由server保存在线程数据中)
            //且是同一个用户(key相同),则传递灰度状态
            if (td && td->data()->_routeKey == routeKey && td->data()->_gridCode != ThreadPrivateData::INVALID_GRID_CODE)
            {
                grid = td->data()->_gridCode;
            }
        }

        if (grid != ThreadPrivateData::INVALID_GRID_CODE)
        {
            req->request.status[ServantProxy::STATUS_GRID_CODE] = TC_Common::tostr(grid);

            SET_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID);

            LOGINFO("[TAF][ServantProxy::invoke, " << _objectProxy->name() << ", grid:" << routeKey << "->" << grid << "]" << endl);
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////
    if (req->type == ReqMessage::SYNC_CALL)
    {
        req->monitor = new ReqMonitor;
    }

    if(_objectProxy->getTunnelEnabled())
    {
        //req->request.status[ServantProxy::STATUS_TUNNEL_OBJ] = _name;

        if (td->data()->_hashCode != ThreadPrivateData::INVALID_HASH_CODE)
        {
            req->request.status[ServantProxy::STATUS_HASH_CODE] = TC_Common::tostr(td->data()->_hashCode);
        }
    }

    TC_TimeProvider::getInstance()->getNow(&req->begtime);

    int ret = _objectProxy->invoke(req);

    string servantFunc = string("servant:") + req->request.sServantName + ",func:" + req->request.sFuncName;

    //调用在客户端就失败了(抛出异常)
    if (ret < 0)
    {
        //还没有进入TimeoutQueue，不能算超时(proxy==NULL，避免在Message.cpp析构时作为超时消息处理)
        req->proxy = NULL;

        ostringstream os;

        if(ret == -2)
        {
            os << "[ServantProxy::invoke fail,no valid servant," << servantFunc << "]";
        }
        else if(ret == -1)
        {
            os << "[ServantProxy::invoke connect fail," << servantFunc << "]";

            if(req->adapter) os << "adapter:" << req->adapter->endpoint().desc();
        }
        else
        {
            os << "[ServantProxy::invoke unknown fail," << servantFunc << "]";
        }

        //上报异常
        _comm->reportToStat(req, _objectProxy->name(), StatReport::STAT_EXCE);

        throw TafException(os.str());
    }

    if (req->type == ReqMessage::SYNC_CALL)
    {
        TC_ThreadLock::Lock lock(*(req->monitor));

        //如果还没有返回且等待超时
        if (req->response.iVersion == -1 && !req->monitor->timedWait(_timeout))
        {
            //上报超时统计
            _comm->reportToStat(req, _objectProxy->name(), StatReport::STAT_TIMEOUT);

            ostringstream os;

            os << "[ServantProxy::invoke timeout(" << _timeout << ")," << servantFunc;

            if(req->adapter) os << ",adapter:" << req->adapter->endpoint().desc();

            os << ",reqid:" << req->request.iRequestId << "]";

            throw TafSyncCallTimeoutException(os.str());
        }

        //上报成功统计
        if (req->response.iRet == JCESERVERSUCCESS)
        {
            snprintf(td->data()->_szHost, sizeof(td->data()->_szHost), "%s", req->adapter->endpoint().desc().c_str());

            _comm->reportToStat(req, _objectProxy->name(), StatReport::STAT_SUCC);

            return true;
        }

        //上报异常统计
        _comm->reportToStat(req, _objectProxy->name(), StatReport::STAT_EXCE);

        //根据服务端返回值抛出异常
        TafException::throwException(req->response.iRet, servantFunc + ",exception info:" + req->response.sResultDesc);
    }

    return false;
}

//同步调用返回，唤醒等待的业务线程
int ServantProxy::finished(ReqMessagePtr& req)
{
    if (req->monitor)
    {
        TC_ThreadLock::Lock sync(*(req->monitor));

        req->monitor->notify();

        return 0;
    }
    return -1;
}

//////////////////////////////////////////////////////////////////
void ServantProxy::taf_invoke_async(char cPacketType,
                                    const string &sFuncName,
                                    const vector<char>& buf,
                                    const map<string, string>& context,
                                    const map<string, string>& status,
                                    const ServantProxyCallbackPtr& callback)
{
    try
    {
        ReqMessagePtr req = new ReqMessage(_comm);

        req->proxy                  = this;
        req->fromRpc                = false;
        req->request.iRequestId     = 0;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = (callback ? cPacketType : JCEONEWAY);
        req->request.sServantName   = _name;
        req->request.sFuncName      = sFuncName;
        req->request.sBuffer        = buf;
        req->request.context        = context;
        req->request.status         = status;
        req->request.iTimeout       = _timeout;
        req->type                   = (callback ? ReqMessage::ASYNC_CALL : ReqMessage::ONE_WAY);
        req->callback               = callback;
        req->response.iVersion      = -1; //使用该字段判断是否有response返回
        req->response.iRet          = JCESERVERUNKNOWNERR;

        this->invoke(req);
    }
    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::taf_invoke_async exception.");
    }
}

void ServantProxy::taf_invoke(char cPacketType,
                              const string& sFuncName,
                              const vector<char>& buf,
                              const map<string, string>& context,
                              const map<string, string>& status,
                              ResponsePacket& rsp)
{
    ReqMessagePtr req = new ReqMessage(_comm);

    try
    {
        req->proxy                  = this;
        req->fromRpc                = false;
        req->request.iRequestId     = 0;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = cPacketType;
        req->request.sServantName   = _name;
        req->request.sFuncName      = sFuncName;
        req->request.sBuffer        = buf;
        req->request.context        = context;
        req->request.status         = status;
        req->request.iTimeout       = _timeout;
        req->type                   = ReqMessage::SYNC_CALL;
        req->response.iVersion      = -1; //使用该字段判断是否有response返回
        req->response.iRet          = JCESERVERUNKNOWNERR;

        this->invoke(req);

        rsp = req->response;
    }
    catch (TafSyncCallTimeoutException &e)
    {
        if(req->adapter)
        {
            req->adapter->finishInvoke(true);
        }
        throw e;
    }
    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::taf_invoke exception.");
    }

    if(req->adapter)
    {
        req->adapter->finishInvoke(false);
    }
}
//////////////////////////////////////////////////////////////////////////////
//服务端是非taf协议，通过rpc_call调用
void ServantProxy::rpc_call(uint32_t requestId, const string& sFuncName,
                            const char* buff, uint32_t len,
                            ResponsePacket& rsp)
{
    ReqMessagePtr req = new ReqMessage(_comm);

    try
    {
        req->proxy                  = this;
        req->fromRpc                = true;
        req->request.iRequestId     = requestId;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = JCENORMAL;
        req->request.sServantName   = _name;
        req->request.sFuncName      = sFuncName;
        req->request.iTimeout       = _timeout;
        req->type                   = ReqMessage::SYNC_CALL;
        req->response.iVersion      = -1; //使用该字段判断是否有response返回
        req->response.iRet          = JCESERVERUNKNOWNERR;

        req->request.sBuffer.resize(len);
        ::memcpy((taf::Char*)&req->request.sBuffer[0], buff, len);

        this->invoke(req);

        rsp = req->response;
    }
    catch (TafSyncCallTimeoutException &e)
    {
        if(req->adapter)
        {
            req->adapter->finishInvoke(true);
        }
        throw e;
    }

    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::rpc_call exception.");
    }

    if(req->adapter)
    {
        req->adapter->finishInvoke(false);
    }
}

void ServantProxy::rpc_call_async(uint32_t requestId, const string& sFuncName,
                                  const char* buff, uint32_t len,
                                  const ServantProxyCallbackPtr& callback)
{
    ReqMessagePtr req = new ReqMessage(_comm);

    try
    {
        req->proxy                  = this;
        req->fromRpc                = true;
        req->request.iRequestId     = requestId;
        req->request.iVersion       = JCEVERSION;
        req->request.cPacketType    = (callback ? JCENORMAL : JCEONEWAY);
        req->request.sServantName   = _name;
        req->request.sFuncName      = sFuncName;
        req->request.iTimeout       = _timeout;
        req->type                   = (callback ? ReqMessage::ASYNC_CALL : ReqMessage::ONE_WAY);
        req->callback               = callback;
        req->response.iVersion      = -1; //使用该字段判断是否有response返回
        req->response.iRet          = JCESERVERUNKNOWNERR;

        req->request.sBuffer.resize(len);
        ::memcpy((taf::Char*)&req->request.sBuffer[0], buff, len);

        this->invoke(req);
    }
    catch (TafException& e)
    {
        throw e;
    }
    catch (exception& e)
    {
        throw TafException(e.what());
    }
    catch (...)
    {
        throw TafException("ServantProxy::rpc_call_async exception.");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
}
