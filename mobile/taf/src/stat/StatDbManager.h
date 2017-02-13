#ifndef __DB_MANAGER_H_
#define __DB_MANAGER_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "jmem/jmem_hashmap.h"
#include "servant/StatF.h"
#include "StatQuery.h"
#include "StatHashMap.h"
#include <regex.h>

using namespace taf;
//typedef  map<StatMicMsgHead, StatMicMsgBody> MapStatMicMsg;

struct DbInfo
{
    DbInfo():pMysql(NULL)
    {
        mMasterTableMsg.clear();
        mSlaveTableMsg.clear();
		memset(&tRegex,0,sizeof(regex_t));
	};

    ~DbInfo()
    {
        if(pMysql)
        {
            delete pMysql;
            pMysql = NULL;
        }
    };
    regex_t tRegex;
    Db tDb;
    TC_Mysql *pMysql;
    StatMsg mMasterTableMsg;
    StatMsg mSlaveTableMsg;
};
struct StatFilterCond
{
    StatFilterCond()
    {

    };

    ~StatFilterCond()
    {

    };

    std::string masterName; //�����ֶΣ���ʱδ��
	std::string slaveName; //�����ֶΣ���ʱδ��
	std::string interFaceName; //�����ֶΣ���ʱδ��
	std::string threshold;
};

class StatDbManager : public TC_Singleton<StatDbManager>,public TC_ThreadMutex
{
public:
    enum CutType
    {
        CUT_BY_DAY      = 0,
        CUT_BY_HOUR     = 1,
        CUT_BY_MINUTE   = 2,
    };

public:
    /**
     *
     */
    StatDbManager();
    ~StatDbManager();

public:

    int loadDbRouter(string & sResult);

    int flush(const string &sDate, const string &sFlag);

    bool filter(const StatMicMsgHead &head, const StatMicMsgBody &body);

    int getDbRouter(const string & sName,Db& tDb);

public:

    int creatTb(const string &strTbName,TC_Mysql *pMysql = NULL);

    int creatTable(const string &sTbName, TC_Mysql *_pstMql);

	int creatEscTb(const string &sTbName, string sSql , TC_Mysql *pMysql);

    int insert2Db(const StatMsg &statmsg,const string &sDate,const string &sFlag,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

    int updateEcsStatus(const string &sLastTime,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

	int checkLastTime(const string &sLastTime,const string &sTbNamePre,TC_Mysql *pMysql);

    int insert2MultiDbs(vector<StatMsg> &vStatmsg,const string &sDate, const string &sFlag);

	int sendAlarmSMS();

	int	getDbNumber();

	int genRandOrder();

	 /**
	  * ��ȡ����db��Ȩ��ֵ������������Ȩ�ص����Լ��ֵ�����Ȩ��
	  * @param iGcd
	  * @param iMaxW
	  * @param vDbWeighted
	  */
	void getDbWeighted(int& iGcd,int& iMaxW,vector<int>& vDbWeighted);
	bool IsEnableWeighted();
private:

    regex_t initReg(const string& s);

    void releaseReg();

    inline bool matchReg(const regex_t& regex,const string& value);

	/**
	 *Greatest Common Divisor
	 */
	int getGcd (int a, int b);

private:
    map<string, DbInfo*> _mDbMysql;         //ģ�����ݿ�����
    vector<string>      _vDbConfig;         //��˳�����õ�DB����
    vector<StatFilterCond>  _vFilterCond;
private:


	string				_sSql;				//������
    string              _sSqlStatus;        //������t_ecstatus
    string              _sTbNamePre;        //��ǰ׺
    int                 _iMaxInsertCount;   //һ������������
    CutType             _eCutType;          //�ֱ�����
    int                 _iInterval;         //ִ��ReapSqlʱ����
    map<string,string>  _mReapSql;          //��ʱִ��sql


	// ��������SSD�ģ����ڲ�ͬ�߳���ʹ��
    vector<TC_Mysql *> _vpSSDThreadMysql; // reapSSDthread��ʹ�á�
	vector<string> _vsTbNamePre;
	int _dbNumber;
	vector<int> _vRandOrder;

	//����db��Ȩ��ֵ
	vector<int> _vDbWeighted;
	bool _EnableWeighted;
};

#endif


