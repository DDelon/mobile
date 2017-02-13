#ifndef _TRANS_IMP_H_
#define _TRANS_IMP_H_

#include "servant/ServantProxy.h"
#include "servant/Servant.h"
#include <sys/syscall.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace taf;

/////////////////////////////////////////////////////////////////////////
class TransImp : public Servant
{

public:
    /**
     * ���캯��
     */
    TransImp();

public:
    /**
     * �����ʼ��
     */
    virtual void initialize();

    /**
     * ��Ϊ����ˣ�����ͻ��˵���������
     * @param current 
     * @param response 
     * @return int 
     */
    virtual int doRequest(taf::JceCurrentPtr current, vector<char>& response);

    /**
     * ��Ϊ�ͻ����첽����ʱ����������server(�ɹ�)����Ӧ
     * @param resp 
     * @return int 
     */
    virtual int doResponse(ReqMessagePtr resp);

    /**
     * ��Ϊ�ͻ����첽����ʱ����������server(�����쳣���߳�ʱ)����Ӧ
     * @param resp 
     * @return int 
     */
    virtual int doResponseException(ReqMessagePtr resp);

    /**
     * ��Ϊ�ͻ����첽����ʱ����������server(û���ҵ�request)����Ӧ
     * ����push����Ϣ���߶Զ�server�ٵ����첽��Ӧ
     * @param resp 
     * @return int 
     */
    virtual int doResponseNoRequest(ReqMessagePtr resp);

    /**
     * ��������
     */
	virtual void destroy();

protected:
    /**
     * ccserver�Ķ�������
     */
    string _ccserverObjName;

	/**
	 * ccserver�Ķ����ַ
	 */
    string _ccserverObjHost;

	/**
	 * ����ccserver�Ĵ���
	 */
	ServantPrx _ccserverPrx;
};
///////////////////////////////////////////////////////////////////////////////
#endif
