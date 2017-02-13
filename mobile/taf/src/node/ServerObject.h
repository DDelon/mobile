#ifndef __SERVER_OBJECT_H_
#define __SERVER_OBJECT_H_
#include "Node.h"
#include "Registry.h"
#include <unistd.h>
#include "Activator.h"
#include "util/tc_file.h"
#include "util/tc_config.h"
#include "servant/AdminF.h"
#include "nodeF/NodeF.h"
#include "log/taf_logger.h"
#include "PlatformInfo.h"
#include "PropertyReporter.h"
#include "ServerLimitResource.h"
using namespace taf;
using namespace std;

class ServerObject : public TC_ThreadRecLock, public TC_HandleBase
{

public:
    enum InternalServerState
    {
        Inactive,
        Activating,
        Active,
        Deactivating,
        Destroying,
        Destroyed,
        Loading,
        Patching,
        BatchPatching,
        AddFilesing
    };
	enum EM_CoreType
	{
		EM_AUTO_LIMIT,
		EM_MANUAL_LIMIT
	};
	/**
	 *@brief ������̵�limit��Դ״̬������core������Դ
	 *
	 *�����corelimit��ʼ״̬Ĭ��ΪEM_AUTO_LIMIT+false
	 */
    struct ServerLimitInfo
    {
		bool    bEnableCoreLimit;  		   //��Դ���Կ���
		bool    bCloseCore;                //core���Ե�״̬������:true,��:false
		EM_CoreType eCoreType;             //����core�ĵ�ǰ���η�ʽ
		int 	iMaxExcStopCount;         //�������쳣ֹͣ����
		int32_t iCoreLimitTimeInterval;  //ʱ������core�����ƣ���λ�Ƿ���
		int32_t iCoreLimitExpiredTime;   //core�رյĳ���ʱ��,��λΪ����
		ServerLimitInfo():bEnableCoreLimit(false),bCloseCore(false),eCoreType(EM_AUTO_LIMIT),
								iMaxExcStopCount(3),iCoreLimitTimeInterval(5),iCoreLimitExpiredTime(30)
		{
		}
    };

public:

    /**
     * ���캯��
     * @param tDesc  server�����ṹ
     */
    ServerObject( const ServerDescriptor& tDesc);

    /**
     * ����
     */
    ~ServerObject() { };

    /**
     * �Ƿ���taf����
     *
     * @return bool
     */
    bool isTafServer() {return _bTafServer;}

    /**
     * �Ƿ���enable
     *
     * @return bool
     */
    bool isEnabled() {return _bEnabled;}

    /**
    * �����Ƿ��Զ�����
    *,������ֹ������񲻶�����Ӱ����������
    * @return  bool
    */
    bool isAutoStart() ;

    /**
     * ���÷���enable
     * @param enable
     */
    void setEnabled(bool enable){_bEnabled = enable; }

     /**
     * ���÷�����patch
     * @param enable
     */
    void setPatched(bool bPatched)
    {
        _bPatched               = bPatched;
        _tPatchInfo.iPercent    = 100;
    }

      /**
      *���÷�����load
     * @param enable
     */
    void setLoaded(bool bLoaded){_bLoaded = bLoaded;}

	/**
	 *�Ƿ�����ͬ�������״̬���������ģ���ĳЩ����£����񷢲��ˣ����ǻ���������ṩ����
	 *������������ֵ
	 *@param  bEn
	 */
	void setEnSynState(bool bEn) { _bEnSynState = bEn;}

	bool IsEnSynState(){ return _bEnSynState;}

public:

    /**
    * ��֤server��Ӧpid�Ƿ����
    * @return  int
    */
    int checkPid();

    /**
    * ����server��Ӧpid
    */
    void setPid( pid_t pid );

    /**
    * ��ȡserver��Ӧpid
    */

    int getPid() { return _pid; }


    /**
    * ��ȡserver��Ӧ����socket
    */

    TC_Endpoint getLocalEndpoint() { return _tLocalEndpoint; }


    /**
    * ����server��Ӧ����socket
    */

    void setLocalEndpoint(const TC_Endpoint &tLocalEndpoint) {  _tLocalEndpoint = tLocalEndpoint; }


    /**
    * ��ȡserver�ϱ���Ϣ
    * @para string obj
    * @para pid_t pid�ϱ�pid
    * @return void
    */
    void keepAlive( pid_t pid, const string &adapter="");

    /**
    * ȡ��server���keepAliveʱ��
    * @return int
    */
    int getLastKeepAliveTime(const string &adapter="");

    /**
    * ����server���keepAliveʱ��
    * @return void
    */
    void setLastKeepAliveTime(int t,const string &adapter="");

    /**
     * �����Ƿ���timeout
     * @param iTimeout
     * @return bool
     */
    bool isTimeOut(int iTimeout);

public:

    /**
     * ��registryͬ��server��ǰ״̬
     * @return void
     */
    void synState();

    /**
     * �첽��registryͬ��server��ǰ״̬
     * @return void
     */
    void asyncSynState();

    /**
     * ����server��ǰ״̬
     * @param eState
     * @param bSynState  �Ƿ�ͬ��server״̬��registry
     * @return  void
     */
    void setState( InternalServerState eState, bool bSynState=true);

    /**
    *����server��һ��״̬����server״̬�����仯ʱ����
    */
    void setLastState(InternalServerState eState);

    /**
     * ��ȡ����״̬
     *
     * @return taf::ServerState
     */
    taf::ServerState getState();

    /**
     * ��ȡserver�ڲ�״̬
     * @return void
     */
    InternalServerState getInternalState() { return  _eState; }

    /**
    *��ȡserver��һ���ڲ�״̬
    *@return InternalServerState
    */
    InternalServerState getLastState() {return _eLastState;}

    /**
    *ת��ָ��server�ڲ�״̬Ϊ�ַ�����ʽ
    * @para  eState
    * @return string
    */
    string toStringState( InternalServerState eState ) const;

    /**
    *ת��ָ��server�ڲ�״̬Ϊone::ServerState��ʽ
    * @para  eState
    * @return ServerState
    */
    ServerState toServerState( InternalServerState eState ) const;

    /**
    *���server״̬
    * @para  iTimeout ������ʱʱ��
    * @return void
    */
    void checkServer(int iTimeout);

    /**
    *���ýű��ļ�
    * @para tConf �ű��ļ�
    * @return void
    */
    string decodeMacro(const string& value) const ;

public:

    /**
    *�Ƿ�Ϊ�ű��ļ�
    * @para sFileName �ļ���
    * @return bool
    */
    bool isScriptFile(const string &sFileName);

    /**
    * ��ȡ�ű��ļ�
    * @para sFileName �ļ���
    * @return bool
    */
    bool getRemoteScript(const string &sFileName);

    /**
    *ִ�м�ؽű��ļ�
    * @para sFileName �ļ���
    * @return void
    */
    void doMonScript();

public:
    /**
    *s�������ذٷֱ�
    * @para iPercent �ļ������ذٷֱ�
    * @return bool
    */
    void setPatchPercent(const int iPercent);

    /**
    *s�������ؽ��
    * @para sPatchResult
    * @return bool
    */
    void setPatchResult(const string &sPatchResult,const bool bSucc = false);

    /**
    *s�������ذ汾
    * @para sPatchResult
    * @return bool
    */
    void setPatchVersion(const string &sVersion);

    /**
     * ��ȡ�������صİ汾��
     *
     * @author kevintian (2010-9-21)
     *
     * @return string
     */
    string getPatchVersion();

    /**
    * ��ȡ������Ϣ
    * @para tPatchInfo
    * @return int
    */
    int getPatchPercent(PatchInfo &tPatchInfo);

public:
    ServerDescriptor getServerDescriptor() { return  _tDesc; }
    ActivatorPtr getActivator() { return  _pActivatorPtr; }
    string getExePath(){return _sExePath;}
    string getExeFile(){return _sExeFile;}
    string getConfigFile(){return _sConfFile;}
    string getLogPath(){return _sLogPath;}
    string getLibPath(){return _sLibPath;}
    string getServerDir(){return _sServerDir;}
    string getServerId(){return _sServerId;}
    string getServerType(){return _sServerType;}
    string getStartScript() {return _sStartScript;}
    string getStopScript() {return _sStopScript;}
    string getMonitorScript() {return _sMonitorScript;}
    string getEnv() { return _sEnv; }
	string getRedirectPath() {return _sRedirectPath;}

    //java����
    string getJvmParams() {return _sJvmParams;}
    string getMainClass() {return _sMainClass;}
    string getClassPath() {return _sClassPath;}
	string getBackupFileNames(){return _sBackupFiles;}

    void setServerDescriptor( const ServerDescriptor& tDesc );
    void setVersion( const string &version );
    void setExeFile(const string &sExeFile){_sExeFile = sExeFile;}
    void setExePath(const string &sExePath){_sExePath = sExePath;}
    void setConfigFile(const string &sConfFile){_sConfFile = sConfFile;}
    void setLogPath(const string &sLogPath){_sLogPath = sLogPath;}
    void setLibPath(const string &sLibPath){_sLibPath = sLibPath;}
    void setServerDir(const  string &sServerDir){_sServerDir = sServerDir;}
    void setNodeInfo(const NodeInfo &tNodeInfo){_tNodeInfo = tNodeInfo;}
    void setServerType( const string &sType ){ _sServerType = TC_Common::lower(TC_Common::trim(sType));_sServerType == "not_taf"?_bTafServer = false:_bTafServer=true;}
    void setMacro(const map<string,string>& mMacro);
    void setScript(const string &sStartScript,const string &sStopScript,const string &sMonitorScript);

    void setEnv(const string & sEnv) { _sEnv = sEnv; }
    void setHeartTimeout(int iTimeout) { _iTimeout = iTimeout; }

    //java����
    void setJvmParams(const string &sJvmParams){_sJvmParams = sJvmParams;}
    void setMainClass(const string &sMainClass){_sMainClass = TC_Common::trim(sMainClass);}
    void setClassPath(const  string &sClassPath){_sClassPath = sClassPath;}
	void setRedirectPath(const string& sRedirectpath) {_sRedirectPath = sRedirectpath;}
	void setBackupFileNames(const string& sFileNames){_sBackupFiles = sFileNames;}

public:
	/**
	 * auto check routine
	 */
	void checkCoredumpLimit();

	/**
	 * server restart and set limitupdate state
	 */
	void setLimitInfoUpdate(bool bUpdate);

	void setServerLimitInfo(const ServerLimitInfo& tInfo);

	bool setServerCoreLimit(bool bCloseCore);

private:
    bool    _bTafServer;                //�Ƿ�taf����
    string  _sServerType;               //��������  taf_cpp taf_java not_taf
private:
    bool    _bEnabled;                  //�����Ƿ���Ч
    bool    _bLoaded;                   //���������Ƿ��ѳɹ�����
    bool    _bPatched;                  //�����ִ�г����Ƿ��ѳɹ�����
    bool    _bNoticed;                  //����ǰ״̬�Ƿ���֪ͨregistry��
    unsigned _uNoticeFailTimes;         //ͬ������״̬��registry����ʧ�ܴ���
    bool    _bEnSynState;				  //�Ƿ�����ͬ�������״̬������

private:
    string  _sApplication;              //��������app
    string  _sServerName;               //��������
    string  _sServerId;                 //����idΪApp.ServerName.Ip��ʽ

private:
    string _sJvmParams;             //java����jvm
    string _sMainClass;             //java����main class
    string _sClassPath;             //java���� class path
	string _sRedirectPath;		     //��׼����ʹ�������ض���Ŀ¼
private:
    string _sStartScript;           //�����ű�
    string _sStopScript;            //ֹͣ�ű�
    string _sMonitorScript;         //��ؽű�

private:
    string _sServerDir;               //��������Ŀ¼
    string _sConfFile;                //���������ļ�Ŀ¼ _sConfPath/conf
    string _sExePath;                 //һ��Ϊ_sServerDir+"/bin" �ɸ���ָ��
    string _sExeFile;                 //һ��Ϊ_sExePath+_sServerName �ɸ���ָ��
    string _sLogPath;                 //������־Ŀ¼
    string _sLibPath;                 //��̬��Ŀ¼ һ��Ϊ_tDesc.basePath/lib
    map<string,string> _mMacro;       //�����

private:
    PatchInfo           _tPatchInfo;        //������Ϣ

private:
    pid_t               _pid;                   //������̺�
    string              _sVersion;              //TAF�汾
    NodeInfo            _tNodeInfo;             //��������node ��Ϣ
    TC_Endpoint         _tLocalEndpoint;        //����socket
    ServerDescriptor    _tDesc;                 //������������Ҫ�������������ļ���
    ActivatorPtr        _pActivatorPtr;         //����������ֹͣ����
    time_t              _tKeepAliveTime;        //��������ϱ�ʱ��
    map<string,time_t>  _mAdapterKeepAliveTime; //��adapter����ϱ�ʱ�䣬�����ж�adapter�Ƿ��ϱ���ʱ

    InternalServerState _eState;                //��ǰ����״̬
    InternalServerState _eLastState;        //��һ�η���״̬

    int                 _iTimeout;              //������ʱʱ��
    string              _sEnv;                  //���������ַ���
    string 				_sBackupFiles;          //���java���񷢲�ʱbinĿ¼����Ҫ�������ļ���������;|���ָ�
private:
	bool 				_bLimitStateUpdated;         //�����limit�����Ƿ��и��£�����Ҳ�����
	ServerLimitInfo  	_tLimitStateInfo;             //ͨ��node�������õ���Դ��Ϣ
	ServerLimitResourcePtr _pServiceLimitResource;
};

typedef TC_AutoPtr<ServerObject> ServerObjectPtr;

#endif

