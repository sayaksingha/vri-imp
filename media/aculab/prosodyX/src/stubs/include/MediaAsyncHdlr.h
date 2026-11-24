//@(#) $Id: MediaAsyncHdlr.h,v 1.1.4.3 2016/04/12 21:59:28 shijuc Exp $
//---------------------------------------------------------------
// NAME
// MediaAsyncHandler.h
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// CLASS TYPE
// abstract or concrete
//
// DESCRIPTION
//
// Originated :                                   DATE:20-JUL-2010
//----------------------------------------------------------------
#ifndef INCL_MEDIA_ASYNC_HDLR_H
#define INCL_MEDIA_ASYNC_HDLR_H

#include <time.h>
#include <sys/time.h>
#include <sys/poll.h>

#include "acu_type.h"
#include "res_lib.h"
#include "cl_lib.h"
#include "sw_lib.h"
#include "iptel_lib.h"
#include "smbesp.h"        
#include "smdrvr.h"
#include "smrtp.h"
#include "smbesp.h"        // Basic Speech Processing functionallity
#include "smhlib.h"        // High-level Speech functionallity
#include "smwavlib.h"      // Wav file replay/record funtions
#include "MediaInterface.h"
#include "AcuMediaConstDef.h"
#include "AcuMediaLimits.h"
#include "AcuMediaUtil.h"

#define COMPANDING_ALAW           1
#define COMPANDING_MULAW          2
#define TIMESLOTS_PER_STREAM      32
#define CHANNELS_PER_MODULE       30

#define COMPANDING                COMPANDING_ALAW

enum EnumToneType
{
   TONE_TYPE_START, 
   TONE_TYPE_END, 
   TONE_TYPE_LEN
};

enum EnumChannStatus
{
   CHANN_STAT_CLOSED,
   CHANN_STAT_DATA,
   CHANN_STAT_UNDERRUN,
   CHANN_STAT_IDLE
};

enum EnumNumPlay
{
   PLAY_ONCE,
   PLAY_REPEAT
};

struct Channel
{
   int st;
   int ts;
   int type;
   int moduleIndx;
};

struct runningChannel
{
   EnumNumPlay play;
   Channel  outPort;
   tSMChannelId channel;
   SM_FILE_REPLAY_PARMS fp;
   long playCtrlPos;
};

struct dtmfChan 
{
   tSMChannelId chan;
   Channel  port;
   SM_LISTEN_FOR_PARMS lp;
};

class MediaHandler
{
   public:

      MediaHandler();
      ~MediaHandler();

      BOOLEAN     Init();
      void        DeInit();

      BOOLEAN                       DisAssociateEvent(int);
      BOOLEAN                       ReleaseChannel(tSMChannelId);

      BOOLEAN                       HandlePause(int lIndex);
      BOOLEAN                       HandleResume(int lIndex);
      BOOLEAN                       HandleForward(int lIndex);
      BOOLEAN                       HandleRewind(int lIndex);

      INT16                         mCmdStream;
      INT16                         mNumOfChann;
      INT16                         mCmdTimeSlot;
      TEXT                          mFileName[20];

   protected:

      BOOLEAN CreateCChan(Channel &);

      BOOLEAN InitializeCard();

      UINT32                        mCardId[20];

      ACU_SNAPSHOT_PARMS            mCardDetails;
      ACU_OPEN_CARD_PARMS           mOpenCardDet;
      SM_OPEN_MODULE_PARMS          mOpenModule[4];
};
#endif

