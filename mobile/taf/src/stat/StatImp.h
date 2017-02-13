#ifndef __STAT_IMP_H_
#define __STAT_IMP_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_mysql.h"
#include "util/tc_config.h"
#include "log/taf_logger.h"
#include "jmem/jmem_hashmap.h"
#include "servant/StatF.h"
#include "StatQueryImp.h"
#include "StatHashMap.h"


using namespace taf;

class StatImp : public StatF,public TC_ThreadLock
{
public:
    /**
     *
     */
    StatImp()
    {
    };
    ~StatImp()
    {
    };

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize();

    /**
     * �˳�
     */
    virtual void destroy()
    {
    };


    /**
    * udp�ϱ�ģ��������Ϣ
    * @param statmsg, �ϱ���Ϣ
    * @return int, ����0��ʾ�ɹ�
    */
    virtual int reportMicMsg( const map<taf::StatMicMsgHead, taf::StatMicMsgBody>& statmsg,
                              bool bFromClient,
                              taf::JceCurrentPtr current );
    /**
    * udp�ϱ�ģ�����ò�����Ϣ
    * @param sample, �ϱ���Ϣ
    * @return int, ����0��ʾ�ɹ�
    */
    virtual int reportSampleMsg(const vector<StatSampleMsg> &msg,taf::JceCurrentPtr current );

protected:

    int addHashMap(const StatMicMsgHead &head, const StatMicMsgBody &body);

    int addQueryHashMap(const StatMicMsgHead &head, const StatMicMsgBody &body);

private:
    void dump2file();
    string getSlaveName(const string& sSlaveName);
	/**
	 * ���taf������õ�����taserver����ʱ�ı��������д���
	 */
	string getNameFromTaServerName(const string& sSlaveName);
};

#endif


