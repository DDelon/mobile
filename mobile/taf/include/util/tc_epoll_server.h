#ifndef __TC_EPOLL_SERVER_H_
#define __TC_EPOLL_SERVER_H_

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include "util/tc_epoller.h"
#include "util/tc_thread.h"
#include "util/tc_clientsocket.h"
#include "util/tc_logger.h"
#include "util/tc_shm.h"
#include "util/tc_common.h"
#include "util/tc_mem_queue.h"
#include "util/tc_squeue.h"
#include "util/tc_mmap.h"
#include "util/tc_fifo.h"

namespace taf
{
/////////////////////////////////////////////////
/**
 * @file  tc_epoll_server.h
 * @brief  EpollServer��
 *
 */
/////////////////////////////////////////////////
/**
 * Server����
 * ע��Э�������
 * ע���߼�������
 * ע�����˿ڴ�����
 */
class TC_EpollServer : public TC_ThreadLock, public TC_HandleBase
{
public:
    //////////////////////////////////////////////////////////////
    class BindAdapter;
    typedef TC_AutoPtr<BindAdapter> BindAdapterPtr;

    class Handle;
    typedef TC_AutoPtr<Handle> HandlePtr;

    class HandleGroup;
    typedef TC_AutoPtr<HandleGroup> HandleGroupPtr;

    /**
     * ����Э������ӿڵĲ�������
     * ע��������̰߳�ȫ�Ļ��ǿ��������
     */
    typedef TC_Functor<int, TL::TLMaker<string &, string&>::Result> protocol_functor;
    typedef TC_Functor<int, TL::TLMaker<int, string&>::Result>      header_filter_functor;
    //////////////////////////////////////////////////////////////
    /**�������ݶ����еĽṹ*/

    struct tagRecvData: public TC_HandleBase
    {
        uint32_t        uid;            /**���ӱ�ʾ*/
        string          buffer;         /**��Ҫ���͵�����*/
        string          ip;             /**Զ�����ӵ�ip*/
        uint16_t        port;           /**Զ�����ӵĶ˿�*/
        time_t          recvTimeStamp;  /**���յ����ݵ�ʱ��*/
        bool            isOverload;     /**�Ƿ��ѹ��� */
        bool            isClosed;       /**�Ƿ��ѹر�*/
        BindAdapterPtr  adapter;        /**��ʶ��һ��adapter����Ϣ*/
    };

    //typedef TC_AutoPtr<tagRecvData> tagRecvDataPtr;

    struct tagSendData: public TC_HandleBase
    {
        char            cmd;            /**����:'c',�ر�fd; 's',��������Ҫ����*/
        uint32_t        uid;            /**���ӱ�ʾ*/
        string          buffer;         /**��Ҫ���͵�����*/
        string          ip;             /**Զ�����ӵ�ip*/
        uint16_t        port;           /**Զ�����ӵĶ˿�*/
    };

    //typedef TC_AutoPtr<tagSendData> tagSendDataPtr;

    typedef TC_ThreadQueue<tagRecvData*, deque<tagRecvData*> > recv_queue;
    typedef TC_ThreadQueue<tagSendData*, deque<tagSendData*> > send_queue;
    typedef recv_queue::queue_type recv_queue_type;


    class Adapter_ShmQueue : public TC_ThreadLock
    {
    public:
        Adapter_ShmQueue();

        bool initialize();

        void push_back(const recv_queue_type & qdata);

        bool pop_front(tagRecvData* & data, size_t millsecond = 0);

    public:
        void setShmKey(const key_t ShmKey) { _shmKey = ShmKey; }

        void setShmSize(const size_t ShmSize) { _shmSize = ShmSize; }

        key_t getShmKey() { return _shmKey; }

        size_t getShmSize() { return _shmSize; }

        size_t getQueueSize();
    private:
        bool _push_back(const tagRecvData* const& data);

        bool _pop_front(tagRecvData* & data);

    private:
        struct shmHeader
        {
            uint32_t        uid;            /**���ӱ�ʾ*/
            char            ip[16];         /**Զ�����ӵ�ip*/
            uint16_t        port;           /**Զ�����ӵĶ˿�*/
            time_t          recvTimeStamp;  /**���յ����ݵ�ʱ��*/
            bool            isOverload;     /**�Ƿ��ѹ���*/
            bool            isClosed;       /**�Ƿ��ѹر�*/
        };

        key_t                   _shmKey;

        size_t                  _shmSize;

        TC_Shm                  _shm;
        TC_MemQueue             _shmQueue;
        list<tagRecvData*>    _memQueue;
    };


    //////////////////////////////////////////////////////////////
    /**
     * ����״̬
     */
    struct ConnStatus
    {
        string          ip;
        int32_t         uid;
        uint16_t        port;
        int             timeout;
        int             iLastRefreshTime;
    };
    ///////////////////////////////////////////////////////////////
    /**
     * ��name��handle���飬
     * ÿ��handle����һ������Adapter��Ϣ
     * ÿ��handle����һ���߳�
     */
    struct HandleGroup : public TC_HandleBase
    {
        string                      name;
        TC_ThreadLock               monitor;
        vector<HandlePtr>           handles;
        map<string, BindAdapterPtr> adapters;
    };
    ///////////////////////////////////////////////////////////////
	/**
	 * @brief ��������߼�����Ľӿ�
	 *
	   */
    /**
     * ������߼��������
     */
    class Handle : public TC_Thread, public TC_ThreadLock, public TC_HandleBase
    {
    public:
        /**
         * ����, Ĭ��û������, �ȴ�10s
         */
        Handle();

        /**
         * ��������
         */
        virtual ~Handle();

        /**
         * ���÷���
         * @param pEpollServer
         */
        void setEpollServer(TC_EpollServer *pEpollServer);

        /**
         * ��ȡ����
         * @return TC_EpollServer*
         */
        TC_EpollServer* getEpollServer();

        /**
         * ����������Group
         * @param pHandleGroup
         */
        void setHandleGroup(HandleGroupPtr& pHandleGroup);

        /**
         * ��ȡ����Group
         * @return HandleGroup*
         */
        HandleGroupPtr& getHandleGroup();

        /**
         * �̴߳�����
         */
        virtual void run();

    public:
        /**
         * ��������
         * @param stRecvData
         * @param sSendBuffer
         */
        void sendResponse(unsigned int uid, const string &sSendBuffer, const string &ip, int port);

        /**
         * �ر�����
         * @param stRecvData
         */
        void close(unsigned int uid);

        /**
         * ���õȴ�ʱ��
         * @param iWaitTime
         */
        void setWaitTime(uint32_t iWaitTime);

        /**
         * �����ʼ��
         */
        virtual void initialize() {};

        /**
         * ����HandleGroup�е�handle�߳�
         */
        virtual void notifyFilter();

    protected:
        /**
         * ����Ĵ����߼�
         */
        virtual void handleImp();

        /**
         * ������
         * @param stRecvData: ���յ�������
         */
        virtual void handle(const tagRecvData &stRecvData) = 0;

        /**
         * ����ʱ����, �������ڶ����е�ʱ���Ѿ�����
         * Ĭ��ֱ�ӹر�����
         * @param stRecvData: ���յ�������
         */
        virtual void handleTimeout(const tagRecvData &stRecvData);

        /**
         * �������ӹر�֪ͨ������
         * 1.close by peer
         * 2.recv/send fail
         * 3.close by timeout or overload
         * @param stRecvData:
         */
        virtual void handleClose(const tagRecvData &stRecvData);

        /**
         * ����overload���� �����ݶ����г����Ѿ���������ֵ
         * Ĭ��ֱ�ӹر�����
         * @param stRecvData: ���յ�������
         */
        virtual void handleOverload(const tagRecvData &stRecvData);

        /**
         * ����Filter����Ϣ
         */
        //virtual void handleFilter() {};
        /**
         * �����첽�ص�����
         */
        virtual void handleAsyncResponse() {}
       	/**
	     * handleFilter��ֵĵڶ����֣������û���������
	     * ����Ϸ�߼��ɺ���bExpectIdle����
         */
        virtual void handleCustomMessage(bool bExpectIdle = false) {}

        /**
         * ����(ÿ������һ��������ߵȴ�����ʱ�������һ��)
         */
        virtual void heartbeat() {}

        /**
         * �߳��Ѿ�����, ������崦��ǰ����
         */
        virtual void startHandle() {}

        /**
         * �߳�����Ҫ�˳�ʱ����
         */
        virtual void stopHandle() {}

        /**
         * �Ƿ����е�Adpater���ж�Ϊ��
         * @return bool
         */
        virtual bool allAdapterIsEmpty();

        /**
         * �Ƿ����е�servant��û��resp��Ϣ������
         * @return bool
         */
        virtual bool allFilterIsEmpty();

        /**
         * ��Ԫ��
         */
        friend class BindAdapter;
    protected:
        /**
         * ����
         */
        TC_EpollServer  *_pEpollServer;

        /**
         * ����handle��
         */
        HandleGroupPtr _handleGroup;

        /**
         * �ȴ�ʱ��
         */
        uint32_t  _iWaitTime;

    };
    //////////////////////////////////////////////////////////////////////////////////
    // ����˿ڹ���,����socket��Ϣ
    class BindAdapter : public TC_ThreadLock, public TC_HandleBase
    {
    public:
        /**
         * ȱʡ��һЩ����
         */
        enum
        {
            DEFAULT_QUEUE_CAP       = 10*1024,    /**����*/
            MIN_QUEUE_TIMEOUT       = 3*1000,     /**������С��ʱʱ��(ms)*/
            DEFAULT_MAX_CONN        = 1024,       /**ȱʡ���������*/
            DEFAULT_QUEUE_TIMEOUT   = 60*1000,    /**ȱʡ�Ķ��г�ʱʱ��(ms)*/
        };
        /**
         * ˳��
         */
        enum EOrder
        {
            ALLOW_DENY,
            DENY_ALLOW
        };

        /**
         * �����ڴ�ͨѶ����Ķ���
         *
         * @author kevintian (10-12-29)
         */
        struct BusCommu
        {
            std::string CommuKey;
            std::string c2sFifoName;
            std::string c2sMmapName;
            uint32_t    c2sMmapSize;
            std::string s2cFifoName;
            std::string s2cMmapName;
            uint32_t    s2cMmapSize;

            TC_SQueue c2sMemQueue;
            TC_Mmap     c2sMmap;
            TC_Fifo     c2sFifo;

            TC_SQueue s2cMemQueue;
            TC_Mmap     s2cMmap;
            TC_Fifo     s2cFifo;
        };
        /**
         * ���캯��
         * @param ppPtr, Э�������
         */
        BindAdapter(TC_EpollServer *pEpollServer);

        /**
         * ��������
         */
        ~BindAdapter();

        /**
         * ����adapter name
         * @param name
         */
        void setName(const string &name);

        /**
         * ��ȡadapter name
         * @return string
         */
        string getName() const;

        /**
         * ��ȡqueue capacity
         * @return int
         */
        int getQueueCapacity() const;

        /**
         * ����queue capacity
         * @param n
         */
        void setQueueCapacity(int n);

        /**
         * ����Э������
         * @param name
         */
        void setProtocolName(const string& name);

        /**
         * ��ȡЭ������
         * @return const string&
         */
        const string& getProtocolName();

        /**
         * �Ƿ�tafЭ��
         * @return bool
         */
        bool isTafProtocol();

        /**
         * �ж��Ƿ���Ҫ���ر���
         * @return bool
         */
        bool isOverload();

        /**
         * ������Ϣ�ڶ����еĳ�ʱʱ��, tΪ����
         * (��ʱʱ�侫��ֻ����s)
         * @param t
         */
        void setQueueTimeout(int t);

        /**
         * ��ȡ��Ϣ�ڶ����еĳ�ʱʱ��, ����
         * @return int
         */
        int getQueueTimeout() const;

        /**
         * ����endpoint
         * @param str
         */
        void setEndpoint(const string &str);

        /**
         * ��ȡip
         * @return const string&
         */
        TC_Endpoint getEndpoint() const;

        /**
         * ����socket
         * @return TC_Socket
         */
        TC_Socket &getSocket();

        /**
         * �������������
         * @param iMaxConns
         */
        void setMaxConns(int iMaxConns);

        /**
         * ��ȡ���������
         * @return size_t
         */
        size_t getMaxConns() const;

        /**
         * ����HeartBeatʱ��
         * @param n
         */
        void setHeartBeatTime(time_t t);

        /**
         * ��ȡHeartBeatʱ��
         * @return size_t
         */
        time_t getHeartBeatTime() const;

        /**
         * ����allow deny����
         * @param eOrder
         */
        void setOrder(EOrder eOrder);

        /**
         * ��������ip
         * @param vtAllow
         */
        void setAllow(const vector<string> &vtAllow);

        /**
         * ���ý�ֹip
         * @param vtDeny
         */
        void setDeny(const vector<string> &vtDeny);

        /**
         * ��ȡ����ip
         * @return vector<string>: ip�б�
         */
        vector<string> getAllow() const;

         /**
         * ��ȡ��ֹip
         * @return vector<string>: ip�б�
         */
        vector<string> getDeny() const;

         /**
         * ��ȡallow deny����
         * @return EOrder
         */
        EOrder getOrder() const;

        /**
         * �Ƿ�Ip������
         * @param ip
         * @return bool
         */
        bool isIpAllow(const string& ip) const;

        /**
         * �Ƿ񳬹������������
         * @return bool
         */
        bool isLimitMaxConnection() const;

        /**
         * ���ٵ�ǰ������
         */
        void decreaseNowConnection();

        /**
         * ���ٵ�ǰ������
         */
        void increaseNowConnection();

        /**
         * ��ȡ��������״̬
         * @return ConnStatus
         */
        vector<ConnStatus> getConnStatus();

        /**
         * ��ȡ��ǰ������
         * @return size_t
         */
        size_t getNowConnection() const;

        /**
         * ��ȡEpollServer
         * @return TC_EpollServer*
         */
        TC_EpollServer* getEpollServer();

        /**
         * ע��Э�������
         * @param pp
         */
        void setProtocol(const protocol_functor& pf, int iHeaderLen = 0, const header_filter_functor& hf = echo_header_filter);

        /**
         * ��ȡЭ�������
         * @return protocol_functor&
         */
        protocol_functor &getProtocol();

        /**
         * ������ͷ�������
         * @return protocol_functor&
         */
        header_filter_functor &getHeaderFilterFunctor();

        /**
         * �������ݵ�������
         * @param vtRecvData
         * @param bPushBack ��˲���
         * @param sBuffer
         */
        void insertRecvQueue(const recv_queue::queue_type &vtRecvData,bool bPushBack = true);

        /**
         * ֪ͨ�ȴ��ڽ��ն���������߳��ѹ���
         */
        void notifyRecvQueue();

        /**
         * �ȴ�����
         * @return bool
         */
        bool waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime);

        /**
         * ���ն��еĴ�С
         * @return size_t
         */
        size_t getRecvBufferSize();

        /**
         * Ĭ�ϵ�Э�������, ֱ��echo
         * @param r
         * @param o
         * @return int
         */
        static int echo_protocol(string &r, string &o);

        /**
         * Ĭ�ϵİ�ͷ����
         * @param i
         * @param o
         * @return int
         */
        static int echo_header_filter(int i, string &o);

        /**
         * ��ȡ��Ҫ���˵İ�ͷ����
         */
        int getHeaderFilterLen();

        /**
         * ����������handle����
         * @param handleGroupName
         */
        void setHandleGroupName(const string& handleGroupName);

        /**
         * ���������handle����
         * @return string
         */
        string getHandleGroupName() const;

        /**
         * ���������handle
         * @return HandleGroupPtr
         */

        HandleGroupPtr getHandleGroup() const
        {
            return _handleGroup;
        }

        /**
         * ����ServantHandle��Ŀ
         * @param n
         */
        void setHandleNum(int n);

        /**
         * ����handle���handle��(ÿ��handleһ������)
         * @return int
         */
        int getHandleNum();

        /**
         * ������Adapter��ͬһ��Group
         * @param otherAdapter
         */
        void setHandle(BindAdapterPtr& otherAdapter)
        {
            _pEpollServer->setHandleGroup(otherAdapter->getHandleGroupName(), this);
        }

        /**
         * ��ʼ�������߳�,�߳̽�������
         */
        template<typename T> void setHandle()
        {
            _pEpollServer->setHandleGroup<T>(_handleGroupName, _iHandleNum, this);
        }

        /**
         * ���ð󶨵�ַ�����ڴ���Ϣ
         *
         * @author kevintian (2010-9-2)
         *
         * @param iKey
         * @param iSize
         */
        void setShm(int iKey, int iSize);

        /**
         * ��ȡ���õĹ����ڴ��ʶ��
         *
         * @author kevintian (2010-9-2)
         *
         * @return key_t
         */
        key_t getShmKey() { return _shmQueue.getShmKey(); }

        /**
         * ��ȡ���õĹ����ڴ��С
         *
         * @author kevintian (2010-9-2)
         *
         * @return size_t
         */
        size_t getShmSize() { return _shmQueue.getShmSize(); }

        /**
         * ��ʼ�����������ڳ�ʼ�������ڴ��
         *
         * @author kevintian (2010-9-13)
         */
        void initialize();
        /**
         * ���ù����ڴ�ͨѶ�������
         */
        void setBusCommuEnable(bool bEnable) { _bUseBusCommu = bEnable; }

        /**
         * ���ù����ڴ�ͨѶ�����С
         */
        void setBusCommuSize(const size_t size) { _sizeBusCommu = size; }

        /**
         * ���ù����ڴ�ͨѶ���·��
         */
        void setBusCommuPath(const string & sPath) { _sBusCommuPath = sPath; }

        /**
         * ���´򿪹����ڴ�ͨѶ���
         */
        void rstBusConnection(BusCommu * pcommu);

        /**
         * ��ʼ�������ڴ�ͨѶ���
         */
        void initBusCommu(BusCommu * pcommu);

        /**
         * ��֤��ǰ�󶨽ӿ��ϵĹ����ڴ�ͨѶ���
         */
        BusCommu * connectBusCommu(const string & sIdName, const string & sCommuKey);

		BusCommu * connectBusCommu(const string & sCommuKey);

    protected:
        friend class TC_EpollServer;
        /**
         * ����
         */
        TC_EpollServer  *_pEpollServer;

        /**
         * Adapter���õ�HandleGroup
         */
        HandleGroupPtr  _handleGroup;

        /**
         * Э�����
         */
        protocol_functor _pf;

        /**
         * �׸����ݰ���ͷ����
         */
        header_filter_functor _hf;

        /**
         * adapter������
         */
        string          _name;

        /**
         * handle��������
         */
        string      _handleGroupName;

        /**
         * ����fd
         */
        TC_Socket       _s;

        /**
         * �󶨵�IP
         */
        TC_Endpoint     _ep;

        /**
         * ���������
         */
        int             _iMaxConns;

        /**
         * ��ǰ������
         */
        TC_Atomic      _iCurConns;

        /**
         * Handle����
         */
        size_t          _iHandleNum;

        /**
         * �����Order
         */
        volatile EOrder _eOrder;

        /**
         * �����ip
         */
        vector<string>  _vtAllow;

        /**
         * ��ֹ��ip
         */
        vector<string>  _vtDeny;

        /**
         * ���յ����ݶ���
         */
        recv_queue      _rbuffer;

        /**
         * �����������
         */
        int             _iQueueCapacity;

        /**
         * ��Ϣ��ʱʱ�䣨������е������м��)(���룩
         */
        int             _iQueueTimeout;

        /**
         * �׸����ݰ���ͷ����
         */
        int             _iHeaderLen;

        /**
         * �ϴ���������ʱ��
         */
        volatile time_t          _iHeartBeatTime;

        /**
         * Э������,ȱʡΪ"taf"
         */
        string          _protocolName;

        /**
         * �Ƿ�ʹ�ù����ڴ����
         */
        bool                _bUseShm;

        /**
         * �����ڴ����
         */
        Adapter_ShmQueue    _shmQueue;
    private:
        bool                    _bUseBusCommu;  //�Ƿ�ʹ�ù����ڴ�ͨѶ���

        size_t                  _sizeBusCommu;  //�����ڴ�ͨѶ����Ĵ�С

        string                  _sBusCommuPath; //�����ڴ�ͨѶ���������Ŀ¼

        map<string, BusCommu *> _mapBusCommu;   //�����ڴ�ͨѶ���ӳ���ϵ��
    };
    ////////////////////////////////////////////////////////////////////////////////////////////
    // �������ӹ���
    /**
     *  �������ӵ�socket��Ϣ
     */
    class Connection
    {
    public:
        enum EnumConnectionType
        {
            EM_TCP = 0,
            EM_UDP = 1,
            EM_BUS = 2
        };

        /**
         * ���캯��
         * @param lfd
         * @param s
         * @param ip
         * @param port
         */
        Connection(BindAdapter *pBindAdapter, int lfd, int timeout, int fd, const string& ip, uint16_t port);

        /**
         * udp����
         * @param fd
         */
        Connection(BindAdapter *pBindAdapter, int fd);
        /**
         * ͨѶ�����ʼ��
         */
        Connection(BindAdapter *pBindAdapter);

	    /**
         * ��������
         */
       virtual ~Connection();

        /**
         * ����������adapter
         */
        BindAdapter* getBindAdapter()       { return _pBindAdapter; }

        /**
         * ��ʼ��
         * @param id, ���ӵ�Ψһid
         */
        void init(unsigned int uid)         { _uid = uid; }

        /**
         * ��ȡ���ӳ�ʱʱ��
         *
         * @return int
         */
        int getTimeout() const              { return _timeout; }

        /**
         * ��ȡ�̵߳�Ωһid
         *
         * @return unsigned int
         */
        uint32_t getId() const              { return _uid; }

        /**
         * ��ȡ����fd
         *
         * @return int
         */
        int getListenfd() const             { return _lfd; }

        /**
         * ��ǰ����fd
         *
         * @return int
         */
        virtual int getfd() const                   { return _sock.getfd(); }

        /**
         * �Ƿ���Ч
         *
         * @return bool
         */
        bool isValid() const                { return _sock.isValid();}

        /**
         * Զ��IP
         *
         * @return string
         */
        string getIp() const                { return _ip; }

        /**
         * Զ�̶˿�
         *
         * @return uint16_t
         */
        uint16_t getPort() const            { return _port; }

        /**
         * �����׸����ݰ���ͷ��Ҫ���˵��ֽ���
         */
        void setHeaderFilterLen(int iHeaderLen)     { _iHeaderLen = iHeaderLen; }

        /**
         * ���ùر�,�����굱ǰ���ݾ͹ر�����
         */
        bool setClose()                             { _bClose = true; return _sendbuffer.empty(); }

        /**
         * ��ȡ��������
         */
        EnumConnectionType getType() const          { return _enType; }

		bool IsEmptyConn() const  {return _bEmptyConn;}

    protected:
        /**
         * �ر�����
         * @param fd
         */
        void close();

        /**
         * ��ӷ���buffer
         * @param buffer
         * @return int, -1:���ͳ���, 0:������, 1:�������, 2:��������
         */
        virtual int send(const string& buffer, const string &ip, uint16_t port);

        /**
         * ��������
         *
         * @return int
         */
         virtual int send();

        /**
         * ��ȡ����
         * @param fd
         * @return int, -1:���ճ���, 0:���ղ�ȫ, 1:���յ�һ��������
         */
        virtual int recv(recv_queue::queue_type &o);

        /**
         * ����Э��
         * @param o
         * @return int: <0:Э�����, 0:û��һ�������İ�, 1:�յ�����һ����
         */
        int parseProtocol(recv_queue::queue_type &o);

        /**
         * �������ݵ�������
         * @param vtRecvData
         */

        void insertRecvQueue(recv_queue::queue_type &vRecvData);
		/**
		 * ����udp��ʽ�����ӣ�����ָ����С�Ľ��ջ�����
		 *@param nSize
		 */
		bool setRecvBuffer(size_t nSize=DEFAULT_RECV_BUFFERSIZE);

        friend class TC_EpollServer;

    public:
        /**
         * ���ˢ��ʱ��
         */
        time_t              _iLastRefreshTime;

    protected:

        /**
         * ������
         */
        BindAdapter         *_pBindAdapter;

        /**
         * TC_Socket
         */
        TC_Socket           _sock;

        /**
         * ���ӵ�Ψһ���
         */
        uint32_t            _uid;

        /**
         * ������socket
         */
        int                 _lfd;

        /**
         * ��ʱʱ��
         */
        int                 _timeout;

        /**
         * ip
         */
        string              _ip;

        /**
         * �˿�
         */
        uint16_t             _port;

        /**
         * ��������buffer
         */
        string              _recvbuffer;

        /**
         * ��������buffer
         */
        string              _sendbuffer;

		size_t				_sendPos;

        /**
         * ��Ҫ���˵�ͷ���ֽ���
         */
        int                 _iHeaderLen;

        /**
         * �����굱ǰ���ݾ͹ر�����
         */
        bool                _bClose;

        /**
        * ��ʱ���е���󳤶�
        */
        int                 _iMaxTemQueueSize;
        /**
         * ��������
         */
        EnumConnectionType  _enType;

		bool				_bEmptyConn;

		/*
		 *�������ݵ���ʱbuffer,�����Ŀ���Ƕ�udp�������ݰ���С��������
		 */
		char				*_pRecvBuffer;

		size_t				_nRecvBufferSize;
    };

    /**
     * �����ڴ�ͨѶ�����Connection
     */
    class BusConnection : public Connection
    {
    public:
        BusConnection(BindAdapter * pBindAdapter, BindAdapter::BusCommu * pcommu);

        virtual ~BusConnection();

        /**
         * ��������
         */
        virtual int send(const string& buffer, const string &ip, uint16_t port);

        /**
         * �ӹ����ڴ�ͨѶ����������
         */
        virtual int recv(recv_queue::queue_type & o);

        /**
         * ��ȡ��ǰ����FD
         */
        virtual int getfd() const { return _pcommu->c2sFifo.fd(); }

        /**
         * ��õ�ǰ���Ӷ�Ӧ�Ĺ����ڴ�ͨѶ����ṹ��
         */
        BindAdapter::BusCommu * getBusCommu() { return _pcommu; }

    private:
        BindAdapter::BusCommu * _pcommu;
    };


    /**
     * ����ʱ�������map
     */
    class ConnectionList : public TC_ThreadLock
    {
    public:

        /**
         * ���캯��
         */
        ConnectionList(TC_EpollServer *pEpollServer);

        /**
         * ��������
         */
        ~ConnectionList() { if(_vConn) { delete[] _vConn; } }

        /**
         * ��ʼ����С
         * @param size
         */
        void init(uint32_t size);

        /**
         * ��ȡΩһID
         *
         * @return unsigned int
         */
        uint32_t getUniqId();

        /**
         * �������
         * @param cPtr
         * @param iTimeOutStamp
         */
        void add(Connection *cPtr, time_t iTimeOutStamp);

        /**
         * ˢ��ʱ����
         * @param uid
         * @param iTimeOutStamp, ��ʱʱ���
         */
        void refresh(uint32_t uid, time_t iTimeOutStamp);

        /**
         * ��鳬ʱ����
         */
        void checkTimeout(time_t iCurTime);

        /**
         * ��ȡĳ�������˿ڵ�����
         * @param lfd
         * @return vector<TC_EpollServer::ConnStatus>
         */
        vector<TC_EpollServer::ConnStatus> getConnStatus(int lfd);

        /**
         * ��ȡĳһ������
         * @param p
         * @return T
         */
        Connection* get(uint32_t uid);

        /**
         * ɾ������
         * @param uid
         */
        void del(uint32_t uid);

        /**
         * ��С
         * @return size_t
         */
        size_t size();

    protected:
        typedef pair<Connection*, multimap<time_t, uint32_t>::iterator> list_data;

        /**
         * �ڲ�ɾ��, ������
         * @param uid
         */
        void _del(uint32_t uid);

    protected:
        /**
         * ����
         */
        TC_EpollServer                  *_pEpollServer;

        /**
         * �ܼ�������
         */
        uint32_t                        _total;

        /**
         * ��������
         */
        list<uint32_t>                  _free;

        /**
         * ������Ԫ�ظ���
         */
        size_t                          _free_size;

        /**
         * ����
         */
        list_data                       *_vConn;

        /**
         * ��ʱ����
         */
        multimap<time_t, uint32_t>      _tl;

        /**
         * �ϴμ�鳬ʱʱ��
         */
        time_t                          _lastTimeoutTime;


        /**
         * ����ID��ħ��
         */
        uint32_t                        _iConnectionMagic;
    };
public:
    /**
     * ���캯��
     */
    TC_EpollServer();

    /**
     * ��������
     */
    virtual ~TC_EpollServer();

    /**
     * ����Э������ķ���ֵ
     */
    enum
    {
        PACKET_LESS = 0,    //��ʾ�յ��İ���ȫ
        PACKET_FULL = 1,    //��ʾ�Ѿ��յ������İ�
        PACKET_ERR  = -1,   //��ʾЭ�����
    };

    /**
     * �󶨼���socket
     * @param ls
     */
    int bind(BindAdapterPtr &lsPtr);

    /**
     * �󶨵�һ���Ѿ��е�handle����
     * @param groupName
     * @param handleNum
     * @param adapter
     */
    void setHandleGroup(const string& groupName, BindAdapterPtr adapter)
    {
        map<string, HandleGroupPtr>::iterator it = _handleGroups.find(groupName);

        if (it != _handleGroups.end())
        {
            it->second->adapters[adapter->getName()] = adapter;
            adapter->_handleGroup = it->second;
        }
    }

    /**
     * ����һ��handle�����飬����Ѿ�������ֱ�ӷ���
     * @param name
     * @return HandlePtr
     */
    template<class T> void setHandleGroup(const string& groupName, int32_t handleNum, BindAdapterPtr adapter)
    {
        map<string, HandleGroupPtr>::iterator it = _handleGroups.find(groupName);

        if (it == _handleGroups.end())
        {
            HandleGroupPtr hg = new HandleGroup();

            hg->name = groupName;

            adapter->_handleGroup = hg;

            for (int32_t i = 0; i < handleNum; ++i)
            {
                HandlePtr handle = new T();

                handle->setEpollServer(this);

                handle->setHandleGroup(hg);

                hg->handles.push_back(handle);
            }

            _handleGroups[groupName] = hg;

            it = _handleGroups.find(groupName);
        }
        it->second->adapters[adapter->getName()] = adapter;

        adapter->_handleGroup = it->second;
    }

    /**
     * ���������߳�
     */
    void startHandle();

	/**
	 * ����epoll
	 */
	void createEpoll();

    /**
     * ����
     */
    void waitForShutdown();

    /**
     * ֹͣ����
     */
    void terminate();

    /**
     * �Ƿ���������
     *
     * @return bool
     */
    bool isTerminate() const    { return _bTerminate; }

    /**
     * ��ȡEpoller����
     * @return TC_Epoller*
     */
    TC_Epoller* getEpoller()    { return &_epoller; }

    /**
     * ��ȡ����socket��Ϣ
     *
     * @return map<int,ListenSocket>
     */
    map<int, BindAdapterPtr> getListenSocketInfo();

    /**
     * �������ƻ�ȡBindAdapter
     * @param sName
     * @return BindAdapterPtr
     */
    BindAdapterPtr getBindAdapter(const string &sName);

    /**
     * �ر�����
     * @param uid
     */
    void close(unsigned int uid);

    /**
     * ��������
     * @param uid
     * @param s
     */
    void send(unsigned int uid, const string &s, const string &ip, uint16_t port);

    /**
     * ��ȡĳһ�����˿ڵ�������
     * @param lfd
     *
     * @return vector<TC_EpollServer::ConnStatus>
     */
    vector<TC_EpollServer::ConnStatus> getConnStatus(int lfd);

    /**
     * ��ȡ������
     *
     * @return size_t
     */
    size_t getConnectionCount()     { return _list.size(); }

    /**
     * ���ñ�����־
     * @param plocalLogger
     */
    void setLocalLogger(RollWrapperInterface *pLocalLogger)       { _pLocalLogger = pLocalLogger; }

    /**
     * ��¼��־
     * @param s
     */
    void debug(const string &s);

    /**
     * INFO��־
     * @param s
     */
    void info(const string &s);

    /**
     * ��¼������־
     * @param s
     */
    void error(const string &s);

	/**
	 * �Ƿ����÷�ֹ�����ӹ����Ļ���
	 * @param bEnable
	 */
	void EnAntiEmptyConnAttack(bool bEnable);

	/**
	 *���ÿ����ӳ�ʱʱ��
	 */
	void setEmptyConnTimeout(int timeout);

	/**
	 *����udp�Ľ��ջ�������С����λ��B,��СֵΪ8192�����ֵΪDEFAULT_RECV_BUFFERSIZE
	 */
	void setUdpRecvBufferSize(size_t nSize=DEFAULT_RECV_BUFFERSIZE);

protected:
    /**
     * ��ȡ����
     * @param id
     *
     * @return ConnectionPtr
     */
    Connection *getConnectionPtr(uint32_t uid)      { return _list.get(uid); }

    /**
     * ���tcp����
     * @param cPtr
     * @param iIndex
     */
    void addTcpConnection(Connection *cPtr);

    /**
     * ���udp����
     * @param cPtr
     * @param index
     */
    void addUdpConnection(Connection *cPtr);
    /**
     * ��ӹ����ڴ�ͨѶ�������
     *
     * @author kevintian (10-12-29)
     *
     * @param cPtr
     */
    void addBusConnection(Connection *cPtr);
    /**
     * ɾ������
     * @param cPtr
     */
    void delConnection(Connection *cPtr, bool bEraseList = true);

    /**
     * ��������
     * @param cPtr
     * @param buffer
     */
    int sendBuffer(Connection *cPtr, const string &buffer, const string &ip, uint16_t port);

    /**
     * ��������
     * @param cPtr
     * @return int
     */
    int sendBuffer(Connection *cPtr);

    /**
     * ����buffer
     * @param cPtr
     * @param buffer
     * @return int
     */
    int recvBuffer(Connection *cPtr, recv_queue::queue_type &v);

    /**
     * ����ܵ���Ϣ
     */
    void processPipe();

    /**
     * ������������
     */
    void processNet(const epoll_event &ev);

    /**
     * ֹͣ�߳�
     */
    void stopThread();

    /**
     * �����ӽ���
     * @param fd
     */
    bool accept(int fd);

    /**
     * �󶨶˿�
     * @param ep
     * @param s
     */
    void bind(const TC_Endpoint &ep, TC_Socket &s);

	/**
	 * �����ӳ�ʱʱ��
	 */
	int getEmptyConnTimeout() const;

	/**
	 *�Ƿ�����Ӽ��
	 */
	 bool IsEmptyConnCheck() const;

    //�����¼�����
    enum
    {
        ET_LISTEN = 1,
        ET_CLOSE  = 2,
        ET_NOTIFY = 3,
        ET_NET    = 0,
    };
	enum
	{
		MIN_EMPTY_CONN_TIMEOUT  = 2*1000,  /*�����ӳ�ʱʱ��(ms)*/
        DEFAULT_RECV_BUFFERSIZE = 64*1024	   /*ȱʡ���ݽ���buffer�Ĵ�С*/
	};
    friend class BindAdapter;
    friend class ConnectionList;

protected:
    /**
     * ����handle����
     */
    map<string, HandleGroupPtr> _handleGroups;

    /**
     * ����socket
     */
    map<int, BindAdapterPtr>    _listeners;

    /**
     * epoll
     */
    TC_Epoller                  _epoller;

    /**
     * ֹͣ
     */
    bool                        _bTerminate;

    /**
     * handle�Ƿ��Ѿ�����
     */
    bool                        _handleStarted;

	/**
	* epoll�Ƿ��Ѿ�����
	*/
	bool						_createEpoll;

    /**
     * �ܵ�(���ڹرշ���)
     */
    TC_Socket                   _shutdown;

    //�ܵ�(����֪ͨ��������Ҫ���;�)
    TC_Socket                   _notify;

    /**
     * �������������
     */
    ConnectionList              _list;

    /**
     * ���Ͷ���
     */
    send_queue                  _sbuffer;

    /**
     * ����ѭ����־
     */
    RollWrapperInterface          *_pLocalLogger;

	/**
	 *�����Ӽ����ƿ���
	 */
	bool 					_bEmptyConnAttackCheck;

    /**
     * �����ӳ�ʱʱ��,��λ�Ǻ���,Ĭ��ֵ2s,
     * ��ʱ�����С�ڵ���adapter����ĳ�ʱʱ��
     */
    int                		 _iEmptyCheckTimeout;

	/**
	 * udp����ʱ���հ������С,�������udp���ջ�����Ч
	 */
	size_t 					 _nUdpRecvBufferSize;
};

typedef TC_AutoPtr<TC_EpollServer> TC_EpollServerPtr;
////////////////////////////////////////////////////////////////////////////////
}

#endif

