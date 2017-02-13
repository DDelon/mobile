#ifndef _JCEAnalyzer_H
#define _JCEAnalyzer_H

#include "parse/parse.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <arpa/inet.h>
#include "jce/Jce.h"

using namespace taf;

#define chars_one_line 16
#define chars_buf_reserve 1



#define CHAR_WIDTH_TAG 2
#define CHAR_WIDTH_ID  12
#define CHAR_WIDTH_TYPE 23

#ifndef JCE_MAX_STRING_LENGTH
#define JCE_MAX_STRING_LENGTH   (100 * 1024 * 1024)
#endif

class JceAnalyzer
{
public:
    JceAnalyzer()
    {
		_startPos = 0;
        _itab = 0;
		_buf_len = 0;
		_buf = NULL;
		_bWup = false;
		_bIsDumpWup = false;
		_wupVersion = 2;
		_wup2_helper.clear();
		_wup3_helper.clear();
	}
    /**
     * ���ô������Ľṹ����
     * @param name
     */
    void setStructName(const string &name)
    {
        _StructName = name;
    }
	/**
	 *wup���ʱput��������,eg.UniPacket<> pack; pack.put<xx>(name,xxx);
	 *@param name
	 */
	void setPutName(const string &name)
    {
        _putName = name;
    }
	/**
	 * ���ô����������������ļ���
	 *@param sFile
	 */
    void setStreamFileName(const string &sFile)
    {
        _streamFileName = sFile;
    }

	void setStartPos(int p)
    {
        _startPos = p;
    }

	bool isWup()
	{
		return _bWup;
	}
	void setWup(bool b)
	{
		_bWup = b;
	}

	enum
	{
		eOK = 0,
		eNeedBackDataHead = 1,
		eNeedSkipTag = 2
	};

protected:
    /**
     * ���������ռ��ȡ�ļ�·��
     * @param ns �����ռ�
     *
     * @return string
     */
    string getFilePath(const string &ns) const
    {
        return  "/" + taf::TC_Common::replace(_packagePrefix, ".", "/") + "/" + ns + "/";
    }

    /**
     * ����ĳ���͵��ַ�������Դ��
     * @param pPtr
     *
     * @return string
     */
    string tostr(const TypePtr &pPtr) ;

    /**
     * �����ڽ����͵��ַ�������Դ��,eg,int,short,float,double
     * @param pPtr
     *
     * @return string
     */
    string tostrBuiltin(const BuiltinPtr &pPtr) ;
    /**
     * ����vector���ַ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrVector(const VectorPtr &pPtr) ;

    /**
     * ����map���ַ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrMap(const MapPtr &pPtr, bool bNew = false) ;

    /**
     * ����ĳ�ֽṹ�ķ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr &pPtr) const;
	string tostrEnum(const EnumPtr &pPtr) ;

protected:
    /**
     * �ӱ���buffer�ж�ȡstruct����
     * @param pPtr
     *
     * @return string
     */

	int readStruct(const StructPtr & pPtr, const NamespacePtr & nPtr) ;

    /**
     * �������ֿռ������Ӧ��struct
     * @param pPtr
     *
     * @return string
     */
    void analyzeNS(const NamespacePtr &pPtr)
    {
        vector<StructPtr>& vStruct = pPtr->getAllStructPtr();
		size_t i;
        for ( i= 0; i < vStruct.size(); i++)
        {
            if (vStruct[i]->getId() == _StructName || _bWup)
            {
            	readStruct(vStruct[i], pPtr);
				break;
            }
        }
		if (i>= vStruct.size())
		{
			cout<< "not find  structName: " << _StructName << endl;
		}
    }

    /**
     * ����ÿ��jce�ļ���java�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void analyzeContext(const ContextPtr &pPtr)
    {
        vector<NamespacePtr> namespaces = pPtr->getNamespaces();

        for(size_t i = 0; i < namespaces.size(); i++)
        {
            analyzeNS(namespaces[i]);
        }
    }

public:

	void skip(size_t len);
	void skipToStructEnd();

	void skipField();


/// ����һ���ֶΣ�������ͷ��Ϣ
	void skipField(uint8_t type);
	void backToLast();
	void peekBuf(void * buf, size_t len, size_t offset = 0);
	void readBuf(void * buf, size_t len);

	template<typename T>
    T readByType();

	void read(double& n, uint8_t expecttag, int& realType, bool isRequire);

	void read(Int32& n, uint8_t& tag, int& type, bool isRequire = true);

	void read(Int64& n, uint8_t& tag, int& type, bool isRequire = true);

	void read(string& s, uint8_t&	tag, int& type, bool isRequire = true );

	template<typename K, typename V, typename Cmp, typename Alloc>
	void read(std::map<K, V, Cmp, Alloc>& m, uint8_t tag, bool isRequire = true);


	template<typename Alloc>
	void read(std::vector<char, Alloc>& v,  uint8_t&  tag, int & realType, bool isRequire = true);

	int readBuiltin(const BuiltinPtr &pPtr, int tag, bool isRequire,  TypeIdPtr pType) ;

	int readType(const TypePtr &pPtr, int tag, bool isRequire);

	int readVector(const VectorPtr &pPtr, int tag,  bool isRequire, TypeIdPtr pType);

	int readMap(MapPtr pType, int tag, bool isRequire);
	int readEnum(EnumPtr pPtr, int expectTag, bool isRequire);
	void analyzeFile(const string &file, string streamFile);

	/**
	 *����wup���ݰ��е�sBuffer����
	 */
	void analyzeWithWupBuffer(const string &file, const string& putName);
	void checkWupBuffer(void);
	int compareTag(int tag, int expectTag, bool bIsRequire = true);

	string getTab();
	string getSep();
	/**
	 * ����tab��
	 */
	void incTab() ;

	/**
	 * ����tab��
	 */
	void decTab() ;

	void  addTab(int count) ;
	void eraseTab(int count) ;
	void setBuffer(const char * buf, size_t len)
	{
		_buf = buf;
		_buf_len = len +chars_buf_reserve; //+1��Ŀ���Ǽ���û�нṹ������־�ֽ�
		_cur = 0;
		_last_pos =0;
	}
	/**
	 * ���wup��������
	 */
	void setDumpWup(bool isDump){_bIsDumpWup = isDump;}
	void dumpWup(const map<string, map<string, vector<char> > >& wupData);
	void dumpWup(const map<string, vector<char> >& wupData);

private:
    string _packagePrefix;
    string _StructName;
    string _putName;  // wup put �ӿڴ��˵�name
	string _streamFileName;

	//��ʼ����λ��
	int    _startPos;
	int _itab;

	const char *		_buf;		///< ������
	size_t				_buf_len;	///< ����������

	size_t				_cur;		///< ��ǰλ��

	size_t				_last_pos;		///< ��ǰλ��
	size_t              _wup_sbuffer_pos;

	//�Ƿ��ڽ���wup���ݰ�
	bool _bWup;
	bool _bIsDumpWup;

	//wup�İ汾�ţ�2����3
	short _wupVersion;

	//wup �汾2��Ӧ����������
	map<string, map<string, vector<char> > > _wup2_helper;

	//wup �汾3��Ӧ����������
	map<string, vector<char> >  _wup3_helper;

};

class AnalyzerDataHead
{

public:
	uint8_t _type;
	uint8_t _tag;
	enum
	{
		eChar = 0,
		eShort = 1,
		eInt32 = 2,
		eInt64 = 3,
		eFloat = 4,
		eDouble = 5,
		eString1 = 6,
		eString4 = 7,
		eMap = 8,
		eList = 9,
		eStructBegin = 10,
		eStructEnd = 11,
		eZeroTag = 12,
		eSimpleList = 13,
	};

	static string tostrType(int type)
	{
		string s;

		switch(type)
		{
		case eChar:
			s =  " char ";
			break;
		case eShort:
			s =  " short ";
			break;
		case eInt32:
			s =  " int ";
			break;
		case eInt64:
			s =  " int64 ";
			break;
		case eFloat:
			s =  " float ";
			break;
		case eDouble:
			s =  " double ";
			break;
		case eString1:
			s =  " string1 ";
			break;
		case eString4:
			s =  " string4 ";
			break;
		case eMap:
			s =  " map ";
			break;
		case eList:
			s =  " vector ";
			break;
		case eStructBegin:
			s =  " struct beg ";
			break;
		case eStructEnd:
			s =  " struct end ";
			break;
		case eZeroTag:
			s =  " zero ";
			break;
		case eSimpleList:
			s =  " vector byte ";
			break;

		default:
			cout << "ERROR, tostrType failed!" << type << endl;
			exit(-1);
			break;

		}
		return s;

	}

	struct helper
	{
		unsigned int	type : 4;
		unsigned int	tag : 4;
	}__attribute__((packed));

public:
	AnalyzerDataHead() : _type(0), _tag(0) {}
	AnalyzerDataHead(uint8_t type, uint8_t tag) : _type(type), _tag(tag) {}

	uint8_t getTag() const		{ return _tag;}
	void setTag(uint8_t t)		{ _tag = t;}
	uint8_t getType() const 	{ return _type;}
	void setType(uint8_t t) 	{ _type = t;}

	/// ��ȡ����ͷ��Ϣ
	void readFrom(JceAnalyzer& is)
	{
		size_t n = peekFrom(is);
		is.skip(n);
	}

	/// ��ȡͷ��Ϣ������ǰ������ƫ����
	size_t peekFrom(JceAnalyzer& is)
	{
		helper h;
		size_t n = sizeof(h);
		is.peekBuf(&h, sizeof(h));
		_type = h.type;
		if (h.tag == 15)
		{
			is.peekBuf(&_tag, sizeof(_tag), sizeof(h));
			n += sizeof(_tag);
		}
		else
		{
			_tag = h.tag;
		}
		return n;
	}



};




#endif

