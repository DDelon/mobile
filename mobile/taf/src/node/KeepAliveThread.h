#ifndef __KEEP_ALIVE_THREAD_H_
#define __KEEP_ALIVE_THREAD_H_
#include "Node.h"
#include <unistd.h>
#include "ServerFactory.h"
#include "util/tc_monitor.h"
#include "util/tc_thread.h"
#include "PlatformInfo.h"

using namespace taf;
using namespace std;

class KeepAliveThread : public TC_Thread,public TC_ThreadLock
{
public:
    /**
    * ���캯��
    */

    KeepAliveThread();
    ~KeepAliveThread();
    /**
     * �����߳�
     */
    void terminate();

protected:

    virtual void run();
    
    /**
    * ��registryע��node��Ϣ
    * @return bool falseʧ��
    */
    
    bool registerNode();
    
    /**
    * ��ȡ��node��registry���÷�����Ϣ
    * @return bool  falseʧ��
    */
    bool loadAllServers();

    /**
    * �ϱ�registry node��ǰ״̬
    * @return int  ��0ʧ��
    */
    int reportAlive();

    /**
    * ���node��server״̬
    * @return void 
    */
    void checkAlive();

    /**
    * ��registry ͬ��node����������״̬
    * @return int  ��0ʧ��
    */
    int synStat();

protected:

    NodeInfo            _tNodeInfo;
    PlatformInfo        _tPlatformInfo;
    RegistryPrx         _pRegistryPrx;
    bool                _bShutDown; 
    time_t              _tRunTime; //node����ʱ��
    int                 _iHeartTimeout ;//ҵ��������ʱʱ��(s)
    int                 _iMonitorInterval ;//���server״̬�ļ��ʱ��(s)
    int                 _iSynInterval;//ͬ����regisrty server״̬�ļ��ʱ��(s)
    string              _sSynStatBatch;//����ͬ��
private:

    vector<ServerStateInfo>     _vStat;//����״̬�б�
};


#endif

