#ifndef _JCE2AS_H
#define _JCE2AS_H
#include "parse/parse.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"


class Jce2as
{
public:
    /**
     * ���ô������ɵĸ�Ŀ¼
     * @param dir
     */
    void setBaseDir(const string &dir)
    {
        _baseDir = dir;
    }

    /**
     * ���ð�ǰ׺
     * @param prefix
     */
    void setBasePackage(const string &prefix);
	void createFile(const string &file);
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
protected:
    /*
     * ����ĳ���͵ĳ�ʼ���ַ���
     * @param pPtr
     *
     * @return string
     */
    string toTypeInit(const TypePtr &pPtr) const;

    void toFunctionName(const TypeIdPtr & pPtr, const std::string & sAction, std::pair<string, string> & pairFunc) const;

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

protected:
    /**
     * ����ö�ٵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateAS(const EnumPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ���ɽṹ��java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateAS(const StructPtr & pPtr, const NamespacePtr &nPtr) const;

    /**
     * ���ɳ�����ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return
     */
    void generateAS(const ConstPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * �������ֿռ�java�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateAS(const NamespacePtr &pPtr) const;

    /**
     * ����ÿ��jce�ļ���java�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
	void generateAS(const ContextPtr &pPtr) const;


};

#endif

