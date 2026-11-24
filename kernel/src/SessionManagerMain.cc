static const char* id="@(#) $Id: SessionManagerMain.cc,v 1.1.1.1.2.7.2.2 2018/01/17 22:50:44 mram Exp $" ;
//---------------------------------------------------------------
// NAME      SessionManagerMain.cc
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
// Originated :                             Date:10/11/2010
//---------------------------------------------------------------
#include "SessionStates.h"
#include "IvrChannelApi.h"


Log       		gLog;
CTrace    		gTrace(TRACE_IVR_SESSION_ENV);
TEXT      		gProcessName[IVR_MAX_PROCESS_NAME];
//HealthCheckApi gHealthCheckObj;

MediaInterface			gMediaInterface;
IvrChannelApi	gIvrChannelApi;
//UINT32        	gReferenceNum;
unsigned int 	gReferenceNum;
INT32          gIntChildPid[IVR_MAX_CHANNEL_ID];

extern IvrDbManager	gIvrDbObj;
EnumCallState      gCurrentState = ST_NO_STATE;
EnumCallEvent      gCurrentEvent = EVT_NULL;
extern UINT16      gInChannelId;
extern KernelMsg   gKernelMsg;
extern TEXT        gLogText[MAX_LOG_TEXT_LEN];
UINT16             gMonitorInterval=10;
BOOLEAN            gScheduleReq = false;

/* Added gConnectToRedis parameter for connecting to Redis incase few required SIP info Params are decoded by Sniffer sipgrep */
BOOLEAN           gConnectToRedis = false;
BOOLEAN           gIsRedisConnected = false;

void ProcessChildFunction(UINT16 channelId, int argc, char* argv[])
{
   TEXT	lLogText[MAX_LOG_TEXT_LEN];
   State*   lStatePtr;
   UINT8    lEvtIdx = 0;

   gInChannelId = channelId;
   memset(argv[0], 0, strlen(argv[0]));
   sprintf(argv[0], "SN-%04d", gInChannelId);
   strcpy(gProcessName, argv[0]);
   gLog.SetProcessName(gProcessName);

   // Child Process
   T(gTrace, printf("%s: %s \33[32mChild created successfully... \33[0m\n", gProcessName, gLogText);)
      gLog.GenerateLog(SESN_IVR10, gLogText);

   //Calculate the start time
   //GetSystemTime(gCdrStartTime);

   gIvrDbObj.ReInit();
   gCurrentState  = ST_NO_STATE;
   gCurrentEvent  = EVT_NULL;
   if(gConnectToRedis)
   {
      if(NULL == IvrRedisUtil::ConnectToIvrRedis()) {
         TERR(gTrace, printf("%s: Error Connecting to Redis Server on 127.0.0.1:6379\n", gProcessName););
      }
      else
      {
         T(gTrace, printf("%s: Connected to Redis Server on 127.0.0.1:6379\n", gProcessName););
         gIsRedisConnected = true;
      }
   }
   while (true  == IvrUtil::KeepRunning())
   {
      lEvtIdx = 0;

      if (true == ReadMsgFromChildQ())
      {
         gCurrentEvent = GetEvent();
      }
      else
      {
         continue;
      }

      lStatePtr = gStateTable[gCurrentState];
      sprintf(lLogText, "%s CurrentState:%s CurrentEvent:%s",	gLogText,
            ConvertStateIdentifier(gCurrentState), ConvertEventIdentifier(gCurrentEvent));
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText);); 
      gLog.GenerateLog(SESN_IVR36, lLogText);

      while (lStatePtr[lEvtIdx].evt != EVT_NULL)
      {
         if (lStatePtr[lEvtIdx].evt == gCurrentEvent)
         {
            for (int lIndex = 0; lIndex < lStatePtr[lEvtIdx].noOfActions; 
                  lIndex++)
            {
               if (gActionTable[lStatePtr[lEvtIdx].fid[lIndex]] != NULL)
               {
                  gActionTable[lStatePtr[lEvtIdx].fid[lIndex]]();
               }
            }

            break;
         }

         lEvtIdx++;
      } 

      if (lStatePtr[lEvtIdx].evt == EVT_NULL)
      {
         for (int lIndex = 0; lIndex < lStatePtr[lEvtIdx].noOfActions; lIndex++)
         {
            if (gActionTable[lStatePtr[lEvtIdx].fid[lIndex]] != NULL)
            {
               gActionTable[lStatePtr[lEvtIdx].fid[lIndex]]();
            }
         }
      }


      T(gTrace, printf("%s: %s State changed/resumed to CurrentState:%s\n",
               gProcessName,
               gLogText,
               ConvertStateIdentifier(gCurrentState));)
   } 
}

//-----------------------------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : argc, argv[0] (used for process name)
// RETURN      : 0
// THROWS      : none
//------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
   TEXT	lLogText[MAX_LOG_TEXT_LEN];
   ProcessLock lProcessLock;
   EnumIVRChannelStatus	lChannelStatus;
   // storing name of executable
   //strcpy(gProcessName, basename(argv[0]));
   strcpy(gProcessName, "IVR_SM");

   if (argc < 2)
   {
      printf("Usage %s <instance number> \n", gProcessName);
      printf ("Trace: export %s=1\n", TRACE_IVR_SESSION_ENV);
      return 1;
   }
   else
   {
      strcat(gProcessName, argv[1]);
      if(argc >= 3)
         gMonitorInterval = atoi(argv[2]);
   }

   IvrUtil::LoadSignals();

   // setting name of executable for log
   gLog.SetProcessName(gProcessName);

   printf("\n%s: Starting... \n", gProcessName);

   if (false == gLog.Init())
   {
      TERR(gTrace, printf("%s:\33[31m Failed to initialize Log Obj. Exiting...\33[0m\n", gProcessName);)
         return 1;
   }

   lProcessLock.SetProdProcName ((TEXT*)IVR_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog (GSYS16);
      printf ("%s: Process already running. Exiting...\n", gProcessName);
      return 1;
   }

   // Start up log
   gLog.GenerateLog(GSYS01);

   // Set Trace Key
   gTrace.SetTraceKey(TRACE_IVR_SESSION_ENV);

   if (false == Init())
   {
      TERR(gTrace, printf("%s: \33[31m Failed to initialize SessionManager Obj. Exiting...\33[0m\n",
               gProcessName);)
         return 1;
   }

   if (false == gIvrChannelApi.Init())
   {
      TERR(gTrace, printf("%s:\33[31m Failed to initialize Channel API. Exiting...\33[0m\n", gProcessName);)
         return 1;
   }

   if (false == gMediaInterface.Init())
   {
      TERR(gTrace, printf("%s:\33[31m Failed to initialize Media Library. Exiting...\33[0m\n", gProcessName);)
         return 1;
   }

   if(gConnectToRedis)
   {
      if(NULL == IvrRedisUtil::ConnectToIvrRedis())
      {
         TERR(gTrace, printf("%s: Error Connecting to Redis Server on 127.0.0.1:6379\n", gProcessName););
         return 1;
      }
      else
      {
         T(gTrace, printf("%s: Connected to Redis Server on 127.0.0.1:6379\n", gProcessName););
         gIsRedisConnected = true;
      }
   }

   // Resetting Channel Instance Status
   for(UINT16 lChannelId = 1; lChannelId < IVR_MAX_CHANNEL_ID; lChannelId++)
      gIntChildPid[lChannelId] = -2;

   T(gTrace, printf("%s: Monitor Interval = %d Seconds\n", gProcessName, gMonitorInterval););

   printf("%s:\33[32m Initialization Okay...\33[0m\n",gProcessName);
   gLog.GenerateLog(GSYS03);

   timeval lTimeValue;
   double lStartTime = 0;

   signal(SIGALRM, MonitorChannelInstance);
   alarm(10);

   while (true == IvrUtil::KeepRunning())
   {
      gettimeofday (&lTimeValue, NULL);
      lStartTime =
         lTimeValue.tv_sec + (lTimeValue.tv_usec / 1000000.00);

      if (true == IvrUtil::ReloadConfig())
      {
         T(gTrace, printf("%s: Reloading config data again...\n",gProcessName);)
            ReloadConfig();
         IvrUtil::ResetConfigFlag();
      }

      if (true ==  ReadCallAlertFromQueue())
      {
         lChannelStatus = gIvrChannelApi.GetChannelStatus(gInChannelId);

         sprintf(lLogText, "%s Channel Status:%d",gLogText, lChannelStatus);
         T(gTrace, printf("%s: %s\n",gProcessName, lLogText);); 
         gLog.GenerateLog(SESN_IVR56, lLogText);

         //if(IVR_CHANNEL_UNDEFINED == lChannelStatus)
         if(IVR_CHANNEL_UNDEFINED == lChannelStatus || 0 == gInChannelId || 0 == gReferenceNum)
         {
            T(gTrace, printf("%s: %s\33[31m Call Received on Undefined CIC. Channel Status:%d\33[0m\n",
                     gProcessName, gLogText, lChannelStatus );); 
            gLog.GenerateLog(SESN_IVR70, gLogText);

            if(ISUP_MSG == gKernelMsg.kernelMsgType && GEN_ISUP_IAM == gKernelMsg.isupMsg.isupMsgType)
            {
               SendRel();
            }
            else if(EXT_REQ == gKernelMsg.kernelMsgType)
            {
               SendExtVxmlTerminateRsp();
            }
         }
         /*else if(((ISUP_MSG == gKernelMsg.kernelMsgType && GEN_ISUP_IAM == gKernelMsg.isupMsg.isupMsgType) 
                  || (EXT_REQ == gKernelMsg.kernelMsgType && EXT_PLAY == gKernelMsg.extRequest.extReqType)
                  || ((EXT_REQ == gKernelMsg.kernelMsgType && EXT_PATCH == gKernelMsg.extRequest.extReqType) 
                     && gCurrentState == ST_NO_STATE)
                  || (DIAL_OUT == gKernelMsg.kernelMsgType && true != gScheduleReq)|| (SCHEDULE_REQ == gKernelMsg.kernelMsgType))
               && ((IVR_CHANNEL_UNINITIALIZED == lChannelStatus) ||
                  (IVR_CHANNEL_UNINITIALIZED_ALLOCATED == lChannelStatus))) */
         else if(((ISUP_MSG == gKernelMsg.kernelMsgType && GEN_ISUP_IAM == gKernelMsg.isupMsg.isupMsgType) 
                  || (EXT_REQ == gKernelMsg.kernelMsgType && EXT_PLAY == gKernelMsg.extRequest.extReqType)
                  || ((EXT_REQ == gKernelMsg.kernelMsgType && EXT_PATCH == gKernelMsg.extRequest.extReqType) 
                     && gCurrentState == ST_NO_STATE)
                  || (SCHEDULE_REQ == gKernelMsg.kernelMsgType))
               && ((IVR_CHANNEL_UNINITIALIZED == lChannelStatus) ||
                  (IVR_CHANNEL_UNINITIALIZED_ALLOCATED == lChannelStatus)))
         //{
         //   INT32 lRetVal;
         {
            INT32 lRetVal;

            lRetVal = fork();

            if (lRetVal < 0)
            {
               T(gTrace, 
                     sprintf(gLogText, "%s: %s Fork failed.Sending REL...\n",gProcessName, gLogText);)
                  gLog.GenerateLog(SESN_IVR08, gLogText);
               //Send Release if fork is unsuccessful
               SendRel();
            }
            else if (lRetVal > 0)
            {
               // Parent Process
               gIntChildPid[gInChannelId] = lRetVal;
               SendCallAlertToChild();
               //gettimeofday (&lTimeValue, NULL);
               //printf ("%s: %06d: Processing Time: %f\n", gProcessName, gInChannelId,
               //		(lTimeValue.tv_sec +
               //		 (lTimeValue.tv_usec / 1000000.00)) - lStartTime);
            }
            else if (lRetVal == 0)
            {
               ProcessChildFunction(gInChannelId, argc, argv);
            }
         }
         else if(((ISUP_MSG == gKernelMsg.kernelMsgType && GEN_ISUP_IAM == gKernelMsg.isupMsg.isupMsgType) 
                  || (EXT_REQ == gKernelMsg.kernelMsgType && EXT_PLAY == gKernelMsg.extRequest.extReqType)
                  || ((EXT_REQ == gKernelMsg.kernelMsgType && EXT_PATCH == gKernelMsg.extRequest.extReqType) 
                     && gCurrentState == ST_NO_STATE))
               && ((IVR_CHANNEL_IN_CALL == lChannelStatus) || (IVR_CHANNEL_PLAYING == lChannelStatus) ||
                   (IVR_CHANNEL_RECORDING == lChannelStatus) || (IVR_CHANNEL_DTMF == lChannelStatus)))
         {
            // Clear the previously existing messages for a child
            ClearChildQueue();

            // Kill any existing child with that circuitId
            SendTermChild();

            T(gTrace, printf("%s: %s Initial Call Alert. Channel-In-Call\n", gProcessName, gLogText););
            //	gLog.GenerateLog(SESN_IVR09);
            SendCallAlertToChild();

         }
         else
         {
            if(GEN_ISUP_IAM == gKernelMsg.isupMsg.isupMsgType 
                  || EXT_PLAY == gKernelMsg.extRequest.extReqType 
                  || (EXT_PATCH == gKernelMsg.extRequest.extReqType && gCurrentState == ST_NO_STATE))
            {
               T(gTrace, printf("%s: %s Initial Call Alert\n",gProcessName, gLogText););
               //gLog.GenerateLog(SESN_IVR09);
            }
            else
            {
               T(gTrace, printf("%s: %s Not a Initial Call Alert\n",gProcessName, gLogText););
               //gLog.GenerateLog(SESN_IVR09);
            }

            SendCallAlertToChild();
         }
      } 
   } // forever while loop

   if(gConnectToRedis)
   {
      IvrRedisUtil::DisconnectRedis();
   }

   T(gTrace, printf("%s: Killing child processes (If any)...\n", gProcessName);)
      kill(0, SIGTERM);
   sleep(1);
   kill(0, SIGTERM);

   printf("Call Process shutting down...\n" );
   gLog.GenerateLog(GSYS02,(TEXT*) "Call Process shutting down...");
   return 0; 
}
