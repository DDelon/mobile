#ifndef __ADMIN_IMP_H__
#define __ADMIN_IMP_H__

#include "AdminReg.h"
#include "EndpointF.h"
#include "DbHandle.h"

using namespace taf;

/**
 *�������ƽӿ���
 */

class AdminRegImp: public AdminReg
{
public:
    /**
     * ���캯��
     */
    AdminRegImp(){};

    /**
     * ��ʼ��
     */
    virtual void initialize();

    /**
     ** �˳�
     */
    virtual void destroy() {};

public:

    /***********application****************/

    /**
     * ��ȡapplication�б�
     *
     * @param null
     * @param out result : �������
     *
     * @return application�б�
     */
    virtual vector<string> getAllApplicationNames(string &result, taf::JceCurrentPtr current);


    /***********node****************/

    /**
     * ��ȡnode�б�
     *
     * @param null
     * @param out result : �������
     *
     * @return node �б�
     */
    virtual vector<string> getAllNodeNames(string &result, taf::JceCurrentPtr current);

    /**
     * ��ȡnode�汾 
     * @param name   node����
     * @param version   node�汾 
     * @param out result �������
     * @return  0-�ɹ� others-ʧ��
     */
    virtual int getNodeVesion(const string &nodeName, string &version, string & result, taf::JceCurrentPtr current);

    /**
     * ping node
     *
     * @param name: node id
     * @param out result : �������
     *
     * @return : true-pingͨ��false-��ͨ
     */
    virtual bool pingNode(const string & name, string &result, taf::JceCurrentPtr current);

    /**
     * ֹͣ node
     *
     * @param name: node id
     * @param out result : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int shutdownNode(const string & name, string &result, taf::JceCurrentPtr current);

    /**
     * ��ȡserver�б�
     *
     * @param name: null
     * @param out result : �������
     *
     * @return: server�б������Ϣ
     */
    virtual vector<vector<string> > getAllServerIds(string &result, taf::JceCurrentPtr current);


    /**
     * ��ȡ�ض�server״̬
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeNmae   : node id
     * @param out state  : ״̬
     * @param out result : �������
     *
     * @return : ������
     */

    virtual int getServerState(const string & application, const string & serverName, const string & nodeName,
            ServerStateDesc &state, string &result, taf::JceCurrentPtr current);
    
    
     /**
     * ��ȡ�ض�ip����group
     *
     * @param sting: ip
     * @param out int  : group id
     * @param out result : �������
     *
     * @return : ������
     */

    virtual int getGroupId(const string & ip,int &groupId, string &result, taf::JceCurrentPtr current);



    /**
     * �����ض�server
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeName   : node id
     * @param out result : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int startServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * ֹͣ�ض�server
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeName   : node id
     * @param out result : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int stopServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * �����ض�server
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeName   : node id
     * @param out result : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int restartServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * ֪ͨ����
     * @param application
     * @param serverName
     * @param nodeName
     * @param command
     * @param result
     * @param current
     *
     * @return int
     */
    virtual int notifyServer(const string & application, const string & serverName, const string & nodeName,
            const string &command, string &result, taf::JceCurrentPtr current);

    /**
     * �����ض�server
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeName   : node id
     * @param shutdown   : �Ƿ���Ҫֹͣserver
     * @param version    : �����İ汾��ʶ
     * @param user       : ������
     * @param out result : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int patchServer(const string & application, const string & serverName, const string & nodeName,
            bool shutdown, const string & version, const string & user, string &result, taf::JceCurrentPtr current);

    /**
     * ��������
     *
     * @param PatchRequest : ��������
     * @param out result   : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int batchPatch(const taf::PatchRequest & req, string &result, taf::JceCurrentPtr current);

    /**
     * �����ض��ļ�
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeName   : node id
     * @param shutdown   : �Ƿ���Ҫֹͣserver
     * @param version    : �����İ汾��ʶ
     * @param user       : ������
     * @param out result : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int patchSubborn(const string & application, const string & serverName, const string & nodeName, 
                             const string & sSrcFile, const string & sDstFile, string &result, taf::JceCurrentPtr current);

    /**
    * ��ȡ���񷢲�����
    * @param application  ��������Ӧ����
    * @param serverName  ������ 
    * @param nodeName   :node id 
    * @out tPatchInfo  :�����ٷֱ�
    * @return :0-�ɹ� others-ʧ��
    */
    virtual int getPatchPercent(const string &application, const string &serverName,const string & nodeName,
            PatchInfo &tPatchInfo, taf::JceCurrentPtr current);

    /**
     * �����ض�server
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeName   : node id
     * @param out result : �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int loadServer(const string & application, const string & serverName, const string & nodeName,
            string &result, taf::JceCurrentPtr current);

    /**
     * ��ȡ��Ӧģ��
     *
     * @param profileName: ģ������
     * @param out profileTemplate: ģ������
     * @param out resultDesc: �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int getProfileTemplate(const std::string & profileName,std::string &profileTemplate,
            std::string & resultDesc, taf::JceCurrentPtr current);

    /**
     * ��ȡ�����Ӧģ��
     *
     * @param application: Ӧ��
     * @param serverName : server��
     * @param nodeName   : node id
     * @param out profileTemplate: ģ������
     * @param out resultDesc: �������
     *
     * @return : 0-�ɹ� others-ʧ��
     */
    virtual int getServerProfileTemplate(const string & application, const string & serverName, const string & nodeName,std::string &profileTemplate,
            std::string & resultDesc, taf::JceCurrentPtr current);

       
protected:

    //���ݿ���������
    CDbHandle _db;
};

#endif
