// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.3 by WSRD Tencent.
// Generated from `SecretBase.jce'
// **********************************************************************

#import "JceObjectV2.h"
#import "QUANContentInfo.h"

@interface QUANContentListRsp : JceObjectV2

@property (nonatomic, assign, JV2_PROP_GS_V2(iFavorNum,setIFavorNum:)) JceInt32 JV2_PROP_NM(o,0,iFavorNum);
@property (nonatomic, assign, JV2_PROP_GS_V2(iCmtNum,setICmtNum:)) JceInt32 JV2_PROP_NM(o,1,iCmtNum);
@property (nonatomic, retain, JV2_PROP_GS_V2(vContents,setVContents:)) NSArray* JV2_PROP_EX(o,2,vContents,VOQUANContentInfo);

@end
