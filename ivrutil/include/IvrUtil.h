// @(#) $Id: IvrUtil.h,v 1.1.1.1.2.3 2016/04/12 21:59:16 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// IvrUtil.h
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
// Originated :                                              22-OCT-2010
//----------------------------------------------------------------------
#ifndef INCL_IVR_UTIL_DEF_H
#define INCL_IVR_UTIL_DEF_H

#include "IvrConstDef.h"
#include "IvrLimits.h"
#include "IvrStructs.h"
#include "IvrEvents.h"
#include "IvrEvrApi.h"
#include "ctrace.h"

void FREE(void* memblock);

class IvrUtil
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

      // Repalces string
      static void Replace(TEXT* str, const TEXT* find, const TEXT* replace);

      static int ReplaceAll(char* str, char* pattern, char* subs, char *dest);

      // Print in Hex
      static void PrintHex (const UINT8 * data, int numberOfBytes,
            int arrayLen = 255);

		static EnumCcxmlCommands GetCommandType (TEXT* cmd);

		static EnumDirection GetJoinDirectionValue (TEXT* joindir);

		static TEXT* ConvertVxmlEventToString (const int event);

		static TEXT* ConvertCcxmlEventToString (const int event);

		static BOOLEAN ReadMappedCktNum(BOOLEAN kernelFlg);

		static UINT32 GetMappedCktNum(UINT32 incomingId);

		static EnumFetchMethod GetMethodType (TEXT* method);

      static EnumQueryType GetQueryType(TEXT* lType);

		static EnumCcxmlEvent GetEventNumber(TEXT* lEvent);

		static TEXT* ConvertChannelStatusToString (const EnumIVRChannelStatus  status);

      static TEXT* ConvertEvrEventToStr(const int);

      static TEXT* ConvertEvrCauseToStr(const int);

      static TEXT* ConvertEvrReasonToStr(const int);

      static TEXT* ConvertReqModeToStr (const int);

      static TEXT* ConvertSchTypeToStr (const int);

      static TEXT* ConvertStatusToStr (const int);

      static void DisplayEvr (IvrEvrMsg & rEvrMsg);

      static void DisplayCdr (IvrCdrMsg & rCdrMsg);

   protected:
      static BOOLEAN    msSigCfg;
		static BOOLEAN    msSigTrc;
		static INT16      mMappedCkt[NUM_OF_CKTS];
		static INT16      mNumOfCkts;
};

#endif
