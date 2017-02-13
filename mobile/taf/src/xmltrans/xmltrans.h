#ifndef __XMLTRANS__H__
#define __XMLTRANS__H__

#include <string>
#include <vector>
#include <map>
#include "Markup.h"
#include "jce/Jce.h"
#include "jce/RequestF.h"

using namespace taf;

class XmlTrans : public taf::RequestPacket
{
public:
	struct XmlRequest
	{
		std::string sSvtName;
		std::string sFunName;

		std::vector<char> vecBuff;
	};
public:
	XmlTrans();

public:
	/**
	 * ����XML���ݣ����з���
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param sXml  XML�ĵ�����
	 * 
	 * @return bool �ɹ�����true������false
	 */
	bool parse(const std::string & sXml);

	/**
	 * ��XMLת����WUPЭ�������ַ���
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapRequest ����XML�ĵ�����������������
	 * @param sResEncode ����Ҫ��Ľ��XML�ĵ��ı��뷽ʽ
	 */
	void xml2wup(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode);

	/**
	 * ��XMLת����JCEЭ�������ַ���
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapRequest 
	 * @param sResEncode 
	 */
	void xml2jce(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode);

	/**
	 * ��ȡģ����XML��ת����WUP
	 * 
	 * @author kevintian (2010-8-27)
	 * 
	 * @param mapRequest 
	 * @param sResEncode 
	 */
	void xml2res(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode);

	/**
	 * ��WUPЭ�������ַ���ת����XML
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapResponse 
	 *  				  ��������֮���õķ���map<����ID������ķ�������>
	 * @param sXmlText 
	 */
	void wup2xml(const std::string & sResponse, std::string & sXmlText);

	/**
	 * ��WUPЭ�������ַ���ת����XML
	 * 
	 * @author kevintian (2010-8-18)
	 * 
	 * @param szResponse 
	 * @param sizeLength 
	 * @param sXmlText 
	 */
	void wup2xml(const char * szResponse, size_t sizeLength, std::string & sXmlText);

	/**
	 * ��ȡ���ؽ����WUP�������Ϣ
	 * 
	 * @author kevintian (2010-8-23)
	 * 
	 * @param szBuff 
	 * @param sizeLength 
	 * @param vecServnat 
	 */
	void getServant(const char * szBuff, size_t sizeLength, std::vector<XmlRequest> & vecServnat);

	/**
	 * ��JCEЭ�������ַ���ת����XML
	 * 
	 * @author kevintian (2010-7-19)
	 * 
	 * @param mapResponse 
	 *  				  ��������֮���õķ���map<����ID������ķ�������>
	 * @param sXmlText 
	 */
	void jce2xml(const std::map<std::string, std::vector<char> > & mapResponse, std::string & sXmlText);

private:
	typedef std::vector<char> MAP_DATA_ITEM_BUFF;
    typedef std::map<std::string, MAP_DATA_ITEM_BUFF> MAP_DATA_ITEM;
	typedef std::map<std::string, MAP_DATA_ITEM> MAP_DATA;

	void _xml2wup(std::map<std::string, XmlRequest> & mapRequest, std::string & sResEncode, bool bWithTaf, bool bWithReturn);
	void _xml2wup(MAP_DATA & mapData);
	void _xml2wupReturn(MAP_DATA & mapData);
	void _xml2jce(taf::JceOutputStream<taf::BufferWriter> & osjce);

	void _writeField(int iTag, taf::JceOutputStream<taf::BufferWriter> & os);
	void _writeField(int iTag, std::string & sVName, std::string & sCName, taf::JceOutputStream<taf::BufferWriter> & os);
	void _readField(const int iTag, bool isRequire, const std::string & sTagName, taf::JceInputStream<taf::BufferReader> & is);

	void _jce2xml(const std::string & sRequestID, const std::vector<char> & vecBuff);	
	void _jce2xmlReturn(taf::JceInputStream<taf::BufferReader> & is);
	void _jce2xmlPara(taf::JceInputStream<taf::BufferReader> &is);

	void _wup2xml(const char * szResponse, size_t sizeLength, std::string & sXmlText, bool bWithTaf);
	void _wup2xml(const char * szResponse, size_t sizeLength, bool bWithTaf);
	void _wup2xmlReturn();
	void _wup2xmlPara();

	void _init();
private:
	CMarkup _xmlReq;	//����XML�ļ�
	CMarkup _xmlRes;	//���XML�ļ�

	MAP_DATA _mapRes;	//����WUP��������ݽṹ
};

#endif

