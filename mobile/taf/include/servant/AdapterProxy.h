#ifndef __TAF_ADAPTER_PROXY_H_
#define __TAF_ADAPTER_PROXY_H_

#include "util/tc_timeout_queue.h"
#include "servant/EndpointInfo.h"
#include "servant/Connector.h"
#include "servant/ObjectProxy.h"
#include "servant/FDReactor.h"
#include "servant/Transceiver.h"
#include "servant/Global.h"

namespace taf
{
/**
 * ÿ��Adapter��Ӧһ��Endpoint��Ҳ����һ������˿�
 */
class AdapterProxy : public TC_ThreadMutex //public BaseProxy
{
public:
    /**
     * ���캯��
     * @param ep
     * @param op
     */
    AdapterProxy(Communicator* comm, const EndpointInfo &ep, ObjectProxy* op);

    /**
     * ��������
     */
    ~AdapterProxy();

public:
    /**
     * ����server�˶��󷽷�
     * @param req
     * @return int
     */
	int invoke(ReqMessagePtr& req);

    /**
     * server�˷�������
     * @param req
     * @return int
     */
    int finished(ResponsePacket &rsp);

    /**
     * �˿��Ƿ���Ч,������ȫ��ʧЧʱ����false
     * @bForceConnect : �Ƿ�ǿ�Ʒ�������,Ϊtrueʱ����״̬�����жϾͷ�������
     * @return bool
     */
    bool checkActive(bool bForceConnect = false);

	/**
	 * ������ס�յ�״̬�Ƿ�Ϊactive,�����ǳ����⵼�£�������Ϊֹͣ�����Ƿ������ߣ�����Ϊ��inactive��
	 * @return bool
	 */
	inline bool isActiveInReg() {return _activeStateInReg;}

	inline void setActiveInReg(bool bActive) {_activeStateInReg = bActive;}

	inline bool isConnTimeout(){ return _bConnTimeout; }

    /**
     * ��ȡ��ʱ����
     *
     * @return TC_TimeoutQueue<ReqMessagePtr>*
     */
    inline TC_TimeoutQueue<ReqMessagePtr>* getTimeoutQueue() { return _timeoutQueue; }

    /**
     * ��Ҫ�ر�����
     *
     * @param closeTrans
     */
    inline void setCloseTrans(bool closeTrans) { _closeTrans = closeTrans; }

    /**
     * �Ƿ���Ҫ�ر�����
     *
     * @return bool
     */
    inline bool shouldCloseTrans() { return _closeTrans; }

    /**
     * ���ӵ�����
     * @param bTimeout, �Ƿ��ǳ�ʱ��
     */
    void finishInvoke(bool bTimeout);

    /**
     * ��ȡ�˿���Ϣ
     * @return const EndpointInfo&
     */
    const EndpointInfo& endpoint();

    /**
     * �Ӷ�����ȡһ������
     * @param trans
     * @return bool
     */
	bool sendRequest();

    /**
     * ��ȡ����
     *
     * @return Transceiver*
     */
    inline Transceiver* trans() { return _trans; }

	/**
	 * �����Ҫ�첽���ӷ�����ô�ٴ��������
	 */
	bool finishConnect();

   /**
	 * ���ӹ����ڴ�ͨѶ���
	 */
   bool doBusReconnect();

   bool finishBusConnect();

   /**
    * �ж��Ƿ�ʹ�ù����ڴ�ͨ��
    */
   bool hasBusCommu() { return _bUseBus; }

protected:

    /**
     * ����û���ҵ�request��response(������push��Ϣ)
     * @param req
     * @return bool
     */
    bool dispatchPushCallback(ResponsePacket &rsp);

    /**
     * ���÷������Ч״̬
     * @param value
     */
    void setActive(bool value);

    /**
     * ����������Ϣ
     */
    void resetInvoke();

    /**
     * �Ӷ����л�ȡһ������
     * @param req
     * @return bool
     */
	bool popRequest(ReqMessagePtr& req);

    /**
     * ����
     * @return TransceiverPtr
     */
    Transceiver* doReconnect();

private:
    bool            _bUseBus;

    Communicator* _comm;

    EndpointInfo _endpoint;

    ObjectProxy* _objectProxy;

    FDReactor* _reactor;

    TC_TimeoutQueue<ReqMessagePtr>* _timeoutQueue;

    Transceiver*             _trans;

    bool                    _closeTrans;

    int32_t _checkTransInterval;

    time_t _lastCheckTransTime;

    int32_t _currentGridGroup;

    uint32_t _timeoutInvoke;

	uint32_t _bConnTimeout;

    uint32_t _totalInvoke;

    uint32_t _frequenceFailInvoke;

    time_t _lastFinishInvokeTime;

    time_t _lastRetryTime;

    bool _activeStatus;

	//�ýڵ������ص�״̬�Ƿ���active��,�����Ϻ���Ϊֹͣ������Ϊ��inactive��
	bool _activeStateInReg;
};
////////////////////////////////////////////////////////////////////
}
#endif
