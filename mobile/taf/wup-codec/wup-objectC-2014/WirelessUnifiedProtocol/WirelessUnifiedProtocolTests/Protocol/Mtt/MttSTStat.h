// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.2 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/renjunyi/MTT.jce'
// **********************************************************************

#import "JceObjectV2.h"
#import "MttETPV.h"
#import "MttFKINFO.h"
#import "MttPerformanceInfo.h"
#import "MttSTPV.h"
#import "MttSTTime.h"
#import "MttSTTotal.h"
#import "MttThirdUse.h"
#import "MttUserBase.h"

@interface MttSTStat : JceObjectV2

@property (nonatomic, retain, JV2_PROP_GS_V2(stUB,setStUB:)) MttUserBase* JV2_PROP_NM(r,0,stUB);
@property (nonatomic, retain, JV2_PROP_GS_V2(stTime,setStTime:)) MttSTTime* JV2_PROP_NM(r,1,stTime);
@property (nonatomic, retain, JV2_PROP_GS_V2(stTotal,setStTotal:)) NSArray* JV2_PROP_EX(r,2,stTotal,VOMTTSTTotal);
@property (nonatomic, retain, JV2_PROP_GS_V2(stOuterPv,setStOuterPv:)) NSArray* JV2_PROP_EX(r,3,stOuterPv,VOMTTSTPV);
@property (nonatomic, retain, JV2_PROP_GS_V2(stInnerPv,setStInnerPv:)) NSArray* JV2_PROP_EX(r,4,stInnerPv,VOMTTSTPV);
@property (nonatomic, assign, JV2_PROP_GS_V2(iUseTime,setIUseTime:)) JceInt32 JV2_PROP_NM(r,5,iUseTime);
@property (nonatomic, retain, JV2_PROP_GS_V2(vEntryPv,setVEntryPv:)) NSArray* JV2_PROP_EX(o,6,vEntryPv,VOMTTETPV);
@property (nonatomic, retain, JV2_PROP_GS_V2(vFastKey,setVFastKey:)) NSArray* JV2_PROP_EX(o,7,vFastKey,VOMTTFKINFO);
@property (nonatomic, retain, JV2_PROP_GS_V2(sProtocol,setSProtocol:)) NSString* JV2_PROP_NM(o,8,sProtocol);
@property (nonatomic, retain, JV2_PROP_GS_V2(vThirdUse,setVThirdUse:)) NSArray* JV2_PROP_EX(o,9,vThirdUse,VOMTTThirdUse);
@property (nonatomic, retain, JV2_PROP_GS_V2(vPerformanceInfo,setVPerformanceInfo:)) NSArray* JV2_PROP_EX(o,10,vPerformanceInfo,VOMTTPerformanceInfo);
@property (nonatomic, retain, JV2_PROP_GS_V2(vUseTime,setVUseTime:)) NSArray* JV2_PROP_EX(o,11,vUseTime,VONSNumber);

@end