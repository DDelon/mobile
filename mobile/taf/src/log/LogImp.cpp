#include "LogImp.h"
#include "log/taf_logger.h"

GlobeInfo   g_globe;

TC_DayLogger &GlobeInfo::makeDayLogger(const string &app, const string &server, const string &logname, const string &format,const string& ip)
{
	string sLogPath = _log_path +  "/" + app + "/" + server + "/" + logname;
	LOG->debug() << "makeDayLogger|sLogPath=" <<sLogPath<<"|"<<format<< endl;
    TC_DayLogger *p = new TC_DayLogger();
    p->init(sLogPath, format);
    p->setupThread(&_group);
    //���б�ʶ����Ҫ
    p->modFlag(0xffff, false);
    _loggers[logname][ip] = p;
    _formats[logname][ip] = format;
    return (*p);
}

TC_DayLogger &GlobeInfo::makeDayLogger(const LogInfo & info, const string &logname, const string &format,const string& ip)
{
	string sLogPath = getRealLogName(info);

	LOG->debug() << "makeDayLoggerEx|sLogPath=" <<sLogPath<<"|"<<format<< endl;
	TC_DayLogger *p = new TC_DayLogger();

	p->init(sLogPath, format,info.bHasSufix,info.sConcatStr);
	p->setSeparator(info.sSepar);
	p->enableSqareWrapper(info.bHasSquareBracket);
	p->setupThread(&_group);
	//���б�ʶ����Ҫ
	p->modFlag(0xffff, false);
	_loggers[logname][ip] = p;
	_formats[logname][ip] = format;
	return (*p);

}
string GlobeInfo::getRealLogName(const LogInfo & info)
{
	string sRealLogname;
	if(info.bHasAppNamePrefix)
	{
		sRealLogname = info.sFilename.empty() ? (info.appname +"." + info.servername) : \
			(info.appname +"." + info.servername + info.sConcatStr + info.sFilename);
	}
	else
	{
		sRealLogname = info.sFilename;
	}

	string setDivision = getSetGoodFormat(info.setdivision);

	string sLogPath = _log_path +  "/" + info.appname + "/" + setDivision  + "/" + info.servername + "/" + sRealLogname;

	return sLogPath;
}

string GlobeInfo::getLogName(const LogInfo & info)
{
	string sLogname = TC_Common::tostr<bool>(info.bHasSufix) + info.sConcatStr + TC_Common::tostr<bool>(info.bHasAppNamePrefix) + info.sConcatStr +\
						TC_Common::tostr<bool>(info.bHasSquareBracket) + info.sConcatStr + info.sSepar + info.sConcatStr +\
						info.setdivision + info.sConcatStr + info.appname + info.sConcatStr + info.servername + info.sConcatStr + info.sFilename;

	return sLogname;
}

bool GlobeInfo::HasSameFormat(const string& logname,const string& format,string& ip)
{
	map<string,string>::iterator  itFormat = _formats[logname].begin();
	bool bHasFormat = false;		 //�Ƿ��д�������format
	for(; itFormat != _formats[logname].end();itFormat++)
	{
		if(itFormat->second == format)	  //�ҵ���������format
		{
			bHasFormat = true;
			ip = itFormat->first; //Ϊ��������ͬformat�Ľڵ�ip
			break;
		}
	}
	return bHasFormat;
}

bool GlobeInfo::IsLoggerAttached(const string& logname,const string& sExcludeIp,const TC_DayLogger* pLogger)
{
	map<string,TC_DayLogger*>::iterator  itLogger = _loggers[logname].begin();
	bool bIsAttached = false;		 //�Ƿ���ip���ø�loggerʵ��
	for(; itLogger != _loggers[logname].end();itLogger++)
	{
		if(itLogger->first != sExcludeIp && itLogger->second == pLogger)
		{
			bIsAttached = true;
			LOG->debug() << "IsLoggerAttached|" << itLogger->first<< endl;
			break;
		}
	}
	return bIsAttached;
}

TC_DayLogger& GlobeInfo::getLogger(const LogInfo & infoOrg,const string& ip)
{
    //�滻app����
	LogInfo info = infoOrg;
	map<string,string>::const_iterator it = _mAppIdcRelation.find(info.appname);
	if(it != _mAppIdcRelation.end() )
	{
		info.appname  =  it->second;
	}
    
	string format = info.sFormat;
	if(binary_search(_vHourlist.begin(),_vHourlist.end(),(info.appname + "." + info.servername)) != false)
	{
		format = "%Y%m%d%H";
	}

	string setDivision = getSetGoodFormat(info.setdivision);

	TC_File::makeDirRecursive(_log_path + "/"  + info.appname + "/" + setDivision + "/" + info.servername);

	string logname = getLogName(info);

	//LOG->debug() <<"getLogger=" <<logname<<"|"<<format<< endl;

	Lock lock(*this);
	map<string, map<string,TC_DayLogger*> >::iterator itLogName = _loggers.find(logname);
	if( itLogName == _loggers.end())	 //û�д�������log
	{
		return makeDayLogger(info, logname, format,ip);
	}

	map<string,TC_DayLogger*>::iterator  itIp = itLogName->second.find(ip);
	if(itIp == itLogName->second.end())   //û�д�������ip�ڵ�
	{
		string sSameIp;
		if(HasSameFormat(logname,format,sSameIp))		  //�д�������format,���µĽڵ�ip���ֵ�ͬ��
		{
			_loggers[logname][ip] = _loggers[logname][sSameIp];//ʹ�þ�����ͬformat��loggerʵ��
			_formats[logname][ip] = format;
			return (*(_loggers[logname][ip]));
		}
		else  //û�д�������format,�½���һ��logname�µ�format��
		{
			return makeDayLogger(info, logname, format,ip);
		}

	}
	else  //��logname�µ����ip�ڵ㴴����
	{
		if(_formats[logname][ip] != format)  //��ip�ڵ����־��ʽ�仯��
		{
			string sSameIp;
			TC_DayLogger* pOldLogger = _loggers[logname][ip];
			if(HasSameFormat(logname,format,sSameIp))//�Ѿ��иø�ʽ������ͬ��
			{
				_loggers[logname][ip] = _loggers[logname][sSameIp];
				_formats[logname][ip] = format;

				if(pOldLogger) //ɾ���ɵ�loger
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return (*(_loggers[logname][ip]));
			}
			else
			{
				_loggers[logname][ip] = NULL;
				_formats[logname][ip] = "";
				if(!IsLoggerAttached(logname,ip,pOldLogger))//�����ж���ڵ��������loger��������Ҫ�ж�
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return makeDayLogger(info, logname, format,ip);
			}
		}
	}
	//û�иı��ʽ
	return (*(_loggers[logname][ip]));

}

TC_DayLogger& GlobeInfo::getLogger(const string &appOrg, const string &server, const string &file, const string &sformat,const string& ip)
{

    //�滻app����
	string app = appOrg;
	map<string,string>::const_iterator it = _mAppIdcRelation.find(app);
	if(it != _mAppIdcRelation.end() )
	{
		app  =  it->second;
	}
	
	TC_File::makeDirRecursive(_log_path + "/"  + app + "/" + server);

	string format = sformat;
	if(binary_search(_vHourlist.begin(),_vHourlist.end(),(app + "." + server)) != false)
	{
		format = "%Y%m%d%H";
	}


	string logname = file.empty() ? (app + "." + server) : (app + "." + server + "_" + file);
	//LOG->debug() <<"getLogger=" <<logname<<"|"<<format<< endl;

	Lock lock(*this);
	map<string, map<string,TC_DayLogger*> >::iterator itLogName = _loggers.find(logname);
	if( itLogName == _loggers.end())     //û�д�������log
	{
		return makeDayLogger(app, server, logname, format,ip);
	}

	map<string,TC_DayLogger*>::iterator  itIp = itLogName->second.find(ip);
	if(itIp == itLogName->second.end())   //û�д�������ip�ڵ�
	{
		string sSameIp;
		if(HasSameFormat(logname,format,sSameIp))         //�д�������format,���µĽڵ�ip���ֵ�ͬ��
		{
			_loggers[logname][ip] = _loggers[logname][sSameIp];//ʹ�þ�����ͬformat��loggerʵ��
			_formats[logname][ip] = format;
			return (*(_loggers[logname][ip]));
		}
		else  //û�д�������format,�½���һ��logname�µ�format��
		{
			return makeDayLogger(app, server, logname, format,ip);
		}

	}
	else  //��logname�µ����ip�ڵ㴴����
	{
		if(_formats[logname][ip] != format)  //��ip�ڵ����־��ʽ�仯��
		{
			string sSameIp;
			TC_DayLogger* pOldLogger = _loggers[logname][ip];
			if(HasSameFormat(logname,format,sSameIp))//�Ѿ��иø�ʽ������ͬ��
			{
				_loggers[logname][ip] = _loggers[logname][sSameIp];
				_formats[logname][ip] = format;

				if(pOldLogger) //ɾ���ɵ�loger
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return (*(_loggers[logname][ip]));
			}
			else
			{
				_loggers[logname][ip] = NULL;
				_formats[logname][ip] = "";
				if(!IsLoggerAttached(logname,ip,pOldLogger))//�����ж���ڵ��������loger��������Ҫ�ж�
				{
					pOldLogger->unSetupThread();
					delete pOldLogger;
				}
				return makeDayLogger(app, server, logname, format,ip);
			}
		}
	}
	//û�иı��ʽ
	return (*(_loggers[logname][ip]));
}
string GlobeInfo::getSetGoodFormat(const string& sSetDivision)
{
	vector<string> vtSetDivisions = TC_Common::sepstr<string>(sSetDivision,".");
	if(vtSetDivisions.size() != 3)
	{
		return "";
	}
	else
	{
		if(vtSetDivisions[2] == "*")
		{
			return string(vtSetDivisions[0] + vtSetDivisions[1]);
		}
		else
		{
			return string(vtSetDivisions[0] + vtSetDivisions[1] + vtSetDivisions[2]);
		}
	}
}

//////////////////////////////////////////
//

void LogImp::initialize()
{

}



void LogImp::logger(const string &app, const string &server, const string &file, const string &format, const vector<string> &buffer, taf::JceCurrentPtr current)
{

    TC_DayLogger &dl = g_globe.getLogger(app, server, file, format,current->getIp());
    //��¼��־
    for(size_t i = 0; i < buffer.size(); i++)
    {
        dl.any() << current->getIp() << "|" << buffer[i];
    }
}

void LogImp::loggerbyInfo(const LogInfo & info,const vector<std::string> & buffer,taf::JceCurrentPtr current)
{
    TC_DayLogger &dl = g_globe.getLogger(info,current->getIp());

    //��¼��־
    for(size_t i = 0; i < buffer.size(); i++)
    {
        dl.any() << current->getIp() << info.sSepar << buffer[i];
    }
}
