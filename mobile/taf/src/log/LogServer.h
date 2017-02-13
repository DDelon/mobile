#ifndef _LOG_SERVER_H
#define _LOG_SERVER_H

#include "servant/Application.h"

using namespace taf;

class LogServer : public Application
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
private:
	bool loadLogFormat(const string& command, const string& params, string& result);
};

#endif

