#include "ReapThread.h"

ReapThread::ReapThread():
_bTerminate(false),
_iLoadObjectsInterval1(10),
_iLeastChangedTime1(60),
_iLoadObjectsInterval2(3600),
_iLeastChangedTime2(30*60),
_iNodeTimeout(150),
_iRegistryTimeout(150),
_bRecoverProtect(true),
_iRecoverProtectRate(30),
_bHeartBeatOff(false)
{
}

ReapThread::~ReapThread()
{
    if (isAlive())
    {
        terminate();
        notifyAll();
        getThreadControl().join();
    }
}


int ReapThread::init()
{
    LOG->debug()<<"begin ReapThread init"<<endl;

    //��ʼ������db����
    extern TC_Config * g_pconf;
    _db.init(g_pconf);

    //���ض����б��ʱ����
    _iLoadObjectsInterval1 = TC_Common::strto<int>((*g_pconf).get("/taf/reap<loadObjectsInterval1>", "10"));
	//��һ�׶μ������ʱ����µļ�¼,Ĭ����60��
	_iLeastChangedTime1    = TC_Common::strto<int>((*g_pconf).get("/taf/reap<LeastChangedTime1>", "60"));

	_iLoadObjectsInterval2 = TC_Common::strto<int>((*g_pconf).get("/taf/reap<loadObjectsInterval2>", "3600"));
	//_iLeastChangedTime2    = TC_Common::strto<int>((*g_pconf).get("/taf/reap<LeastChangedTime2>", "60"));

    //node������ʱʱ��
    _iNodeTimeout = TC_Common::strto<int>((*g_pconf)["/taf/reap<nodeTimeout>"]);

    //����������ʱʱ��
    _iRegistryTimeout = TC_Common::strto<int>((*g_pconf)["/taf/reap<reigistryTimeout>"]);

    //�Ƿ�����DB�ָ���������
    _bRecoverProtect = (*g_pconf).get("/taf/reap<recoverProtect>", "Y") == "N"?false:true;

    //����DB�ָ���������״̬�¼���ֵ
    _iRecoverProtectRate = TC_Common::strto<int>((*g_pconf).get("/taf/reap<recoverProtectRate>", "30"));

	_bHeartBeatOff = (*g_pconf).get("/taf/reap<heartbeatoff>", "N") == "Y"?true:false;

    //��Сֵ����
    _iLoadObjectsInterval1 = _iLoadObjectsInterval1 < 5 ? 5 : _iLoadObjectsInterval1;

	//node�ĳ�ʱ��ⲻ��Ҫ��ôƵ��
    _iNodeTimeout         = _iNodeTimeout         < 15 ? 5 : _iNodeTimeout;


    _iRegistryTimeout       = _iRegistryTimeout       < 5 ? 5 : _iRegistryTimeout;

    _iRecoverProtectRate  = _iRecoverProtectRate  < 1 ? 30: _iRecoverProtectRate;

    //���ض����б�
    _db.initFileCache();
    _db.loadObjectIdCache(_bRecoverProtect, _iRecoverProtectRate,60,true);
	_db.loadGroupPriority();

    LOG->debug()<<"ReapThread init ok."<<endl;

    return 0;
}

void ReapThread::terminate()
{
    LOG->debug() << "[ReapThread terminate.]" << endl;
    _bTerminate = true;
}

void ReapThread::run()
{
	//�������ط���������׶�
	//��һ�׶μ���ʱ��
    time_t tLastLoadObjectsStep1 = TC_TimeProvider::getInstance()->getNow();

	//ȫ������ʱ��
	time_t tLastLoadObjectsStep2 = TC_TimeProvider::getInstance()->getNow();

    time_t tLastCheckNode = 0;
    time_t tLastQueryServer = 0;
    time_t tNow;
	while(!_bTerminate)
    {
        try
        {
            tNow = TC_TimeProvider::getInstance()->getNow();

            //���ض����б�
            if(tNow - tLastLoadObjectsStep1 >= _iLoadObjectsInterval1)
            {
                tLastLoadObjectsStep1 = tNow;

				_db.updateRegistryInfo2Db(_bHeartBeatOff);

				if(tNow - tLastLoadObjectsStep2 >= _iLoadObjectsInterval2)
				{
					tLastLoadObjectsStep2 = tNow;
					//ȫ������,_iLeastChangedTime2����û������
                	_db.loadObjectIdCache(_bRecoverProtect, _iRecoverProtectRate,_iLeastChangedTime2,true);
				}
				else
				{
					_db.loadObjectIdCache(_bRecoverProtect, _iRecoverProtectRate,_iLeastChangedTime1,false);
				}

				_db.loadGroupPriority();
            }

            //��ѯ������ʱ��node
            if(tNow - tLastCheckNode >= _iNodeTimeout)
            {
                tLastCheckNode = tNow;
                _db.checkNodeTimeout(_iNodeTimeout);
            }

            //��ѯ������ʱ������
            if(tNow - tLastQueryServer >= _iRegistryTimeout)
            {
                tLastQueryServer = tNow;
                _db.checkRegistryTimeout(_iRegistryTimeout);
            }


            TC_ThreadLock::Lock lock(*this);
            timedWait(100); //ms
        }
        catch(exception & ex)
        {
            LOG->error() <<"ReapThread exception:" << ex.what() << endl;
        }
        catch(...)
        {
            LOG->error() <<"ReapThread unknown exception:" << endl;
        }
    }
}



