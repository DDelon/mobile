#ifndef __NODE_SERVER_H_
#define __NODE_SERVER_H_

#include "servant/Application.h"
#include "KeepAliveThread.h"
#include "QueryF.h"
#include "BatchPatchThread.h"

using namespace taf;

class NodeServer;
extern NodeServer g_app;
class NodeServer : public Application
{
public:
     /**
     * ��ȡAdapter Endpoint
     */
    TC_Endpoint getAdapterEndpoint(const string& name ) const;

      /**
     * ���Config
     * @param filename
     */
    bool addConfig(const string &filename){return Application::addConfig(filename);}

    /*
    *   Ȩ�޿���
    */
    bool isValid(const string& ip);

     /*
    *   �ϱ�string��notify
    */
    void reportServer(const string &sServerId,const string &sResult);

public:
    /*
    *   ����ָ��
    */
    bool cmdViewServerDesc(const string& command, const string& params, string& result);
	/**
	 *@brief  ���ط��������ļ�,��ʹ֮��Ч
	 *
	 */
	bool cmdReLoadConfig(const string& command, const string& params, string& result);

protected:

    /**
     * ��ʼ��, ֻ����̵���һ��
     */
    virtual void initialize();

    /**
     * ����, ֻ����̵���һ��
     */
    virtual void destroyApp();

    /**
     * ��ʼ��hashmap
     */
    void initHashMap();
private:
	/**
	 *��ʼ������obj����
	 */
	void initRegistryObj();
private:
    KeepAliveThread *   _pKeepAliveThread;

    BatchPatch *        _pBatchPatchThread;

    static string g_sNodeIp;
};
#endif


