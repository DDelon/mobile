#ifndef __PROPERTY_SERVER_H_
#define __PROPERTY_SERVER_H_

#include "servant/Application.h"
#include "PropertyReapThread.h"

using namespace taf;

class PropertyServer : public Application
{
    
public:
    static void getTimeInfo(time_t &tTime,string &sDate,string &sFlag);
    
        
protected:
    /**
     * ��ʼ��, ֻ����̵���һ��
     */
    virtual void initialize();

    /**
     * ����, ÿ�����̶������һ��
     */
    virtual void destroyApp();
    
public:
    static string g_sClonePatch;
    //���ݿ������
    static int g_iInsertInterval;

private:

    void initHashMap();
    
    PropertyReapThread* _pReapThread;
    


};

#endif

