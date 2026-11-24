//--------------------------------------------------------------------------
// NAME
// BillingServerMain.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright (C) 2006,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd.
//
// DESCRIPTION
//
// Originated:                                           Date:31-July-2008
//--------------------------------------------------------------------------
#include "BillingServer.h"

Log                gLog;
CTrace             gTrace(TRACE_BILLING_SERVER_ENV);
TEXT               gProcessName[MAX_PROCESS_NAME + 1];
BillingServer      gBillingServerObj;         
//HealthCheckApi     gHealthCheckObj;
TEXT               gLogText[MAX_LOG_TEXT_LEN];
unsigned int       gReferenceNum;
//--------------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name)
// RETURN      : 0
// THROWS      : none
//--------------------------------------------------------------------------
int main(int argc, char* argv[])
{
   // Thread Id's
   ProcessLock       lProcessLockObj;
   BillingRequest    lBillingRequest;

   // storing name of executable
   strcpy(gProcessName, basename(argv[0]) );
   if (argc != 2)
   {
      cout << "USAGE: "<<gProcessName <<" <instance number>" << endl ;
      cout << "Trace: export "<< TRACE_BILLING_SERVER_ENV << "=1"<< endl;
      
      printf( "//--------- Health Configuration ---------------------------------------//\n");
      printf( " File: health.cfg\n");
      printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_ALERT_THRESHOLD      = 3\n", IVR_BILLING_SERVER);
      printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_ERR_THRESHOLD        = 5\n", IVR_BILLING_SERVER);
      printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_PROCESS_MON_ERR_CODE = 1\n", IVR_BILLING_SERVER);

      printf( " File: ipc.cfg\n");
      printf( "HC_MODULE_%d_MSG_LEN = %ld\n", IVR_BILLING_SERVER, sizeof (BillingRequest));
      printf( "MSG_HC_MODULE_%d_Q_RCV should be same as MSG_UGW_ASE_HDLR_Q_RCV\n", IVR_BILLING_SERVER);
      printf( "//----------------- End ------------------------------------------------//\n");
      exit(1);
   }
   else
      strcat(gProcessName, argv[1]);

   // Load signals...
   IvrUtil::LoadSignals();

   gLog.SetProcessName(gProcessName);
   cout << gProcessName <<": Starting..." << endl;
   
   // Initialize log object
   if (false == gLog.Init() )
   {
      cout << gProcessName <<": Could not initialize log object. Exiting..." << endl;
      return -1;
   }

   // Startup log
   gLog.GenerateLog(GSYS01);

   lProcessLockObj.SetProdProcName((TEXT*)BILLING_SERVER_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLockObj.Lock() )
   {
      gLog.GenerateLog(GSYS16);
      cout << gProcessName <<": Process already running. Exiting..." << endl;
      return -1;
   }

   // Set Trace Key
   gTrace.SetTraceKey(TRACE_BILLING_SERVER_ENV);
  
   // Health check object.. 
   /*if (false == gHealthCheckObj.Init(IVR_BILLING_SERVER))
   {
      printf("%s: Health Check Process Init Failed\n", gProcessName);
      return -1;
   }
    */
   // Class Object initilization including Thread start up && Db object initilization.
   if(false == gBillingServerObj.Init())
   {
       printf ("%s: \33[31mBilling Server Initilization failed... exiting...\33[0m\n", gProcessName);
      exit(-1);
   }
   else
      printf ("%s: \33[32mInitialization Okay...\33[0m\n", gProcessName);

   while (IvrUtil::KeepRunning() )
   {
      if (true == IvrUtil::ReloadConfig() )
      {
         if ( true == gBillingServerObj.ReloadConfig())
         {
            IvrUtil::ResetConfigFlag();
         }
         else
         {
            cout << gProcessName <<": Reloading config data Failed." << endl;
            IvrUtil::ResetConfigFlag();
         }
      }

      if ( true == gBillingServerObj.ReadBillingRequest(lBillingRequest) )
      { 
         if(false == gBillingServerObj.ProcessBillingRequest(lBillingRequest))
            continue;
      }
      else
         usleep(1000);
   }//while loop

   gLog.GenerateLog(GSYS02);
   cout << gProcessName <<": Billing Server Shutting Down..." << endl;
   return 0;
}

