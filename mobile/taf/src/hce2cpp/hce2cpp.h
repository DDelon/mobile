#ifndef _HCE2HESSIAN_H
#define _HCE2HESSIAN_H

#include "parse.h"

#include <cassert>
#include <string>

/**
 * ����hce����c++�ļ�
 */
class Hce2Cpp
{
public:

    /**
     * ����Դ�ļ�
     * @param file
     */
    void createFile(const string &file);

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
     * �ṹ��md5
     * @param pPtr
     *
     * @return string
     */
	string MD5(const StructPtr &pPtr) const;

    //����ͷ�ļ���cpp�ļ�����
protected:
    /**
     * ���ɽṹ��ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
	string generateH(const StructPtr &pPtr) const;

    /**
     * ���ɽṹ��cpp�ļ�����
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const StructPtr &pPtr) const;

    /**
     * ����������ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
	string generateH(const ContainerPtr &pPtr) const;

    /**
     * ����������cppԴ��
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const ContainerPtr &pPtr) const;

    /**
     * ���ɲ���������ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
	string generateH(const ParamDeclPtr &pPtr) const;

    /**
     * ���ɲ���������cpp�ļ�����
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const ParamDeclPtr &pPtr) const;

    /**
     * ������������proxy��cpp�ļ�����
     * @param pPtr
     * @param cn
     *
     * @return string
     */
	string generateCpp(const OperationPtr &pPtr, const string &cn) const;

    /**
     * ���ɲ���ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
	string generateH(const OperationPtr &pPtr) const;

    /**
     * ���ɽӿڵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
	string generateH(const InterfacePtr &pPtr) const;

    /**
     * ���Ľӿڵ�cpp�ļ���Դ��
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const InterfacePtr &pPtr) const;

    /**
     * �������ֿռ��ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
	string generateH(const NamespacePtr &pPtr) const;

    /**
     * �������ֿռ�cpp�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const NamespacePtr &pPtr) const;

    /**
     * �������ֿռ��ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
	void generateH(const ContextPtr &pPtr) const;

    /**
     * �������ֿռ�cpp�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
	void generateCpp(const ContextPtr &pPtr) const;
};

#endif

