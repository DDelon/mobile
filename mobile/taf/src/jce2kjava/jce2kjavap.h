#ifndef __JCE2KJAVAP_H__
#define __JCE2KJAVAP_H__

#include <string>
#include "parse/parse.h"

#define JCE_PACKAGE     ".taf.jce"
#define PROXY_PACKAGE   ".taf.proxy"
#define HOLDER_PACKAGE  ".taf.holder"
#define SERVER_PACKAGE  ".taf.server"
#define WUP_PACKAGE     ".jce.wup"

#define TAB g_parse->getTab()
#define INC_TAB g_parse->incTab()
#define DEL_TAB g_parse->delTab()

struct CNode
{
	std::string sInitName;
	std::string sInitValue;
	int iTag;
	bool bIsRequire;
	TypePtr pType;					//��ǰnode������

	std::string sFinalName;			//������ɳ�Ա������
	std::string sArray;				//�����iType���sFinalName������
	std::string sLocator;			//sFinalName[iTag_5][iTag_5_1] = readInt
	
	std::string sLoopPrefix;		//ѭ��������ǰ׺
	std::string sTypeValue;			//ԭ�����͵ĳ�ʼֵ

	std::vector<CNode *> vecNodes;
	CNode():iTag(0), bIsRequire(false), pType(NULL){}
};

class Jce2KJavaP
{
public:
	Jce2KJavaP();
    /**
     * ���ô������ɵĸ�Ŀ¼
     * @param dir
     */
    void setBaseDir(const string &dir);

    /**
     * ���ð�ǰ׺
     * @param prefix
     */
    void setBasePackage(const string &prefix);

    /**
     * �����Ƿ���Ҫ����˴���
     */
    void setWithServant(bool bWithServant) { _bWithServant = bWithServant;}

    /**
     * ����
     * @param file
     * @param isFramework �Ƿ��ǿ��
     */
	void createFile(const string &file) const;


	/**
	 * �趨�Ƿ�����read�ӿ�
	 */
	void setRead(const bool bRead) { _bRead = bRead; }

	/**
	 * �趨�Ƿ�����Write�ӿ�
	 */
	void setWrite(const bool bWrite) { _bWrite = bWrite; }

	/**
	 * �趨�Ƿ�����EventHandlerʾ��
	 */
	void setEventHandler(const bool bEvent) { _bEvent = bEvent; }

    /**
     * ����TAF��ı���
     */
    void setTafPacket(const std::string & sPacket) 
    {
        s_JCE_PACKAGE 		= sPacket + JCE_PACKAGE;
        s_PROXY_PACKAGE		= sPacket + PROXY_PACKAGE;
        s_HOLDER_PACKAGE	= sPacket + HOLDER_PACKAGE;
        s_SERVER_PACKAGE	= sPacket + SERVER_PACKAGE;
        s_WUP_PACKAGE		= sPacket + WUP_PACKAGE;
    }
        
protected:
    /**
     * ���������ռ��ȡ�ļ�·��
     * @param ns �����ռ�
     *
     * @return string
     */
    string getFilePath(const string &ns) const;

    string _packagePrefix;
    string _baseDir;
    bool   _bWithServant;
	bool   _bRead;
	bool   _bWrite;
	bool   _bEvent;

private:
	/**
     * ���ɽṹ��java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    const std::string generateJava(const StructPtr & pPtr, const NamespacePtr & nPtr) const;
    
	/**
     * ����Const������java�ļ�����
     * @param pPtr
     * @param cs
     * @return void
     */
    void generateConst(const vector<ConstPtr> &cs,const NamespacePtr & pPtr) const;
    
    /**
     * �������ֿռ�java�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateJava(const NamespacePtr &pPtr) const;

	/**
     * ����ÿ��jce�ļ���java�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateJava(const ContextPtr &pPtr) const;

	const std::string toTypeName(const TypePtr & pType) const;
	const std::string toTypeInit(const TypePtr & pType) const;
	const std::string toReadFunction(const TypePtr & pType) const;
	
	void createTree(const TypePtr &in_tPtr, CNode & node) const ;
	void createTreeForBuiltin(const BuiltinPtr & bPtr, CNode & thisnode) const;
	void createTreeForEnum(const EnumPtr & ePtr, CNode & thisnode) const;
	void createTreeForStruct(const StructPtr & sPtr, CNode & thisnode) const;
	void createTreeForVector(const VectorPtr & vPtr, CNode & thisnode) const;
	void createTreeForMap(const MapPtr & mPtr, CNode & thisnode) const;

	void createParams(const CNode & thisnode, std::vector<std::pair<std::string, std::pair<std::string, std::string> > > & vec_params) const;

	const std::string generateRead(const CNode & thisnode) const;
	const std::string generateReadVector(const CNode & thisnode, const CNode & firstnode, const std::string & sSize) const;
	const std::string generateWrite(const CNode & thisnode) const;

private:
    std::string s_JCE_PACKAGE;
    std::string s_PROXY_PACKAGE;
    std::string s_HOLDER_PACKAGE;
    std::string s_SERVER_PACKAGE;
    std::string s_WUP_PACKAGE;
};

#endif

