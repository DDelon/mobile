#ifndef _LOG_IMP_H
#define _LOG_IMP_H

#include "Log.h"
#include "util/tc_common.h"
#include "util/tc_file.h"
#include "util/tc_logger.h"
#include "util/tc_monitor.h"
#include "LogServer.h"

using namespace taf;

/**
 * ȫ����Ϣ
 */
struct GlobeInfo : public TC_ThreadLock
{
public:
    /**
     * ��ȡ������־
     * @param app
     * @param server
     * @param file
     * @param format
     * @param ip �������ڽڵ�ip
     *
     * @return TC_DayLogger&
     */
    TC_DayLogger& getLogger(const string &app, const string &server, const string &file, const string &format,const string& ip);
    /**
     * ��ȡ������־
     * @param info ����ο�LogInfo
     * @param ip �������ڽڵ�ip
     *
     * @return TC_DayLogger&
     */
	TC_DayLogger& getLogger(const LogInfo & info,const string& ip);
protected:
    /**
     * ���ɰ�����־
     * @param info ����ο�LogInfo
     * @param logname
     * @param format
     * @param ip �������ڽڵ�ip
     *
     * @return TC_DayLogger&
     */
    TC_DayLogger &makeDayLogger(const LogInfo & info, const string &logname, const string &format,const string& ip);

	/**
     * ���ɰ�����־
     * @param app
     * @param server
     * @param logname
     * @param format
     * @param ip �������ڽڵ�ip
     *
     * @return TC_DayLogger&
     */
    TC_DayLogger &makeDayLogger(const string &app, const string &server, const string &logname, const string &format,const string& ip);

	/**
     * �ж���Ӧ��logname���Ƿ���ָ����format
     * @param logname ��file.empty() ? (app + "." + server) : (app + "." + server + "_" + file)��ʽ���
     * @param format
     * @param[out] ip �����ָ����format���ڣ��򷵻ظ�format��Ӧ�Ľڵ�ip
     *
     * @return bool
     */
    bool HasSameFormat(const string& logname,const string& format,string& ip);
	/**
     * �ж���logname�£����˽ڵ�sExcludeIp�⣬�Ƿ��������ڵ���ʹ��pLogerָ����loggerʵ��
     * @param logname
     * @param sExcludeIp
     * @param pLogger
     *
     * @return bool
     */
	bool IsLoggerAttached(const string& logname,const string& sExcludeIp,const TC_DayLogger* pLoger);
private:
	/**
	 * ��ȡset������ʽ
	 * @param sSetDivision set������Ϣ,����:mtt.s.1 mtt.s.*
	 *
	 * @return ������ĸ�ʽ������:mtt.s.* ȥ��"*"���ţ�����mtts��mtts1��ԭ������
	 */
	string getSetGoodFormat(const string& sSetDivision);
	/**
	 * ��LogInfo�л�ȡ�����Ϣ�õ���ʶһ��logger������ַ���
	 * @param info
	 */
	string getLogName(const LogInfo & info);

	/**
	 * ��LogInfo�л�ȡ��־�ļ����ƣ�����·��������
	 * @param info
	 */
	string getRealLogName(const LogInfo & info);

public:
    /**
     * д��־�߳�
     */
    TC_LoggerThreadGroup        _group;

    /**
     * д��־·��
     */
    string                      _log_path;

    /**
     * logger����:logname,ip,logger
     */
    map<string, map<string,TC_DayLogger*> >  _loggers;

    /**
     * logger��ʽ:logname,ip,format
     */
    map<string, map<string,string> >        _formats;

	vector<string> _vHourlist;
	
	/**
     * app idc��ϵ:MDWWX,MDW
     */	 
    map<string,string> _mAppIdcRelation;

};

extern GlobeInfo   g_globe;

/**
 * logʵ��
 */
class LogImp : public Log
{
public:
    /**
     *
     */
    LogImp(){};

    /**
     *
     */
    ~LogImp(){};

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize();

    /**
     * �˳�
     */
    virtual void destroy() {};

    /**
     * �����־��Ϣ��ָ���ļ�
     * @param app    ҵ������
     * @param server ��������
     * @param file   ��־�ļ�����
     * @param format ��־�����ʽ
     * @param buffer ��־����
     *
     *
     */
    void logger(const string &app, const string &server, const string &file, const string &format, const vector<string> &buffer, taf::JceCurrentPtr current);
    /**
     * ��ȡ����
     * @param info
     * @param buffer
     *
     */
	void loggerbyInfo(const LogInfo & info,const vector<std::string> & buffer,taf::JceCurrentPtr current);
protected:

    /**
     * ���������ļ�����־format format��ͻ��˲�һ��ʱ
     * ��logserver�����ļ��и�ʽΪ׼
     */

    //bool loadLogFormat(const string& command, const string& params, string& result);
    /**
     * �����־·��
     * @param app
     * @param server
     */
    //void checkLogPath(const string &app, const string &server);

private:

    //vector<string> _vHourlist;
};

#endif

