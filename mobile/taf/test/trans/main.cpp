#include "struct.h"
#include "xmltrans.h"
#include "util/tc_file.h"
#include "jce/wup.h"


int towup(XmlTrans & trans)
{
	std::map<std::string, XmlTrans::XmlRequest> mapRequest;
	std::string sEncode;
	
	//��һ�η���
	try
	{
		trans.xml2wup(mapRequest, sEncode);
	}
	catch (std::exception & ex)
	{
		std::cout << "exception:" << ex.what() << std::endl;
	}

	std::cout << "size:" << mapRequest.size() << std::endl;

	std::map<std::string, std::vector<char> > mapResponse;
	for (std::map<std::string, XmlTrans::XmlRequest>::iterator it = mapRequest.begin(); it != mapRequest.end(); it++)
	{
		std::cout << "id :" << it->first << std::endl;
		std::cout << "svt:" << it->second.sSvtName << std::endl;
		std::cout << "fun:" << it->second.sFunName << std::endl;

		mapResponse.insert(std::make_pair(it->first, it->second.vecBuff));
	}

	try
	{
		{
			//�����ת����xml�ļ�
			std::string sXmlText;
			trans.wup2xml(mapResponse, sXmlText);
			std::cout << sXmlText << std::endl;
		}

		//�ڶ���ת����xml�ļ�
		{
			std::cout << "�ڶ���ת����xml�ļ�" << std::endl;

			std::string sXmlText;
			trans.wup2xml(mapResponse, sXmlText);
			std::cout << sXmlText << std::endl;
		}


		#if 0
		XmlTrans::XmlRequest & request = mapRequest.begin()->second;
		wup::UniAttribute<> w;
		w.decode(request.vecBuff);

		//��ȡ��һ���ṹ��
		Test::stUserInfo_t st;
		w.get("myuser", st);
		st.display(std::cout);
		
		
		//��ȡ�ڶ���boolֵ
		bool bs = w.get<bool>("is");
		std::cout << boolalpha << bs << std::endl;
		
		//��ȡ����������
		std::vector<char> vc;
		w.get("vec", vc);

		std::cout << "vc" << vc.size() << ":" << std::string(static_cast<char *>(&vc[0]), vc.size()) << std::endl;
		
		//��ȡ���ĸ�����
		std::vector<Test::stUserInfo_t> vecUser;
		std::cout <<"class:" << taf::Class< std::vector<Test::stUserInfo_t> >::name() << std::endl;
		
		w.get("vecuser", vecUser);
		
		std::cout <<"vecUser:" << vecUser.size() << std::endl;
		
		for (std::vector<Test::stUserInfo_t>::size_type i = 0; i < vecUser.size(); i++)
			vecUser[i].display(std::cout);

		//��ȡ���������
		std::map<std::string, std::string> mymap;
		
		w.get("mymap", mymap);
		
		std::cout << mymap.size() << std::endl;
		
		for (std::map<std::string, std::string>::iterator it = mymap.begin(); it != mymap.end(); it++)
			std::cout << it->first << "   " << it->second << std::endl;
		#endif
		
	}
	catch (std::exception & ex)
	{
		std::cout << "exception:" << ex.what() << std::endl;
	}


	return 0;
}

int tojce(XmlTrans & trans)
{
	std::map<std::string, XmlTrans::XmlRequest> mapRequest;
	std::string sEncode;
	
	try
	{
		trans.xml2jce(mapRequest, sEncode);
	}
	catch (std::exception & ex)
	{
		std::cout << "exception:" << ex.what() << std::endl;
	}

	std::cout << "size:" << mapRequest.size() << std::endl;

	std::map<std::string, std::vector<char> > mapResponse;
	for (std::map<std::string, XmlTrans::XmlRequest>::iterator it = mapRequest.begin(); it != mapRequest.end(); it++)
	{
		std::cout << "id :" << it->first << std::endl;
		std::cout << "svt:" << it->second.sSvtName << std::endl;
		std::cout << "fun:" << it->second.sFunName << std::endl;

		mapResponse.insert(std::make_pair(it->first, it->second.vecBuff));
	}

	if (mapRequest.size() < 1) 
	{
		std::cout << "û�б��뷵��" << std::endl;
		return 0;
	}

	try
	{
		//�����ת����xml�ļ�
		std::string sXmlText;
		trans.jce2xml(mapResponse, sXmlText);
		std::cout << sXmlText << std::endl;

#if 0		

		taf::JceInputStream<taf::BufferReader> is;
		is.setBuffer(mapRequest.begin()->second.vecBuff);


		//��ȡ��һ������
		Test::stUserInfo_t myuser;
		is.read(myuser, 1);
		myuser.display(std::cout);

		//��ȡ�ڶ���boolֵ
		bool bs;
		is.read(bs, 2);
		std::cout << boolalpha << bs << std::endl;
		
		//��ȡ����������
		std::vector<char> vc;
		is.read(vc, 3);

		std::cout << "vc" << vc.size() << ":" << std::string(static_cast<char *>(&vc[0]), vc.size()) << std::endl;
		
		//��ȡ���ĸ�����
		std::vector<Test::stUserInfo_t> vecUser;
		is.read(vecUser, 4);
		std::cout <<"vecUser:" << vecUser.size() << std::endl;
		
		for (std::vector<Test::stUserInfo_t>::size_type i = 0; i < vecUser.size(); i++)
			vecUser[i].display(std::cout);

		//��ȡ���������
		std::map<std::string, std::string> mymap;
		is.read(mymap, 5);
		std::cout << mymap.size() << std::endl;
		
		for (std::map<std::string, std::string>::iterator it = mymap.begin(); it != mymap.end(); it++)
			std::cout << it->first << "   " << it->second << std::endl;
#endif
	}
	catch (std::exception & ex)
	{
		std::cout << "exception:" << ex.what() << std::endl;
	}


	return 0;
}


int main()
{
	std::string content = taf::TC_File::load2str("./request.xml");
		
	XmlTrans trans;
	if (trans.parse(content) == false)
	{
		std::cout << "parse error" << std::endl;
		return 1;
	}

	towup(trans);

	
	return 0;
}
