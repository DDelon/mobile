#ifndef __REAP_SSD_THREAD_H__
#define __REAP_SSD_THREAD_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "servant/StatF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"
#include "StatImp.h"
#include "StatDbManager.h"

using namespace taf;

/**
 * ����ִ�ж�ʱ�������߳���
 */
class ReapSSDThread : public TC_Thread,public TC_ThreadLock
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
    ReapSSDThread();

    /**
     * ����
     */
    ~ReapSSDThread();

    /**
     * �����߳�
     */
    void terminate();

    /**
     * ��ѯ����
     */
    virtual void run();

private:
    void getData(const string &sCloneFile,vector<StatMsg> &vAllStatMsg);

	/**
	 * ͨ��Ȩ����ѯ�����㷨��ȡҪ�������ݵ�db index
	 * @param iMaxDb db����
	 * @param iGcd  ����Ȩ�ص����Լ��
	 * @param iMaxW ���Ȩ��ֵ
	 * @param vDbWeight ����db��Ȩ��ֵ
	 *
	 * @return int
	 */
	int getIndexWithWeighted(int iMaxDb,int iGcd,int iMaxW,const vector<int>& vDbWeight);

private:
	bool _bTerminate;
	int _iCurWeight;
	int _iLastSq;
};

#endif
