//----------------------------------------------------------------------
// NAME
// IvrFeedbackMain.cc 
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright (C) 2005
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated: Divya K                        Date: 17-JAN-2012
//----------------------------------------------------------------------
#include "IvrFeedback.h"

Log              gLog;
CTrace           gTrace (TRACE_IVR_SCHEDULER_ENV);
TEXT             gProcessName[MAX_PROCESS_NAME + 1] = "";
extern TEXT        gLogText[MAX_LOG_TEXT_LEN];
//HealthCheckApi   gHealthCheckObj;
unsigned int   gReferenceNum;

//-------------------------------------------------------------
// METHOD      : HandleResponse
// DESCRIPTION : Handles the response sent from Session Mgr
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
void  * HandleResponse (void* ptr)
{
   IvrFeedback        lSchedulerObj;

   if (false == lSchedulerObj.Init ())
      return NULLPTR;
   
   while (IvrUtil::KeepRunning ())
   {
      if (IvrUtil::ReloadConfig ())
      {
         IvrUtil::ResetConfigFlag ();
      }

      if (true == lSchedulerObj.ReadResponse ())
      {
         lSchedulerObj.ProcessResponse();
      }
      else
      {
         sleep(1);
      }
   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}
//-------------------------------------------------------------
// METHOD      :
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
int main(int argc, char **argv)
{
   //KernelMsg        lKernelMsg;
   ProcessLock      lProcessLock;
   IvrFeedback        lSchedulerObj;

   printf ("Trace: export %s=1\n", TRACE_IVR_SCHEDULER_ENV);
   if (argc != 2)
   {
      printf ("Usage %s <Instance> \n", gProcessName);
      return 1;
   }
   else
   {
      strcpy (gProcessName, basename(argv[0]));
      strcat (gProcessName, argv[1]);
   }

   IvrUtil::LoadSignals ();

   gLog.SetProcessName (gProcessName);
   printf ("%s: Starting...\n", gProcessName);

   // Initialize log object
   if (true != gLog.Init ())
   {
      printf ("%s: \33[31mCould not initialize log object. Exiting...\33[0m\n",
            gProcessName);
      return 1;
   }

   lProcessLock.SetProdProcName ((TEXT*)IVR_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog (GSYS16);
      printf ("%s: \33[31mProcess already running. Exiting...\33[0m\n",
            gProcessName);
      return 1;
   }
   // Startup log
   gLog.GenerateLog (GSYS01);

   // Set Trace Key
   gTrace.SetTraceKey (TRACE_IVR_SCHEDULER_ENV);

   //Initialise Evr Writer
   if (false == lSchedulerObj.Init ())
      return 1;

   gLog.GenerateLog (GSYS03);
   printf ("%s: \33[32mInitialization Okay...\33[0m\n", gProcessName);

   while (IvrUtil::KeepRunning ())
   {
      if (IvrUtil::ReloadConfig ())
      {
         IvrUtil::ResetConfigFlag ();
      }

      if (true == lSchedulerObj.ReadRequest ())
      {
      }
      else
      {
         sleep(1);
      }
   }

   printf ("%s: \33[31mShutting down...\33[0m\n", gProcessName);
   gLog.GenerateLog (GSYS02);

   return 0;
}

