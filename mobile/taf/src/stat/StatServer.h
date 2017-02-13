#include "servant/Application.h"
#include "servant/StatF.h"
#include "ReapSSDThread.h"

using namespace taf;

class StatServer : public Application
{
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

	void getTimeInfo(time_t &tTime,string &sDate,string &sFlag);

	bool cmdSetRandOrder(const string& command, const string& params, string& result);

	//��ȡ��������ipӳ��
	map<string, string>& getVirtualMasterIp(void);

	//�ж��Ƿ��������MTT��Ĺ���
	bool IsEnableMTTDomain(void);

	string getRandOrder(void);

	string getClonePath(void);

	int getInserInterv(void);

private:
    void initHashMap();
	ReapSSDThread* _pReapSSDThread;

	//��������ip����
	map<string, string> _mVirtualMasterIp;

	//���MTT ͳ����������Ŀ���
	bool _bEnableMTTDomain;

	// �����⿪��
	string  _sRandOrder;

	//���ݻ���Ŀ¼
	string _sClonePath;

    //���ݿ������,��λ����
    int _iInsertInterval;
};

extern StatServer g_app;

