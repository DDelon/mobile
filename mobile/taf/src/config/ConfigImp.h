#ifndef _CONFIG_IMP_H_
#define _CONFIG_IMP_H_

#include "util/tc_common.h"
#include "util/tc_config.h"
#include "util/tc_mysql.h"
#include "log/taf_logger.h"

#include "Config.h"


using namespace taf;
enum Level
{
    eLevelApp = 1,
    eLevelAllServer,
    eLevelIpServer
};

class ConfigImp : public Config
{
public:
    /**
     *
     */
    ConfigImp(){};

    /**
     *
     */
    ~ConfigImp(){};

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
     * ��ȡ�����ļ��б� 
     * param app :Ӧ�� 
     * param server:  server�� 
     * param vf: �����ļ���
     *
     * return  : �����ļ�����
     */
    virtual int ListConfig(const string &app,const string &server,  vector<string> &vf,taf::JceCurrentPtr current);

    /**
     * ���������ļ�
     * param app :Ӧ��
     * param server:  server��
     * param filename:  �����ļ���
     *
     * return  : �����ļ�����
     */
    virtual int loadConfig(const std::string & app, const std::string & server, const std::string & filename, string &config, taf::JceCurrentPtr current);

    /**
     * ����ip��ȡ����
     * @param appServerName
     * @param filename
     * @param host
     * @param config
     *
     * @return int
     */
    virtual int loadConfigByHost(const string &appServerName, const string &filename, const string &host, string &config, taf::JceCurrentPtr current);

    /**
     *
     * @param appServerName
     * @param filename
     * @param host
     * @param current
     *
     * @return int
     */
    virtual int checkConfig(const string &appServerName, const string &filename, const string &host, string &result, taf::JceCurrentPtr current);

	 /**
     * ��ȡ�����ļ��б�
     * param configInfo ConfigInfo
     * param vf: �����ļ���
     *
     * return  : �����ļ�����
     */
	virtual int ListConfigByInfo(const ConfigInfo& configInfo,  vector<string> &vf,taf::JceCurrentPtr current);
    /**
     * ���������ļ�
     * param configInfo ConfigInfo
     * param config:  �����ļ�����
     *
     * return  :
     */
	virtual int loadConfigByInfo(const ConfigInfo & configInfo, string &config,taf::JceCurrentPtr current);
    /**
     *
     * @param configInfo ConfigInfo
     *
     * @return int
     */
    virtual int checkConfigByInfo(const ConfigInfo & configInfo, string &result,taf::JceCurrentPtr current);

protected:
    /**
     * ���ر�server�������ļ�
     */
    void loadconf();

private:

    /**
     *
     * ��ѯconfigId��Ӧ������
     *
     * @param iConfigId
     * @param sConfig
     *
     * @return int 0���ɹ������޴�config,��ʾӦ�ü�����
     *             2,��ʾsetӦ������
     */
    int loadConfigByPK(int iConfigId, string &sConfig);
    /**
     *
     * ��ѯconfigId��Ӧ��������Ϣ��ͬʱ��鿴��
     *
     * @param iConfigId
     * @param setdivision set������Ϣ
     * @param sConfig
     *
     * @return int 0���ɹ�
     *
     */
    int loadRefConfigByPK(int iConfigId, const string& setdivision,string &sConfig);
    /**
     * 
     * ��ȡiConfigId��Ӧ������id�б�
     * 
     * @param iConfigId 
     * @param referenceIds 
     * 
     * @return int 
     */
    int getReferenceIds(int iConfigId,vector<int> &referenceIds);


    /**
     * ��ȡӦ�ü���������
     * @param appName  Ӧ������
     * @param fileName �ļ�����
     * @out param      ��ȡ���������ļ�����
     *
     * @return int 
     */
    int loadAppConfig(const std::string& appName, const std::string& fileName, string &config, taf::JceCurrentPtr current);

    /**
     * ��ȡӦ�ü���������
     * @param configInfo  ConfigInfo
     * @out param      ��ȡ���������ļ�����
     *
     * @return int
     */
    int loadAppConfigByInfo(const ConfigInfo & configInfo, string &config, taf::JceCurrentPtr current);
    /**
     * ����ip��ȡ����
     * @param configInfo  ConfigInfo��configInfo��ָ��ip�����û�����current��ȡ��ǰ�����ip
     * @param config
     *
     * @return int
     */
	int loadConfigByHost(const ConfigInfo & configInfo, string &config, taf::JceCurrentPtr current);

    /**
     * ��ȡset������Ϣ
     * @out param sSetName  set����
     * @out param sSetArea  set��������
     * @out param sSetGroup set��������
     * @param  sSetDivision   set��Ϣ:sSetName.sSetArea.sSetGroup��ʽ
     *
     * @return bool set��Ϣ��ȷ�򷵻�true
     */
	bool getSetInfo(string& sSetName,string& sSetArea,string& sSetGroup,const string& sSetDivision);

	/**
	 * �ϲ����������ļ�,����г�ͻ��,��sHighConf���ȼ���sLowConf��
	 *
	 * @param sLowConf  Ӧ������
	 * @param sHighConf  Ӧ������
	 *
	 * @ return string �ϲ��Ľ��
	 */
	string mergeConfig(const string& sLowConf,const string& sHighConf);

protected:
    TC_Mysql _mysqlConfig;
};

#endif

