#include "TCServer.h"
#include "HelloImp.h"
#include "echo.h"
#include "http.h"
#include "string.h"

using namespace std;

TCServer g_app;

string ip;
string port;

/////////////////////////////////////////////////////////////////
void
TCServer::initialize()
{
	//initialize application here:
	//...

    TafRollLogger::getInstance()->logger()->setLogLevel(TC_RollLogger::INFO_LOG);
	addServant<HelloImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".HelloObj");

    bind<HttpHandle>("tcp -h " + ip + " -p " + port + " -t 50000");

	//bind<HttpHandle>("tcp -h 172.25.38.21 -p 8080 -t 5000");
//	bind<EchoHandle>("udp -h 127.0.0.1 -p 8082 -t 5000");
//	bind<EchoHandle>("udp -h 172.25.38.67 -p 8082 -t 5000");
//  bind<EchoHandle>("tcp -h 172.25.38.14 -p 8882 -t 5000");
//  bind<StringHandle>("tcp -h 172.25.38.67 -p 8082 -t 5000");
}

/////////////////////////////////////////////////////////////////
void
TCServer::destroyApp()
{
	//destroy application here:
	//...
}

/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
        ip = argv[1];
        port = argv[2];

		g_app.main(argc, argv);
		g_app.waitForShutdown();
	}
	catch (std::exception& e)
	{
		cerr << "std::exception:" << e.what() << std::endl;
	}
	catch (...)
	{
		cerr << "unknown exception." << std::endl;
	}
	return -1;
}
/////////////////////////////////////////////////////////////////

/*
�������TC_EpollServer����ʽ
#include "echo.h"
#include "http.h"
#include "string.h"

using namespace std;
using namespace taf;

class TCServer : public TC_EpollServer
{
public:
    template<typename T>
    void bind(const string &str)
    {
        TC_EpollServer::BindAdapterPtr lsPtr = new TC_EpollServer::BindAdapter(this);

        //����adapter����, Ψһ
        lsPtr->setName(str);
        //���ð󶨶˿�
        lsPtr->setEndpoint(str);
        //�������������
        lsPtr->setMaxConns(20000);
        //���������߳���
        lsPtr->setHandleNum(10);
        //����Э�������
        lsPtr->setProtocol(&T::parse);
        //�����߼�������
        lsPtr->setHandle<T>();

        //�󶨶���
        TC_EpollServer::bind(lsPtr);
    }
};

int main(int argc, char *argv[])
{
    try
	{
        TC_RollLogger logger;

        TCServer ss;
        ss.setLocalLogger(&logger);

        ss.bind<HttpHandle>("tcp -h 172.25.38.67 -p 8083 -t 5000");
        ss.bind<EchoHandle>("tcp -h 127.0.0.1 -p 8082 -t 5000");
        ss.bind<EchoHandle>("udp -h 127.0.0.1 -p 8082 -t 5000");
        ss.bind<EchoHandle>("tcp -h 172.25.38.14 -p 8882 -t 5000");
        ss.bind<StringHandle>("tcp -h 172.25.38.67 -p 8082 -t 5000");

        ss.waitForShutdown();
	}
	catch(exception &ex)
	{
        cout << ex.what() << endl;
	}

	return 0;
}

*/

