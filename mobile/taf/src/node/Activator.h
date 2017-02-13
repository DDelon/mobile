#ifndef __ACTIVATOR_H_
#define __ACTIVATOR_H_
#include "Node.h"
#include <unistd.h>
#include "util/tc_file.h"
#include "util/tc_monitor.h"
#include <iostream>

using namespace taf;
using namespace std;
////////////////////////////////////////////////////
// 

//������־�ű�����
/////////////////////////////////////////////////////////
// ��������
struct EnvironmentEval : std::unary_function<string, string>
{
    string
    operator()(const std::string& value)
    {
        string::size_type assignment = value.find("=");
        if(assignment == string::npos || assignment >= value.size() - 1)
        {
            return value;
        }
        string v = value.substr(assignment + 1);
        assert(v.size());
        string::size_type beg = 0;
        string::size_type end;

        //���滻
        while((beg = v.find("$", beg)) != string::npos && beg < v.size() - 1)
        {
            string variable;
            if(v[beg + 1] == '{')
            {
            end = v.find("}");
            if(end == string::npos)
            {
                break;
            }
            variable = v.substr(beg + 2, end - beg - 2);
            }
            else
            {
            end = beg + 1;
            while((isalnum(v[end]) || v[end] == '_')  && end < v.size())
            {
                ++end;
            }
            variable = v.substr(beg + 1, end - beg - 1);
            --end;
            }

            char* val = getenv(variable.c_str());
            string valstr = val ? string(val) : "";
            v.replace(beg, end - beg + 1, valstr);
            beg += valstr.size();
        }
		//�˴���Ҫ�������ã��������Ϻ��滻�л�ȡ�Ļ�������Ϊ��
		setenv((value.substr(0, assignment)).c_str(),v.c_str(),true);

        return value.substr(0, assignment) + "=" + v;
    }
};

class Activator : public TC_ThreadLock, public TC_HandleBase
{
public:
    /**
     * �������
     * iTimeInterval����������iMaxCount�������� �ﵽ�������������ʧ�ܺ�iPunishInterval����������һ��
     * @param iTimeInterval
     * @param iMaxCount 
     * @param iPunishInterval �ͷ�ʱ���� 
     *
     */
    Activator( int iTimeInterval,int iMaxCount,int iPunishInterval)
    :_bLimited(false)
    ,_iCurCount(0)
    ,_iMaxCount(iMaxCount)
    ,_iTimeInterval(iTimeInterval)
    ,_iPunishInterval(iPunishInterval)
    ,_bTermSignal(false)
    ,_sRedirectPath("")
    {
        _vActivatingRecord.clear();
    };

    ~Activator()
    {
    };
    
    /**
     * ��������
     * 
     * @param strExePath ��ִ���ļ�·��
     * @param strPwdPath ��ǰ·�� 
     * @param strRollLogPath ��־·�� 
     * @param vOptions �������� 
     * @param vEnvs �������� 
     * @return pid_t �����ӽ���id 
     *
     */
    pid_t activate(const string& strExePath, 
                   const string& strPwdPath,
                   const string &strRollLogPath, 
                   const vector<string>& vOptions, 
                   vector<string>& vEnvs);

    /**
     * �ű���������
     * 
     * @param sServerId ����id
     * @param strStartScript �ű�·��  
     * @param strMonitorScript �ű�·�� 
     * @param sResult ִ�н��  
     * @return pid_t �����ӽ���id 
     *
     */
    pid_t activate(const string &strServerId, 
                   const string& strStartScript,
                   const string &strMonitorScript,
                   string &strResult);

    /**
     * ֹͣ����
     * 
     * @param pid ����id
     * @return int 0 �ɹ�  ����ʧ��
     */
    int deactivate( int pid );

    /**
     * ֹͣ���� ������core�ļ�
     * 
     * @param pid ����id
     * @return int 0 �ɹ�  ����ʧ��
     */
    int deactivateAndGenerateCore( int pid );
    
    /**
     * �����ź�
     * 
     * @param pid   ����id 
     * @param signal �ź�
     * @return int 0 �ɹ�  ����ʧ��
     */
    int sendSignal( int pid, int signal ) const;
public:
    
    bool isActivatingLimited (); //��������,������ֹ������񲻶�����Ӱ����������

    void addActivatingRecord();

    //���нű�
    bool doScript(const string &sServerId,
                  const string &strScript,
                  string &strResult,
                  map<string,string> &mResult,
                  const string &sEndMark = "");

    map <string,string> parseScriptResult(const string &strServerId,const string &strResult);
	void setRedirectPath(const string& sRedirectpath) {_sRedirectPath = sRedirectpath;}

private:
    int pclose2(FILE *fp);
    FILE* popen2(const char *cmdstring, const char *type);
       
private:
    vector <time_t> _vActivatingRecord;   
    bool    _bLimited;   //�Ƿ���������
    int     _iCurCount;  //��ǰ��������
    int     _iMaxCount;  //�����������
    int     _iTimeInterval; //ʱ��
    int     _iPunishInterval; //�ͷ�����ʱ����
    
private:
    bool     _bTermSignal;      //��taf����ű����г�ʱֹͣ�ź�
    string _sRedirectPath;		     //��׼����ʹ�������ض���Ŀ¼
};

typedef TC_AutoPtr<Activator> ActivatorPtr;
#endif

