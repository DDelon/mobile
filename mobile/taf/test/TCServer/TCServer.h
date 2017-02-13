#ifndef _HelloServer_H_
#define _HelloServer_H_

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "util/tc_parsepara.h"
#include "servant/Application.h"

using namespace taf;

/**
 *
 **/
class TCServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~TCServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

protected:
    template<typename T>
    void bind(const string &str)
    {
        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(getEpollServer().get());

        //����adapter����, Ψһ
        lsPtr->setName(str);
        //���ð󶨶˿�
        lsPtr->setEndpoint(str);
        //�������������
        lsPtr->setMaxConns(20000);
        //���������߳���
        lsPtr->setHandleNum(10);
        //����Э�������
        lsPtr->setProtocol(&T::parse);

        //�󶨶���
        getEpollServer()->bind(lsPtr);

        //�����߼�������
        lsPtr->setHandle<T>();
    }
};

extern TCServer g_app;

////////////////////////////////////////////
#endif
