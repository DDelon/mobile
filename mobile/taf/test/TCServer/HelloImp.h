#ifndef _TC_IMP_H_
#define _TC_IMP_H_

#include "Hello.h"
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace Test;

class HelloImp : public Hello
{
public:
    /**
     *
     */
    HelloImp(){};

     /**
     * ��ʼ����Hello�����⺯����HelloImp��ʼ��ʱ����
     *
     * @return int
     */
    virtual void initialize()
	{
	}

    /**
     * �˳���Hello�����⺯���������˳�ʱ����
     */
	virtual void destroy(){};

    /**
     * testHello��ʵ��jce�ļ��еĽӿ�
     *
     * @return int
     */
	int testHello(const string &s, string &r, JceCurrentPtr current)
	{
        LOG->debug() << s << endl;
        LOG->debug() << TC_Common::tostr(current->getContext())<< endl;
//        FDLOG("test") << s << endl;
		r = s;
	    return 0;
	}
};


#endif



