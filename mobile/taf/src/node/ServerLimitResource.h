#ifndef __SERVERLIMITRESOURCE_H_
#define __SERVERLIMITRESOURCE_H_

#include <unistd.h>
#include <iostream>
#include "util/tc_monitor.h"
#include "util/tc_autoptr.h"

using namespace taf;
using namespace std;

class ServerLimitResource : public TC_ThreadLock, public TC_HandleBase
{
public:

	ServerLimitResource(int iMaxCount=3,int iTimeInterval=5/*����*/,int iExpiredTime=30/*����*/,const string& app="",const string& servername="");

	~ServerLimitResource(){}

public:
	void setLimitCheckInfo(int iMaxCount,int iTimeInterval,int iExpiredTime);
	/**
	 * @brief �����쳣������¼
	 *
	 *1 �����_iCoreLimitTimeIntervalʱ����ڣ��������쳣����
	 *2 �ж��쳣�����������ʱ����ڴﵽcore����_iMaxExcStopCount��������core����_bCloseCore
	 *  ����¼��ǰʱ���_tEnableCoreLimitTime
	 */
	void addExcStopRecord();
	/**
	 *@brief ����Ƿ���Ҫ�رջ��ߴ�core
	 *
	 *1 �������core���رգ����ȼ�鱻�رյ�ʱ�����������ڣ��ұ�����û������manual�رգ����core����
	 *2 ���core���أ����true��ر�core�����첽֪ͨ����رգ�����򿪿���
	 *
	 */
	int IsCoreLimitNeedClose(bool& bClose);

private:
	bool    _bCloseCore;  //����core����
	int 	_iCurExcStopCount;  //��ǰ�����쳣ֹͣ����
	int 	_iMaxExcStopCount;  //�������쳣ֹͣ����
	time_t  _iExcStopRecordTime;//��һ���쳣ֹͣʱ��
	int32_t _iCoreLimitTimeInterval; //ʱ������core�����ƣ���λ�Ƿ���
	time_t  _tEnableCoreLimitTime; //��һ�ιر�core��ʱ��
	int32_t  _iCoreLimitExpiredTime;//core�رյĳ���ʱ��,��λΪ����
	string _sAppname;
	string _sServername;
};

typedef TC_AutoPtr<ServerLimitResource> ServerLimitResourcePtr;

#endif


