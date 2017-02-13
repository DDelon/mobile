#include "GetSecretImp.h"
#include "servant/Application.h"
#include <algorithm>
#include <functional>
#include "ConstValue.h"
#include "GetSecretCallback.h"

using namespace std;

//////////////////////////////////////////////////////
// ȡ�����б� callback
//////////////////////////////////////////////////////
taf::Int32 GetSecretCallback::getFriendsListCallback(SecretListReq tReq, taf::JceCurrentPtr current, GetActiveFriendsRsp const& tRsp)
{
	Int32 iRet = FAILURE;

	LOG->debug() << __FUNCTION__ << " begin " << endl;

	// this->returnStatus(0, current);
	// return SUCCESS;
	PROC_BEGIN
	__TRY__

	FrdsVector            vFrdsAll;
	FrdsVector            vFrdsFavor;
	FrdsVectorPair        tFrdsPair;
	Int32                 iRegTime;
	GetActiveFriendsRsp   tFrdsRsp = tRsp;

	// ת����Ѷ���
	FrdsRelat(tFrdsRsp.vPostFriends);
	FrdsRelat(tFrdsRsp.vFavorFriends);

	vFrdsAll.swap(tFrdsRsp.vPostFriends);
	vFrdsFavor.swap(tFrdsRsp.vFavorFriends);

	if (!vFrdsAll.empty())
	{
		FrdsIterator iBegin = vFrdsAll.begin();
		FrdsIterator iEnd = vFrdsAll.end();
		for (FrdsIterator i = iBegin; i != iEnd; ++i)
		{
			if (i->iRelat == FRIEND)
			{
				tFrdsPair.first.push_back(*i);
			}
			else
			{
				tFrdsPair.second.push_back(*i);
			}
		}
	}

	// �û�ע��ʱ��
	iRegTime = tFrdsRsp.iRegistTime;

	display(" Active Post  Friends List: ", vFrdsAll);
	display(" Active Favor Friends List: ", vFrdsFavor);

	iRet = this->getSecretList(tReq, vFrdsAll, vFrdsFavor, tFrdsPair, iRegTime, current);
	if (iRet == FAILURE)
	{
        LOG->debug() << __FUNCTION__<< " get Secret List failure! "<<endl;
		break;
	}

	iRet = SUCCESS;
	__CATCH__
	PROC_END

	if (iRet != SUCCESS)
	{
		this->returnStatus(iRet, current);
	}

	LOG->debug() << __FUNCTION__ << " end " << endl;

	return iRet;
}




//////////////////////////////////////////////////////
// ��ȡϵͳ�Ƽ� �����Ƽ�
//////////////////////////////////////////////////////
taf::Int32 GetSecretCallback::getRecommendSecrets(SecretListReq const& tReq, 
											      SecretListRsp& tRsp, 
												  Int32 iFrdsNum, 
												  FrdsVectorPair const& tFrdsPair, 
												  FrdsVector const& vFrdsAll, 
												  FrdsVector const& vFrdsFavor, 
												  Int32 iRegTime,
												  ScrtVector& vSecrets,
												  taf::JceCurrentPtr current)
{
    int iRet;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;
    
    ScrtVector    vRmdScrts;        // ϵͳ�Ƽ�����
	ScrtVector    vFvrScrts;        // �����޹�����

	// ��ȡϵͳ�Ƽ����� 
	{
		TC_ThreadLock::Lock  tLock(*LockDbHandle::getInstance());
    	iRet = LockDbHandle::getInstance()->getRecommendSecrets(tReq.tUserId, _u24SecretsNum, _u72SecretsNum, vSecrets);
	}
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Sys RecommendSecrets failure! "<<endl;
        return iRet;
    }

    LOG->info() << __FUNCTION__<< " get Sys RecommendSecrets success! "<<endl;

    // �жϺ�������
    if (size_t(iFrdsNum) < _uLockFriendsNum)
    {
        //��ȡ1�������޹��������б� 1�ȹ�ϵ ���20��
        iRet = this->getBatchUserFavorSecrets(tReq, tRsp, _uMinFrdsFvrNum, tFrdsPair, vFrdsFavor, vFrdsFavor, iRegTime, vSecrets, current); 
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " 1favor get Friends Favor Secrets failure! "<<endl;
            return iRet;
        }
        LOG->info() << __FUNCTION__<< " 1favor get Friends Favor Secrets success! "<<endl;
    }
    else 
    {
        // ��ȡ3�������޹��������б� 1�ȹ�ϵ ���20�� 
        iRet = this->getBatchUserFavorSecrets(tReq, tRsp, _uMaxFrdsFvrNum, tFrdsPair, vFrdsFavor, vFrdsFavor, iRegTime, vSecrets, current); 
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " 3favor get Friends Favor Secrets failure! "<<endl;
            return iRet;
        }
        LOG->info() << __FUNCTION__<< " 3favor get Friends Favor Secrets success! "<<endl;
    }

    display(" Friends Favor Vector Secrets : ", vFvrScrts);

    if (tReq.tRange.lBeginId != SECRET_ID_BEGIN)
    {
        vFvrScrts.erase(remove_if(vFvrScrts.begin(),  vFvrScrts.end(),  bind2nd(greater<Int64>(), tReq.tRange.lBeginId)), vFvrScrts.end()); 
        vRmdScrts.erase(remove_if(vRmdScrts.begin(),  vRmdScrts.end(),  bind2nd(greater<Int64>(), tReq.tRange.lBeginId)), vRmdScrts.end()); 
    }

    // ����
    copy(vRmdScrts.begin(), vRmdScrts.end(), back_inserter(vSecrets));
    copy(vFvrScrts.begin(), vFvrScrts.end(), back_inserter(vSecrets));

    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// ��ȡ���������б�
//////////////////////////////////////////////////////
taf::Int32 GetSecretCallback::getFriendsSecrets(SecretListReq const& tReq, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, Int64& lNextBeginId, Int32& iLockCount, ScrtVector& vSecrets)
{
    int iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;
  
    ScrtVector    vSecretsFriends;     // ���������б�

    // ������������б����20���谴beginIDȡ
    size_t iR   = 0;
    size_t iC   = 0;
    size_t iSum = 0;
    size_t iFrdsNum = tFrdsPair.first.size();
    FrdsVector  vFriends;

    LOG->debug() << __FUNCTION__<< " Friends Num "<<iFrdsNum<<" _uLockFriendsNum: "<<_uLockFriendsNum<<endl;

    if (size_t(iFrdsNum) >= _uLockFriendsNum)
    {
        iR = vFrdsAll.size() / _uTopN;
        iC = vFrdsAll.size() % _uTopN;
        iSum = iR + (iC > 0 ? 1 : 0);
        vFriends = vFrdsAll;
    }
    else
    {
        iR = tFrdsPair.second.size() / _uTopN;
        iC = tFrdsPair.second.size() % _uTopN;
        iSum = iR + (iC > 0 ? 1 : 0);
        vFriends = tFrdsPair.second;
    }

    LOG->debug() << __FUNCTION__<< " iR "<<iR<<" iC "<<iC<<" iSum "<<iSum<<endl;

    for (size_t i = 0; i < iSum; i++)
    {
        FrdsVector   vTmp;
        if (vFriends.size() < _uTopN * (i+1)) 
        { 
            copy(vFriends.begin(), vFriends.end(), back_inserter(vTmp));
        }
        else
        {
            copy(vFriends.begin(), vFriends.begin() + (i+1) * _uTopN, back_inserter(vTmp));
        }

        // ��ȡ���ѣ����ѵĺ��ѵ������б�; 
		{
			TC_ThreadLock::Lock  lock( *LockDbHandle::getInstance());
			iRet = LockDbHandle::getInstance()->getAllUserSecrets(vTmp, tReq.iCount, tReq.tRange.lBeginId, lNextBeginId, vSecretsFriends);
		}
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " get Friends User Secrets  failure! "<<endl;
            break;
        }
        display(" Friends Vector Secrets : ", vSecretsFriends);
        LOG->info() << __FUNCTION__<< " get Friends User Secrets success! "<<endl;
        if (vFriends.size() > (i+1)*_uTopN)
        {
            if (vFriends[(i+1)*_uTopN+1].iTimespan > 0)
            {
                vSecretsFriends.erase(remove_if(vSecretsFriends.begin(), vSecretsFriends.end(), LessTime(vFriends[(i+1)*_uTopN+1].iTimespan)), vSecretsFriends.end());
            }
            if (vSecretsFriends.size() >= (size_t)tReq.iCount)
            {
                break;
            }
        }
    }

    // �жϺ�������
    if (size_t(iFrdsNum) < _uLockFriendsNum)
    {
        //��ѯ���ѵ�������;
        iLockCount = vSecretsFriends.size();
 
        if (iFrdsNum == 0 || iLockCount == 0)
        {
            // ���������Ϊ0��Ĭ������Ϊ1
            iLockCount = 1;
        }

        LOG->info() << __FUNCTION__<< " 1favor get Friends Favor Secrets success! "<<endl;
    }
    else 
    {
        LOG->info() << __FUNCTION__<< " 3favor get Friends Favor Secrets success! "<<endl;
        // ����
        iLockCount = 0;
    }

    // ���ƺ��������б�
    if (!vSecretsFriends.empty())
    {
        copy(vSecretsFriends.begin(), vSecretsFriends.end(), back_inserter(vSecrets));
    }

    iRet = SUCCESS; 
    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// ȡ�����б�
//////////////////////////////////////////////////////
taf::Int32 GetSecretCallback::getFriendsList(UserId const& tUserId, FrdsVector& vFrdsFavor, FrdsVectorPair& tFrdsPair, FrdsVector& vFrdsAll, Int32& iRegTime)
{
    Int32 iRet;

    LOG->debug() <<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    GetActiveFriendsReq   tFrdsReq;
    GetActiveFriendsRsp   tFrdsRsp;
    tFrdsReq.lId = tUserId.lUId;

    iRet = _pUserProxy->taf_hash(tUserId.lUId)->getActiveFriends(tFrdsReq, tFrdsRsp);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< "get Active FriendsList  1 + 2 failure !"<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<<  "get Active FriendsList  1 + 2 SUCCESS !"<<endl;

    // ת����Ѷ���
    FrdsRelat(tFrdsRsp.vPostFriends);
    FrdsRelat(tFrdsRsp.vFavorFriends);

    vFrdsAll.swap(tFrdsRsp.vPostFriends);
    vFrdsFavor.swap(tFrdsRsp.vFavorFriends);

    if (!vFrdsAll.empty())
    {
        FrdsIterator iBegin = vFrdsAll.begin();
        FrdsIterator iEnd   = vFrdsAll.end();
        for (FrdsIterator i = iBegin; i != iEnd; ++i)
        {
            if (i->iRelat == FRIEND)
            {
                tFrdsPair.first.push_back(*i);
            }
			else
			{
				tFrdsPair.second.push_back(*i);
			}
        }
    }

	// �û�ע��ʱ��
	iRegTime = tFrdsRsp.iRegistTime;

    display(" Active Post  Friends List: ", vFrdsAll);
    display(" Active Favor Friends List: ", vFrdsFavor);

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;
    LOG->debug() <<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// ����ӿ�: ��ȡ�����б�
//////////////////////////////////////////////////////
taf::Int32 GetSecretCallback::getSecretList(const QUAN::SecretListReq & tReq, FrdsVector& vFrdsAll, FrdsVector& vFrdsFavor, FrdsVectorPair& tFrdsPair, Int32 iRegTime, taf::JceCurrentPtr current)
{
    Int32 iRet = FAILURE;

    // �������
    string sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tReq);
    LOG->debug() <<endl;
    LOG->debug() <<"--------------------now  access-------------------------------------"<<endl;
    LOG->debug() << __FUNCTION__<< " begin "<<sLog<<endl;
    FDLOG() <<endl;
    FDLOG() <<"--------------------now  access-------------------------------------"<<endl;
    FDLOG() << __FUNCTION__<< " begin "<<sLog<<endl;

	SecretListRsp    tRsp;

	PROC_BEGIN
	__TRY__

	ScrtVector    vSecrets;         // ����ID

	// ��ѯ�������Ƿ�������
	{
		TC_ThreadLock::Lock  tLock( *LockDbHandle::getInstance());
		iRet = LockDbHandle::getInstance()->getUserSecretListFromCache(tReq.tUserId.lUId, tReq.tRange.lBeginId, tReq.iCount, vSecrets, tRsp.iLockCount);
	}
    if (iRet == SUCCESS)
    {
        LOG->debug() << __FUNCTION__<< " get UserSecretList From Cache SUCCESS! "<<endl;
        tRsp.lNextBeginId = vSecrets.back();
        display(" Cache Secret Accept: ", vSecrets);
    }
    else if (iRet == TIMEOUT)
    {
        LOG->debug() << __FUNCTION__<< " get Pull User Secret List From DB ! "<<endl;

        // ������ȡ�������ݴ�(��cache)����DB������
        iRet = this->getPullSecretList(tReq, tRsp, tFrdsPair, vFrdsAll, vFrdsFavor, iRegTime, vSecrets, current);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " get Pull Secret List FAILURE! "<<endl;
            break;
        }
		else
		{
			return SUCCESS;
		}
    }
    else
    {
        LOG->debug() << __FUNCTION__<< " get User Secret List From Cache failure! "<<endl;
        break;
    }

	// ���������ҳ
   	iRet = this->uniqueSecretList(tReq, tRsp, tFrdsPair, vSecrets, iRegTime, current);
   	if (iRet == FAILURE)
   	{
       	LOG->debug() << __FUNCTION__<< " unique Secret List FAILURE! "<<endl;
       	break;
   	}

    iRet = SUCCESS;
    __CATCH__
    PROC_END

	if (iRet != SUCCESS)
	{
		this->returnStatus(iRet, current);
	}

    LOG->debug() << __FUNCTION__<< " the Lock Count : "<<tRsp.iLockCount<<" NextBeginId : "<<tRsp.lNextBeginId<<endl;
    display(" Vector Secrets ", tRsp.vSecrets);
    sLog = " UId: " + TC_Common::tostr(tReq.tUserId.lUId) + JCETOSTR(tRsp);
    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    LOG->debug() <<"---------------------- now  exit -----------------------------------"<<endl;
    LOG->debug() <<endl;
    FDLOG() << __FUNCTION__<< " iRet: "<<iRet<<" | "<<sLog<<endl;
    FDLOG() <<"---------------------- now  exit -----------------------------------"<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// ��ȡ�����б� -- ������
//////////////////////////////////////////////////////
taf::Int32 GetSecretCallback::getPullSecretList(const QUAN::SecretListReq & tReq,
                                           QUAN::SecretListRsp &tRsp, 
                                           FrdsVectorPair const& tFrdsPair, 
                                           FrdsVector const& vFrdsAll, 
                                           FrdsVector const& vFrdsFavor, 
										   Int32 iRegTime,
                                           ScrtVector& vSecrets,
										   taf::JceCurrentPtr current)
{
    Int32 iRet = FAILURE;

    // �������
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    // ���û���������б�
    FrdsVector vMes;
    FriendsList tMe;
    tMe.lUId = tReq.tUserId.lUId;
    tMe.iRelat  = ME;
	vMes.push_back(tMe);

	// ��ȡ�Լ��������б�;  
	{
		TC_ThreadLock::Lock  lock( *LockDbHandle::getInstance());
    	iRet = LockDbHandle::getInstance()->getAllUserSecrets(vMes, tReq.iCount, tReq.tRange.lBeginId, tRsp.lNextBeginId, vSecrets);
	}
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Secrets MySelf failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Secrets MySelf success! "<<endl;

    display(" MySelf Vector Secrets : ", vSecrets);

    // ��ȡ���������б�
    Int32 iFrdsNum = tFrdsPair.first.size();
    iRet = this->getFriendsSecrets(tReq, tFrdsPair, vFrdsAll, tRsp.lNextBeginId, tRsp.iLockCount, vSecrets);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Secrets Friends failure! "<<endl;
        break;
    }

    LOG->info() << __FUNCTION__<< " get Friends Secrets success! "<<endl;

    // ��ȡϵͳ,�����Ƽ�
    iRet = this->getRecommendSecrets(tReq, tRsp, iFrdsNum, tFrdsPair, vFrdsAll, vFrdsFavor, iRegTime, vSecrets, current);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Recommend Secrets failure! "<<endl;
        break;
    }

    LOG->debug() << __FUNCTION__<< " get Recommend Secrets success! "<<endl;

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;
    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// �������������б��ҳ
//////////////////////////////////////////////////////
taf::Int32 GetSecretCallback::uniqueSecretList(const QUAN::SecretListReq & tReq, QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, ScrtVector& vSecrets, Int32 iRegTime, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

	// ����1�����Ѳ���
	if (tFrdsPair.first.size() >= 1 && tRsp.iLockCount != 0)
	{
        // 2��Сʱ���Զ����� 
		time_t iTime = TC_TimeProvider::getInstance()->getNow();
        LOG->debug() << __FUNCTION__<< " now Time :"<<iTime<<" RegTime: "<<iRegTime<<endl;
		if (iTime - iRegTime > 7200)
		{
			tRsp.iLockCount = 0;
		}
	}

    // ��ȡ���������б�
    iRet = this->getNearSecrets(tReq, tRsp, tFrdsPair, vSecrets, iRegTime, current);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " get Near Secrets failure! "<<endl;
		break;
    }

    iRet = SUCCESS;
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;

    return iRet;
}

//////////////////////////////////////////////////////
// ��ȡ�û������б�
//////////////////////////////////////////////////////
Int32 GetSecretCallback::getBatchUserFavorSecrets(SecretListReq const& tReq, 
												  SecretListRsp& tRsp, 
												  Int32 iFrdsFvrNum, 
												  FrdsVectorPair const& tFrdsPair, 
												  FrdsVector const& vFrdsAll, 
												  FrdsVector const& vFrds, 
												  Int32 iRegTime,
												  ScrtVector& vSecrets,
												  taf::JceCurrentPtr current)
{
    int iRet = FAILURE;

    LOG->debug() << __FUNCTION__<< " begin "<<endl;

	// this->returnStatus(0, current);
	// return SUCCESS;

    PROC_BEGIN
    __TRY__

	GetSecretCallback  tGetSecret(*this);
	TC_AutoPtr< SharedCallback< PostPrxCallback > > ptr = new SharedCallback< PostPrxCallback >(current);
	ptr->setBindgetUserOpBatch(boost::bind(&GetSecretCallback::getBatchUserFavorSecretsCallback, tGetSecret, tReq, tRsp, iFrdsFvrNum, tFrdsPair, iRegTime, vSecrets, _1, _2));
    _pPostProxy->taf_hash(tReq.tUserId.lUId)->async_getUserOpBatch(ptr, tReq.tUserId.lUId, vFrds);
	current->setResponse(false);

    iRet = SUCCESS;

	__CATCH__
	PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

Int32 GetSecretCallback::getBatchUserFavorSecretsCallback(SecretListReq tReq, 
														  SecretListRsp tRsp, 
														  Int32 iFrdsFvrNum,
														  FrdsVectorPair tFrdsPair, 
														  Int32 iRegTime,
														  ScrtVector vSecrets, 
														  taf::JceCurrentPtr current, 
														  vector<map<Int64, Int32> > const& vOp)
{
    Int32 iRet = FAILURE;

    LOG->info() << __FUNCTION__<<  " begin "<<endl;

	// this->returnStatus(0, current);
	// return SUCCESS;

	PROC_BEGIN
	__TRY__

    map<Int64, Int32> mOp;
    for (vector<map<Int64, Int32> >::const_iterator i = vOp.begin(); i != vOp.end(); ++i)
    {
        for (map<Int64, Int32>::const_iterator j = i->begin(); j != i->end(); ++j)
        {
            if (j->second == USER_FAVORY)
            {
                map<Int64, Int32>::iterator pos = mOp.find(j->first);
                if (pos != mOp.end())
                {
                    mOp[j->first] += 1;
                }
                else
                {
                    mOp.insert(make_pair(j->first, 1));
                }
            }
        }
    }

    for (map<Int64, Int32>::iterator i = mOp.begin(); i != mOp.end(); ++i)
    {
        if (i->second >= iFrdsFvrNum) 
        {
            vSecrets.push_back(i->first);
        }
    }

    if (tReq.tRange.lBeginId != SECRET_ID_BEGIN)
    {
        vSecrets.erase(remove_if(vSecrets.begin(),  vSecrets.end(),  bind2nd(greater<Int64>(), tReq.tRange.lBeginId)), vSecrets.end()); 
    }

    // ���� 
    stable_sort(vSecrets.rbegin(),  vSecrets.rend());
    // ȥ��
    vSecrets.erase(unique(vSecrets.begin(), vSecrets.end()), vSecrets.end());

	display(" Pull List: ", vSecrets);
	LOG->info() << __FUNCTION__ << " get Pull Secret List SUCCESS! " << endl;

	// ���������������б����������
	{
		TC_ThreadLock::Lock  tLock(*LockDbHandle::getInstance());
		iRet = LockDbHandle::getInstance()->setBatchSecretsToCache(vSecrets, tReq.tUserId.lUId, tRsp.iLockCount);
	}
	if (iRet == FAILURE)
	{
		LOG->debug() << __FUNCTION__ << " Set Batch Secrets ToCache failure! " << endl;
		break;
	}

	LOG->info() << __FUNCTION__ << " Set Batch Secrets ToCache success! " << endl;

    // ���������ҳ
    iRet = this->uniqueSecretList(tReq, tRsp, tFrdsPair, vSecrets, iRegTime, current);
    if (iRet == FAILURE)
    {
        LOG->debug() << __FUNCTION__<< " unique Secret List FAILURE! "<<endl;
        break;
    }

    iRet = SUCCESS;

    __CATCH__
    PROC_END

	if (iRet != SUCCESS)
	{
		this->returnStatus(iRet, current);
	}

    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;
}

taf::Int32 GetSecretCallback::test(taf::Int64 lId,taf::JceCurrentPtr current)
{
    string sId;
    getId(2, sId); 
    LOG->debug() <<"---------------------------------------------------------"<<endl;
    LOG->debug() <<"the id is: "<<sId<<endl;

    time_t now = TC_TimeProvider::getInstance()->getNow();

    Int64 lTmp = (now & 0x00000000FFFFFFFF) << 32;

    Int64 llSId = lTmp + ((2 & 0x00000000FFFFFFFF) ^ gUIdXorNum);
    LOG->debug() <<"the id is: "<<now<<endl;
    LOG->debug() <<"the id is: "<<(now & 0x00000000FFFFFFFF)<<endl;
    LOG->debug() <<"the id is: "<<lTmp<<endl;
    LOG->debug() <<"the id is: "<<((2 & 0x00000000FFFFFFFF) ^ gUIdXorNum)<<endl;
    LOG->debug() <<"the id is: "<<llSId<<endl;
    LOG->debug() <<"the de id is: "<<decodeSecretId(llSId)<<endl;
    LOG->debug() <<"the de id is: "<<decodeSecretIdUId(llSId)<<endl;

    LOG->debug() <<"---------------------------------------------------------"<<endl;

    
    long lEncodeId = encodeUId(lId);
    LOG->debug() <<"encode UId: "<<lEncodeId<<" MAX "<<LONG_MAX<<endl;
    LOG->debug() <<"decode UId: "<<decodeUId(lEncodeId)<<endl;

    return SUCCESS;
}

taf::Int32 GetSecretCallback::getNearSecretsCallback(const QUAN::SecretListReq & tReq, 
													QUAN::SecretListRsp &tRsp, 
													FrdsVectorPair const& tFrdsPair, 
													ScrtVector& vSecrets, 
													Int32 iRegTime, 
													taf::JceCurrentPtr current, 
													LbsRsp const& tLbsRsp)
{
	ScrtMap vMaps;
    Int32 iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    for (vector<LbsUser>::const_iterator i = tLbsRsp.vId.begin(); i != tLbsRsp.vId.end(); ++i)
    {
        // LOG->debug() << __FUNCTION__ << "lSId: " <<i->lId<<" Distance: "<<i->iDistance<< endl;
        vMaps.insert(make_pair(i->lId, i->iDistance * 1000));
        vSecrets.push_back(i->lId);
    }

    // ���� 
    stable_sort(vSecrets.rbegin(), vSecrets.rend());
    // ȥ��
    vSecrets.erase(unique(vSecrets.begin(), vSecrets.end()), vSecrets.end());
  
    // ��������
    if (!vSecrets.empty())
    {
        if (tReq.tRange.lBeginId == SECRET_ID_BEGIN || vSecrets[0] < tReq.tRange.lBeginId)
        {
        }
        else
        {
            vSecrets.erase(remove_if(vSecrets.begin(), vSecrets.end(), bind2nd(greater_equal<Int64>(), tReq.tRange.lBeginId)), vSecrets.end());
        }

        tRsp.lNextBeginId = vSecrets.back();
    }
    else
    {
        tRsp.lNextBeginId = SECRET_ID_END;
    }

    if (!vSecrets.empty())
    {
        // display(" the SECRET ID: ", vSecrets);

        // ����IDת������
        iRet = this->tranSecretsId(tReq, tRsp, tFrdsPair, vSecrets, vMaps, current);
        if (iRet == FAILURE)
        {
            LOG->debug() << __FUNCTION__<< " tran SecretsId failure! "<<endl;
            break;
        }
    }
    else
    {
        tRsp.lNextBeginId = SECRET_ID_END;
    }

	    // ���Ƴ���
    if (tRsp.vSecrets.size() >= size_t(tReq.iCount))
    {
        tRsp.vSecrets.resize(tReq.iCount);
        if (tRsp.lNextBeginId < tRsp.vSecrets.back().lSId)
            tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
    }

    iRet = SUCCESS; 
	__CATCH__
	PROC_END

	if (iRet != SUCCESS)
	{
		this->returnStatus(iRet, current);
	}

    LOG->debug() << __FUNCTION__<< " end iRet:"<<iRet<<endl;

    return iRet;
}

taf::Int32 GetSecretCallback::getNearSecrets(const QUAN::SecretListReq & tReq, QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, ScrtVector& vSecrets, Int32 iRegTime, taf::JceCurrentPtr current)
{
	ScrtMap vMaps;
    Int32 iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    LbsReq tLbsReq;
    LbsRsp tLbsRsp;

    if (tReq.sCoord.empty())
    {
		iRet = this->getNearSecretsCallback(tReq, tRsp, tFrdsPair, vSecrets, iRegTime, current, tLbsRsp);
		if (iRet == FAILURE)
		{
			LOG->debug() << __FUNCTION__ << "no coords !" << endl;
			break;
		}
		else
		{
			return SUCCESS;
		}
    }

    if (tReq.sCoord.find(",") != string::npos)
    {
        string sLat = tReq.sCoord.substr(0, tReq.sCoord.find(",") + 1);
        string sLnt = tReq.sCoord.substr(tReq.sCoord.find(",") + 1);
        tLbsReq.dLat = TC_Common::strto<Double>(sLat);
        tLbsReq.dLng = TC_Common::strto<Double>(sLnt);
        LOG->debug() << __FUNCTION__ << "coords :" <<tLbsReq.dLat<<" | "<<tLbsReq.dLng<< endl;
    }
    else
    {
        LOG->debug() << __FUNCTION__ << "no coords !" << endl;
		iRet = this->getNearSecretsCallback(tReq, tRsp, tFrdsPair, vSecrets, iRegTime, current, tLbsRsp);
		if (iRet == FAILURE)
		{
			LOG->debug() << __FUNCTION__ << "no coords !" << endl;
			break;
		}
		else
		{
			return SUCCESS;
		}
    }

    tLbsReq.lId = tReq.tUserId.lUId;
    LOG->debug() <<__FUNCTION__<< " Lbs Request :" << tReq.tUserId.lUId << endl;

	GetSecretCallback  tGetSecret(*this);
	TC_AutoPtr< SharedCallback< LbsPrxCallback > > ptr = new SharedCallback< LbsPrxCallback >(current);
	ptr->setBindgetNearbyUser(boost::bind(&GetSecretCallback::getNearSecretsCallback, tGetSecret, tReq, tRsp, tFrdsPair, vSecrets, iRegTime, _1, _2));
	_pLbsProxy->taf_hash(tReq.tUserId.lUId)->async_getNearbyUser(ptr, tLbsReq);
	current->setResponse(false);

    iRet = SUCCESS; 
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " iRet: "<<iRet<<" end "<<endl;

    return iRet;
}

taf::Int32 GetSecretCallback::tranSecretsId(const QUAN::SecretListReq & tReq, QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, ScrtVector& vSecrets, ScrtMap const& vMaps, taf::JceCurrentPtr current)
{
    int iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

        if (vSecrets.empty())
        {
            LOG->debug()<< " tranSecretsId is empty"<<tReq.tUserId.lUId<<endl;
            return SUCCESS;
        }

        Int32       iStop = vSecrets.size();
        Int32       iSize = tReq.iCount * 2;
        ScrtVector  vTmpIds;

        if (iStop >= iSize)
        {
            copy(vSecrets.begin(), vSecrets.begin() + iSize, back_inserter(vTmpIds));
        }
        else
        {
            copy(vSecrets.begin(), vSecrets.end(), back_inserter(vTmpIds));
        }

		GetSecretCallback  tGetSecret(*this);
		TC_AutoPtr< SharedCallback< PostPrxCallback > > ptr = new SharedCallback< PostPrxCallback >(current);
		ptr->setBindgetBatchSecrets(boost::bind(&GetSecretCallback::tranSecretsIdCallback, tGetSecret, tReq, tRsp, tFrdsPair, vSecrets, iStop - vTmpIds.size(), vMaps, _1, _2));
    	_pPostProxy->taf_hash(tReq.tUserId.lUId)->async_getBatchSecrets(ptr, tReq.tUserId.lUId, vTmpIds);
		current->setResponse(false);

    iRet = SUCCESS; 
    __CATCH__
    PROC_END

    LOG->debug() << __FUNCTION__<< " end "<<endl;

    return iRet;

}

taf::Int32 GetSecretCallback::tranSecretsIdCallback(QUAN::SecretListReq tReq, 
								     QUAN::SecretListRsp tRsp, 
									 FrdsVectorPair tFrdsPair, 
									 ScrtVector vSecrets, 
									 Int32 iStop, 
									 ScrtMap vMaps, 
									 taf::JceCurrentPtr current, 
									 vector<SecretInfo> const& vSecretsInfo)
{
    int iRet = FAILURE;
    LOG->debug() << __FUNCTION__<< " begin "<<endl;

    PROC_BEGIN
    __TRY__

    LOG->debug()<< " Batch Secrets From Post :"<<tReq.tUserId.lUId<<endl;

    Int32    iSize = tReq.iCount * 2;

    iStop   -= iSize;

    copy(vSecretsInfo.begin(), vSecretsInfo.end(), back_inserter(tRsp.vSecrets));
    LOG->debug()<< " iStop: "<<iStop<<endl;

	if (tRsp.vSecrets.size() < size_t(tReq.iCount) && iStop > 0)
	{
        ScrtVector  vTmpIds;
		Int32       iStart = vSecrets.size() - iStop;
        if (iStop >= iSize)
        {
            copy(vSecrets.begin() + iStart, vSecrets.begin() + iStart + iSize, back_inserter(vTmpIds));
        }
        else
        {
            copy(vSecrets.begin() + iStart, vSecrets.end(), back_inserter(vTmpIds));
        }

		GetSecretCallback  tGetSecret(*this);
		TC_AutoPtr< SharedCallback< PostPrxCallback > > ptr = new SharedCallback< PostPrxCallback >(current);
		ptr->setBindgetBatchSecrets(boost::bind(&GetSecretCallback::tranSecretsIdCallback, tGetSecret, tReq, tRsp, tFrdsPair, vSecrets, iStop - vTmpIds.size(), vMaps, _1, _2));
    	_pPostProxy->taf_hash(tReq.tUserId.lUId)->async_getBatchSecrets(ptr, tReq.tUserId.lUId, vTmpIds);
		current->setResponse(false);

		return SUCCESS;
    }
	else
	{
        vector<SecretInfo> vTmp;
        FrdsMap mFrds = Vector2Map(tFrdsPair);
        for (vector<SecretInfo>::iterator i = tRsp.vSecrets.begin(); i != tRsp.vSecrets.end(); ++i)
        {
            if (i->lUId == tReq.tUserId.lUId)
            {
                i->iType = ME;
                vTmp.push_back(*i);
            }
            else
            {
                FrdsMapIterator pos = mFrds.find(i->lUId);
                if (pos != mFrds.end())
                {
                    if (tRsp.iLockCount == 0 && pos->second.iRelat == FRIEND)
                    {
                        i->iType = FRIEND;
                        vTmp.push_back(*i);
                    }
                    else if (tRsp.iLockCount == 0 && pos->second.iRelat == FFRIEND)
                    {
                        i->iType = FFRIEND;
                    }
                }
                else
                {
                    ScrtMapCIterator mpos = vMaps.find(i->lSId);
                    if (mpos != vMaps.end())
                    {
                        if (mpos->second < 500)
                        {
							i->sLocal = Near_500;
                        }
                        if (mpos->second < 100)
                        {
							i->sLocal = Near_100;
                        }
                    }
                    i->iType = RECMD;
                }
            }
        }

		// �Ƽ���ɾ���յ���λ��
        tRsp.vSecrets.erase(remove_if(tRsp.vSecrets.begin(),  tRsp.vSecrets.end(),  NullLocation()), tRsp.vSecrets.end()); 

        // ����Ϊ������ֻȡ����
        if (tReq.iType == GET_SECRET_FRIENDS)
        {
            tRsp.vSecrets.swap(vTmp);
			tRsp.lNextBeginId = vTmp.back().lSId;
        }

        // ��ȡϵͳ��������        
        if (tReq.tRange.lBeginId == SECRET_ID_BEGIN && tRsp.iLockCount == 0 && tReq.iType != GET_SECRET_FRIENDS)
        {
			SecretInfo            tSysScrt;
			{
				TC_ThreadLock::Lock  tLock(*LockDbHandle::getInstance());
				iRet = LockDbHandle::getInstance()->getSystemSecret(tSysScrt);
			}
            if (iRet == FAILURE)
            {
                LOG->debug() << __FUNCTION__<< " get SystemSecret failure! "<<endl;
                break;
            }

            tSysScrt.isFavor = false;

            LOG->info() << __FUNCTION__<< " get SystemSecret success! "<<endl;
        
            // ����ϵͳ������
            if (tRsp.vSecrets.size() < _uSysScrtLoc)
            {
                tRsp.vSecrets.push_back(tSysScrt);
            }
            else
            {
                tRsp.vSecrets.insert(tRsp.vSecrets.begin()+ _uSysScrtLoc - 1, tSysScrt);
            }
        }

	    // ���Ƴ���
		if (tRsp.vSecrets.size() >= size_t(tReq.iCount))
    	{
        	tRsp.vSecrets.resize(tReq.iCount);
        	if (tRsp.lNextBeginId < tRsp.vSecrets.back().lSId)
            	tRsp.lNextBeginId = tRsp.vSecrets.back().lSId;
    	}

		LOG->debug() << __FUNCTION__<< " async_response_getSecretList  "<<endl;
		LOG->debug() << __FUNCTION__<< " end "<<JCETOSTR(tRsp)<<endl;
		GetSecret::async_response_getSecretList(current, iRet, tRsp);
	}
    iRet = SUCCESS; 

    __CATCH__
    PROC_END

    return iRet;
}
//////////////////////////////////////////////////////
// ����IDת����������
//////////////////////////////////////////////////////
