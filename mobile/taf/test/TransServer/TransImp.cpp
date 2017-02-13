#include "TransImp.h"
#include "TransServer.h"
#include "CCProtocol.h"

using namespace taf;

TransImp::TransImp()
: _ccserverPrx(NULL)
{ 
}

void TransImp::initialize()
{
    cout << "doInitialize......" << endl;

	_ccserverObjName = "CC.CCServer.Object";

	_ccserverObjHost = "tcp -h 172.25.38.142 -p 40195 -t 10000";

	//���÷���CCServer��Э�������
    ProxyProtocol prot;

    prot.requestFunc = ProxyProtocol::streamRequest;

    prot.responseFunc = ProxyProtocol::streamResponse<0, uint32_t, true, 9, uint32_t, true>;

	_ccserverPrx = Application::getCommunicator()->stringToProxy<ServantPrx>(_ccserverObjName + "@" + _ccserverObjHost);

    _ccserverPrx->taf_set_protocol(prot);
}

void TransImp::destroy()
{
}

/**
 * ��Ϊserver�ˣ�����ͻ��˵�����
 */
int TransImp::doRequest(taf::JceCurrentPtr current, vector<char>& response)
{
	//����CCClient��capЭ������
    const vector<char>& request = current->getRequestBuffer();

    string buf((const char*)(&request[0]), request.size());

    ResponsePacket rsp;

	//Э�����
    CCProtocol::packc pc;

    uint32_t len = buf.length();

    pc.Decode(buf.c_str(), len);

    LOG->debug() << "[Adapter:" << current->getBindAdapter()->getName() << "]->C2S Cmd:" << pc.Cmd << ",Seq:" <<  pc.SeqNo << ",Len:" << pc.Len << endl;


	/*(1)��Ϊ�ͻ��ˣ�ͬ������ccserver*/
    /*
    try
    {
        _ccserverPrx->rpc_call(pc.SeqNo, "Transfer", buf.c_str(), buf.length(), rsp);

        LOG->debug() << "[rcp_call,ret len:" << rsp.sBuffer.size() << "]" << endl;

        response = rsp.sBuffer;
    }
    catch (TafSyncCallTimeoutException& e)
    {
        //ͬ�����ó�ʱ
        //...
    }
    catch (TafException& e)
    {
        //�����쳣����
        //...
    }
    */
	/**/

	/*(2)��Ϊ�ͻ��ˣ��첽����ccserver*/
	ServantCallbackPtr cb = new ServantCallback("ServantCallback", this, current);

    try
    {
        _ccserverPrx->rpc_call_async(pc.SeqNo, "AysncTransfer", buf.c_str(), buf.length(), cb);
    }
    catch (TafException& e)
    {
        //���ص��쳣�������絽���������ȫ��ʧЧ�ȵ�
        //(��ʱ�������쳣��doResponseException����)
        //...
    }

	current->setResponse(false);
	/**/

    return 0;
}

/**
 * ��Ϊ�ͻ����첽����ʱ����������server(�ɹ�)����Ӧ
 */
int TransImp::doResponse(ReqMessagePtr resp)
{
	//(1)�����������server�������objname�ж����ĸ�server��resp
	//(2)�����һ��server�ж��callback���ͣ�����type�ж�����
	//...	
	if (resp->proxy->taf_name() == _ccserverObjName 
		&& resp->callback->getType() == "ServantCallback")
	{
		ServantCallback* cb = dynamic_cast<ServantCallback*>(resp->callback.get());

		vector<char>& buff = resp->response.sBuffer;

		if (!buff.empty())
		{
			CCProtocol::packs ps;

			uint32_t len = buff.size();

			ps.Decode((const char*)(&buff[0]), len);

			LOG->debug() << "[CBType:" << cb->getType() << ",FromObject:" << resp->proxy->taf_name() << "]<-S2C Cmd:" << ps.Cmd << ",Seq:" <<  ps.SeqNo << ",Len:" << ps.Len << endl;
			
            //������Ӧ��ccapi
			cb->getCurrent()->sendResponse((const char*)(&buff[0]), buff.size());
		}
	}
    return 0;
}

/**
 * ��Ϊ�ͻ����첽����ʱ����������server(�����쳣���߳�ʱ)����Ӧ
 */
int TransImp::doResponseException(ReqMessagePtr resp)
{
    //��ʱ����(ע��:��ʱֻ��ʾ�涨ʱ����û���յ���Ӧ������ȷ���Է��ķ����һ��û�д���
    if (resp->response.iRet == JCEASYNCCALLTIMEOUT)
    {
        if (resp->proxy->taf_name() == _ccserverObjName 
            && resp->callback->getType() == "ServantCallback")
        {
            //�ط���ҵ���߼�����...
        }
    }
    return 0;
}

/**
 * ��Ϊ�ͻ����첽����ʱ����������server(û���ҵ�request)����Ӧ
 * ����push����Ϣ���߶Զ�server�ٵ����첽��Ӧ
 */
int TransImp::doResponseNoRequest(ReqMessagePtr resp)
{
    //��ʱ�Զ�server�����ݰ���resp->response.buff��
    //...
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
