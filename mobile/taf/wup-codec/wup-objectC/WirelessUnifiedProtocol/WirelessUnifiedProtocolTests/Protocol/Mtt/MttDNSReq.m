// **********************************************************************
// This file was generated by a TAF parser!
// TAF version 2.1.4.2 by WSRD Tencent.
// Generated from `/usr/local/resin_system.mqq.com/webapps/communication/taf/upload/renjunyi/MTT.jce'
// **********************************************************************

#import "MttDNSReq.h"

@implementation MttDNSReq

@synthesize JV2_PROP_NM(r,0,sDomain);
@synthesize JV2_PROP_NM(r,1,iPort);

+ (void)initialize
{
    if (self == [MttDNSReq class]) {
        [super initialize];
    }
}

- (id)init
{
    if (self = [super init]) {
        JV2_PROP(sDomain) = DefaultJceString;
    }
    return self;
}

- (void)dealloc
{
    JV2_PROP(sDomain) = nil;
    [super dealloc];
}

+ (NSString*)jceType
{
    return @"MTT.DNSReq";
}

@end