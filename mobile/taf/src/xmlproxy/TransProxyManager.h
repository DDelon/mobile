#ifndef __TRANSPROXYMANAGER_H__
#define __TRANSPROXYMANAGER_H__

#include "util/tc_singleton.h"
#include "servant/Application.h"

class TransProxyManager : public TC_Singleton<TransProxyManager>, public TC_ThreadLock
{
public:
	TransProxyManager(){}

	/**
	 * ���������ļ� 
	 *  
	 * @param sFile 
	 * @return string 
	 */

	std::string loadproxy(const std::string & sFile);

	/**
	 * ��������������ô����� 
	 *  
	 *  @return ServantPrx;
	 */

	ServantPrx getProxy(const std::string & sMidName);

protected:
	std::map<std::string, ServantPrx> _mapProxy;	//����sMidName��ServantProxy��ת��
	std::map<std::string, std::string> _mapName;	//����sMidName��ServantName��ת��
};

#endif

