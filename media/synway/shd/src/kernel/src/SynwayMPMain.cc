static const char* id="@(#) $Id: SynwayMPMain.cc,v 1.1.4.3 2016/04/12 21:59:39 shijuc Exp $" ;
//-----------------------------------------------------------------------------
// NAME           : SynwayMPMain.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Chethan V M           Date:
//----------------------------------------------------------------------------

#include "SynwayMPManager.h"
#include "ProcessLock.h"

#define TRACE_SYNWAY_MP_MANAGER_ENV "TRACE_SYNWAY_MP_MANAGER"

TEXT                    gProcessName[SYNWAY_MAX_PROCESS_NAME];
CTrace                  gTrace(TRACE_SYNWAY_MP_MANAGER_ENV);
Log                     gLog;

SynwayMPManager   gSynwayMPManagerObj;

//--------------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name)
// RETURN      : 0
// THROWS      : none
//--------------------------------------------------------------------------
int main(int argc,char* argv[])
{
   ProcessLock       lProcessLock;
   TEXT              lTempStr[20 + 1] = "";
   TEXT		     lLogText[MAX_LOG_TEXT_LEN] = "";	
   // storing name of executable
   strcpy(gProcessName, basename(argv[0]) );
   if (argc != 4)
   {
      printf("Usage %s <BoardId> <ShConfig File> <ShIndex File>\n",
				gProcessName);
		printf("If Configuration File Not Provided Takes Default\n");
      printf("export %s=1\n",TRACE_SYNWAY_MP_MANAGER_ENV);
		exit(1);
   }
   else
   {
      sprintf(lTempStr,"%d", atoi(argv[1]));
		gSynwayMPManagerObj.mBoardId = atoi(argv[1]);
		sprintf(gSynwayMPManagerObj.mInPutShCfgFile, "%s", argv[2]);
		sprintf(gSynwayMPManagerObj.mInPutShIdxFile, "%s", argv[3]);
		strcat(gProcessName, lTempStr);
	}

   SynwayUtil::LoadSignals();

   printf("\n%s: Starting...\n", gProcessName);
   gLog.SetProcessName(gProcessName);

   if (false == gLog.Init() )
   {
      printf("%s: Failed to initialize Log Obj. Exiting...\n", gProcessName);
      exit(1);
   }

   // Start up log
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Init...");
   gLog.GenerateLog(GSYS01, lLogText);
   lProcessLock.SetProdProcName((TEXT*)"MP_NAME", gProcessName);

   if (LOCK_SUCCESS != lProcessLock.Lock())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Process already running. Exiting...");
      gLog.GenerateLog(GSYS16, lLogText);
      printf("%s: Process already running. Exiting...\n",gProcessName);
      return false;
   }

   // Set Trace Key
   gTrace.SetTraceKey(TRACE_SYNWAY_MP_MANAGER_ENV);

   if (false == gSynwayMPManagerObj.Init() )
   {
      TERR(gTrace, printf("%s: Tone Server Init Failed.\n", gProcessName);)
      exit(1);
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Initialization Okay...");
   gLog.GenerateLog(GSYS03, lLogText);
   printf("%s:\33[32m Initialization Okay...\33[0m\n",gProcessName);

   while(SynwayUtil::KeepRunning())
   {
      if (true == SynwayUtil::ReloadConfig())
      {
         if ( true == gSynwayMPManagerObj.ReloadConfig())
         {
            T(gTrace, printf("%s: Reloading config data.\n", gProcessName);)
            SynwayUtil::ResetConfigFlag();
         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "Reloading config data Failed.\n");
	    TERR(gTrace, printf("%s: %s", gProcessName, lLogText);)
            gLog.GenerateLog(GSYS11, lLogText);
            SynwayUtil::ResetConfigFlag();
         }
      }

      if(gSynwayMPManagerObj.ReadReq())
      {
         gSynwayMPManagerObj.ProcessReq();
      }
   }//while loop
   if (false == gSynwayMPManagerObj.DeInit())
   {
      printf("%s: DeInit Libraries Failed ...\n", gProcessName);
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Shutting down... and Killing all Threads\n");
   gLog.GenerateLog(GSYS02, lLogText);
   printf("%s:  \33[31mShutting down... and Killing all Threads\33[0m\n ", gProcessName);
   return 0;
}
