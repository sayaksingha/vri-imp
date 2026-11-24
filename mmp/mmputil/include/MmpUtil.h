// @(#) $Id: MmpUtil.h,v 1.1.4.3 2016/04/12 21:59:45 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// MmpUtil.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2006
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Divya K                                   24-JAN-2012
//----------------------------------------------------------------------
#ifndef INCL_MMP_UTIL_DEF_H
#define INCL_MMP_UTIL_DEF_H

#include "MmpConstDef.h"
#include "MmpLimits.h"
#include "MmpStructs.h"
#include "IvrStructs.h"
#include "BillingStructs.h"
#include "BillingApi.h"
#include "ctrace.h"

class MmpUtil
{
   public:
      static BOOLEAN    msKeepRunning;
      static INT32      msSignal;

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

      static void PrintHex (const UINT8 * data, int numberOfBytes,
            int arrayLen = 255);

      static void PrintUnicode (const UINT8 * data,
            int numberOfBytes, int arrayLen);

      static void ConvertToUnicode (UINT8 * data, UINT8 & length);

      static void ConvertToUnicode ( BINARY & binary );

      static void ConvertToUnicode (GgiReqMsg & reqMsg);

      static TEXT* ConvertProvKeyTypeToStr (const int);

      static TEXT * ConvertKeywordToStr (const int);

      static TEXT * ConvertInterfaceIdToStr (const int);
      
      static TEXT * ConvertSubsTypeToStr (const int);

      static TEXT * ConvertSubsStatusToStr (const int);

      static TEXT * ConvertPkgTypeToStr (const int);

      static void ConvertUniodeToAscii (UINT8 * pData, UINT16 dataLen, TEXT * pAscii);

   protected:
      static BOOLEAN    msSigCfg;
      static BOOLEAN    msSigTrc;
      static INT16      mMappedCkt[NUM_OF_CKTS] ;
      static INT16      mNumOfCkts;
};

#endif
