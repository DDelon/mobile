#ifndef _TransServer_H_
#define _TransServer_H_

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "servant/Application.h"

using namespace taf;

class TransServer : public Application
{
public:
	/**
	 * ��������
	 **/
	virtual ~TransServer() {};

	/**
	 * �����ʼ��
	 **/
	virtual void initialize();

	/**
	 * ��������
	 **/
	virtual void destroyApp();
};
////////////////////////////////////////////
#endif
