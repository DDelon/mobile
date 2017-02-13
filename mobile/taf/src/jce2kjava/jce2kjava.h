#ifndef _Jce2KJava_H
#define _Jce2KJava_H

#include "parse/parse.h"

#include <cassert>
#include <string>

#define JCE_PACKAGE     ".taf.jce"
#define PROXY_PACKAGE   ".taf.proxy"
#define HOLDER_PACKAGE  ".taf.holder"
#define SERVER_PACKAGE  ".taf.server"
#define WUP_PACKAGE     ".jce.wup"

/**
 * ����jce����java�ļ�
 * �����ṹ�ı�����Լ�����Proxy��Servant
 */
class Jce2KJava
{
public:
	Jce2KJava();

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
    void createFile(const string &file);

	/**
	 * �����Ƿ���Ĭ��ֵ
	 */
	void setCheckDefault(bool bCheck) 	{ _bCheckDefault = bCheck; }

	/**
	 * �����Ƿ����ɱȽϺ���
	 */
	void setEquals(bool bEquals) 		{ _bEquals = bEquals; }
	

	/**
	 * �����Ƿ���ݽṹ������ƣ���ѡ������readFrom����writeTo����
	 */
	void setByname(bool bByname) 		{ _bByname = bByname; }
	
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

	bool   _bCheckDefault;

	bool   _bEquals;

	bool   _bByname;
    //�����Ǳ�����Դ������
protected:

    /**
     * ����ĳ���͵Ľ���Դ��
     * @param pPtr
     *
     * @return string
     */
    string writeTo(const TypeIdPtr &pPtr) const;

    /**
     * ����ĳ���͵ı���Դ��
     * @param pPtr
     *
     * @return string
     */
    string readFrom(const TypeIdPtr &pPtr) const;

    /**
     * 
     * @param pPtr
     * 
     * @return string
     */
    string display(const TypeIdPtr &pPtr) const;

    //����������������Դ������
protected:

    /*
     * ����ĳ���͵ĳ�ʼ���ַ���
     * @param pPtr
     *
     * @return string
     */
    string toTypeInit(const TypePtr &pPtr) const;

    /*
     * ����ĳ���Ͷ�Ӧ����ĳ�ʼ���ַ���
     * @param pPtr
     *
     * @return string
     */


    string toObjTypeInit(const TypePtr &pPtr) const;

    /**
     * ����ĳ���͵Ķ�Ӧ������ַ�������Դ��
     * @param pPtr
     *
     * @return string
     */
    string toObjStr(const TypePtr &pPtr) const;

    /**
     * �ж��Ƿ��Ƕ�������
     */
    bool isObjType(const TypePtr &pPtr) const;

    /**
     * ����ĳ���͵��ַ�������Դ��
     * @param pPtr
     *
     * @return string
     */
    string tostr(const TypePtr &pPtr) const;

    /**
     * �����ڽ����͵��ַ���Դ��
     * @param pPtr
     *
     * @return string
     */
    string tostrBuiltin(const BuiltinPtr &pPtr) const;
    /**
     * ����vector���ַ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrVector(const VectorPtr &pPtr) const;

    /**
     * ����map���ַ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrMap(const MapPtr &pPtr, bool bNew = false) const;

    /**
     * ����ĳ�ֽṹ�ķ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr &pPtr) const;

    /**
     * ����ĳ��ö�ٵķ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrEnum(const EnumPtr &pPtr) const;

    /**
     * �������ͱ����Ľ���Դ��
     * @param pPtr
     *
     * @return string
     */
    string decode(const TypeIdPtr &pPtr) const;

    /**
     * �������ͱ����ı���Դ��
     * @param pPtr
     *
     * @return string
     */
    string encode(const TypeIdPtr &pPtr) const;

    //������h��java�ļ��ľ�������
protected:
    /**
     * �ṹ��md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr &pPtr) const;

    /**
     * ���ɽṹ��Holder�࣬�������ô���
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateHolder(const StructPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Ĭ��Ԫ������ʶ��map/list����
     * @param pPtr
     * @param sElemName Ԫ������
     *
     * @return string
     */
    string generateDefautElem(const TypePtr &pPtr, const string & sElemName) const;

    /**
     * ���ɽṹ��java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const StructPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����������javaԴ��
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const ContainerPtr &pPtr) const;

    /**
     * ���ɲ���������java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const ParamDeclPtr &pPtr) const;

    /**
     * ���ɲ���holder��java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateHolder(const ParamDeclPtr &pPtr) const;

    /**
     * ������������proxy��java�ļ�����
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateJava(const OperationPtr &pPtr, const string &cn) const;

    /**
     * ���ɲ���java�ļ��������÷ַ���Դ��
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateDispatchJava(const OperationPtr &pPtr, const string &cn) const;


    /**
     * ���ɽӿڵ�java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateJava(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Proxy�ӿڵ�java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrx(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Proxy�������java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxHelper(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Proxy�ص����java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxCallback(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ���ɷ��������java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateServant(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����ö�ٵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateJava(const EnumPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ���ɳ���javaԴ��
     * @param pPtr
     * 
     * @return string
     */
    string generateJava(const ConstPtr &pPtr, const NamespacePtr &nPtr) const;

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
	
	
	void dependCompute(const StructPtr & sPtr, int flag) const;

	/**
	 * ����ṹ��������ϵ�Ľṹ�� 
	 * */ 
	//0x01: ����readFrom;response����
	//0x02������writeTo ;request����

	mutable std::map<std::string, int> _mapDepend;
    
private:
    std::string s_JCE_PACKAGE;
    std::string s_PROXY_PACKAGE;
    std::string s_HOLDER_PACKAGE;
    std::string s_SERVER_PACKAGE;
    std::string s_WUP_PACKAGE;
};

#endif


