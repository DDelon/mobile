#ifndef __TAF_COMMUNICATOR_H_
#define __TAF_COMMUNICATOR_H_

#include "util/tc_thread.h"
#include "util/tc_config.h"
#include "util/tc_singleton.h"
#include "util/tc_timeout_queue.h"
#include "servant/Global.h"
#include "servant/ServantProxy.h"
#include "servant/ServantProxyFactory.h"
#include "servant/ObjectProxyFactory.h"
#include "servant/AsyncProcThread.h"
#include "servant/FDReactor.h"
#include "servant/QueueTimeout.h"

#define CONFIG_ROOT_PATH "/taf/application/client"

namespace taf
{
////////////////////////////////////////////////////////////////////////
/**
 * �ͻ�������
 */
struct ClientConfig
{
    /**
     * �ͻ���IP��ַ
     */
    static string LocalIp;
    /**
     * �ͻ���ģ������
     */
    static string ModuleName;
    /**
     * �ͻ������е�IP��ַ
     */
    static set<string>  SetLocalIp;
   /**
   *�ͻ����Ƿ��set����
   */
   static bool SetOpen;
   /**
   *�ͻ���set����
   */
   static string SetDivision;

};
////////////////////////////////////////////////////////////////////////
/**
 * ͨ����,���ڴ�����ά���ͻ���proxy
 */
class Communicator : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     */
    Communicator();

    /**
     * �������ù���
     * @param conf
     * @param path
     */
    Communicator(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * ����
     * ����ʱ�Զ���������߳�
     */
    ~Communicator();

    /**
     * ���ɴ���
     * @param T
     * @param objectName
     * @return T
     */
    template<class T> T stringToProxy(const string& objectName)
    {
        T prx = NULL;

        stringToProxy<T>(objectName, prx);

        return prx;
    }

    /**
     * ���ɴ���
     * @param T
     * @param objectName
     * @param proxy
     */
    template<class T> void stringToProxy(const string& objectName, T& proxy)
    {

        ServantPrx prx = getServantProxy(objectName);

        proxy = (typename T::element_type*)(prx.get());
    }

public:
    /**
     * ��ȡ����
     * @param name
     * @param dft, ȱʡֵ
     * @return string
     */
    string getProperty(const string& name, const string& dft = "");

    /**
     * ��������
     * @param properties
     */
    void setProperty(const map<string, string>& properties);

    /**
     * ����ĳһ������
     * @param name
     * @param value
     */
    void setProperty(const string& name, const string& value);

    /**
     * ��������
     * @param conf
     * @param path
     */
    void setProperty(TC_Config& conf, const string& domain = CONFIG_ROOT_PATH);

    /**
     * �ϱ�ͳ��
     * @return StatReport*
     */
    StatReport* getStatReport();

    /**
     * ���¼�������
     */
    int reloadProperty(string & sResult);
    
    /**
     * ��ȡobj��Ӧ����ip port�б�  ������÷���,ֻ����ͬ����ķ����ip 
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint(const string & objName);

    /**
     * ��ȡobj��Ӧ����ip port�б� ��������IDC��
     * @param sObjName
     * @return vector<TC_Endpoint>
     */
    vector<TC_Endpoint> getEndpoint4All(const string & objName);
    

    /**
     * �������͸��
     * @param sTunnel ���obj����
     * @param sRuler  ����
     * setTunnel("MDWWX.*.*|Comm.*.*","MDWWX.TunnelServer.TunnelObj")
     */

    void setTunnel(const string &sRuler,const string &sTunnel);

    /**
     * ����
     */
    void terminate();
    
    
    /**
     * ��ȡ����������
     * @return ObjectProxyFactoryPtr
     */
    ObjectProxyFactory* objectProxyFactory();

    /**
     * ��ȡ�������������
     * @return ServantProxyFactoryPtr
     */
    ServantProxyFactory* servantProxyFactory();

protected:
    /**
     * ��ʼ��
     */
    void initialize();

    /**
     * �Ƿ�������
     * @return bool
     */
    bool isTerminating();

    /**
     * ��Property��ʼ���ͻ�������
     */
    void initClientConfig();


    /**
     * fd��Ӧ��
     * @return FDReactorPtr
     */
    FDReactor* fdReactor();

    /**
     * ��ȡ�첽�����߳�
     * @return AsyncProcThreadPtr
     */
    AsyncProcThread* asyncProcThread();

    /**
     * ��ȡͨ�ö���
     * @param objectName
     * @return ServantPrx
     */
    ServantPrx getServantProxy(const string& objectName);


    /**
     * �����Ƿ������������ȡ�������
     * @param objectName
     * @return string
     */
    string getTunnelName(const string& objectName);

    /**
     * �ϱ�ģ������
     * @param req
     */
    void reportToStat(const ReqMessagePtr& req, const string& obj, int result);

    /**
     * �ϱ�ģ������ ����
     * @param req
     */
    void sampleToStat(const ReqMessagePtr& req, const string& obj, const string& ip);

    /**
     * ����ڲ���Ҫֱ�ӷ���ͨ��������
     */
    friend class ReqMessage;

    friend class BaseProxy;

    friend class FDReactor;

    friend class QueueTimeout;

    friend class AdapterProxy;

    friend class ServantProxy;

    friend class ObjectProxy;

    friend class ServantProxyFactory;

    friend class ObjectProxyFactory;

    friend class AsyncProcThread;

    friend class AsyncProcThreadRunner;

protected:
    bool _initialized;

    bool _terminating;
        
    map<string, vector<string> > _mTunnel;

    map<string, string> _properties;

    ObjectProxyFactoryPtr _objectProxyFactory;

    ServantProxyFactoryPtr _servantProxyFactory;

    StatReportPtr _statReportPtr;

    FDReactorPtr _fdReactor;

    QueueTimeoutPtr _qTimeout;

    AsyncProcThreadPtr _asyncProcThread;
};
////////////////////////////////////////////////////////////////////////
}
#endif
