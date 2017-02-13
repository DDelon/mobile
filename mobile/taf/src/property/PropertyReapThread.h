#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "servant/PropertyF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"
#include "PropertyImp.h"


using namespace taf;

extern TC_Config* g_pconf;



/**
 * ����ִ�ж�ʱ�������߳���
 */
class PropertyReapThread : public TC_Thread,public TC_ThreadLock
{
public:
     /**
     * ���峣��
     */
    enum
    {
        REAP_INTERVAL = 10000, /**��ѵ�����ʱ��**/
    };

    /**
     * ����
     */
    PropertyReapThread();

    /**
     * ����
     */
    ~PropertyReapThread();

    /**
     * �����߳�
     */
    void terminate();

    /**
     * ��ѯ����
     */
    virtual void run();

private:


    void getData(const string &sCloneFile,map<PropHead,PropBody> &mStatMsg, vector<map<PropHead,PropBody> > &mAllStatMsg);

private:
	bool _bTerminate;
    //mysql���Ӷ���
    TC_Mysql _mysql;
};

#endif
