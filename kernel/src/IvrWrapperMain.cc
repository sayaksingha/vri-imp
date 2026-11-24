static const char* id="@(#) $Id: IvrWrapperMain.cc,v 1.1.1.1.2.5.2.1 2016/05/13 02:14:34 mram Exp $";
//---------------------------------------------------------------------
// NAME      IvrWrapperMain.cc
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
//
// DESCRIPTION
//
// Originated : Anupama R                           Date : 21-JAN-2011 
//---------------------------------------------------------------------
#include "IvrWrapper.h"

Log       gLog;
CTrace    gTrace(TRACE_IVR_WRAPPER);
TEXT      gProcessName[IVR_MAX_PROCESS_NAME];

//---------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name), argv[1] (used for instance)
// RETURN      : 0
// THROWS      : none
//---------------------------------------------------------------------
int main(int argc,char* argv[])
{
   ProcessLock    lProcessLock;
   IvrWrapper    	lIvrWrapper;
   TEXT           lLogText[MAX_LOG_TEXT_LEN];

	// storing name of executable
   strcpy(gProcessName, "MSG_WRP");
   if (argc != 2)
   {
       printf("Usage %s <instance number> \n", gProcessName);
       return 1;
   }
   else
   {
     //strcat(gProcessName, argv[1]);
	}

	IvrUtil::LoadSignals();
   
   gLog.SetProcessName(gProcessName);
   printf("%s: Starting...\n", gProcessName);
   
   if (false == gLog.Init())
   {
	 	printf("%s: Could not initialize log object. Exiting...\n", gProcessName);
      return 1;
   }
   
	// Startup log
	gLog.GenerateLog(GSYS01);
	
   lProcessLock.SetProdProcName((TEXT*)"IvrWrapper", gProcessName);
   if (LOCK_SUCCESS != lProcessLock.Lock())
   {
       printf("%s: Process already running. Exiting...\n", gProcessName);
       gLog.GenerateLog(GSYS16);
        return 1;
   }

	// Set Trace Key
	gTrace.SetTraceKey(TRACE_IVR_WRAPPER);

   if (false == lIvrWrapper.Init())
   {
       return -1;
   }

   printf ("%s: \33[32mInitialization Okay...\33[0m\n", gProcessName);
   gLog.GenerateLog(GSYS03, lLogText);
   
   while (true == IvrUtil::KeepRunning())
   {
      lIvrWrapper.ReadAndProcessRequest();
   }//while loop
   
	T(gTrace, printf("%s: Shutting down...\n", gProcessName);)
   gLog.GenerateLog(GSYS02, lLogText);
   return 0;
}
