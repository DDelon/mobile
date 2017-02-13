#include "servant/ObjectProxyFactory.h"
#include "log/taf_logger.h"

namespace taf
{
ObjectProxyFactory::ObjectProxyFactory(Communicator* cm)
: _comm(cm) 
{
}

ObjectProxyFactory::~ObjectProxyFactory()
{
}

ObjectPrx::element_type* ObjectProxyFactory::getObjectProxy(const string& name, bool bCanTunneled)
{
    string sObjectName  = name;
    string sObjectKey   = name;

    //�����͸��, ��������ķ���,ͨ�����endpointת��
    bool bTunnelEnabled = false;
    
    if( bCanTunneled == true)
    {
        string sTunnel =  _comm->getTunnelName(name);
        if(!sTunnel.empty())
        {
            sObjectName     = sTunnel;
            sObjectKey      = name+"/"+sTunnel;  //��ͬobj��ͬtunnel
            bTunnelEnabled  = true;
        }
    }

    {
        TC_LockT<TC_ThreadRecMutex> lock(*this);

        map<string, ObjectPrx>::iterator it = _objectProxys.find(sObjectKey);
        
        if(it != _objectProxys.end())
        {
            return it->second.get();
        }
    }

    int timeout = TC_Common::strto<int>(_comm->getProperty("async-invoke-timeout", "5000"));

    int refreshInterval = TC_Common::strto<int>(_comm->getProperty("refresh-endpoint-interval", "60000"));

    int cacheInterval = TC_Common::strto<int>(_comm->getProperty("cache-endpoint-interval", "1800000"));

    ObjectPrx op = NULL;

    op = new ObjectProxy(_comm, sObjectName);

    if(bTunnelEnabled == true)
    {
        op->setTunnelEnabled(true);
    }

    op->timeout(timeout);

    op->refreshEndpointInterval(refreshInterval);
    
    op->cacheEndpointInterval(cacheInterval); //����endpoint �����ز�����ʱ����ʹ��

    op->initialize();

    TC_LockT<TC_ThreadRecMutex> lock(*this);

    _objectProxys.insert(make_pair(sObjectKey,op));

    return _objectProxys[sObjectKey].get();
}

map<string, ObjectPrx> ObjectProxyFactory::getObjectProxys()
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    return _objectProxys;
}

int ObjectProxyFactory::loadObjectLocator()
{
    TC_LockT<TC_ThreadRecMutex> lock(*this);

    map<string, ObjectPrx>::iterator it;

    for (it = _objectProxys.begin(); it != _objectProxys.end(); ++it)
    {
        it->second->loadLocator();
    }
    return 0;
}
///////////////////////////////////////////////////////////////////
}
