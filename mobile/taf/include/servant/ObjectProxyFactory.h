#ifndef __TAF_OBJECT_PROXY_FACTORY_H_
#define __TAF_OBJECT_PROXY_FACTORY_H_

#include "servant/Communicator.h"
#include "servant/ObjectProxy.h"

namespace taf
{
/**
 * ��ȡObjectProxy����
 * ÿ��objectname�ڽ��̿ռ�Ψһ��Ӧһ��objectproxy
 * object proxy �̰߳�ȫ
 *
 */
class ObjectProxyFactory : public TC_HandleBase, public TC_ThreadRecMutex
{
public:
    /**
     * ���캯��
     * @param cm
     */
    ObjectProxyFactory(Communicator* cm);

    /**
     * ��ȡ�������
     * @param name
     * @param bCanTunneled �Ƿ������������ 
     * @return ObjectPrx
     */
    ObjectPrx::element_type* getObjectProxy(const string& name, bool bCanTunneled = false);

    /**
     * ��ȡ���е�adapterproxy
     * 
     * @return map<string, ObjectPrx> 
     */
    map<string, ObjectPrx> getObjectProxys();

    /**
     *
     */
    ~ObjectProxyFactory();

    /**
     * ���ж���������locator��Ϣ
     */
    int loadObjectLocator();

private:
    /**
     * ͨ����
     */
    Communicator* _comm;

    /**
     * �����Ѵ�����objectproxy
     */
    map<string, ObjectPrx> _objectProxys;
};
/////////////////////////////////////////////////////
}
#endif
