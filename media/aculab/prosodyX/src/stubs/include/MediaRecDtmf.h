//@(#) $Id: MediaRecDtmf.h,v 1.1.4.3 2016/04/12 21:59:29 shijuc Exp $
//---------------------------------------------------------------
// NAME
// MediaAsyncRecordHdlr.h
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
#ifndef INCL_MEDIA_ASYNC_RECORD_HDLR_H
#define INCL_MEDIA_ASYNC_RECORD_HDLR_H

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

struct Channel
{
   int st;
   int ts;
   int type;
};

struct runningChannel
{
   Channel      outPort;
   tSMChannelId channel;
   SM_FILE_RECORD_PARMS rp;
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

      INT16                         mCmdStream;
      INT16                         mNumOfChann;
      INT16                         mCmdTimeSlot;
      TEXT                          mFileName[20];

   protected:

      BOOLEAN RecordDtmfChan (Channel &);

      BOOLEAN InitializeCard();

      UINT32                        mCardId[20];

      ACU_SNAPSHOT_PARMS            mCardDetails;
      ACU_OPEN_CARD_PARMS           mOpenCardDet;
      SM_OPEN_MODULE_PARMS          mOpenModule;
};
#endif

