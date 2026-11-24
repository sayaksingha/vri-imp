static const char* id="@(#) $Id: AculabMpMain.cc,v 1.1.4.3 2016/04/12 21:59:28 shijuc Exp $";
//-----------------------------------------------------------------------------
// NAME           : AculabMpMain.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2012 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Roopa A M                               Date: 30-JUL-2012
//----------------------------------------------------------------------------

#include "AculabMpMgr.h"
#include "ProcessLock.h"

#define TRACE_ACULAB_MP_MGR_ENV "TRACE_ACULAB_MP_MGR"

TEXT                    gProcessName[ACULAB_MAX_PROCESS_NAME + 1] = "";
CTrace                  gTrace(TRACE_ACULAB_MP_MGR_ENV);
Log                     gLog;

AculabMPMgr             gAculabMpMgrObj;

//--------------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name)
// RETURN      : 0
// THROWS      : none
//--------------------------------------------------------------------------
int main(int argc,char* argv[])
{
   TEXT lCmd[100] = "";
   ProcessLock lProcessLock;

   strcpy(gProcessName, basename(argv[0]) );
   sprintf(lCmd, "killall -12 %s", gProcessName);
   if (argc != 2)
   {
      printf("Usage %s <CardId>\n", gProcessName);
      printf("export %s=1\n", TRACE_ACULAB_MP_MGR_ENV);
      exit(1);
   }

   gAculabMpMgrObj.mCardId = atoi(argv[1]);

   strcat (gProcessName, argv[1]);

   AcuMediaUtil::LoadSignals();

   printf("\n%s: Starting...\n", gProcessName);
   gLog.SetProcessName(gProcessName);

   if (false == gLog.Init() )
   {
      printf("%s: Failed to initialize Log Obj. Exiting...\n", gProcessName);
      exit(1);
   }

   gLog.GenerateLog(GSYS01);
   lProcessLock.SetProdProcName("MEDIA_NAME", gProcessName);

   if (LOCK_SUCCESS != lProcessLock.Lock())
   {
      gLog.GenerateLog(GSYS16);
      printf("%s: Process already running. Exiting...\n",gProcessName);
      return false;
   }

   gTrace.SetTraceKey(TRACE_ACULAB_MP_MGR_ENV);

   if (false == gAculabMpMgrObj.Init() )
   {
      TERR(gTrace, printf("%s: ACU Server Init Failed.\n", gProcessName););
      exit(1);
   }

   gLog.GenerateLog(GSYS03);
   printf("%s:\33[32m Initialization Okay...\33[0m\n",gProcessName);

   while(AcuMediaUtil::KeepRunning())
   {
      if (true == AcuMediaUtil::ReloadConfig())
      {
         if ( true == gAculabMpMgrObj.ReloadConfig())
         {
            T(gTrace, printf("%s: Reloading config data.\n", gProcessName););
            AcuMediaUtil::ResetConfigFlag();
            system(lCmd);
         }
         else
         {
            TERR(gTrace, printf("%s: Reloading config data Failed.\n", gProcessName););
            gLog.GenerateLog(GSYS11);
            AcuMediaUtil::ResetConfigFlag();
         }
      }

      if(gAculabMpMgrObj.ReadReq())
      {
         gAculabMpMgrObj.DistributeReq();
      }
   }

   kill(getpid(), 0);

   gLog.GenerateLog(GSYS02);
   printf("%s: Shutting down... and Killing all Threads\n", gProcessName);
   return 0;
}
