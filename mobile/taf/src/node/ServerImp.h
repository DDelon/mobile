#ifndef __SERVER_IMP_H_
#define __SERVER_IMP_H_
#include "Node.h"
#include <unistd.h>
#include "ServerFactory.h"
#include "util/tc_common.h"



using namespace taf;
using namespace std;

class ServerImp : public ServerF
{
public:
    /**
     *
     */
    ServerImp(){};


    /**
     * ���ٷ���
     * @param k
     * @param v
     *
     * @return int
     */
    virtual ~ServerImp()
    {
    };

    /**
    * ��ʼ��
    */
    virtual void initialize()
    {
    };

    /**
    * �˳�
    */
    virtual void destroy()
    {
    };
    /**
    * �˳�
    */
    virtual int keepAlive( const taf::ServerInfo& serverInfo, taf::JceCurrentPtr current ) ;
    
    virtual int reportVersion( const string &app,const string &serverName,const string &version,taf::JceCurrentPtr current) ;

public:
    int getServers();
private:
};

typedef TC_AutoPtr<ServerImp> ServerImpPtr;

#endif

