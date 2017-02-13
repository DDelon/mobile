#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jce2mcarecpp.h"


void usage()
{
    cout << "Usage : jce2mcarecpp [OPTION] jcefile" << endl;
    cout << "  create interface encode and decode api" << endl;
	cout << "  --dir=DIRECTORY                   generate source file to DIRECTORY(�����ļ���Ŀ¼DIRECTORY,Ĭ��Ϊ��ǰĿ¼)" << endl;
    cout << "  --check-default=<true,false>      ���optional�ֶ�ֵΪĬ��ֵ�����(Ĭ�ϲ����)" << endl;
    cout << "  support type: bool byte short int long float double vector map" << endl;
    exit(0);
}

void check(vector<string> &vJce)
{
    for(size_t i  = 0; i < vJce.size(); i++)
    {
        string ext  = taf::TC_File::extractFileExt(vJce[i]);
        if(ext == "jce")
        {
            if(!taf::TC_File::isFileExist(vJce[i]))
            {
                cerr << "file '" << vJce[i] << "' not exists" << endl;
				usage();
                exit(0);
            }
        }
        else
        {
            cerr << "only support jce file." << endl;
            exit(0);
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage();
    }

    taf::TC_Option option;
    option.decode(argc, argv);
    vector<string> vJce = option.getSingle();

    check(vJce);

    if(option.hasParam("help"))
    {
        usage();
    }


    //�Ƿ������taf��ͷ
    g_parse->setTaf(option.hasParam("with-taf"));
    g_parse->setHeader(option.getValue("header"));

    Jce2Cpp j2c;
	if (option.hasParam("dir"))
	{
		j2c.setBaseDir(option.getValue("dir"));
	}
	else
	{
		j2c.setBaseDir(".");
	}

    j2c.setCheckDefault(taf::TC_Common::lower(option.getValue("check-default")) == "false"?false:true);
	try
	{
	    for(size_t i = 0; i < vJce.size(); i++)
	    {
	        g_parse->parse(vJce[i]);
	        j2c.createFile(vJce[i]);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

