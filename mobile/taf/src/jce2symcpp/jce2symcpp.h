#ifndef _JCE2SYMCPP_H
#define _JCE2SYMCPP_H

#include "parse/parse.h"
#include "servant/BaseF.h"

#include <cassert>
#include <string>

using namespace taf;

/**
 * ����jce����symbain 
 * c�ļ� �����ṹ�ı�����Լ�����Proxy��Servant 
 */
class Jce2Cpp
{
public:

    /**
     * ����
     * @param file
     */
    void createFile(const string &file);

	/**
	* ���������ļ���Ŀ¼
	*
	*/
	void setBaseDir(const std::string & sPath) { m_sBaseDir = sPath; }

	/**
	 * �����Ƿ񴫵�Ĭ��ֵ
	 * 
	 * @author kevintian (2010-10-8)
	 * 
	 * @param bCheck 
	 */
	void setCheckDefault(const bool bCheck) { m_bCheckDefault = bCheck; }
	/**
	 * �����Ƿ񴫵�Ĭ��ֵ
	 * 
	 * @author kevintian (2011-04-20)
	 * 
	 * @param bCheck 
	 */
	void setWithImportC(const bool bWith) { m_bWithImportC = bWith; }


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

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string displaySimple(const TypeIdPtr &pPtr, bool bSep) const;

    //����������������Դ������
protected:
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
    string tostrMap(const MapPtr &pPtr) const;

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

    //������h��cpp�ļ��ľ�������
protected:
    /**
     * �ṹ��md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr &pPtr) const;

    /**
     * ���ɽṹ��ͷ�ļ�����
     * @param pPtr
     * @param namespaceId �����ռ�
     *
     * @return string
     */
    string generateH(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * ���ɲ���������ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ParamDeclPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string generateOutH(const ParamDeclPtr &pPtr) const;



    /**
     * ����ö�ٵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const EnumPtr &pPtr) const;

    /**
     * ���ɳ���ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ConstPtr &pPtr) const;


    /**
     * ���ɽṹ��cpp�ļ�����
     * @param pPtr
     * @param namespaceId �����ռ�
     *
     * @return string
     */
    string generateCpp(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * ����ö�ٵ�cpp�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const EnumPtr &pPtr) const;




    /**
     * 
     * ���ɽӿڱ�������
     * @param pPtr 
     * @param interface 
     */
    void generateCoder(const ContextPtr &pPtr) const;

    string generateCoder(const NamespacePtr &pPtr) const;

    string generateCoder(const InterfacePtr &pPtr) const;

    string generateCoder(const OperationPtr &pPtr) const;

    string generateCoderCpp(const NamespacePtr &pPtr) const;


	std::string m_sBaseDir;


	bool m_bCheckDefault;
	bool m_bWithImportC;
};

#endif


