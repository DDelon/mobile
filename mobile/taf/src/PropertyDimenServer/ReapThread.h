#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__
#include <iostream>
#include "util/tc_thread.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "servant/PropertyF.h"
#include "util/tc_common.h"
#include "log/taf_logger.h"
#include "PropertyDimenImp.h"


using namespace taf;

extern TC_Config* g_pconf;



/**
 * ����ִ�ж�ʱ�������߳���
 */
class PropertyDimenReapThread : public TC_Thread,public TC_ThreadLock
{
public:
     /**
     * ���峣��
     */
    enum
    {
        REAP_INTERVAL = 10000, /**��ѵ�����ʱ��**/
    };

    enum CutType
    {
        CUT_BY_DAY      = 0,
        CUT_BY_HOUR     = 1,
        CUT_BY_MINUTE   = 2,
    };
    
    /**
     * ����
     */
    PropertyDimenReapThread();

    /**
     * ����
     */
    ~PropertyDimenReapThread();

    /**
     * �����߳�
     */
    void terminate();

    /**
     * ��ѯ����
     */
    virtual void run();

private:
    
    void getData(const string &sCloneFile, vector<map<PropDimenHead,PropBody> > &mAllStatMsg);

protected:

    
    bool _bTerminate;
private:
    

};

#endif
