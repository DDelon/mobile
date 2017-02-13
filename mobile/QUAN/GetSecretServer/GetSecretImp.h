#ifndef _GetSecretImp_H_
#define _GetSecretImp_H_

#include "servant/Application.h"
#include "Base.h"
#include "SecretBase.h"
#include "GetSecret.h"
#include "UserInfo.h"
#include "Post.h"
#include "Lbs.h"
#include "Circle.h"
#include "DbHandle.h"

using namespace QUAN;

/**
 *
 *
 */
class GetSecretImp : public QUAN::GetSecret
{
public:
	friend class GetSecretCallback;
	/**
	 *
	 */
	virtual ~GetSecretImp() {}

	/**
	 *
	 */
	virtual void initialize();

	/**
	 *
	 */
        virtual void destroy();

	/**
	 *
	 */
        virtual taf::Int32 test(taf::Int64 lId,taf::JceCurrentPtr current);
        // ��ȡ�����б�
        virtual taf::Int32 getSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current);
        // ��������
        virtual taf::Int32 setUserSecretToCache(const QUAN::UserSecretCacheReq & tReq,taf::JceCurrentPtr current);
        // ������ȡ��������
        virtual taf::Int32 getUserSingleSecret(const QUAN::SglSecretReq & tReq,QUAN::SglSecretRsp &tRsp,taf::JceCurrentPtr current);
        // ���÷���״̬
        virtual taf::Int32 setUserSecretShare(const QUAN::ShareSecretReq & tReq,QUAN::ShareSecretRsp &tRsp,taf::JceCurrentPtr current);
        // ����
        virtual taf::Int32 feedBack(const QUAN::FeedBackReq & tReq,QUAN::FeedBackRsp &tRsp,taf::JceCurrentPtr current);
        // ��ȡϵͳ������
        virtual taf::Int32 getSystemList(const QUAN::SystemListReq & tReq,QUAN::SystemListRsp &tRsp,taf::JceCurrentPtr current);

		taf::Int32 getFriendsList(SecretListReq const& tReq, UserId const& tUserId, taf::JceCurrentPtr current);
   
        // ��ȡ���������б�
        virtual taf::Int32 getCircleList(const QUAN::GetCircleListReq & tReq,QUAN::GetCircleListRsp &tRsp,taf::JceCurrentPtr current);
		virtual taf::Int32 getAllSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp,taf::JceCurrentPtr current);

		virtual taf::Int32 cleanCircleIdCache(taf::Int64 lCircleId, taf::JceCurrentPtr current);
private:

        // begin  ��ȡ�����б���ú���
		taf::Int32 getRecommendSecrets(SecretListReq const& tReq, Int32 iFrdsNum, FrdsVector const& vFrdsFavor, ScrtVector& vSecrets);
        // ��ȡ���������б� 
        taf::Int32 getFriendsSecrets(SecretListReq const& tReq, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, Int64& lNextBeginId, Int32& iLockCount, ScrtVector& vSecrets, Int32& iTimeSpan);
        // ��ȡ�����б�
        taf::Int32 getFriendsList(UserId const& tUserId, FrdsVector& vFrdsFavor, FrdsVectorPair& tFrdsPair, FrdsVector& vFrdsAll, Int32& iRegTime);
        Int32 getBatchUserFavorSecrets(Int64 lUId, Int32 iSum, Int32 iFrdsFvrNum, FrdsVector const& vFrds, ScrtVector& vSecrets);
        // ��cache�������ݿ�������
        taf::Int32 getPullSecretList(const QUAN::SecretListReq & tReq,
                                     QUAN::SecretListRsp &tRsp, 
                                     FrdsVectorPair const& tFrdsPair, 
                                     FrdsVector const& vFrdsAll, 
                                     FrdsVector const& vFrdsFavor, 
                                     ScrtVector& vSecrets,
									 FrdsVector const& vFrdsCircle,
									 Int32& iEraseTime);
        // ��������
        taf::Int32 uniqueSecretList(const QUAN::SecretListReq & tReq,QUAN::SecretListRsp &tRsp, FrdsVectorPair const& tFrdsPair, FrdsVector const& vFrdsAll, map<Int64, CircleInfo> const& mCircles, ScrtVector& vSecrets, Int32 iRegTime);
		// ��ȡ�����б�
		taf::Int32 getNearSecrets(SecretListReq const& tReq, ScrtMap& vMaps, ScrtVector& vSecrets);

		// ����IDת������
		taf::Int32 tranSecretsId(SecretListReq const& tReq, SecretListRsp& tRsp, ScrtVector& vSecrets, FrdsMap const& mFrds);

		// ��ȡ�����б�
		taf::Int32 getCirclesList(SecretListReq const& tReq, FrdsVector& vFrdsFavor, map<Int64, CircleInfo>& mCircles);

		taf::Int32 getCitySecrets(SecretListReq const& tReq, ScrtVector& vSecrets);

private:
        size_t              _uMaxSecretsNum;
        size_t              _u24SecretsNum;
        size_t              _u72SecretsNum;
        size_t              _uLockFriendsNum;
        size_t              _uMaxFrdsFvrNum;
        size_t              _uMinFrdsFvrNum;
        size_t              _uSysScrtLoc;
        size_t              _uActiveFriends;
        size_t              _uTopN;
        UserInfoPrx         _pUserProxy;
        PostPrx             _pPostProxy;
        LbsPrx              _pLbsProxy;
        CirclePrx           _pCircleProxy;
        DbHandle            _tDbHandle;
};
/////////////////////////////////////////////////////
#endif
