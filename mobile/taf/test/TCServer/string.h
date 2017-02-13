#include "util/tc_common.h"
#include "util/tc_epoll_server.h"
#include "util/tc_parsepara.h"

#include <sys/un.h>
#include <iostream>

using namespace std;
using namespace taf;

/**
 * ������, ÿ�������߳�һ������
 */
class StringHandle : public TC_EpollServer::Handle
{
public:

    /**
     * ��ʼ��
     */
    virtual void initialize()
    {
        cout << "StringHandle::initialize:" << pthread_self() << endl;
    }

    /**
     * ����
     * @param in
     * @param out
     * 
     * @return int
     */
    static int parse(string &in, string &out)
    {
        string::size_type pos = in.find("\r\n");

        if(pos != string::npos)
        {
            out = in.substr(0, pos);
            in  = in.substr(pos+2);

            return TC_EpollServer::PACKET_FULL;   //����1��ʾ�յ��İ��Ѿ���ȫ
        }

        return TC_EpollServer::PACKET_LESS;		//����0��ʾ�յ��İ�����ȫ
//		return TC_EpollServer::PACKET_ERR;	    //����-1��ʾ�յ���Э����󣬿�ܻ��Զ��رյ�ǰ����
    }

    /**
     * ���崦���߼�
     * @param stRecvData
     */
    virtual void handle(const TC_EpollServer::tagRecvData &stRecvData)
    {
        try
        {
            taf::TC_Parsepara para(stRecvData.buffer);
            cout << TC_Common::tostr(para.toMap()) << endl;
            
            sendResponse(stRecvData.uid, para.tostr(), stRecvData.ip, stRecvData.port);
        }
        catch(exception &ex)
        {
            close(stRecvData.uid);
            cout << ex.what() << endl;
        }
    }

    /**
     * ���ر���
     * @param stRecvData
     */
    virtual void handleTimeout(const TC_EpollServer::tagRecvData &stRecvData)
    {
        cout << "StringHandle::handleTimeout:" << pthread_self() << endl;
        close(stRecvData.uid);
    }
};


