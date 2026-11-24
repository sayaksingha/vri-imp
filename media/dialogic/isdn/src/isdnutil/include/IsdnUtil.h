// @(#) $Id: IsdnUtil.h,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME  IsdnUtil.h
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
// AUTHOR         : Chethan V M                     25/07/2013
//----------------------------------------------------------------------
#ifndef INCL_ISDN_UTIL_DEF_H
#define INCL_ISDN_UTIL_DEF_H

#include "IsdnConstDef.h"
#include "IsdnLimits.h"
#include "IsdnStructs.h"

class IsdnUtil
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

      static TEXT * ConvertQueErrToStr(const msgQueueError queErr);

      static TEXT * ConvertIsdnVoiceEventToStr(const int);

      static TEXT * ConvertIsdnGcEventToStr(const int);

      static TEXT * VoiceTermReason(const long);

      static TEXT * ConvertVoiceChnlStatToStr (EnumIsdnVoiceChannelState);

      static TEXT * ConvertNextReqStatToStr (EnumIsdnVoiceChannelReqStatus);

      static TEXT * ConvertCmdToStr(EnumMediaCmdTypes);

      static TEXT * ConvertIsupMsgToStr(const GenIsupMsgType);

      static TEXT * ConvertDlgCalStToStr(const int);

   protected:
      static BOOLEAN    msSigCfg;
      static BOOLEAN    msSigTrc;
};
#endif
