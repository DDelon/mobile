#ifndef __CHECKSETINGSTATE_H__
#define __CHECKSETINGSTATE_H__

#include <iostream>

#include "util/tc_thread.h"
#include "Node.h"
#include "DbHandle.h"

using namespace taf;

/**
 * ����ִ�ж�ʱ�������߳���
 */
class CheckSettingState : public TC_Thread, public TC_ThreadLock
{
public:

    CheckSettingState();

    ~CheckSettingState();

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
private:
    //�߳̽�����־
    bool _bTerminate;

    //���ݿ����
    CDbHandle _db;

    //��ѯserver״̬�ļ��ʱ��
    int _iCheckingInterval;

    //������ѯ����״̬��ʱ��,��λ����
    int _iLeastChangedTime;
};

#endif
