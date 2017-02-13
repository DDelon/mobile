#ifndef __REAP_THREAD_H__
#define __REAP_THREAD_H__

#include <iostream>

#include "util/tc_thread.h"

#include "DbHandle.h"

using namespace taf;

/**
 * ����ִ�ж�ʱ�������߳���
 */
class ReapThread : public TC_Thread, public TC_ThreadLock
{
public:

    ReapThread();

    ~ReapThread();

    /*
     * �����߳�
     */
    void terminate();

    /**
     * ��ʼ��
     */
    int init();

    /**
     * ��ѯ����
     */
    virtual void run();

protected:
    //�߳̽�����־
    bool _bTerminate;

    //���ݿ����
    CDbHandle _db;

    //���ض����б��ʱ����,��λ����
    //��һ�׶μ���ʱ��
    int _iLoadObjectsInterval1;
	int _iLeastChangedTime1;

	//ȫ������ʱ��,��λ����
	int _iLoadObjectsInterval2;
	int _iLeastChangedTime2;

    //node������ʱʱ��
    int _iNodeTimeout;

    //registry������ʱʱ��
    int _iRegistryTimeout;

	//�Ƿ�����DB�ָ��������ܣ�Ĭ��Ϊ��
	bool _bRecoverProtect;

	//����DB�ָ���������״̬�¼���ֵ
	int _iRecoverProtectRate;

	//��������ʱ����¿���
	bool _bHeartBeatOff;
};

#endif
