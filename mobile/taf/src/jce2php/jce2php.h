#ifndef _JCE2PHP_H
#define _JCE2PHP_H

#include "parse/parse.h"
//#include "servant/BaseF.h"

#include <cassert>
#include <string>

using namespace taf;

/**
 * ����jce����c++�ļ�
 * �����ṹ�ı�����Լ�����Proxy��Servant
 */
class Jce2Php
{
public:
	Jce2Php();

    /**
     * ����
     * @param file
     */
    void createFile(const string &file, const vector<string> &vsCoder);


    /**
    * ���������ļ���Ŀ¼
    *
    */
    void setBaseDir(const std::string & sPath) { m_sBaseDir = sPath; }

	/**
    * �����Ƿ��׳��쳣
    *
    */
	void setPDU(bool bPDU) { m_bPDU = bPDU; }

	/**
	 * �����Ƿ���Ҫ���Ĭ��ֵ
	 */
	void setCheckDefault(bool bCheck) { m_bCheckDefault = bCheck; }

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
    string readFrom(const TypeIdPtr &pPtr, bool bIsRequire = true) const;


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
     * ��ȡ�������������
     * @param pPtr
     *
     * @return string
     */

    string toStrSuffix(const TypeIdPtr &pPtr) const;

    /**
     * ��ȡ�������������
     * @param pPtr
     *
     * @return int
     */
    int getSuffix(const TypeIdPtr &pPtr) const;

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
     * ���ɽṹ��ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generatePHP(const StructPtr &pPtr, const string& namespaceId) const;


    /**
     * �������ֿռ��ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generatePHP(const NamespacePtr &pPtr) const;


    /**
     * �������ֿռ��ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generatePHP(const ContextPtr &pPtr) const;

    void generatePHP_Pdu(const ContextPtr &pPtr) const;

private:
    std::string m_sBaseDir;

	bool m_bPDU;
	bool m_bCheckDefault;
};

#endif


