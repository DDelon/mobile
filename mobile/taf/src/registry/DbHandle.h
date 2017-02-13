#ifndef __DB_HANDLE_H__
#define __DB_HANDLE_H__

#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_monitor.h"
#include "util/tc_mysql.h"
#include "util/tc_file.h"
#include "jmem/jmem_hashmap.h"

#include "Registry.h"
#include "Node.h"
#include "log/taf_logger.h"

using namespace taf;

typedef map<string, ObjectItem> ObjectsCache;
typedef JceHashMap<ObjectName, ObjectItem, ThreadLockPolicy, FileStorePolicy> FileHashMap;

/**
 *  ���ݿ������
 */
class CDbHandle
{
private:
	struct GroupPriorityEntry
	{
		std::string		sGroupID;
		std::string		sStation;
		std::set<int> 	setGroupID;
	};

	enum GroupUseSelect
	{
		ENUM_USE_WORK_GROUPID,
		ENUM_USE_REAL_GROUPID
	};

	//set�з������Ϣ
	struct SetServerInfo
	{
		bool	   bActive;
		string 	   sServant;
		EndpointF  epf;
	};

	typedef map<string,map<string, map<string,vector<CDbHandle::SetServerInfo> > > > SetDivisionCache;

public:
    /**
     * ���캯��
     */
    CDbHandle(){_bEnMultiSql = false;}

    /**
     * ��ʼ��
     * @param pconf �����ļ�
     * @return 0-�ɹ� others-ʧ��
     */
    int init(TC_Config *pconf);

    /**
     * ��ȡ�ض�node id�Ķ������
     * @param nodeName : node id
     * @return :  ������������ָ��
     */
    NodePrx getNodePrx(const string & nodeName);

    /**
     * ����nodeע���session
     * @param name node����
     * @param ni   node��ϸ��Ϣ
     * @param li    node����������Ϣ
     * @return ע���Ƿ�ɹ�
     */
    int registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li);

    /**
     * ����node����ʱ�估��������
     *
     * @param name    node����
     * @param load    node����������Ϣ
     * @return 0-�ɹ� others-ʧ��
     */
    int keepAlive(const string & name, const LoadInfo & li);

    /**
     * ��ȡ�node�б�endpoint��Ϣ
     * @param out result �������
     * @return map : ��Ӧid node��obj
     */
    map<string, string> getActiveNodeList(string & result);

    /**
     * ��ȡnode�汾
    * @param name   node����
     * @param version   node�汾
     * @param out result �������
     * @return  0-�ɹ� others-ʧ��
     */
    int getNodeVersion(const string &nodeName, string &version, string & result);

    /**
     * ��ȡ�ڸ�node�����server�б�
     *
     * @param app:       Ӧ��
     * @param serverName: server ��
     * @param nodeName : node id
     *
     * @return server��Ϣ�б�
     */
    vector<ServerDescriptor> getServers(const string & app, const string & serverName, const string & nodeName,bool withDnsServer = false);

    /**
     * ��ȡserver������ģ��
     * @param sTemplateName ģ������
     * @param sResultDesc �������
     * @return ģ������
     */
    string getProfileTemplate(const string & sTemplateName, string & sResultDesc);


protected:
    /**
     * ��ȡserver������ģ��
     * @param sTemplateName ģ������
     * @param mapRecursion ���ݹ鵽��ģ��
     * @param sResultDesc �������
     * @return ģ������
     */
    string getProfileTemplate(const string & sTemplateName, map<string, int> & mapRecursion, string & sResultDesc);

public:
    /**
     * ����server״̬
     *
     * @param app:       Ӧ��
     * @param serverName: server ��
     * @param nodeName : node id
     * @param stateFields: ����״̬�ֶ���
     * @param state :  server״̬
     *
     * @return server��Ϣ�б�
     */
    int updateServerState(const string & app, const string & serverName, const string & nodeName,
            const string & stateFields, taf::ServerState state, int processId = -1);

    /**
     * ��������server״̬
     *
     * @param vecStateInfo:       ����server��״̬
     *
     * @return server��Ϣ�б�
     */
	int updateServerStateBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo);


    /**
     * ����node��״̬Ϊinactive
     *
     * @param name    node����
     *
     * @return 0-�ɹ� others-ʧ��
     */
    int destroyNode(const string & name);

    /** ����id��ȡ����
     *
     * @param id ��������
     *
     * @return  �������иö���Ļendpoint�б�
     */
    vector<EndpointF> findObjectById(const string & id);

    /** ����id��ȡ����
     *
     * @param id ��������
     * @out param activeEp    �����б�
     * @out param inactiveEp  �Ǵ����б�
     *
     * @return 0-�ɹ� others-ʧ��
     */
    int findObjectById4All(const string & id,
                           vector<EndpointF>& activeEp,
                           vector<EndpointF>& inactiveEp);

     /** ����id��ȡͬ�����
     *
     * @param id ��������
     * @param ip
     * @out param activeEp    �����б�
     * @out param inactiveEp  �Ǵ����б�
     * @out param os          ��ӡ��־ʹ��
     *
     * @return 0-�ɹ� others-ʧ��
     */
    int findObjectByIdInSameGroup(const string & id,
                                 const string & ip,
                                 vector<EndpointF>& activeEp,
                                 vector<EndpointF>& inactiveEp,
                                 ostringstream &os);

     /** ����id��ȡ���ȼ������еĶ���
     *
     * @param id ��������
     * @param ip
     * @out param activeEp    �����б�
     * @out param inactiveEp  �Ǵ����б�
     * @out param os          ��ӡ��־ʹ��
     *
     * @return 0-�ɹ� others-ʧ��
     */
	int findObjectByIdInGroupPriority(const std::string &sID, const std::string &sIP, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);

     /** ����id�͹����ػ�ȡȫ������
     *
	 * @param id ��������
	 * @param sStation ������
     * @out param activeEp    �����б�
     * @out param inactiveEp  �Ǵ����б�
     * @out param os          ��ӡ��־ʹ��
     *
     * @return 0-�ɹ� others-ʧ��
     */
	int findObjectByIdInSameStation(const std::string &sID, const std::string & sStation, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);

	int findObjectByIdInSameSet(const string &sID, const vector<string> &vtSetInfo, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);

	int findObjectByIdInSameSet(const string &sSetId, const map<string, vector<SetServerInfo> > &mpSetServerInfo, std::vector<EndpointF> & vecActive, std::vector<EndpointF> & vecInactive, std::ostringstream & os);
	/**
     * ��ȡapplication�б�
     * @param null
     * @param out reuslt
     * @return application�б�
     */
    vector<string> getAllApplicationNames(string & result);


    /**
     * ��ȡserver�б�
     * @param null
     * @return node �б�
     */
    vector<vector<string> > getAllServerIds(string & result);

    /**
     * ��ȡ�ϲ��������ļ�
     * @param appServerName
     * @param fileName
     * @param host
     * @param config
     * @param resultDesc
     *
     * @return int
     */
    int getConfig(const string &appServerName, const string &fileName, const string &host, string &config, std::string &resultDesc);

    /**
     * ����server�����汾
     * @param app:       Ӧ��
     * @param serverName: server ��
     * @param nodeName : node id
     * @param version  : server �汾
     * @param user     : ������
     */
    int setPatchInfo(const string & app, const string & serverName, const string & nodeName,
            const string & version, const string & user);

    /**
     * ����server��taf��汾
     * @param app:       Ӧ��
     * @param serverName: server ��
     * @param nodeName : node id
     * @param version:  server���ڵ�taf�汾��
     *
     * @return 0-�ɹ� others-ʧ��
     */
    int setServerTafVersion(const string & app, const string & serverName, const string & nodeName,
            const string & version);

    /**
     * ��ѯ���ݿ⣬��������ʱ�Ľڵ㼰server״̬��Ϊ�����
     * @param iTiemout ��ʱʱ��
     * @return
     */
    int checkNodeTimeout(unsigned uTimeout);

    /**
     * ��ѯ���ݿ⣬��������ʱ��registry��Ϊ�����
     * @param iTiemout ��ʱʱ��
     * @return
     */
    int checkRegistryTimeout(unsigned uTimeout);

    /**
     * ��ʱ��������ݿ������״̬Ϊ��active���ķ�����Node�ڵ��ϵ�״̬��
     * ���������Node������״̬���ǡ�active������֪ͨNode���������÷���
     *
     * @param iCheckTimeInterval ÿ����ѯ������µļ�¼����λΪ��
     * @return
     */
    int checkSettingState(const int iCheckLeastChangedTime=3600);

    /**
     * �ڼ��ض����б�֮ǰ�����㵱ǰ�ǻ״̬��node�ı���
     * @param NULL
     * @return
     */
	int computeInactiveRate();

    /**
     * ���ض����б��ڴ�
     * @param bRecoverProtect
     * @param iRecoverProtectRate
     * @param iLoadTimeInterval �������iLoadTimeInterval���ڱ仯�ļ�¼
     * @param bLoadAll  �Ƿ�������з���
     *
     * @return
     */
    int loadObjectIdCache(const bool bRecoverProtect,
                            const int iRecoverProtectRate,
                            const int iLoadTimeInterval=60,
                            const bool bLoadAll=false);

    /**
     * ���������ȼ����ڴ�
     * @param NULL
     * @return
     */
	int loadGroupPriority();

    /**
     * ����registry��Ϣ��db
     */
    int updateRegistryInfo2Db(bool bRegHeartbeatOff=false);

    /**
     * ��ȡ��Ӧnode��ģ������
     */
    int getNodeTemplateName(const string nodeName, string & sTemplateName);

    int initFileCache();

    /**
     * ����ip��ȡ��id
     * @return int <0 ʧ�� ��������
     */
    int getGroupId(const string& ip);

    /**
    *����������ȡ��id
    * @return int <0 ʧ�� ��������
    */
    int getGroupIdByName(const string& sGroupName);

    /**
     * ����IP���������Ϣ
    */
    int loadIPPhysicalGroupInfo();
protected:

    /**
     * ����group id��ȡEndpoint
     */
    vector<EndpointF> getEpsByGroupId(const vector<EndpointF> & vecEps, const GroupUseSelect GroupSelect, int iGroupId, ostringstream &os);

	vector<EndpointF> getEpsByGroupId(const vector<EndpointF> & vecEps, const GroupUseSelect GroupSelect, const set<int> & setGroupID, ostringstream & os);


    /**
     * ���·�����Ϣ��node��
     */
    int updateGroupId2DB();

    /**
     * updateServerStateBatch�ĵײ�ʵ�ֺ���
     */
	int doUpdateServerStateBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo, const size_t sizeBegin, const size_t sizeEnd);

protected:
    int dumpCache2HashMap(ObjectsCache & cache,const bool bLoadAll=false);
    int loadCacheFromHashMap(ObjectsCache & cache);
private:
	/**
	 * ���»����еķ���״ֵ̬
	 *
	 * @param mStatus
	 * @param updateAll �Ƿ�ȫ������
	 */
	 void updateStatusCache(const std::map<std::string, int>& mStatus,bool updateAll=false);
	/**
	 * ���»����еķ�����Ϣ
	 *
	 * @param objCache
	 * @param updateAll �Ƿ�ȫ������
	 */
	 void updateObjectsCache(const ObjectsCache& objCache,bool updateAll=false);

	/**
	 * ���»����е�set��Ϣ
	 *
	 * @param objCache
	 * @param updateAll �Ƿ�ȫ������
	 */
	 void updateDivisionCache(const SetDivisionCache& setDivisionCache,bool updateAll=false);
	/**
	 * �����ݿ��ѯ���ִ�����ϲ���
	 *
	 * @param data1
	 * @param data2
	 *
	 * @return ���ϵĽ��
	 */
	 TC_Mysql::MysqlData UnionRecord(TC_Mysql::MysqlData& data1,TC_Mysql::MysqlData& data2);

protected:
    //mysql���Ӷ���
    taf::TC_Mysql _mysqlReg;

    //node�ڵ�����б�
    static map<string , NodePrx> _mapNodePrxCache;
    static TC_ThreadLock _NodePrxLock;

    //�����б���
    static ObjectsCache _objectsCache[2];
    //����ʹ�õĶ��󻺴��±�
    static int _objectsFlag;

    //set���ֻ���
    static SetDivisionCache _setDivisionCache[2];
    //����ʹ�õ�set�����±�
    static int _setDivisionFlag;

	//���ȼ�������
	static std::map<int, GroupPriorityEntry> _mapGroupPriority[2];
	static int _mapGroupPriorityFlag;

	//servant״̬��
	static std::map<std::string, int> _mapServantStatus[2];
	static int _mapServantStatusFlag;

    //���ڱ��ݷ����б��hash
    static FileHashMap _fileCache;

    //���ڳ�ʼ������
    static TC_ThreadLock _mutex;

    //������Ϣ
    static map<string,int> _mServerGroupCache;
    static map<string,int> _mGroupNameIDCache;

    //ƥ�������Ϣ
    static vector< map<string,string> > _vServerGroupRule;

	//�Ƿ�������ö������ͬʱִ�з�ʽ
	bool _bEnMultiSql;
};

#endif
