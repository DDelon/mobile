#ifndef __TAF_OBJECT_PROXY_H_
#define __TAF_OBJECT_PROXY_H_

#include "servant/Communicator.h"
#include "servant/Message.h"
#include "servant/AdapterProxy.h"
#include "servant/EndpointInfo.h"
#include "servant/EndpointF.h"
#include "servant/AppProtocol.h"
#include "servant/Global.h"

namespace taf
{
class QueryEndpointImp;

/**
 * ���ڱ������˵�һ���б�
 * (������˻Ҷ�״̬���з���)
 */
struct AdapterProxyGroup
{
    AdapterProxyGroup();

    vector<AdapterProxy*> adapterProxys;

    vector<AdapterProxy*> adapterProxysDeleted;

    vector<AdapterProxy*> allProxys;

    int32_t lastRoundPosition;

    AdapterProxy* getHashProxy(int64_t hashCode);

    AdapterProxy* getNextValidProxy();
};

struct SocketOpt
{
    int level;

    int optname;

    const void *optval;

    socklen_t optlen;
};

///////////////////////////////////////////////////////////////////
/**
 * ÿ��objectname�ڽ��̿ռ�Ψһ��һ��objectproxy
 * �����շ�����Ϣ����
 */
class ObjectProxy : public TC_HandleBase, public TC_ThreadMutex //public BaseProxy
{
public:
    /**
     * ���캯��
     * @param comm
     * @param name
     */
    ObjectProxy(Communicator* comm, const string& name);

    /**
     * ��������
     */
    ~ObjectProxy();

    /**
     * ����timeoutqueue
     *
     * @param comm
     * @param timeout
     *
     * @return TC_TimeoutQueue<ReqMessagePtr>*
     */
    static TC_TimeoutQueue<ReqMessagePtr>* createTimeoutQueue(Communicator *comm, int timeout);

    /**
     * ��ʼ����Ϣ���е�
     */
    void initialize();

    /**
     * ��������
     * @param req
     * @return int
     */
    int invoke(ReqMessagePtr& req);

    /**
     * �Ӷ����л�ȡһ������
     * @param req
     * @return bool
     */
    bool popRequest(ReqMessagePtr& req);

    /**
     * ��ȡobject����
     * @return const string&
     */
    const string& name() const;

    /**
     * ��ȡ��Ϣ���г�ʱ����ʱ�����Ϣ������
     * @return int
     */
    int timeout() const;

    /**
     * ������Ϣ���г�ʱ����ʱ�����Ϣ������
     * @param msec
     */
    void timeout(int msec);

    /**
     * ��ʱ���Ի�ȡ������
     * @return CheckTimeoutInfo&
     */
    CheckTimeoutInfo& checkTimeoutInfo();

    /**
     * ����Э�������
     * @return UserProtocol&
     */
    void setProxyProtocol(const ProxyProtocol& protocol);

    /**
     * ��ȡЭ�������
     * @return ProxyProtocol&
     */
    ProxyProtocol& getProxyProtocol();

    /**
    *�����׽ӿ�ѡ��
    */
    void setSocketOpt(int level, int optname, const void *optval, socklen_t optlen);

     /**
     *��ȡ�׽���ѡ��
     */
     vector<SocketOpt>& getSocketOpt();

    /**
     * ��ȡ���÷����б� ������÷���,ֻ����ͬ����ķ����ip
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint();

    /**
     * ��ȡ���з����б�  ��������IDC
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All();

    /**
     * ��ȡ���з����б�  ��������ID�����ͷǻ����
     * @param vecActive    ����񼯺�
     * @param vecInactive �ǻ���񼯺�
     */
    void getEndpoint4All(vector<EndpointInfo> & vecActive, vector<EndpointInfo> & vecInactive);

    /**
     * ��ȡ���з����б� ����ָ��������
     *  @return void
     */
    void getEndpoint4All(const std::string & sStation, vector<TC_Endpoint> & vecActive, vector<TC_Endpoint> & vecInactive);

    /**
     * ��ȡˢ�·����б�ļ��
     * @return int
     */
    int refreshEndpointInterval() const;

    /**
     * ����ˢ�·����б�ļ��
     * @param msecond
     */
    void refreshEndpointInterval(int msecond);

     /**
     * ���û�������б�ļ��
     * @param msecond
     */
    void cacheEndpointInterval(int msecond);

    /**
     * ���س�ʱ���ж���
     */
    inline TC_TimeoutQueue<ReqMessagePtr>* getTimeoutQueue() { return _timeoutQueue; }

    /**
     * ����AdapterProxy�ĻҶ�״̬
     * @param gridFrom
     * @param gridTo
     * @param adapter
     * @return bool
     */
    bool resetAdapterGrid(int32_t gridFrom, int32_t gridTo, AdapterProxy* adapter);

    /**
     * ����locator
     * @param
     * @return AdapterPrx
     */
    int loadLocator();

    /**
     * ����PUSH����Ϣ��callback����
     * @param cb
     */
    void setPushCallbacks(const ServantProxyCallbackPtr& cb);

    /**
     * ��ȡPUSH����Ϣ��callback����
     */
    ServantProxyCallback* getOneRandomPushCallback();

    /**
     * ������г�ʱ
     */
    void handleQueueTimeout();

    /**
     * �˳�ʱ��������ͬ�����õ�����
     */
    void handleQuitReqMessage();
    /**
     * �����Ƿ���ù����ڴ�ͨ��
     */
    void setBusCommuEnable(bool enable) { _bBusCommuEnable = enable; }

    /**
     * ����Ƿ�����ù����ڴ�ͨ��
     */
    bool getBusCommuEnable() { return _bBusCommuEnable; }


    /**
     * ����ʹ��tunnel
     */
    void setTunnelEnabled(bool enable) { _bTunneleEnable = enable; }

    /**
     * ����Ƿ�����ʹ��tunnel
     */
    bool getTunnelEnabled() { return _bTunneleEnable; }


protected:
    /**
     * ��ʱˢ�·����б���registry��
     */
    void refreshEndpointInfos();

    /**
     * ��������б�
     */
    void sortEndpointInfos();

    /**
     * ˢ�±��ػ����б�������������ˣ�Ҫ��ɾ�����ػ���
     */
    void refreshEndpoints(const set<EndpointInfo>& vecActive, const set<EndpointInfo>& vecInactive);
    //void printEndpointInfos();

    /**
     * ��ʼ���г�ʱ
     *
     * @param timeoutQueue
     */
    void handleQueueTimeout(TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue);

    /**
     * �˳�ʱ��������ͬ�����õ�����
     *
     * @param timeoutQueue
     */
    void handleQuitReqMessage(TC_TimeoutQueue<ReqMessagePtr>* timeoutQueue);

    /**
     * ѡȡһ��AdapterProxy
     * @param req
     * @return AdapterPrx
     */
    AdapterProxy* selectAdapterProxy(const ReqMessagePtr& req);

    /**
     * ����adapterproxy
     *
     * @param comm
     * @param ep
     * @param op
     * @return AdapterProxy*
     */
    AdapterProxy* createAdapterProxy(Communicator* comm, const EndpointInfo &ep);

    /**
     * ��ѯѡȡ
     * @param adapters
     * @return AdapterPrx
     */
    AdapterProxy* selectFromGroup(const ReqMessagePtr& req, AdapterProxyGroup& group);

    /**
     * ����adapter���ļ�
     */
    void setEndPointToCache();

    /**
     * �ϲ������б�
     *
     * @param del
     * @param add
     */
    void mergeEndPoint(set<EndpointInfo> &del, set<EndpointInfo> &add);

private:
    Communicator* _comm;

    bool    _bBusCommuEnable;

    string _name;

    string _locator;

    int32_t _timeout;

    //int32_t _maxTransNum;

    string _queryFObjectName;

    bool _isDirectProxy;

    bool _serverHasGrid;

    time_t _lastRefreshEndpointTime;

    int32_t _refreshEndpointInterval;

    time_t _lastCacheEndpointTime;

    int32_t _cacheEndpointInterval;

    int32_t _roundStartIndex;

    TC_TimeoutQueue<ReqMessagePtr>* _timeoutQueue;

    ServantProxyCallback* _queryEndpoint;

    set<EndpointInfo> _activeEndpoints; //���ڵ�

    map<int32_t, AdapterProxyGroup> _adapterGroups; //���ջҶ�״̬����adapterproxy

    vector<AdapterProxy*>           _adapters;

    CheckTimeoutInfo _checkTimeoutInfo;

    ProxyProtocol _proxyProtocol;

    vector<SocketOpt> _SocketOpts;

    vector<ServantProxyCallbackPtr> _pushCallbacks;

    bool  _bTunneleEnable;

private:
    int32_t _lastRefreshEndpoint4AllTime;

    set<EndpointInfo> _activeEndpoints4All; //���д��ڵ� ��������ͬIDC��

    std::map<std::string, std::pair<std::vector<TC_Endpoint>, std::vector<TC_Endpoint> > > _mapEndpoints;//���治ͬ�����صĽڵ�

    pair<std::vector<EndpointInfo>, std::vector<EndpointInfo> > _allEndpoints; //<active,inactive>,���л�ĺͷǻ�Ľڵ�
};
///////////////////////////////////////////////////////////////////////////////////
}
#endif
