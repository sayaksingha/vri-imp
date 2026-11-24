static const char *id="@(#) $Id: MediaPlayDtmfMain.cc,v 1.1.4.3 2016/04/12 21:59:31 shijuc Exp $";
//-----------------------------------------------------------------------------
// NAME
//   MediaPlayDtmfMain.cc -- MediaHandler Main process
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// CLASS TYPE
//
// DESCRIPTION
//
// Originated : 
//------------------------------------------------------------------------------
#include "MediaPlayDtmf.h"

Log                             gLog;
CTrace                          gTrace(TRACE_MEDIA_HANDLER_ENV);
ConfigInfo                      gConfig;
TEXT                            gProcessName[ACULAB_MAX_PROCESS_NAME];
MediaHandler                    gMediaHandlerObj;
ACU_EVENT_QUEUE                 gEvtQueId;

//------------------------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name)
// RETURN      : 0
// THROWS      : none
//------------------------------------------------------------------------------------
int main(int argc,char *argv[])
{

   ProcessLock   lProcessLock;
   TiNGtrace = 3;

   strcpy(gProcessName, basename(argv[0]));

   if(argc != 4 && argc != 5)
   {
      printf("Play Usage:%s <Stream> <TimeSlot> <NumChannels> <FileName>\n", gProcessName);
      return 1;
   }

   gMediaHandlerObj.mCmdStream = atoi(argv[1]);
   gMediaHandlerObj.mCmdTimeSlot = atoi(argv[2]);

   if(!atoi(argv[3]))
      gMediaHandlerObj.mNumOfChann = 1;
   else
      gMediaHandlerObj.mNumOfChann = atoi(argv[3]);

   sprintf(gMediaHandlerObj.mFileName, "%s", argv[4]);

   AcuMediaUtil::LoadSignals();

   gLog.SetProcessName(gProcessName);
   printf("%s: Starting....\n", gProcessName);

   if (false == gLog.Init())
   {
      printf("%s: Could not initialize log object. Exiting...\n", gProcessName);
      return 0;
   }

   gLog.GenerateLog(GSYS01);
   lProcessLock.SetProdProcName(MEDIA_NAME, gProcessName);

   if (LOCK_SUCCESS != lProcessLock.Lock())
   {
      printf("%s: Process already running. Exiting...\n", gProcessName);
      gLog.GenerateLog(GSYS16);
      return 0;
   }

   gTrace.SetTraceKey(TRACE_MEDIA_HANDLER_ENV);
   if (false == gMediaHandlerObj.Init())
   {
      return 0;
   }

   printf("%s: Initialization Okay...\n", gProcessName);
   gLog.GenerateLog(GSYS03);


   /*   while (true == Ss7Util::KeepRunning())
        {
   // Read from application Que
   if( true == gMediaHandlerObj.ReadReqMsg())
   {
   gMediaHandlerObj.ProcessReqMsg();
   }

   } // End of while
    */ gMediaHandlerObj.DeInit();
   printf("%s: Shutting down... \n", gProcessName);
   gLog.GenerateLog(GSYS02);
   return 0;
}


