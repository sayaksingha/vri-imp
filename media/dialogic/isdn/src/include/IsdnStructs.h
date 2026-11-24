// @(#) $Id: IsdnStructs.h,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       IsdnStructs.h
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
// AUTHOR         : Chethan V M                         25/07/2013
//----------------------------------------------------------------------
#ifndef INCL_ISDN_STRUCTS_DEF_H
#define INCL_ISDN_STRUCTS_DEF_H

#include "IsdnConstDef.h"
#include "MediaInterface.h"


enum EnumIsdnDeviceType
{
   ISDN_DX_DEVICE     = 1,
   ISDN_DTI_DEVICE
};

enum EnumIsdnVoiceChannelState
{
   ISDN_VOICE_CHANNEL_IDLE         = 0,
   ISDN_VOICE_CHANNEL_PLAYING,
   ISDN_VOICE_CHANNEL_PLAY_STOPING,
   ISDN_VOICE_CHANNEL_RECORDING,
   ISDN_VOICE_CHANNEL_RECORD_STOPING,
   ISDN_VOICE_CHANNEL_GET_DTMF,
   ISDN_VOICE_CHANNEL_GET_DTMF_STOPING
};

enum EnumIsdnVoiceChannelReqStatus
{
   ISDN_NEXT_IDLE               = 0,
   ISDN_NEXT_PLAY_REQUIRED,
   ISDN_NEXT_RECORD_REQUIRED,
   ISDN_NEXT_GET_DTMF_REQUIRED
};

enum EnumIsdnCallDirection
{
   ISDN_NO_CALL                = 0,
   ISDN_IN_BOUND_CALL,
   ISDN_OUT_BOUND_CALL
};

enum EnumIsdnDChannelState
{
   ISDN_D_CHANNEL_INACTIVE     = 0,
   ISDN_D_CHANNEL_ACTIVE
};

enum EnumDxDeviceState
{
   ISDN_DX_DEVICE_IDLE        = 0,
   ISDN_DX_DEVICE_BUSY
};

enum EnumIsdnRecvCallState
{
   ISDN_RECVCALL_UNKNOWN        = 0x00,
   ISDN_RECV_ACCEPTED           = 0x01,
   ISDN_RECV_CONNECTED          = 0x04,
   ISDN_RECV_IDLE               = 0x20,
   ISDN_RECV_DISCONNECTED       = 0x40,
   ISDN_RECV_OFFERED            = 0x08
};

enum EnumIsdnMakeCallState
{
   ISDN_MAKECALL_UNKNOWN        = 0x00,
   ISDN_MAKE_ALERTING           = 0x02,
   ISDN_MAKE_DIALING            = 0x10,
   ISDN_MAKE_CONNECTED          = 0x04,
   ISDN_MAKE_IDLE               = 0x20,
   ISDN_MAKE_DISCONNECTED       = 0x40
};

enum EnumIsdnProcRetType
{
   ISDN_PROC_RETURN_FALSE          = -1,
   ISDN_PROC_RETURN_TRUE           = 0,
   ISDN_PROC_RETURN_TRUE_WITH_RSP  = 1
};

typedef struct
{
   INT32 numOfPhysicalBoards;
   struct
   {
      INT32      physicalBoradIndex;
      TEXT       nameofPhysicalBoards[ISDN_DEVICE_NAME_LENGTH];
      INT32      numberOfVirtualBoard;
      struct
      {
         INT32       virtualBoardIndex;
         TEXT        virtualBoardName[ISDN_DEVICE_NAME_LENGTH];
         INT32       virtualBoardType;
         INT32       numberOfSubDevices;
         struct
         {
            INT32         subDeviceIndex;
            TEXT          subDeviceName[ISDN_DEVICE_NAME_LENGTH];
            INT32         subDeviceType;
         }subDevice[ISDN_MAX_SUB_DEVICES];
      }virtualBoard[ISDN_MAX_VIRTUAL_BOARD];
   }physicalBoard[ISDN_MAX_PHY_BOARD];
}DeviceInfo;

struct IsdnDx
{
   INT32                index;
   INT32                gcIndex;
   TEXT                 dxDeviceName[ISDN_DX_DEVICE_NAME_LENGTH];
   INT32                dxHandle;
   long                 scts;
   DX_IOTT              iott[ISDN_DX_MAXIMUM_FILES];
   DV_DIGIT             digits;
   EnumDxDeviceState   deviceState;
};

struct IsdnDti
{
   INT32             index;
   TEXT              dtiDeviceName[ISDN_DTI_DEVICE_NAME_LENGTH];
   INT32             dtiHandle;
   long              scts;
};

struct IsdnCall
{
   int               call_state;
   TEXT              destNum[MAX_ADDRESS_LEN];
   TEXT              origNum[MAX_ADDRESS_LEN];
};

struct   VoiceChannelStatus
{
   EnumIsdnVoiceChannelState   status;
   INT32                   dxHandleIdx;
};

struct ChannelStatus
{
   VoiceChannelStatus   voiceChannel;
   int                  call_state;           
};

struct IsdnVoiceDetails
{
   EnumIsdnVoiceChannelReqStatus  lChnlReqStatus;
   MpRequestDetails           requestDetails;
   MpRequestDetails           nextReqDetails;
};

struct IsdnCallDetails
{
   IsdnCall                         isdnCall;
   EnumIsdnCallDirection            callDirection;
   EnumIsdnDChannelState            dChnlState;
   EnumIsdnRecvCallState            recvCallState;
   EnumIsdnMakeCallState            makeCallState;
   INT16                            resetLineDevActive;
   INT32                            makeCallTimeOut;
};

struct IsdnChannel
{
   INT32                            cic;
   long                             crn;
   INT32                            chnlHandle;
   TEXT                             chnlDeviceName[ISDN_GC_DEVICE_NAME_LENGTH];
   INT32                            indxIsdnDti;
   INT32                            indxIsdnDx;
   BOOLEAN                          taskFail;
   IsdnCallDetails                  isdnCallDetails;
   IsdnVoiceDetails                 isdnVoiceDetails;
};

#endif
