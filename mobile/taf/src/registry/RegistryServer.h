#include "servant/Application.h"
#include "RegistryImp.h"
#include "QueryImp.h"
#include "AdminRegImp.h"
#include "ReapThread.h"
#include "CheckSettingState.h"
#include "RegistryProcThread.h"

using namespace taf;


/**
 *  Registry Server
 */
class RegistryServer : public Application
{
protected:
    /**
     * ��ʼ��, ֻ����̵���һ��
     */
    virtual void initialize();

    /**
     * ����, ÿ�����̶������һ��
     */
    virtual void destroyApp();

public:
    /**
     * ��ȡregistry����Ķ˿���Ϣ
     */
    map<string, string> getServantEndpoint() { return _mapServantEndpoint; }

    /**
     * ����registry����Ķ˿���Ϣ
     */
    int loadServantEndpoint();

    RegistryProcThread * getRegProcThread(){ return _registryProcThread;}

protected:
    //����ִ�ж�ʱ�������̶߳���
    ReapThread _reapThread;
	CheckSettingState _checksetingThread;

    RegistryProcThread *_registryProcThread;

    //����-������ �б�
    map<string, string> _mapServantEndpoint;

};


