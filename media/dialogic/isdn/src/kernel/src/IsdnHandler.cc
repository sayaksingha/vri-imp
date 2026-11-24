static const char *id = "@(#) $Id: IsdnHandler.cc,v 1.1.4.3 2016/04/12 21:59:36 shijuc Exp $";
//----------------------------------------------------------------------
// NAME IsdnHandler.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Chethan V M
//----------------------------------------------------------------------
#include "IsdnHandler.h"

extern  IsdnHandler gIsdnHandler;
pthread_t      gThreadSrlEvt;
pthread_t      gThreadIsdnCall;
pthread_t      gThreadChnlMgmt;


//----------------------------------------------------------------------
// METHOD NAME : HandleIsdnCall
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void* HandleIsdnCall(void *ptr)
{
   IsdnHandler* pIsdnHandler = (IsdnHandler*) ptr;

#ifdef __x86_64__
   T(gTrace, printf("%s: Isdn Call Thread started  %d \n", gProcessName, syscall(186)););
#else
   T(gTrace, printf("%s: Isdn Call Thread started  %ld \n", gProcessName, syscall(224)););
#endif

   GenIsupMsg        lGenIsupMsg;

   while(IsdnUtil::KeepRunning())
   {
      memset(&lGenIsupMsg, 0, sizeof(lGenIsupMsg));
      if(true == pIsdnHandler->ReadIsupMsg(lGenIsupMsg))
      {
         pIsdnHandler->ProcessIsupMsg(lGenIsupMsg);
      }
   }

   return NULL;
}

//----------------------------------------------------------------------
// METHOD NAME : HandleChnlMgmt
// DESCRIPTION :
// PARAMETERS  :
// RETURN      :
//----------------------------------------------------------------------
void* HandleChnlMgmt(void *ptr)
{
   IsdnHandler* pIsdnHandler = (IsdnHandler*) ptr;

#ifdef __x86_64__
   T(gTrace, printf("%s: Chnl Mgmt Thread started  %d \n", gProcessName, syscall(186)););
#else
   T(gTrace, printf("%s: Chnl Mgmt Thread started  %ld \n", gProcessName, syscall(224)););
#endif


   while(IsdnUtil::KeepRunning())
   {
      pIsdnHandler->ProcessChnlMgmtReq();
   }

   return NULL;
}

//----------------------------------------------------------------------
// METHOD NAME : PollEvts
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void* PollEvts(void *pContext)
{
   IsdnHandler* pIsdnHandler = (IsdnHandler*) pContext;
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

#ifdef __x86_64__
   T(gTrace, printf("%s: Event Thread started  %d \n", gProcessName, syscall(186)););
#else
   T(gTrace, printf("%s: Event Thread started  %ld \n", gProcessName, syscall(224)););
#endif

   METAEVENT         metaevent;
   while(IsdnUtil::KeepRunning())
   {
      if( true == pIsdnHandler->mIsdnDevice.GetSrEvent(metaevent)) 
      {
         if ((metaevent.flags & GCME_GC_EVENT) == 0)
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "DEV:%03ld SrEvent Received NON-GC EvtType:%ld Flag:%lu",
                  metaevent.evtdev, metaevent.evttype, metaevent.flags);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            pIsdnHandler->ProcessNonGcEvents(metaevent);
         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "DEV:%03ld SrEvent Received GC EvtType:%ld Flag:%lu",
                  metaevent.evtdev, metaevent.evttype, metaevent.flags);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            pIsdnHandler->ProcessGcEvents(metaevent);
         }
      }
   }

   return NULL;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessIsupMsg
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ProcessIsupMsg(GenIsupMsg &genIsupMsg)
{
   INT16 lRet = 0;

   lRet = mIsdnDevice.SendIsdnCallMsgToNw(genIsupMsg);
   if(-1 == lRet)
      mIsdnQueApi.WriteToIsupRspQue(genIsupMsg);
   else if(0 == lRet)
      return false;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessChnlMgmtReq
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ProcessChnlMgmtReq()
{
   CktCmdRsp        cktCmdRsp;
   CktCmdReq        cktCmdReq;
   memset(&cktCmdReq, 0, sizeof(CktCmdReq));
   memset(&cktCmdRsp, 0, sizeof(CktCmdRsp));
   if(true == mIsdnQueApi.ReadFromChnlMgmtReqQue(cktCmdReq))
   {
      if(true == mIsdnDevice.SendIsdnChnlMgmtMsgToNw(cktCmdReq, cktCmdRsp))
      {
         return mIsdnQueApi.WriteToChnlMgmtRspQue(cktCmdRsp);
      }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessGcEvents
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ProcessGcEvents(METAEVENT &metaevent)
{
   GenIsupMsg lGenIsupMsg;
   memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));

   if(true == mIsdnDevice.ProcessGlobalCallEvents(metaevent, lGenIsupMsg))
      mIsdnQueApi.WriteToIsupRspQue(lGenIsupMsg);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessNonGcEvents
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ProcessNonGcEvents(METAEVENT &metaevent)
{
   MpResponseDetails lMpRsp;
   memset(&lMpRsp, 0, sizeof(MpResponseDetails));

   if(true == mIsdnDevice.ProcessVoiceEvents(metaevent.evttype, metaevent.evtdev, lMpRsp))
      mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : IsdnHandler
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnHandler::IsdnHandler()
{
}

//----------------------------------------------------------------------
// METHOD NAME : ~IsdnHandler
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnHandler::~IsdnHandler()
{
}

//----------------------------------------------------------------------
// METHOD NAME : DeInit
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::DeInit()
{
   /*   if( 0 != pthread_join(gThreadSrlEvt, NULL))
        {
        TERR (gTrace, printf("%s: Wait till Thread Killed Failed\n", gProcessName););
        return false;
        }
        else
        {
        T (gTrace, printf("%s: Wait till Thread Kill Success\n", gProcessName););
        }
    */
   mIsdnDevice.DeInit();

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::Init()
{

   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(true != mIsdnQueApi.Init())
   {
      sprintf(lLogText, "IsdnQueApi Init Failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(true != mIsdnConfigApi.Init())
   {
      sprintf(lLogText, "ConfigApi Init Failed" );
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(true != mIsdnDevice.Init(mMinCktId, mMaxCktId))
   {
      sprintf(lLogText, "Device Init Failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(true != this->StartSrlThread())
   {
      sprintf(lLogText, "Start Srl Thread Failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(true == mIsdnConfigApi.mHandleIsdnCall)
   {
      if(true != this->StartIsdnCallThread())
      {
         sprintf(lLogText, "Start Isdn Call Thread Failed");
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS04, lLogText);
         return false;
      }
   }

   if(true == mIsdnConfigApi.mHandleChnlMgmt)
   {
      if(true != this->StartChnlMgmtThread())
      {
         sprintf(lLogText, "Start Chnl Mgmt Thread Failed");
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(GSYS04, lLogText);
         return false;
      }
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ReloadConfig()
{
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : StartIsdnCallThread
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::StartIsdnCallThread()
{
   int            lRet = 0;
   void * (*lpStartRoutine) (void *) = HandleIsdnCall;
   TEXT           lLogText[MAX_LOG_TEXT_LEN] = "";

   lRet = pthread_create(&gThreadIsdnCall, NULL, lpStartRoutine, &gIsdnHandler);
   if(0 != lRet)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Cannot Create StartIsdnCallThread pthread_create Failed Ret:%d",
            lRet);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND01, lLogText);
      return false;
   }

   T (gTrace, printf("%s: \33[32m Start Isdn Call Thread Success. ...\33[0m\n", gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : StartChnlMgmtThread
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::StartChnlMgmtThread()
{
   int            lRet = 0;
   void * (*lpStartRoutine) (void *) = HandleChnlMgmt;
   TEXT           lLogText[MAX_LOG_TEXT_LEN] = "";

   lRet = pthread_create(&gThreadChnlMgmt, NULL, lpStartRoutine, &gIsdnHandler);
   if(0 != lRet)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Cannot Create StartChnlMgmtThread pthread_create Failed Ret:%d",
            lRet);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND01, lLogText);
      return false;
   }
   T (gTrace, printf("%s: \33[32m Start Chnl Mgmt Thread Success. ...\33[0m\n", gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : StartSrlThread
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::StartSrlThread()
{
   int            lRet = 0;
   void * (*lpStartRoutine) (void *) = PollEvts;
   TEXT           lLogText[MAX_LOG_TEXT_LEN] = "";

   lRet = pthread_create(&gThreadSrlEvt, NULL, lpStartRoutine, &gIsdnHandler);
   if(0 != lRet)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Cannot Create StartSrlThread pthread_create Failed Ret:%d",
            lRet);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND04, lLogText);
      return false;
   }

   T (gTrace, printf("%s: \33[32m Start Srl Thread Success. ...\33[0m\n", gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadIsupMsg
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ReadIsupMsg(GenIsupMsg &genIsupMsg)
{
   return mIsdnQueApi.ReadFromIsupReqQue(genIsupMsg);
}

//----------------------------------------------------------------------
// METHOD NAME : ReadRequest
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ReadRequest()
{
   return mIsdnQueApi.ReadFromMediaCmdReqQue(mMpRequest);
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessRequest
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::ProcessRequest()
{
   BOOLEAN              lRet = false;
   MpResponseDetails    lMpRsp;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC:%03d COMMAND:%s Received Request", 
         mMpRequest.mpReqData.stream.seqNum, 
         mMpRequest.mpReqData.stream.circuitId,
         IsdnUtil::ConvertCmdToStr(mMpRequest.cmdType));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_HAND02, lLogText);

   memset(&lMpRsp, 0, sizeof(MpResponseDetails));

   switch(mMpRequest.cmdType)
   {
      case MC_CNTTN:
         {
            EnumIsdnProcRetType lPlayRet = ISDN_PROC_RETURN_FALSE;

            lPlayRet = mIsdnDevice.PlayFile(mMpRequest);

            lMpRsp.cmdType             = MC_CNTTN;
            lMpRsp.mpRspData.playMsg   = mMpRequest.mpReqData.playMsg;

            if ( ISDN_PROC_RETURN_TRUE == lPlayRet || ISDN_PROC_RETURN_TRUE_WITH_RSP == lPlayRet)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC:%03d NonBl:%d RetType:%d Play Cmd Issue Success",
                     mMpRequest.mpReqData.playMsg.seqNum,
                     mMpRequest.mpReqData.playMsg.circuitId,
                     mMpRequest.mpReqData.playMsg.nonBlocking, lPlayRet);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND07, lLogText);

               lMpRsp.voiceProcessErr     = MP_SUCCESS;
               lMpRsp.rspMsg              = MP_RSP_SUCCESS;

               if(true == mIsdnConfigApi.mSendRspFlag || ISDN_PROC_RETURN_TRUE_WITH_RSP == lPlayRet)
                  mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC:%03d NonBl:%d RetType:%d Play Cmd Issue Failed",
                     mMpRequest.mpReqData.playMsg.seqNum,
                     mMpRequest.mpReqData.playMsg.circuitId,
                     mMpRequest.mpReqData.playMsg.nonBlocking, lPlayRet);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND06, lLogText);

               lMpRsp.voiceProcessErr     = MP_PLAY_CMD_ERR;
               lMpRsp.rspMsg              = MP_RSP_FAILURE;

               mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }

         }
         break;

      case MC_DISTN:
         {
            EnumIsdnVoiceChannelState lTmpChnlSt;

            lTmpChnlSt = mIsdnDevice.GetVoiceChannelStatus(mMpRequest.mpReqData.stopMsg.circuitId);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CIC:%03d Status before issuing STOP CS:%s",
                  mMpRequest.mpReqData.stopMsg.seqNum,
                  mMpRequest.mpReqData.stopMsg.circuitId,
                  IsdnUtil::ConvertVoiceChnlStatToStr(lTmpChnlSt));
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_HAND30, lLogText);

            switch(lTmpChnlSt)
            {
               case ISDN_VOICE_CHANNEL_IDLE:
               case ISDN_VOICE_CHANNEL_PLAYING:
               case ISDN_VOICE_CHANNEL_PLAY_STOPING:
                  lRet = IssuePlayStop();
                  break;
               case ISDN_VOICE_CHANNEL_RECORDING:
               case ISDN_VOICE_CHANNEL_RECORD_STOPING:
                  lRet = IssueRecordStop();
                  break;
               case ISDN_VOICE_CHANNEL_GET_DTMF:
               case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
                  lRet = IssueGetDigitStop();
                  break;
               default:
                  lRet = IssuePlayStop();
            }
         }
         break;

      case MC_CNTSP:
         {
            lRet = mIsdnDevice.ConnectSpeech(mMpRequest);

            lMpRsp.cmdType              = MC_CNTSP;
            lMpRsp.mpRspData.switchChnl = mMpRequest.mpReqData.switchChnl;

            if(true != lRet)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC_1:%03d CIC_2:%03d ConnectSpeech Cmd Issue Failed",
                     mMpRequest.mpReqData.switchChnl.seqNum,
                     mMpRequest.mpReqData.switchChnl.circuitId_1,
                     mMpRequest.mpReqData.switchChnl.circuitId_2);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND10, lLogText);

               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               lMpRsp.voiceProcessErr      = MP_CONNECT_CMD_ERR;
               mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC_1:%03d CIC_2:%03d ConnectSpeech Cmd Issue success",
                     mMpRequest.mpReqData.switchChnl.seqNum,
                     mMpRequest.mpReqData.switchChnl.circuitId_1,
                     mMpRequest.mpReqData.switchChnl.circuitId_2);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND11, lLogText);

               lMpRsp.voiceProcessErr     = MP_SUCCESS;
               lMpRsp.rspMsg              = MP_RSP_SUCCESS;

               if(true == mIsdnConfigApi.mSendRspFlag)
                  mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
         }
         break;

      case MC_DISSP:
         {
            lRet = mIsdnDevice.DisconnectSpeech(mMpRequest);

            lMpRsp.cmdType              = MC_DISSP;
            lMpRsp.mpRspData.switchChnl = mMpRequest.mpReqData.switchChnl;

            if(true != lRet)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC_1:%03d CIC_2:%03d DisconnectSpeech Cmd Issue Failed",
                     mMpRequest.mpReqData.switchChnl.seqNum,
                     mMpRequest.mpReqData.switchChnl.circuitId_1,
                     mMpRequest.mpReqData.switchChnl.circuitId_2);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND12, lLogText);

               lMpRsp.voiceProcessErr     = MP_CONNECT_CMD_ERR;
               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC_1:%03d CIC_2:%03d DisconnectSpeech Cmd Issue success",
                     mMpRequest.mpReqData.switchChnl.seqNum,
                     mMpRequest.mpReqData.switchChnl.circuitId_1,
                     mMpRequest.mpReqData.switchChnl.circuitId_2);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND13, lLogText);

               lMpRsp.voiceProcessErr     = MP_SUCCESS;
               lMpRsp.rspMsg              = MP_RSP_SUCCESS;

               if(true == mIsdnConfigApi.mSendRspFlag)
                  mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
         }
         break;

      case MC_GETDIG:
         {
            lRet = mIsdnDevice.GetDigit(mMpRequest);

            lMpRsp.cmdType             = MC_GETDIG;
            lMpRsp.mpRspData.getDigit  = mMpRequest.mpReqData.getDigit;

            if(true != lRet)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC:%03d GetDtmfDigits Cmd Issue Failed",
                     mMpRequest.mpReqData.getDigit.seqNum,
                     mMpRequest.mpReqData.getDigit.circuitId);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND14, lLogText);

               lMpRsp.voiceProcessErr     = MP_GET_DIG_CMD_ERR;
               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC:%03d GetDtmfDigits Cmd Issue success",
                     mMpRequest.mpReqData.getDigit.seqNum,
                     mMpRequest.mpReqData.getDigit.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND15, lLogText);

               lMpRsp.voiceProcessErr     = MP_SUCCESS;
               lMpRsp.rspMsg              = MP_RSP_SUCCESS;

               if(true == mIsdnConfigApi.mSendRspFlag)
                  mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }

         }
         break;

      case MC_GETDIGSTP:
         {
            lRet = IssueGetDigitStop();
         }
         break;

      case MC_RECSTR:
         {
            lRet = mIsdnDevice.RecordFile(mMpRequest);
            lMpRsp.cmdType             = MC_RECSTR;
            lMpRsp.mpRspData.recordMsg = mMpRequest.mpReqData.recordMsg;

            if(true != lRet )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC:%03d StartRecord Cmd Issue Failed",
                     mMpRequest.mpReqData.recordMsg.seqNum,
                     mMpRequest.mpReqData.recordMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND18, lLogText);

               lMpRsp.voiceProcessErr     = MP_RECORD_CMD_ERR;
               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CIC:%03d StartRecord Cmd Issue Success",
                     mMpRequest.mpReqData.recordMsg.seqNum,
                     mMpRequest.mpReqData.recordMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_HAND19, lLogText);

               lMpRsp.voiceProcessErr     = MP_SUCCESS;
               lMpRsp.rspMsg              = MP_RSP_SUCCESS;

               if(true == mIsdnConfigApi.mSendRspFlag)
                  mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
            }
         }
         break;

      case MC_RECSTP:
         {
            lRet = IssueRecordStop();
         }
         break;

      case MC_SETVOL:
         break;

      case MC_SETSPD:
         break;

      case MC_PAUSE:
         break;

      case MC_RESUME:
         break;

      case MC_FORWARD:
         break;

      case MC_REWIND:
         break;

      case MC_INVAL_CMD_TYPE:
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d COMMAND:%s Invalid Command", 
                  mMpRequest.mpReqData.stream.seqNum, 
                  mMpRequest.mpReqData.stream.circuitId,
                  IsdnUtil::ConvertCmdToStr(mMpRequest.cmdType));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_HAND05, lLogText);
         }
   }

   return lRet;
}

//----------------------------------------------------------------------
// METHOD NAME : IssuePlayStop
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::IssuePlayStop()
{
   BOOLEAN              lRet = false;
   MpResponseDetails    lMpRsp;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lRet = mIsdnDevice.Stop(mMpRequest.mpReqData.stopMsg.seqNum,  
         mMpRequest.mpReqData.stopMsg.circuitId);

   lMpRsp.cmdType             = MC_DISTN;
   lMpRsp.mpRspData.stopMsg   = mMpRequest.mpReqData.stopMsg;

   if ( true != lRet)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d Stop Cmd Issue Failed",
            mMpRequest.mpReqData.stopMsg.seqNum,
            mMpRequest.mpReqData.stopMsg.circuitId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND08, lLogText);

      lMpRsp.voiceProcessErr     = MP_PLAY_STOP_CMD_ERR;
      lMpRsp.rspMsg              = MP_RSP_FAILURE;

      mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d Stop Cmd Issue Success",
            mMpRequest.mpReqData.stopMsg.seqNum,
            mMpRequest.mpReqData.stopMsg.circuitId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND09, lLogText);

      lMpRsp.voiceProcessErr     = MP_SUCCESS;
      lMpRsp.rspMsg              = MP_RSP_SUCCESS;

      if(true == mIsdnConfigApi.mSendRspFlag)
         mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
   }

   return lRet;
}

//----------------------------------------------------------------------
// METHOD NAME : IssueGetDigitStop
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::IssueGetDigitStop()
{
   BOOLEAN              lRet = false;
   MpResponseDetails    lMpRsp;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lRet = mIsdnDevice.Stop(mMpRequest.mpReqData.stopMsg.seqNum,
         mMpRequest.mpReqData.stopMsg.circuitId);

   lMpRsp.cmdType             = MC_GETDIGSTP;
   lMpRsp.mpRspData.stopMsg   = mMpRequest.mpReqData.stopMsg;

   if(true != lRet)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d CancelGetDtmfDigits Cmd Issue Failed",
            mMpRequest.mpReqData.stopMsg.seqNum,
            mMpRequest.mpReqData.stopMsg.circuitId);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND16, lLogText);

      lMpRsp.voiceProcessErr     = MP_CANCEL_GET_DIG_CMD_ERR;
      lMpRsp.rspMsg              = MP_RSP_FAILURE;
      mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d CancelGetDtmfDigits Cmd Issue success",
            mMpRequest.mpReqData.stopMsg.seqNum,
            mMpRequest.mpReqData.stopMsg.circuitId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND17, lLogText);

      lMpRsp.voiceProcessErr     = MP_SUCCESS;
      lMpRsp.rspMsg              = MP_RSP_SUCCESS;

      if(true == mIsdnConfigApi.mSendRspFlag)
         mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
   }

   return lRet;
}

//----------------------------------------------------------------------
// METHOD NAME : IssueRecordStop
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnHandler::IssueRecordStop()
{
   BOOLEAN              lRet = false;
   MpResponseDetails    lMpRsp;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lRet = mIsdnDevice.Stop(mMpRequest.mpReqData.stopMsg.seqNum,
         mMpRequest.mpReqData.stopMsg.circuitId);

   lMpRsp.cmdType             = MC_RECSTP;
   lMpRsp.mpRspData.stopMsg   = mMpRequest.mpReqData.stopMsg;

   if(true != lRet )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d StopRecord Cmd Issue Failed",
            mMpRequest.mpReqData.stopMsg.seqNum,
            mMpRequest.mpReqData.stopMsg.circuitId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND20, lLogText);

      lMpRsp.voiceProcessErr     = MP_RECORD_STOP_CMD_ERR;
      lMpRsp.rspMsg              = MP_RSP_FAILURE;
      mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d StopRecord Cmd Issue Success",
            mMpRequest.mpReqData.stopMsg.seqNum,
            mMpRequest.mpReqData.stopMsg.circuitId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_HAND21, lLogText);

      lMpRsp.voiceProcessErr     = MP_SUCCESS;
      lMpRsp.rspMsg              = MP_RSP_SUCCESS;

      if(true == mIsdnConfigApi.mSendRspFlag)
         mIsdnQueApi.WriteToMediaCmdRspQue(lMpRsp);
   }

   return lRet;
}
