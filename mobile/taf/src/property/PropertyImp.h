#ifndef __PROPERTY_IMP_H_
#define __PROPERTY_IMP_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "log/taf_logger.h"
#include "servant/PropertyF.h"
#include "PropertyHashMap.h"

using namespace taf;

class PropertyImp : public PropertyF,public TC_ThreadLock
{
public:


    /**
     *
     */
    PropertyImp();

    /**
     * ��������
     */
    ~PropertyImp()  { }

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize();

    /**
     * �˳�
     */
    virtual void destroy() { }

    /**
    * �ϱ�������Ϣ
    * @param statmsg, �ϱ���Ϣ
    * @return int, ����0��ʾ�ɹ�
    */
    virtual int reportPropMsg(const map<StatPropMsgHead,StatPropMsgBody>& statmsg, taf::JceCurrentPtr current );

private:
    void dump2file();

};

#endif


