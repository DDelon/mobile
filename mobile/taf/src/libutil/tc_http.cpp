#include "util/tc_http.h"
#include "util/tc_common.h"
#include "util/tc_clientsocket.h"

namespace taf
{

bool TC_URL::isValid() const
{
    return !_sURL.empty();
}

string TC_URL::getURL() const
{
    return _sURL;
}

string TC_URL::type2String() const
{
    switch(_iURLType)
    {
    case HTTP:
        return "http";
    case HTTPS:
        return "https";
    case FTP:
        return "ftp";
    }

    return "http";
}

string TC_URL::getDefaultPort() const
{
    switch(_iURLType)
    {
    case HTTP:
        return "80";
    case HTTPS:
        return "433";
    case FTP:
        return "21";
    }

    return "80";
}

bool TC_URL::isDefaultPort() const
{
    return getFragment("port") == getDefaultPort();
}

string TC_URL::toURL() const
{
    string sUserName    = getFragment("user");
    string sPassword    = getFragment("pass");
    string sDomain      = getFragment("domain");
    string sPort        = getFragment("port");
    string sUrlPath     = getFragment("path");
    string sUrlQuery    = getFragment("query");
    string sUrlRef      = getFragment("ref");

    string sURL = type2String() + "://";

    if(!sUserName.empty())
        sURL += sUserName;

    if(!sUserName.empty() && !sPassword.empty())
        sURL += ":" + sPassword;

    if(!sUserName.empty())
        sURL += "@";

    sURL += sDomain;

    if(!isDefaultPort())
        sURL += ":" + sPort;

    sURL += getRequest();

    return sURL;
}

string TC_URL::getRequest() const
{
    string sURL;

    string sUrlPath     = getFragment("path");

    string sUrlQuery    = getFragment("query");

    string sUrlRef      = getFragment("ref");

    if(!sUrlPath.empty())
        sURL += sUrlPath;

    if(!sUrlQuery.empty())
        sURL += "?" + sUrlQuery;

    if(!sUrlRef.empty())
        sURL += "#" + sUrlRef;

    return sURL;
}

bool TC_URL::parseURL(const string &sURL)
{
    string originRequest  = TC_Common::trim(sURL, " ");

    if(originRequest.empty())
    {
        return false;
    }

    clear();

	string sUrlPath;

	if(strncasecmp(originRequest.c_str(), "http://" ,7) == 0)
	{
        //http��ͷ
        _iURLType  = HTTP;
		sUrlPath   = originRequest.substr(7);
	}
    else if(strncasecmp(originRequest.c_str(), "https://" ,8) == 0)
    {
        //https��ͷ
        _iURLType  = HTTPS;
        sUrlPath   = originRequest.substr(8);
    }
    else if(strncasecmp(originRequest.c_str(), "ftp://", 6) == 0)
    {
        //ftps��ͷ
        _iURLType  = FTP;
        sUrlPath   = originRequest.substr(6);
    }
    else
    {
        //Ĭ����http
        _iURLType  = HTTP;
        sUrlPath   = originRequest;
    }

    //����url, www.qq.com?a=b/c ==> www.qq.com/?a=b/c   www.qq.com?a=b ==> www.qq.com/?a=b
    //����url, www.qq.com#ab/c ==> www.qq.com/#a=b/c    www.qq.com#abc ==> www.qq.com/#a=b
	string::size_type index            = sUrlPath.find("/");
    string::size_type nQuestionIndex   = sUrlPath.find("?");
    string::size_type nNumbersignIndex = sUrlPath.find("#");

  	if(nQuestionIndex < index)
  	{
  		sUrlPath.insert(nQuestionIndex, "/");
  	}
  	else if(nNumbersignIndex < index)
  	{
  		sUrlPath.insert(nNumbersignIndex, "/");
  	}

    //��ȡauthority, urlPathȥ��host����
    string sUrlAuthority;
    index = sUrlPath.find("/");
	if(index == string::npos)
	{
		sUrlAuthority= sUrlPath.substr(0, index);
		sUrlPath     = "";
	}
	else
	{
		sUrlAuthority= sUrlPath.substr(0, index);
		sUrlPath     = sUrlPath.substr(index);
	}

    string sUrlQuery;

    index = sUrlPath.find("?");
    if(index != string::npos)
    {
        sUrlQuery = sUrlPath.substr(index + 1);

        sUrlPath  = sUrlPath.substr(0, index);
    }

    string sUrlRef;

    index = sUrlQuery.rfind("#");
    if(index != string::npos)
    {
        sUrlRef     = sUrlQuery.substr(index + 1);

        sUrlQuery   = sUrlQuery.substr(0, index);
    }

    if(sUrlPath.empty())
    {
        sUrlPath = "/";
    }

    string sUrlUserInfo;
    string sUrlHost;
    string sUserName;
    string sPassword;
    string sDomain;
    string sPort;

    //////����Authority
    index = sUrlAuthority.find("@");
    if(index != string::npos)
    {
        sUrlHost    = sUrlAuthority.substr(index + 1);

        sUrlUserInfo= sUrlAuthority.substr(0, index);
    }
    else
    {
        sUrlHost    = sUrlAuthority;
    }

    //////����User:Pass
    index = sUrlUserInfo.find(":");
    if(index != string::npos)
    {
        sPassword   = sUrlUserInfo.substr(index + 1);

        sUserName   = sUrlUserInfo.substr(0, index);
    }
    else
    {
        sUserName   = sUrlUserInfo;
    }

    //////����sUrlHost:Port
    index = sUrlHost.find(":");
    if(index != string::npos)
    {
        sPort       = sUrlHost.substr(index + 1);

        sDomain     = sUrlHost.substr(0, index);
    }
    else
    {
        sPort       = getDefaultPort();

        sDomain     = sUrlHost;
    }

    _fragment["scheme"] = type2String();
    _fragment["user"]   = sUserName;
    _fragment["pass"]   = sPassword;
    _fragment["domain"] = TC_Common::lower(sDomain);
    _fragment["port"]   = sPort;
    _fragment["path"]   = sUrlPath;
    _fragment["query"]  = sUrlQuery;
    _fragment["ref"]    = sUrlRef;

    _sURL = toURL();

    //��������IP�������һ����
    if(sDomain.find(".") == string::npos)
    {
        return false;
    }

    return true;
}

void TC_URL::specialize()
{
    //������·��
    string sUrlPath = simplePath(getPath());

    _fragment["path"]   = sUrlPath;

    _sURL = toURL();
}

void TC_URL::clear()
{
    _fragment.clear();

    _sURL.clear();
}

string TC_URL::getScheme() const
{
    return type2String();
}

string TC_URL::getUserName() const
{
    return getFragment("user");
}

string TC_URL::getPassword() const
{
    return getFragment("pass");
}

string TC_URL::getDomain() const
{
    return getFragment("domain");
}

string TC_URL::getPort() const
{
    return getFragment("port");
}

string TC_URL::getPath() const
{
    return getFragment("path");
}

string TC_URL::getQuery() const
{
    return getFragment("query");
}

string TC_URL::getRef() const
{
    return getFragment("ref");
}

string TC_URL::getFragment(const string& frag) const
{
    map<string, string>::const_iterator it = _fragment.find(frag);
    if(it == _fragment.end())
    {
        return "";
    }

    return it->second;
}

string TC_URL::getRelativePath() const
{
    string sURL = getPath();

	string::size_type pos;


	pos   = sURL.rfind("/");

	if(pos == string::npos)
	{
        return "/";
	}
	else
	{
		return sURL.substr(0, pos + 1);
	}
}

string TC_URL::getRootPath() const
{
    string sUserName    = getFragment("user");
    string sPassword    = getFragment("pass");
    string sDomain      = getFragment("domain");
    string sPort        = getFragment("port");

    string sURL = type2String() + "://";

    if(!sUserName.empty())
        sURL += sUserName;

    if(!sUserName.empty() && !sPassword.empty())
        sURL += ":" + sPassword;

    if(!sUserName.empty())
        sURL += "@";

    sURL += sDomain;

    if(!isDefaultPort())
        sURL += ":" + sPort;

    sURL += "/";

    return sURL;
}

TC_URL TC_URL::buildWithRelativePath(const string &sRelativePath) const
{
    string sURL;

	if(!sRelativePath.empty() && sRelativePath[0] == '/')
	{
	    sURL = sRelativePath.substr(1); //�����������"/"��ͷ����Ե�ַ����ôӦ����Host+��Ե�ַ
	}
	else if(sRelativePath[0] == '#')
	{
		//#
		sURL = getPath().substr(1);

        if(!getQuery().empty())
            sURL += "?" + getQuery();

        sURL += sRelativePath;

	}
	else
	{
		//��Ե�ַ
		sURL = getRelativePath().substr(1) + sRelativePath;
	}

    sURL = getRootPath() + simplePath("/" + sURL).substr(1);

    TC_URL url;

    url.parseURL(sURL);

    return url;
}

string TC_URL::simplePath(const string &sPath) const
{
    //����./��ȥ��
    size_t pos      = 0;
	string sNewPath = sPath;

    while (true)
    {
        size_t dotPos = sNewPath.find("./", pos);

        if (dotPos != string::npos)
        {
            if ((dotPos == 0) || (sNewPath.at(dotPos - 1) == '/'))
            {
                sNewPath.erase(dotPos, 2);
            }
            else
            {
                pos = dotPos + 2;
            }
        }
        else
        {
            break;
        }
    }

    //���·������.��β��, ��.ȥ��
    if (((sNewPath.length() >= 2) && (sNewPath.substr(sNewPath.length()-2) == "/.")) || (sNewPath == "."))
    {
        sNewPath.erase(sNewPath.length() - 1);
    }

    //����/../����ʽ
    pos = 0;
    size_t startPos = 0;

    while (1)
    {
        size_t slashDot = sNewPath.find("/../", pos);

        if (slashDot != string::npos)
        {
            if (0 == slashDot)
            {
                sNewPath.erase(0, 3);
                continue;
            }

            if ( (slashDot > 1) && (sNewPath.substr(slashDot - 2, 2) == "..") )
            {
                pos = slashDot + 4;
                continue;
            }

            startPos = sNewPath.rfind('/', slashDot - 1);

            if (startPos == string::npos) startPos = 0;

            sNewPath.erase(startPos, slashDot + 4 - startPos - 1);
        }
        else
        {
            break;
        }
    }

    //����/..��β�����
    if ((sNewPath.size() >= 3) && (sNewPath.substr(sNewPath.size() - 3, 3) == "/.."))
    {
        size_t slashDot = sNewPath.size() - 3;
        if (!((slashDot > 1) && (sNewPath.substr(slashDot - 2, 2) == "..")))
        {
            startPos = sNewPath.rfind ('/', slashDot - 1);
            if (startPos == string::npos) startPos = 0;
            sNewPath.erase (startPos + 1);
        }
    }

	return sNewPath;
}

////////////////////////////////////////////////////////////////////

string TC_Http::getHeader(const string& sHeader) const
{
    http_header_type::const_iterator it = _headers.find(sHeader);
    if(it == _headers.end())
    {
        return "";
    }

    return it->second;
}

string TC_Http::getContentType() const
{
    return getHeader("Content-Type");
}

string TC_Http::getHost() const
{
    return getHeader("Host");
}

size_t TC_Http::getContentLength() const
{
    string s = getHeader("Content-Length");
    if(s.empty())
    {
        return 0;
    }

    return TC_Common::strto<size_t>(s);
}

string TC_Http::genHeader() const
{
	ostringstream sHttpHeader;

	for(http_header_type::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
        if(it->second != "")
        {
            sHttpHeader << it->first << ": " << it->second << "\r\n";
        }
	}

	return sHttpHeader.str();
}

vector<string> TC_Http::getHeaderMulti(const string &sHeadName) const
{
    vector<string> v;

    http_header_type::const_iterator itEnd = _headers.end();

    for( http_header_type::const_iterator it = _headers.begin(); it != itEnd; ++it)
    {
        if(strcasecmp(it->first.c_str(), sHeadName.c_str()) == 0)
        {
            v.push_back(it->second);
        }
    }

    return v;
}

string TC_Http::getLine(const char** ppChar)
{
	string sTmp;

	sTmp.reserve(512);

	while((**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0')
	{
		sTmp.append(1, (**ppChar));
		(*ppChar)++;
	}

	if((**ppChar) == '\r')
    {
		(*ppChar)++;   /* pass the char '\n' */
    }

	(*ppChar)++;

	return sTmp;
}


string TC_Http::getLine(const char** ppChar, int iBufLen)
{
	string sTmp;

	sTmp.reserve(512);

	int iCurIndex= 0;
	while( (**ppChar) != '\r' && (**ppChar) != '\n' && (**ppChar) != '\0')
	{
		if ( iCurIndex < iBufLen )
		{
			sTmp.append(1, (**ppChar));
			(*ppChar)++;
			iCurIndex++;
		}
		else
		{
			//MTT_ERRDAY << "parseHttp WARN: iCurIndex < iBufLen 1 " << endl;
			break;
		}
	}

	if( (**ppChar) == '\r')
	{
		if ( iCurIndex < iBufLen )
		{
			(*ppChar)++;   /* pass the char '\n' */
			iCurIndex++;
		}
		else
		{
			//MTT_ERRDAY << "parseHttp WARN: iCurIndex < iBufLen 2 " << endl;
		}
	}

	if( iCurIndex < iBufLen )
	{
		(*ppChar)++;
		iCurIndex++;
	}
	else
	{
		//MTT_ERRDAY << "parseHttp WARN: iCurIndex < iBufLen 3 " << endl;
	}

	return sTmp;
}


const char* TC_Http::parseHeader(const char* szBuffer, http_header_type &sHeader)
{
    sHeader.clear();

	const char **ppChar = &szBuffer;

	long length = strlen(szBuffer);
	long srcPtr = (long)(*ppChar);

	while(true)
	{
		string sLine = getLine(ppChar);

		if(sLine.empty()) break;

        //����ǵ�һ��, ����Ե�
        if(strncasecmp(sLine.c_str(), "GET ", 4) ==0
           || strncasecmp(sLine.c_str(), "POST ", 5) ==0
           || strncasecmp(sLine.c_str(), "HTTP/", 5) ==0)
        {
            continue;
        }

		string::size_type index = sLine.find(":");
		if(index != string::npos)
		{
            sHeader.insert(multimap<string, string>::value_type(TC_Common::trim(sLine.substr(0, index), " "), TC_Common::trim(sLine.substr(index + 1), " ")));
		}

		long offset = (long)(*ppChar - srcPtr);

		if(offset >= length)
		{
			break;
		}
	}

	return *ppChar;
}

void TC_Http::reset()
{
    _headers.clear();
    _headLength = 0;
    _content.clear();
    _bIsChunked = false;
}

/********************* TC_HttpCookie ***********************/

bool TC_HttpCookie::matchDomain(const string &sCookieDomain, const string &sDomain)
{
    string sCookieDomainNew = TC_Common::lower(sCookieDomain);

    //û�е���Զ��ӵ�
    if(sCookieDomainNew.find(".") != 0)
    {
        sCookieDomainNew = "." + sCookieDomainNew;
    }

    string::size_type pos = sCookieDomainNew.find(".");

    //sCookieDomain��������������
    if(pos == string::npos || (pos == 0 && sCookieDomainNew.rfind(".") == 0))
    {
        return false;
    }

    string sLowerDomain = TC_Common::lower(sDomain);

    //�����������
    if(sDomain.length() >= sCookieDomainNew.length() &&
       sLowerDomain.compare(sDomain.length() - sCookieDomainNew.length(), sCookieDomainNew.length(), sCookieDomainNew) == 0)
    {
        return true;
    }

    //ȥ��.���
    if(sLowerDomain == sCookieDomainNew.substr(1))
    {
        return true;
    }

    return false;
}

size_t TC_HttpCookie::matchPath(const string &sCookiePath, const string &sPath)
{
    if(sCookiePath.empty() || sPath.empty()) return 0;

    //��������/��ƥ��
    string sCookiePath1 = (sCookiePath.at(sCookiePath.length() - 1) == '/') ? sCookiePath:sCookiePath + "/";

    string sPath1 = (sPath.at(sPath.length() - 1) == '/') ? sPath:sPath + "/";

    if(sPath1.find(sCookiePath1) == 0)
    {
        return sCookiePath1.length();
    }

    return 0;
}

void TC_HttpCookie::clear()
{
    _cookies.clear();
}

bool TC_HttpCookie::fixDomain(const string &sDomain, string &sFixDomain)
{
    if(sDomain.empty())
    {
        return false;
    }

    sFixDomain = sDomain;

    //�Զ���.
    if(sDomain.at(0) != '.')
    {
        sFixDomain = "." + sDomain;
    }

    //domain��������
    if(sFixDomain.find(".") == sFixDomain.rfind("."))
        return false;

    return true;
}

void TC_HttpCookie::addCookie(const Cookie &cookie, list<Cookie> &cookies)
{
    string sDomain;

    Cookie cookieNew = cookie;

    if(!fixDomain(cookieNew._domain, sDomain))
        return;

    cookieNew._domain = sDomain;

    if(cookieNew._path.empty())
        return;

    list<Cookie>::iterator it = cookies.begin();

    string sName;
    if(cookieNew._data.size() >= 1)
    {
        sName = cookieNew._data.begin()->first;
    }

    while(it != cookies.end())
    {
        //���Cookie�Ƿ����
        if(isCookieExpires(*it))
        {
            cookies.erase(it++);
        }
        else if(strcasecmp(it->_domain.c_str(), cookieNew._domain.c_str()) == 0
            && strcmp(it->_path.c_str(), cookieNew._path.c_str()) == 0
            && it->_isSecure == cookieNew._isSecure)
        {
            if(it->_expires == cookieNew._expires)
            {
                //domain/path/expires��ƥ��������, ������cookie������
                cookieNew._data.insert(it->_data.begin(), it->_data.end());

                cookies.erase(it++);
            }
            else
            {
                //��ʱ�䲻һ��,������ͬ����key����ɾ��
                if(it->_data.find(sName) != it->_data.end())
                {
                    it->_data.erase(sName);
                }
                ++it;
            }
        }
        else
        {
           ++it;
        }
    }

    cookies.push_back(cookieNew);
}

void TC_HttpCookie::addCookie(const Cookie &cookie)
{
    addCookie(cookie, _cookies);
}

void TC_HttpCookie::addCookie(const list<Cookie> &cookie)
{
    list<Cookie>::const_iterator it = cookie.begin();

    while(it != cookie.end())
    {
        addCookie(*it);

        ++it;
    }
}

void TC_HttpCookie::addCookie(const string &sRspURL, const vector<string> &vCookies)
{
    TC_URL cURL;

    cURL.parseURL(sRspURL);

    string sRspURLDomain= TC_Common::lower(cURL.getDomain());

    string sRspURLPath  = cURL.getPath();

    for(size_t i = 0; i < vCookies.size(); i++)
    {
        //����һ��SetCookie
        vector<string> v = TC_Common::sepstr<string>(vCookies[i], ";");

        Cookie cookie;

        cookie._isSecure = false;
        cookie._expires  = 0;

        //����Cookie����
        for(size_t j = 0; j < v.size(); ++j)
        {
    		string::size_type index = v[j].find("=");

            string name;

            string value;

    		if(index != string::npos)
    		{
                name  = TC_Common::trim(v[j].substr(0, index), " ");

                value = TC_Common::trim(v[j].substr(index + 1));
    		}
            else
            {
                name  = TC_Common::trim(v[j]);
            }
            if(strcasecmp(name.c_str(), "secure") == 0)
            {
                cookie._isSecure = true;
            }
            else if(strcasecmp(name.c_str(), "expires") == 0)
            {
                struct tm stTm;
                //����ʱ���ʽ
                //expires=Mon, 09-May-41 08:39:32 GMT
                //expires=Thu, 01-Jan-1970 01:00:00 GMT
                value = TC_Common::replace(value, "-", " ");
                if(value.length() == 27  && value.at(11) == ' ' && value.at(14) == ' ')
                {
                    int year = TC_Common::strto<int> (value.substr(12, 2));
                    if(year >= 69 && year <= 99)
                        value = value.substr(0, 12) + "19" + value.substr(12);
                    else
                        value = value.substr(0, 12) + "20" + value.substr(12);
                }

                TC_Common::strgmt2tm(value, stTm);

                cookie._expires = timegm(&stTm);
            }
            else if(strcasecmp(name.c_str(), "path") == 0)
            {
                cookie._path = value;
            }
            else if(strcasecmp(name.c_str(), "domain") == 0)
            {
                cookie._domain = value;
            }
            else if(strcasecmp(name.c_str(), "httponly") == 0)
            {
                //TODO
                //cookie._isHttpOnly = true;
            }
            else
                cookie._data.insert(http_cookie_data::value_type(name, value));
        }

        ///������ƥ��domain/////////////////////////////////////////
        if(cookie._domain.empty())
            cookie._domain = sRspURLDomain;

        if(!fixDomain(cookie._domain, cookie._domain))
            continue;

        //ƥ������
        if(!matchDomain(cookie._domain, sRspURLDomain))
            continue;

        //�޸ĺ�ƥ��path/////////////////////////////
        if(cookie._path.empty())
        {
            string sCookiePath;

            //ȱʡ��ȫ·��
            string sRequest = sRspURLPath;

            string::size_type pos = sRequest.rfind("/");

            if(pos == string::npos)
                sCookiePath = "/";
            else
                sCookiePath = sRequest.substr(0, pos+1);

            cookie._path = sCookiePath;
        }

        //URL��Path��Χ��,Cookie ��Ч
        if(!matchPath(cookie._path, sRspURLPath))
            continue;

        //����Cookie
        addCookie(cookie);
    }
}

bool TC_HttpCookie::isCookieExpires(const Cookie &cookie) const
{
    //������
    if(cookie._expires !=0 && cookie._expires < time(NULL))
        return true;

    return false;
}

size_t TC_HttpCookie::isCookieMatch(const Cookie &cookie, const TC_URL &tURL) const
{
    //����û��ƥ��
    if(!matchDomain(cookie._domain, tURL.getDomain()))
        return 0;

    //·��û��ƥ��
    size_t len = matchPath(cookie._path, tURL.getPath());
    if(len == 0)
        return 0;

    //��ȫ��cookie,����ȫ��URL
    if(cookie._isSecure && (tURL.getType() != TC_URL::HTTPS))
        return 0;

    return len;
}

void TC_HttpCookie::getCookieForURL(const string &sReqURL, list<TC_HttpCookie::Cookie> &cookies)
{
    TC_URL tURL;

    tURL.parseURL(sReqURL);

    cookies.clear();

    list<Cookie>::iterator it = _cookies.begin();

    while(it != _cookies.end())
    {
        //���Cookie�Ƿ����
        if(isCookieExpires(*it))
        {
            _cookies.erase(it++);
            continue;
        }

        size_t len = isCookieMatch(*it, tURL);
        if(len == 0)
        {
            ++it;
            continue;
        }

        cookies.push_back(*it);

        ++it;
    }
}

void TC_HttpCookie::getCookieForURL(const string &sReqURL, string &sCookie)
{
    list<Cookie> cookies;

    sCookie.clear();

    getCookieForURL(sReqURL, cookies);

    list<Cookie>::iterator it = cookies.begin();
    while(it != cookies.end())
    {
        http_cookie_data::iterator itd = it->_data.begin();

        while(itd != it->_data.end())
        {
            //��ɾ����cookie�����
            if(itd->first != "" && itd->second != "" && TC_Common::lower(itd->second) != "null"
                    && TC_Common::lower(itd->second) != "deleted")
                sCookie += itd->first + "=" + itd->second + "; ";

            ++itd;
        }

        ++it;
    }

    //ȥ��ĩβ�� "; "
    if(sCookie.length() >= 2)
        sCookie = sCookie.substr(0, sCookie.length()-2);
}

list<TC_HttpCookie::Cookie> TC_HttpCookie::getSerializeCookie(time_t t)
{
    list<Cookie> cookies;

    list<Cookie>::iterator it = _cookies.begin();

    while(it != _cookies.end())
    {
        if(isCookieExpires(*it))
        {
            _cookies.erase(it++);
            continue;
        }
        else if(it->_expires != 0)  //�ǵ�ǰ�Ự��
        {
            cookies.push_back(*it);

            ++it;
        }
        else
            ++it;
    }

    return cookies;
}

list<TC_HttpCookie::Cookie> TC_HttpCookie::getAllCookie()
{
    deleteExpires(time(NULL));

    return _cookies;
}

void TC_HttpCookie::deleteExpires(time_t t, bool bErase)
{
    list<Cookie>::iterator it = _cookies.begin();

    while(it != _cookies.end())
    {
        if(bErase && it->_expires ==0)
        {
            _cookies.erase(it++);
            continue;
        }
        else if(isCookieExpires(*it))
        {
            _cookies.erase(it++);
            continue;
        }
        else
            ++it;
    }
}

/********************* TC_HttpResponse ***********************/

void TC_HttpResponse::parseResponseHeader(const char* szBuffer)
{
	const char **ppChar = &szBuffer;

	_headerLine = TC_Common::trim(getLine(ppChar));

    string::size_type pos = _headerLine.find(' ');

    if(pos != string::npos)
    {
        _version    = _headerLine.substr(0, pos);

        string left = TC_Common::trim(_headerLine.substr(pos));

        string::size_type pos1 = left.find(' ');

        if(pos1 != string::npos)
        {
            _status  = TC_Common::strto<int>(left.substr(0, pos));

            _about   = TC_Common::trim(left.substr(pos1 + 1));
        }
        else
        {
            _status  = TC_Common::strto<int>(left);

            _about   = "";
        }

        parseHeader(*ppChar, _headers);
        return;
    }
    else
    {
        _version = _headerLine;
        _status  = 0;
        _about   = "";
    }

//    throw TC_HttpResponse_Exception("[TC_HttpResponse_Exception::parseResponeHeader] http response format error : " + _headerLine);
}

void TC_HttpResponse::reset()
{
    TC_Http::reset();

    _status = 200;
    _about  = "OK";
    _version = "HTTP/1.1";

    _iTmpContentLength = 0;
}

vector<string> TC_HttpResponse::getSetCookie() const
{
    return getHeaderMulti("Set-Cookie");
}

bool TC_HttpResponse::incrementDecode(string &sBuffer)
{
    //����ͷ��
    if(_headLength == 0)
    {
        string::size_type pos = sBuffer.find("\r\n\r\n");

        if(pos == string::npos)
        {
            return false;
        }

        parseResponseHeader(sBuffer.c_str());

        http_header_type::const_iterator it = _headers.find("Content-Length");
        if(it != _headers.end())
        {
            _iTmpContentLength = getContentLength();
        }
        else
        {
            //û��ָ��ContentLength, ���յ��������ر�����
            _iTmpContentLength = -1;
        }

        _headLength = pos + 4;

        sBuffer = sBuffer.substr(_headLength);

        //�ض������Ϊ�ɹ���
        if((_status == 301 || _status == 302) && !getHeader("Location").empty())
        {
            return true;
        }

        //�Ƿ���chunk����
        _bIsChunked = (getHeader("Transfer-Encoding") == "chunked");

        //ɾ��ͷ������
        eraseHeader("Transfer-Encoding");
    }

    if(_bIsChunked)
    {
        while(true)
        {
            string::size_type pos   = sBuffer.find("\r\n");
            if(pos == string::npos)
                return false;

            //���ҵ�ǰchunk�Ĵ�С
            string sChunkSize       = sBuffer.substr(0, pos);
            int iChunkSize          = strtol(sChunkSize.c_str(), NULL, 16);

            if(iChunkSize <= 0)     break;      //����chunk���������

            if(sBuffer.length() >= pos + 2 + (size_t)iChunkSize + 2)   //���յ�һ��������chunk��
            {
                //��ȡһ��chunk������
                _content += sBuffer.substr(pos + 2, iChunkSize);

                //ɾ��һ��chunk
                sBuffer   =  sBuffer.substr(pos + 2 + iChunkSize + 2);
            }
            else
            {
                //û�н���������chunk
                return false;
            }

            setContentLength(getContent().length());
        }

        sBuffer = "";

        if(_iTmpContentLength == 0 || _iTmpContentLength == (size_t)-1)
        {
            setContentLength(getContent().length());
        }

        return true;
    }
    else
    {
        if(_iTmpContentLength == 0)
        {
            _content += sBuffer;
            sBuffer   = "";

            //�Զ���дcontent-length
            setContentLength(getContent().length());

            return true;
        }
        else if(_iTmpContentLength == (size_t)-1)
        {
            _content += sBuffer;
            sBuffer   = "";

            //�Զ���дcontent-length
            setContentLength(getContent().length());

            return false;
        }
        else
        {
            //������ģʽ, ���յ����ȴ���ͷ��Ϊֹ
            _content += sBuffer;
            sBuffer   = "";

            size_t iNowLength = getContent().length();

            //ͷ���ĳ���С�ڽ��յ�����, ����Ҫ�������ӽ���������buffer
            if(_iTmpContentLength > iNowLength)
                return false;

            return true;
        }
    }

    return true;
}

bool TC_HttpResponse::decode(const string &sBuffer)
{
    string::size_type pos = sBuffer.find("\r\n\r\n");

    if(pos == string::npos)
    {
        return false;
    }

    string tmp = sBuffer;

    incrementDecode(tmp);

    //body���ݳ���Ϊ0����û��content-length  �� ��chunkģʽ, ����Ϊ����ȫ��, ֱ�ӷ���
    if((_iTmpContentLength == 0 || _iTmpContentLength == (size_t)-1) && !_bIsChunked)
        return true;

    return getContentLength() + getHeadLength() <= sBuffer.length();
}

bool TC_HttpResponse::decode(const char *sBuffer, size_t iLength)
{
    assert(sBuffer != NULL);

    const char *p = strstr(sBuffer, "\r\n\r\n");
    if( p == NULL)
    {
        return false;
    }

    string tmp(sBuffer, iLength);

    incrementDecode(tmp);

    //body���ݳ���Ϊ0����û��content-length  �� ��chunkģʽ, ����Ϊ����ȫ��, ֱ�ӷ���
    if((_iTmpContentLength == 0 || _iTmpContentLength == (size_t)-1) && !_bIsChunked)
        return true;

    return (getContentLength() + getHeadLength() <= iLength);
}

string TC_HttpResponse::encode() const
{
    ostringstream os;
	os << _version << " " << _status << " " << _about << "\r\n";
	os << genHeader();
	os <<  "\r\n";

    os << _content;

    return os.str();
}

void TC_HttpResponse::encode(vector<char> &buffer) const
{
    buffer.clear();

    string s = encode();

    buffer.resize(s.length());
    memcpy(&buffer[0], s.c_str(), s.length());
}

void TC_HttpResponse::setResponse(int status, const string& about, const string& body)
{
    _status = status;
    _about  = about;
    _content= body;

    ostringstream os;

	os << _version << " " << _status << " " << _about;

    _headerLine = os.str();

    setHeader("Content-Length", TC_Common::tostr(_content.length()));
}

void TC_HttpResponse::setResponse(int status, const string& about, const char *sBuffer, size_t iLength)
{
    _status = status;
    _about  = about;

    ostringstream os;

	os << _version << " " << _status << " " << _about;

    _headerLine = os.str();

    if(sBuffer != NULL && iLength > 0)
    {
        _content.assign(sBuffer, iLength);
    }
    else
    {
        _content.clear();
    }


    setHeader("Content-Length", TC_Common::tostr(_content.length()));
}

void TC_HttpResponse::setResponse(const char *sBuffer, size_t iLength)
{
    setResponse(200, "OK", sBuffer, iLength);
}

/********************* TC_HttpRequest ***********************/

void TC_HttpRequest::reset()
{
    TC_Http::reset();

    _httpURL.clear();
}

string TC_HttpRequest::requestType2str(int iRequestType) const
{
    if(iRequestType == REQUEST_GET)
    {
        return "GET";
    }
    else if(iRequestType == REQUEST_POST)
    {
        return "POST";
    }

    assert(true);
    return "";
}

vector<string> TC_HttpRequest::getCookie()
{
    vector<string> v;

    http_header_type::const_iterator itEnd = _headers.end();

    for( http_header_type::const_iterator it = _headers.begin(); it != itEnd; ++it)
    {
        if(it->first == "Cookie")
        {
            v.push_back(it->second);
        }
    }

    return v;
}

void TC_HttpRequest::parseURL(const string& sUrl)
{
    _httpURL.parseURL(sUrl);

    //����Host
    if(getHeader("Host").empty())
    {
        string sPort = _httpURL.isDefaultPort() ? "" : ":" + _httpURL.getPort();

        //ȱʡ�˿ڿ��Բ��÷Ž�ȥ
        setHost(_httpURL.getDomain() + sPort);
    }
}

void TC_HttpRequest::encode(int iRequestType, ostream &os)
{
	os << requestType2str(iRequestType) << " " << _httpURL.getRequest() << " HTTP/1.1\r\n";
	os << genHeader();
	os << "\r\n";
}

void TC_HttpRequest::setGetRequest(const string &sUrl, bool bNewCreateHost)
{
    if(bNewCreateHost)
    {
        eraseHeader("Host");
    }

    parseURL(sUrl);

    _requestType    = REQUEST_GET;

    _content        = "";

    eraseHeader("Content-Length");
}

void TC_HttpRequest::setPostRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost)
{
    if(bNewCreateHost)
    {
        eraseHeader("Host");
    }

    parseURL(sUrl);

    _requestType    = REQUEST_POST;

    _content        = sPostBody;

    setHeader("Content-Length", TC_Common::tostr(_content.length()));
}

void TC_HttpRequest::setPostRequest(const string &sUrl, const char *sBuffer, size_t iLength, bool bNewCreateHost)
{
    assert(sBuffer != NULL);

    if(bNewCreateHost)
    {
        eraseHeader("Host");
    }

    parseURL(sUrl);

    _requestType    = REQUEST_POST;

    if(iLength > 0)
    {
        _content.assign(sBuffer, iLength);
    }
    else
    {
        _content.clear();
    }

    setHeader("Content-Length", TC_Common::tostr(_content.length()));
}

string TC_HttpRequest::encode()
{
//    assert(_requestType == REQUEST_GET || _requestType == REQUEST_POST || !_originRequest.empty());

    ostringstream os;

    if(_requestType == REQUEST_GET)
    {
        encode(REQUEST_GET, os);
    }
    else
    {
        setContentLength(_content.length());
        encode(REQUEST_POST, os);
        os << _content;
    }

    return os.str();
}

void TC_HttpRequest::encode(vector<char> &buffer)
{
//    assert(_requestType == REQUEST_GET || _requestType == REQUEST_POST || !_originRequest.empty());

    buffer.clear();

    string s = encode();
    buffer.resize(s.length());
    memcpy(&buffer[0], s.c_str(), s.length());
}

bool TC_HttpRequest::decode(const string &sBuffer)
{
    return decode(sBuffer.c_str(), sBuffer.length());
}

bool TC_HttpRequest::decode(const char *sBuffer, size_t iLength)
{
    assert(sBuffer != NULL);

    if(strncasecmp(sBuffer, "GET " ,4) !=0 && strncasecmp(sBuffer, "POST " ,5) !=0)
    {
        throw TC_HttpRequest_Exception("[TC_HttpRequest::decode] protocol not support, only support GET and POST");
    }

    if(strstr(sBuffer, "\r\n\r\n") == NULL)
    {
        return false;
    }

    _headLength = parseRequestHeader(sBuffer);

   bool bChunk = (getHeader("Transfer-Encoding") == "chunked");

    if(bChunk)
    {
        string sTmp(sBuffer + _headLength, iLength - _headLength);
        while(true)
        {
            string::size_type pos   = sTmp.find("\r\n");
            if(pos == string::npos)
                return false;

            //���ҵ�ǰchunk�Ĵ�С
            string sChunkSize       = sTmp.substr(0, pos);
            int iChunkSize          = strtol(sChunkSize.c_str(), NULL, 16);

            if(iChunkSize <= 0)     break;      //����chunk���������

            if(sTmp.length() >= pos + 2 + (size_t)iChunkSize + 2)   //���յ�һ��������chunk��
            {
                //��ȡһ��chunk������
                _content += sTmp.substr(pos + 2, iChunkSize);

                //ɾ��һ��chunk
                sTmp   =  sTmp.substr(pos + 2 + iChunkSize + 2);
            }
            else
            {
                //û�н���������chunk
                return false;
            }

            setContentLength(getContent().length());
        }
    }
    else
    {
        _content.assign((sBuffer + _headLength), iLength - _headLength);
//        _content    = sBuffer.substr(_headLength);
    }

    return (getContentLength() + getHeadLength() == iLength);
}


bool TC_HttpRequest::checkRequest(const char* sBuffer, size_t iLen)
{
   if(strncasecmp(sBuffer, "GET " ,4) !=0 && strncasecmp(sBuffer, "POST " ,5) !=0)
   {
	   throw runtime_error("[TC_HttpRequest::checkRequest] protocol not support, only support GET and POST");
   }

   const char *p = strstr(sBuffer, "\r\n\r\n");
   if( p == NULL)
   {
	   return false;
   }

   size_t pos = p - sBuffer;

   size_t iHeadLen= pos + 4;

   const char **ppChar	= &sBuffer;

   bool bChunk = false;

   //�ҵ�\r\n\r\n֮ǰ�ĳ��ȱ�ʾ
   while(true)
   {
	   size_t iMoveLen= (*ppChar) - sBuffer;
		if ( iMoveLen >= iHeadLen )
	   {
		   //MTT_ERRDAY << "parseHttp WARN: iMoveLen >= iLen" << "|" << iMoveLen << "|" <<iLen<<endl;
		   break;
	   }

	   size_t len = 0;
	   string sLine = getLine(ppChar, iHeadLen-iMoveLen);
	   if(sLine == "")
	   {
		   len = 0;
	   }
	   else if(strncasecmp(sLine.c_str(), "Transfer-Encoding:", 18) == 0)
	   {
		   bChunk = (taf::TC_Common::trim(sLine.substr(18)) == "chunked");
		   if(bChunk) break;
	   }
	   else if(strncasecmp(sLine.c_str(), "Content-Length:", 15) != 0)
	   {
		   continue;
	   }
	   else
	   {
		   len = taf::TC_Common::strto<size_t>(TC_Common::trim(sLine.substr(15), " "));
	   }

	   if(len + pos + 4 <= iLen)
	   {
		   return true;
	   }
	   else
	   {
		   break;

	   }
   }

/*
	if(bChunk)
	{
		string sTmp = string(p + 4, iLen - iHeadLen);
		while(true)
	    {
	        string::size_type pos   = sTmp.find("\r\n");
	        if(pos == string::npos)
	        {
	            return false;
			}

	        //���ҵ�ǰchunk�Ĵ�С
	        string sChunkSize       = sTmp.substr(0, pos);
	        int iChunkSize          = strtol(sChunkSize.c_str(), NULL, 16);

	        if(iChunkSize <= 0)
	        {
	        	return true; //����chunk���������
			}
	        if(sTmp.length() >= pos + 2 + (size_t)iChunkSize + 2)   //���յ�һ��������chunk��
	        {
	            //ɾ��һ��chunk
	            sTmp   =  sTmp.substr(pos + 2 + iChunkSize + 2);
	        }
	        else
	        {
				return false;
	        }
	    }
    }
*/
    if(bChunk)
    {
        int	remain_len = iLen - iHeadLen;
        int move_len = 0;
        const char * pCur = p + 4;
        while(true)
        {
            p = strstr(pCur , "\r\n");
            if( p == NULL )
            {
                return false;
            }

            //���ҵ�ǰchunk�Ĵ�С
            int iChunkSize = strtol(string(pCur, p - pCur).c_str(), NULL, 16);
             if(iChunkSize <= 0)
            {
                return true; //����chunk���������
            }

            move_len = (p - pCur) + 2 + iChunkSize + 2;
            if( remain_len >= move_len )   //���յ�һ��������chunk��
            {
                //�ƶ�����һ��chunk
                remain_len -= move_len;
                pCur = p + 2 + iChunkSize + 2;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
//	 throw TC_HttpRequest_Exception("[TC_HttpRequest::checkRequest] Content-Length not exists.");

//	 return true;
}


size_t TC_HttpRequest::parseRequestHeader(const char* szBuffer)
{
    const char *szBuffer_copy = szBuffer;
    const char **ppChar = &szBuffer;

    /* parse the first line and get status */
    string sLine = getLine(ppChar);

    string::size_type pos = sLine.find(" ");
    if(pos == string::npos)
    {
        throw TC_HttpRequest_Exception("[TC_HttpRequest::parseRequestHeader] http request format error: " + sLine);
    }

    string sMethod = TC_Common::trim(sLine.substr(0, pos));

    //������������
    if(sMethod == "GET")
    {
        _requestType = REQUEST_GET;
    }
    else if(sMethod == "POST")
    {
        _requestType = REQUEST_POST;
    }
    else
    {
        throw TC_HttpRequest_Exception("[TC_HttpRequest::parseRequestHeader] http request command error: " + sMethod);
    }

    string::size_type pos1 = sLine.rfind(" ");
    if(pos1 == string::npos || pos1 <= pos)
    {
        throw TC_HttpRequest_Exception("[TC_HttpRequest::parseRequestHeader] http request format error: " + sLine);
    }

    //URL��ַ
    string sURL = TC_Common::trim(sLine.substr(pos+1, pos1 - pos));

    //HTTPЭ��汾
    string sVersion = TC_Common::upper(TC_Common::trim(sLine.substr(pos1+1)));

    if(sVersion != "HTTP/1.1" || sVersion != "HTTP/1.0")
    {
        sVersion = "HTTP/1.1";
    }

    size_t n = parseHeader(*ppChar, _headers) - szBuffer_copy;

    if(strncasecmp(sURL.c_str(), "https://", 8) !=0 )
    {
        if(strncasecmp(sURL.c_str(), "http://", 7) !=0 )
        {
            sURL = "http://" + getHost() + sURL;
        }
    }

    parseURL(sURL);

    return n;
}

void TC_HttpRequest::getHostPort(string &sDomain, uint32_t &iPort)
{
    sDomain = _httpURL.getDomain();

    iPort   = TC_Common::strto<uint32_t>(_httpURL.getPort());
}

int TC_HttpRequest::doRequest(TC_HttpResponse &stHttpRsp, int iTimeout)
{
    //ֻ֧�ֶ�����ģʽ
    setConnection("close");

    string sSendBuffer = encode();

    string sHost;
    uint32_t iPort;

    getHostPort(sHost, iPort);

    TC_TCPClient tcpClient;
    tcpClient.init(sHost, iPort, iTimeout);

    int iRet = tcpClient.send(sSendBuffer.c_str(), sSendBuffer.length());
    if(iRet != TC_ClientSocket::EM_SUCCESS)
    {
        return iRet;
    }

    stHttpRsp.reset();

    string sBuffer;

    char *sTmpBuffer = new char[10240];
    size_t iRecvLen  = 10240;

    while(true)
    {
        iRecvLen = 10240;

        iRet = tcpClient.recv(sTmpBuffer, iRecvLen);

        if(iRet == TC_ClientSocket::EM_SUCCESS)
            sBuffer.append(sTmpBuffer, iRecvLen);

        switch(iRet)
        {
        case TC_ClientSocket::EM_SUCCESS:
            if(stHttpRsp.incrementDecode(sBuffer))
            {
                delete []sTmpBuffer;
                return TC_ClientSocket::EM_SUCCESS;
            }
            continue;
        case TC_ClientSocket::EM_CLOSE:
            delete []sTmpBuffer;
            stHttpRsp.incrementDecode(sBuffer);
            return TC_ClientSocket::EM_SUCCESS;
        default:
            delete []sTmpBuffer;
            return iRet;
        }
    }

    assert(true);

    return 0;
}

}

