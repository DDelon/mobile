#ifndef __TAF_TRANSCEIVER_H_
#define __TAF_TRANSCEIVER_H_

#include "servant/EndpointInfo.h"
#include "servant/Message.h"
#include "servant/Global.h"
#include "servant/NetworkUtil.h"
#include <list>

using namespace std;

namespace taf
{
/**
 * ���紫����࣬��Ҫ�ṩsend/recv�ӿ�
 */
class Transceiver
{
public:
    /**
     * ����״̬
     */
    enum ConnectStatus
    {
        eUnconnected,
        eConnecting,
        eConnected,
	eBusConnecting,
    };

    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    Transceiver(ObjectProxy *op, const EndpointInfo &ep);

    /**
     * ��ʼ��
     * 
     * @param fd 
     * @param connected 
     */
    void reInitialize(int fd, ConnectStatus connStatus);

    /**
     * ��ʼ��FIFO��MMAP
     */
    virtual void init(){};

    /**
     *
     *��������
     */
    virtual ~Transceiver();

     /**
     * ��ȡ�ļ�������
     * @return int
     */
    virtual int fd() const;

	/**
	 * ��ȡ���ļ�������
	 */
	virtual int getrfd() const { return _fd; }

	/**
	 * ��ȡд�ļ�������
	 */
	virtual int getwfd() const { return _fd; }

    /**
     * �ر�����
     */
   //virtual void doClose();
	
    /**
     * �Ƿ���Ч
     * @return bool
     */
	bool isValid() const;

    /**
     * �˿���Ϣ
     * @return const EndpointInfo& 
     */
	EndpointInfo& getEndpointInfo() { return _ep;}

public:
    /**
     * ���������ж�Send BufferCache�Ƿ��������İ�
     * @return int
     */
	virtual int doRequest();

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done) = 0;

    /**
     * ���͵�server����Ϣ������
     *
     */
    void writeToSendBuffer(const string& msg);

    /**
     * �ر�����
     */
    void close();

public:
	/**
	 * �ж��Ƿ��Ѿ����ӵ������
	 */
	bool hasConnected() { return isValid() && (_connStatus == eConnected); }
	
	/**
	 * ���õ�ǰ����̬
	 */
	void setConnected() { _connectFailed = false; _connStatus = eConnected; }

    
    /**
     * ����Bus����
     * 
     * @return bool 
     */
    bool isConnecting() { return isValid() && (_connStatus == eConnecting); }

    /**
     * ���õ�ǰ����̬
     */
    void setBusConnecting() { _connectFailed = false; _connStatus = eBusConnecting; }

    /**
     * ��������
     * 
     * @return bool 
     */
    bool isBusConnecting() { return isValid() && (_connStatus == eBusConnecting); }

    /**
     * �Ƿ������쳣
     * 
     * @return bool 
     */
    bool isConnectFailed() { return _connectFailed; }

    /**
     * ��������ʧ��
     * 
     */
    void setConnectFailed() { _connectFailed = true; _connStatus = eUnconnected; }



public:

    /**
     * ���緢�ͽӿ�
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag) = 0;

    /**
     * ������սӿ�
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag) = 0;

protected:
    int _fd;

	EndpointInfo _ep;

    ObjectProxy *_objectProxy;

    string _sendBuffer;

    string _recvBuffer;

	ConnectStatus _connStatus;	    //��ʾ�Ƿ��Ѿ����ӵ������

    bool          _connectFailed;   //�Ƿ�������ʧ��
};

//////////////////////////////////////////////////////////
/**
 * TCP ����ʵ��
 */
class TcpTransceiver : public Transceiver
{
public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    TcpTransceiver(ObjectProxy *op, const EndpointInfo &ep);

    /**
     * TCP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * TCP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     *
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int, =1,��ʾ�����ݾͰ�
     */
	virtual int doResponse(list<ResponsePacket>& done);
};
//////////////////////////////////////////////////////////
/**
 * UDP ����ʵ��
 */
class UdpTransceiver : public Transceiver
{
public:
	enum
	{
        DEFAULT_RECV_BUFFERSIZE = 64*1024	   /*ȱʡ���ݽ���buffer�Ĵ�С*/
	};

public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    UdpTransceiver(ObjectProxy *op, const EndpointInfo &ep);

	~UdpTransceiver();
    /**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done);

private:
	char				*_pRecvBuffer;

};
//////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////
//	BusTransceiver
class BusTransceiver : public Transceiver
{
public:
    /**
     * ���캯��
     * @param ep
     * @param fd
     */
    BusTransceiver(ObjectProxy *op, EndpointInfo ep);

	/**
	 * ��������
	 */
	virtual ~BusTransceiver();

	/**
	 * �жϵ�ǰ�����Ƿ���Ч
	 * 
	 * @author kevintian (11-01-01)
	 * 
	 * @return bool 
	 */
	virtual bool isValid() const { return _s2cFifo.fd() >= 0 && _c2sFifo.fd() >= 0; }

	/**
	 * ���ع����ڴ�ͨѶ�����дFD
	 * 
	 * @author kevintian (11-01-04)
	 * 
	 * @return int 
	 */
	virtual int fd() const { return _c2sFifo.fd(); }

	/**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int send(const void* buf, uint32_t len, uint32_t flag);

    /**
     * UDP ����ʵ��
     * @param buf
     * @param len
     * @param flag
     * @return int
     */
    virtual int recv(void* buf, uint32_t len, uint32_t flag);

    /**
     * �ر�����
     */
    void close();

    /**
     * �����أ��ж�Recv BufferCache�Ƿ��������İ�
     * @param done
     * @return int
     */
	virtual int doResponse(list<ResponsePacket>& done);

	/**
	 * ��ȡ���ļ�������
	 */
	virtual int getrfd() const { return _s2cFifo.fd(); }

	/**
	 * ��ȡд�ļ�������
	 */
	virtual int getwfd() const { return _c2sFifo.fd(); }

	/**
	 * ���ÿͻ��˵�����˵���Ϣ
	 */
	void setc2s(const std::string & sFifoName, const std::string & sMmapName, const uint32_t iMmapSize)
	{
		_c2sFifoName = sFifoName;
		_c2sMmapName = sMmapName;
		_c2sMmapSize = iMmapSize;
	}

	/**
	 * ���÷���˵��ͻ��˵���Ϣ
	 */
	void sets2c(const std::string & sFifoName, const std::string & sMmapName, const uint32_t iMmapSize)
	{
		_s2cFifoName = sFifoName;
		_s2cMmapName = sMmapName;
		_s2cMmapSize = iMmapSize;
	}

	/**
	 * ��ʼ��FIFO��MMAP
	 */
	void init();

private:
	std::string 	_c2sFifoName;
	std::string 	_c2sMmapName;
	uint32_t		_c2sMmapSize;
	TC_Fifo 		_c2sFifo;
	TC_Mmap 		_c2sMmap;
	TC_SQueue		_c2sMemQueue;

	std::string		_s2cFifoName;
	std::string 	_s2cMmapName;
	uint32_t		_s2cMmapSize;
	TC_Fifo 		_s2cFifo;
	TC_Mmap 		_s2cMmap;
	TC_SQueue 	_s2cMemQueue;
};
}
#endif
