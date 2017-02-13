#include "PatchServer.h"
#include <iostream>

using namespace std;

TC_Config *g_conf;

/**
 * ˵��
 * 1 ��Ҫ����һ��·��
 * 2 �����Ŀ¼��linkĿ¼, �򲻽���linkĿ¼����
 * 3 ����ļ���link, ���ȡlink���ļ�
 * 4 ���ͬ�����ļ���linkĿ¼, �����linkĿ¼����, ����Ĺ�����1
 * @param argc
 * @param argv
 *
 * @return int
 */
int main(int argc, char *argv[])
{
    try
    {
        PatchServer app;
        g_conf = &app.getConfig();
        app.main(argc, argv);
        app.waitForShutdown();
    }
    catch(exception &ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}


