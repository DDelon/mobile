#ifndef __SERVER_FACTORY_H_
#define __SERVER_FACTORY_H_
#include "Node.h"
#include <unistd.h>
#include "ServerObject.h"
#include "util/tc_common.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_singleton.h"

using namespace taf;
using namespace std;

typedef JceHashMap<ServerInfo,ServerDescriptor,ThreadLockPolicy,FileStorePolicy> HashMap;

extern HashMap g_serverInfoHashmap;

typedef map<string, ServerObjectPtr> ServerGroup;

class ServerFactory : public TC_Singleton<ServerFactory>, public TC_ThreadLock
{
public:
    /**
     *
     */
    ServerFactory();
    ~ServerFactory()
    {
    };

    /**
    * ����ָ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  ServerObjectPtr
    */
    ServerObjectPtr createServer( const ServerDescriptor& tDesc)
    {
        string result;
        return createServer(tDesc,result);
    }
    ServerObjectPtr createServer( const ServerDescriptor& tDesc,string& result);

    /**
    * ɾ��ָ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    int eraseServer( const string& application, const string& serverName );

    /**
    * ��ȡָ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  ServerObjectPtr  ���񲻴��ڷ���NULL
    */
    ServerObjectPtr getServer( const string& application, const string& serverName );

    /**
    * load���� ��application
    * serverNameΪ��load���з���ֻ�������һ��load����
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  ServerObjectPtr  ���񲻴��ڷ���NULL
    */
    ServerObjectPtr loadServer( const string& application="", const string& serverName="",bool enableCache = true)
    {
        string result;
        return loadServer(application,serverName,enableCache,result);
    }
    ServerObjectPtr loadServer( const string& application, const string& serverName,bool enableCache, string& result);
    /**
    * get���� ��application
    * serverNameΪ�ջ�ȡ���з���ֻ�������һ��load����
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  vector<ServerDescriptor>  ���񲻴��ڷ���NULL
    */
    vector<ServerDescriptor> getServerFromRegistry( const string& application, const string& serverName, string& result);
    /**
    * get���� ��application
    * serverNameΪ�ջ�ȡ���з���ֻ�������һ��load����
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  vector<ServerDescriptor>  ���񲻴��ڷ���NULL
    */
    vector<ServerDescriptor> getServerFromCache( const string& application, const string& serverName, string& result);

    /**
    * ��ȡnode�Ϸ������з���
    * @return    map<string, ServerGroup>
    */
    map<string, ServerGroup> getAllServers();
	/**
	 *@brief ���ط���core���Ե������ļ�
	 *@return bool
	 */
	bool loadConfig();
	/**
	 *@brief ����node�����з����core���ԣ����node���¹�core���ã�����Ҫ����
	 *
	 */
	void setAllServerResourceLimit();
private:
	bool IsCoreLimitEnable() const { return _bCoreLimitEnable;}
private:
    //<app,<server,serverObjectPtr>>
    map<string, ServerGroup> _mmServerList;
    PlatformInfo _tPlatformInfo;
private:

	//<app.server,ServerLimitStateInfo>
	map<string,ServerObject::ServerLimitInfo> _mCoreLimitConfig;

	//�����Ƿ��и���
	bool _bReousceLimitConfChanged;
	//�������쳣ֹͣ�����������������Զ�����coredump
    int     _iMaxExcStopCount;
	//�������쳣ֹͣ�����������������Զ�����coredump
    int32_t _iCoreLimitTimeInterval;
	//�����coredump���Ա�����֮�����Ч���ޣ���λ�Ƿ���
    int32_t  _iCoreLimitExpiredTime;
	//��Դ���Կ���
	bool     _bCoreLimitEnable;
};


#endif


