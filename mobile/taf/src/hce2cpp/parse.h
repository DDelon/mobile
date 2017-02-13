#ifndef JCE_DRIVER_H
#define JCE_DRIVER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <stack>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include "element.h"

using namespace std;

/**
 * Hce�ļ�������
 *
 */
class HceParse : public taf::TC_HandleBase
{
public:
    /**
     * ���캯��
     */
    HceParse();

    /**
     * ����ĳһ���ļ�
     * @param sFileName
     */
    void parse(const string &sFileName);

    /**
     * ������ʾ
     * @param msg
     */
    void error(const string &msg);

    /**
     * ���ؼ���
     * @param s
     * 
     * @return int
     */
    int  checkKeyword(const string &s);

    /**
     * ��һ��
     */
    void nextLine();

    /**
     * Ŀǰ�������ļ�����
     * 
     * @return string
     */
	string getCurrFileName();

    /**
     * tab
     * 
     * @return string
     */
    string getTab();

    /**
     * ����tab��
     */
    void incTab() { _itab++; }

    /**
     * ����tab��
     */
    void delTab() { _itab--; }

    /**
     * �����ļ�
     * @param file
     */
    void pushFile(const string &file);

    /**
     * ���������ļ�
     */
    ContextPtr popFile();

    /**
     * ��ȡ���е�������
     * 
     * @return std::vector<ContextPtr>
     */
    std::vector<ContextPtr> getContexts() { return _vcontexts; }

    /**
     * ��ȡĿǰ������
     * 
     * @return ContainerPtr
     */
    ContainerPtr currentContainer();

    /**
     * push����
     * @param c
     */
    void pushContainer(const ContainerPtr &c);

    /**
     * Ŀǰ��������
     * 
     * @return ContextPtr
     */
    ContextPtr currentContextPtr();

    /**
     * ��������
     * 
     * @return ContainerPtr
     */
    ContainerPtr popContainer();

    /**
     * ����BuiltinԪ��
     * @param kind
     * 
     * @return BuiltinPtr
     */
    BuiltinPtr createBuiltin(Builtin::Kind kind);

    /**
     * ����VectorԪ��
     * @param ptr
     * 
     * @return VectorPtr
     */
    VectorPtr createVector(const TypePtr &ptr);

    /**
     * ����MapԪ��
     * @param pleft
     * @param pright
     * 
     * @return MapPtr
     */
    MapPtr createMap(const TypePtr &pleft, const TypePtr &pright);

    /**
     * ��ӽṹԪ��
     * @param sPtr
     */
    void addStructPtr(const StructPtr &sPtr);

    /**
     * ���ҽṹ
     * @param id
     * 
     * @return StructPtr
     */
    StructPtr findStruct(const string &sid);

    /**
     * ����ͻ
     * @param id
     */
    void checkConflict(const string &sid);

    /**
     * �����Զ�������
     * @param sid
     * 
     * @return TypePtr
     */
    TypePtr findUserType(const string &sid);

    /**
     * �������ֿռ�
     * @param id
     * 
     * @return NamespacePtr
     */
    NamespacePtr findNamespace(const string &id);

    /**
     * Ŀǰ�����ֿռ�
     * 
     * @return NamespacePtr
     */
    NamespacePtr currentNamespace();

    /**
     * ��鳣�����ͺ�ֵ�Ƿ�һ��
     * @param c
     * @param b
     */
    void checkConstValue(TypeIdPtr &tPtr, int b);

    /**
     * ��ȡ�ļ���
     * @param s
     * 
     * @return string
     */
    bool getFilePath(const string &s, string &file);

protected:
    /**
     * ������ֿռ�
     * @param nPtr
     */
    void addNamespacePtr(const NamespacePtr &nPtr);

    /**
     * ��ʼ��
     */
    void initScanner();

    /**
     * ���
     */
    void clear();

protected:
    std::map<std::string, int>      _keywordMap;
    int                             _itab;
    std::stack<ContextPtr>          _contexts;
    std::stack<ContainerPtr>        _contains;
    std::vector<ContextPtr>         _vcontexts;
    std::vector<StructPtr>          _structs;
    std::vector<NamespacePtr>       _namespaces;
};

extern int yyparse();
extern int yylex();
extern FILE *yyin, *yyout;

typedef taf::TC_AutoPtr<HceParse> HceParsePtr;

extern HceParsePtr g_parse;

#endif

