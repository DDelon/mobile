#ifndef _ELEMENT_H
#define _ELEMENT_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <stack>
#include <sstream>
#include <errno.h>
#include "util/tc_autoptr.h"

using namespace std;

#define YYSTYPE GrammarBasePtr

void yyerror(char const *msg);

/**
 * �﷨Ԫ�ػ���
 */
class GrammarBase : virtual public taf::TC_HandleBase
{
public:
    virtual ~GrammarBase(){};
};

typedef taf::TC_AutoPtr<GrammarBase> GrammarBasePtr;

/**
 * ���������е�������
 */
class StringTok : public GrammarBase
{
public:

    StringTok() { }
    string v;
};

typedef taf::TC_AutoPtr<StringTok> StringTokPtr;

/**
 * ���������е�������
 */
class IntegerTok : public GrammarBase
{
public:

    IntegerTok():v(0) { }
    long long v;
};

typedef taf::TC_AutoPtr<IntegerTok> IntegerTokPtr;

/**
 * ���������еĸ�����
 */
class FloatTok : public GrammarBase
{
public:

    FloatTok():v(0.0f) { }
    double v;
};

typedef taf::TC_AutoPtr<FloatTok> FloatTokPtr;

/**
 * ���������е�bool����
 */
class BoolTok : public GrammarBase
{
public:

    BoolTok():v(true) { }
    bool v;
};

typedef taf::TC_AutoPtr<BoolTok> BoolTokPtr;

/**
 * ���������е���ֵ����
 */
class ConstTok : public GrammarBase
{
public:

    ConstTok():t(VALUE) { }

    enum
    {
        VALUE,
        STRING,
        BOOL,
    };

    int    t;
    string v;
};

typedef taf::TC_AutoPtr<ConstTok> ConstTokPtr;

///////////////////////////////////////////////

/**
 * ���ͻ���
 */
class Type : virtual public GrammarBase
{
public:
    virtual bool isSimple() const { return false;}
};

typedef taf::TC_AutoPtr<Type> TypePtr;

/**
 * �ڽ�����
 */
class Builtin : public Type
{
public:

    enum Kind
    {
        KindVoid,
        KindBool,
        KindByte,
        KindShort,
        KindInt,
        KindLong,
        KindFloat,
        KindDouble,
        KindString,
        KindBinary,
        KindVector,
        KindMap
    };

    /**
     * ���캯��
     * @param kind
     */
    Builtin(Kind kind);

    /**
     * ����
     *
     * @return Kind
     */
    Kind kind() const;

    /**
     * �Ƿ��Ǽ�����
     *
     * @return bool
     */
    bool isSimple() const;

    /**
     * �ַ�����ʾ
     */
    static const char* builtinTable[];

protected:

    Kind _kind;
};

typedef taf::TC_AutoPtr<Builtin> BuiltinPtr;

/**
 * Vector����
 */
class Vector : public Type
{
public:
    /**
     * ���캯��
     * @param ptr
     */
    Vector(const TypePtr& ptr);

    /**
     * ��ȡ����
     *
     * @return TypePtr&
     */
	TypePtr& getTypePtr() {return _ptr;}
protected:
    TypePtr _ptr;
};

typedef taf::TC_AutoPtr<Vector> VectorPtr;

/**
 * Map����
 */
class Map : public Type
{
public:
    /**
     * ���캯��
     * @param pleft
     * @param pright
     */
    Map(const TypePtr& pleft, const TypePtr& pright);

    /**
     * ��ȡ������
     *
     * @return TypePtr&
     */
	TypePtr& getLeftTypePtr() {return _pleft;}

    /**
     * ��ȡ������
     *
     * @return TypePtr&
     */
	TypePtr& getRightTypePtr() {return _pright;}

protected:
    TypePtr _pleft;
    TypePtr _pright;
};

typedef taf::TC_AutoPtr<Map> MapPtr;

/**
 * ��������
 */
class TypeId : public GrammarBase
{
public:
    /**
     * ���캯��
     * @param ptr
     * @param id
     */
    TypeId(const TypePtr& ptr, const string& id);

    /**
     * ��������
     *
     * @return string
     */
    string getId() const { return _id; }

    /**
     * ��������
     *
     * @return Type*
     */
    TypePtr& getTypePtr() { return _ptr;}

protected:
    TypePtr _ptr;
    string  _id;
};

typedef taf::TC_AutoPtr<TypeId> TypeIdPtr;


////////////////////////////////////////////////////
//
class Namespace;
typedef taf::TC_AutoPtr<Namespace> NamespacePtr;
/**
 * ��������, ���п��԰�������Ԫ�ص�Ԫ�ض��Ӹû���̳�
 */
class Container : virtual public GrammarBase
{
public:
    /**
     * ���캯��
     * @param id
     */
    Container(const string &id) : _id(id)
    {
    }

    /**
     * �������ֿռ�
     * @param id
     *
     * @return NamespacePtr
     */
    NamespacePtr createNamespace(const string &id);

    /**
     * ��ȡID
     *
     * @return string
     */
    string getId() const { return _id;}

    /**
     * ��ȡ���е����ֿռ�
     *
     * @return vector<NamespacePtr>&
     */
    vector<NamespacePtr> &getAllNamespacePtr() { return _ns; }
protected:
    string                  _id;
    vector<NamespacePtr>    _ns;
};

typedef taf::TC_AutoPtr<Container> ContainerPtr;

////////////////////////////////////////////////////
//
class Const : public GrammarBase
{
public:
    /**
     *
     * @param tPtr
     * @param cPtr
     */
    Const(TypeIdPtr &tPtr, ConstTokPtr &cPtr) : _tPtr(tPtr), _cPtr(cPtr)
    {
    }

    /**
     *
     *
     * @return TypePtr&
     */
    TypeIdPtr &getTypeIdPtr()       { return _tPtr; }

    /**
     *
     *
     * @return ConstTokPtr
     */
    ConstTokPtr getConstTokPtr()    { return _cPtr; }

protected:
    TypeIdPtr   _tPtr;
    ConstTokPtr _cPtr;
};

typedef taf::TC_AutoPtr<Const> ConstPtr;

////////////////////////////////////////////////////
//
/**
 * �ṹ
 */
class Struct : virtual public Container, virtual public Type
{
public:
    /**
     * ���캯��
     * @param id
     */
    Struct(const string& id, const string &sid) : Container(id), _sid(sid)
    {
    }

    /**
     * ��ӳ�Ա����
     * @param ptid
     */
    void addTypeId(const TypeIdPtr &ptr);

    /**
     * ��ȡ���еĳ�Ա����
     *
     * @return vector<TypeIdPtr>&
     */
    vector<TypeIdPtr>& getAllMemberPtr() {return _members;}

    /**
     * ��ȡ�ṹ������
     *
     * @return string
     */
    string getSid() const { return _sid; }

protected:
    vector<TypeIdPtr>   _members;
    string              _sid;
};

typedef taf::TC_AutoPtr<Struct> StructPtr;

////////////////////////////////////////////////////
//
/**
 * ��������
 */
class ParamDecl : public GrammarBase
{
public:
    /**
     * ����
     * @param typeIdPtr
     * @param v
     */
    ParamDecl(const TypeIdPtr &typeIdPtr) : _typeIdPtr(typeIdPtr)
    {
    }

    /**
     * ��������
     *
     * @return TypeIdPtr&
     */
    TypeIdPtr& getTypeIdPtr() { return _typeIdPtr; }

protected:
    TypeIdPtr   _typeIdPtr;
};
typedef taf::TC_AutoPtr<ParamDecl> ParamDeclPtr;

///////////////////////////////////////////////////////
//
/**
 * ������
 */
class Operation : public Container
{
public:
    /**
     * ���캯��
     * @param id
     * @param typePtr
     */
    Operation(const string &id, const TypePtr &typePtr) : Container(id), _typePtr(typePtr)
    {
    }

    /**
     * ����һ������
     * @param typeIdPtr
     *
     * @return ParamDeclPtr
     */
    ParamDeclPtr createParamDecl(const TypeIdPtr &typeIdPtr);

    /**
     * ��ȡ����ֵ����
     *
     * @return TypePtr&
     */
	TypePtr &getTypePtr() { return _typePtr; }

    /**
     * ��ȡ���в���
     *
     * @return vector<ParamDeclPtr>&
     */
	vector<ParamDeclPtr> &getAllParamDeclPtr() { return _ps; }
protected:
    TypePtr _typePtr;
    vector<ParamDeclPtr>    _ps;
};

typedef taf::TC_AutoPtr<Operation> OperationPtr;
///////////////////////////////////////////////////////
//
/**
 * �ӿ�����
 */
class Interface : public Container
{
public:
    /**
     * ����
     * @param id
     */
    Interface(const string &id) : Container(id)
    {

    }

    /**
     * ����һ������
     * @param id
     * @param typePtr
     *
     * @return OperationPtr
     */
    OperationPtr createOperation(const string &id, const TypePtr &typePtr);

    /**
     * ��ȡ���в���
     *
     * @return vector<OperationPtr>&
     */
	vector<OperationPtr> &getAllOperationPtr() { return _ops; }
protected:
    vector<OperationPtr>    _ops;
};

typedef taf::TC_AutoPtr<Interface> InterfacePtr;

/////////////////////////////////////////////////////////
//
/**
 * ���ֿռ�
 */
class Namespace : public Container
{
public:
    /**
     * ���캯��
     * @param id
     */
    Namespace(const string &id) : Container(id)
    {
    }

    /**
     * ���ɽӿ�
     * @param id
     *
     * @return InterfacePtr
     */
    InterfacePtr createInterface(const string &id);

    /**
     * ���ɽṹ
     * @param id
     *
     * @return StructPtr
     */
    StructPtr createStruct(const string& id);

    /**
     *
     * @param pPtr
     * @param cPtr
     *
     * @return ConstPtr
     */
    ConstPtr createConst(TypeIdPtr &pPtr, ConstTokPtr &cPtr);

    /**
     * �Ƿ��нӿ�
     *
     * @return bool
     */
    bool hasInterface() const { return !_is.empty(); }

    /**
     * ��ȡ���еĽӿ�
     *
     * @return vector<InterfacePtr>&
     */
	vector<InterfacePtr> &getAllInterfacePtr() { return _is; }

    /**
     * ��ȡ���еĽṹ
     *
     * @return vector<StructPtr>&
     */
	vector<StructPtr> &getAllStructPtr() { return _ss; }

    /**
     * ��������
     *
     * @return vector<ConstPtr>&
     */
    vector<ConstPtr> &getAllConstPtr()     { return _cs; }

protected:
    vector<InterfacePtr>    _is;
    vector<StructPtr>       _ss;
    vector<ConstPtr>        _cs;
};

/////////////////////////////////////////////////////////////////
/**
 * ������
 */
class Context : public taf::TC_HandleBase
{
public:
    /**
     * ���캯��
     */
    Context(const string &file) : _currline(1), _filename(file)
    {
    }

    /**
     * ��һ��
     */
    void nextLine() { _currline++; }

    /**
     * Ŀǰ����
     *
     * @return size_t
     */
    size_t getCurrLine() { return _currline; }

    /**
     * ��ǰ�ļ���
     *
     * @return string
     */
    string getFileName() { return _filename; }

    /**
     * ���include���ļ�
     * @param incl
     */
    void addInclude(const string &incl);

    /**
     * �����������ļ������ֿռ�
     * @param c
     */
    void addNamespacePtr(const NamespacePtr &c)
    {
        _namespaces.push_back(c);
    }

    /**
     * ��ȡincludes���ļ�
     *
     * @return vector<string>
     */
    vector<string> getIncludes() { return _includes; }

    /**
     * ��ȡ���ֿռ�
     *
     * @return vector<NamespacePtr>
     */
    vector<NamespacePtr> getNamespaces() { return _namespaces; }

protected:
    size_t                  _currline;
    string                  _filename;
    vector<string>          _includes;
    vector<NamespacePtr>    _namespaces;
};

typedef taf::TC_AutoPtr<Context> ContextPtr;

#endif

