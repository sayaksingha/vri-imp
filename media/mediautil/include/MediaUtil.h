// @(#) $Id: MediaUtil.h,v 1.1.4.3 2016/04/12 21:59:38 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       MediaUtil.h
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
#ifndef INCL_MEDIA_UTIL_DEF_H
#define INCL_MEDIA_UTIL_DEF_H


#include <signal.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/poll.h>
#include <openssl/md5.h>

#include "Types.h"
//nclude "Log.h"
#include "LogApi.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ProcessLock.h"
#include "CfgRead.h"
//#include "OamLogCodes.h"
//#include "DbConstDef.h"
//#include "TssDbMgr.h"
#include "HashTable.h"
#include "StrTokenizer.h"
#include "MediaInterface.h"
#include "IvrConstDef.h"

class MediaUtil
{
   public:
      static BOOLEAN    msKeepRunning;
      static INT32     msSignal;

      // load required signal for the processes
      static void LoadSignals();

      static INT32 GetSignalNo();

      // load required signal for the processes
      static void HandleSignal(int sig);

      // Returns status whether process should continue or not
      static BOOLEAN KeepRunning();

      static void ResetKeepRunning();

      static void ProcessToExit();

      // Returns status whether process should  reload config
      static BOOLEAN ReloadConfig();

      // Reset config reload status
      static void ResetConfigFlag();

      // Returns the config error string
      static TEXT* GetConfigError(const ConfigReturn lCfgRet);

      // Returns status whether process should check trace variable
      static BOOLEAN CheckTrace();

      // Reset config trace variable
      static void ResetTraceFlag();

      // Recreate the Queue on Error
      static BOOLEAN ReCreateMsgQueue(MsgQueue& queue, const key_t key);

      //Handle Queue Create Error
      static BOOLEAN HandleQueueCreateError(msgQueueError err,
            MsgQueue&     queue);

      // Handle Queue Read Error
      static BOOLEAN HandleQueueReadError(msgQueueError err,
            MsgQueue&     queue);

      // Handle Queue Write Error
      static BOOLEAN HandleQueueWriteError(msgQueueError err,
            MsgQueue&     queue,
            void*         ptr,
            INT16&        size,
            int           mtype,
            modeOption    mode = BLOCKING);

      static TEXT * ConvertQueErrToStr (const msgQueueError queErr);

      static TEXT* ConvertCmdToStr(EnumMediaCmdTypes cmdType, unsigned int &lseq, 
            UINT16 &lCkt, MpData &lMpData);

      static TEXT * ConvertStatusToStr(EnumChannelStatus chanStatus);

   protected:
      static BOOLEAN    msSigCfg;
      static BOOLEAN    msSigTrc;
};
#endif
