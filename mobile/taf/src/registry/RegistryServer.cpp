#include "RegistryServer.h"
#include "routerProxy.h"

//taserver prx
routerProxy::RouterProxyServicePrx _routerPrx;

void RegistryServer::initialize()
{
    LOG->debug()<< "RegistryServer::initialize..." << endl;

    try
    {
        extern TC_Config * g_pconf;

        //����registry����Ķ˿���Ϣ
        loadServantEndpoint();

        //��ѯ�߳�
        _reapThread.init();
        _reapThread.start();

		_checksetingThread.init();
		_checksetingThread.start();

		//�����첽�����߳�
        _registryProcThread = new RegistryProcThread();
        _registryProcThread->start(2);

        //��node���ʵĶ���
        addServant<RegistryImp>((*g_pconf)["/taf/objname<RegistryObjName>"]);

        //������client���ʵĶ���
        addServant<QueryImp>((*g_pconf)["/taf/objname<QueryObjName>"]);

        //��admin client���ʵĶ���
        addServant<AdminRegImp>((*g_pconf)["/taf/objname<AdminRegObjName>"]);

		//taserver
		string obj = g_pconf->get("/taf/objname<taserverObjName>");
		_routerPrx = Application::getCommunicator()->stringToProxy<routerProxy::RouterProxyServicePrx>(obj);
    }
    catch(TC_Exception & ex)
    {
        LOG->error() << "RegistryServer initialize exception:" << ex.what() << endl;
        cerr << "RegistryServer initialize exception:" << ex.what() << endl;
        exit(-1);
    }

    LOG->debug()<< "RegistryServer::initialize OK!" << endl;
}


void RegistryServer::destroyApp()
{
    LOG->debug()<< "RegistryServer::destroyApp ok" << endl;
}


int RegistryServer::loadServantEndpoint()
{
    map<string, string> mapAdapterServant;
    mapAdapterServant = ServantHelperManager::getInstance()->getAdapterServant();

    map<string, string>::iterator iter;
    for(iter = mapAdapterServant.begin(); iter != mapAdapterServant.end(); iter++ )
    {
        TC_Endpoint ep = getEpollServer()->getBindAdapter(iter->first)->getEndpoint();

        _mapServantEndpoint[iter->second] = ep.toString();

        LOG->debug()<<"registry obj: " << iter->second << " = " << ep.toString() <<endl;
    }

    return 0;
}


