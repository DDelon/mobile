#ifndef _CONFIG_SERVER_H_
#define _CONFIG_SERVER_H_

#include "servant/Application.h"

using namespace taf;

class ConfigServer : public Application
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

