#ifndef __LOAD_COMMAND_H_
#define __LOAD_COMMAND_H_

#include "ServerCommand.h"

/**
 * ���ط���
 *
 */
class CommandLoad : public ServerCommand
{

public:
    CommandLoad(const ServerObjectPtr &pServerObjectPtr,const NodeInfo &tNodeInfo,bool bByNode = false);
    ExeStatus canExecute(string &sResult);
    int execute(string &sResult);
private:

    /**
    *����server��Ӧ�����ļ�
    * @return int
    */
    int updateConfigFile(string &sResult);
    /**
    *���滻
    * @para  macro ��map
    * @para  value ���滻�ַ���
    * @return string �滻���ַ���
    */
    string decodeOption(const map<string,string>& macro,const string& value);

    /**
    * ��ȡserver�����ļ�
    * @return int
    */
    void getRemoteConf();

private:
    bool                _bByNode;
    NodeInfo            _tNodeInfo;
    ServerDescriptor    _tDesc;
    ServerObjectPtr     _pServerObjectPtr;
    StatExChangePtr     _pStatExChange;

private:
    string _sServerDir;               //��������Ŀ¼
    string _sConfPath;                //���������ļ�Ŀ¼
    string _sConfFile;                //���������ļ�Ŀ¼ _strConfPath/conf
    string _sExePath;                 //�����ִ�г���Ŀ¼���ɸ����趨,һ��Ϊ_strServerDir/bin
    string _sExeFile;                 //һ��Ϊ_strExePath+_strServerName �ɸ���ָ��
    string _sLogPath;                 //������־Ŀ¼
    string _sLibPath;                 //��̬��Ŀ¼ һ��Ϊ_tDesc.basePath/lib
    string _sServerType;              //��������
private:
    string _sStartScript;               //�����ű�
    string _sStopScript;                //ֹͣ�ű�
    string _sMonitorScript;             //��ؽű�
};

//////////////////////////////////////////////////////////////
//
inline CommandLoad::CommandLoad(const ServerObjectPtr &pServerObjectPtr,const NodeInfo &tNodeInfo,bool bByNode)
:_bByNode(bByNode)
,_tNodeInfo(tNodeInfo)
,_pServerObjectPtr(pServerObjectPtr)
{
    _tDesc      = _pServerObjectPtr->getServerDescriptor();
}
//////////////////////////////////////////////////////////////
//

inline ServerCommand::ExeStatus CommandLoad::canExecute(string &sResult)
{

    TC_ThreadRecLock::Lock lock( *_pServerObjectPtr );

    LOG->debug() << _tDesc.application<< "." << _tDesc.serverName <<"|"<<_tDesc.setId<< "| beging loaded------|"<< endl;

    ServerObject::InternalServerState eState = _pServerObjectPtr->getInternalState();

    if(_tDesc.application == "" || _tDesc.serverName == "" )
    {
        sResult = "app or server name is empty";
        LOG->debug()<< sResult <<endl;
        return DIS_EXECUTABLE;
    }

    if( _pServerObjectPtr->toStringState(eState).find( "ing" ) != string::npos )
    {
       sResult = "cannot loading the config, the server state is " + _pServerObjectPtr->toStringState( eState );
       LOG->debug() << sResult << endl;
       return DIS_EXECUTABLE;
    }
    //�赱ǰ״̬Ϊ����loading
    _pStatExChange = new StatExChange(_pServerObjectPtr,ServerObject::Loading, eState);

    return EXECUTABLE;
}


//////////////////////////////////////////////////////////////
//
inline int CommandLoad::execute(string &sResult)
{
    //��ʼ������Ŀ¼��Ϣ
    _sServerDir     = TC_Common::trim(_tDesc.basePath);
    _sExePath       = TC_Common::trim(_tDesc.exePath);
    _sServerType    = TC_Common::lower(TC_Common::trim(_tDesc.serverType));

    //��serverDir���Ϸ�����Ĭ��·��
    if( _sServerDir.empty() || TC_File::isAbsolute(_sServerDir ) ==  false )
    {
        _sServerDir = TC_File::simplifyDirectory(_tNodeInfo.dataDir + "/" +  _tDesc.application +"."+ _tDesc.serverName );
    }

    //��exePath���Ϸ�����Ĭ��·��
    //ע��java����������ʽ���� ��ִ���ļ�Ϊjava �����⴦��
    if(_sExePath.empty())
    {
        _sExePath =  _sServerDir + "/bin/";
        _sExeFile =  _sServerType == "taf_java"?"java":_sExePath + _tDesc.serverName;
    }
    else if( TC_File::isAbsolute(_sExePath) ==  false)
    {
       //��ʱ_tDesc.exePathΪ�ֹ�ָ�����ֹ�ָ��ʱ_tDesc.exePathΪ�ļ� ����·��Ҫ�۳���ִ���ļ���
      //���ҿ�ִ���ļ������Բ�����_strServerName ������ͬһ��ִ���ļ�����ͬ���������������
         _sExeFile =  _sServerDir + "/bin/" + _sExePath;
         _sExePath = TC_File::extractFilePath(_sExeFile);
    }
    else
    {
      //��ʱ_tDesc.exePathΪ�ֹ�ָ�����ֹ�ָ��ʱ_tDesc.exePathΪ�ļ� ����·��Ҫ�۳���ִ���ļ���
      //���ҿ�ִ���ļ������Բ�����_strServerName ������ͬһ��ִ���ļ�����ͬ���������������
        _sExeFile   = _tDesc.exePath;
        _sExePath   = _sServerType == "taf_java"?_sServerDir + "/bin/":TC_File::extractFilePath(_tDesc.exePath);
    }
    _sExeFile = TC_File::simplifyDirectory(_sExeFile);

    _sExePath = TC_File::simplifyDirectory(_sExePath)+"/";

    //�����ű�����
    _sStartScript   = TC_Common::trim(_tDesc.startScript);
    if(!_sStartScript.empty() && TC_File::isAbsolute(_sStartScript) ==  false)
    {
        _sStartScript =  _sExePath + _sStartScript;
    }

    //ֹͣ�ű�����
    _sStopScript   = TC_Common::trim(_tDesc.stopScript);
    if(!_sStopScript.empty() && TC_File::isAbsolute(_sStopScript) ==  false)
    {
        _sStopScript =  _sExePath + _sStopScript;
    }

    //��ؽű�����
    _sMonitorScript   = TC_Common::trim(_tDesc.monitorScript);
    if(!_sMonitorScript.empty() && TC_File::isAbsolute(_sMonitorScript) ==  false)
    {
        _sMonitorScript =  _sExePath + _sMonitorScript;
    }

    _sStartScript   = TC_File::simplifyDirectory(_sStartScript);
    _sStopScript    = TC_File::simplifyDirectory(_sStopScript);
    _sMonitorScript = TC_File::simplifyDirectory(_sMonitorScript);

    //��������lib�ļ�Ŀ¼
    _sLibPath     = _tNodeInfo.dataDir +"/lib/";

    //��ȡ�����������ļ�
    _sConfPath    = _sServerDir + "/conf/";

    _sConfFile    = _sConfPath+"/"+_tDesc.application+"."+_tDesc.serverName+".config.conf";

    //ɾ���ɰ汾��config  �°汾�����Ѳ�ͬ  1.17�汾��node��ȥ��
    if(TC_File::isFileExist( _sConfPath+"/config.conf" ))
    {
        TC_File::removeFile(_sConfPath+"/config.conf",false);
        TC_File::removeFile(_sConfPath+"/config.conf.bak",false);
    }

    //����Ŀ¼
     if(!TC_File::makeDirRecursive(_sExePath ) )
    {
        sResult =  "cannot create dir: " + _sExePath + " erro:"+ strerror(errno);
        LOG->debug() <<sResult<< endl;
        return -1;
    }
     if(!TC_File::makeDirRecursive( _sLibPath ))
    {
        sResult =  "cannot create dir: " + _sLibPath+ " erro:"+ strerror(errno);
        LOG->debug() <<sResult<< endl;
        return -1;
    }
    if(!TC_File::makeDirRecursive( _sConfPath ) )
    {
        sResult =  "cannot create dir: " + _sConfPath+ " erro:"+ strerror(errno);
        LOG->debug() <<sResult<< endl;
        return -1;
    }
    if(updateConfigFile(sResult) != 0 )
    {
        sResult += "update config error";
        LOG->debug() <<sResult << endl;
        return -1;
    }
    getRemoteConf();
    LOG->debug()<<_tDesc.application<< "." << _tDesc.serverName <<" succ loaded"<< endl;
    return 0;
}

inline int CommandLoad::updateConfigFile(string &sResult)
{
    try
    {
        //node����server desc�������á�
        TC_Config           tConf;
        TC_Endpoint         tEp;
        map<string, string> m;
        m["node"] =ServerConfig::Application + "." + ServerConfig::ServerName +".ServerObj@"+g_app.getAdapterEndpoint("ServerAdapter").toString();
        tConf.insertDomainParam( "/taf/application/server", m, true );
        m.clear();
        map<string, AdapterDescriptor>::const_reverse_iterator itAdapters;
        for ( itAdapters = _tDesc.adapters.rbegin(); itAdapters != _tDesc.adapters.rend(); itAdapters++ )
        {
            LOGINFO("get adapter " << itAdapters->first << endl);
            _tDesc.display(LOG->info());
            if(itAdapters->first == "")
            {
                continue;
            }

            tEp.parse(itAdapters->second.endpoint);
            m["endpoint"]       = tEp.toString();
            m["allow"]          = itAdapters->second.allowIp;
            m["queuecap"]       = TC_Common::tostr( itAdapters->second.queuecap );
            m["queuetimeout"]   = TC_Common::tostr( itAdapters->second.queuetimeout);
            m["maxconns"]       = TC_Common::tostr( itAdapters->second.maxConnections );
            m["threads"]        = TC_Common::tostr( itAdapters->second.threadNum );
            m["servant"]        = TC_Common::tostr( itAdapters->second.servant );
            m["protocol"]       = itAdapters->second.protocol == ""?"taf":itAdapters->second.protocol;
            m["handlegroup"]    = itAdapters->second.handlegroup == ""?itAdapters->first:itAdapters->second.handlegroup;
            m["shmkey"]         = TC_Common::tostr( itAdapters->second.shmkey );
            m["shmcap"]         = TC_Common::tostr( itAdapters->second.shmcap );
            tConf.insertDomainParam("/taf/application/server/" + itAdapters->first, m, true );
        }
        //��ȡ����socket
        TC_Endpoint tLocalEndpoint;
        uint16_t p = tEp.getPort();
        if( p == 0 )
        {
            try
            {
                //ԭʼ�����ļ�����admin�˿���, ֱ��ʹ��
                TC_Config conf;
                conf.parseFile(_sConfFile);
                TC_Endpoint ep;
                ep.parse( conf.get("/taf/application/server<local>"));
                p = ep.getPort();
            }
            catch(exception &ex)
            {
                //�������һ���˿�
                TC_Socket t;
                t.createSocket();
                t.bind("127.0.0.1", 0);
                string d;
                t.getSockName(d, p);
                t.close();
            }

        }
        tLocalEndpoint.setPort(p);
        tLocalEndpoint.setHost("127.0.0.1");
        tLocalEndpoint.setTcp(true);
        tLocalEndpoint.setTimeout(3000);

        //��Ҫ���滻��������
        TC_Config tConfMacro;
        map<string,string> mMacro;
        mMacro.clear();
        mMacro["locator"]       = Application::getCommunicator()->getProperty("locator");

        //>>�޸ĳɴ����ػ�ȡlocator��ַ
        vector<taf::EndpointF> activeEp;
        vector<taf::EndpointF> inactiveEp;
        QueryFPrx queryProxy = AdminProxy::getInstance()->getQueryProxy();
	int iRet = 0;
	try
	{
		 iRet = queryProxy->findObjectById4All(AdminProxy::getInstance()->getQueryProxyName(), activeEp, inactiveEp);
		LOG->debug() << __FUNCTION__ << ":" << __LINE__ <<  ":" << _pServerObjectPtr->getServerId() << "|iRet|" << iRet << "|"<< activeEp.size() << "|" << inactiveEp.size() << endl;
	}
	catch(exception &e)
	{//��ȡ���ص�ַ�쳣ʱ,��ʹ��node�е�locator
		LOG->error() << __FUNCTION__ << ":" << __LINE__ <<":get registry locator excetpion:" << e.what() << "|" << _pServerObjectPtr->getServerId() << endl;
		iRet = -1;
	}
	catch(...)
	{
		LOG->error() << __FUNCTION__ << ":" << __LINE__ <<":get registry locator unknown exception|" << _pServerObjectPtr->getServerId() << endl;
		iRet = -1;
	}

	if (0 == iRet && activeEp.size()>0)
	{
		string sLocator = AdminProxy::getInstance()->getQueryProxyName() + "@";
		for (size_t i=0; i<activeEp.size(); ++i)
		{
			string sSingleAddr = "tcp -h " + activeEp[i].host + " -p " + TC_Common::tostr(activeEp[i].port);
			sLocator += sSingleAddr + ":";
		}
		sLocator = sLocator.substr(0,sLocator.length()-1);
		mMacro["locator"] = sLocator;
		LOG->debug() << __FUNCTION__ << ":" << __LINE__ << "|" << _pServerObjectPtr->getServerId() << "|locator|" << sLocator << endl;
	}
        //<<modified by spinnerxu@20110726

        mMacro["modulename"]    = _tDesc.application+"."+_tDesc.serverName;
        mMacro["app"]           = _tDesc.application;
        mMacro["server"]        = _tDesc.serverName;
        mMacro["serverid"]      = _pServerObjectPtr->getServerId();
        mMacro["localip"]       = g_app.getAdapterEndpoint("ServerAdapter").getHost();
        mMacro["exe"]           = TC_File::simplifyDirectory(_sExeFile);
        mMacro["basepath"]      = TC_File::simplifyDirectory(_sExePath)+"/";
        mMacro["datapath"]      = TC_File::simplifyDirectory(_sServerDir) + "/data/";
        mMacro["logpath"]       = ServerConfig::LogPath;
        mMacro["local"]         = tLocalEndpoint.toString();

        mMacro["mainclass"]             = "com.qq."+TC_Common::lower(_tDesc.application)+"."+TC_Common::lower(_tDesc.serverName)+"."+_tDesc.serverName;
        mMacro["config-center-port"]    = TC_Common::tostr(_tDesc.configCenterPort);

		mMacro["setdivision"] = _tDesc.setId;

		mMacro["enableset"] = "n";
		if (!mMacro["setdivision"].empty())
		{
			mMacro["enableset"] = "y";
		}
		else
		{
			mMacro["setdivision"] = "NULL";
		}

		mMacro["asyncthread"] = TC_Common::tostr(_tDesc.asyncThreadNum);

        //����Ŀ¼
        TC_File::makeDirRecursive(mMacro["basepath"]);
        TC_File::makeDirRecursive(mMacro["datapath"]);
        TC_File::makeDirRecursive(_sLogPath + "/"+_tDesc.application+"/"+_tDesc.serverName+"/");

        //�ϲ���������
        _pServerObjectPtr->setMacro(mMacro);
        string strProfile = _pServerObjectPtr->decodeMacro(_tDesc.profile);
        tConfMacro.parseString(strProfile);
        tConf.joinConfig(tConfMacro,true);
        string sStream  = TC_Common::replace(tConf.tostr(),"\\s"," ");
        string sConfigFileBak = _sConfFile + ".bak";
        if(TC_File::isFileExist( _sConfFile ) && TC_File::load2str(_sConfFile) != sStream)
        {
            TC_File::copyFile(_sConfFile,sConfigFileBak);
        }
        ofstream configfile(_sConfFile.c_str());
        if(!configfile.good())
        {
            sResult = "cannot create configuration file: " + _sConfFile;
            LOG->error() << sResult << endl;
            return -1;
        }
        configfile<<sStream;
        configfile.close();
        _sLogPath       = tConf.get("/taf/application/server<logpath>","");

        _pServerObjectPtr->setJvmParams(tConf.get("/taf/application/server<jvmparams>",""));
        _pServerObjectPtr->setMainClass(tConf.get("/taf/application/server<mainclass>",""));
        _pServerObjectPtr->setClassPath(tConf.get("/taf/application/server<classpath>",""));
        _pServerObjectPtr->setEnv(tConf.get("/taf/application/server<env>", ""));
        _pServerObjectPtr->setHeartTimeout(TC_Common::strto<int>(tConf.get("/taf/application/server<hearttimeout>", "")));

		_pServerObjectPtr->setRedirectPath(tConf.get("/taf/application/<redirectpath>", ""));

		_pServerObjectPtr->setBackupFileNames(tConf.get("/taf/application/server<backupfiles>", "classes/autoconf"));

		bool bEn = (TC_Common::lower(tConf.get("/taf/application/server<enableworking>", "y")) == "y")?true:false;

		_pServerObjectPtr->setEnSynState(bEn);

        _pServerObjectPtr->setExeFile(_sExeFile);
        _pServerObjectPtr->setConfigFile(_sConfFile);
        _pServerObjectPtr->setExePath(_sExePath);
        _pServerObjectPtr->setLogPath(_sLogPath);
        _pServerObjectPtr->setLibPath(_sLibPath);
        _pServerObjectPtr->setServerDir(_sServerDir);
        _pServerObjectPtr->setNodeInfo(_tNodeInfo);
        _pServerObjectPtr->setLocalEndpoint(tLocalEndpoint);
        _pServerObjectPtr->setServerType(_sServerType);
        _pServerObjectPtr->setScript(_sStartScript,_sStopScript,_sMonitorScript);
        _pServerObjectPtr->setLoaded(true);
        return 0;
    }
    catch (exception& e)
    {
        sResult =e.what();
        LOG->error() << sResult << endl;
    }
    catch (...)
    {
        sResult = "updateConfigFile catch unkown erro";
        LOG->error() << sResult << endl;
    }
    return -1;
}

//���������ļ��ļ�����
inline void CommandLoad::getRemoteConf()
{
    string sResult;
    try
    {
        ConfigPrx pPtr = Application::getCommunicator()->stringToProxy<ConfigPrx>(ServerConfig::Config);
        vector<string> vf;
		int ret;

		if(_tDesc.setId.empty())
		{
			ret = pPtr->ListConfig(_tDesc.application, _tDesc.serverName,vf);
		}
		else
		{
			struct ConfigInfo confInfo;
			confInfo.appname = _tDesc.application;
			confInfo.servername = _tDesc.serverName;
			confInfo.setdivision = _tDesc.setId;
	        ret = pPtr->ListConfigByInfo(confInfo,vf);
		}
        if (ret != 0)
        {
            sResult = "[fail] get remote file list";
            LOG->error() <<sResult<< endl;
            g_app.reportServer(_pServerObjectPtr->getServerId(),sResult);
        }
        for(unsigned i=0; i<vf.size();i++)
        {
            //�ű���ȡ  ��Ҫ���滻
            if(_pServerObjectPtr->isScriptFile(vf[i]) == true)
            {
                _pServerObjectPtr->getRemoteScript(vf[i]);
                continue;
            }

            //��taf���������ļ���Ҫnode��ȡ taf�������÷�������ʱ�Լ���ȡ
            if(_pServerObjectPtr->isTafServer() != true)
            {

                TafRemoteConfig tTafRemoteConfig;
                tTafRemoteConfig.setConfigInfo(Application::getCommunicator(),ServerConfig::Config,_tDesc.application, _tDesc.serverName, _sExePath,_tDesc.setId);
                tTafRemoteConfig.addConfig(vf[i],sResult);
                g_app.reportServer(_pServerObjectPtr->getServerId(),sResult);
            }
        }
   }
   catch (exception& e)
   {
        LOG->error()<<"CommandLoad::getRemoteConf " << e.what() << endl;
   }
}
#endif
