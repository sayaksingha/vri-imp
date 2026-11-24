static const char* id="@(#) $Id: SessionManager.cc,v 1.1.1.1.2.8.2.6 2018/02/26 23:17:34 mram Exp $" ;
//---------------------------------------------------------------
// NAME      SessionManager.cc
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
// Originated : Ramprasad M                 Date:10/11/2010
//--------------------------------------------------------------------------
#include "SessionManager.h"
#include "IvrQueApi.h"
#include "IvrChannelApi.h"
#include "IvrXmlFileApi.h"
#include "IvrDbManager.h"

#define INTERNAL_ROUTING 1

TEXT              gCallingNumber[MAX_MSISDN_LEN + 1];
TEXT              gCalledNumber[MAX_MSISDN_LEN + 1];
TEXT          	   gAppName[20 + 1] = "";
TEXT        	   gLogText[MAX_LOG_TEXT_LEN];
TEXT        	   glLogText[MAX_LOG_TEXT_LEN];
TEXT          	   gShortCode[20 +1] = "";
TEXT              gCountryCode[10 + 1]="";
UINT8             gAllowAllCountryCode=1;
TEXT              gDtmf[MAX_DTMF_LEN + 1]="";

INT16             gDuration;

UINT8             gRelCause;
UINT8             gRelLoc;

UINT16            gInChannelId;
UINT16            gPatchCircuitId;
UINT16            gPrimaryCircuitId;
UINT16            gNdcSnLen;
UINT16            gLandLineLen;

UINT32            gExtSessionId;
UINT32            gScheuldeId;
UINT32            gConnectTime;
UINT32            gAnswerTime;
UINT32            gDisconnectTime;
UINT32            gVxmlDelayTime;

BOOLEAN           gIsVxmlCallFlowInProgress = false;
BOOLEAN           gBillingInProgress=false;
BOOLEAN           gAllLogsReqd;
BOOLEAN		      gDiffRefNumFlag = false;
BOOLEAN           gJoinStatus = false;
BOOLEAN           gIsupRelease = false;
BOOLEAN           gScheduleRspFlg = false;
BOOLEAN           gIsRejectedCall = false;
BOOLEAN           gNormalizeFlg = false;

/* Added gConnectToRedis parameter for connecting to Redis incase few required SIP info Params are decoded by Sniffer sipgrep */
extern BOOLEAN           gConnectToRedis;
extern BOOLEAN           gIsRedisConnected;

_Variable         gVariable;

GenIsupMsg        gGenIsupMsg;

GenIsupIam        gInIam;
GenIsupAcm        gInAcm;
GenIsupAnm        gInAnm;
GenIsupRel        gInRel;
GenIsupRel        gOutRel;
ScheduleRsp       gScheduleRsp;

IvrEvrApi         gIvrEvrApi;
IvrEvrMsg         gIvrEvrMsg;
EnumIvrEvent      gIvrEvrEvent;
EnumIvrCause      gIvrEvrCause;
EnumIvrReason     gIvrEvrReason;

BillingApi        gBillingApi;
BillingRequest    gBillingRequest;
BillingResponse   gBillingResponse;

KernelMsg         gKernelMsg;
IvrQueApi         gIvrQueApi;
IvrDbManager	   gIvrDbObj;
IvrConfigApi      gIvrConfigApi;

extern EnumCallState      gCurrentState;
extern EnumCallEvent      gCurrentEvent;
extern MediaInterface	  gMediaInterface;
extern IvrChannelApi		  gIvrChannelApi;
extern unsigned int		  gReferenceNum;
extern INT32              gIntChildPid[];
extern UINT16             gMonitorInterval;
extern BOOLEAN            gScheduleReq;

//---------------------------------------------------------------
// METHOD      : Init()
// DESCRIPTION : Reads Config data and Creates Queues
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN  Init()
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN];

   if (false ==  gIvrQueApi.Init())
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Queue Api Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if(false == gIvrConfigApi.Init())
   {
      sprintf(lLogText, "ConfigApi Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if (DB_NO_ERR !=  gIvrDbObj.Init(gIvrConfigApi.mDbClient))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Ivr Db Object Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if(false == gIvrConfigApi.Init(gIvrDbObj, CONFIG_IVR_KERNEL))
   {
      sprintf(lLogText, "ConfigApi Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if (false == ReadConfigData())
   {    
      return false;
   }

   /*if (false == IvrUtil::ReadMappedCktNum(true))
     {
     return false;
     }
    */
   if (false == gIvrEvrApi.Init())
   {    
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Ivr EVR API Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if (false == gBillingApi.Init(IVR_SES_MNGR))
   {    
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Ivr Billing API Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadConfigData()
// DESCRIPTION : Reads Config Data
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//-------------------------------------------------------------
BOOLEAN ReadConfigData()
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN];

   if (false == ReadSs7Cfg())
   {   
      sprintf(lLogText,"SS7 Configuration reading failed. Exiting...");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if (false == ReadKernelCfg())
   {   
      sprintf(lLogText,"Kernel Configuration reading failed. Exiting...");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   return true;

}

//-------------------------------------------------------------
// METHOD      : ReadSs7Cfg()
// DESCRIPTION : Reads Config Data
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//--------------------------------------------------------------
BOOLEAN  ReadSs7Cfg() 
{
   //TEXT        lLogText[MAX_LOG_TEXT_LEN];
   return true;	 
}

//-------------------------------------------------------------
// METHOD      : ReadKernelCfg()
// DESCRIPTION : Reads Config Data from kernel.cfg
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//--------------------------------------------------------------
BOOLEAN  ReadKernelCfg() 
{
   CfgRead           lCfgRead(gProcessName);
   ConfigReturn      lCfgRet;
   TEXT              lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == lCfgRead.CfgInit("system.cfg") )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s",
            "system.cfg");
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, "system.cfg"););
      gLog.GenerateLog(GSYS10, lLogText);
      return false;
   }


   if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum(
               "IVR_NORMALIZATION_REQUIRED",
               gNormalizeFlg,
               0,
               1)))
   {
      snprintf( lLogText, MAX_LOG_TEXT_LEN,
            "Error in Reading Parameter %s \n",
            "IVR_NORMALIZATION_REQUIRED");
      TERR(gTrace, printf("%s: Config read failed for %s\n",
               gProcessName, "IVR_NORMALIZATION_REQUIRED");)
         gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: IVR_NORMALIZATION_REQUIRED = %d\n",
               gProcessName, gNormalizeFlg);)
   }

   if(1 == gNormalizeFlg)
   {
      if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigStr("IVR_COUNTRY_CODE",
                  gCountryCode,
                  1,
                  5)))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Error in Reading Parameter %s \n",
               " IVR_COUNTRY_CODE");
         TERR(gTrace, printf("%s: Config read failed for %s\n",
                  gProcessName, "IVR_COUNTRY_CODE"););
         gLog.GenerateLog(GSYS11, lLogText);
         lCfgRead.CfgDeInit();
         return false;
      }
      else
      {
         T(gTrace, printf("%s: IVR_COUNTRY_CODE = %s\n",
                  gProcessName, gCountryCode););
      }

      if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum(
                  "IVR_NDC_SN_LENGTH",
                  gNdcSnLen,
                  4,
                  20)))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Error in Reading Parameter %s \n",
               "IVR_NDC_SN_LENGTH");
         TERR(gTrace, printf("%s: Config read failed for %s\n",
                  gProcessName, "IVR_NDC_SN_LENGTH");)
            gLog.GenerateLog(GSYS11, lLogText);
         lCfgRead.CfgDeInit();
         return false;
      }
      else
      {
         T(gTrace, printf("%s: IVR_NDC_SN_LENGTH = %d\n",
                  gProcessName, gNdcSnLen);)
      }
      if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum(
                  "IVR_LANDLINE_NDC_SN_LENGTH",
                  gLandLineLen,
                  4,
                  20)))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Error in Reading Parameter %s \n",
               "IVR_LANDLINE_NDC_SN_LENGTH");
         TERR(gTrace, printf("%s: Config read failed for %s\n",
                  gProcessName, "IVR_LANDLINE_NDC_SN_LENGTH");)
            gLog.GenerateLog(GSYS11, lLogText);
         lCfgRead.CfgDeInit();
         return false;
      }
      else
      {
         T(gTrace, printf("%s: IVR_LANDLINE_NDC_SN_LENGTH = %d\n",
                  gProcessName, gLandLineLen);)
      }
      if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum(
                  "IVR_ALLOW_ALL_COUNTRY_CODE",
                  gAllowAllCountryCode,
                  0,
                  1)))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "Error in Reading Parameter %s \n",
               "IVR_ALLOW_ALL_COUNTRY_CODE");
         TERR(gTrace, printf("%s: Config read failed for %s, (Default : 1)\n",
                  gProcessName, "IVR_ALLOW_ALL_COUNTRY_CODE");)
            gLog.GenerateLog(GSYS11, lLogText);
         gAllowAllCountryCode = 1;
      }
      T(gTrace, printf("%s: IVR_ALLOW_ALL_COUNTRY_CODE = %d\n",
               gProcessName, gAllowAllCountryCode);)
   }

   lCfgRead.CfgDeInit();

   if (false == lCfgRead.CfgInit(IVR_KER_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s",
            IVR_KER_CFG);
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, IVR_KER_CFG););
      gLog.GenerateLog(GSYS10, lLogText);
      return false;
   }

   if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum(
               "IVR_VXML_REQ_DELAY_TIME",
               gVxmlDelayTime,
               0,
               60000000)))
   {
      snprintf( lLogText, MAX_LOG_TEXT_LEN,
            "Error in Reading Parameter %s \n",
            "IVR_VXML_REQ_DELAY_TIME");
      TERR(gTrace, printf("%s: Config read failed for %s\n",
               gProcessName, "IVR_VXML_REQ_DELAY_TIME");)
         gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: IVR_VXML_REQ_DELAY_TIME = %d\n",
               gProcessName, gVxmlDelayTime);)
   }

   if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum(
               "IVR_REDIS_CONNECTION_REQUIRED",
               gConnectToRedis,
               0,
               1)))
   {
      snprintf( lLogText, MAX_LOG_TEXT_LEN,
            "Error in Reading Parameter %s \n",
            "IVR_REDIS_CONNECTION_REQUIRED");
      TERR(gTrace, printf("%s: Config read failed for %s (Using Default : 0)\n",
               gProcessName, "IVR_REDIS_CONNECTION_REQUIRED");)
         gLog.GenerateLog(GSYS11, lLogText);
      gConnectToRedis = false;
   }
   else
   {
      T(gTrace, printf("%s: IVR_REDIS_CONNECTION_REQUIRED = %d\n",
               gProcessName, gConnectToRedis);)
   }

   lCfgRead.CfgDeInit();
   return true;
}

//-------------------------------------------------------------
// METHOD      : ReloadConfig()
// DESCRIPTION : Reads Config Data on SIGCFG
// PARAMETER   : no arguments
// RETURN      : void
// THROWS      : none
//--------------------------------------------------------------
void ReloadConfig()
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN];
   ConfigReturn lCfgRet;
   CfgRead     lCfgRead(gProcessName);

   if (false == lCfgRead.CfgInit(IVR_KER_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s",
            IVR_KER_CFG);
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, IVR_KER_CFG););
      gLog.GenerateLog(GSYS10, lLogText);
      //return false;
   }
   else
   {
      if ( CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum(
                  "IVR_VXML_REQ_DELAY_TIME",
                  gVxmlDelayTime,
                  0,
                  60000000)))
      {
         snprintf( lLogText, MAX_LOG_TEXT_LEN,
               "Error in Reading Parameter %s \n",
               "IVR_VXML_REQ_DELAY_TIME");
         TERR(gTrace, printf("%s: Config read failed for %s\n",
                  gProcessName, "IVR_VXML_REQ_DELAY_TIME");)
            gLog.GenerateLog(GSYS11, lLogText);
         //lCfgRead.CfgDeInit();
         //return false;
      }
      else
      {
         T(gTrace, printf("%s: IVR_VXML_REQ_DELAY_TIME = %d\n",
                  gProcessName, gVxmlDelayTime);)
      }

      lCfgRead.CfgDeInit();
   }

   gIvrChannelApi.ReloadConfig();

   return ;
}
//--------------------------------------------------------------
// METHOD         :     ReadCallAlertFromQueue()
// DESCRIPTION    :     Parent Reads Iam from validation queue
// PARAMETER      :     no arguments
// RETURN         :     BOOLEAN
// THROWS         :     none
//---------------------------------------------------------------
BOOLEAN ReadCallAlertFromQueue()
{
   //TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(&gLogText, 0, sizeof(gLogText));
   memset(&gKernelMsg, 0, sizeof(KernelMsg));

   if(true != gIvrQueApi.ReadFromCallAlertQue (gKernelMsg))
   {
      return false;
   }
   //Added By Komal
   gReferenceNum   = gKernelMsg.refNum;

   switch(gKernelMsg.kernelMsgType)
   {
      case ISUP_MSG:
         {
            gInChannelId = gKernelMsg.isupMsg.circuitId;
            gPrimaryCircuitId = gKernelMsg.isupMsg.circuitId;

            if(GEN_ISUP_IAM == gKernelMsg.isupMsg.isupMsgType)
            {
               gKernelMsg.refNum = gIvrChannelApi.AllocateNewReferenceNumber();
               gRelCause = 0x10;
               snprintf(gCallingNumber, MAX_MSISDN_LEN, "%s", gKernelMsg.isupMsg.isupIam.callingNumber);
               snprintf(gCalledNumber, MAX_MSISDN_LEN, "%s",  gKernelMsg.isupMsg.isupIam.calledNumber);
            }
            else
               gKernelMsg.refNum = gIvrChannelApi.GetReferneceNum(gInChannelId);
            gReferenceNum   = gKernelMsg.refNum;

            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CID:%06d Kmsg:%d Imsg:%d Clg:%s Cld:%s ISUP Call Alert",
                  gReferenceNum,
                  gKernelMsg.isupMsg.circuitId, 
                  gKernelMsg.kernelMsgType, 
                  gKernelMsg.isupMsg.isupMsgType, 
                  gCallingNumber,
                  gCalledNumber);
            //gKernelMsg.isupMsg.isupIam.callingNumber, 
            //gKernelMsg.isupMsg.isupIam.calledNumber);

            //T(gTrace, printf("%s: %s Call Alert:Isup Request\n", 
            T(gTrace, printf("%s: %s\n",gProcessName, gLogText);); 
            gLog.GenerateLog(SESN_IVR01, gLogText);
         }
         break;

      case EXT_REQ:
         {
            gInChannelId      = gKernelMsg.extRequest.inChannelId;
            gPatchCircuitId	=	gKernelMsg.extRequest.patchChannelId;
            gExtSessionId     =  gKernelMsg.extRequest.sessionId;
            gConnectTime = gAnswerTime = gDuration = gDisconnectTime = 0;

            if(EXT_PLAY == gKernelMsg.extRequest.extReqType)
               gKernelMsg.refNum = gIvrChannelApi.AllocateNewReferenceNumber();
            else
               gKernelMsg.refNum = gIvrChannelApi.GetReferneceNum(gInChannelId);
            gReferenceNum   = gKernelMsg.refNum;

            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X InCID:%06d PatchCID:%06d KernelMsgType:%d Ext-Session:%d Call Alert:External Request", 
                  gReferenceNum, 
                  gKernelMsg.extRequest.inChannelId,
                  gKernelMsg.extRequest.patchChannelId,
                  gKernelMsg.kernelMsgType,
                  gKernelMsg.extRequest.sessionId);

            T(gTrace,printf("%s: %s\n",  gProcessName, gLogText);); 
            gLog.GenerateLog(SESN_IVR02, gLogText);

         }
         break;

      case CCXML_RSP:
         {
            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CID:%06d CCRSP-KernelMsgType:%d Call Alert:Ccxml Response Received", gReferenceNum, 
                  gKernelMsg.ccxmlResponse.channelId,
                  gKernelMsg.kernelMsgType);

            T(gTrace,printf("%s: %s\n", gProcessName, gLogText);); 
            gLog.GenerateLog(SESN_IVR54, gLogText);
         }
         break;

      case VXML_RSP:
         {
            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CID:%06d VXRSP-KernelMsgType:%d Call Alert:Vxml Response Received", gKernelMsg.vxmlResponse.refNum, 
                  gKernelMsg.vxmlResponse.channelId,
                  gKernelMsg.kernelMsgType);

            gInChannelId = gKernelMsg.vxmlResponse.channelId;
            T(gTrace,printf("%s: %s\n", 
                     gProcessName, gLogText);); 
            gLog.GenerateLog(SESN_IVR55, gLogText); 
         }
         break;

         /*case SCHEDULE_REQ:
           {
           printf("\n\n TBR ############### Allocate New Ref Num\n\n");
           gKernelMsg.refNum = gIvrChannelApi.AllocateNewReferenceNumber();
           gReferenceNum   = gKernelMsg.refNum;
           gInChannelId = gIvrChannelApi.GetOutChannelId();

           snprintf(gCallingNumber, MAX_MSISDN_LEN, "%s", gKernelMsg.scheduleReq.origAddr);
           snprintf(gCalledNumber, MAX_MSISDN_LEN, "%s",  gKernelMsg.scheduleReq.destAddr);

           snprintf(gLogText,MAX_LOG_TEXT_LEN,
           "RefNum:%08X CID:%06d Kmsg:%d SchldId:%ld AppName:%s AppId:%s Clg:%s Cld:%s",
           gReferenceNum,
           gInChannelId,
           gKernelMsg.kernelMsgType,
           gKernelMsg.scheduleReq.scheduleId,
           gKernelMsg.scheduleReq.appName,
           gKernelMsg.scheduleReq.shortCode,
           gKernelMsg.scheduleReq.origAddr,
           gKernelMsg.scheduleReq.destAddr);

         //gScheduleReq = true;
         T(gTrace, printf("%s: %s Schedule Request\n",gProcessName, gLogText);); 
         gLog.GenerateLog(SESN_IVR57, gLogText);
         break;
         }
          */
      case SCHEDULE_REQ:
         {
            gKernelMsg.refNum = gIvrChannelApi.AllocateNewReferenceNumber();
            gReferenceNum   = gKernelMsg.refNum;
            gInChannelId = gIvrChannelApi.GetOutChannelId(gKernelMsg.scheduleReq.shortCode);
            gConnectTime = gAnswerTime = gDuration = gDisconnectTime = 0;

            snprintf(gCallingNumber, MAX_MSISDN_LEN, "%s", gKernelMsg.scheduleReq.origAddr);
            snprintf(gCalledNumber, MAX_MSISDN_LEN, "%s",  gKernelMsg.scheduleReq.destAddr);
            gPrimaryCircuitId = gInChannelId;

            gScheduleRsp.status = IVR_DELIVERED;
            gScheduleRsp.scheduleId = gKernelMsg.scheduleReq.scheduleId;
            strcpy(gScheduleRsp.origAddr, gKernelMsg.scheduleReq.origAddr);
            strcpy(gScheduleRsp.destAddr, gKernelMsg.scheduleReq.destAddr);
            strcpy(gScheduleRsp.shortCode, gKernelMsg.scheduleReq.shortCode);
            sprintf(gScheduleRsp.callRefNum, "%08X", gReferenceNum);

            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CID:%06d Kmsg:%d SchldId:%d AppName:%s Clg:%s Cld:%s Schedule Req",
                  gReferenceNum,
                  gInChannelId,
                  gKernelMsg.kernelMsgType,
                  gKernelMsg.scheduleReq.scheduleId,
                  gKernelMsg.scheduleReq.shortCode,
                  gKernelMsg.scheduleReq.origAddr,
                  gKernelMsg.scheduleReq.destAddr);
            //gKernelMsg.scheduleReq.appName,

            //if(-1 == gInChannelId)
            if(0 == gInChannelId)
            {
            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CID:%06d Kmsg:%d SchldId:%d AppName:%s Clg:%s Cld:%s Schedule Req, All Out Channels are Busy",
                  gReferenceNum,
                  gInChannelId,
                  gKernelMsg.kernelMsgType,
                  gKernelMsg.scheduleReq.scheduleId,
                  gKernelMsg.scheduleReq.shortCode,
                  gKernelMsg.scheduleReq.origAddr,
                  gKernelMsg.scheduleReq.destAddr);
               T(gTrace, printf("%s: %s\n",gProcessName, gLogText);); 
               gLog.GenerateLog(SESN_IVR69, gLogText);
            }
            //gInChannelId = gKernelMsg.isupMsg.circuitId;
            //T(gTrace, printf("%s: %s Call Alert:Isup Request\n", 
            T(gTrace, printf("%s: %s\n",gProcessName, gLogText);); 
            gLog.GenerateLog(SESN_IVR57, gLogText);
         }
         break;

      case DIAL_OUT:
         {
            gPrimaryCircuitId = gKernelMsg.ccxmlResponse.channelId;
            //printf("\n\n\n");
            //printf("******************  gKernelMsg.ccxmlResponse.channelId:%d  gInChannelId:%d\n", 
            //      gKernelMsg.ccxmlResponse.channelId, gInChannelId);
            ///printf("\n\n\n");

            //gKernelMsg.isupMsg.circuitId = gInChannelId;
            gInChannelId = gKernelMsg.ccxmlResponse.channelId;
            gKernelMsg.isupMsg.circuitId = gKernelMsg.ccxmlResponse.channelId;
            gKernelMsg.isupMsg.isupMsgType = GEN_ISUP_IAM;
            strcpy(gKernelMsg.isupMsg.isupIam.callingNumber,
                  gKernelMsg.ccxmlResponse.createcall.callerId);
            strcpy(gKernelMsg.isupMsg.isupIam.calledNumber,
                  gKernelMsg.ccxmlResponse.createcall.destAddr);

            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CID:%06d Kmsg:%d Imsg:%d Clg:%s Cld:%s Dial Out",
                  gReferenceNum,
                  gKernelMsg.isupMsg.circuitId,
                  gKernelMsg.kernelMsgType,
                  gKernelMsg.isupMsg.isupMsgType,
                  gKernelMsg.isupMsg.isupIam.callingNumber,
                  gKernelMsg.isupMsg.isupIam.calledNumber);

         }
         break;

      default:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Error: Call Alert: Unknown Message Received", gLogText);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText););
            gLog.GenerateLog(SESN_IVR03, glLogText);
            return false;
         }
   }

   return true;
}

//--------------------------------------------------------------
// METHOD         :     ClearChildQueue()
// DESCRIPTION    :     Parent clears the previously existing messages 
// PARAMETER      :     no arguments
// RETURN         :     void
// THROWS         :     none
//---------------------------------------------------------------
void ClearChildQueue()
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN];
   //KernelMsg	lKernelMsg;

   sprintf(lLogText, "RefNum:%08X Session-%d Clearing the Child Q", gReferenceNum, gInChannelId);
   strcat(gLogText, lLogText);
   T(gTrace,printf("%s: %s\n", gProcessName, gLogText);); 
   gLog.GenerateLog(SESN_IVR06, gLogText);

   gIvrQueApi.ClearEventChildQue (gInChannelId);
   return;
}

//--------------------------------------------------------------
// METHOD         :     SendCallAlertToChild()
// DESCRIPTION    :     Parent Writes Iam to Child 
// PARAMETER      :     no arguments
// RETURN         :     BOOLEAN
// THROWS         :     none
//---------------------------------------------------------------
BOOLEAN SendCallAlertToChild()
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN];
   T(gTrace, printf("%s: %s Sending Call Alert to Session-%d\n", 
            gProcessName, gLogText, gInChannelId);) 

   
      switch(gKernelMsg.kernelMsgType)
      {
         case ISUP_MSG:
            {
               /*snprintf(lLogText,MAX_LOG_TEXT_LEN,
                 "KernelMsgType:%d GenIsupMsgType:%d CircuitId:%d CallingParty:%s CalledParty:%s",
                 gKernelMsg.kernelMsgType, 
                 gKernelMsg.isupMsg.isupMsgType, 
                 gKernelMsg.isupMsg.circuitId, 
                 gKernelMsg.isupMsg.isupIam.callingNumber, 
                 gKernelMsg.isupMsg.isupIam.calledNumber);
                 */
               sprintf(lLogText, "%s Clearing the Child Q", gLogText);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText);); 
               gLog.GenerateLog(SESN_IVR04, lLogText);

            }
            break;

         case EXT_REQ:
            {
               /*snprintf(lLogText,MAX_LOG_TEXT_LEN,
                 "KernelMsgType:%d", gKernelMsg.kernelMsgType);
                */
               sprintf(lLogText, "%s Sending External Call Alert to Child", gLogText);
               T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
               gLog.GenerateLog(SESN_IVR05, lLogText);
            }
            break;

         case CCXML_RSP:
            {
               /*snprintf(lLogText,MAX_LOG_TEXT_LEN,
                 "KernelMsgType:%d", gKernelMsg.kernelMsgType);
                */
               sprintf(lLogText, "%s Sending Ccxml Response to Child", gLogText);
               T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
               gLog.GenerateLog(SESN_IVR02, lLogText); 
            }
            break;

         case VXML_RSP:
            {
               /*snprintf(lLogText,MAX_LOG_TEXT_LEN,
                 "KernelMsgType:%d", gKernelMsg.kernelMsgType);
                */
               sprintf(lLogText, "%s Sending Vxml Response to Child", gLogText);
               T(gTrace,printf("%s : %s\n", gProcessName, lLogText);); 
               gLog.GenerateLog(SESN_IVR02, lLogText);
            }
            break;

         case DIAL_OUT:
         case SCHEDULE_REQ:
            {
               /*snprintf(lLogText,MAX_LOG_TEXT_LEN,
                 "KernelMsgType:%d", gKernelMsg.kernelMsgType);
                */
               sprintf(lLogText, "%s Sending Dial Out Request to Child", gLogText);
               T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
               gLog.GenerateLog(SESN_IVR02, lLogText); 
            }
            break;


         default:
            {
               T(gTrace, printf("%s: RefNum:%08X Unknown Call Alert\n", gProcessName, gReferenceNum););
               return false;
            }
      }

   if(true != gIvrQueApi.WriteToEventChildQue 
         (gKernelMsg, gInChannelId))
   {
      return false;
   }

   return true;
}

//--------------------------------------------------------------
// METHOD         :     ReadMsgFromChildQ()
// DESCRIPTION    :     Child Reads Isup/Map/Vs msgs
// PARAMETER      :     no arguments
// RETURN         :     BOOLEAN
// THROWS         :     none
//---------------------------------------------------------------
BOOLEAN ReadMsgFromChildQ()
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN];

   if(true != gIvrQueApi.ReadFromEventChildQue 
         (gKernelMsg, gInChannelId))
   {
      return false;
   }
   // Added By Komal
   switch(gKernelMsg.kernelMsgType)
   {
      case ISUP_MSG:
         {
            gReferenceNum = gKernelMsg.refNum;
            if(GEN_ISUP_IAM == gKernelMsg.isupMsg.isupMsgType)
            {
               gConnectTime = gAnswerTime = gDuration = gDisconnectTime = 0;
               snprintf(gCallingNumber, MAX_MSISDN_LEN, "%s", gKernelMsg.isupMsg.isupIam.callingNumber);
               snprintf(gCalledNumber, MAX_MSISDN_LEN, "%s",  gKernelMsg.isupMsg.isupIam.calledNumber);
               //-----------------------------------------------------------------
               BOOLEAN lIsOrigCalledUpdated = false;

               if(true != NumberNormalize(gCallingNumber,gCalledNumber))
               {
                  SendRel();
                  gIsRejectedCall = false;
                  ExitChild();
                  return false;
               }

               if(gConnectToRedis)
               {
                  if(!gIsRedisConnected)
                  {
                     if(NULL == IvrRedisUtil::ConnectToIvrRedis()){
                        TERR(gTrace, printf("%s: Error Connecting to Redis Server on 127.0.0.1:6379\n", gProcessName););
                     }
                     else
                        gIsRedisConnected = true;
                  }
                  if(gIsRedisConnected)
                  {
                     char to[20]="", from[20]="", origcalled[20]="";
                     if(IvrRedisUtil::GetCallDetailsFromIvrRedis(gCallingNumber, to, from, origcalled))
                     {
                        T(gTrace, printf("%s: Call Details Retrieved from Redis for %s", gProcessName, gCallingNumber);
                              printf(" TO:%s,", to);
                              printf(" FROM:%s,", from);
                              printf(" ORIG_CALLED:%s\n", origcalled););

                        strncpy(gCalledNumber, origcalled, MAX_MSISDN_LEN);
                     }
                     else
                     {
                        T(gTrace, printf("%s: Failed to retrieve call details from Redis!!!\n", gProcessName););
                     }
                  }
               }
               //-----------------------------------------------------------------
               gIvrEvrEvent   = EVT_IVR_IN_CALL;
               gIvrEvrCause   = CAS_IVR_CALL_RECEIVED;
               gIvrEvrReason  = RSN_IVR_NONE;
               GenerateEvr();

               if(true != ProcessCallCharge(BILLING_CALL_RECEIVED))
               {
                  SendRel();
                  return false;
               }

            }
            else if(GEN_ISUP_ANM == gKernelMsg.isupMsg.isupMsgType)
            {
               gIvrEvrEvent   = EVT_IVR_OUT_CALL;
               gIvrEvrCause   = CAS_IVR_CALL_ANSWERED;
               gIvrEvrReason  = RSN_IVR_NONE;
               GenerateEvr();
               gAnswerTime = time(NULL);

               if(gScheduleRspFlg)
               {
                  gScheduleRsp.answerTime = time(NULL);
               }

               if(gBillingInProgress)
               {
                  if(true != ProcessCallCharge(BILLING_CALL_ANSEWERED))
                  {
                     return false;
                  }
               }
            }
            else if(GEN_ISUP_ACM == gKernelMsg.isupMsg.isupMsgType)
            {
               gConnectTime  = time(NULL); 
               if(gScheduleRspFlg)
               {
                  gScheduleRsp.connectTime = time(NULL);
               }
            }
            else if(GEN_ISUP_REL == gKernelMsg.isupMsg.isupMsgType)
            {
	       if( 0 == gDisconnectTime )
	       {
		       gDisconnectTime  = time(NULL); 
		       if(gScheduleRspFlg)
		       {
			       gScheduleRsp.disconnectTime = time(NULL);
		       }
		       gIvrEvrCause   = CAS_IVR_CALL_DISCONNECTED;
		       gIvrEvrReason  = RSN_IVR_NONE;
		       gDuration   =  (gAnswerTime? (gDisconnectTime - gAnswerTime): 0);

		       GenerateEvr();
	       }
	    }

	    snprintf(gLogText,MAX_LOG_TEXT_LEN,
			    "RefNum:%08X CID:%06d Kmsg:%d Imsg:%d Clg:%s Cld:%s",
			    gReferenceNum,
			    gKernelMsg.isupMsg.circuitId, 
			    gKernelMsg.kernelMsgType, 
			    gKernelMsg.isupMsg.isupMsgType, 
			    gCallingNumber,
			    gCalledNumber);
	    //gKernelMsg.isupMsg.isupIam.callingNumber, 
	    //gKernelMsg.isupMsg.isupIam.calledNumber);
	    //	snprintf(lLogText, MAX_LOG_TEXT_LEN, 
	    //				"KernelMsgType:%d GenIsupMsgType:%d CircuitId:%d CallingParty:%s CalledParty:%s",
	    //				gKernelMsg.kernelMsgType, 
	    //				gKernelMsg.isupMsg.isupMsgType, 
	    //				gKernelMsg.isupMsg.circuitId, 
	    //				gKernelMsg.isupMsg.isupIam.callingNumber, 
            //				gKernelMsg.isupMsg.isupIam.calldedNumber);
               sprintf(lLogText, "%s ISUP Msg Read from Child", gLogText);
            T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
            gLog.GenerateLog(SESN_IVR11, lLogText);

            //gInChannelId = gKernelMsg.isupMsg.circuitId;
         }
         break;

      case CCXML_RSP:
         {
            gInChannelId = gKernelMsg.ccxmlResponse.channelId;
            gPatchCircuitId = gKernelMsg.ccxmlResponse.join.id2;
            snprintf(gLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X KernelMsgType:%d", gKernelMsg.refNum, gKernelMsg.kernelMsgType);
               sprintf(lLogText, "%s CCXML Rsp Msg Read from Child", gLogText);
            T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
            gLog.GenerateLog(SESN_IVR12, lLogText);

         }
         break;

      case VXML_RSP:
         {
            if(gKernelMsg.refNum == 0)               //TBD -Added to avoid hanging of the call when disconnecting it
               gKernelMsg.refNum = gReferenceNum;    //in the Ringing state.

            snprintf(gLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X DTMF:%s KernelMsgType:%d", gKernelMsg.refNum, gKernelMsg.vxmlResponse.dtmf, gKernelMsg.kernelMsgType);
            strcpy(gDtmf, gKernelMsg.vxmlResponse.dtmf);
               sprintf(lLogText, "%s VXML Rsp Msg Read from Child", gLogText);
            T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
            gLog.GenerateLog(SESN_IVR13, lLogText);
         }
         break;

      case EXT_REQ:
         {
            gReferenceNum = gKernelMsg.refNum;
            gInChannelId = gKernelMsg.extRequest.inChannelId;
            gPatchCircuitId = gKernelMsg.extRequest.patchChannelId;
            snprintf(gLogText,MAX_LOG_TEXT_LEN,
                  "RefNum:%08X InCID:%06d PatchCID:%06d KernelMsgType:%d", 
                  gReferenceNum, 
                  gKernelMsg.extRequest.inChannelId, 
                  gKernelMsg.extRequest.patchChannelId,
                  gKernelMsg.kernelMsgType);

            //snprintf(lLogText, MAX_LOG_TEXT_LEN,
            //		" KernelMsgType:%d", gKernelMsg.kernelMsgType );
               sprintf(lLogText, "%s External Request Msg Read from Child", gLogText);
            T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
            gLog.GenerateLog(SESN_IVR14, lLogText);
         }
         break;

      case TERM_MSG:
         {
            gReferenceNum = gKernelMsg.refNum;
            snprintf(gLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X KernelMsgType:%d", gReferenceNum, gKernelMsg.kernelMsgType);
               sprintf(lLogText, "%s Terminate Msg Read from Child", gLogText);
            T(gTrace,printf("%s: %s\n", gProcessName, lLogText);); 
            gLog.GenerateLog(SESN_IVR15, lLogText);
         }
         break;

      case DIAL_OUT:
      case SCHEDULE_REQ:
         {
            memset(gDtmf, 0, sizeof(gDtmf));
            gReferenceNum = gKernelMsg.refNum;
            if(SCHEDULE_REQ != gKernelMsg.kernelMsgType)
            {
               snprintf(gLogText,MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CID:%06d Kmsg:%d Imsg:%d Clg:%s Cld:%s",
                     gReferenceNum,
                     gKernelMsg.isupMsg.circuitId,
                     gKernelMsg.kernelMsgType,
                     gKernelMsg.isupMsg.isupMsgType,
                     gKernelMsg.isupMsg.isupIam.callingNumber,
                     gKernelMsg.isupMsg.isupIam.calledNumber);
               gScheduleReq = false;
            }
            else
            {
               gScheduleRspFlg = true;  
               gScheduleReq = true;
               snprintf(gCallingNumber, MAX_MSISDN_LEN, "%s", gKernelMsg.scheduleReq.origAddr);
               snprintf(gCalledNumber, MAX_MSISDN_LEN, "%s",  gKernelMsg.scheduleReq.destAddr);
               snprintf(gLogText,MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CID:%06d Kmsg:%d SchldId:%d AppName:%s Clg:%s Cld:%s Var:%d",
                     gReferenceNum,
                     gInChannelId,
                     gKernelMsg.kernelMsgType,
                     gKernelMsg.scheduleReq.scheduleId,
                     gKernelMsg.scheduleReq.shortCode,
                     gKernelMsg.scheduleReq.origAddr,
                     gKernelMsg.scheduleReq.destAddr,
                     gKernelMsg.scheduleReq.varCount);
               //gKernelMsg.scheduleReq.appName,

               memset(&gVariable, 0, sizeof(_Variable));
               if(gKernelMsg.scheduleReq.varCount)
               {
                  gScheuldeId = gKernelMsg.scheduleReq.scheduleId;
                  gVariable.varCount = gKernelMsg.scheduleReq.varCount;
                  for(int lCnt=0; lCnt<gKernelMsg.scheduleReq.varCount; lCnt++)
                     strcpy(gVariable.var[lCnt], gKernelMsg.scheduleReq.var[lCnt]);
               }

               memset(&gScheduleRsp, 0, sizeof(ScheduleRsp));
               gScheduleRsp.status = IVR_DELIVERED;
               gScheduleRsp.scheduleId = gKernelMsg.scheduleReq.scheduleId;
               strcpy(gScheduleRsp.origAddr, gKernelMsg.scheduleReq.origAddr);
               strcpy(gScheduleRsp.destAddr, gKernelMsg.scheduleReq.destAddr);
               strcpy(gScheduleRsp.shortCode, gKernelMsg.scheduleReq.shortCode);
               sprintf(gScheduleRsp.callRefNum, "%08X", gReferenceNum);

               //gIvrEvrEvent   = EVT_IVR_OUT_CALL;
               //gIvrEvrCause   = CAS_IVR_CALL_DIALLED;
               //gIvrEvrReason  = RSN_IVR_NONE;
               //GenerateEvr();
            }

            //gInChannelId = gKernelMsg.isupMsg.circuitId;

            //T(gTrace, printf("%s: %s Call Alert:Isup Request\n",
            sprintf(lLogText, "%s Dial Out Read from Child", gLogText);
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SESN_IVR58, lLogText);
         }
         break;

      default:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s Unknown Msg Read From Child", gLogText); 
            T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
            gLog.GenerateLog(SESN_IVR16, gLogText);
            return false;
         }
   }

   if(gReferenceNum != gKernelMsg.refNum)
   {
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s Received Msg is not valid in Current Context", gLogText);
      TERR(gTrace, printf("%s: %s Received Msg is not valid in Current Context\n", gProcessName, gLogText););
      gLog.GenerateLog(SESN_IVR73, glLogText);
      return false;
   }
   return true;
}

//----------------------------------------------------------------
// METHOD		  :   GetEvent()
// DESCRIPTION	  :	Finds the event received from the kernel message
// PARAMETER     :	NONE
// RETURN		  :	EnumCallEvent 
// ---------------------------------------------------------------
EnumCallEvent GetEvent()
{
   snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Getting Event KernelMsgType :%d", 
         gLogText, gKernelMsg.kernelMsgType);
   T(gTrace, printf("%s: %s Getting Event KernelMsgType :%d\n", 
            gProcessName, gLogText, gKernelMsg.kernelMsgType);)
      gLog.GenerateLog(SESN_IVR18, LOG_LEVEL_3, glLogText);

   switch(gKernelMsg.kernelMsgType)
   {
      case ISUP_MSG:
         {
            return GetIsupEvent();
         }

      case CCXML_RSP:
         {
            return GetCcxmlEvent();
         }

      case VXML_RSP:
         {
            gIsVxmlCallFlowInProgress=false;
            return GetVxmlEvent();
         }

      case EXT_REQ:
         {
            return GetExtEvent();
         }

      case SCHEDULE_REQ:
         {
            gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_IN_CALL, gKernelMsg.refNum);
            return EVT_IAM;
         }
      case DIAL_OUT:
         {
            gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_IN_CALL, gKernelMsg.refNum);

            //   if(true != gScheduleReq)
            {
               return EVT_DIAL_OUT;
            }
            //  else
            // {
            //}
         }

      case TERM_MSG:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X Returning Terminate Child Event :%d", gReferenceNum, gKernelMsg.kernelMsgType);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR34, LOG_LEVEL_3, glLogText);
            return EVT_TERMINATE_CHILD;
         }

      default:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X Unknown Kernel Message, returning NULL Event: %d", 
                  gReferenceNum, gKernelMsg.kernelMsgType);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR35, LOG_LEVEL_2, gLogText);
            return EVT_NULL;
         }
   }
}
//----------------------------------------------------------------
// METHOD		  :   GetIsupEvent()
// DESCRIPTION	  :	Processes the Isup message
// PARAMETER     :	NONE
// RETURN		  :	BOOLEAN 
// ---------------------------------------------------------------
EnumCallEvent GetIsupEvent()
{
   T(gTrace, printf("%s: %s Got ISUP Event\n",	gProcessName, gLogText);)

      GenIsupMsgType  lMsgType; 
   lMsgType   = gKernelMsg.isupMsg.isupMsgType;

   switch (lMsgType)
   {
      case GEN_ISUP_IAM:
         {
            gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_IN_CALL, gKernelMsg.refNum);
            //gReferenceNum	= gKernelMsg.refNum;

            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got ISUP_IAM Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR19, glLogText);
            memcpy(&gInIam, &(gKernelMsg.isupMsg.isupIam), sizeof(GenIsupIam));
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_IAM_RCV);
            return EVT_IAM;
         }

      case GEN_ISUP_REL:
         {
            gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_REL_IN_PROGRESS, gKernelMsg.refNum);
            memcpy(&gInRel, &(gKernelMsg.isupMsg.isupRel), sizeof(GenIsupRel));
            gIsupRelease = true;
            gRelCause = gInRel.cause;
            snprintf(glLogText, MAX_LOG_TEXT_LEN, 
                  "%s Got GEN_ISUP_REL Event, Cause:%02X", gLogText, gRelCause);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR20, glLogText);
            //gRelLoc = gInRel.location;
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_REL_RCV);
            //if (gKernelMsg.isupMsg.circuitId == gInChannelId)
            {
               //Generate Peg for the event
               return EVT_REL; 
            }
         }

      case GEN_ISUP_ACM:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got GEN_ISUP_ACM Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR20, glLogText);
            memcpy(&gInAcm, &(gKernelMsg.isupMsg.isupAcm), sizeof(GenIsupAcm));
            //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_REL_RCV);
            return EVT_ACM; 
         }

      case GEN_ISUP_ANM:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got GEN_ISUP_ANM Event", gLogText);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR20, glLogText);
            memcpy(&gInAnm, &(gKernelMsg.isupMsg.isupAnm), sizeof(GenIsupAnm));
            //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_REL_RCV);
            return EVT_ANM; 
         }
      
      case GEN_ISUP_CPG:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got GEN_ISUP_CPG Event", gLogText);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR20, glLogText);
            //memcpy(&gInAnm, &(gKernelMsg.isupMsg.isupCpg), sizeof(GenIsupCpg));
            return EVT_CPG; 
         }


      default:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, 
                  "%s Got Unknow ISUP MsgType, Returning NULL Event", gLogText);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR21, glLogText);
            return EVT_NULL;
         }
   }//switch
}

//----------------------------------------------------------------
// METHOD		  :   GetCcxmlEvent()
// DESCRIPTION	  :	Processes the Isup message
// PARAMETER     :	NONE
// RETURN		  :	BOOLEAN 
// ---------------------------------------------------------------
EnumCallEvent GetCcxmlEvent()
{
   EnumCcxmlCommands  lMsgType; 
   lMsgType   = gKernelMsg.ccxmlResponse.ccxmlMsgType;

   switch (lMsgType)
   {
      case CCXML_ACCEPT:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Accept Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR22, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_ALT_ACK_RCV);
            return EVT_CCXML_ALT_ACK_RCV;
         }

      case CCXML_REJECT:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Reject Event", gLogText);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR23, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_ALT_NACK_RCV);
            return EVT_CCXML_ALT_NACK_RCV; 
         }

      case CCXML_DIALOGSTART:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Dialog Start Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR24, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DLG_STR_RCV);
            return EVT_CCXML_DLG_STR_RCV; 
         }

      case CCXML_DISCONNECT:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Disconnect Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR25, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DISCONN_RCV);
            return EVT_CCXML_DISCONN_RCV; 
         }

      case CCXML_EXIT:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Exit Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR26, glLogText);

            if(ST_CCXML_DLG_EXT_SND == gCurrentState)
            {
               gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_EXT_ON_SUC_RCV);
            }
            else
            {
               gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_EXT_RCV);
            }
            return EVT_CCXML_EXT_RCV; 
         }

      case CCXML_JOIN:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Join Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR62, glLogText);
            //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DISCONN_RCV);
            return EVT_CCXML_JOIN; 
         }

      case CCXML_UNJOIN:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Unjoin Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR66, glLogText);
            //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DISCONN_RCV);
            return EVT_CCXML_UNJOIN; 
         }

      default:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got CCXML Unknown Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR27, glLogText);
            return EVT_NULL;
         }
   }//switch
}

//----------------------------------------------------------------
// METHOD		  :   GetVxmlEvent()
// DESCRIPTION	  :	Processes the Isup message
// PARAMETER     :	NONE
// RETURN		  :	BOOLEAN 
// ---------------------------------------------------------------
EnumCallEvent GetVxmlEvent()
{
   int  lMsgType; 
   lMsgType   = gKernelMsg.vxmlResponse.result;

   switch (lMsgType)
   {
      case VXIplatform_RESULT_SUCCESS:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, 
                  "%s DTMF:%s Got VXML Success Event", gLogText, gKernelMsg.vxmlResponse.dtmf);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR28, glLogText);

            /*	if(ST_EXT_VXML_PLY_SND == gCurrentState)
               {
               gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_PLAY_RSP_RCV);
               }
               else if(ST_EXT_VXML_TERM_SND == gCurrentState)
               {
               gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_STOP_RSP_RCV);
               }
               else
               {
               gIvrQueApi.WriteToIvrPegQueue(PEG_SM_VXML_RSP_RCV);
               }
             */
            switch(gCurrentState)
            {
               case ST_EXT_VXML_PLY_SND:
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_PLAY_RSP_RCV);
                  break;

               case ST_EXT_VXML_TERM_SND:
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_PLAY_RSP_RCV);
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_STOP_RSP_RCV);
                  break;

               default:
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_VXML_RSP_RCV);
            }

            return EVT_VXML_RSP_RCV;
         }
         break;

      default:
         {
            switch(gCurrentState)
            {
               case ST_EXT_VXML_PLY_SND:
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_DLG_EXT_FAIL);
                  break;

               case ST_EXT_VXML_TERM_SND:
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_PLAY_RSP_RCV);
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_STOP_RSP_FAIL);
                  break;

               default:
                  gIvrQueApi.WriteToIvrPegQueue(PEG_SM_VXML_RSP_RCV);
            }
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got VXML Unknown Failure Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR29, glLogText);
            // Komal: Modified to return EVT_VXML_RSP_RCV instead of EVT_NULL, 
            //  as Success or failure both are VXML responses only
            //return EVT_NULL;
            return EVT_VXML_RSP_RCV;
         }
   }//switch
}

//----------------------------------------------------------------
// METHOD		  :   GetExtEvent()
// DESCRIPTION	  :	Processes the Isup message
// PARAMETER     :	NONE
// RETURN		  :	BOOLEAN 
// ---------------------------------------------------------------
EnumCallEvent GetExtEvent()
{
   int  lMsgType; 
   lMsgType   = gKernelMsg.extRequest.extReqType;

   switch (lMsgType)
   {
      case EXT_PLAY:
         {
            gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_IN_CALL, gKernelMsg.refNum);
            //gReferenceNum	= gKernelMsg.refNum; //gIvrChannelApi.GetReferneceNum(gInChannelId);

            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got External Play Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR30, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_PLY_RCV);
            return EVT_EXT_PLY_RCV;
         }
         break;

      case EXT_STOP:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got External Stop Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR31, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_STP_RCV);
            return EVT_EXT_STP_RCV;
         }
         break;

      case EXT_PATCH:
         {
            gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_CMD_IN_PROGRESS, gKernelMsg.refNum);
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got External Patch Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR32, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_PTH_RCV);
            return EVT_EXT_PTH_RCV;
         }
         break;

      case EXT_TEAR:
         {
            gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_CMD_IN_PROGRESS, gKernelMsg.refNum);
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got External Tear Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR70, glLogText);
            gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_TEAR_RCV);
            return EVT_EXT_TEAR_RCV;
         }
         break;

      default:
         {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Got External Unknown Event", gLogText);
            T(gTrace, printf("%s: %s\n",	gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR33, glLogText);
            return EVT_NULL;
         }
   }//switch
}

//--------------------------------------------------------------
// METHOD      : SendAcm()
// DESCRIPTION : Writes ACM to ISUP Encoder
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendAcm()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   gGenIsupMsg.isupMsgType       = GEN_ISUP_ACM;
   gGenIsupMsg.circuitId         = gInChannelId;
   gGenIsupMsg.isupAcm.cpc       = gInAcm.cpc;
   gGenIsupMsg.isupAcm.eventInfo = gInAcm.eventInfo;
   gGenIsupMsg.isupAcm.optLen    = gInAcm.optLen; 
   memcpy(gGenIsupMsg.isupAcm.opt, gInAcm.opt, gInAcm.optLen);

   sprintf(lLogText, "RefNum:%08X CID:%06d GenIsupMsgType:%d",  
         gReferenceNum, 
         gGenIsupMsg.circuitId, 
         gGenIsupMsg.isupMsgType);

   T(gTrace, printf("%s: %s Sending ACM to ISUP\n", gProcessName, lLogText );)
      gLog.GenerateLog(SESN_IVR37, lLogText);

   if(true != gIvrQueApi.WriteToIsupEncQue (gGenIsupMsg))
   {
      return false;
   }

   gConnectTime  = time(NULL); 
   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_ACM_SEND);
   sleep(gIvrConfigApi.mRingDuration);
   gCurrentState = ST_ACM_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : ValidateCall()
// DESCRIPTION : Validate incomin or out going call
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN ValidateCall(CcxmlRequest &ccxmlReq, INT32 rRoutedCall)
{
   TEXT          	lLogText[MAX_LOG_TEXT_LEN];
   Application    lApplication;
   INT16          lCallType = gScheduleReq;

   memset(&lApplication, 0, sizeof(Application));


   if(DB_NO_ERR !=  gIvrDbObj.GetAppName(gShortCode, lCallType, lApplication))
   {
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Error: Getting App Name from DB Failed AppName:%s", gLogText, gShortCode);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
      gLog.GenerateLog(SESN_IVR74, glLogText);
      gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
      gIvrEvrReason  = RSN_IVR_CALL_INVALID_DEST_NUMBER;
      gRelCause = 0x15; //Call  Rejected
      GenerateEvr();
      return false;
   }
   else
   {
      sprintf(lLogText, "SC:%s AppName:%s AppId:%d AppType:%d AppStatus:%d ScStatus:%d ClgNum:%s TestNum:%s",
            gShortCode, lApplication.appName, lApplication.appId, lApplication.appType, 
            lApplication.status, lApplication.scStatus, gCallingNumber, lApplication.testCli);
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s %s Getting App Name from DB Success", gLogText,lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText););
      gLog.GenerateLog(SESN_IVR75, glLogText);
      strcpy(gAppName, lApplication.appName);
      strcpy(lApplication.shortCode, gShortCode);
      lApplication.channel = gInChannelId;
   }

   if((APP_UPLOADED == lApplication.status || APP_TESTED == lApplication.status)
         && (0 != strcmp(gCallingNumber, lApplication.testCli)))
   {
      T(gTrace, printf("%s: %s Calling Number is Not a Test Number %s\n", 
               gProcessName, gLogText, lLogText);); 
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s Calling Number is Not a Test Number %s", gLogText, lLogText);
      gLog.GenerateLog(SESN_IVR76, glLogText);
      gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
      gIvrEvrReason  = RSN_IVR_CALL_APP_NOT_ACTIVE;
      gRelCause = 0x12; // No User Responding;
      GenerateEvr();
      return false;
   }
   else if(APP_BLOCKED == lApplication.status || APP_DELETED == lApplication.status || APP_REJECTED == lApplication.status)
   {
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Error: Package Blocked/Deleted/Rejected %s", gLogText, lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
         gLog.GenerateLog(SESN_IVR77, glLogText);
      gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
      gIvrEvrReason  = RSN_IVR_CALL_APP_NOT_ACTIVE;
      gRelCause = 0x1b; // Destination Out /of Order;
      GenerateEvr();
      return false;
   }
   else if(0 != lApplication.scStatus) //0 -- AVAILABLE, 1-- BLOCKED
   {
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Error: ShortCode Not Active %s", gLogText, lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
         gLog.GenerateLog(SESN_IVR78, glLogText);
      gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
      gIvrEvrReason  = RSN_IVR_CALL_SC_NOT_ACTIVE;
      gRelCause = 0x1b; // Destination Out /of Order;
      GenerateEvr();
      return false;
   }

#if INTERNAL_ROUTING
   TEXT *lpCli, *lpRoutedShortCode;
   lpCli = strtok(lApplication.testCli, "@");
   if (lpCli != NULL)
   {
      lpRoutedShortCode = strtok(NULL,"@");
      if((lpRoutedShortCode != NULL ) && (0 == strcmp(gCallingNumber, lApplication.testCli)))
      {
         if(rRoutedCall == 0)
         {
            sprintf(lLogText, "%s Internal Routing From SC:%s to SC:%s",
                  gLogText, gShortCode, lpRoutedShortCode);
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SESN_IVR79, lLogText);
            strcpy(gShortCode, lpRoutedShortCode);
            return ValidateCall(ccxmlReq, (rRoutedCall+1));
         }
         else
         {
               snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Error: Multiple Routing Not Allowed %s", gLogText, lLogText);
            T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
               gLog.GenerateLog(SESN_IVR80, glLogText);
            gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
            gIvrEvrReason  = RSN_IVR_CALL_MULTPLE_ROUTE;
            gRelCause = 0x15; // Call Rejected
            GenerateEvr();
            return false;
         }
      }
   }
#endif

   if(APP_INCOMING == lApplication.appType)
   {
      sprintf(ccxmlReq.ccxmlUrlRequest.url, "%s/xml/main_ic.ccxml", gAppName);
   }
   else if(APP_OUTGOING == lApplication.appType)
   {
      sprintf(ccxmlReq.ccxmlUrlRequest.url, "%s/xml/main_og.ccxml", gAppName);
   }
   else
   {
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Error: Unknown App Type %s", gLogText, lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         gLog.GenerateLog(SESN_IVR81, glLogText);
      gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
      gIvrEvrReason  = RSN_IVR_CALL_APP_NOT_ACTIVE;
      gRelCause = 0x1b; // Destination Out /of Order;
      GenerateEvr();
      return false;
   }

   TEXT     lShortCode[20 + 1] = "";
   if(DB_NO_ERR !=  gIvrDbObj.CheckChannelNo(lApplication))
   {
      sprintf(lLogText, "RefNum:%08X CID:%06d SC:%s DLD-SC:%s", 
            gReferenceNum, gInChannelId, lShortCode, gShortCode);
      T(gTrace, printf("%s: %s Short Code Match Failed\n", 
               gProcessName, lLogText);)
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
         gLog.GenerateLog(SESN_IVR82, lLogText);
      gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
      gIvrEvrReason  = RSN_IVR_SCH_REQ_CHANNEL_UNAVAILABLE;
      gRelCause = 0x09; // Circuit Reserved For reuse
      GenerateEvr();
      return false;
   }
   else
   {
      sprintf(lLogText, "RefNum:%08X CID:%06d SC:%s", gReferenceNum, gInChannelId, lShortCode);
      T(gTrace, printf("%s: %s Short Code Configured for Channel\n", 
               gProcessName, lLogText);) 
         gLog.GenerateLog(SESN_IVR83, gLogText);
   }
   return true;
}
//--------------------------------------------------------------
// METHOD      : SendCcxmlCallAlert()
// DESCRIPTION : Writes Call Alert to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendCcxmlCallAlert()
{
   TEXT          	lLogText[MAX_LOG_TEXT_LEN];
   CcxmlRequest  	lCcxmlReq;
   memset(&lCcxmlReq, 0, sizeof(CcxmlRequest));

   if(SCHEDULE_REQ != gKernelMsg.kernelMsgType)
   {
      strcpy(gShortCode, gKernelMsg.isupMsg.isupIam.calledNumber);
      //strcpy(gShortCode, "8809876543210");  //TBR for testing
   }
   else
   {
      strcpy(gShortCode, gKernelMsg.scheduleReq.shortCode);
   }

   if((SCHEDULE_REQ == gKernelMsg.kernelMsgType) && (IVR_OBD == gKernelMsg.scheduleReq.type))
   {   
      gIvrEvrEvent   = EVT_IVR_OUT_CALL;
      strcpy(gAppName, gKernelMsg.scheduleReq.appName);
      sprintf(lCcxmlReq.ccxmlUrlRequest.url, "%s/xml/main_og.ccxml", gAppName);
   }
   else
   {
      if(true != ValidateCall(lCcxmlReq))
      {
         gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_FREE);
         gIsRejectedCall = true;
         SendRel();
         gIsRejectedCall = false;
         gScheduleReq = false;
         gCurrentState = ST_NO_STATE;
         return false;
      }
   }

   gIvrChannelApi.SetChannelDetails(gInChannelId, 
         (EVT_IVR_OUT_CALL==gIvrEvrEvent?IVR_CHANNEL_DIR_OUT:IVR_CHANNEL_DIR_IN),
         gCallingNumber, 
         gCalledNumber, 
         gAppName,
         gKernelMsg.refNum);

   if(SCHEDULE_REQ == gKernelMsg.kernelMsgType)
   {
      strcpy(lCcxmlReq.ccxmlUrlRequest.var1, gKernelMsg.scheduleReq.origAddr);
      strcpy(lCcxmlReq.ccxmlUrlRequest.var2, gKernelMsg.scheduleReq.destAddr);
      lCcxmlReq.ccxmlUrlRequest.varCount = 2;
   }
   lCcxmlReq.channelId = gInChannelId;
   lCcxmlReq.sessionId = gInChannelId;
   lCcxmlReq.refNum 	  = gReferenceNum;

   sprintf(lLogText, "RefNum:%08X CID:%06d Url:%s SessionId:%d OrigAddr:%s", 
         gReferenceNum, lCcxmlReq.channelId, lCcxmlReq.ccxmlUrlRequest.url,
         lCcxmlReq.sessionId, gKernelMsg.isupMsg.isupIam.callingNumber);

   T(gTrace, printf("%s: %s Sending CCXML Request\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR38, lLogText);

   if(true != gIvrQueApi.WriteToCcxmlReqQue (lCcxmlReq))
   {
      return false;
   }

   memset(&lCcxmlReq.ccxmlEventRequest, 0, sizeof(CcxmlEventRequest));
   if(SCHEDULE_REQ != gKernelMsg.kernelMsgType)
   {
      strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, gKernelMsg.isupMsg.isupIam.callingNumber);
      strcpy(lCcxmlReq.ccxmlEventRequest.destAddr, gKernelMsg.isupMsg.isupIam.calledNumber);

      gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_ALT_SND);
      gCurrentState = ST_CCXML_ALT_SND;

      strcpy(lCcxmlReq.ccxmlEventRequest.event,
            IvrUtil::ConvertCcxmlEventToString (EVT_CC_ALERTING));

      sprintf(glLogText, "%s Sending CCXML Alerting Event", lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
         gLog.GenerateLog(SESN_IVR39, glLogText);

      if(true != gIvrQueApi.WriteToCcxmlEventQue (lCcxmlReq))
      {
         return false;
      }
   }
   else
   {
      strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, gKernelMsg.scheduleReq.origAddr);
      strcpy(lCcxmlReq.ccxmlEventRequest.destAddr, gKernelMsg.scheduleReq.destAddr);
      //gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_FREE);

      gCurrentState = ST_NO_STATE;
      //gCurrentState = ST_CCXML_ALT_SND;
      //gScheduleReq = false;
   }

   return true;
}

//--------------------------------------------------------------
// METHOD      : SendCcxmlCallConnected()
// DESCRIPTION : Writes Call Alert to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendCcxmlCallConnected()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   CcxmlRequest  lCcxmlReq;
   memset(&lCcxmlReq, 0, sizeof(CcxmlRequest));

   snprintf(lCcxmlReq.ccxmlEventRequest.origAddr,MAX_MSISDN_LEN, "%s", gCallingNumber);
   snprintf(lCcxmlReq.ccxmlEventRequest.destAddr,MAX_MSISDN_LEN, "%s", gCalledNumber);
   //strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, "1234567890");
   lCcxmlReq.channelId = gInChannelId;
   lCcxmlReq.sessionId = gInChannelId;
   lCcxmlReq.refNum 	  = gReferenceNum;

   strcpy(lCcxmlReq.ccxmlEventRequest.event, 
         IvrUtil::ConvertCcxmlEventToString (EVT_CC_CONNECTED));

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%s SessionId:%d OrigAddr:%s Sending CCXML Connected Event", 
         gReferenceNum, lCcxmlReq.channelId, lCcxmlReq.ccxmlEventRequest.event,
         lCcxmlReq.sessionId, lCcxmlReq.ccxmlEventRequest.origAddr);

   T(gTrace, printf("%s: %s\n", gProcessName,  lLogText);); 
   gLog.GenerateLog(SESN_IVR40, lLogText);

   if(true != gIvrQueApi.WriteToCcxmlEventQue (lCcxmlReq))
   {
      return false;
   }


   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_CONN_SND);
   gCurrentState = ST_CCXML_CONN_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendCcxmlDlgExit()
// DESCRIPTION : Writes Call Alert to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendCcxmlDlgExit()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   CcxmlRequest  lCcxmlReq;

   memset(&lCcxmlReq, 0, sizeof(CcxmlRequest));

   //T(gTrace, printf("%s: %s Sending CCXML Dialog Exit Event.111\n", gProcessName, lLogText););
   snprintf(lCcxmlReq.ccxmlEventRequest.origAddr,MAX_MSISDN_LEN, "%s", gCallingNumber);
   snprintf(lCcxmlReq.ccxmlEventRequest.destAddr,MAX_MSISDN_LEN, "%s", gCalledNumber);
   //strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, "1234567890"); //TBD
   lCcxmlReq.channelId = gInChannelId;
   lCcxmlReq.sessionId = gInChannelId;
   lCcxmlReq.refNum 	  = gReferenceNum;

   //T(gTrace, printf("%s: %s Sending CCXML Dialog Exit Event.222\n", gProcessName, lLogText););
   strcpy(lCcxmlReq.ccxmlEventRequest.event, 
         IvrUtil::ConvertCcxmlEventToString (EVT_CC_DLG_EXIT));
   strcpy(lCcxmlReq.ccxmlEventRequest.dialogexit.keyvalue.key[0], "dtmf");
   strcpy(lCcxmlReq.ccxmlEventRequest.dialogexit.keyvalue.value[0], 
         gKernelMsg.vxmlResponse.dtmf);

   //T(gTrace, printf("%s: %s Sending CCXML Dialog Exit Event.333\n", gProcessName, lLogText););
   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%s SessionId:%d OrigAddr:%s Sending CCXML Dialog Exit Event", 
         gReferenceNum, lCcxmlReq.channelId, lCcxmlReq.ccxmlEventRequest.event,
         lCcxmlReq.sessionId, lCcxmlReq.ccxmlEventRequest.origAddr);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SESN_IVR41, lLogText);

   if(true != gIvrQueApi.WriteToCcxmlEventQue (lCcxmlReq))
   {
      return false;
   }

   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DLG_EXT_SND);
   gCurrentState = ST_CCXML_DLG_EXT_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendExternalDlgExit()
// DESCRIPTION : Writes Call Alert to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendExternalDlgExit()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   ExtResponse   lExtResponse;
   gDiffRefNumFlag = false;

   memset(&lExtResponse, 0, sizeof(ExtResponse));
   sprintf(lLogText, 
         "RefNum:%08X CID:%06d SessionId:%d Rsp-RefNum:%08X DTMF:%s Ext-Session:%d", 
         gReferenceNum, gInChannelId, gInChannelId, gKernelMsg.vxmlResponse.refNum, 
         gKernelMsg.vxmlResponse.dtmf, gExtSessionId);

   if(gKernelMsg.vxmlResponse.refNum == gReferenceNum)
   {
      lExtResponse.extReqType		= EXT_PLAY;
      lExtResponse.result			= 1;
      lExtResponse.inChannelId 	= gInChannelId;
      lExtResponse.sessionId 		= gExtSessionId;
      snprintf(lExtResponse.dtmf, 50, "%s", gKernelMsg.vxmlResponse.dtmf);

      sprintf(glLogText, "%s Sending External VXML Dialog Exit Event", lLogText);
      T(gTrace, 
            printf("%s: %s\n", gProcessName, glLogText);) 
         gLog.GenerateLog(SESN_IVR42, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_DLG_EXT_SND);

      if(true != gIvrQueApi.WriteToExtVxmlRspQue (lExtResponse))
      {
         return false;
      }
   }
   else
   {
      gDiffRefNumFlag = true;
      sprintf(glLogText, "%s Response Got for different Reference Number DiffRefNumFlag:%d", 
            lLogText, gDiffRefNumFlag);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         gLog.GenerateLog(SESN_IVR42, glLogText);
         gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_PLY_SND);
      gCurrentState = ST_EXT_VXML_PLY_SND;
   }

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendCcxmlDisconnect()
// DESCRIPTION : Writes Call Alert to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendCcxmlDisconnect()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   CcxmlRequest  lCcxmlReq;

   memset(&lCcxmlReq, 0, sizeof(CcxmlRequest));
   lCcxmlReq.refNum 	  = gReferenceNum;

   snprintf(lCcxmlReq.ccxmlEventRequest.origAddr,MAX_MSISDN_LEN, "%s", gCallingNumber);
   snprintf(lCcxmlReq.ccxmlEventRequest.destAddr,MAX_MSISDN_LEN, "%s", gCalledNumber);
   //strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, "1234567890"); //TBD
   lCcxmlReq.channelId = gPrimaryCircuitId;
   lCcxmlReq.sessionId = gInChannelId;

   strcpy(lCcxmlReq.ccxmlEventRequest.event, 
         IvrUtil::ConvertCcxmlEventToString (EVT_CC_DISCONNECTED));

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%s SessionId:%d OrigAddr:%s Sending CCXML Disconnect Event", 
         gReferenceNum, lCcxmlReq.channelId, lCcxmlReq.ccxmlEventRequest.event,
         lCcxmlReq.sessionId, lCcxmlReq.ccxmlEventRequest.origAddr);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR43, lLogText);

   if(true != gIvrQueApi.WriteToCcxmlEventQue (lCcxmlReq))
   {
      return false;
   }

   if(gBillingInProgress)
   {
      if(true != ProcessCallCharge(BILLING_CALL_DISCONNECTED))
      {
         return false;
      }
   }

   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DISCO_SND);
   gCurrentState = ST_CCXML_DISCO_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendVxmlRequest()
// DESCRIPTION : Writes Vxml Requesr to VXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendVxmlRequest()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   VxmlRequest   lVxmlReq;
   memset(&lVxmlReq, 0, sizeof(VxmlRequest));

   lVxmlReq.vxmlEvent = EVT_VX_URL;
   lVxmlReq.channelId = gInChannelId;
   lVxmlReq.refNum =		gReferenceNum;

   usleep(gVxmlDelayTime); 

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%d",
         gReferenceNum, lVxmlReq.channelId, lVxmlReq.vxmlEvent); 

   if(gScheduleRspFlg)
   {
      lVxmlReq.scheduleId = gScheuldeId;
      lVxmlReq.varCount = gVariable.varCount;
      sprintf(glLogText, "%s Number of Variables in Schedule Request:%d", lLogText, gVariable.varCount);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 

         for(int i = 0; i < lVxmlReq.varCount; i++)
         {
            strcpy(lVxmlReq.var[i], gVariable.var[i]);
            T(gTrace, printf("%s: %s Variable Name in Schedule Request:%s\n", 
                     gProcessName, lLogText, lVxmlReq.var[i]);) 
         }
   }

   if(CCXML_RSP == gKernelMsg.kernelMsgType)
   {
      if('\0' != gKernelMsg.ccxmlResponse.dlgStart.dlgSrc[0])
      {
         strcpy(lVxmlReq.vxmlUrl, gKernelMsg.ccxmlResponse.dlgStart.dlgSrc);
snprintf(lVxmlReq.vxmlUrl, MAX_URL_LEN, "%s/xml/%s", gAppName, gKernelMsg.ccxmlResponse.dlgStart.dlgSrc);
         snprintf(lVxmlReq.vxmlAppl, MAX_URL_LEN, "%s", gAppName);
      }
      else
      {
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "%s Error: Error in Dialogue start", lLogText);
         TERR(gTrace, printf("%s: %s\n", gProcessName, glLogText);)
            gLog.GenerateLog(SESN_IVR44, glLogText);
         return false;
      }
   }
   else
   {
      strcpy(lVxmlReq.vxmlUrl, gKernelMsg.extRequest.fileName);
      lVxmlReq.varCount = 	gKernelMsg.extRequest.varCount;

      T(gTrace, printf("%s: %s Number of Variables in External Request:%d\n", 
               gProcessName, lLogText, gKernelMsg.extRequest.varCount);) 

         for(int i = 0; i < lVxmlReq.varCount; i++)
         {
            strcpy(lVxmlReq.var[i], gKernelMsg.extRequest.var[i]);
            T(gTrace, printf("%s: %s Variable Name in Ext Request:%s\n", 
                     gProcessName, lLogText, lVxmlReq.var[i]);) 
         }
   }

   /* Komal */
   /*BOOLEAN lIsOrigCalledUpdated = false;
   if(gConnectToRedis)
   {
      if(!gIsRedisConnected)
      {
         if(NULL == IvrRedisUtil::ConnectToIvrRedis()){
            TERR(gTrace, printf("%s: Error Connecting to Redis Server on 127.0.0.1:6379\n", gProcessName););
         }
         else
            gIsRedisConnected = true;
      }
      if(gIsRedisConnected)
      {
         char to[20]="", from[20]="", origcalled[20]="";
         if(IvrRedisUtil::GetCallDetailsFromIvrRedis(gCallingNumber, to, from, origcalled))
         {
            T(gTrace, printf("%s: Call Details Retrieved from Redis for %s", gProcessName, gCallingNumber);
            printf(" TO:%s,", to);
            printf(" FROM:%s,", from);
            printf(" ORIG_CALLED:%s\n", origcalled););
            strncpy(lVxmlReq.destAddr, origcalled, MAX_MSISDN_LEN);
            lIsOrigCalledUpdated= true;
         }
         else
         {
            T(gTrace, printf("%s: Failed to retrieve call details from Redis!!!\n", gProcessName););
         }
      }
   }*/

   strncpy(lVxmlReq.origAddr, gCallingNumber, MAX_MSISDN_LEN);
   //if(!lIsOrigCalledUpdated)
      strncpy(lVxmlReq.destAddr, gCalledNumber, MAX_MSISDN_LEN);

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%d DlgSource:%s OrigAddr:%s DestAddr:%s(%s) Sending VXML Request",
         gReferenceNum, lVxmlReq.channelId, lVxmlReq.vxmlEvent,
         lVxmlReq.vxmlUrl, lVxmlReq.origAddr, lVxmlReq.destAddr, gCalledNumber); 

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR45, lLogText);

   //if(true != gIvrQueApi.WriteToVxmlReqQue (lVxmlReq))
   if(true != gIvrQueApi.WriteToVxmlChildReqQue (lVxmlReq))
   {
      return false;
   }

   gIsVxmlCallFlowInProgress = true;

   if(EXT_REQ == gKernelMsg.kernelMsgType)
   {
      switch(gKernelMsg.extRequest.extReqType)
      {
         case EXT_PLAY:
            {
               gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_PLY_SND);
               gCurrentState = ST_EXT_VXML_PLY_SND;
            }
            break;

            /*	case EXT_STOP:
               {
               gCurrentState = ST_EXT_VXML_TERM_SND;
               }
               break;

               case EXT_PATCH:
               {
               gCurrentState = ST_EXT_VXML_PTH_SND;
               }
               break;

               case EXT_TEAR:
               {
               gCurrentState = ST_EXT_VXML_TEAR_SND;
               }
               break;*/

         default:
            {
               //T(gTrace, printf("%s:  Request %s\n", gProcessName, lLogText);) 
               //gLog.GenerateLog(SESN_IVR45, lLogText);
               return false;
            }
      }
   }
   else
   {
      gIvrQueApi.WriteToIvrPegQueue(PEG_SM_VXML_REQ_SND);
      gCurrentState = ST_VXML_REQ_SND;
   }

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendVxmlTerminate()
// DESCRIPTION : Writes Vxml Terminate Request to VXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendVxmlTerminate()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   VxmlRequest   lVxmlReq;

   lVxmlReq.vxmlEvent = EVT_VX_TERM;
   lVxmlReq.channelId = gInChannelId;
   lVxmlReq.refNum    = gReferenceNum;

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%d Sending VXML TERM Request",
         gReferenceNum, lVxmlReq.channelId, lVxmlReq.vxmlEvent); 

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR46, lLogText);

   gIvrQueApi.ClearVxmlReqQue(gInChannelId);

   if(true != gIvrQueApi.WriteToVxmlReqQue (lVxmlReq))
   {
      return false;
   }

   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_VXML_TERM_SND);
   //gCurrentState = ST_NO_STATE;
   gCurrentState = ST_EXT_VXML_TERM_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendExtVxmlTerminate()
// DESCRIPTION : Writes Vxml Terminate Request to VXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendExtVxmlTerminate()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   VxmlRequest   lVxmlReq;

   lVxmlReq.vxmlEvent = EVT_VX_TERM;
   lVxmlReq.channelId = gInChannelId;
   lVxmlReq.refNum    = gReferenceNum;

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%d Sending External VXML TERM Request",
         gReferenceNum, lVxmlReq.channelId, lVxmlReq.vxmlEvent); 

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR47, lLogText);

   if(true != gIvrQueApi.WriteToVxmlReqQue (lVxmlReq))
   {
      return false;
   }

   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_TERM_SND);
   gCurrentState = ST_EXT_VXML_TERM_SND;
   gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_REL_IN_PROGRESS);

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendExtVxmlTerminateRsp()
// DESCRIPTION : Writes Vxml Terminate Request to VXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendExtVxmlTerminateRsp()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   ExtResponse   lExtResponse;
   memset(&lExtResponse, 0, sizeof(ExtResponse));

   lExtResponse.extReqType    = EXT_STOP;
   lExtResponse.result        = 1;
   lExtResponse.inChannelId   = gInChannelId;
   lExtResponse.sessionId     = gInChannelId;

   sprintf(lLogText, "RefNum:%08X CID:%06d SessionId:%d Sending External VXML Terminate Response",
         gReferenceNum, lExtResponse.inChannelId, lExtResponse.sessionId);

   T(gTrace, printf("%s: %s\n",  gProcessName, lLogText);)
      gLog.GenerateLog(SESN_IVR42, lLogText);

   if(true != gIvrQueApi.WriteToExtVxmlRspQue (lExtResponse))
   {
      return false;
   }

   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_STOP_RSP_SND);

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendExtPatchCircuit()
// DESCRIPTION : Notifies Patch Circuit to Media Library
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendExtPatchCircuit()
{
   TEXT          		lLogText[MAX_LOG_TEXT_LEN];
   MpRequestDetails 	lMpRequestDetails;
   MpResponseDetails lMpResponseDetails;

   lMpRequestDetails.cmdType = MC_CNTSP;
   lMpRequestDetails.mpReqData.switchChnl.circuitId_1 = gInChannelId; 
   lMpRequestDetails.mpReqData.switchChnl.circuitId_2 = gPatchCircuitId;
   sprintf(lLogText, "RefNum:%08X ChannelId-1:%d ChannelId-2:%d",
         gReferenceNum, 
         lMpRequestDetails.mpReqData.switchChnl.circuitId_1, 
         lMpRequestDetails.mpReqData.switchChnl.circuitId_2);

   gMediaInterface.SwitchCalls(lMpRequestDetails, lMpResponseDetails);

   if(MP_SUCCESS != lMpResponseDetails.voiceProcessErr)
   {
      sprintf(glLogText, "%s Error:%d Sending External Patch Circuit Failure",
            lLogText, lMpResponseDetails.voiceProcessErr);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         gLog.GenerateLog(SESN_IVR48, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_PTH_FAIL);
   }
   else
   {
      sprintf(glLogText, "%s Sending External Patch Circuit Success", lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         gLog.GenerateLog(SESN_IVR48, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_PTH_SND);
   }

   gCurrentState = ST_EXT_VXML_PTH_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendExtTearCircuit()
// DESCRIPTION : Notifies Tear Circuit to Media Library
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendExtTearCircuit()
{
   TEXT          		lLogText[MAX_LOG_TEXT_LEN];
   MpRequestDetails 	lMpRequestDetails;
   MpResponseDetails lMpResponseDetails;

   lMpRequestDetails.cmdType = MC_DISSP;
   lMpRequestDetails.mpReqData.switchChnl.circuitId_1 = gInChannelId; 
   lMpRequestDetails.mpReqData.switchChnl.circuitId_2 = gPatchCircuitId;
   sprintf(lLogText, "RefNum:%08X ChannelId-1:%d ChannelId-2:%d",
         gReferenceNum,
         lMpRequestDetails.mpReqData.switchChnl.circuitId_1, 
         lMpRequestDetails.mpReqData.switchChnl.circuitId_2);

   gMediaInterface.SwitchCalls(lMpRequestDetails, lMpResponseDetails);

   if(MP_SUCCESS != lMpResponseDetails.voiceProcessErr)
   {
      sprintf(glLogText, "%s Error:%d Sending External Tear Circuit Failure", 
            lLogText, lMpResponseDetails.voiceProcessErr);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         gLog.GenerateLog(SESN_IVR48, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_TEAR_FAIL);
   }
   else
   {
      sprintf(glLogText, "%s Sending External Tear Circuit Success", lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         gLog.GenerateLog(SESN_IVR48, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_TEAR_SND);
   }

   //	gCurrentState = ST_EXT_VXML_PTH_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendAnm()
// DESCRIPTION : Writes ANM to ISUP Encoder
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendAnm()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   gGenIsupMsg.isupMsgType        = GEN_ISUP_ANM;
   gGenIsupMsg.circuitId          = gInChannelId;
   gGenIsupMsg.isupAnm.typeOfAns  = gInAnm.typeOfAns;
   gGenIsupMsg.isupAnm.optLen     = gInAnm.optLen;
   memcpy(gGenIsupMsg.isupAnm.opt, gInAnm.opt, gInAnm.optLen);

   sprintf(lLogText, "RefNum:%08X CID:%06d GenIsupMsgType:%d Sending ANM to ISUP",  
         gReferenceNum, gGenIsupMsg.circuitId, gGenIsupMsg.isupMsgType);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);); 
   gLog.GenerateLog(SESN_IVR49, lLogText);

   if(true != gIvrQueApi.WriteToIsupEncQue (gGenIsupMsg))
   {
      return false;
   }

   gAnswerTime = time(NULL);
   gIvrEvrCause   = CAS_IVR_CALL_ANSWERED;
   gIvrEvrReason  = RSN_IVR_NONE;
   GenerateEvr();

   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_ANM_SND);

   gCurrentState = ST_ANM_SND;


   if(gBillingInProgress)
   {
      if(true != ProcessCallCharge(BILLING_CALL_ANSEWERED))
      {
         return false;
      }
   }

   if (gAllLogsReqd)
   {
      //gLog.GenerateLog(CPM11,lLogText);   
   }

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendRel()
// DESCRIPTION : Writes ACM to ISUP Encoder
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendRel()
{
   //msgQueueError lQErr;
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   gDisconnectTime = time(NULL);
   gGenIsupMsg.isupMsgType      = GEN_ISUP_REL;
   gGenIsupMsg.circuitId        = gInChannelId;
   gGenIsupMsg.isupRel.cause    = gRelCause ;
   gGenIsupMsg.isupRel.location = gRelLoc;
   gGenIsupMsg.isupRel.optLen   =  gInRel.optLen  ;
   memcpy(gGenIsupMsg.isupRel.opt, gInRel.opt, gInRel.optLen);

   sprintf(lLogText, "RefNum:%08X CktId:%d Cause:%02X Sending REL to ISUP",
         gReferenceNum,
         gGenIsupMsg.circuitId,
         gGenIsupMsg.isupRel.cause);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR50, lLogText);

   if(true != gIvrQueApi.WriteToIsupEncQue (gGenIsupMsg))
   {
      return false;
   }

   // change state to REL SND
   gIvrQueApi.WriteToIvrPegQueue(PEG_SM_REL_SND);
   gCurrentState = ST_REL_SND;
   gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_FREE);

   if(!gIsRejectedCall)
   {
      gIvrEvrCause   = CAS_IVR_CALL_COMPLETE;
      gIvrEvrReason  = RSN_IVR_NONE;
      gDuration   =  (gAnswerTime? (gDisconnectTime - gAnswerTime): 0);

      GenerateEvr();
   }

   if(gScheduleRspFlg)
      gScheduleRsp.disconnectTime = time(NULL);

   if(gBillingInProgress)
   {
      if(true != ProcessCallCharge(BILLING_CALL_DISCONNECTED))
      {
         return false;
      }
   }

   return true;
}

//--------------------------------------------------------------
// METHOD      : SendIam()
// DESCRIPTION : Writes IAM to ISUP Encoder
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendIam()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   //strcpy(gKernelMsg.isupMsg.isupIam.callingNumber, gKernelMsg.scheduleReq.origAddr);
   //strcpy(gKernelMsg.isupMsg.isupIam.calledNumber, gKernelMsg.scheduleReq.shortCode);
   //gKernelMsg.isupMsg.circuitId = gInChannelId;
   gGenIsupMsg.isupMsgType           =  GEN_ISUP_IAM;
   gGenIsupMsg.circuitId             =  gInChannelId;
   gRelCause   =  0x10;
   gConnectTime = gAnswerTime = gDuration = 0;
   if(SCHEDULE_REQ != gKernelMsg.kernelMsgType)
   {
      strcpy(gGenIsupMsg.isupIam.callingNumber,gCallingNumber);
      //            gKernelMsg.isupMsg.isupIam.callingNumber);
      strcpy(gGenIsupMsg.isupIam.calledNumber, gCalledNumber);
      //            gKernelMsg.isupMsg.isupIam.calledNumber);
   }
   else
   {
      strcpy(gGenIsupMsg.isupIam.callingNumber, gCallingNumber);
      strcpy(gGenIsupMsg.isupIam.calledNumber, gCalledNumber); 
   }

   if(gScheduleRspFlg)
   {
      gScheduleRsp.callTime = time(NULL);
   }

   sprintf(lLogText, "RefNum:%08X CktId:%d OrigAddr:%s DestAddr:%s Sending IAM to ISUP",
         gReferenceNum, gGenIsupMsg.circuitId,
         gCallingNumber, gCalledNumber);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR59, lLogText);

   if(true != gIvrQueApi.WriteToIsupEncQue (gGenIsupMsg))
   {
      return false;
   }


   // change state to REL SND
   //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_DIAL_OUT_IAM_SND);
   gCurrentState = ST_DIAL_OUT_IAM_SND;   

   gIvrEvrEvent   = EVT_IVR_OUT_CALL;
   gIvrEvrCause   = CAS_IVR_CALL_DIALLED;
   gIvrEvrReason  = RSN_IVR_NONE;
   GenerateEvr();
   return true;
}

//--------------------------------------------------------------
// METHOD      : SendRlc()
// DESCRIPTION : Writes RLC to ISUP Encoder
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendRlc()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   gGenIsupMsg.isupMsgType           =  GEN_ISUP_RLC;
   gGenIsupMsg.circuitId             =  gInChannelId;
   strcpy(gGenIsupMsg.isupIam.callingNumber,
         gKernelMsg.isupMsg.isupIam.callingNumber);
   strcpy(gGenIsupMsg.isupIam.calledNumber,
         gKernelMsg.isupMsg.isupIam.calledNumber);

   sleep(1);

   sprintf(lLogText, "RefNum:%08X CktId:%d Sending RLC to ISUP",
         gReferenceNum,
         gGenIsupMsg.circuitId);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      //gLog.GenerateLog(SESN_IVR59, lLogText);  TBD

      if(true != gIvrQueApi.WriteToIsupEncQue (gGenIsupMsg))
      {
         return false;
      }

   gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_FREE);
   // change state to REL SND
   //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_DIAL_OUT_IAM_SND);
   //gCurrentState = ST_DIAL_OUT_IAM_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : ChangeStateToAcm
// DESCRIPTION : Changes state to ACM 
// PARAMETER   : no arguments
// RETURN      : VOID
//---------------------------------------------------------------
BOOLEAN ChangeStateToAcm()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   sprintf(lLogText, "RefNum:%08X CktId:%d Changing State to ACM", 
         gReferenceNum, gInChannelId );
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 

      gCurrentState = ST_ACM_RCVD;
   gLog.GenerateLog(SESN_IVR60, lLogText);

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendCallCreated()
// DESCRIPTION : Writes Call Created to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendCallCreated()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   CcxmlRequest  lCcxmlReq;

   memset(&lCcxmlReq, 0, sizeof(CcxmlRequest));

   strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, 
         gKernelMsg.isupMsg.isupIam.callingNumber);
   lCcxmlReq.channelId = gPrimaryCircuitId;
   lCcxmlReq.sessionId = gInChannelId;
   lCcxmlReq.refNum 	  = gReferenceNum;

   strcpy(lCcxmlReq.ccxmlEventRequest.event, 
         IvrUtil::ConvertCcxmlEventToString (EVT_CC_CONNECTED));

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%s SessionId:%d OrigAddr:%s Sending CCXML Call Created Event", 
         gReferenceNum, lCcxmlReq.channelId,
         lCcxmlReq.ccxmlEventRequest.event, 
         lCcxmlReq.sessionId, lCcxmlReq.ccxmlEventRequest.origAddr);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR61, lLogText);

   if(true != gIvrQueApi.WriteToCcxmlEventQue (lCcxmlReq))
   {
      return false;
   }

   //	gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DLG_EXT_SND);  
   if(SCHEDULE_REQ != gKernelMsg.kernelMsgType)
   {
      gCurrentState = ST_CCXML_CONN_SND;
   }
   else
   {
      gCurrentState = ST_CCXML_CALL_CREATED;
   }

   return true;
}
//--------------------------------------------------------------
// METHOD      : JoinCcxmlCalls()
// DESCRIPTION : Notifies Patch Circuit to Media Library
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN JoinCcxmlCalls()
{
   TEXT          		lLogText[MAX_LOG_TEXT_LEN];
   MpRequestDetails 	lMpRequestDetails;
   MpResponseDetails lMpResponseDetails;

   lMpRequestDetails.cmdType = MC_CNTSP;
   lMpRequestDetails.mpReqData.switchChnl.circuitId_1 = gInChannelId; 
   lMpRequestDetails.mpReqData.switchChnl.circuitId_2 = gPatchCircuitId;
   sprintf(lLogText, "RefNum:%08X ChannelId-1:%d ChannelId-2:%d",
         gReferenceNum,
         lMpRequestDetails.mpReqData.switchChnl.circuitId_1, 
         lMpRequestDetails.mpReqData.switchChnl.circuitId_2);


   gMediaInterface.SwitchCalls(lMpRequestDetails, lMpResponseDetails);

   if(MP_SUCCESS != lMpResponseDetails.voiceProcessErr)
   {
      gJoinStatus = false; 
      sprintf(glLogText, "%s Error:%d Sending Ccxml Patch Circuit Failure",
            lLogText, lMpResponseDetails.voiceProcessErr);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         //gIvrQueApi.WriteToIvrPegQueue();
         gLog.GenerateLog(SESN_IVR63, glLogText);
   }
   else
   {
      gJoinStatus = true; 
      sprintf(glLogText, "%s Sending Ccxml Patch Circuit Success", lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         //gIvrQueApi.WriteToIvrPegQueue();
         gLog.GenerateLog(SESN_IVR64, glLogText);
   }

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendCcxmlJoinRsp()
// DESCRIPTION : Writes Call Created to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendCcxmlJoinRsp()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   CcxmlRequest  lCcxmlReq;

   memset(&lCcxmlReq, 0, sizeof(CcxmlRequest));

   //strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, 
   //      gKernelMsg.isupMsg.isupIam.callingNumber);
   lCcxmlReq.channelId = gInChannelId;
   lCcxmlReq.sessionId = gInChannelId;
   lCcxmlReq.refNum 	  = gReferenceNum;

   if(gJoinStatus)
   {
      strcpy(lCcxmlReq.ccxmlEventRequest.event, 
            IvrUtil::ConvertCcxmlEventToString (EVT_CC_CONF_JOINED));
   }
   else
   {
      strcpy(lCcxmlReq.ccxmlEventRequest.event, 
            IvrUtil::ConvertCcxmlEventToString (EVT_CC_FAILED));
   }

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%s SessionId:%d OrigAddr:%s Sending CCXML Call Join Response", 
         gReferenceNum, 
         lCcxmlReq.channelId, 
         lCcxmlReq.ccxmlEventRequest.event, 
         lCcxmlReq.sessionId, lCcxmlReq.ccxmlEventRequest.origAddr);

   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR65, lLogText);

   if(true != gIvrQueApi.WriteToCcxmlEventQue (lCcxmlReq))
   {
      return false;
   }

   //	gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DLG_EXT_SND);  
   gCurrentState = ST_CCXML_CALL_JOIN;
   gInChannelId   =  gPatchCircuitId;

   return true;
}
//--------------------------------------------------------------
// METHOD      : UnjoinCcxmlCalls()
// DESCRIPTION : Notifies Tear Circuit to Media Library
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN UnjoinCcxmlCalls()
{
   TEXT          		lLogText[MAX_LOG_TEXT_LEN];
   MpRequestDetails 	lMpRequestDetails;
   MpResponseDetails lMpResponseDetails;

   lMpRequestDetails.cmdType = MC_DISSP;
   lMpRequestDetails.mpReqData.switchChnl.circuitId_1 = gInChannelId; 
   lMpRequestDetails.mpReqData.switchChnl.circuitId_2 = gPatchCircuitId;
   sprintf(lLogText, "RefNum:%08X ChannelId-1:%d ChannelId-2:%d",
         gReferenceNum,
         lMpRequestDetails.mpReqData.switchChnl.circuitId_1, 
         lMpRequestDetails.mpReqData.switchChnl.circuitId_2);

   gMediaInterface.SwitchCalls(lMpRequestDetails, lMpResponseDetails);

   if(MP_SUCCESS != lMpResponseDetails.voiceProcessErr)
   {
      gJoinStatus = false; 
      sprintf(glLogText, "%s Error:%d Sending Ccxml Unjoin Circuit Failure",
            lLogText, lMpResponseDetails.voiceProcessErr);
      T(gTrace, printf("%s: %s\n",  gProcessName, glLogText);) 
         //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_TEAR_FAIL);
         gLog.GenerateLog(SESN_IVR67, glLogText);
   }
   else
   {
      gJoinStatus = true; 
      sprintf(glLogText, "%s Sending Ccxml Unjoin Circuit Success", lLogText);
      T(gTrace, printf("%s: %s\n", gProcessName, glLogText);) 
         //gIvrQueApi.WriteToIvrPegQueue(PEG_SM_EXT_VXML_TEAR_SND);
         gLog.GenerateLog(SESN_IVR68, glLogText);
   }

   //	gCurrentState = ST_EXT_VXML_PTH_SND;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendCcxmlUnjoinRsp()
// DESCRIPTION : Writes Call Created to CCXML Processor
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendCcxmlUnjoinRsp()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   CcxmlRequest  lCcxmlReq;

   memset(&lCcxmlReq, 0, sizeof(CcxmlRequest));

   //	strcpy(lCcxmlReq.ccxmlEventRequest.origAddr, 
   //       gKernelMsg.isupMsg.isupIam.callingNumber);
   lCcxmlReq.channelId = gInChannelId;
   lCcxmlReq.sessionId = gInChannelId;
   lCcxmlReq.refNum 	  = gReferenceNum;

   if(gJoinStatus)
   {
      strcpy(lCcxmlReq.ccxmlEventRequest.event, 
            IvrUtil::ConvertCcxmlEventToString (EVT_CC_CONF_UNJOINED));
   }
   else
   {
      strcpy(lCcxmlReq.ccxmlEventRequest.event, 
            IvrUtil::ConvertCcxmlEventToString (EVT_CC_FAILED));
   }

   sprintf(lLogText, "RefNum:%08X CID:%06d Event:%s SessionId:%d OrigAddr:%s Sending CCXML Call Join Response", 
         gReferenceNum,
         lCcxmlReq.channelId, 
         lCcxmlReq.ccxmlEventRequest.event, 
         lCcxmlReq.sessionId, lCcxmlReq.ccxmlEventRequest.origAddr);

   T(gTrace, printf("%s: %s\n",  gProcessName, lLogText);) 
      gLog.GenerateLog(SESN_IVR65, lLogText);

   if(true != gIvrQueApi.WriteToCcxmlEventQue (lCcxmlReq))
   {
      return false;
   }

   //	gIvrQueApi.WriteToIvrPegQueue(PEG_SM_CCXML_DLG_EXT_SND);  
   if(ST_CCXML_CONN_SND != gCurrentState)
   {
      gInChannelId   =  gPatchCircuitId;
   }
   gCurrentState = ST_CCXML_CALL_UNJOIN;

   return true;
}
//--------------------------------------------------------------
// METHOD      : SendTermChild
// DESCRIPTION : Terminates any previous Child 
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
void SendTermChild()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];
   KernelMsg     lKernelMsg;
   /*			printf("%s: Call Alert: KernelMsgType:%d GenIsupMsgType:%d CircuitId:%d CallingParty:%s CalledParty:%s\n\n\n",
            gProcessName,
            gKernelMsg.kernelMsgType, 
            gKernelMsg.isupMsg.isupMsgType, 
            gKernelMsg.isupMsg.circuitId, 
            gKernelMsg.isupMsg.isupIam.callingNumber, 
            gKernelMsg.isupMsg.isupIam.calledNumber);
    */

   //		gIvrChannelApi.GetChannelStatus(gInChannelId));

   memset(&lKernelMsg, 0, sizeof(KernelMsg));

   if(IVR_CHANNEL_REL_IN_PROGRESS != gIvrChannelApi.GetChannelStatus(gInChannelId))
   {
      lKernelMsg.refNum = gReferenceNum;
      lKernelMsg.kernelMsgType = TERM_MSG;

      sprintf(lLogText, "RefNum:%08X Session-%d Sending TERM_CHILD",gReferenceNum, gInChannelId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText);) 
         gLog.GenerateLog(SESN_IVR07, lLogText);

      gIvrQueApi.WriteToEventChildQue (lKernelMsg, gInChannelId);
   }
   //gCurrentState = ST_NO_STATE;

   return;
}

//--------------------------------------------------------------
// METHOD      : ExitChild()
// DESCRIPTION : Writes Terminate Child to Child Queue 
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN ExitChild()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   // Added to check whether ACT_EXIT is invoked on ST_VXML_REQ_SND state or ST_CCXML_DISCO_SND
   // If invoked from ST_CCXML_DISCO_SND, and call is answered (VXML Call Flow is running,
   //  No action is required here, otherwise continue with ExitChild functionality

   if(gIsVxmlCallFlowInProgress)
      return true;

   gConnectTime = gAnswerTime = gDuration = gDisconnectTime = 0;
   if( true == gIsupRelease)
   {
      SendRlc();
      gIsupRelease = false;
   }

   if(true == gScheduleRspFlg)
   {
      gScheduleRspFlg = false;
      if(0x10 != gRelCause)
      {
         gScheduleRsp.status = IVR_FAILED;
      }

      gScheduleRsp.errorCode = gRelCause; 
      strcpy(gScheduleRsp.dtmf, gDtmf);
      gIvrQueApi.WriteToSchRspQ(gScheduleRsp);
   }

   if(true == gDiffRefNumFlag)
   {
      //T(gTrace, printf("%s: Response Got for different Reference Number\n", gProcessName, lLogText);) 
      gDiffRefNumFlag	=	false;
   }
   else
   {
      gCurrentState = ST_NO_STATE;
      gIvrChannelApi.SetChannelStatus(gInChannelId, IVR_CHANNEL_FREE);
      gScheduleReq = false;

      sprintf(lLogText, "RefNum:%08X Session-%d Revoking State to NO_STATE",gReferenceNum, gInChannelId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SESN_IVR51, lLogText);
   }
   //	exit(0); 
   return true;
}

//--------------------------------------------------------------
// METHOD      : TerminateChild()
// DESCRIPTION : Writes Terminate Child to Child Queue 
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN TerminateChild()
{
   //TEXT          lLogText[MAX_LOG_TEXT_LEN];
   SendCcxmlDisconnect();
   SendVxmlTerminate();
   /*	VxmlRequest   lVxmlReq;

      lVxmlReq.vxmlEvent = EVT_VX_TERM;
      lVxmlReq.channelId = gInChannelId;
      lVxmlReq.refNum    = gReferenceNum;

      sprintf(lLogText, "Event:%d CID:%06d",
      lVxmlReq.vxmlEvent, lVxmlReq.channelId);

      T(gTrace, printf("%s: Sending External VXML TERM Request %s\n", gProcessName, lLogText);)
      gLog.GenerateLog(SESN_IVR47, lLogText);

      if(true != gIvrQueApi.WriteToVxmlReqQue (lVxmlReq))
      {
      return false;
      }
    */

   //gCurrentState = ST_NO_STATE;
   //	gCurrentState = ST_EXT_VXML_TERM_SND;

   //sprintf(lLogText, "CktId:%d", gInChannelId);
   //T(gTrace, printf("%s: Previous Child exiting... %s\n\n", gProcessName, lLogText);)
   //	gLog.GenerateLog(SESN_IVR52, lLogText);
   //exit(0); 
   return true;
}

//--------------------------------------------------------------
// METHOD      : SendDoNothing()
// DESCRIPTION : Writes ACM to ISUP Encoder
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN SendDoNothing()
{
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CurrentState:%s Current Event:%s No action defined",
         gReferenceNum,
         ConvertStateIdentifier(gCurrentState),
         ConvertEventIdentifier(gCurrentEvent));
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText);); 
   gLog.GenerateLog(SESN_IVR53, lLogText);
   return true;
}

//-------------------------------------------------------------------------------
// METHOD      : ConvertStateIdentifier
// DESCRIPTION : Converts Identifier to text
// PARAMETER   : no arguments
// RETURN      : TEXT *
// THROWS      : none
//-------------------------------------------------------------------------------
TEXT* ConvertStateIdentifier(UINT8 Identifier)
{
   switch (Identifier)
   {     
      case ST_NO_STATE:
         {
            return (TEXT*) "NO_STATE";
         }
      case ST_ACM_SND:
         {
            return (TEXT*) "ACM_SND";
         }
      case ST_ANM_SND:
         {
            return (TEXT*) "ANM_SND";
         }
      case ST_REL_SND:
         {
            return (TEXT*) "REL_SND";
         }
      case ST_CCXML_ALT_SND:
         {
            return (TEXT*) "CCXML_ALT_SND";
         }
      case ST_CCXML_CONN_SND:
         {
            return (TEXT*) "CCXML_CONN_SND";
         }
      case ST_VXML_REQ_SND:
         {
            return (TEXT*) "VXML_REQ_SND";
         }
      case ST_CCXML_DLG_EXT_SND:
         {
            return (TEXT*) "CCXML_DLG_EXT_SND";
         }
      case ST_CCXML_DISCO_SND:
         {
            return (TEXT*) "CCXML_DISCO_SND";
         }
      case ST_EXT_VXML_PLY_SND:
         {
            return (TEXT*) "EXT_VXML_PLY_SND";
         }
      case ST_EXT_VXML_TERM_SND:
         {
            return (TEXT*) "EXT_VXML_TERM_SND";
         }
      case ST_EXT_VXML_PTH_SND:
         {
            return (TEXT*) "EXT_VXML_PTH_SND";
         }
      case ST_EXT_VXML_TEAR_SND:
         {
            return (TEXT*) "EXT_VXML_TEAR_SND";
         }
      case ST_DIAL_OUT_IAM_SND:
         {
            return (TEXT*) "DIAL_OUT_IAM_SND";
         }
      case ST_ACM_RCVD:
         {
            return (TEXT*) "ACM_RCVD";
         }
      case ST_CCXML_CALL_CREATED:
         {
            return (TEXT*) "CCXML_CALL_CREATED";
         }
      case ST_CCXML_CALL_JOIN:
         {
            return (TEXT*) "CCXML_CALL_JOIN";
         }
      case ST_CCXML_CALL_UNJOIN:
         {
            return (TEXT*) "CCXML_CALL_UNJOIN";
         }
      default:
         {
            return (TEXT*) "UNKNOWN";
         }
   }
}

//-------------------------------------------------------------------------------
// METHOD      : ConvertEventIdentifier
// DESCRIPTION : Converts Identifier to text
// PARAMETER   : no arguments
// RETURN      : TEXT *
// THROWS      : none
//-------------------------------------------------------------------------------
TEXT* ConvertEventIdentifier(UINT8 Identifier)
{
   switch (Identifier)
   {     
      case EVT_NULL:
         {
            return (TEXT*) "NULL";
         }
      case EVT_IAM:
         {
            return (TEXT*) "IAM";
         }
      case EVT_TERMINATE_CHILD:
         {
            return (TEXT*) "TERMCHILD";
         }
      case EVT_CCXML_ALT_ACK_RCV:
         {
            return (TEXT*) "CCXML_ALT_ACK_RCV";
         }
      case EVT_CCXML_ALT_NACK_RCV:
         {
            return (TEXT*) "CCXML_ALT_NACK_RCV";
         }
      case EVT_CCXML_DISCONN_RCV:
         {
            return (TEXT*) "CCXML_DISCONN_RCV";
         }
      case EVT_CCXML_DLG_STR_RCV:
         {
            return (TEXT*) "CCXML_DLG_STR_RCV";
         }
      case EVT_VXML_RSP_RCV:
         {
            return (TEXT*) "VXML_RSP_RCV";
         }
      case EVT_REL:
         {
            return (TEXT*) "REL";
         }
      case EVT_CCXML_EXT_RCV:
         {
            return (TEXT*) "CCXML_EXT_RCV";
         }
      case EVT_EXT_PLY_RCV:
         {
            return (TEXT*) "EXT_PLY_RCV";
         }
      case EVT_EXT_STP_RCV:
         {
            return (TEXT*) "EXT_STP_RCV";
         }
      case EVT_EXT_PTH_RCV:
         {
            return (TEXT*) "EXT_PTH_RCV";
         }
      case EVT_EXT_TEAR_RCV:
         {
            return (TEXT*) "EXT_TEAR_RCV";
         }
      case EVT_DIAL_OUT:
         {
            return (TEXT*) "DIAL_OUT";
         }
      case EVT_ACM:
         {
            return (TEXT*) "ACM";
         }
      case EVT_ANM:
         {
            return (TEXT*) "ANM";
         }
      case EVT_CPG:
         {
            return (TEXT*) "CPG";
         }
      case EVT_CCXML_JOIN:
         {
            return (TEXT*) "CCXML_JOIN";
         }
      case EVT_CCXML_UNJOIN:
         {
            return (TEXT*) "CCXML_UNJOIN";
         }
      default:
         {
            return (TEXT*) "EVT_NULL";
         }
   }
}

//-------------------------------------------------------------
// METHOD      : GetSystemTime
// DESCRIPTION : It calculates system time in the format [ddmmyyyy]
//               and fills it in the buffer passed by  address.
// PARAMETER   : TEXT*
//                  Buf - Buffer to get the system time
// RETURN      : void
// THROWS      : None
//-------------------------------------------------------------
void GetSystemTime(TEXT *Buf)
{
   TEXT      lLogTime[SIZE_25];
   long      lCurrentTime;
   struct tm *tm_ptr;

   time(&lCurrentTime);
   tm_ptr = localtime(&lCurrentTime);
   strftime(lLogTime,SIZE_25,"%Y%m%d%I%M",tm_ptr);
   strcpy(Buf,lLogTime);
}

//--------------------------------------------------------------------------------------------------
// METHOD      : MonitorChannelInstance
// DESCRIPTION : Checks whether the given instance of Channel is running or not, if not then Reset the 
//               Channel Status to IVR_CHANNEL_UNINITIALIZED.
// PARAMETER   : None
// RETURN      : BOOLEAN
//--------------------------------------------------------------------------------------------------
void MonitorChannelInstance(int sigNum)
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN];
   int    lRetVal = 0;

   for(UINT16 lChannelId = 1; lChannelId < IVR_MAX_CHANNEL_ID; lChannelId++)
   {
      if(gIntChildPid[lChannelId] > 0)
      {  
         lRetVal =  kill(gIntChildPid[lChannelId], 0);
         //Kill all the child of this particular instance and Restart all childs

         snprintf(lLogText, MAX_LOG_TEXT_LEN, "CHANNEL_ID: %d PID: %d Monitored",
               lChannelId,
               gIntChildPid[lChannelId]);

         if (lRetVal < 0)
         {
            TERR(gTrace, printf("%s:  %s Monitored -  Not Running\n", gProcessName, lLogText););
            gLog.GenerateLog(SESN_IVR72 , lLogText);
            gIntChildPid[lChannelId] = -2;
            gIvrChannelApi.SetChannelStatus(lChannelId, IVR_CHANNEL_UNINITIALIZED, 0); 
         }
         else
         {
            //T(gTrace, printf("%s: %s Monitored - Found Running\n", gProcessName, lLogText););
            //   gLog.GenerateLog(SESN_IVR71, lLogText);
         }
      }
   }

   signal(SIGALRM, MonitorChannelInstance);
   alarm(gMonitorInterval);

   return ;
}
//--------------------------------------------------------------------------------------------------
// METHOD      : GenerateEvr
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : BOOLEAN
//--------------------------------------------------------------------------------------------------
BOOLEAN GenerateEvr()
{
   memset(&gIvrEvrMsg, 0, sizeof(IvrEvrMsg));

   gIvrEvrMsg.evrEvent    =  gIvrEvrEvent;
   gIvrEvrMsg.evrCause    =  gIvrEvrCause;
   gIvrEvrMsg.evrReason   =  gIvrEvrReason;
   strcpy(gIvrEvrMsg.shortCode, gShortCode);
   strcpy(gIvrEvrMsg.cldNum, gCalledNumber);
   strcpy(gIvrEvrMsg.clgNum, gCallingNumber);
   gIvrEvrMsg.refId   =  gInChannelId;
   gIvrEvrMsg.duration = gDuration;

   snprintf(gIvrEvrMsg.callRefNum, 20,  "%08X", gReferenceNum);

   if(true != gIvrEvrApi.GenerateEvr (gIvrEvrMsg))
   {
      return false;
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessCallCharge
// DESCRIPTION : Blocks Amount
// PARAMETERS  : NONE
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN ProcessCallCharge(EnumBillingEvents billingEvents)
{
   TEXT       lLogText[MAX_LOG_TEXT_LEN] = "";
   BOOLEAN    lRetVal = true;

   gBillingRequest.appId = IVR_SES_MNGR;
   gBillingRequest.mode     = CHARGE_MODE_UNKNOWN;
   gBillingRequest.referenceNum = gReferenceNum;
   gBillingRequest.expiryTime =  BILLING_DEFAULT_EXPIRY_TIME;
   gBillingRequest.event = billingEvents;

   strcpy(gBillingRequest.msisdn, gCallingNumber);
   strcpy(gBillingRequest.shortCode, gCalledNumber);
   strcpy(gBillingRequest.purpose, "Call");

   //gBillingRequest.subsType = mProvDetails.subsInfo.subsType;
   //gBillingRequest.shortCode,
   //gBillingRequest.imsi
   //gBillingRequest.chargeId;
   //gBillingRequest.contentId  =  mContentId;
   gBillingRequest.channelId = gInChannelId;

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "Msisdn:%s Event:%d(%s) RefNo:%d SubsType:%d(%s) ChargeId:%d Expiry:%d",
         gBillingRequest.msisdn, gBillingRequest.event,
         BillingUtil::ConvertBillingEventTypeToStr (gBillingRequest.event),
         gBillingRequest.referenceNum, gBillingRequest.subsType,
         BillingUtil::ConvertSubsTypeToStr (gBillingRequest.subsType),
         gBillingRequest.chargeId, gBillingRequest.expiryTime);

   if(billingEvents == BILLING_CALL_RECEIVED)
   {
      gBillingInProgress = false;

      if(true != gBillingApi.ProcessBillingRequest (gBillingRequest, gBillingResponse))
      {
         sprintf(lLogText, "%s Status:%d",
               gLogText, gBillingResponse.status);
         TERR (gTrace, printf ("%s: %s Billing Failed\n",
                  gProcessName, lLogText););
         gBillingInProgress = false;
         //   gLog.GenerateLog(lLogText);
         lRetVal = false;
         //return false;
      }
      else
      {
         if(gBillingResponse.status == BILLING_SUCCESS)
            gBillingInProgress = true;
      }
   }
   else if(gBillingInProgress)
   {
      if(true != gBillingApi.ProcessBillingRequest (gBillingRequest, gBillingResponse))
      {
         sprintf(lLogText, "%s Status:%d",
               gLogText, gBillingResponse.status);
         TERR (gTrace, printf ("%s: %s Billing Failed\n",
                  gProcessName, lLogText););
         gBillingInProgress = false;
         //   gLog.GenerateLog(lLogText);
         lRetVal = false;
      }
   }
   else
   {
      T(gTrace, printf("%s: %s Billing Disabled\n", gProcessName, lLogText););
   }


   if(billingEvents == BILLING_CALL_DISCONNECTED)
   {
      gBillingInProgress = false;
   }
   return lRetVal;
}

//----------------------------------------------------------------------
// METHOD      : NumberNormalize
// DESCRIPTION :
// PARAMETER   : None
// RETURN      :
//----------------------------------------------------------------------
BOOLEAN  NumberNormalize(TEXT  *lMsisdn, TEXT  *lCalledNumber)
{
   UINT16   lCcLen = strlen(gCountryCode);
   UINT16   lMsisdnLen = 0;
   TEXT     lMsisdnLocal[21]="";
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";
   UINT16   lCalledNumLen = 0;
   TEXT     lCalledLocal[21] = "";

   lCalledNumLen= strlen(lCalledNumber);
   lMsisdnLen = strlen(lMsisdn);

   T(gTrace, printf("%s: NumberNormalize CC:%s MSISDN:%s CALLED:%s LEN:%d,%d\n",
            gProcessName, gCountryCode, lMsisdn, lCalledNumber, lMsisdnLen, lCalledNumLen););

   if (true == gNormalizeFlg)
   {
      //Calling number normalize

      if(lMsisdnLen > gNdcSnLen && lMsisdnLen >gLandLineLen )
      {
         if(lMsisdn[0] == '+')
         {
            snprintf(lMsisdnLocal, 20, "%s", lMsisdn+1);
            snprintf(lMsisdn, 20, "%s", lMsisdnLocal);
         }
         if(lMsisdn[0] == '0' || lMsisdnLocal[0] =='0')
         {
            snprintf(lMsisdnLocal, 20, "%s", lMsisdn+1);
            if(lMsisdnLocal[0] == '0')
            {
               snprintf(lMsisdnLocal, 20, "%s", lMsisdnLocal+1);
               //      snprintf(lMsisdn, 20, "%s", lMsisdnLocal);
            }
            snprintf(lMsisdn, 20, "%s", lMsisdnLocal);
         }
         if(!gAllowAllCountryCode)
         {
            if (0 != strncmp(lMsisdn, gCountryCode, lCcLen))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "%s. The country code in  msisdn is wrong,rejecting the request",
                     gLogText);
               TERR(gTrace,printf("%s: %s\n", gProcessName, lLogText););
               snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
               gLog.GenerateLog(SESN_IVR84, glLogText);
               return false;
            }
         }
         lMsisdnLen = strlen(lMsisdn);
         if(lMsisdnLen < gNdcSnLen && lMsisdnLen < gLandLineLen)
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s. The Received msisdn length is lessthan expected, rejecting the request",
                  gLogText);
            TERR(gTrace,printf("%s: %s\n", gProcessName, lLogText););
            //gLog.GenerateLog(DIA_INTFC26, lLogText);
            gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
            gIvrEvrReason  = RSN_IVR_CALL_INVALID_DEST_NUMBER;
            gRelCause = 0x15; //Call  Rejected
            GenerateEvr();
            gIsRejectedCall = true;
            return false;
         }
         else if(lMsisdnLen == gNdcSnLen ||  lMsisdnLen == gLandLineLen)
         {
            snprintf(lMsisdnLocal, 20, "%s%s", gCountryCode, lMsisdn);

            snprintf(lMsisdn, 20, "%s", lMsisdnLocal);
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s. Calling Number Normalized:%s", gLogText, lMsisdn);
            T(gTrace,printf("%s: %s (%s)\n", gProcessName, lLogText, gCountryCode););
            //gLog.GenerateLog(DIA_INTFC26, lLogText);
         }
      }
      else if(lMsisdnLen == gNdcSnLen ||  lMsisdnLen == gLandLineLen)
      {
         //strcpy(lMsisdnLocal, lMsisdn);
         snprintf(lMsisdnLocal, 20, "%s%s", gCountryCode, lMsisdn);
         snprintf(lMsisdn, 20, "%s", lMsisdnLocal);

         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s. Calling Number Normalized:%s", gLogText, lMsisdn);
         T(gTrace,printf("%s: %s (%s)\n", gProcessName, lLogText, gCountryCode););
         //gLog.GenerateLog(SESN_IVR84, lLogText);
      }
      else
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s. The Received msisdn length is lessthan expected, rejecting the request",
               gLogText);
         TERR(gTrace,printf("%s: %s\n", gProcessName, lLogText););
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
         gLog.GenerateLog(SESN_IVR84, glLogText);
         gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
         gIvrEvrReason  = RSN_IVR_CALL_INVALID_DEST_NUMBER;
         gRelCause = 0x15; //Call  Rejected
         GenerateEvr();
         gIsRejectedCall = true;
         return false;
      }
      // called number normalize
      if(lCalledNumLen > 0 )
      {
         if(lCalledNumber[0] == '+')
         {
            snprintf(lCalledLocal, 20, "%s", lCalledNumber+1);
            snprintf(lCalledNumber, 20, "%s", lCalledLocal);
         }
         if(lCalledNumber[0] == '0' || lCalledLocal[0] =='0')
         {
            snprintf(lCalledLocal, 20, "%s", lCalledNumber+1);
            if(lCalledLocal[0] == '0')
            {
               snprintf(lCalledLocal, 20, "%s", lCalledLocal+1);
               //      snprintf(lCalledNumber, 20, "%s", lCalledLocal);
            }
            snprintf(lCalledNumber, 20, "%s", lCalledLocal);
         }
         lCalledNumLen = strlen(lCalledNumber);
         if(lCalledNumLen == gNdcSnLen ||  lCalledNumLen == gLandLineLen)
         {
            snprintf(lCalledLocal, 20, "%s%s", gCountryCode, lCalledNumber);

            snprintf(lCalledNumber, 20, "%s", lCalledLocal);
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s. Called Number Normalized:%s", gLogText, lCalledNumber);
            T(gTrace,printf("%s: %s (%s)\n", gProcessName, lLogText, gCountryCode););
            //gLog.GenerateLog(DIA_INTFC26, lLogText);
         }
      }
   }
   return true;
}

