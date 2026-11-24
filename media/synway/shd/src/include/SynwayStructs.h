// @(#) $Id: SynwayStructs.h,v 1.1.4.2 2016/04/12 21:59:38 shijuc Exp $
//----------------------------------------------------------------------
// NAME 
// SynwayStructs.h
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated : Chethan V M                                  22-Nov-2013
//----------------------------------------------------------------------
#ifndef INCL_SYNWAY_STRUCTS_DEF_H
#define INCL_SYNWAY_STRUCTS_DEF_H

#include "MediaInterface.h"
#include "Ss7ConstDef.h"
#include "IsupStructs.h"
#include "SynwayLogCodes.h"
#include "SynwayConsDef.h"
#include "SynwayLimits.h"

//Standard Synway headers
#include "shpa3api.h"

#define SNW_RET_ERR_FRW_APP -1
#define SNW_RET_ERR          0
#define SNW_RET_SUCCESS      1


//Structure which store all information for each channel
enum EnumPlaystatus{
   PLAY_NEXT_NOT_REQD      = 0, //Request came when channel status is idle
   PLAY_NEXT_REQD          = 1, //Request came when channel status is stopping/getdtmf
   PLAY_NEXT_IMMEDIATE     = 2  //Request came when channel status is playing
};

enum EnumSigChannelStatus{
     SNW_CHNL_NOT_AVAILABLE   = 0,
     SNW_CHNL_IDLE            = 1,
     SNW_IAM_SENT             = 2,
     SNW_WAIT_DAIL_ANS        = 3, //IAM Send Success
     SNW_DIALING              = 4,
     SNW_WAIT_REMOTE_PICKUP   = 5, //ACM RCVD
     SNW_ANM_RCVD             = 6,
     SNW_TALKING              = 7, //Call Connected
     SNW_IAM_RCVD             = 8,
     SNW_WAIT_ACM_SND         = 9, //Wait For ACM from App to Send
     SNW_RINGING              = 10, //ACM SENT
     SNW_WAIT_RLC             = 11,
     SNW_REMOTE_HANGUP        = 12, //REL RCVD
     SNW_WAIT_REL_SND         = 13, //Wait For REL from App to send
};

enum EnumSnwCallDirection
{
   SNW_NO_CALL                = 0,
   SNW_IN_BOUND_CALL,
   SNW_OUT_BOUND_CALL
};

struct synwayChannel
{
   INT16                     cktId;
   EnumChannelStatus         status;
   EnumPlaystatus            playStatus;
   MpPlay                    currentPlay;
   MpRecord                  currentRecord;
   MpGetDigit                currentGetdigit;
   MpPlay                    nextPlay;
   time_t                    mediaCompTime;
};

struct synwaySigChannel
{
   INT16                     cic;
   EnumSnwCallDirection      callDirection;
   EnumSigChannelStatus      status;
   time_t                    statusUpdateTime;
};

struct synwaySystem
{
   INT16                     minChannel;
   INT16                     maxChannel;
   INT16                     boardID;
   INT16                     boardType;
   INT16                     mMaxChannel;
   synwayChannel             channel[MAX_SYNWAY_CHANNEL];
   synwaySigChannel          sigChannel[MAX_SYNWAY_CHANNEL];
};

struct ScheduledAnm
{
   BOOLEAN    isScheduled;
   GenIsupAnm schAnm;
};

struct ScheduledAcm
{
   BOOLEAN    isScheduled;
   GenIsupAnm schAcm;
};

struct ScheduledIam
{
   BOOLEAN    isScheduled;
   GenIsupIam schIam;
};

#endif
