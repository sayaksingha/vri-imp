static const char *id = "@(#) $Id: HmpHandlerMain.cc,v 1.1.2.1 2012/02/21 08:31:37 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpHandler.cc
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
// AUTHOR         : Ramprasad M                              12-JAN-2012
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "HmpHandler.h"
#include "ProcessLock.h"

Log                     gLog;
CTrace                  gTrace (TRACE_HMP_ENV);
TEXT                    gProcessName[HMP_MAX_PROCESS_NAME + 1] = "";
//HmpDbMgr                gDbObj;
HealthCheckApi          gHealthCheckObj;
HmpHandler              gHmpHandler;

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
   strcpy(gProcessName, "HMP");

   if( 3 != argc)
   {
      printf ("Usage: <%s> <MinCkt> <MaxCkt>\n", gProcessName);
      printf ("Trace: export %s=1\n", TRACE_HMP_ENV);
      return 1;
   }
   
   gHmpHandler.mMinCktId = atoi(argv[1]);
   gHmpHandler.mMaxCktId = atoi(argv[2]);
   if(gHmpHandler.mMinCktId > gHmpHandler.mMaxCktId)
   {
      printf("%s: Min Circuit ID is Greater than Max Circuit ID. Exiting..... \n",
            gProcessName);
      return 1;
   }
   sprintf(gProcessName, "%s%d-%d", gProcessName,
         gHmpHandler.mMinCktId,
         gHmpHandler.mMaxCktId);

   printf("%s: Starting.....\n",gProcessName);
   
   // Initialize Log object
   if(true != gLog.Init ())
   {
      printf("%s: Could not initialize log object. Exiting..... \n",
            gProcessName);
      return 1;
   }

   gLog.SetProcessName(gProcessName);

   // Handle Signals
   HmpUtil::LoadSignals ();

   // Check Process Lock
   lProcessLock.SetProdProcName(HMP_NAME, gProcessName);

   if(LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog(GSYS16);
      printf("%s: Process already running. Exiting.....\n", gProcessName);
      return 1;
   }
   
   // Startup Log
   gLog.GenerateLog(GSYS01);
   // Set Trace Key
   gTrace.SetTraceKey (TRACE_HMP_ENV);
   // Initialize DB object

//   if(DB_NO_ERR != gDbObj.Init ())
  // {
    //  printf("%s: Db object Init failed. Exiting.....\n", gProcessName);
      //return 1;
//   }

   if(true != gHmpHandler.Init())
   {
      printf("%s: Shutting down\n", gProcessName);
      gLog.GenerateLog(GSYS02);
      return 1;
   }

   gLog.GenerateLog(GSYS03);
   printf("%s: Initialization Okay.....\n", gProcessName);

   while(HmpUtil::KeepRunning())
   {
      if (HmpUtil::ReloadConfig ())
      {
         gHmpHandler.ReloadConfig ();
         HmpUtil::ResetConfigFlag ();
      }

      if(true == gHmpHandler.ReadRequest())
      {
         gHmpHandler.ProcessRequest();
      }
   }

   gHmpHandler.DeInit();

   printf("%s: Shutting down\n", gProcessName);
   gLog.GenerateLog(GSYS02);
   sleep(2);
   return 0;
}
