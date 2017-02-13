#ifndef __REGISTRY_IMP_H__
#define __REGISTRY_IMP_H__

#include "util/tc_common.h"
#include "util/tc_config.h"

#include "Registry.h"
#include "DbHandle.h"

using namespace taf;


/*
 * �ṩ��node���õĽӿ���
 */
class RegistryImp: public Registry
{
public:
    /**
     * ���캯��
     */
    RegistryImp(){};

    /**
     * ��ʼ��
     */
    virtual void initialize();

    /**
     ** �˳�
     */
    virtual void destroy() {};

    /**
     * ��ȡ����
     * @param k
     * @param v
     *
     * @return int
     */
    int get(int &i, taf::JceCurrentPtr current);


    /**
     * node������ʱ����registryע��һ��session
     *
     * @param name node����
     * @param ni   node��ϸ��Ϣ
     * @param li    node����������Ϣ
     *
     * @return ע���Ƿ�ɹ�
     */
    virtual int registerNode(const string & name, const NodeInfo & ni, const LoadInfo & li, taf::JceCurrentPtr current);


    /**
     * node�ϱ���������
     *
     * @param name node����
     * @param li    node����������Ϣ
     *
     * @return ��������״̬
     */
    virtual int keepAlive(const string& name, const LoadInfo & li, taf::JceCurrentPtr current);


    /**
     * ��ȡ�ڸ�node�����server�б�
     *
     * @param name    node����
     *
     * @return server�����б�
     */
    virtual vector<ServerDescriptor> getServers(const string & nodeName, const string & app, const string & serverName, taf::JceCurrentPtr current);

    /**
     * ����server״̬
     *
     * @param nodeName : node id
     * @param app:       Ӧ��
     * @param serverName: server ��
     * @param state :  server״̬
     *
     * @return server��Ϣ�б�
     */
    virtual int updateServer(const string & nodeName, const string & app, const string & serverName,
            const taf::ServerStateInfo & stateInfo, taf::JceCurrentPtr current);

    /**
     * ��������server״̬
     *
     * @param vecStateInfo : ����server״̬
     *
     * @return server��Ϣ�б�
     */
	virtual int updateServerBatch(const std::vector<taf::ServerStateInfo> & vecStateInfo, taf::JceCurrentPtr current);

    /**
     * nodeֹͣ���ͷ�node�ĻỰ
     *
     * @param name    node����
     */
    virtual int destroy(const string & name, taf::JceCurrentPtr current);


    /**
     * �ϱ�server��taf��汾
     * @param app:       Ӧ��
     * @param serverName: server ��
     * @param nodeName : node id
     * @param version:  server���ڵ�taf�汾��
     *
     * @return 0-�ɹ� others-ʧ��
     */
    virtual int reportVersion(const string & app, const string & serverName, const string & nodeName,
            const string & version, taf::JceCurrentPtr current);

    
    /**
     * ��ȡnode��ģ������
     * @param nodeName: node����
     * @param out profileTemplate:  ��Ӧģ������
     *
     * @return 0-�ɹ� others-ʧ��
     */
    virtual taf::Int32 getNodeTemplate(const std::string & nodeName,std::string &profileTemplate,taf::JceCurrentPtr current);


    /**
     * nodeͨ���ӿڻ�ȡ���������ص�node ip
     * @param sNodeIp:  node ��ip
     *
     * @return 0-�ɹ� others-ʧ��
     */
    virtual taf::Int32 getClientIp(std::string &sClientIp,taf::JceCurrentPtr current);

    
    /**
     * ����������ɺ�UPDATE�汾�źͷ����� 
     * @param PatchResult: ������� 
     *  
     * @return 0-�ɹ� othres-ʧ�� 
     */
    virtual taf::Int32 updatePatchResult(const PatchResult & result, taf::JceCurrentPtr current);

protected:
    //���ݿ����
    CDbHandle _db;

};


#endif
