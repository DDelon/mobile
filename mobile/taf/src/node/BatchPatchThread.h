#ifndef __BATCH_PATCH_THREAD_H_
#define __BATCH_PATCH_THREAD_H_
#include "servant/Application.h"
#include "Node.h"
#include "ServerObject.h"

class BatchPatchThread;

class BatchPatch : public taf::TC_ThreadLock
{
public:
    /**
    * ���캯��
    */
    BatchPatch();

    /**
    * ��������
    */
    ~BatchPatch();

    /**
     * �����߳�
     */
    void terminate();

    /**
     * �߳�����
     */
    void start(int iThreadNum);

public:
    /**
     * ���뷢������
     */
    void push_back(const taf::PatchRequest & request, ServerObjectPtr servePtr);

    /**
     * �ӷ��������л�ȡ��������
     */
    bool pop_front(taf::PatchRequest & request, ServerObjectPtr &servePtr);

    /**
     * ��ȡ�������д�С
     */
    size_t getPatchQueueSize();

    /**
     * ���������ݴ�Ŀ¼
     */
    void setPath(const std::string & sDownloadPath)
    {
        _sDownLoadPath = sDownloadPath;
    }

private:
    taf::TC_ThreadLock                  _QueueMutex;

    std::deque<taf::PatchRequest>       _PatchQueue;
    std::deque<ServerObjectPtr>    _PatchServerQueue;


    std::set<std::string>               _PatchIng;

    std::vector<BatchPatchThread *>     _vecRunners;

    std::string                         _sDownLoadPath;       
};


class BatchPatchThread : public TC_Thread
{
public:
    BatchPatchThread(BatchPatch * patch);

    ~BatchPatchThread();

    virtual void run();

    void terminate();

public:
    /**
     * ���������ݴ�Ŀ¼
     */
    void setPath(const std::string & sDownloadPath)
    {
        _sDownloadPath = sDownloadPath;
    }

    /**
     * ִ�з�����������
     */
    void doPatchRequest(const taf::PatchRequest & request, ServerObjectPtr server);

protected:
    BatchPatch *    _BatchPatch;

    std::string     _sDownloadPath; //�ļ�����Ŀ¼

protected:
    bool            _bShutDown; 
};

#endif
