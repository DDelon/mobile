#include "servant/Application.h"
#include "Server.h"

using namespace taf;
using namespace Server1;
using namespace Server2;

class TestObj : public Test
{
public:
    /**
     *
     */
    TestObj(){};

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize();

    /**
     * �˳�
     */
    virtual void destroy();

    virtual Server1::TestInfo get(const Server1::TestInfo& info, taf::JceCurrentPtr);
    virtual int getOut(int i, AInfo &ai1, AInfo &ai2, JceCurrentPtr current);

    virtual int test(const Server1::TestInfo &ti, JceCurrentPtr current);
    virtual std::string echo(const std::string &ti, JceCurrentPtr current);
    virtual int echo_int(int i, JceCurrentPtr current);

    virtual ETest testETest(ETest t, JceCurrentPtr current);

    bool procAdmin(const string& command, const string& params, string& result);
};

class TestExObj : public TestEx
{
public:
    /**
     *
     */
    TestExObj(){};

    /**
     * ��ʼ��
     *
     * @return int
     */
    virtual void initialize() {};

    /**
     * �˳�
     */
    virtual void destroy() {};

    /**
     * ��ȡ����
     * @param k
     * @param v
     *
     * @return int
     */
    virtual int test(const Server1::TestInfo &ti, JceCurrentPtr current);


    virtual int testEx(const Server2::TestInfo &ti, JceCurrentPtr current);
};


class Server : public Application
{
protected:

    /**
     * ����
     */
    virtual void initialize();

    /**
     *
     */
    virtual void destroyApp();
};


