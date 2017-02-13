#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "servant/StatF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"


using namespace taf;

/**
 * ����ִ�ж�ʱ�������߳���
 */
class DBThread : public TC_Thread,public TC_ThreadLock
{
public:
     /**
     * ���峣��
     */
    enum
    {
        REAP_INTERVAL = 30000
    };
    /**
     * ����
     */
    DBThread();

    /**
     * ����
     */
    ~DBThread();

    /**
     * �����߳�
     */
    void terminate();

    /**
     * ��ѯ����
     */
    virtual void run();

	void tryConnect();

	int connect(MYSQL **_pstMql,const TC_DBConf& tcConf);

private:
    bool _bTerminate;

};

#endif
