#ifndef __QUERY_IMP_H__
#define __QUERY_IMP_H__

#include "QueryF.h"
#include "DbHandle.h"
#include "RouterProxyCallback.h"

using namespace taf;

/**
 * �����ѯ�ӿ���
 */

class QueryImp: public QueryF
{
public:
    /**
     * ���캯��
     */
    QueryImp(){};

    /**
     * ��ʼ��
     */
    virtual void initialize();

    /**
     ** �˳�
     */
    virtual void destroy() {};


    /** ����id��ȡ����
     *
     * @param id ��������
     *
     * @return  �������иö���Ļendpoint�б�
     */
    virtual vector<EndpointF> findObjectById(const string & id, taf::JceCurrentPtr current);

	/**����id��ȡ���ж���,������ͷǻ����
	 *
     * @param id         ��������
     * @param activeEp   ���endpoint�б�
     * @param inactiveEp �Ǵ��endpoint�б�
     * @return:  0-�ɹ�  others-ʧ��
     */
    virtual taf::Int32 findObjectById4Any(const std::string & id,vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current);

    /** ����id��ȡ��������endpoint�б�
     *
     * @param id         ��������
     * @param activeEp   ���endpoint�б�
     * @param inactiveEp �Ǵ��endpoint�б�
     * @return:  0-�ɹ�  others-ʧ��
     */
    Int32 findObjectById4All(const std::string & id,
            vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current);

     /** ����id��ȡ����ͬ��endpoint�б�
     *
     * @param id         ��������
     * @param activeEp   ���endpoint�б�
     * @param inactiveEp �Ǵ��endpoint�б�
     * @return:  0-�ɹ�  others-ʧ��
     */
    Int32 findObjectByIdInSameGroup(const std::string & id,
            vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp,taf::JceCurrentPtr current);

	/** ����id��ȡ����ָ�������ص�endpoint�б�
	 *
	 * @param id         ��������
	 * @param activeEp   ���endpoint�б�
	 * @param inactiveEp �Ǵ��endpoint�б�
     * @return:  0-�ɹ�  others-ʧ��
     */
	Int32 findObjectByIdInSameStation(const std::string & id, const std::string & sStation, vector<taf::EndpointF> &activeEp, vector<taf::EndpointF> &inactiveEp, taf::JceCurrentPtr current);

	/** ����id��ȡ����ͬset endpoint�б�
	*
	* @param id         ��������
	* @param setId      setȫ��,��ʽΪsetname.setarea.setgroup
	* @param activeEp   ���endpoint�б�
	* @param inactiveEp �Ǵ��endpoint�б�
	* @return:  0-�ɹ�  others-ʧ��
	*/
	Int32 findObjectByIdInSameSet(const std::string & id,const std::string & setId,vector<taf::EndpointF> &activeEp,vector<taf::EndpointF> &inactiveEp, taf::JceCurrentPtr current);

private:
	/**
	 * �ж������id�Ƿ�Ϊtaserver�ķ���
	 */
	bool IsTaServerObjById(const std::string & id);
	/**
	* @param id        taserver��������
	* @param eFnId     ����Ľӿ�����ö�٣�������ο�FUNID
	* @param current   �����������
	* @return:  0-�ɹ�  others-ʧ��
	 */
	int findTaServerObjById(const std::string & id,const FUNID eFnId,const taf::JceCurrentPtr& current);

    void doDaylog(const string& id,const vector<taf::EndpointF> &activeEp, const vector<taf::EndpointF> &inactiveEp, const taf::JceCurrentPtr& current,const std::ostringstream& os,const string& sSetid="");
protected:
    //���ݿ����
    CDbHandle _db;

};

#endif
