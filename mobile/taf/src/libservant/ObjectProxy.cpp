#include "servant/ObjectProxy.h"
#include "servant/Communicator.h"
#include "servant/QueryEndpoint.h"
#include "servant/Global.h"
#include "servant/AppCache.h"
#include "util/tc_common.h"
#include "util/tc_clientsocket.h"
#include "log/taf_logger.h"

namespace taf
{
///////////////////////////////////////////////////////////////////////////////////
AdapterProxyGroup::AdapterProxyGroup()
: lastRoundPosition(0)
{
}

AdapterProxy* AdapterProxyGroup::getHashProxy(int64_t hashCode)
{
    //hashֻ��active�ڵ㣬ȥ��inactive�ġ�
    /*
    if (allProxys.empty())
    {
        LOG->error() << "[TAF][getHashProxy allProxys is empty]" << endl;
        return NULL;
    }

    // 1 allProxys�ӿͻ�������֮�󣬾Ͳ����ٸı䣬�����нڵ�����
    // 2 ��������ӽڵ㣬��allProxys˳�����������,֮ǰ��hash��ı�
    // 3 �ڵ����ߺ���Ҫ�´������ͻ��˺�,allPorxys���ݲŻ���Ч
    size_t hash = ((int64_t)hashCode) % allProxys.size();

    //��hash���Ľڵ���������active�Ĳ���������
    if (allProxys[hash]->isActiveInReg() && allProxys[hash]->checkActive())
    {
        return allProxys[hash];
    }
    else
    { 
    */ 

        if(adapterProxys.empty())
        {
            LOG->error() << "[TAF][getHashProxy adapterProxys is empty]" << endl;
            return NULL;

        }
        //��active�ڵ����ٴ�hash
        vector<AdapterProxy*> thisHash = adapterProxys;
        vector<AdapterProxy*> conn;

        do
        {
            size_t hash = ((int64_t)hashCode) % thisHash.size();

            if (thisHash[hash]->checkActive())
            {
                return thisHash[hash];
            }
            if(!thisHash[hash]->isConnTimeout())
            {
                conn.push_back(thisHash[hash]);
            }
            thisHash.erase(thisHash.begin() + hash);
        }
        while(!thisHash.empty());

        if(conn.size() > 0)
        {
            //��������, ���ѡ��һ��û��connect��ʱ�ķ���
            AdapterProxy *adapterProxy = conn[((int64_t)hashCode) % conn.size()];

            //��proxy�����Ѿ�������,��������һ��
            adapterProxy->checkActive(true);
            return adapterProxy;
        }
        return NULL;
    //}
}

AdapterProxy* AdapterProxyGroup::getNextValidProxy()
{
    if (adapterProxys.empty())
    {
        LOG->error() << "[TAF][getNextValidProxy adapterProxys is empty]" << endl;
        return NULL;
    }

    vector<AdapterProxy*> conn;

    int32_t all = (int32_t)adapterProxys.size();

    while (all-- > 0)
    {
        ++lastRoundPosition;

        lastRoundPosition %= adapterProxys.size();

        if (adapterProxys[lastRoundPosition]->checkActive() == true)
        {
            return adapterProxys[lastRoundPosition];
        }
        if(!adapterProxys[lastRoundPosition]->isConnTimeout())
        {
            conn.push_back(adapterProxys[lastRoundPosition]);
        }
    }
    if(conn.size() > 0)
    {
        //��������, ���ѡ��һ��û��connect��ʱ�ķ���
        AdapterProxy *adapterProxy = conn[((uint32_t)rand() % conn.size())];

        //��proxy�����Ѿ�������,��������һ��
        adapterProxy->checkActive(true);
        return adapterProxy;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////
ObjectProxy::ObjectProxy(Communicator* comm, const string& name)
: _comm(comm)
, _bBusCommuEnable(false)
, _name(name)
, _timeout(5*1000)
, _isDirectProxy(false)
, _serverHasGrid(false)
, _lastRefreshEndpointTime(0)
, _refreshEndpointInterval(60*1000)
, _lastCacheEndpointTime(0)
, _cacheEndpointInterval(30*60*1000)
, _roundStartIndex(0)
, _timeoutQueue(NULL)
, _queryEndpoint(NULL)
, _bTunneleEnable(false)
, _lastRefreshEndpoint4AllTime(0)
{
    _proxyProtocol.requestFunc = ProxyProtocol::tafRequest;

    _proxyProtocol.responseFunc = ProxyProtocol::tafResponse;
}

ObjectProxy::~ObjectProxy()
{
    delete _timeoutQueue;

    _timeoutQueue = NULL;

    vector<AdapterProxy*>::iterator it = _adapters.begin();

    while (it != _adapters.end())
    {
        delete *it;

        ++it;
    }
    /*
    if( _queryEndpoint != NULL)
    {
        delete _queryEndpoint;
    }
    _queryEndpoint = NULL;
    */
}

TC_TimeoutQueue<ReqMessagePtr>* ObjectProxy::createTimeoutQueue(Communicator *comm, int timeout)
{
    int size = TC_Common::strto<int>(comm->getProperty("timeout-queue-size", "50000"));

    TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue = new TC_TimeoutQueue<ReqMessagePtr>(timeout, size); //Ĭ��hash_map size����Ϊ5W

    return timeoutQueue;
}

AdapterProxy* ObjectProxy::createAdapterProxy(Communicator* comm, const EndpointInfo &ep)
{
    AdapterProxy* ap = new AdapterProxy(comm, ep, this);

    _adapterGroups[ep.grid()].adapterProxys.push_back(ap);

    //��ʼ���������µĽڵ㲿��Żᵽ����
    _adapterGroups[ep.grid()].allProxys.push_back(ap);

    _adapters.push_back(ap);

    return ap;
}

void ObjectProxy::initialize()
{
    _timeoutQueue = createTimeoutQueue(_comm, _timeout);

    string::size_type n = _name.find_first_of('@');

    string endpoints;

    string name = _name;

    //[ֱ������]ָ�������IP�Ͷ˿��б�
    if (n != string::npos)
    {
        _name = name.substr(0, n);

        _isDirectProxy = true;

        endpoints = name.substr(n + 1);
    }
    //[�������]ͨ��registry��ѯ����˵�IP�Ͷ˿��б�
    //[�������]��һ��ʹ��cache
    else
    {
        _locator = _comm->getProperty("locator");

        if (_locator.find_first_not_of('@') == string::npos)
        {
            LOG->error() << "[Locator is not valid:" << _locator << "]" << endl;

            throw TafRegistryException("locator is not valid:" + _locator);
        }
        QueryFPrx prx = _comm->stringToProxy<QueryFPrx>(_locator);

        _queryEndpoint = new taf::QueryEndpoint(prx);

        endpoints = AppCache::getInstance()->get(_name,_locator);
    }

    vector<string> eps = TC_Common::sepstr<string>(endpoints, ":", false);

    for (uint32_t i = 0; i < eps.size(); ++i)
    {
        try
        {
            TC_Endpoint ep(eps[i]);

            EndpointInfo::EType type = (ep.isTcp() ? EndpointInfo::TCP : EndpointInfo::UDP);

            string sSetDivision;
            //����set������Ϣ
            if (!_isDirectProxy)
            {
                string sep = " -s ";

                size_t pos = eps[i].rfind(sep);

                if (pos != string::npos)
                {
                    sSetDivision = TC_Common::trim(eps[i].substr(pos+sep.size()));

                    size_t endPos = sSetDivision.find(" ");

                    if (endPos != string::npos)
                    {
                        sSetDivision = sSetDivision.substr(0, endPos);
                    }
                }
            }

            EndpointInfo epi(ep.getHost(), ep.getPort(), type, ep.getGrid(),sSetDivision,ep.getQos());

            _activeEndpoints.insert(epi);

            //����adapterproxy
            createAdapterProxy(_comm, epi);
        }
        catch (...)
        {
            LOG->error() << "[endpoints parse error:" << name << ":" << eps[i] << "]" << endl;
        }
    }

    sortEndpointInfos();

    _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);
}

int ObjectProxy::loadLocator()
{
    if(_isDirectProxy)
    {
        //ֱ������
        return 0;
    }

    string locator = _comm->getProperty("locator");

    if (locator.find_first_not_of('@') == string::npos)
    {
        LOG->error() << "[Locator is not valid:" << locator << "]" << endl;

        return -1;
    }

    QueryFPrx prx = _comm->stringToProxy<QueryFPrx>(locator);

    ((QueryEndpoint*)_queryEndpoint)->setLocatorPrx(prx);

    return 0;
}

ServantProxyCallback* ObjectProxy::getOneRandomPushCallback()
{
    if (!_pushCallbacks.empty())
    {
        return _pushCallbacks[((uint32_t)rand() % _pushCallbacks.size())].get();
    }
    return NULL;
}

void ObjectProxy::setPushCallbacks(const ServantProxyCallbackPtr& cb)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    _pushCallbacks.push_back(cb);
}

const string& ObjectProxy::name() const
{
    return _name;
}

int ObjectProxy::timeout() const
{
    return _timeout;
}

void ObjectProxy::setProxyProtocol(const ProxyProtocol& protocol)
{
    _proxyProtocol = protocol;
}

ProxyProtocol& ObjectProxy::getProxyProtocol()
{
    return _proxyProtocol;
}

void ObjectProxy::setSocketOpt(int level, int optname, const void *optval, socklen_t optlen)
{
    SocketOpt socketOpt;

    socketOpt.level        = level;
    socketOpt.optname = optname;
    socketOpt.optval     = optval;
    socketOpt.optlen     = optlen;

    _SocketOpts.push_back(socketOpt);
}

vector<SocketOpt>& ObjectProxy::getSocketOpt()
{
    return _SocketOpts;
}

CheckTimeoutInfo& ObjectProxy::checkTimeoutInfo()
{
    return _checkTimeoutInfo;
}

void ObjectProxy::timeout(int msec)
{
    //�������첽��ʱʱ�䲻��С��1��
    if (msec >= 1000)
    {
        _timeout = msec;
    }
}

vector<TC_Endpoint> ObjectProxy::getEndpoint()
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    refreshEndpointInfos();

    vector<TC_Endpoint>  v;

    for(set<EndpointInfo>::iterator it = _activeEndpoints.begin(); it != _activeEndpoints.end(); ++it)
    {
       TC_Endpoint ep(it->host(),it->port(),_timeout,it->type() == EndpointInfo::TCP?1:0,it->grid());

       v.push_back(ep);
    }
    return v;
}

vector<TC_Endpoint> ObjectProxy::getEndpoint4All()
{
    //ֱ�� ֱ�ӷ��س�ʼ��ʱ����б�
    if (_isDirectProxy)
    {
        return getEndpoint();
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);

    //����Ǽ�����ӣ�ͨ��registry��ѯ�����б�
    if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now) ||
        (_activeEndpoints4All.empty() && (_lastRefreshEndpoint4AllTime + 2 < now))) //2s��������
    {
        _lastRefreshEndpoint4AllTime = now;

        set<EndpointInfo> activeEndpoints = ((QueryEndpoint*)_queryEndpoint)->findObjectById(_name);

        if (!activeEndpoints.empty())
        {
            _activeEndpoints4All = activeEndpoints;
        }
    }

    vector<TC_Endpoint>  v;

    for (set<EndpointInfo>::iterator it = _activeEndpoints4All.begin(); it != _activeEndpoints4All.end(); ++it)
    {
       TC_Endpoint ep(it->host(),it->port(),_timeout,it->type() == EndpointInfo::TCP?1:0,it->grid(),it->qos());

       v.push_back(ep);
    }
    return v;
}

void ObjectProxy::getEndpoint4All(vector<EndpointInfo> &vecActive, vector<EndpointInfo> &vecInactive)
{
    vecActive.clear();

    vecInactive.clear();

    //ֱ�� ֱ�ӷ��س�ʼ��ʱ����б�
    if (_isDirectProxy)
    {
        set<EndpointInfo>::const_iterator siit =  _activeEndpoints.begin();

        while (siit != _activeEndpoints.end())
        {
            vecActive.push_back(*siit);

            ++siit;
        }
        return;
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);

     if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now)
         || (_allEndpoints.first.empty() && _allEndpoints.second.empty() && (_lastRefreshEndpoint4AllTime + 2 < now)))
     {
        _lastRefreshEndpoint4AllTime = now;

        set<EndpointInfo> setActive, setInactive;

        ((QueryEndpoint*)_queryEndpoint)->findObjectById4All(_name, setActive, setInactive);

        set<EndpointInfo>::const_iterator sait =  setActive.begin();

        while (sait != setActive.end())
        {
            vecActive.push_back(*sait);

            ++sait;
        }

        set<EndpointInfo>::const_iterator siit =  setInactive.begin();

        while (siit!= setInactive.end())
        {
            vecInactive.push_back(*siit);

            ++siit;
        }

        _allEndpoints.first = vecActive;

        _allEndpoints.second = vecInactive;

        return;
     }

    vecActive = _allEndpoints.first;

    vecInactive = _allEndpoints.second;
}

void ObjectProxy::getEndpoint4All(const string& sStation, vector<TC_Endpoint>& vecActive, vector<TC_Endpoint>& vecInactive)
{
    vecActive.clear();

    vecInactive.clear();

    //ֱ�� ֱ�ӷ��س�ʼ��ʱ����б�
    if (_isDirectProxy)
    {
        vecActive = getEndpoint();

        return;
    }

    time_t now = TC_TimeProvider::getInstance()->getNow();

    TC_LockT<TC_ThreadMutex> lock(*this);

    map<string, pair<vector<TC_Endpoint>, vector<TC_Endpoint> > >::iterator itEndpoint = _mapEndpoints.find(sStation);

    if ((_lastRefreshEndpoint4AllTime + _refreshEndpointInterval/1000 < now)
         || (itEndpoint == _mapEndpoints.end() && (_lastRefreshEndpoint4AllTime + 2 < now)))
    {
        set<EndpointInfo> setActive, setInactive;

        ((QueryEndpoint*)_queryEndpoint)->findObjectByStation(_name, sStation, setActive, setInactive);

        for (set<EndpointInfo>::iterator it = setActive.begin(); it != setActive.end(); ++it)
        {
            TC_Endpoint ep(it->host(), it->port(), _timeout, it->type() == EndpointInfo::TCP?1:0, it->grid());

            vecActive.push_back(ep);
        }

        for (set<EndpointInfo>::iterator it = setInactive.begin(); it != setInactive.end(); ++it)
        {
            TC_Endpoint ep(it->host(), it->port(), _timeout, it->type() == EndpointInfo::TCP?1:0, it->grid());

            vecInactive.push_back(ep);
        }

        _mapEndpoints[sStation].first = vecActive;

        _mapEndpoints[sStation].second= vecInactive;

        return ;
    }

    vecActive = _mapEndpoints[sStation].first;

    vecInactive = _mapEndpoints[sStation].second;
}

int ObjectProxy::refreshEndpointInterval() const
{
    return _refreshEndpointInterval;
}

void ObjectProxy::refreshEndpointInterval(int msecond)
{
    //������ˢ�·�����б��ʱ�䲻��С��1��
    _refreshEndpointInterval = msecond > 1000 ? msecond : _refreshEndpointInterval;
}

void ObjectProxy::cacheEndpointInterval(int msecond)
{
    //������cache������б��ʱ�䲻��С��60�� Ĭ��30����
    _cacheEndpointInterval = msecond > 1000*60 ? msecond : _cacheEndpointInterval;
}


int ObjectProxy::invoke(ReqMessagePtr& req)
{
    //ѡ��һ��Զ�̷����Adapter������
    AdapterProxy* adp = selectAdapterProxy(req);

    if (!adp)
    {
        LOG->error() << "[TAF][invoke, " << _name << ", select adapter proxy ret NULL]" << endl;

        return -2;
    }
    req->adapter = adp;
    return adp->invoke(req);
}

//�ɺ�����AdapterProxyȡ��һ����Ϣ����
bool ObjectProxy::popRequest(ReqMessagePtr& req)
{
    //��FIFO�����л�ȡһ����Ϣ���з���
    return _timeoutQueue->pop(req);
}

void ObjectProxy::mergeEndPoint(set<EndpointInfo> &del, set<EndpointInfo> &add)
{
    LOGINFO("[TAF][mergeEndPoint," << _name << ", add:" << add.size() << ",del:" << del.size() << ",isGrid:" << _serverHasGrid << "]" << endl);

    //ɾ��������Ч�ķ���ڵ�
    for (set<EndpointInfo>::iterator it = del.begin(); it != del.end(); ++it)
    {
        map<int32_t, AdapterProxyGroup>::iterator mit = _adapterGroups.find(it->grid());

        if (mit == _adapterGroups.end())
        {
            continue;
        }

        vector<AdapterProxy*>::iterator vit = mit->second.adapterProxys.begin();

        while (vit != mit->second.adapterProxys.end())
        {
            if ((*it) == (*vit)->endpoint())
            {
                //�ýڵ������ص�״̬Ϊinactive
                (*vit)->setActiveInReg(false);

                //����ɾ���Ķ���, ��delete, ������߳�crash, ������һ�����񲻻���̫������ɾ����adapter
                mit->second.adapterProxysDeleted.push_back(*vit);

                mit->second.adapterProxys.erase(vit);

                break;
            }
            ++vit;
        }
    }

    //��ӱ��ػ�û�еĽڵ�
    for (set<EndpointInfo>::iterator it = add.begin(); it != add.end(); ++it)
    {
        map<int32_t, AdapterProxyGroup>::iterator mit = _adapterGroups.find(it->grid());

        if (mit == _adapterGroups.end())
        {
            //�����µ�adapterproxy
            createAdapterProxy(_comm, *it);
        }
        else
        {
            vector<AdapterProxy*>::iterator vit = mit->second.adapterProxysDeleted.begin();

            //��ʶ��delete�����Ƿ��Ѿ���Ҫ��ӵĽڵ�
            bool bFoundInDeletedGrp = false;

            while (vit != mit->second.adapterProxysDeleted.end())
            {
                //if ((*it).equalNoSetInfo((*vit)->endpoint()))
                if ((*it) == (*vit)->endpoint())
                {
                    //�ýڵ������ص�״̬Ϊactive
                    (*vit)->setActiveInReg(true);
                    _adapterGroups[it->grid()].adapterProxys.push_back(*vit);
                    mit->second.adapterProxysDeleted.erase(vit);
                    bFoundInDeletedGrp = true;
                    break;
                }
                ++vit;
            }

            //������, ������adapter
            //(��Ҫ��������ͬʱ�жϣ���������ڴ�й©����)
            if (!bFoundInDeletedGrp && vit == mit->second.adapterProxysDeleted.end())
            {
                createAdapterProxy(_comm, *it);
            }
        }
    }

    //�����µ��б����������´�ˢ��ʱʹ��
    _activeEndpoints.clear();

    for (map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.begin(); it != _adapterGroups.end(); ++it)
    {
        for (uint32_t i = 0; i < it->second.adapterProxys.size(); ++i)
        {
            _activeEndpoints.insert(it->second.adapterProxys[i]->endpoint());
        }

        LOGINFO("[TAF][mergeEndPoint," << _name << ", grid:" << it->first << ",proxy:" << it->second.adapterProxys.size() << ",delete proxy:" << it->second.adapterProxys.size()  << "]" << endl);

    }
}

void ObjectProxy::sortEndpointInfos()
{
    for (map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.begin(); it != _adapterGroups.end(); ++it)
    {
        map<string, AdapterProxy*> mpAdapter;

        for (uint32_t i = 0; i < it->second.adapterProxys.size(); ++i)
        {
            mpAdapter.insert(pair<string, AdapterProxy*>(it->second.adapterProxys[i]->endpoint().desc(), it->second.adapterProxys[i]));
        }

        it->second.adapterProxys.clear();
        for (map<string, AdapterProxy*>::iterator mit = mpAdapter.begin(); mit != mpAdapter.end(); ++mit)
        {
            it->second.adapterProxys.push_back(mit->second);
        }
        LOGINFO("adapterProxys has : " << it->second.adapterProxys.size() << endl);

        map<string, AdapterProxy*> mpAllAdapter;
        for (uint32_t i = 0; i < it->second.allProxys.size(); ++i)
        {
            mpAllAdapter.insert(pair<string, AdapterProxy*>(it->second.allProxys[i]->endpoint().desc(), it->second.allProxys[i]));
        }

        it->second.allProxys.clear();

        for (map<string, AdapterProxy*>::iterator mit = mpAllAdapter.begin(); mit != mpAllAdapter.end(); ++mit)
        {
            //inactive�Ľڵ㶼��mergeEndPoint��������setActiveInReg
            it->second.allProxys.push_back(mit->second);
        }
        LOGINFO("allProxys has : " << it->second.allProxys.size() << endl);
    }
}

void ObjectProxy::refreshEndpoints(const set<EndpointInfo>& vecActive, const set<EndpointInfo>& vecInactive)
{
    /**
     * ɾ��allProxys�Ѿ����ߵĽڵ�,
     * �ж��Ѿ����ߵ�������:
     * (1)��allProxys��
     * (2)������״̬Ϊinactive
     * (3)����vecInactive�еĽڵ�
     */
    for (map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.begin(); it != _adapterGroups.end(); ++it)
    {
        //(1)��allProxys��
        vector<AdapterProxy*> vTempAdapter;
        bool bDelete = false;
        vector<AdapterProxy*>::iterator vit = it->second.allProxys.begin();
        while(vit != it->second.allProxys.end())
        {
            //(2)������״̬Ϊinactive
            if (!(*vit)->isActiveInReg())
            {
                bool bInvecInactive = false;
                for (set<EndpointInfo>::iterator itIn = vecInactive.begin(); itIn != vecInactive.end(); ++itIn)
                {
                    if((*itIn) == (*vit)->endpoint())
                    {
                        bInvecInactive = true;
                        break;
                    }
                }
                //(3)����vecInactive�еĽڵ�,��Ϊ�ڵ��Ѿ�������,ɾ��
                if(!bInvecInactive)
                {
                    bDelete = true;
                    LOG->debug()<<"refreshEndpoints: delete adaptproxy :"<<(*vit)->endpoint().desc()<<endl;
                }
                else
                {
                    vTempAdapter.push_back(*vit);
                }
            }
            else
            {
                vTempAdapter.push_back(*vit);
            }
            vit++;
        }
        //�и���,size>0��������
        if(bDelete && vTempAdapter.size() > 0)
        {
            it->second.allProxys.swap(vTempAdapter);
        }
    }
}

//��ʱˢ�·����б�
void ObjectProxy::refreshEndpointInfos()
{
    try
    {
        //�����ֱ���ģ����ó�ʼ��ʱ��ķ����б�
        //��Ϊ����ɾ����Ч��AdapterProxy�����Բ���ˢ��
        if (_isDirectProxy)
        {
            return;
        }

        time_t now = TC_TimeProvider::getInstance()->getNow();

        //����Ǽ�����ӣ�ͨ��registry��ʱ��ѯ�����б�
        if ((_lastRefreshEndpointTime + _refreshEndpointInterval/1000 < now) ||
            (_adapterGroups.empty() && (_lastRefreshEndpointTime + 2 < now))) //2s��������
        {
            _lastRefreshEndpointTime = now;

            //��ʼ��ʱʹ��ͬ�����ã��Ժ����첽ˢ��
            bool sync = (_activeEndpoints.empty() || (_lastCacheEndpointTime == 0));
            if(ClientConfig::SetOpen)
            {
                ((QueryEndpoint*)_queryEndpoint)->findObjectByIdInSameSet(_name, ClientConfig::SetDivision, sync);
            }
            else
            {
                ((QueryEndpoint*)_queryEndpoint)->findObjectById4All(_name, sync);
            }
        }

        //�������ж��Ƿ��Ѿ���ˢ��
        if (!((QueryEndpoint*)_queryEndpoint)->hasRefreshed())
        {
            return;
        }

        set<EndpointInfo> activeEps;

        set<EndpointInfo> inactiveEps;

        set<int32_t> allGridCodes;

        //�첽���û�û��callback����
        if (!((QueryEndpoint*)_queryEndpoint)->hasNewEndpoints(activeEps, inactiveEps, allGridCodes))
        {
            LOGINFO("[TAF][refreshEndpointInfos,`findObjectById4All` hasNewEndpoints false:" << _name << "]" << endl);

            return;
        }

        //���registry����Active�����б�Ϊ�գ���������
        if (activeEps.empty())
        {
            LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` ret activeEps is empty:" << _name << "]" << endl;

            return;
        }

        set<EndpointInfo> del; //��Ҫɾ���ķ����ַ

        set<EndpointInfo> add; //��Ҫ���ӵķ����ַ

        set_difference(_activeEndpoints.begin(), _activeEndpoints.end(), activeEps.begin(), activeEps.end(), inserter(del,del.begin()));

        set_difference(activeEps.begin(), activeEps.end(), _activeEndpoints.begin(), _activeEndpoints.end(), inserter(add,add.begin()));

        //��������ֻ��һ��״̬�������·������
        _serverHasGrid = (allGridCodes.size() > 1 ? true : false);

        //û����Ҫ���µķ���ڵ�
        if (del.empty() && add.empty())
        {
            //need to refresh
            refreshEndpoints(activeEps,inactiveEps);
            return;
        }

        //�ϲ��б�
        mergeEndPoint(del, add);

        sortEndpointInfos();
        //allProxys refresh
        refreshEndpoints(activeEps,inactiveEps);
    }
    catch (exception& e)
    {
        LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` exception:" << _name << ":" << e.what() << "]" << endl;
    }
    catch (...)
    {
        LOG->error() << "[TAF][refreshEndpointInfos,`findObjectById4All` exception:" << _name << "]" << endl;
    }
}

//��ָ����һ��adapter��ѡȡһ����Ч��
AdapterProxy* ObjectProxy::selectFromGroup(const ReqMessagePtr& req, AdapterProxyGroup& group)
{
    //�����hash������ʹ��hash����
    if (req->hashCode != ThreadPrivateData::INVALID_HASH_CODE)
    {
        return group.getHashProxy(req->hashCode);
    }

    return group.getNextValidProxy();
}

void ObjectProxy::setEndPointToCache()
{
    time_t now = TC_TimeProvider::getInstance()->getNow();

    //��ֱ������Ҫ��ʱ���»�������б�
    if (!_isDirectProxy && _lastCacheEndpointTime + _cacheEndpointInterval/1000 < now)
    {
        _lastCacheEndpointTime = now;

        string sEndpoints = "";

        for (set<EndpointInfo>::iterator it = _activeEndpoints.begin(); it != _activeEndpoints.end(); ++it)
        {
            bool isTcp = (it->type() == EndpointInfo::TCP ? true : false);

            TC_Endpoint ep(it->host(), it->port(), _timeout, isTcp, it->grid());

            if (sEndpoints != "") sEndpoints += ":";

            sEndpoints += ep.toString();

            if (!it->setDivision().empty())
            {
                sEndpoints += " -s " + it->setDivision();
            }
        }

        AppCache::getInstance()->set(_name,sEndpoints,_locator);

        LOGINFO("[TAF][setEndPointToCache,obj:" << _name << ",endpoint:" << sEndpoints << "]" << endl);
    }
}

void ObjectProxy::handleQueueTimeout()
{
    handleQueueTimeout(_timeoutQueue);

    TC_LockT<TC_ThreadMutex> lock(*this);

    vector<AdapterProxy*>::iterator it = _adapters.begin();

    while(it != _adapters.end())
    {
        handleQueueTimeout((*it)->getTimeoutQueue());

        ++it;
    }
}

void ObjectProxy::handleQueueTimeout(TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue)
{
    static TC_TimeoutQueue<ReqMessagePtr>::data_functor df(&ReqMessage::timeout);

    timeoutQueue->timeout(df);
}

//�ӿ��õķ����б�ѡ��һ������ڵ�
AdapterProxy* ObjectProxy::selectAdapterProxy(const ReqMessagePtr& req)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    refreshEndpointInfos();

    //û�п��õķ����б�ֱ�ӷ���NULL��ҵ����յ��쳣
    if (_adapterGroups.empty())
    {
        LOG->error() << "[TAF][selectAdapterProxy," << _name << ",adapter proxy groups is empty!]" << endl;

        return NULL;
    }

    //��������б��ļ�
    setEndPointToCache();

    int32_t gridCode = 0;

    bool isValidGrid = false;

    string gridKey = "";

    const map<string, string>& status = req->request.status;

    //����ǻҶ�·����Ϣ���������Ч��
    if (IS_MSG_TYPE(req->request.iMessageType, taf::JCEMESSAGETYPEGRID))
    {
        map<string, string>::const_iterator keyIt = status.find(ServantProxy::STATUS_GRID_KEY);

        map<string, string>::const_iterator codeIt = status.find(ServantProxy::STATUS_GRID_CODE);

        if (keyIt != status.end() && codeIt != status.end())
        {
            gridCode = TC_Common::strto<int32_t>(codeIt->second);

            if (gridCode != ThreadPrivateData::INVALID_GRID_CODE)
            {
                isValidGrid = true;
            }
        }
    }

    //��Ч��·����Ϣ���ҷ�����ж���״̬
    if (isValidGrid == true && _serverHasGrid == true)
    {
        map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.find(gridCode);

        if (it != _adapterGroups.end())
        {
            return selectFromGroup(req, it->second);
        }

        LOG->error() << "[TAF][selectAdapterProxy," << _name << ",grid router fail,gridKey:" << gridKey << "->gridCode:" << gridCode << "]" << endl;

        return NULL;
    }

    //��·����Ϣ������ֻ��һ��״̬(�п�������reset��Ӧ)
    return selectFromGroup(req, _adapterGroups.begin()->second);
}

//�Ҷ�·�ɷ��͵�����ˣ�����˼�����״̬��ƥ�䣬�򷵻�reset��Ϣ����ȷ��״̬
//�ͻ��˽��÷����adapterת�Ƶ���ȷ��group�����ǲ���ı䱾��adapter��endpoint����
//ע�⣺�������˼��״̬��ƥ�䣬���Ƿ����״̬Ϊ0���򲻻᷵��reset
bool ObjectProxy::resetAdapterGrid(int32_t gridFrom, int32_t gridTo, AdapterProxy* adapter)
{
    TC_LockT<TC_ThreadMutex> lock(*this);

    LOGINFO("[TAF][resetAdapterGrid," << _name << ",reset adapter grid:" << gridFrom << "->" << gridTo <<":" << adapter->endpoint().desc() << "]" << endl);

    map<int32_t, AdapterProxyGroup>::iterator it = _adapterGroups.find(gridFrom);

    if (gridFrom == gridTo || it == _adapterGroups.end())
    {
        return false;
    }

    vector<AdapterProxy*>& vap = it->second.adapterProxys;

    for (vector<AdapterProxy*>::iterator vit = vap.begin(); vit != vap.end(); ++vit)
    {
        if ((*vit)->endpoint() == adapter->endpoint())
        {
            //������ȷ��group
            _adapterGroups[gridTo].adapterProxys.push_back(*vit);

            //�����е�group��ɾ��
            vap.erase(vit);

            _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);

            return true;
        }
    }

    vector<AdapterProxy*>& vAllap = it->second.allProxys;

    for (vector<AdapterProxy*>::iterator vit = vAllap.begin(); vit != vAllap.end(); ++vit)
    {
        if ((*vit)->endpoint() == adapter->endpoint())
        {
            //������ȷ��group
            _adapterGroups[gridTo].allProxys.push_back(*vit);

            //�����е�group��ɾ��
            vAllap.erase(vit);

            _serverHasGrid = (_adapterGroups.size() > 1 ? true : false);

            return true;
        }
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////////////
}
