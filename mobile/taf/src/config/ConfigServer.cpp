#include "ConfigServer.h"
#include "ConfigImp.h"

void ConfigServer::initialize()
{
    //���Ӷ���
    addServant<ConfigImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".ConfigObj");
}

void ConfigServer::destroyApp()
{
}

