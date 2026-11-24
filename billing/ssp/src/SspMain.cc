//--------------------------------------------------------------------------
// NAME
// InRelayMain.cc
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
#include "InRelayStates.h"

Log                             gLog;
CTrace                          gTrace(TRACE_IN_RELAY_ENV);
TEXT                            gProcessName[IN_RLY_MAX_PROCESS_NAME + 1];
InRelayProc                     gInRelayObj;         
HealthCheckApi                  gHealthCheckObj;

// Will be used in Traces.. 
pthread_t  gInRspTid;
pid_t      gPid;

//*********************------Global Functions------********************
//----------------------------------------------------------------------
// METHOD NAME : StartUpFunInRspThread 
// DESCRIPTION : In Rsp Handling Thread Startup Function.
// PARAMETERS  : void*
// RETURN      : void*
//----------------------------------------------------------------------
void* StartUpFunInRspThread(void*)
{
   InRelayStates*  lpStateptr;
   UINT8          lEvtIdx = 0;
   
   printf( "//--------- Health Configuration ---------------------------------------//\n");
   printf( " File: health.cfg\n");
   printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_ALERT_THRESHOLD      = 3\n", IN_RLY_RELAY+1);
   printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_ERR_THRESHOLD        = 5\n", IN_RLY_RELAY+1);
   printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_PROCESS_MON_ERR_CODE = 1\n", IN_RLY_RELAY+1);

   printf( " File: ipc.cfg\n");
   printf( "HC_MODULE_%d_MSG_LEN = %u\n", IN_RLY_RELAY+1, sizeof (TcapMsg));
   printf( "MSG_HC_MODULE_%d_Q_RCV should be same as MSG_UGW_ASE_HDLR_Q_RCV\n",IN_RLY_RELAY+1);
   printf( "//----------------- End ------------------------------------------------//\n");

   // In Rsp Thread...
   while (InRelayUtil::KeepRunning() )
   {
      if (true == InRelayUtil::ReloadConfig() )
      {
         if ( true == gInRelayObj.ReloadConfig())
         {
            InRelayUtil::ResetConfigFlag();
         }
         else
         {
            cout << gProcessName <<": Reloading config data Failed." << endl;
            InRelayUtil::ResetConfigFlag();
         }
      }

      if ( true == gInRelayObj.ReadRspFromInQue(gInRspTid) )
      {
         // If some error ... drop the request and proced
         // with new request.. But don disturb the Existing Dlg info 
         if(false == gInRelayObj.ProcessInRsp(gInRspTid))
         continue;
      }
      else
         continue;

      // Print the current state and event..
      lEvtIdx   = 0;
      lpStateptr = gInRealyStateTable[gInRelayObj.mInThrdState];

      // Search for matching event in current state
      while (lpStateptr[lEvtIdx].event != IN_RLY_EVT_NULL)
      {
         // if event is defined in current state
         if (lpStateptr[lEvtIdx].event == gInRelayObj.mInThrdEvt)
         {
            // Event matched. Execute the corresponding functions
            for (int lIndex = 0; lIndex < lpStateptr[lEvtIdx].noOfActions; lIndex++)
            {
               EnumInRelayActions lAct = lpStateptr[lEvtIdx].act[lIndex];

               //if((gInRelayObj.*gInRealyActionTable[lAct])!= NULL)
               //{
                  if (false == (gInRelayObj.*gInRealyActionTable[lAct])(gInRelayObj.mInRspToMscReqDlgId,gInRspTid) )
                  {
                     // Send Error Response ???
                     gInRelayObj.ClearDlgInfo(gInRelayObj.mInRspToMscReqDlgId, gInRspTid);
                     break;
                  }
               //}
               }
            break;
         }

         lEvtIdx++;
      }
      
      // If no event is matching....
      if (lpStateptr[lEvtIdx].event == IN_RLY_EVT_NULL)
      {
         for (int lIndex = 0; lIndex < lpStateptr[lEvtIdx].noOfActions; lIndex++)
         {
            EnumInRelayActions lAct = lpStateptr[lEvtIdx].act[lIndex];

            //if ((gInRelayObj.*gInRealyActionTable[lAct]) != NULL)
            //{
               if (false == (gInRelayObj.*gInRealyActionTable[lAct])(gInRelayObj.mInRspToMscReqDlgId, gInRspTid))
               {
                  // Send Error Response ???
                  gInRelayObj.ClearDlgInfo(gInRelayObj.mInRspToMscReqDlgId, gInRspTid);
                  break;
               }
            //}
         }
      }
   }//Main while loop

}

//----------------------------------------------------------------------
// METHOD NAME : StartInRspThread
// DESCRIPTION : Initilize the object.
// PARAMETERS  : pthread_t*
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN StartInRspThread (pthread_t* id)
{

   // In Rsp Thread...
   if(0 == pthread_create(id, 0, StartUpFunInRspThread, 0))
   {
      T(gTrace, printf("%s: IN Rsp Handling Thread Creation Success....\n", gProcessName););
      pthread_detach(*id);
      return true;
   }

   else
   {
      TERR(gTrace, printf("%s: Failed to Create IN Rsp Handling Thread...\n", gProcessName););
      return false;
   }

   return true;
}

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
   InRelayStates*    lpStateptr;
   UINT8             lEvtIdx = 0;

   // storing name of executable
   strcpy(gProcessName, basename(argv[0]) );
   if (argc != 2)
   {
      cout << "USAGE: "<<gProcessName <<" <instance number>" << endl ;
      cout << "Trace: export "<< TRACE_IN_RELAY_ENV << "=1"<< endl;
      exit(1);
   }
   else
      strcat(gProcessName, argv[1]);

   printf( "//--------- Health Configuration ---------------------------------------//\n");
   printf( " File: health.cfg\n");
   printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_ALERT_THRESHOLD      = 3\n", IN_RLY_RELAY);
   printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_ERR_THRESHOLD        = 5\n", IN_RLY_RELAY);
   printf( "HC_MODULE_%d_HEARTBEAT_FAILURE_PROCESS_MON_ERR_CODE = 1\n", IN_RLY_RELAY);

   printf( " File: ipc.cfg\n");
   printf( "HC_MODULE_%d_MSG_LEN = %u\n", IN_RLY_RELAY, sizeof (InRlyDlgDetails));
   printf( "MSG_HC_MODULE_%d_Q_RCV should be same as MSG_UGW_ASE_HDLR_Q_RCV\n", IN_RLY_RELAY);
   printf( "//----------------- End ------------------------------------------------//\n");


   // Load signals...
   InRelayUtil::LoadSignals();

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

   lProcessLockObj.SetProdProcName(IN_RLY_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLockObj.Lock() )
   {
      gLog.GenerateLog(GSYS16);
      cout << gProcessName <<": Process already running. Exiting..." << endl;
      return -1;
   }

   // Set Trace Key
   gTrace.SetTraceKey(TRACE_IN_RELAY_ENV);
  
   // Health check object.. 
   if (false == gHealthCheckObj.Init(IN_RLY_RELAY))
   {
      printf("%s: Health Check Process Init Failed\n", gProcessName);
      return -1;
   }

   // Evr Api Obj...
   // ????

   // Class Object initilization including Thread start up && Db object initilization.
   if(false == gInRelayObj.Init())
   {
      cout << gProcessName <<": Initilization failed... exiting..." << endl;
      exit(-1);
   }
   else
      cout << "Success" << endl;

   // Start In Rsp thread..
   //if(false == StartInRspThread(&gInRspTid))
   //{
   //   cout << gProcessName <<": Failed to create threads exiting..." << endl;
   //   exit(-1);
   //}
   
   // will be used in traces.
   gPid = getpid();

   while (InRelayUtil::KeepRunning() )
   {
      if (true == InRelayUtil::ReloadConfig() )
      {
         if ( true == gInRelayObj.ReloadConfig())
         {
            InRelayUtil::ResetConfigFlag();
         }
         else
         {
            cout << gProcessName <<": Reloading config data Failed." << endl;
            InRelayUtil::ResetConfigFlag();
         }
      }

      if ( true == gInRelayObj.ReadReqFromMscQue(gPid) )
      { 
         // If some error ... drop the request and proced
         // with new request.. But don disturb the Existing Dlg info 
         if(false == gInRelayObj.ProcessMscRequest(gPid))
            continue;
      }
      else
         continue;
#if 0
      // Print the current state and event..
      lEvtIdx   = 0;
      lpStateptr = gInRealyStateTable[gInRelayObj.mMscThrdState];

      // Search for matching event in current state
      while (lpStateptr[lEvtIdx].event != IN_RLY_EVT_NULL)
      {
         // if event is defined in current state
         if (lpStateptr[lEvtIdx].event == gInRelayObj.mMscThrdEvt)
         {
            // Event matched. Execute the corresponding functions
            for (int lIndex = 0; lIndex < lpStateptr[lEvtIdx].noOfActions; lIndex++)
            {
               EnumInRelayActions lAct = lpStateptr[lEvtIdx].act[lIndex];

               //if((gInRelayObj.*gInRealyActionTable[lAct])!= NULL)
               //{
               if (false == (gInRelayObj.*gInRealyActionTable[lAct])(gInRelayObj.mMscReqDlgId, gPid) )
               {
                  // Send Error Response ???
                  gInRelayObj.ClearDlgInfo(gInRelayObj.mMscReqDlgId, gPid);
                  break;
               }
               // }
            }
            break;
         }

         lEvtIdx++;
      }

      // If no event is matching....
      if (lpStateptr[lEvtIdx].event == IN_RLY_EVT_NULL)
      {
         for (int lIndex = 0; lIndex < lpStateptr[lEvtIdx].noOfActions; lIndex++)
         {
            EnumInRelayActions lAct = lpStateptr[lEvtIdx].act[lIndex];

            //if ((gInRelayObj.*gInRealyActionTable[lAct]) != NULL)
            //{
            if (false == (gInRelayObj.*gInRealyActionTable[lAct])(gInRelayObj.mMscReqDlgId,gPid))
            {
               // Send Error Response ???
               gInRelayObj.ClearDlgInfo(gInRelayObj.mMscReqDlgId, gPid);
               break;
            }
            //}
         }
      }
      #endif
   }//while loop

   gLog.GenerateLog(GSYS02);
   cout << gProcessName <<": InRealy process Shutting Down..." << endl;
   return 0;
}

