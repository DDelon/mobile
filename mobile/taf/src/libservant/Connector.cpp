#include "servant/Connector.h"
#include "servant/Transceiver.h"
#include "servant/NetworkUtil.h"
#include "servant/Global.h"
#include "servant/EndpointInfo.h"

namespace taf
{

Transceiver* Connector::create(ObjectProxy *op, const EndpointInfo &ep)
{
	Transceiver* trans = NULL;

	if (ep.type() == EndpointInfo::UDP)
	{
		if (ep.port() != 0)
		{
			trans = new UdpTransceiver(op, ep);
		}
		else
		{
			trans = new UdpTransceiver(op, ep);
		}
	}
	else
	{
		if (ep.port() != 0)
		{
			trans = new TcpTransceiver(op, ep);
		}
		else
		{
			trans = new TcpTransceiver(op, ep);
		}
	}

	assert(trans != NULL);

	return trans;
}

void Connector::connect(Transceiver* trans)
{
	int fd = -1;

	const EndpointInfo &ep = trans->getEndpointInfo();

	if (ep.type() == EndpointInfo::UDP)
	{
		fd = NetworkUtil::createSocket(true);

        NetworkUtil::setBlock(fd, false);
        //��������qos��dscp��־
        if(0 != trans->getEndpointInfo().qos())
        {
            int iQos=trans->getEndpointInfo().qos();
            ::setsockopt(fd,SOL_IP,IP_TOS,&iQos,sizeof(iQos));
            //info("setSockOpt [" + ip + ":" + TC_Common::tostr(port) + "][" + TC_Common::tostr(cs.getfd()) + "] listen server qos is:"+TC_Common::tostr(iQos));
        }
		//����connect����ֹ���ܽ��մӷ�����Ŀ�Ļ����صİ�
//		NetworkUtil::doConnect(fd, ep.addr());

		//udp�˿�, Ĭ�Ͼ�������
		trans->reInitialize(fd, Transceiver::eConnected);
	}
	else
	{
		fd = NetworkUtil::createSocket(false);

		NetworkUtil::setBlock(fd, false);
        //��������qos��dscp��־
        if(0 != trans->getEndpointInfo().qos())
        {
            int iQos=trans->getEndpointInfo().qos();
            ::setsockopt(fd,SOL_IP,IP_TOS,&iQos,sizeof(iQos));
            //info("setSockOpt [" + ip + ":" + TC_Common::tostr(port) + "][" + TC_Common::tostr(cs.getfd()) + "] listen server qos is:"+TC_Common::tostr(iQos));
        }

		bool bConnected = NetworkUtil::doConnect(fd, ep.addr());

		trans->reInitialize(fd, bConnected?Transceiver::eConnected : Transceiver::eConnecting);
	}
}
////////////////////////////////////////////////////////
}
