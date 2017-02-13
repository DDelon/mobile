#ifndef __TAF_MESSAGE_H_
#define __TAF_MESSAGE_H_

#include "servant/Global.h"
#include "servant/EndpointInfo.h"
#include "util/tc_autoptr.h"
#include "util/tc_monitor.h"

namespace taf
{

/**
 * ��ʱһ�����ʺ�����л�
 * ���ó�ʱ������
 * ���㵽ĳ̨�������ĳ�ʱ��, ���������ʱ�������߳�ʱ����������ֵ
 * Ĭ��60s��, ��ʱ���ô���>=2, ��ʱ����0.5����������ʱ����>5,
 * ��ʧЧ
 * ����ʧЧ��, ���󽫾����ܵ��л����������ܵķ�����, ��ÿ��tryTimeInterval����һ��, ����ɹ�����Ϊ�ָ�
 * ���������������ʧЧ, �����ѡ��һ̨����
 * ����ǻҶ�״̬�ķ���, ����ʧЧ����������л�, Ҳֻ��ת������ͬ�Ҷ�״̬�ķ���
 * @uint16_t minTimeoutInvoke, �������С�ĳ�ʱ����, Ĭ��2��(��checkTimeoutIntervalʱ���ڳ�����minTimeoutInvoke, �ż��㳬ʱ)
 * @uint32_t frequenceFailInvoke, ����ʧ�ܴ���
 * @uint32_t checkTimeoutInterval, ͳ��ʱ����, (Ĭ��60s, ����С��30s)
 * @float radio, ��ʱ���� > ��ֵ����Ϊ��ʱ�� ( 0.1<=radio<=1.0 )
 * @uint32_t tryTimeInterval, ����ʱ����
 */
struct CheckTimeoutInfo
{
    uint16_t minTimeoutInvoke;

    uint32_t checkTimeoutInterval;

    uint32_t frequenceFailInvoke;

    float radio;

    uint32_t tryTimeInterval;

    CheckTimeoutInfo() : minTimeoutInvoke(2), checkTimeoutInterval(60), frequenceFailInvoke(5), radio(0.5), tryTimeInterval(30) {}
};

/**
 * ����ͬ������ʱ����������
 */
struct ReqMonitor : public TC_ThreadLock {};//, public TC_HandleBase {};

//typedef TC_AutoPtr<ReqMonitor> ReqMonitorPtr;

#define IS_MSG_TYPE(m, t) ((m & t) != 0)
#define SET_MSG_TYPE(m, t) do { (m |= t); } while (0);
#define CLR_MSG_TYPE(m, t) do { (m &=~t); } while (0);

/**
 * Proxy�˵�������Ϣ�ṹ
 */
struct ReqMessage : public TC_HandleBase//, public TC_ThreadPool::ThreadData
{
    /**
     * ��ʱ��ʱ�����(�첽���ó�ʱ)
     * 
     * @param ptr 
     */
    static void timeout(ReqMessagePtr& ptr);

    //��������
    enum CallType
    {
        SYNC_CALL = 1, //ͬ��
        ASYNC_CALL,    //�첽
        ONE_WAY,       //����
    };

    /**
     * ���캯��
     */
    ReqMessage(Communicator *comm);

    /**
     * ��������
     */
    ~ReqMessage();

public:
    /**
     * �ж���Ϣ����
     * 
     * @return bool
     */
    bool is_message_type(uint32_t msg, uint32_t type);
    /**
     * 
     * @param msg
     * @param type
     */
    void set_message_type(uint32_t& msg, uint32_t type);
    /**
     * 
     * @param msg
     * @param type
     */
    void clr_message_type(uint32_t& msg, uint32_t type);

protected:
    //���ܸ�ֵ
    ReqMessage& operator=(const ReqMessage &);

protected:
    Communicator*               comm;       //ͨ����

public:
    ReqMessage::CallType        type;       //��������
    ServantPrx::element_type*   proxy;      //���ö˵�proxy����
    ReqMonitor*                 monitor;    //����ͬ����monitor
    AdapterProxy*               adapter;    //��̨���������͵�
    RequestPacket               request;    //������Ϣ��
    ResponsePacket              response;   //��Ӧ��Ϣ��
    ServantProxyCallbackPtr     callback;   //�첽����ʱ�Ļص�����
    timeval                     begtime;    //����ʱ��
    timeval                     endtime;    //���ʱ��
    int64_t                     hashCode;   //hashֵ���û���֤һ���û��������͵�ͬһ��server(���ϸ�֤)
    bool                        fromRpc;    //�Ƿ��ǵ�����Э���rcp_call��ȱʡΪfalse

};
////////////////////////////////////////////////////////////////////////////////////////////////////
}
#include "servant/ServantProxy.h"

#endif
