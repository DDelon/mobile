#ifndef __NODE_IMP_H_
#define __NODE_IMP_H_
#include "Node.h"
#include <unistd.h>
#include "PlatformInfo.h"
#include "Activator.h"
#include "KeepAliveThread.h"
#include "patch/taf_patch.h"

using namespace taf;
using namespace std;

class NodeImp : public Node
{
public:
    /**
     *
     */
    NodeImp()
    {
    };

    /**
     * ���ٷ���
     * @param k
     * @param v
     *
     * @return
     */
    ~NodeImp()
    {
    };

    /**
    * ��ʼ��
    */
    virtual void initialize();

    /**
    * �˳�
    */
    virtual void destroy()
    {
    };

    /**
    * ����ָ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    virtual int destroyServer( const string& application, const string& serverName, string &result,JceCurrentPtr current );

    /**
    * patchָ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @param appDistrib  ��patch server �ϵ�appĿ¼
    * @param shutdown  �Ƿ�Ҫ�����״̬Ϊdisactive
    */
    virtual int patch(const string& application, const string& serverName,  bool shutdown,const string& patchServer, string &result,JceCurrentPtr current ) ;


    /**
    * ��ǿ�ķ����ӿ�
    * pushRequest ���뷢�����󵽶���
    * @param req  ��������
    * @return  int 0�ɹ� ����ʧ��
    */
    int patchPro(const taf::PatchRequest & req, string & result, JceCurrentPtr current);


    /**
    * patchָ���ļ�·��
    * @param srcFile        �ļ�����·����patch�������ϵ�Ŀ¼
    * @param dstFile        �ļ�����·����node�������ϴ�ŵ�Ŀ¼
    * @param patchServer    patch������
    * @return  int 0�ɹ� ����ʧ��
    */
    virtual int patchSubborn(const string & application, const string & serverName, const string & srcFile, const string & dstFile, const string & patchServer, string & result, JceCurrentPtr current);

    /**
    * ����ָ���ļ�
    * @param out result  ʧ��˵��
    * @return  int 0�ɹ�  ��0ʧ��
    */
    virtual int addFile(const string &application,const string &serverName,const string &file, string &result, JceCurrentPtr current);

    /**
    * ��ȡnode����
    * @return  string
    */
    virtual string getName( JceCurrentPtr current ) ;

    /**
    * ��ȡnode�ϸ���
    * @return  LoadInfo
    */
    virtual taf::LoadInfo getLoad( JceCurrentPtr current ) ;

    /**
    * �ر�node
    * @return  int
    */
    virtual int shutdown( string &result, JceCurrentPtr current );


    /**
    * �ر�nodes�����з���
    * @return  int
    */

    virtual int stopAllServers( string &result,JceCurrentPtr current );

    /**
    * ����ָ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    virtual int loadServer( const string& application, const string& serverName, string &result, JceCurrentPtr current );

    /**
    * ����ָ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    virtual int startServer( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * ָֹͣ������
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    virtual int stopServer( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
     * ֪ͨ����
     * @param application
     * @param serverName
     * @param result
     * @param current
     *
     * @return int
     */
    virtual int notifyServer( const string& application, const string& serverName, const string &command, string &result, JceCurrentPtr current );

    /**
    *  ��ȡָ������pid���̺�
    * @param application  ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    int getServerPid( const string& application, const string& serverName, string &result, JceCurrentPtr current);

    /**
    *  ��ȡָ������registry���õ�״̬
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    virtual ServerState getSettingState( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * ��ȡָ������״̬
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  ServerState
    */
    virtual ServerState getState( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * ͬ��ָ������״̬
    * @param application    ��������Ӧ����
    * @param serverName  ������
    * @return  int
    */
    virtual int synState( const string& application, const string& serverName, string &result, JceCurrentPtr current ) ;

    /**
    * �����������
    * @param application  ��������Ӧ����
    * @param serverName  ������
    * @out tPatchInfo  ������Ϣ
    * @return  int
    */
    virtual int getPatchPercent( const string& application, const string& serverName, PatchInfo &tPatchInfo, JceCurrentPtr current);

    virtual taf::Int32 delCache(const std::string & sFullCacheName, const std::string &sBackupPath, const std::string & sKey, std::string &result,JceCurrentPtr current);

private:

    string          _sDownLoadPath; //�ļ�����Ŀ¼
    NodeInfo        _tNodeInfo;     //node��Ϣ
    PlatformInfo    _tPlatformInfo; //ƽ̨��Ϣ
    RegistryPrx     _pRegistryPrx;  //���ش���

};

typedef TC_AutoPtr<NodeImp> NodeImpPtr;

#endif

