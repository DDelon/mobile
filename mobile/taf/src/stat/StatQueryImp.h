#ifndef __StatQuery_IMP_H_
#define __StatQuery_IMP_H_

#include "StatQuery.h"
#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "jmem/jmem_hashmap.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "StatQueryHashMap.h"
#include "StatDbManager.h"


using namespace taf;


class StatQueryImp : public StatQuery
{
public:
    /**
     *
     */
    StatQueryImp()
    {
    };
    ~StatQueryImp()
    {
    };

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize()
    {
        cout << "ok" << endl;
    };

    /**
     * �˳�
     */
    virtual void destroy()
    {
    };

    /**
    * udp�ϱ�ģ��������Ϣ
    * @param StatQuery, �ϱ���Ϣ
    * @return int, ����0��ʾ�ɹ�
    */
    virtual int query(const StatQueryKey& tKey,bool bMaster,StatQueryValue &tValue, taf::JceCurrentPtr current );

    /**
    * ��ȡģ�����ݴ洢db��Ϣ
    * @param string, ��ѯģ����
    * @param DbInf, db��Ϣ
    * @return int, ����0��ʾ�ɹ�
    */
    virtual int queryDbInf(const string &sModule, Db &tDb, taf::JceCurrentPtr current );
protected:
};

#endif


