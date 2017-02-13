#ifndef __PROPERTY_IMP_H_
#define __PROPERTY_IMP_H_

#include "util/tc_common.h"
#include "util/tc_thread.h"
#include "util/tc_option.h"
#include "log/taf_logger.h"
#include "servant/PropertyF.h"
#include "PropertyDimenHashMap.h"

using namespace taf;

class PropertyDimenImp : public ReportPropertyF,public TC_ThreadLock
{
public:
 

    /**
     *
     */
    PropertyDimenImp()   { }

    /**
     * ��������
     */
    ~PropertyDimenImp()  { }

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize() { _tTime = 0 ;}

    /**
     * �˳�
     */
    virtual void destroy() { }

    /**
    * �ϱ�������Ϣ
    * @param statmsg, �ϱ���Ϣ
    * @return int, ����0��ʾ�ɹ�
    */
    virtual int reportPropMsg(const map<ReportPropMsgHead,ReportPropMsgBody>& statmsg, taf::JceCurrentPtr current );

private:
    void dump2file();
       
private:
     time_t     _tTime; 
     string     _sDate;
     string     _sFlag; 
};

#endif


