#include "util/tc_option.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include "jceAnalyzer.h"
#include "wupRequestF.h"

void usage()
{
    cout << "Usage : ./jceAnalyzer jcefile --FileBuff=jceBuffer.txt --structName=TestInfo  [--startPos=0] [--wupPutName=mystruct]" << endl;

    cout << "  jceAnalyzer        ������" << endl;
    cout << "  jcefile            ����ָ�������ļ��а�����������jce�ṹ����" << endl;

    cout << "  --structName       ����ָ����ָ���������Ľṹ�����ṹ��������JCE�ļ�������" << endl;


	cout << "  --FileBuff         ָ����������jce�����Ʊ����ļ���" << endl;
	cout << "  --startPos         ָ���ӱ����ļ��ĵڼ����ֽڿ�ʼ�ǽṹ�ı��룬ȱʡ�ӵ�0�ֽڿ�ʼ" << endl;

    cout << "  --wupPutName       wup����ʱ���������ƣ�û�и�ѡ���ʾ�������Ķ��������ݲ���wup����" << endl;
	cout << "  --dumpWup          �Ƿ����wup���е�sBuffer����,�˹���������wupPutName�Ĭ���ǲ����"<<endl;
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
			usage();
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
    vector<string> vJceFiles = option.getSingle();

    check(vJceFiles);

    if(option.hasParam("help"))
    {
        usage();
    }

    string streamFile;

    JceAnalyzer jAnalyzer;

    if(option.hasParam("FileBuff"))
    {
        streamFile = option.getValue("FileBuff");
		jAnalyzer.setStreamFileName(streamFile);
    }
	else
	{
        usage();
	}


    if(option.hasParam("structName"))
    {
		string sStructName = option.getValue("structName");
		if(!sStructName.empty())
		{
			jAnalyzer.setStructName(sStructName);
		}
		else
		{
			cout<<"structName shouldn't be empty"<<endl;
			exit(1);
		}
    }

    if(option.hasParam("startPos"))
    {
        string pos = option.getValue("startPos");
		if(taf::TC_Common::strto<int>(pos) >= 0)
		{
			jAnalyzer.setStartPos(taf::TC_Common::strto<int>(pos));
		}
		else
		{
			cout<<"startPos should be >=0 "<<endl;
			exit(1);

		}
    }

	string wupPutName = option.getValue("wupPutName");
    if( wupPutName != "")
    {
		jAnalyzer.setStartPos(4); //wup��ͷ��4���ֽڵĳ���

		jAnalyzer.setPutName(wupPutName);

		jAnalyzer.setWup(true);
		if(option.hasParam("dumpWup"))
		{
			jAnalyzer.setDumpWup(true);
		}
		//
		taf::TC_File::save2file("./tmpWupRequestF.jce", g_sWupPacket);
	    g_parse->parse("./tmpWupRequestF.jce");

		taf::TC_File::removeFile("./tmpWupRequestF.jce", true);

		//analyze wup jce
		jAnalyzer.analyzeFile("./tmpWupRequestF.jce", streamFile);
		jAnalyzer.setWup(false);


		if (vJceFiles.size() > 0)
		{
			//��Ҫ����wup���е������ڶ��jce�ļ������
			g_parse->parse(vJceFiles[0]);
			//����wup���ݰ��е�sBuffer����,��structName��Ӧ������
			jAnalyzer.analyzeWithWupBuffer(vJceFiles[0], "");
		}
		return 0;

	}
	try
	{
		for (size_t i = 0; i < vJceFiles.size(); i++)
	    {
	        g_parse->parse(vJceFiles[i]);
	        jAnalyzer.analyzeFile(vJceFiles[i], streamFile);
	    }
	}catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}

    return 0;
}

