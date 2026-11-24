//@(#) $Id: MediaHandler.h,v 1.1.4.3 2016/04/12 21:59:29 shijuc Exp $
//---------------------------------------------------------------
// NAME
// MediaHandler.h
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
// Originated :                                   DATE:01-SEP-2010
//----------------------------------------------------------------
#ifndef INCL_MEDIA_HDLR_H
#define INCL_MEDIA_HDLR_H

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

#define VALID_DIGIT               true
#define INVALID_DIGIT             false
#define NO_DIGIT                  -1

#define COMPANDING                COMPANDING_ALAW

class MediaHandler
{
   public:

      MediaHandler();
      ~MediaHandler();

      BOOLEAN     Init();
      void        DeInit();

      BOOLEAN                       ConnectToProsody(SM_SWITCH_CHANNEL_PARMS *);
      BOOLEAN                       DisAssociateEvent(int);
      BOOLEAN                       DisAssociateEvent(tSMEventId &);
      BOOLEAN                       DisConnectFromProsody(void *);
      BOOLEAN                       ReleaseChannel();

      BOOLEAN                       DetectLoop(tSMChannelId &, tSM_INT &, tSM_INT &, tSMEventId &);
      BOOLEAN                       HandleRecog(int &, int &, tSMChannelId &);

      SM_SWITCH_CHANNEL_PARMS       mSwitchChannelParms;
      SM_CHANNEL_SET_EVENT_PARMS    mChannelSetEvtParms;
      SM_CHANNEL_ALLOC_PLACED_PARMS mChannelAllocParms;
      SM_FILE_REPLAY_PARMS          mFileReplayParms;   
      SM_FILE_RECORD_PARMS          mFileRecordParms;
      SM_CHANNEL_INFO_PARMS         mChannelInfoParms;

      SM_RECOGNISED_PARMS           mRecogParms;
      SM_LISTEN_FOR_PARMS           mListenParms;
      SM_CATSIG_LISTEN_FOR_PARMS    mCatsigListenParms;

      OPEN_PORT_PARMS               mOpenPort;
      PORT_INFO_PARMS               mPortInfo;

      INT16                         mCmdStream;
      INT16                         mCmdTimeSlot;
      TEXT                          mFileName[20];
      INT16                         mNumOfChann;

   protected:

      BOOLEAN InitializeCard();

      UINT32                        mCardId[20];

      ACU_SNAPSHOT_PARMS            mCardDetails;
      ACU_OPEN_CARD_PARMS           mOpenCardDet;
      ACU_OPEN_PROSODY_PARMS        mOpenProsody;
      ACU_CARD_INFO_PARMS           mCardInfo;

      ACU_OPEN_SWITCH_PARMS         mOpenSwitch;
      ACU_OPEN_CALL_PARMS           mOpenCall;

      SM_OPEN_MODULE_PARMS          mOpenModule;
      SM_MODULE_INFO_PARMS          mModuleInfo;
      SM_CARD_INFO_PARMS            mSmCardInfo;

      SM_REPLAY_STATUS_PARMS        mReplayStatusParms;

      OUTPUT_PARMS                  mConnectParms;
      OUTPUT_PARMS                  mDisConnectParms;

      INT16                         mStream;
      INT16                         mTimeSlot;
      INT16                         mChannels;
      INT16                         mMinStream;
      INT16                         mModules;
};
#endif

