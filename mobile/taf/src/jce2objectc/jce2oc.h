#ifndef _JCE2OC_H
#define _JCE2OC_H

#include "parse/parse.h"

#include <cassert>
#include <string>

using namespace taf;

/**
 * ����jce����h��m�ļ�
 */
class Jce2OC
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
	 * �����Ƿ���Ĭ��ֵ
	 *
	 * @param bCheck
	 */
	void setArc(const bool bArc) { m_bARC = bArc; }
	/**
	 * ���ø�����Ա�����Ƿ�������ֿռ�ǰ׺
	 */
	void setNeedNS(bool bNeed) { m_bNeedNS = bNeed;}
    /**
	 * �����û��Զ������ֿռ�
	 */
	void setNS(const string& sNS) {m_sNamespace = sNS;}
    //����������������Դ������
protected:
	/**
	 * ��ȡö�ٻ��߽ṹ������ǰ׺�ַ���
	 * @param sJceNS  jce�ļ��ж�������ֿռ�
	 * @param bEnum �Ƿ���ö������
	 *
	 * @return ������ǰ׺�ַ���
	 */
	string getNamePrix(const string& sJceNS) const;
	
	/**
	 * ��ȡ���Ϲ淶�ĳ�Ա����
	 */
	string getPropertyName(const string& sId) const;

    /**
	 *
	 * ��ȡ�Զ���setter������
	 */
	string getSetterName(const string& sId) const;
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
	 * ����OBJC �汾2�����Զ���
	 * @param pPtr
	 *
	 * @return string
	 */
	string toJceV2Procstr(const TypeIdPtr &pPtr) const;

	/**
	 * ����OBJC�汾2�ĸ�����Ϣ����
	 * @param pPtr
	 *
	 * @return string
	 */
	string toAddtionalClassName(const TypePtr &pPtr) const;

	/**
	 * ���ɽṹ�����Ե�synthesize����
	 * @param vMember
	 *
	 * @return string
	 */
    string writesynthesize(const vector<TypeIdPtr>& vMember) const;

	/**
	 * ����Initialize����
	 * @param pPtr
	 *
	 * @return string
	 */
    string writeInitialize(const StructPtr &pPtr) const;

	/**
	 * ���ɳ�ʼ�����庯��init
	 * @param vMember
	 *
	 * @return string
	 */
	string writeInit(const vector<TypeIdPtr>& vMember) const;

	/**
	 * ������������
	 * @param vMember
	 *
	 * @return string
	 */
	string writedealloc(const vector<TypeIdPtr>& vMember) const;

	/**
	 * ���ɽṹ���ƶ���
	 * @param pPtr
	 *
	 * @return string
	 */
	string writeJceType(const StructPtr &pPtr) const;

	/**
	 * �������ͱ�ʾ��
	 * @param pPtr
	 *
	 * @return string
	 */
    string toClassName(const TypePtr &pPtr) const;
protected:
    /**
	 * ��ȡjce�ж�����������ͣ�require��optional
	 * @param pPtr
	 *
	 */
	string getReqOptionType(const TypeIdPtr &pPtr) const;
	/**
	 * �ж������Ƿ�����retain���ͣ�string�Ͷ������Ͷ�����retain
	 * @param pPtr
	 *
	 */
	bool IsRetainType(const TypePtr &pPtr) const;
	/**
	 * ��ȡ�ṹ���������������͵ı�ʶ�����ƺ����ͣ�Ŀǰ�ǽṹ���ö����������
	 *
	 * @param pPtr
	 * @param namespaceId
	 *
	 * @return
	 */
	map<string,int> getReferences(const StructPtr &pPtr, const string& namespaceId="") const;

	/**
	 * ��ȡ��Ա�������������ͱ�ʶ������,
	 * ����, stHello hello;stHelloΪ���ͱ�ʶ������,����Ϊ�ṹ��
	 *
	 * @param pPtr
	 * @param mRefs
	 */
	void toIncludeName(const TypePtr &pPtr,map<string,int>& mRefs) const;

    /**
     * ���ɽṹ��ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateH(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * ���ɽṹ��m�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateM(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * ����ö�ٵ�m�ļ�����
     * @param pPtr
     *
     * @return string
     */
	string generateM(const EnumPtr &pPtr, const string& namespaceId) const;
    /**
     * ����������ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ContainerPtr &pPtr) const;

    /**
     * ����ö�ٵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const EnumPtr &pPtr, const string& namespaceId) const;

    /**
     * �������ֿռ��ͷ�ļ���ʵ��Դ��
     * @param pPtr
     *
     * @return string
     */
    string generate(const NamespacePtr &pPtr) const;

    /**
     * �������ֿռ�m�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateM(const NamespacePtr &pPtr) const;

    /**
     * �������ֿռ��ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generate(const ContextPtr &pPtr) const;

    /**
     * �������ֿռ�m�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateM(const ContextPtr &pPtr) const;

private:
	enum EM_TYPE
	{
		EM_STRUCT_TYPE = 1,
		EM_ENUM_TYPE = 2
	};
    std::string m_sBaseDir;

    //<modulename,EM_TYPE>, 2:enum,1,stuct;
    mutable map<string,int> m_mIncludes;
	/**
	 * �Ƿ�֧��arc�汾
	 *
	 */
	bool m_bARC;
	/**
	 * �Ƿ�֧��������ֿռ�ǰ׺
	 */
	bool m_bNeedNS;
	/**
	 *�û��Զ�������ֿռ�ǰ׺�������ֵΪ�գ�Ĭ����ȡjce�ļ��ж����ֵ
	 */
	string m_sNamespace;
};

#endif


