static const char* id="@(#) $Id: SNWChannelDisplay.cc,v 1.1.4.2 2016/04/12 21:59:40 shijuc Exp $" ;

//----------------------------------------------------------------------
// NAME
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// CLASS TYPE
//
// DESCRIPTION
//
// Originated :                                         DATE:13-JAN-2011
//----------------------------------------------------------------------
#include "SynwayVoiceMgrApi.h"
#include "SynwayUtil.h"

#define CHANNEL_IN_ONE_LINE  10
#define CHANNEL_PER_PAGE     100

Log gLog;
CTrace gTrace ("TRACE_SNW_CHANNEL_STATUS");
TEXT gProcessName[SYNWAY_MAX_PROCESS_NAME + 1] = "SNWChannelDisplay";

INT32                gChannelCount = MAX_SYNWAY_CHANNEL;
EnumChannelStatus    gStatus[MAX_SYNWAY_CHANNEL+1];
EnumSigChannelStatus gSigStatus[MAX_SYNWAY_CHANNEL+1];
UINT8                gPageNum=0;
UINT8                gTotalPages=1;
UINT16               gChannelStat[20];
UINT16               gSigChannelStat[20];


synwaySystem    gSynwaySystem;
synwaySystem    *gpSynwaySystem = NULL;
key_t           gChnlStShmKey;
int             gShmChnlMgmtId;
int             gChnlStSemKey;


BOOLEAN ReadConfig()
{

   CfgRead      lCfgReadObj(gProcessName);
   ConfigReturn lCfgRet;
   TEXT         lLogText[MAX_LOG_TEXT_LEN] = "";

   if (false == lCfgReadObj.CfgInit(SYNWAY_IPC_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n",
               gProcessName, lLogText););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }

   if(CFG_OK != ( lCfgRet = lCfgReadObj.GetConfigNum("SNW_CHNL_STATUS_SHM_KEY",
               gChnlStShmKey,
               SYNWAY_MIN_SHM_KEY,
               SYNWAY_MAX_SHM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Error in Reading Parameter %s : %s in %s",
            "SNW_CHNL_STATUS_SHM_KEY", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: SNW_CHNL_STATUS_SHM_KEY = %d\n", gProcessName, gChnlStShmKey););
   }

   if(CFG_OK != ( lCfgRet = lCfgReadObj.GetConfigNum("SNW_CHNL_STATUS_SEM_KEY",
               gChnlStSemKey,
               SYNWAY_MIN_SEM_KEY,
               SYNWAY_MAX_SEM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Error in Reading Parameter %s : %s in %s",
            "SNW_CHNL_STATUS_SEM_KEY", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: SNW_CHNL_STATUS_SEM_KEY = %d\n", gProcessName, gChnlStSemKey););
   }
   lCfgReadObj.CfgDeInit();

   return true;
}



//-------------------------------------------------------------
// FUNCTION    : InitChnlMgmt
// DESCRIPTION : Initializes Shared Memory
// PARAMETER   : NONE
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN InitChnlMgmt()
{
   UINT32           lSharedMemSize = 0;
   BOOLEAN          lShmCreated    = false;
   TEXT             lLogText[MAX_LOG_TEXT_LEN] = "";

   lSharedMemSize = sizeof(synwaySystem);

   gShmChnlMgmtId = shmget (gChnlStShmKey, lSharedMemSize, IPC_PERM);
   snprintf (lLogText,MAX_LOG_TEXT_LEN, "ShmChnlMgmtKey:%u Size:%u",
         gChnlStShmKey, lSharedMemSize);
   if (gShmChnlMgmtId < 0)
   {
      // Share memory Not present. Creating....
      gShmChnlMgmtId = shmget (gChnlStShmKey, lSharedMemSize, IPC_PERM | IPC_CREAT);
      if (gShmChnlMgmtId < 0)
      {
         TERR (gTrace, printf ("%s: %s Shared Memory creation Failed Err:%d ErrStr:%s\n",
                  gProcessName, lLogText, errno, strerror(errno)););
         return false;
      }

      lShmCreated = true;
      T (gTrace, printf ("%s: %s Shared Memory Creation Success\n", gProcessName, lLogText););
   }
   else
   {
      T (gTrace, printf ("%s: %s Shared Memory Already Present, Attaching\n", gProcessName, lLogText););
   }

   gpSynwaySystem = (synwaySystem*) shmat (gShmChnlMgmtId, (void *) 0, 0);
   if (NULL == gpSynwaySystem)
   {
      TERR (gTrace, printf ("%s: %s Error Attaching to Shared Memory Err:%d ErrStr:%s\n",
               gProcessName, lLogText, errno, strerror(errno)););
      return false;
   }
   T (gTrace, printf ("%s: %s Shared Memory Attached\n", gProcessName, lLogText););

   return true;
}



//-------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN Init ()
{
   if(false == ReadConfig())
      return false;

   if(false == InitChnlMgmt())
      return false;

   gPageNum = 0;
   gTotalPages = gChannelCount/CHANNEL_PER_PAGE;
   if(gChannelCount > (gTotalPages*CHANNEL_PER_PAGE))
      gTotalPages++;
   return true;
}


void *StatusDisplay(void *data)
{
   INT32 lStart = 1;
   INT32 lEnd   = 1;
   INT32 lLineEnd = 1;

   while(SynwayUtil::KeepRunning())
   {
      memset(&gSynwaySystem, 0, sizeof(synwaySystem));

      gSynwaySystem = *gpSynwaySystem;

      lStart = (gPageNum * CHANNEL_PER_PAGE)+1;
      lEnd   = (gPageNum * CHANNEL_PER_PAGE) + CHANNEL_PER_PAGE;
      if( lEnd > gChannelCount)
         lEnd = gChannelCount;

      memset(&gChannelStat, 0, sizeof(gChannelStat));
      memset(&gSigChannelStat, 0, sizeof(gSigChannelStat));

      for(int i=1; i<=gChannelCount; i++)
      {
         gStatus[i] = gSynwaySystem.channel[i-1].status;
         gSigStatus[i] = gSynwaySystem.sigChannel[i-1].status;
         gChannelStat[gStatus[i]]++;
         gSigChannelStat[gSigStatus[i]]++;
      }

      system("clear");
      printf("\33[47m\33[30m\33[1m");
      printf("------------------------------------------------------------------------------------------------------\n");
      printf("-   %04d-%04d/%04d                    \33[34mSNW SIGNAL CHANNEL STATUS\33[30m                %02d/%02d                 -\n",
            lStart, lEnd, gChannelCount, (gPageNum+1), gTotalPages);
      printf("------------------------------------------------------------------------------------------------------\n\33[0m");
      for(int i=lStart; i<= lEnd; i+=CHANNEL_IN_ONE_LINE)
      {
         printf("\33[47m\33[30m|\33[0m");
         lLineEnd = (i+CHANNEL_IN_ONE_LINE)-1;
         if(lLineEnd > lEnd)
            lLineEnd = lEnd;

         for(int j=i; j<= lLineEnd; j++)
            printf("\33[47m\33[30m\33[1m%04d %-13s\33[47m\33[30m ",
                  j, SynwayUtil::ConvertSigStatusToStr(gSigStatus[j]));
         for(int j=1; j<(CHANNEL_IN_ONE_LINE - (lLineEnd-i));j++)
            printf("\33[47m\33[30m\33[1m          \33[47m\33[0m");
         printf("\33[47m|\33[30m\n");
      }
      printf("\33[47m\33[30m------------------------------------------------------------------------------------------------------\33[0m\n");

      printf("\33[1;31m%-15s = %04d\33[0m\33[1;33m   %-18s = %04d\33[0m\33[1;35m   %-15s = %04d\33[0m\33[1;35m   %-15s = %04d\33[0m\n", 
            "NOT_AVAILABLE", gSigChannelStat[SNW_CHNL_NOT_AVAILABLE], 
            "CHNL_IDLE", gSigChannelStat[SNW_CHNL_IDLE], 
            "IAM_SENT", gSigChannelStat[SNW_IAM_SENT],
            "WAIT_DAIL_ANS", gSigChannelStat[SNW_WAIT_DAIL_ANS]);
      printf("\33[33m%-15s = %04d\33[0m\33[1;34m   %-18s = %04d\33[0m   \33[1;34m%-15s = %04d\33[0m   \33[1;32m%-15s = %04d\33[0m\n", 
            "DIALING", gSigChannelStat[SNW_DIALING], 
            "WAIT_REMOTE_PICKUP", gSigChannelStat[SNW_WAIT_REMOTE_PICKUP],
            "ANM_RCVD", gSigChannelStat[SNW_ANM_RCVD],
            "TALKING", gSigChannelStat[SNW_TALKING]);
      printf("\33[33m%-15s = %04d\33[0m\33[1;34m   %-18s = %04d\33[0m   \33[1;34m%-15s = %04d\33[0m   \33[1;36m%-15s = %04d\33[0m\n", 
            "IAM_RCVD", gSigChannelStat[SNW_IAM_RCVD],
            "WAIT_ACM_SND", gSigChannelStat[SNW_WAIT_ACM_SND], 
            "RINGING", gSigChannelStat[SNW_RINGING],
            "WAIT_RLC", gSigChannelStat[SNW_WAIT_RLC]);
      printf("\33[1;36m%-15s = %04d\33[0m\33[1;34m   %-18s = %04d\33[0m\n", 
            "REMOTE_HANGUP", gSigChannelStat[SNW_REMOTE_HANGUP],
            "WAIT_REL_SND", gSigChannelStat[SNW_WAIT_REL_SND]);

      printf("\33[47m\33[30m------------------------------------------------------------------------------------------------------\33[0m\n");
      printf("\33[1;31m%-4s => %-18s\33[0m\33[1;33m%-4s => %-22s\33[0m\33[1;35m%-4s => %-15s\33[0m\33[1;35m%-4s => %-18s\n",
            "N/A", "NOT_AVAILABLE", "IDLE", "CHNL_IDLE", "IAM ", "IAM_SENT", "IAMs", "WAIT_DAIL_ANS");
      printf("\33[1;33m%-4s => %-18s\33[0m\33[1;34m%-4s => %-22s\33[0m\33[1;34m%-4s => %-15s\33[0m\33[1;32m%-4s => %-18s\33[0m\n",
            "DIAL", "DIALING", "ACMr", "WAIT_REMOTE_PICKUP", "ANMr", "ANM_RCVD", "CON", "TALKING");
      printf("\33[1;33m%-4s => %-18s\33[0m\33[1;34m%-4s => %-22s\33[0m\33[1;34m%-4s => %-15s\33[0m\33[1;36m%-4s => %-18s\33[0m\n",
            "IAMr", "IAM_RCVD", "ACMw", "WAIT_ACM_SND", "RING", "RINGING", "RLCw", "WAIT_RLC");
      printf("\33[1;36m%-4s => %-18s\33[0m\33[1;34m%-4s => %-22s\33[0m\n",
            "RELr", "REMOTE_HANGUP", "RELw", "WAIT_REL_SND");
      printf("\33[47m\33[30m------------------------------------------------------------------------------------------------------\33[0m\n");



      //system("clear");
      printf("\33[47m\33[30m\33[1m");
      printf("------------------------------------------------------------------------------------------------------\n");
      printf("-   %04d-%04d/%04d                     \33[34mSNW MEDIA CHANNEL STATUS\33[30m                %02d/%02d                 -\n", 
            lStart, lEnd, gChannelCount, (gPageNum+1), gTotalPages);
      printf("------------------------------------------------------------------------------------------------------\n\33[0m");

      for(int i=lStart; i<= lEnd; i+=CHANNEL_IN_ONE_LINE)
      {
         printf("\33[47m\33[30m|\33[0m");
         lLineEnd = (i+CHANNEL_IN_ONE_LINE)-1;
         if(lLineEnd > lEnd)
            lLineEnd = lEnd;

         for(int j=i; j<= lLineEnd; j++)
            printf("\33[47m\33[30m\33[1m%04d %-13s\33[47m\33[30m ",
                  j, SynwayUtil::ConvertStatusToStr(gStatus[j]));
         for(int j=1; j<(CHANNEL_IN_ONE_LINE - (lLineEnd-i));j++)
            printf("\33[47m\33[30m\33[1m          \33[47m\33[0m");
         printf("\33[47m|\33[30m\n");

      }
      printf("\33[47m\33[30m------------------------------------------------------------------------------------------------------\33[0m\n");
      printf("\33[1;31m%-20s = %04d\33[0m\33[1;32m     %-25s = %04d\33[0m\33[1;35m     %-25s = %04d\33[0m\n", 
            "CKT_UNAVAILABLE", gChannelStat[CKT_UNAVAILABLE], 
            "CKT_IDLE", gChannelStat[CKT_IDLE], 
            "CKT_PLAYING", gChannelStat[CKT_PLAYING]);
      printf("\33[34m%-20s = %04d\33[0m\33[34m     %-25s = %04d\33[0m\33[1;33m     %-25s = %04d\33[0m\n", 
            "CKT_PLAY_STOPING", gChannelStat[CKT_PLAY_STOPING], 
            "CKT_RECORD_STOPING", gChannelStat[CKT_RECORD_STOPING], 
            "CKT_GETDIG", gChannelStat[CKT_GETDIG]);
      printf("\33[1;36m%-20s = %04d\33[0m\33[34m     %-25s = %04d\33[0m\33[34m     %-25s = %04d\33[0m\n", 
            "CKT_RECORDING", gChannelStat[CKT_RECORDING], 
            "CKT_PLAYING_AND_GETDIG", gChannelStat[CKT_PLAYING_AND_GETDIG], 
            "CKT_RECORDING_AND_GETDIG", gChannelStat[CKT_RECORDING_AND_GETDIG]);
      printf("\33[34m%s = %04d\33[0m\33[34m     %s = %04d\33[0m\n", 
            "CKT_PLAY_STOPING_AND_GETDIG", gChannelStat[CKT_PLAY_STOPING_AND_GETDIG], 
            "CKT_RECORD_STOPING_AND_GETDIG", gChannelStat[CKT_RECORD_STOPING_AND_GETDIG]);

      printf("\33[47m\33[30m------------------------------------------------------------------------------------------------------\33[0m\n");
      printf("\33[1;31m%-4s => %-30s\33[0m\33[1;32m%-4s => %-30s\33[0m\33[1;35m%-4s => %s\33[0m\n",
            "N/A", "CKT_UNAVAILABLE", "IDLE", "CKT_IDLE", "PLAY", "CKT_PLAYING");
      printf("\33[34m%-4s => %-30s\33[0m\33[34m%-4s => %-30s\33[0m\33[1;33m%-4s => %s\33[0m\n",
            "PLST", "CKT_PLAY_STOPING", "RCST", "CKT_RECORD_STOPING", "GDIG", "CKT_GETDIG");
      printf("\33[1;36m%-4s => %-30s\33[0m\33[34m%-4s => %-30s\33[0m\n",
            "RECD", "CKT_RECORDING", "PATC", "CKT_PATCHED");
      printf("\33[34m%-4s => %-30s\33[0m\33[34m%-4s => %-30s\33[0m\n",
            "RDIG", "CKT_RECORDING_AND_GETDIG", "PSGD", "CKT_PLAY_STOPING_AND_GETDIG");
      printf("\33[34m%-4s => %-30s\33[0m\33[34m%-4s => %s\33[0m\n",
            "RSGD", "CKT_RECORD_STOPING_AND_GETDIG", "PDIG", "CKT_PLAYING_AND_GETDIG");
      printf("\33[47m\33[30m------------------------------------------------------------------------------------------------------\33[0m\n");
      sleep(1);
   }

   return NULL;
}

//-------------------------------------------------------------
// METHOD      :
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
int main(int argc, char* argv[])
{
   ProcessLock  lProcessLock;
   pthread_t   lThreadId;
   INT32      lRetVal;
   TEXT        lChoice;


   if(argc>2)
   {
      printf("Usage : %s <ChannelCount>\n", argv[0]);
      printf("Trace : export %s=1\n", "TRACE_SNW_CHANNEL_STATUS");
      exit(0);
   }

   lProcessLock.SetProdProcName((TEXT*)"MP_NAME", gProcessName);

   if (LOCK_SUCCESS != lProcessLock.Lock())
   {
      printf("%s: Process already running. Exiting...\n",gProcessName);
      return false;
   }


   if(argc>=2)
   {
      gChannelCount = atoi(argv[1]);
   }
   if((gChannelCount > MAX_SYNWAY_CHANNEL) || (gChannelCount <=0))
   {
      gChannelCount = MAX_SYNWAY_CHANNEL;
   }

   if(false == Init())
      return false;

   lRetVal = pthread_create(&lThreadId, NULL, StatusDisplay, NULL);

   if (lRetVal == -1)
   {
      TERR(gTrace, printf ("%s: Failed to Create Status Display Thread. Exiting...\n", gProcessName););
      return -1;
   }

   while(SynwayUtil::KeepRunning())
   {
      cin>>lChoice;
      switch(lChoice)
      {
         case '+':
            if((gPageNum+1) < gTotalPages)
               gPageNum++;
            break;
         case '-':
            if(gPageNum > 0)
               gPageNum--;
            break;
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            if(lChoice-0x30 <= gTotalPages)
               gPageNum = lChoice-0x30-1;
            break;
         case 'q':
         case 'Q':
            return 0;
         default:
            break;
      }
   }


   shmdt(gpSynwaySystem);

   return 0;
}
