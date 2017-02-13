#include "jceAnalyzer.h"
#include <string.h>

using namespace  std;


/// ��ȡһ��ָ�����͵����ݣ��������ͣ�
template<typename T>
T JceAnalyzer::readByType()
{
	T n;
	readBuf(&n, sizeof(n));
	return n;
}

void JceAnalyzer::analyzeWithWupBuffer(const string &file, const string& putName)
{
	string stName;
	if(_wupVersion == 2)
	{
	    map<string, map<string, vector<char> > >::iterator it1 = _wup2_helper.find(_putName);
		if (it1 != _wup2_helper.end())
		{
			cout << "put    name:" << it1->first << endl;
			map<string, vector<char> >::iterator it2 = it1->second.begin();
			while(it2 != it1->second.end())
			{
				//ȥ�����ֿռ�
				string::size_type pos = 0;
				pos = it2->first.find(".");
	            if (pos != string::npos)
	            {
					stName = it2->first.substr(pos + 1);
				}
				else
				{
					stName = it2->first;
				}

				_StructName = stName; //�趨��Ҫ�����Ľṹ��
				cout << "struct name:" << stName << endl;

				vector<char> v = it2->second;

				//����wup���е�sBuffer
				setBuffer(&v[0], v.size()) ;
				std::vector<ContextPtr> contexts = g_parse->getContexts();
				for(size_t i = 0; i < contexts.size(); i++)
				{
					if(file == contexts[i]->getFileName())
					{
						analyzeContext(contexts[i]);
					}
				}
				it2++;
			}
		}
		else
		{
			cout << "not find wup put name:" << _putName<<endl;
		}
	}
	else if(_wupVersion == 3)//�����û��structname,how to do ?
	{
		map<string, vector<char> >::const_iterator it = _wup3_helper.find(_putName);
		if( it != _wup3_helper.end())
		{
			cout << "put    name:" << it->first << endl;
			cout << "struct name:" << _StructName << endl;
			vector<char> v = it->second;

			//����wup���е�sBuffer
			setBuffer(&v[0], v.size()) ;
			std::vector<ContextPtr> contexts = g_parse->getContexts();
			for(size_t i = 0; i < contexts.size(); i++)
			{
				if(file == contexts[i]->getFileName())
				{
					analyzeContext(contexts[i]);
				}
			}

		}
		else
		{
			cout << "not find wup put name:" << _putName<<endl;
		}
	}


}




void JceAnalyzer::analyzeFile(const string &file, string streamFile)
{
	string content = taf::TC_File::load2str(streamFile);
	cout << "analyzeFile:" << taf::TC_File::extractFileName(streamFile) << "'s buffer size:" <<  content.length() << endl;
	setBuffer(content.c_str(), content.length());

	if (_startPos > 0)
	{
		_cur = _startPos;
		_last_pos = _startPos;

	}
	if (_bWup)
	{
		checkWupBuffer();
	}

	std::vector<ContextPtr> contexts = g_parse->getContexts();
	for(size_t i = 0; i < contexts.size(); i++)
	{
		if(file == contexts[i]->getFileName())
		{
			analyzeContext(contexts[i]);
		}
	}
}

void JceAnalyzer::checkWupBuffer(void)
{
	if(_buf_len < sizeof(int))
	{
		cout << "ERROR, wup buffer too short!" << endl;
		exit(-1);
	}
	uint32_t iHeaderLen;

	if (_buf)
	{
		iHeaderLen = ntohl(*(uint32_t*)(_buf));
	}
	else
	{
		cout << "ERROR, wup buffer not set!" << endl;
		exit(-1);
	}

	//���ȴ���10M
	if (iHeaderLen > 10000000)
	{
		cout << "packet length too long,len:" + taf::TC_Common::tostr(iHeaderLen) << endl;
		exit(-1);
	}

	if (iHeaderLen != (_buf_len - chars_buf_reserve))
	{
		cout << "ERROR, buffer size: " << _buf_len - chars_buf_reserve <<   " not eq header len:" << iHeaderLen << endl;
		exit(-1);
	}

	cout << "check wup buffer ok!" << endl;


}

void JceAnalyzer::peekBuf(void * buf, size_t len, size_t offset)
{
	if (_cur + offset + len > _buf_len)
	{

		cout<< "ERROR, buffer overflow when peekBuf, buf_len:" << _buf_len << " cur:" << _cur << " len:"<< len <<endl;
		exit(-1);
	}
	::memcpy(buf, _buf + _cur + offset, len);

}

/// ��ȡ����
void JceAnalyzer::readBuf(void * buf, size_t len)
{
	peekBuf(buf, len);
	_last_pos = _cur;
	_cur += len;
}


/// ����len���ֽ�
void JceAnalyzer::skip(size_t len)
{
	_last_pos = _cur;
	_cur += len;
}


/// ������ǰ�ṹ�Ľ���
void JceAnalyzer::skipToStructEnd()
{
	AnalyzerDataHead h;
	do
	{
		h.readFrom(*this);
		skipField(h.getType());
	}while (h.getType() != AnalyzerDataHead::eStructEnd);
}

/// ����һ���ֶ�
void JceAnalyzer::skipField()
{
	AnalyzerDataHead h;
	h.readFrom(*this);
	skipField(h.getType());
}

/// ����һ���ֶΣ�������ͷ��Ϣ
void JceAnalyzer::skipField(uint8_t type)
{
	unsigned char tag = 0;
	int realType;
	switch (type)
	{
	case AnalyzerDataHead::eChar:
		this->skip(sizeof(char));
		break;
	case AnalyzerDataHead::eShort:
		this->skip(sizeof(short));
		break;
	case AnalyzerDataHead::eInt32:
		this->skip(sizeof(int));
		break;
	case AnalyzerDataHead::eInt64:
		this->skip(sizeof(long long));
		break;
	case AnalyzerDataHead::eFloat:
		this->skip(sizeof(float));
		break;
	case AnalyzerDataHead::eDouble:
		this->skip(sizeof(double));
		break;
	case AnalyzerDataHead::eString1:
		{
			size_t len = readByType<uint8_t>();
			this->skip(len);
		}
		break;
	case AnalyzerDataHead::eString4:
		{
			size_t len = ntohl(readByType<uint32_t>());
			this->skip(len);
		}
		break;
	case AnalyzerDataHead::eMap:
		{
			Int32 size;
			read(size, tag, realType);
			for (int i = 0; i < size * 2; ++i)
				skipField();
		}
		break;
	case AnalyzerDataHead::eList:
		{
			Int32 size;
			read(size, tag, realType);
			for (int i = 0; i < size; ++i)
				skipField();
		}
		break;
	case AnalyzerDataHead::eSimpleList:
		{
			AnalyzerDataHead h;
			h.readFrom(*this);
			if (h.getType() != AnalyzerDataHead::eChar)
			{

				cout << "ERROR, skipField with invalid type, type value: " << (int)type <<", " << h.tostrType(h.getType()) << endl;

				cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
				exit(-1);
			}
			Int32 size;
			read(size, tag, realType);
			this->skip(size);
		}
		break;
	case AnalyzerDataHead::eStructBegin:
		skipToStructEnd();
		break;
	case AnalyzerDataHead::eStructEnd:
	case AnalyzerDataHead::eZeroTag:
		break;
	default:
		{
			cout << "ERROR, skipField with invalid type, type value:" << (int)type << endl;
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
			exit(-1);
		}
	}
}


void JceAnalyzer::backToLast()
{
	_cur = _last_pos;
}


int JceAnalyzer::readType(const TypePtr &pPtr, int expectTag, bool isRequire)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr) return readBuiltin(bPtr, expectTag, isRequire, NULL);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return readVector(vPtr, expectTag,isRequire, NULL);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if(mPtr) return readMap(mPtr, expectTag, isRequire);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return readStruct(sPtr,  NULL);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return 0;

    if (!pPtr) return 0;
	cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
    exit(-1);
    return 0;
}

/*******************************BuiltinPtr********************************/
string JceAnalyzer::tostrBuiltin(const BuiltinPtr &pPtr)
{
	string s;

	switch(pPtr->kind())
	{
	case Builtin::KindBool:
		s = " bool ";
		break;
	case Builtin::KindByte:
		s = " char ";
		break;
	case Builtin::KindShort:
	//Ϊ�˼���java��unsigned, �����ʱ��jce�ʼ��� unsigned char ��Ӧ��short
	//c++����Ҫ��ԭ����
		s = (pPtr->isUnsigned()?"uchar":" short ");
		break;
	case Builtin::KindInt:
		s = (pPtr->isUnsigned()?"ushort":" int ");
		break;
	case Builtin::KindLong:
		s =  (pPtr->isUnsigned()?"uint":" int64 ");
		break;
	case Builtin::KindFloat:
		s = " float ";
		break;
	case Builtin::KindDouble:
		s = " double ";
		break;
	case Builtin::KindString:
		if(pPtr->isArray())
			s = "char[]"; //char a [8];
		else
			s = " string ";//string a;
		break;
	case Builtin::KindVector:
		s = " vector ";
		break;
	case Builtin::KindMap:
		s = " map ";
		break;
	default:
		cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;
		exit(-1);
		break;
	}

	return s;
}
string JceAnalyzer::tostr(const TypePtr &pPtr)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if(bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if(vPtr) return tostrVector(vPtr);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if(mPtr) return tostrMap(mPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if(sPtr) return tostrStruct(sPtr);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    //if(ePtr) return tostrEnum(ePtr);

    if(!pPtr) return "void";
	cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
    exit(-1);
    return "";
}

string JceAnalyzer::tostrMap(const MapPtr &pPtr, bool bNew)
{
    string s = Builtin::builtinTable[Builtin::KindMap] + string("<") + tostr(pPtr->getLeftTypePtr()) + ", " + tostr(pPtr->getRightTypePtr());
    if(MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
    {
        s += " >";
    }
    else
    {
        s += ">";
    }
    return s;
}


string JceAnalyzer::getTab()
{
    ostringstream s;
    for(int i = 0; i < _itab; i++)
    {
        s << "    ";
    }

    return s.str();
}
string JceAnalyzer::getSep()
{
	ostringstream s;
	for(int i = 0; i < CHAR_WIDTH_ID; i++)
	{
		s << "-";
	}

	return s.str();
}

/**
 * ����tab��
 */
void JceAnalyzer::incTab()
{
	_itab++;
}

void JceAnalyzer::addTab(int count)
{
	_itab += count;
}

void JceAnalyzer::eraseTab(int count)
{
	_itab -= count;
}

/**
 * ����tab��
 */
void JceAnalyzer::decTab()
{
	_itab--;
}

void JceAnalyzer::read(double& n, uint8_t expecttag, int & realType, bool isRequire)
{
	AnalyzerDataHead h;
	h.readFrom(*this);

	int ret = compareTag(h.getTag(), expecttag, isRequire);
	expecttag = h.getTag();
	realType = h.getType();
	n = 0.0f;
	if (ret != eOK)
	{
		return;
	}

	switch (h.getType())
	{
	case AnalyzerDataHead::eZeroTag:
		n = 0;
		break;
	case AnalyzerDataHead::eFloat:
		n = readByType<float>();
		n = jce_ntohf(n);
		break;
	case AnalyzerDataHead::eDouble:
		this->readBuf(&n, sizeof(n));
		n = jce_ntohd(n);
		break;
	default:
		{
			cout << "ERROR, read string type mismatch, tag: " << (int) expecttag << ", get type: " <<  (int)h.getType() << endl;
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
			exit(-1);
		}

	}
}

void JceAnalyzer::read(string& s, uint8_t&  expecttag, int& realType, bool isRequire )
{
	AnalyzerDataHead h;
	h.readFrom(*this);

	int ret = compareTag(h.getTag(), expecttag, isRequire);

	expecttag = h.getTag();
	realType = h.getType();
	s = "";

	if (ret != eOK)
	{
		return;
	}
	switch (h.getType())
	{
	case AnalyzerDataHead::eString1:
		{
			size_t len = readByType<uint8_t>();
			char ss[256];
			this->readBuf(ss, len);
			s.assign(ss, ss + len);
		}
		break;
	case AnalyzerDataHead::eString4:
		{
			//��ȡstring4�ĳ���
			uint32_t len = ntohl(readByType<uint32_t>());
			if (len > JCE_MAX_STRING_LENGTH)
			{

			}
			char *ss = new char[len];
			try
			{
				this->readBuf(ss, len);
				s.assign(ss, ss + len);
			}
			catch (...)
			{
				delete[] ss;
				throw;
			}
			delete[] ss;
		}
		break;
	default:
		{
			cout << "ERROR, read string type mismatch, tag: " << (int) expecttag << ", get type: " <<  (int)h.getType() << endl;
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
			exit(-1);
		}
	}
}
void JceAnalyzer::read(Int32& n, uint8_t&  tag, int & realType, bool isRequire)
{
	AnalyzerDataHead h;
	h.readFrom(*this);

	int ret = compareTag(h.getTag(), tag, isRequire);
	tag = h.getTag();
	realType = h.getType();
	n = 0;
	if (ret != eOK)
	{
		return;
	}
	switch (h.getType())
	{
	case AnalyzerDataHead::eZeroTag:
		n = 0;
		break;
	case AnalyzerDataHead::eChar:
		n = readByType<char>();
		break;
	case AnalyzerDataHead::eShort:
		n = (short) ntohs(readByType<short>());
		break;
	case AnalyzerDataHead::eInt32:
		this->readBuf(&n, sizeof(n) );
		n = ntohl((Int32)n);
		break;
	default:
		{
			cout<< "ERROR, read numeric type mismatch, buf tag: " << (int)tag << ", get type: " << h.tostrType(h.getType()) << endl;
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
			exit(-1);
		}
	}

}


void JceAnalyzer::read(Int64& n, uint8_t&  tag, int & realType, bool isRequire)
{
	AnalyzerDataHead h;
	h.readFrom(*this);

	int ret = compareTag(h.getTag(), tag, isRequire);
	tag = h.getTag();
	realType = h.getType();
	n = 0;
	if (ret != eOK)
	{
		return;
	}
	switch (h.getType())
	{
	case AnalyzerDataHead::eZeroTag:
		n = 0;
		break;
	case AnalyzerDataHead::eChar:
		n = readByType<char>();
		break;
	case AnalyzerDataHead::eShort:
		n = (short) ntohs(readByType<short>());
		break;
	case AnalyzerDataHead::eInt32:
		n = (Int32) ntohl(readByType<Int32>());
		break;
	case AnalyzerDataHead::eInt64:
		this->readBuf(&n, sizeof(n) );
		n = jce_ntohll((Int64)n);
		break;
	default:
		{
			cout<< "ERROR, read numeric type mismatch, buf tag: " << (int)tag << ", get type: " << h.tostrType(h.getType()) << endl;
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
			exit(-1);
		}
	}

}


template<typename K, typename V, typename Cmp, typename Alloc>
void JceAnalyzer::read(std::map<K, V, Cmp, Alloc>& m, uint8_t tag, bool isRequire)
{
	uint8_t utag = 0;
	int realtype;

	AnalyzerDataHead h;
	h.readFrom(*this);
	switch (h.getType())
	{
	case AnalyzerDataHead::eMap:
		{
			int size;
			read(size, utag, realtype);
			if (size < 0)
			{
				char s[128];
				snprintf(s, sizeof(s), "invalid map, tag: %d, size: %d", tag, size);
				cout << s << endl;
			}
			m.clear();

			for (int i = 0; i < size; ++i)
			{
				std::pair<K, V> pr;
				utag = 0;
				read(pr.first, utag, realtype);
				utag = 1;
				read(pr.second, utag, realtype);
				m.insert(pr);
			}
		}
		break;
	default:
		{
			char s[64];
			snprintf(s, sizeof(s), "read 'map' type mismatch, tag: %d, get type: %d.", tag, h.getType());
			cout << s << endl;
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
		}
	}

}

template<typename Alloc>
void JceAnalyzer::read(std::vector<char, Alloc>& v,  uint8_t&  tag, int & realType, bool isRequire )
{
	//if (skipToTag(tag))
	{
		uint8_t t=0;
		int realType;
		AnalyzerDataHead h;
		h.readFrom(*this);
		switch (h.getType())
		{
		case AnalyzerDataHead::eSimpleList:
			{
				AnalyzerDataHead hh;
				hh.readFrom(*this);
				if (hh.getType() != AnalyzerDataHead::eChar)
				{
					char s[128];
					snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d, %d", tag, h.getType(), hh.getType());
					//throw JceDecodeMismatch(s);
				}
				int size;
				read(size, t, realType);
				if (size < 0)
				{
					char s[128];
					snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, %d, size: %d", tag, h.getType(), hh.getType(), size);
					//throw JceDecodeInvalidValue(s);
				}
				v.resize(size);
				readBuf(&v[0], size);
			}
			break;


		default:
			{
				char s[128];
				snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d", tag, h.getType());
				//throw JceDecodeMismatch(s);
			}
		}
	}


}

int JceAnalyzer::readVector(const VectorPtr &pPtr, int expectTag, bool isRequire, TypeIdPtr pType)
{
	{
		int type;
		AnalyzerDataHead h;
		h.readFrom(*this);

		uint8_t t=0;
		uint8_t tag =h.getTag();
		int ret = compareTag(tag, expectTag,isRequire);
		if (ret != eOK)
		{
			return 0;
		}
		switch (h.getType())
		{
		case AnalyzerDataHead::eList:
			{
				int size;
				read(size, t, type);
				cout << getTab()<< "read buf tag " <<  (int)tag  <<  " vector size:" << size << endl;
				if (size < 0|| size > 10 * 1024 * 1024)
				{
					cout<< getTab() << "ERROR, invalid size, tag: " <<  (int)tag << endl;
					exit(-1);
				}
				//addTab(7);

				cout << getTab()<<"{ //tag:" << (int)tag << endl;
				for (int i = 0; i < size; ++i)
				{

					cout  << getTab() << " index:" << i << " ";
					readType(pPtr->getTypePtr(), 0, isRequire);
				}

				cout << getTab()<<"} //tag:" << (int)tag << endl;
				//eraseTab(7);
			}
			break;

		case AnalyzerDataHead::eSimpleList:
			{

				AnalyzerDataHead hh;
				hh.readFrom(*this);
				if (hh.getType() != AnalyzerDataHead::eChar)
				{
					cout<< getTab() << "error, type mismatch, tag: " <<  (int)tag;

					cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
				}
				int size;
				read(size, t, type);

				cout<< getTab() << "read buf tag " <<  (int)tag << " vector byte size:" << size << endl;
				if (size < 0 || size > 10 * 1024 * 1024)
				{
					cout<< getTab() << "ERROR, invalid size " << size << endl;
					cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
					exit(-1);

				}

				char *ss = new char[size];
				try
				{
				    memset(ss, 0, size);
					readBuf(ss, size);
					// debug �ٴ�
					//cout << getTab()<< "tag " << (int)tag << " vector byte :\n" << taf::TC_Common::bin2str(ss, size, " ",  chars_one_line) << endl;
				}
				catch (...)
				{
					delete[] ss;
					throw;
				}
				delete[] ss;


			}
			break;
		default:
			{
 				cout << getTab() << "ERROR, read 'vector' type mismatch, tag: " << (int)tag << "  get type: " <<  h.tostrType(h.getType()) << endl;
				cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
				exit(-1);
			}
		}
	}

	return 0;
}



int JceAnalyzer::readMap(MapPtr pPtr, int expectTag, bool isRequire)
{

	int type;
	AnalyzerDataHead h;
	h.readFrom(*this);

	uint8_t tag = h.getTag();

	int ret = compareTag(tag, expectTag,isRequire);
	if (ret != eOK)
	{
		return 0;
	}
	switch (h.getType())
	{
	case AnalyzerDataHead::eMap:
		{
			int size;
			uint8_t t = 0;
			read(size, t, type);

			cout << "read " << " map size:" << size << endl;
			if (size < 0|| size > 10 * 1024 * 1024)
			{
				cout<< getTab() << "ERROR, invalid size " << size << endl;
				cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
				exit(-1);
			}
			//addTab(7);
			cout << getTab()<< "{ //tag:" << (int)tag << endl;
			for (int i = 0; i < size; ++i)
			{

				cout  << getTab() << " index:" << i << " first:";
				readType(pPtr->getLeftTypePtr(), 0,  isRequire);
			    cout<< getTab()<< " index:" << i << " second:" ;
				readType(pPtr->getRightTypePtr(), 1,  isRequire);

			}

			cout << getTab()<< "} //tag:" << (int)tag << endl;
			//eraseTab(7);
		}
		break;
	default:
		{
			cout << "ERROR, read map type mismatch, tag:" <<  (int)tag <<"get type " << h.tostrType(h.getType() )<< endl;
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
			exit(-1);
		}
	}

	return 0;

}

int JceAnalyzer::readEnum(EnumPtr pPtr, int expectTag, bool isRequire)
{

	int n = 0;
	AnalyzerDataHead h;
	h.readFrom(*this);
	uint8_t tag = h.getTag();
	int ret = compareTag(tag, expectTag,isRequire);
	if (ret != eOK)
	{
		return 0;
	}
	switch (h.getType())
	{
			case AnalyzerDataHead::eZeroTag:
				n = 0;
				break;
			case AnalyzerDataHead::eChar:
				n = readByType<char>();
				break;
			case AnalyzerDataHead::eShort:
				n = (short) ntohs(readByType<short>());
				break;
			case AnalyzerDataHead::eInt32:
				this->readBuf(&n, sizeof(n));
				n = ntohl(n);
				break;

			default:
				{
					cout<< "ERROR, read enum type mismatch, buf tag: " << (int)tag << ", get type: " << h.tostrType(h.getType()) << endl;
					cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
					exit(-1);
				}
	}
	cout << "read enum:" << n << endl;
	return 0;


}

int JceAnalyzer::readBuiltin(const BuiltinPtr &pPtr, int expectTag, bool bIsRequire, TypeIdPtr pType)
{
	uint8_t tag = expectTag;
	int realType = 0 ;

    switch (pPtr->kind())
    {
        case Builtin::KindBool:

        case Builtin::KindByte:

        case Builtin::KindShort:

        case Builtin::KindInt:

        case Builtin::KindLong:
			{

				Int64 n;
				read(n, tag, realType, bIsRequire);
				cout <<  "read buf tag [" <<(int)tag <<"|"<< AnalyzerDataHead::tostrType(realType)<<"|"<<n<<"]"<<endl;
			}
			//compareTag(tag, expectTag, bIsRequire);
			break;

        case Builtin::KindFloat:

        case Builtin::KindDouble:
			{
				double n;
				read(n, tag, realType, bIsRequire);
				cout <<  "read buf tag [" <<(int)tag <<"|"<< AnalyzerDataHead::tostrType(realType) << n<<"]"<<endl;
			}
			//compareTag(tag, expectTag, bIsRequire);
			break;

        case Builtin::KindString:
			{
				string s;
	            read(s, tag, realType, bIsRequire);
				cout <<  "read buf tag [" <<(int)tag <<"|"<< AnalyzerDataHead::tostrType(realType)  << s<<"]"<<endl;
				//compareTag(tag, expectTag, bIsRequire);
       	    }
			break;

        default:
			cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
			exit(-1);
    }

    return 0;
}

int JceAnalyzer::compareTag(int tag, int expectTag, bool bIsRequire)
{
	if (tag != expectTag && bIsRequire)
	{
		 cout << getTab() << "ERROR ,required tag is " << expectTag << ", but read tag:" << tag<< " from buf !" ;
		 cout << getTab() << " _cur:0x" << ::hex<<_cur << " _last_pos:0x" << ::hex<<_last_pos << endl;
		 //assert(false);
		 exit(-1);
	}
    // optional tag
	if (expectTag < tag) //JCE ��tagС�ڴ�buf��������tag
	{
		// ���ܶԷ�û�б���expectTag��buf
		//��Ϊ�ǿ�ѡ��������jce�ļ��е�expectTag
		//������AnalyzerDataHead��jce�ļ�����һ��tag���õ�
		cout << getTab() << "warning ,optional tag is " << expectTag << " < tag:" << tag<< " from buf !"  ;
		cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << " " << endl;
		backToLast();
		return eNeedBackDataHead;
	}
	else if (expectTag > tag)
	{
		// ������JCE�ļ��������˿�ѡ�ֶ�
		//����JCE�ļ���tag��д����
		//������JCE�ļ��а�ԭ���Ŀ�ѡ�ֶ�ȥ����
		cout << getTab() << "warning ,optional tag is " << expectTag << " < tag:" << tag<< " from buf !"  ;
		cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << " ";
		backToLast();
		skipField();
		return eNeedSkipTag;
	}
	return eOK;
}
string JceAnalyzer::tostrVector(const VectorPtr &pPtr)
{
	//��������
	if(pPtr->isArray())
	{
		return tostr(pPtr->getTypePtr());
	}

	//ָ������
	if(pPtr->isPointer())
	{
		return tostr(pPtr->getTypePtr())+" *";
	}

	string s = Builtin::builtinTable[Builtin::KindVector] + string("<") + tostr(pPtr->getTypePtr());

	if(MapPtr::dynamicCast(pPtr->getTypePtr()) || VectorPtr::dynamicCast(pPtr->getTypePtr()))
	{
		s += " >";
	}
	else
	{
		s += ">";
	}
	return s;
}


string JceAnalyzer::tostrStruct(const StructPtr &pPtr) const
{
    return pPtr->getSid();
}

string JceAnalyzer::tostrEnum(const EnumPtr &pPtr)
{
    return pPtr->getSid();
}


int JceAnalyzer::readStruct(const StructPtr & pPtr, const NamespacePtr & nPtr)
{
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

	AnalyzerDataHead h;
    size_t hLen = h.peekFrom(*this);
	if (AnalyzerDataHead::eStructBegin == h.getType()) // ���ṹ��ʼ
	{
		cout << endl;
		incTab();
		cout << getTab()<<  "struct " << pPtr->getId() << " {"  << endl;
		skip(hLen);
	}

    //����struct��Ա����
    for (size_t i = 0; i < member.size(); i++)
    {
		cout<< getTab() <<"expect tag ";
		cout.setf(ios::right);
		cout.width(CHAR_WIDTH_TAG);
		cout<< member[i]->getTag();
		cout.width(CHAR_WIDTH_ID); // ��Ա��12�ַ���
		cout<< member[i]->getId();

		//��ȡwup�汾��
		if(isWup() && (pPtr->getId() == "RequestPacket"))
		{
			if(member[i]->getTag() == 1)
			{
				char iVer = 2;
				this->peekBuf(&iVer, sizeof(iVer),1);
				_wupVersion = iVer;
			}
			else if(member[i]->getTag() == 7)///����sbuffer
			{
				_wup_sbuffer_pos = _cur;
				_cur +=2; //7D 00

				int n;
				uint8_t  tag = 0;
				int  realType;
				read(n,  tag,  realType, true ); //sbuffer�ĳ���

				if(_wupVersion == 2)
				{
					read(_wup2_helper, 7);
					_bIsDumpWup?dumpWup(_wup2_helper):(void(0));
				}
				else///3
				{
					read(_wup3_helper, 7);
					_bIsDumpWup?dumpWup(_wup3_helper):(void(0));
				}

				_cur = _wup_sbuffer_pos;
			}
		}

		//��ȡ���û�������
        BuiltinPtr bPtr = BuiltinPtr::dynamicCast(member[i]->getTypePtr());
        if (bPtr)
        {
			cout.width(CHAR_WIDTH_TYPE);
			cout<<tostrBuiltin(bPtr);

			cout<<getSep();
			readBuiltin(bPtr, member[i]->getTag(), member[i]->isRequire(), NULL);
        }

        VectorPtr vPtr = VectorPtr::dynamicCast(member[i]->getTypePtr());
        if (vPtr)
        {
			cout.width(CHAR_WIDTH_TYPE);
			cout<<tostrVector(vPtr);
			cout<<getSep();
			readVector(vPtr, member[i]->getTag(),member[i]->isRequire(), NULL);
        }

		MapPtr mPtr   = MapPtr::dynamicCast(member[i]->getTypePtr());
		if (mPtr)
		{
			cout.width(CHAR_WIDTH_TYPE);
			cout<<tostrMap(mPtr);
			cout<<getSep();
			readMap(mPtr, member[i]->getTag(), member[i]->isRequire());
		}

        StructPtr sPtr = StructPtr::dynamicCast(member[i]->getTypePtr());
        if (sPtr)
        {
        	cout.width(CHAR_WIDTH_TYPE);
			cout<< tostrStruct(sPtr);
			cout<<getSep();
			readStruct(sPtr, NULL);
	    }

        EnumPtr ePtr = EnumPtr::dynamicCast(member[i]->getTypePtr());
		if (ePtr)
		{

        	cout.width(CHAR_WIDTH_TYPE);
			cout<< tostrEnum(ePtr);
			cout<<getSep();
			readEnum(ePtr , member[i]->getTag(), member[i]->isRequire());
		}

    }

 	if (!_bWup)
 	{
	    hLen = h.peekFrom(*this); //buf��û�нṹ������־��Ҳ�������쳣����ΪsetBuffer��buf_len��1�������ֽ�
		if (AnalyzerDataHead::eStructEnd == h.getType() || _cur >= _buf_len-chars_buf_reserve) // ���ṹ����
		{
			cout << getTab() <<  "} //struct " << pPtr->getId() <<  endl;
			decTab();
			skip(hLen);
			if (pPtr->getId() == _StructName)
			{
				cout << "analyze " << _streamFileName << " with struct " << _StructName << " ok!" << endl;
			}
		}
 	}
	return 0;

}

void JceAnalyzer::dumpWup(const map<string, vector<char> >& wupData)
{
	map<string, vector<char> >::const_iterator it = wupData.begin();
	cout << "\nwup attr begin:" << endl;
	while(it != wupData.end())
	{
		cout << "wup Attribute name: " << it->first << endl;
		vector<char> vContent = it->second;
		cout << "struct buff: ";
		cout << taf::TC_Common::bin2str(&vContent[0], vContent.size(), " ", chars_one_line ) << endl;
		it++;
	}
	cout << "wup attr end:\n" << endl;
}

void JceAnalyzer::dumpWup(const map<string, map<string, vector<char> > >& wupData)
{
	map<string, map<string, vector<char> > >::const_iterator it1 = wupData.begin();
	cout << "\nwup attr begin:" << endl;
	while (it1 != wupData.end())
	{
		cout << "wup Attribute name: " << it1->first << endl;
		map<string, vector<char> >::const_iterator it2 = it1->second.begin();
		while(it2 != it1->second.end())
		{

			cout << "struct name: " << it2->first << endl;
			vector<char> vContent = it2->second;

			//int lines = vContent.size()/chars_one_line + 1;
			//cout << "struct buff lines:"<< lines << endl;
			cout << "struct buff: ";
			cout << taf::TC_Common::bin2str(&vContent[0], vContent.size(), " ", chars_one_line ) << endl;
			it2++;
		}

		it1++;
	}
	cout << "wup attr end:\n" << endl;
}

