#ifndef __NOTIFY_COMMAND_H_
#define __NOTIFY_COMMAND_H_

#include "ServerCommand.h"

/**
 * ֪ͨ����
 * 
 */
class CommandNotify : public ServerCommand
{
public:
    CommandNotify(const ServerObjectPtr &pServerObjectPtr,const string &sMsg,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);
    
private:
    bool    _bByNode;
    string  _sMsg;
    ServerDescriptor    _tDesc;
    ServerObjectPtr     _pServerObjectPtr;     
};

//////////////////////////////////////////////////////////////
//
inline CommandNotify::CommandNotify(const ServerObjectPtr &pServerObjectPtr,const string &sMsg,bool bByNode)
:_bByNode(bByNode)
,_sMsg(sMsg)
,_pServerObjectPtr(pServerObjectPtr)
{ 
    _tDesc  = _pServerObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//
inline ServerCommand::ExeStatus CommandNotify::canExecute(string &sResult)
{ 
    //Lock lock(*this); ����Ҫ����
    
    LOG->debug()<<"notify begining "<<  _tDesc.application + "." + _tDesc.serverName<<" " <<_sMsg<<endl;
    
    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();
    
    if ( eState == ServerObject::Inactive)
    {
        sResult = "server state is Inactive. ";
        LOG->debug()<<sResult<<endl;
        return DIS_EXECUTABLE;
    }
    if ( eState == ServerObject::Destroying)
    {
        sResult = "server state is Destroying. ";
        LOG->debug()<<sResult<< endl;
        return DIS_EXECUTABLE;
    }
    return EXECUTABLE;
    
}
//////////////////////////////////////////////////////////////
//
inline int CommandNotify::execute(string &sResult)
{   
    try
    {
        TC_Endpoint ep;
        AdminFPrx pAdminPrx;    //����������

        string  sAdminPrx = "AdminObj@"+_pServerObjectPtr->getLocalEndpoint().toString();
        try
        {
            //����Ƿ�������ļ�һ��
            TC_Config conf;
            conf.parseFile(_pServerObjectPtr->getConfigFile());           
            ep.parse(conf.get("/taf/application/server<local>"));
            if(ep.getPort() != 0  && ep.toString() != _pServerObjectPtr->getLocalEndpoint().toString())
            {
                sAdminPrx = "AdminObj@"+ep.toString();
            }
        }
        catch(...)
        {
        }        
        pAdminPrx = Application::getCommunicator()->stringToProxy<AdminFPrx>(sAdminPrx);
        sResult = pAdminPrx->notify(_sMsg);
    }
    catch (exception& e)
    {
        LOG->debug()<<"CommandNotify::execute notify "<<_tDesc.application + "." + _tDesc.serverName << " error:" << e.what() << endl;
        sResult = "error" + string(e.what());
        return -1;
    }
    return 0;
}
#endif 
