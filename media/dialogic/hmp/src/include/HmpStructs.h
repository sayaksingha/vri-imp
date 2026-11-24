// @(#) $Id: HmpStructs.h,v 1.1.2.1 2012/02/21 08:31:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpStructs.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Hmp                                  23-Nov-2011
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_HMP_STRUCTS_DEF_H
#define INCL_HMP_STRUCTS_DEF_H

#include "HmpConstDef.h"
#include "MediaInterface.h"

enum EnumHmpModuleId
{
   HMP_NULL = 0,
   HMP_EVR_WRITER = 1,
   HMP_1xProc = 2
};

enum EnumHmpSubscriptionType
{
   HMP_SUBSCRIPTION_UNKNOWN = -1,
   HMP_SUBSCRIPTION_POSTPAID = 0,
   HMP_SUBSCRIPTION_PREPAID = 1
};

enum EnumHMPDeviceType
{
   HMP_DX_DEVICE     = 1,
   HMP_IPM_DEVICE    = 2,
   HMP_DTI_DEVICE
};

enum EnumHmpDeviceState
{
   DEVICE_IDLE        = 0,
   DEVICE_BUSY
};

enum EnumVoiceChannelState
{
   VOICE_CHANNEL_IDLE         = 0,
   VOICE_CHANNEL_PLAYING,
   VOICE_CHANNEL_PLAY_STOPING,
   VOICE_CHANNEL_RECORDING,
   VOICE_CHANNEL_RECORD_STOPING,
   VOICE_CHANNEL_GET_DTMF,
   VOICE_CHANNEL_GET_DTMF_STOPING
};

enum EnumIpmChannelState
{
   IPM_CHANNEL_IDLE         = 0,
   IPM_CHANNEL_STREAM
};

enum EnumStreamType
{
   IPM_NULL         = 0,
   IPM_LIVE_STREAM,
   IPM_ON_DEMAND_STREAM
};

enum EnumChannelReqStatus
{
   NEXT_IDLE               = 0,
   NEXT_PLAY_REQUIRED,
   NEXT_RECORD_REQUIRED,
   NEXT_GET_DTMF_REQUIRED
};

struct HmpDxStruct 
{
   INT32                index;
   TEXT                 dxDeviceName[DX_DEVICE_NAME_LENGTH];
   INT32                dxHandle;
   long                 scts;
   DX_IOTT              iott[DX_MAXIMUM_FILES];
   DV_DIGIT             digits;
   EnumHmpDeviceState   deviceState;
};

struct HmpIpmStruct 
{
   INT32                index;
   TEXT                 ipmDeviceName[IPM_DEVICE_NAME_LENGTH];
   INT32                ipmHandle;
   long                 scts;
   IPM_MEDIA_INFO       mediaInfo;
   unsigned int         audioPort;
   RtspClient           *rtspClient;
   TEXT                 rtspUrl[240 + 1];
   EnumStreamType       streamType;
   EnumHmpDeviceState   deviceState;
   BOOLEAN              rtspObjCreated;
   BOOLEAN              rtspSesCreated;
};

struct HmpDtiStruct
{
   INT32             index;
   TEXT              dtiDeviceName[DTI_DEVICE_NAME_LENGTH];
   INT32             dtiHandle;
   long              scts;
};

struct HmpReqMap
{
   EnumHMPDeviceType    deviceType;
   INT32                dxHandleIdx;
   INT32                ipmHandleIdx;
   UINT16               totalDtiDevice;
   INT32                dtiHandleIdx[MAX_DTI_LSTN_IPM];
   EnumChannelReqStatus lChnlReqStatus;
   MpRequestDetails     requestDetails;
   MpRequestDetails     nextReqDetails;
   TEXT                 key[20 +1];
};

struct  HmpConfig
{
   TEXT              configKey[HMP_MAX_CONFIG_PARAM_LEN + 1];
   TEXT              configValue[HMP_MAX_CONFIG_VALUE_LEN +1];
};

struct   VoiceChannelStatus
{
   EnumVoiceChannelState   status;
   INT32                   dxHandleIdx;
};

struct   IpmChannelStatus
{
   EnumIpmChannelState   status;
   INT32                 ipmHandleIdx;
};

struct ChannelStatus
{
   VoiceChannelStatus   voiceChannel;
   IpmChannelStatus     ipmChannel;
};
#endif
