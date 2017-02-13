#ifndef _queryStatServer_H_
#define _queryStatServer_H_

#include <iostream>
#include "servant/Application.h"
#include "DbThread.h"
#include "util/tc_thread_mutex.h"
#include "util/tc_lock.h"

using namespace taf;

/**
 *
 **/
class QueryStatServer : public Application , public TC_ThreadMutex
{
public:
	/**
	 *
	 **/
	virtual ~QueryStatServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();

	int getInsertInterval() const;

	size_t getStatDbNumber() const;

	void addActiveDb(const TC_DBConf& db);

	void setActiveDb(const vector<TC_DBConf> &vDbInfo);

	void clearActiveDb();

	size_t getActiveDbSize() const;

	vector<TC_DBConf> getActiveDbInfo() const;

	vector<TC_DBConf> getStatDbInfo() const;

	string dumpDbInfo(const vector<TC_DBConf>& vDbInfo) const;

	uint32_t genUid();
	int getThreadPoolSize() const;
private:

	//���ӵ����ݿ����
	size_t _dbNumber;

	// ��ѯ������ʱ��ʹ�ã�����ʱ���ᳬ�������,��λΪ����
	int _insertInterval;

	vector<TC_DBConf> _mDbInfo;

	vector<TC_DBConf> _mActiveDbInfo;

	DBThread * _pDBThread;

	//Ψһid
	uint32_t _uniqId;

	int _ThreadPoolSize;
};

extern QueryStatServer g_app;

////////////////////////////////////////////
#endif
