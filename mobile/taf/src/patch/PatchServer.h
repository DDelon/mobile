#include "servant/Application.h"
#include "Patch.h"

using namespace taf;

class PatchServer : public Application
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
};


