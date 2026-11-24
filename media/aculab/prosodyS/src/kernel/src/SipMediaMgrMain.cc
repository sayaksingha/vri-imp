static const char *id = "@(#) $Id: SipMediaMgrMain.cc,v 1.1.4.1.2.2 2016/07/05 03:10:51 mram Exp $";
//----------------------------------------------------------------------
// NAME
//       SipMediaMgrMain.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "SipMediaMgr.h"
#include "ProcessLock.h"


Log               gLog;
CTrace            gTrace (TRACE_SIP_HDLR_ENV);
TEXT              gProcessName[SIP_MAX_PROCESS_NAME + 1] = "";
SipMediaMgr       gSipMediaMgr;

//uid_t gEuid, gRuid;

void *CallControlThread (void *);

//----------------------------------------------------------------------
// METHOD      : CallControlThread
// DESCRIPTION : Thread execution C function
// PARAMETER   : void *
// RETURN      : void
//----------------------------------------------------------------------
void * CallControlThread (void *lPtr)
{
   while (true == SipUtil::KeepRunning())
   {
      // Reload the configuration parameters when SIGCFG signal is received
      if (true == SipUtil::ReloadConfig())
      {
         //T(gTrace, printf("%s: Reloading config data in Thread...\n", gProcessName););
         gSipMediaMgr.ReloadConfig();
         SipUtil::ResetConfigFlag();
      }
      if (false == gSipMediaMgr.ProcessEvents())
      {
         usleep(10);
      }
   }
   pthread_exit(NULLPTR);
}

//--------------------------------------------------------------------
// METHOD      : HandleApplnMsg
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
// THROWS      : none
//--------------------------------------------------------------------
void * HandleApplnMsg(void *ptr)
{
   while (true == SipUtil::KeepRunning())
   {
      GenIsupMsg  lRxIsupMsg;
      memset(&lRxIsupMsg,0,sizeof(GenIsupMsg));

      // Reload the configuration parameters when SIGCFG signal is received
      if (true == SipUtil::ReloadConfig())
      {
         //T(gTrace, printf("%s: Reloading config data in Thread...\n", gProcessName););
         gSipMediaMgr.ReloadConfig();
         SipUtil::ResetConfigFlag();
      }

      if( true == gSipMediaMgr.ReadApplnMsg(lRxIsupMsg))
      {
         if (false == gSipMediaMgr.ProcessApplnMsg(lRxIsupMsg))
         {
            usleep(10); // check
         }
      }
   }
   pthread_exit(NULLPTR);
}

//----------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name)
// RETURN      : INT
//----------------------------------------------------------------------
int main(int argc, char* argv[])
{
   TiNGtrace = 2;
   ProcessLock       lProcessLock;

   //strcpy(gProcessName, basename(argv[0]));
   strcpy(gProcessName, "ACU_SIP");

   //------------------- Health Configuration -----------------------

   if( 2 != argc)
   {
      printf ("Usage %s <instance number>\n", gProcessName);
      printf ("Trace: export %s=1\n", TRACE_SIP_HDLR_ENV);
      return 1;
   }

   //strcat(gProcessName, argv[1]);
   printf("%s: Starting.....\n",gProcessName);

   //gRuid = getuid();
   //gEuid = geteuid();
   //printf("%s: Starting.....gRuid:%d gEuid:%d\n",gProcessName, gRuid, gEuid);
   //if(0 != seteuid(gEuid))
   //{
    //  printf("%s: Setting Real User Id Failed. Exiting..... \n",
     //       gProcessName);
     // return 1;
   //}

   //if (-1 == system("source /opt/aculab/setV6.sh"))
   //{
    //  printf("%s: System Failed. Exiting..... \n",
     //       gProcessName);
      //return 1;
   //}


   // Initialize Log object
   if(true != gLog.Init ())
   {
      printf("%s: Could not initialize log object. Exiting..... \n",
            gProcessName);
      return 1;
   }

   gLog.SetProcessName(gProcessName);

   // Handle Signals
   SipUtil::LoadSignals ();

   // Check Process Lock
   lProcessLock.SetProdProcName(SIP_NAME, gProcessName);

   if(LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog(GSYS16);
      printf("%s: Process already running. Exiting.....\n", gProcessName);
      return 1;
   }

   // Startup Log
   gLog.GenerateLog(GSYS01);

   if(true != gSipMediaMgr.Init())
   {
      return 1;
   }

   BOOLEAN     lRetVal;
   pthread_t   lEventThread;

   lRetVal = pthread_create (&lEventThread, NULL, &CallControlThread, NULL);
   if (0 == lRetVal)
   {
      printf ("%s: Tid:%lu CallControlThread Thread Creation success\n",
            gProcessName, lEventThread);
      pthread_detach (lEventThread);
   }
   else
   {
      printf ("%s:\33[31m Error CallControlThread Thread Creation failed. ErrCode:%d Exiting...\33[0m\n", 
            gProcessName, lRetVal);
      return false;
   }
   
   pthread_t   lApplnThread;

   lRetVal  =    pthread_create (&lApplnThread, NULL, &HandleApplnMsg, NULL);
   if (0 == lRetVal)
   {
      printf ("%s: Tid:%lu Application Thread Creation success\n",
            gProcessName, lApplnThread);
      pthread_detach (lApplnThread);
   }
   else
   {
      printf ("%s:\33[31m Error Application Thread Creation failed. ErrCode:%d Exiting...\33[0m\n", 
            gProcessName, lRetVal);
      return false;
   }

   gLog.GenerateLog(GSYS03);
   printf("%s: Initialization Okay.....\n", gProcessName);

   while(SipUtil::KeepRunning())
   {
      if(SipUtil::ReloadConfig ())
      {
         gSipMediaMgr.ReloadConfig ();
         SipUtil::ResetConfigFlag();
      }
      if(gSipMediaMgr.ReadMediaMsg())
      {
         gSipMediaMgr.ProcessMediaMsg();
      }
   }

   //gSipMediaMgr.DeInit();
   printf("%s: Shutting down\n", gProcessName);
   gLog.GenerateLog(GSYS02);
   return 0;
}
