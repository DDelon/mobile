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
#include "servant/PropertyF.h"
#include "PropertyDB.h"
#include "PropertyHashMap.h"
#include <regex.h>

using namespace taf;
//typedef  map<StatMicMsgHead, StatMicMsgBody> MapStatMicMsg;

struct DbInfo
{
    DbInfo():pMysql(NULL)
    {
        mMasterTableMsg.clear();
        //mSlaveTableMsg.clear();
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
    map<PropHead,PropBody> mMasterTableMsg;
    //StatMsg mSlaveTableMsg;
};

class PropertyDbManager : public TC_Singleton<PropertyDbManager>,public TC_ThreadMutex 
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
    PropertyDbManager();
    ~PropertyDbManager(){}

public:

    int loadDbRouter(string & sResult);

    int flush(const string &sDate, const string &sFlag);

    bool filter(const PropHead &head, const PropBody &body);

    int getDbRouter(const string & sName,Db& tDb);

public:
	int creatTable(const string &sTbName, TC_Mysql *_pstMql);
	int sendAlarmSMS();

    int creatTb(const string &strTbName,TC_Mysql *pMysql = NULL);

    int insert2Db(const map<PropHead,PropBody>& statmsg,const string &sDate,const string &sFlag,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);

    int updateEcsStatus(const string &sLastTime,const string &sTbNamePre = "",TC_Mysql *pMysql = NULL);
	int creatEscTb(const string &sTbName, string sSql , TC_Mysql *pMysql);

    void updateRelation(const string &sDate,TC_Mysql *pMysql = NULL);
	
    int insert2MultiDbs(vector<map<PropHead,PropBody > > &statmsg,const string &sDate, const string &sFlag);
	int	getDbNumber();
private:

    regex_t initReg(const string& s);

    void releaseReg();

    inline bool matchReg(const regex_t& regex,const string& value);
	

private:
    map<string, DbInfo*> _mDbMysql;         //ģ�����ݿ�����
    vector<string>      _vDbConfig;         //��˳�����õ�DB����
    
    TC_Mysql *_pDefaultMysql;               //Ĭ��mysql���Ӷ���
private:

    string              _sSql;              //������
    string              _sSqlStatus;              //������t_ecstatus
    string              _sTbNamePre;        //��ǰ׺
    string              _sConcerns;         // �澯��ע��
    int                 _iMaxInsertCount;   //һ������������
    CutType             _eCutType;          //�ֱ�����    
    int                 _iInterval;         //ִ��ReapSqlʱ����
    map<string,string>  _mReapSql;          //��ʱִ��sql  
    

    vector<TC_Mysql *> _vpMysql; 
	vector<string> _vsTbNamePre;
	int dbNumber;
};

#endif


