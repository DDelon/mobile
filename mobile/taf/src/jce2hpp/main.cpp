#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jce2hpp.h"


void usage()
{
    cout << "Usage : jce2cpp [OPTION] jcefile" << endl;
    cout << "  --coder=Demo::interface1;Demo::interface2   create interface encode and decode api" << endl;
    cout << "  --dir=DIRECTORY                             generate source file to DIRECTORY(�����ļ���Ŀ¼DIRECTORY,Ĭ��Ϊ��ǰĿ¼)" << endl;
    cout << "  --pdu                                       special format for pdu-protocol" << endl;
    cout << "  --check-default=<true,false>                ���optional�ֶ�ֵΪĬ��ֵ�����(Ĭ�ϴ��)" << endl;
    cout << "  --lua                                       gracefully export structures to lua with luavatar" << endl;
    cout << "  --os                                        ֻ����jce�ļ��нṹ������ĳ����" << endl;
    cout << "  --json                                      ����json�����" << endl;
    cout << "  --tafMSF                                    ����tafMSF���ֿռ�Ĵ���" << endl;
    cout << "  --unknown                                   ���ɴ���jce�������е�unkown field�Ĵ���" << endl;
    cout << "  jce2cpp support type: bool byte short int long float double vector map" << endl;
    exit(0);
}

void check(vector<string> &vJce)
{
    bool b = true;

    for(size_t i  = 0; i < vJce.size(); i++)
    {
        string ext  = taf::TC_File::extractFileExt(vJce[i]);
        if(ext == "jce")
        {
            if(!b)
            {
                usage();
            }
            if(!taf::TC_File::isFileExist(vJce[i]))
            {
                cerr << "file '" << vJce[i] << "' not exists" << endl;
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

    bool bCoder = option.hasParam("coder");
    vector<string> vCoder;
    if(bCoder)
    {
        vCoder = taf::TC_Common::sepstr<string>(option.getValue("coder"), ";", false);
        if(vCoder.size() == 0)
        {
            usage();
            return 0;
        }
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

    j2c.setPDU(option.hasParam("pdu"));
    j2c.setCheckDefault(taf::TC_Common::lower(option.getValue("check-default")) == "true"?true:false);

	//added by forrestliu @20101027 for lua support
	j2c.setLuaSupport(option.hasParam("lua"));

	//added by zhangcunli @20111205 for json support
    j2c.setJsonSupport(option.hasParam("json"));


    j2c.setOnlyStruct(option.hasParam("os"));

    //added by johnson
	if (option.hasParam("tafMSF"))
	{
		j2c.setTafMSF("tafMSF");
	}
	else
	{
		j2c.setTafMSF("taf");
	}
	try
	{
		j2c.setUnknownField(option.hasParam("unknown"));
	    for(size_t i = 0; i < vJce.size(); i++)
	    {

	        g_parse->parse(vJce[i]);
	        j2c.createFile(vJce[i], vCoder);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

