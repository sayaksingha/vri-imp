static const char *id = "@(#) $Id: IsdnHandlerMain.cc,v 1.1.4.3 2016/04/12 21:59:37 shijuc Exp $";
//----------------------------------------------------------------------
// NAME  IsdnHandler.cc
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
// AUTHOR         : Chethan V M
//----------------------------------------------------------------------
#include "IsdnHandler.h"
#include "ProcessLock.h"

Log                     gLog;
CTrace                  gTrace (TRACE_ISDN_ENV);
TEXT                    gProcessName[ISDN_MAX_PROCESS_NAME + 1] = "";
HealthCheckApi          gHealthCheckObj;
IsdnHandler             gIsdnHandler;

//----------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name)
// RETURN      : INT
//----------------------------------------------------------------------
int main(int argc, char* argv[])
{
   ProcessLock             lProcessLock;

   //strcpy(gProcessName, basename(argv[0]));
   strcpy(gProcessName, "ISDN");

   if( 3 != argc)
   {
      printf ("Usage: <%s> <MinCkt> <MaxCkt>\n", gProcessName);
      printf ("Trace: export %s=1\n", TRACE_ISDN_ENV);
      return 1;
   }
   
   gIsdnHandler.mMinCktId = atoi(argv[1]);
   gIsdnHandler.mMaxCktId = atoi(argv[2]);
   if(gIsdnHandler.mMinCktId > gIsdnHandler.mMaxCktId)
   {
      printf("%s: Min Circuit ID is Greater than Max Circuit ID. Exiting..... \n",
            gProcessName);
      return 1;
   }
   sprintf(gProcessName, "%s%d-%d", gProcessName,
         gIsdnHandler.mMinCktId,
         gIsdnHandler.mMaxCktId);

   printf("%s: Starting.....\n",gProcessName);
   
   // Initialize Log object
   if(true != gLog.Init ())
   {
      printf("%s: Could not initialize log object. Exiting..... \n",
            gProcessName);
      return 1;
   }


   gLog.SetProcessName(gProcessName);

   // Set Trace Key
   gTrace.SetTraceKey (TRACE_ISDN_ENV);

   // Handle Signals
   IsdnUtil::LoadSignals ();

   // Check Process Lock
   lProcessLock.SetProdProcName(ISDN_NAME, gProcessName);

   if(LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog(GSYS16);
      printf("%s: Process already running. Exiting.....\n", gProcessName);
      return 1;
   }
   
   // Startup Log
   gLog.GenerateLog(GSYS01);

   if(true != gIsdnHandler.Init())
   {
      printf("%s: Shutting down\n", gProcessName);
      gLog.GenerateLog(GSYS02);
      return 1;
   }

   gLog.GenerateLog(GSYS03);
   printf("%s: Initialization Okay.....\n", gProcessName);

   while(IsdnUtil::KeepRunning())
   {
      if (IsdnUtil::ReloadConfig ())
      {
         gIsdnHandler.ReloadConfig ();
         IsdnUtil::ResetConfigFlag ();
      }

      if(true == gIsdnHandler.ReadRequest())
      {
         gIsdnHandler.ProcessRequest();
      }
   }

   gIsdnHandler.DeInit();

   printf("%s: Shutting down\n", gProcessName);
   gLog.GenerateLog(GSYS02);
   sleep(2);
   return 0;
}
