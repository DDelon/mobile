#ifndef _NOTIFY_SERVER_H_
#define _NOTIFY_SERVER_H_

#include "servant/Application.h"

using namespace taf;

class NotifyServer : public Application
{
protected:
    /**
     * ��ʼ��, ֻ����̵���һ��
     */
    virtual void initialize();

    /**
     * ����, ÿ�����̶������һ��
     */
    virtual void destroyApp();
};

#endif

