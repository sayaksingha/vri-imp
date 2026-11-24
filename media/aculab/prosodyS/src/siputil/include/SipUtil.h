// @(#) $Id: SipUtil.h,v 1.1.4.1 2016/04/12 21:59:25 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       SipUtil.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_SIP_UTIL_DEF_H
#define INCL_SIP_UTIL_DEF_H

#include "SipConstDef.h"
#include "SipLimits.h"
#include "SipStructs.h"
#include "SipEvrApi.h"
#include "MediaUtil.h"
#include "ctrace.h"

class SipUtil : public MediaUtil
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

      static TEXT* ConvertQueErrToStr(const msgQueueError queErr);

      static TEXT* ConvertEvrEventToStr(const int);

      static TEXT* ConvertEvrCauseToStr(const int);

      static TEXT * ConvertEvrTypeToStr (const int evrType);

      static void DisplayEvr (SipEvrMsg & rEvrMsg);
   protected:
      static BOOLEAN    msSigCfg;
      static BOOLEAN    msSigTrc;
};
#endif
