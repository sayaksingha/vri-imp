static const char *id = "@(#) $Id: IsdnDevice.cc,v 1.1.4.3 2016/04/12 21:59:36 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       IsdnDevice.cc
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
// AUTHOR         : Chetan V M                 25/07/2013
//----------------------------------------------------------------------
#include "IsdnDevice.h"

pthread_mutex_t Chnl_mutex[ISDN_MAX_GC_DEVICE];
pthread_mutex_t Call_mutex[ISDN_MAX_GC_DEVICE];


//----------------------------------------------------------------------
// METHOD NAME : IsdnDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnDevice::IsdnDevice()
{
   memset(mIsdnChannel, 0, sizeof(mIsdnChannel));
   memset(mScheduledConnect, 0, sizeof(mScheduledConnect));
   mpIsdnChnlStatus = NULL;
}

//----------------------------------------------------------------------
// METHOD NAME : ~IsdnDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnDevice::~IsdnDevice()
{  
}

//----------------------------------------------------------------------
// METHOD NAME : DeInit
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::DeInit()
{
   CloseDevices();

   return DeInitGc();
}

//----------------------------------------------------------------------
// METHOD NAME : Init()
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::Init(UINT16 minCkt, UINT16 maxCkt)
{
   mMinCkt = minCkt;
   mMaxCkt = maxCkt;

   if(true != mIsdnConfigApi.Init())
   {
      return false;
   }
   if(true != InitSharedMem())
   {
      return false;
   }

   if(true != GetDeviceInfo())
   {
      return false;
   }

   PrintDevice();

   
   if(true != InitGc())
   {
      return false;
   }

   if(mNumOfDxDevice > ISDN_MAX_DX_DEVICE || mNumOfDtiDevice > ISDN_MAX_DTI_DEVICE)
   {
      TERR (gTrace, 
            printf ("%s: Device in System is Greater than Process Can Handle Dx:%d MaxDx:%d Dti:%d MaxDti:%d\n",
               gProcessName, mNumOfDxDevice, ISDN_MAX_DX_DEVICE, mNumOfDtiDevice, ISDN_MAX_DTI_DEVICE););
      return false;
   }

   if(maxCkt > mNumOfDxDevice || maxCkt > mNumOfDtiDevice)
   {
      TERR (gTrace, printf ("%s: MaxCkt:%d Greater then Device Available Dx:%d Dti:%d\n",
               gProcessName, maxCkt, mNumOfDxDevice, mNumOfDtiDevice););
      return false;
   }
   
   if(true != OpenDevices())
   {
      return false;
   }

   mCountDevHdlsForSrWait = 0;
   for(UINT32 lIdx = mMinCkt; lIdx <= mMaxCkt; lIdx++)
   {
      mDevHdlsForSrWait[mCountDevHdlsForSrWait++] = mIsdnChannel[lIdx].chnlHandle;
      mDevHdlsForSrWait[mCountDevHdlsForSrWait++] = mIsdnDx[mIsdnChannel[lIdx].indxIsdnDx].dxHandle;
   }

   T (gTrace, printf ("%s: Srl Poll Device Count:%d\n", gProcessName, mCountDevHdlsForSrWait););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : InitGc
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::InitGc()
{
   GC_INFO  lErInfo;
   TEXT     lLogText[MAX_LOG_TEXT_LEN]  = "";

   if(gc_Start(NULL) != GC_SUCCESS)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Error In Starting Global Call Libraries ErrVl:%d Err:%s",
            lErInfo.gcValue, lErInfo.gcMsg);
      TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "Starting Global Call Libraries");
   T  (gTrace, printf("%s: %s\n",gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DeInitGc
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::DeInitGc()
{
   GC_INFO  lErInfo;
   TEXT     lLogText[MAX_LOG_TEXT_LEN]  = "";

   if(gc_Stop() != GC_SUCCESS)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Error In Stop Global Call Libraries ErrVl:%d Err:%s",
            lErInfo.gcValue, lErInfo.gcMsg);
      TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "Stoping Global Call Libraries");
   T  (gTrace, printf("%s: %s\n",gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : InitSharedMem
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOELAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::InitSharedMem()
{
   UINT32           lSharedMemSize = 0;
   BOOLEAN          lShmCreated    = false;
   TEXT             lLogText[MAX_LOG_TEXT_LEN] = "";
   
   lSharedMemSize = (ISDN_MAX_GC_DEVICE) * sizeof(ChannelStatus);

   mShmChnlMgmtId = shmget (mIsdnConfigApi.mShmChnlMgmtKey, lSharedMemSize, IPC_PERM);
   snprintf (lLogText,MAX_LOG_TEXT_LEN, "ShmChnlMgmtKey:%u Size:%u", 
         mIsdnConfigApi.mShmChnlMgmtKey, lSharedMemSize);
   if (mShmChnlMgmtId < 0)
   {
      // Share memory Not present. Creating....
      mShmChnlMgmtId = shmget (mIsdnConfigApi.mShmChnlMgmtKey, lSharedMemSize, IPC_PERM | IPC_CREAT);
      if (mShmChnlMgmtId < 0)
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

   
   mpIsdnChnlStatus = (ChannelStatus*) shmat (mIsdnConfigApi.mShmChnlMgmtKey, (void *) 0, 0);
   if (NULL == mpIsdnChnlStatus)
   {
      TERR (gTrace, printf ("%s: %s Error Attaching to Shared Memory\n", gProcessName, lLogText););
      return false;
   }
   T (gTrace, printf ("%s: %s Shared Memory Attached\n", gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetSrEvent
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::GetSrEvent(METAEVENT &metaevent)
{
   int      lRet;
   long     lEvent_handlep = 0;
   GC_INFO  lErInfo;
   TEXT     lLogText[MAX_LOG_TEXT_LEN]  = "";

   //lRet = sr_waitevt(500);            

   lRet = sr_waitevtEx(mDevHdlsForSrWait, mCountDevHdlsForSrWait, 1000, &lEvent_handlep);
   
   if(lRet < 0)
   {
      //Error Occured
      if(ESR_NOERR != ATDV_LASTERR(SRL_DEVICE))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "Error Occured Ret:%ld Err:%d", ATDV_LASTERR(SRL_DEVICE), lRet);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_DEVICE31, lLogText);
      }
      return false;
   }

   if(0 > gc_GetMetaEvent(&metaevent))
   {
      GetGcErrInfo(lErInfo);
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Get gc_GetMetaEvent Failed ErrVl:%d Err:%s",
            lErInfo.gcValue, lErInfo.gcMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE32, lLogText);
      return false;
   }


   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : MapVoiceDevHdl
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
INT32 IsdnDevice::MapVoiceDevHdl(int devHdl)
{
   for(UINT32 lIdx = mMinCkt; lIdx <= mMaxCkt; lIdx++)
   {
      if(devHdl == mIsdnDx[lIdx].dxHandle)
         return mIsdnDx[lIdx].gcIndex;
   }

   return 0;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessVoiceEvents
// DESCRIPTION : Destructor
// PARAMETERS  : long, int
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::ProcessVoiceEvents(long evtTyp, int devHdl, 
      MpResponseDetails &mpRsp)
{
   INT32       lCicIdx = 0;
   TEXT        lLogText[MAX_LOG_TEXT_LEN]  = "";

   lCicIdx = MapVoiceDevHdl(devHdl);
   if(0 > lCicIdx || 0 > mIsdnChannel[lCicIdx].cic || lCicIdx != mIsdnChannel[lCicIdx].cic)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Evt:%ld DeH:%d idx:%d cic:%d Unable to Map dxDevice to CIC",
            evtTyp, devHdl, lCicIdx, mIsdnChannel[lCicIdx].cic);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "CIC:%03d Evt:%s EvtId:%ld DeH:%d Voice Evt Received", 
         mIsdnChannel[lCicIdx].cic, IsdnUtil::ConvertIsdnVoiceEventToStr(evtTyp), evtTyp, devHdl);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE23, lLogText);

   switch(evtTyp)
   {
      case TDX_PLAY:
         {
            return HandlePlayStopped(lCicIdx, mpRsp);
         }
         break;
      case TDX_RECORD:
         {
            return HandleRecordStopped(lCicIdx, mpRsp);
         }
         break;
      case TDX_GETDIG:
         {
            return HandleGetDigitStopped(lCicIdx, mpRsp);
         }
         break;
      case TDX_ERROR:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d ErrVl:%ld Err:%s Rcv TDX_ERROR",
                  mIsdnChannel[lCicIdx].cic, devHdl, ATDV_LASTERR(devHdl), ATDV_ERRMSGP(devHdl));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE24, lLogText);

            return HandleTdxError(lCicIdx, mpRsp);
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "CIC:%03d DeviceHandle:%d not processed for this Event:0x%lx", 
                  mIsdnChannel[lCicIdx].cic, devHdl, evtTyp);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE01, lLogText);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : HandleGetDigitStopped
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::HandleGetDigitStopped(INT32 cicIdx, MpResponseDetails &rsp)
{
   INT32            lDxIndex = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32            lCic     = mIsdnChannel[cicIdx].cic;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   INT32            lSeq     = rIsdnVD.requestDetails.mpReqData.playMsg.seqNum;
   INT16            lDigitLen   = 0;
   long             lGetDigitStpRes             = -1;
   TEXT             lLogText[MAX_LOG_TEXT_LEN]  = "";

   lGetDigitStpRes = ATDX_TERMMSK(mIsdnDx[lDxIndex].dxHandle);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC:%03d DxH:%d Rcvd GetDigit Stop Event Reason:%s CS:%s NRS:%s", 
         lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, IsdnUtil::VoiceTermReason(lGetDigitStpRes),
         IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status), 
         IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE02, lLogText);

   pthread_mutex_lock(&Chnl_mutex[cicIdx]);  
   EnumIsdnVoiceChannelState lVCst = mChannelStatus[cicIdx].voiceChannel.status;
   pthread_mutex_unlock(&Chnl_mutex[cicIdx]);  

   switch(lVCst)
   {
      case ISDN_VOICE_CHANNEL_IDLE:
      case ISDN_VOICE_CHANNEL_PLAYING:
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d was not waiting for digits CS:%d",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE03, lLogText);
         }
         break;
      case ISDN_VOICE_CHANNEL_GET_DTMF:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d GetDigit Stopped CS:%d NRS:%s", 
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, mChannelStatus[cicIdx].voiceChannel.status,
                  IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););


            if(ISDN_NEXT_IDLE != rIsdnVD.lChnlReqStatus)
            {
               return HandleNextRequest(cicIdx, rsp);
            }
            else
            {
               rsp.rspMsg                       = MP_RSP_SUCCESS;
               rsp.cmdType                      = MC_GETDIGSTP;
               rsp.mpRspData.getDigit           = rIsdnVD.requestDetails.mpReqData.getDigit;

               lDigitLen = strlen(mIsdnDx[lDxIndex].digits.dg_value);

               if(0 == lDigitLen)
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                        "RefNum:%08X CIC:%03d DxH:%d GetDigit Stopped But No Digit Present In Buffer",
                        lSeq, lCic, mIsdnDx[lDxIndex].dxHandle);
               else
               {
                  if(0 == rIsdnVD.requestDetails.mpReqData.getDigit.includeTerminateChar && 
                        TM_DIGIT == lGetDigitStpRes)
                  {
                     strncpy(rsp.mpRspData.getDigit.rspDigits, 
                           mIsdnDx[lDxIndex].digits.dg_value, lDigitLen - 1);
                     rsp.mpRspData.getDigit.rspDigits[lDigitLen - 1] = '\0';
                  }
                  else
                  {
                     strcpy(rsp.mpRspData.getDigit.rspDigits, mIsdnDx[lDxIndex].digits.dg_value);
                  }

                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "RefNum:%08X CIC:%03d DxH:%d ITC:%d Digits:%s GetDigit Stopped",
                        lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
                        rIsdnVD.requestDetails.mpReqData.getDigit.includeTerminateChar,
                        rsp.mpRspData.getDigit.rspDigits);
               }
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE04, lLogText);

               ChannelReset(cicIdx);
               return true;

            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d Invalid Dx Channel Status",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE05, lLogText);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : HandleRecordStopped
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::HandleRecordStopped(INT32 cicIdx, MpResponseDetails & rsp)
{
   long             lRecordStpRes = -1;
   INT32            lDxIndex = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32            lCic     = mIsdnChannel[cicIdx].cic;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   INT32            lSeq     = rIsdnVD.requestDetails.mpReqData.playMsg.seqNum;
   TEXT             lLogText[MAX_LOG_TEXT_LEN]  = "";

   lRecordStpRes = ATDX_TERMMSK(mIsdnDx[lDxIndex].dxHandle);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC:%03d DxH:%d Rcvd Record Stop Event Reason:%s CS:%s NRS:%s", 
         lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, IsdnUtil::VoiceTermReason(lRecordStpRes), 
         IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status), 
         IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE06, lLogText);

   switch(mChannelStatus[cicIdx].voiceChannel.status)
   {
      case ISDN_VOICE_CHANNEL_IDLE:
      case ISDN_VOICE_CHANNEL_PLAYING:
      case ISDN_VOICE_CHANNEL_GET_DTMF:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d was not recording CS:%d",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE03, lLogText);
         }
         break;
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
         {
            //close file open for recording 
            CloseDxIOFiles(lDxIndex, 1);

            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d Record Stopped CS:%d NRS:%s", 
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, mChannelStatus[cicIdx].voiceChannel.status,
                  IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

            if(ISDN_NEXT_IDLE != rIsdnVD.lChnlReqStatus)
            {
               return HandleNextRequest(cicIdx, rsp);
            }
            else
            {

               rsp.rspMsg                       = MP_RSP_SUCCESS;
               rsp.cmdType                      = MC_RECSTP;
               rsp.mpRspData.stopMsg.seqNum     = rIsdnVD.requestDetails.mpReqData.recordMsg.seqNum;
               rsp.mpRspData.stopMsg.circuitId  = rIsdnVD.requestDetails.mpReqData.recordMsg.circuitId;

               ChannelReset(cicIdx);
               return true;
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d Invalid Dx Channel Status",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE05, lLogText);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : HandlePlayStopped
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::HandlePlayStopped(INT32 cicIdx, MpResponseDetails & rsp)
{
   INT32            lDxIndex = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32            lCic     = mIsdnChannel[cicIdx].cic;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   INT32            lSeq     = rIsdnVD.requestDetails.mpReqData.playMsg.seqNum;
   long             lPlayStpRes                 = -1;
   TEXT             lLogText[MAX_LOG_TEXT_LEN]  = "";

   lPlayStpRes = ATDX_TERMMSK(mIsdnDx[lDxIndex].dxHandle);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC:%03d DxH:%d Rcvd Play Stoped Event Reason:%s CS:%s NRS:%s Repeat:%d", 
         lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, IsdnUtil::VoiceTermReason(lPlayStpRes),
         IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status), 
         IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus), 
         rIsdnVD.requestDetails.mpReqData.playMsg.repeatPlay);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE07, lLogText);


   switch(mChannelStatus[cicIdx].voiceChannel.status)
   {
      case ISDN_VOICE_CHANNEL_IDLE:
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
      case ISDN_VOICE_CHANNEL_GET_DTMF:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d was not playing CS:%d",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE03, lLogText);
         }
         break;

      case ISDN_VOICE_CHANNEL_PLAYING:
         {
            //Close opened File for playing
            CloseDxIOFiles(lDxIndex, rIsdnVD.requestDetails.mpReqData.playMsg.vFileCount);

            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d Rcvd Play Stop Event CS:%d NRS:%s Repeat:%d NonBl:%d", 
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, mChannelStatus[cicIdx].voiceChannel.status, 
                  IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus), 
                  rIsdnVD.requestDetails.mpReqData.playMsg.repeatPlay,
                  rIsdnVD.requestDetails.mpReqData.playMsg.nonBlocking);
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            
            if(1 == rIsdnVD.requestDetails.mpReqData.playMsg.repeatPlay && TM_EOD == lPlayStpRes &&
                  ISDN_NEXT_IDLE == rIsdnVD.lChnlReqStatus)
            {
               if(true == Play(rIsdnVD.requestDetails.mpReqData.playMsg, lDxIndex))
               {
                  pthread_mutex_lock(&Chnl_mutex[cicIdx]);
                  mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_PLAYING;
                  pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

                  return false;;
               }
               else
               {
                  //Negative Response AS play Failed while repeating, 
                  //No reponse if play Stoped of NonBlocking Req
                  if(true == rIsdnVD.requestDetails.mpReqData.playMsg.nonBlocking)
                  {
                     ChannelReset(cicIdx);
                     return false;
                  }
                  else
                  {
                     rsp.rspMsg              = MP_RSP_FAILURE;
                     rsp.cmdType             = MC_CNTTN;
                     rsp.voiceProcessErr     = MP_PLAY_CMD_ERR;
                     rsp.mpRspData           = rIsdnVD.requestDetails.mpReqData;

                     ChannelReset(cicIdx);
                     return true;
                  }
               }
            }
            else
            {
               if(ISDN_NEXT_IDLE != rIsdnVD.lChnlReqStatus)
               {
                   return HandleNextRequest(cicIdx, rsp);
               }
               else
               {
                  //No reponse if play Stoped of NonBlocking Req
                  if(true == rIsdnVD.requestDetails.mpReqData.playMsg.nonBlocking)
                  {
                     ChannelReset(cicIdx);
                     return false;
                  }
                  else
                  {
                     rsp.rspMsg                       = MP_RSP_SUCCESS;
                     rsp.cmdType                      = MC_DISTN;
                     rsp.mpRspData.stopMsg.seqNum     = rIsdnVD.requestDetails.mpReqData.playMsg.seqNum;
                     rsp.mpRspData.stopMsg.circuitId  = rIsdnVD.requestDetails.mpReqData.playMsg.circuitId;

                     ChannelReset(cicIdx);
                     return true;
                  }
               }
            }
         }
         break;
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
         {
            //Close opened File for playing
            CloseDxIOFiles(lDxIndex, rIsdnVD.requestDetails.mpReqData.playMsg.vFileCount);

            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d Play Stopped CS:VOICE_CHNL_PLAY_STOPING NRS:%s NonBl:%d", 
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
                  IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus),
                  rIsdnVD.requestDetails.mpReqData.playMsg.nonBlocking);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE25, lLogText);
            
            if(ISDN_NEXT_IDLE != rIsdnVD.lChnlReqStatus)
            {
               return HandleNextRequest(cicIdx, rsp);
            }
            else
            {
               //No reponse if play Stoped of NonBlocking Req
               if(true == rIsdnVD.requestDetails.mpReqData.playMsg.nonBlocking)
               {
                  ChannelReset(cicIdx);
                  return false;
               }
               else
               {
                  rsp.rspMsg                       = MP_RSP_SUCCESS;
                  rsp.cmdType                      = MC_DISTN;
                  rsp.mpRspData.stopMsg.seqNum     = rIsdnVD.requestDetails.mpReqData.playMsg.seqNum;
                  rsp.mpRspData.stopMsg.circuitId  = rIsdnVD.requestDetails.mpReqData.playMsg.circuitId;

                  ChannelReset(cicIdx);
                  return true;
               }
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d Invalid Dx Channel Status",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE05, lLogText);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : GetDeviceInfo
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::GetDeviceInfo()
{
   AUID*          lPhysicalBoard;
   long           lRetVal;
   int            lNumPylBds  = 0;
   int            lLen        = 0;
   TEXT           lPhyBrdName[ISDN_MAX_BOARD_NAME_LEN] = "";

   memset(&mDeviceInfo, 0, sizeof(DeviceInfo));

   lRetVal = SRLGetAllPhysicalBoards(&lNumPylBds, 0);
   if(ESR_INSUFBUF == lRetVal)
   {
      if(lNumPylBds != 0)
         lPhysicalBoard = (AUID *)malloc(lNumPylBds * sizeof(AUID));

      if(lPhysicalBoard)
      {
         lRetVal = SRLGetAllPhysicalBoards(&lNumPylBds,lPhysicalBoard);
         if(ESR_NOERR == lRetVal)
         {
            mDeviceInfo.numOfPhysicalBoards = lNumPylBds;
         }
         else
         {
            TERR (gTrace, printf("%s: SRLGetAllPhysicalBoards Error .....\n", 
                     gProcessName););
            return false;
         }
      }
   }
   else
   {
      TERR (gTrace, printf("%s: SRLGetAllPhysicalBoards Error .....\n", 
               gProcessName););
      return false;
   }

   for(int lBrdCnt = 1;lBrdCnt <= lNumPylBds; lBrdCnt++)
   {
      lLen = ISDN_MAX_BOARD_NAME_LEN;

      SRLGetPhysicalBoardName(lPhysicalBoard[lBrdCnt-1], &lLen, lPhyBrdName);

      mDeviceInfo.physicalBoard[lBrdCnt].physicalBoradIndex = lBrdCnt ;
      strcpy(mDeviceInfo.physicalBoard[lBrdCnt].nameofPhysicalBoards, lPhyBrdName);

      if(VirtualBoardCount(lPhysicalBoard[lBrdCnt-1], lBrdCnt) == false)
      {
         TERR(gTrace, printf("%s: No Virtual Borads Vailable on Physical Board: %d\n", 
                  gProcessName, lBrdCnt););
      }
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : VirtualBoardCount
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::VirtualBoardCount(AUID PhyBd, int brdCnt)
{
   int                 lNumVB = 0;
   long                lRetVal;
   AUID                lPhyBd;
   SRLDEVICEINFO       *lVirBrdPtr;

   lPhyBd = PhyBd;

   lRetVal = SRLGetVirtualBoardsOnPhysicalBoard(lPhyBd, &lNumVB, 0);
   if(ESR_INSUFBUF == lRetVal)
   {
      lVirBrdPtr = (SRLDEVICEINFO *)malloc(lNumVB * sizeof(SRLDEVICEINFO));
      if(lVirBrdPtr)
      {
         lRetVal = SRLGetVirtualBoardsOnPhysicalBoard(lPhyBd, &lNumVB, lVirBrdPtr);
         if(ESR_NOERR == lRetVal)
         {
            mDeviceInfo.physicalBoard[brdCnt].numberOfVirtualBoard = lNumVB;
         }
      }
      for(int lVbCnt = 1;lVbCnt <= lNumVB;lVbCnt++)
      {
         mDeviceInfo.physicalBoard[brdCnt].virtualBoard[lVbCnt].virtualBoardIndex = lVbCnt;
         strcpy(mDeviceInfo.physicalBoard[brdCnt].virtualBoard[lVbCnt].virtualBoardName, lVirBrdPtr -> szDevName);
         mDeviceInfo.physicalBoard[brdCnt].virtualBoard[lVbCnt].virtualBoardType = lVirBrdPtr -> iDevType;

         if(Subdevices(lVirBrdPtr -> szDevName, brdCnt, lVbCnt) == false)
         {
            TERR(gTrace, printf("%s: No Subdevices Avilable on Virtual Board: %d\n", 
                     gProcessName, lVbCnt););
         }
         lVirBrdPtr++;
      }
   }
   else
   {
      TERR(gTrace, printf("%s: No SubDevices on Any Virtual Board\n", 
               gProcessName););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Subdevices
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::Subdevices(TEXT* VBName, int brdCnt, int vbCnt)
{
   char             szVB[12];
   int              lNumSDs = 0;
   long             lRetVal;
   SRLDEVICEINFO    *lVBPtr;

   strcpy(szVB, VBName);

   lRetVal = SRLGetSubDevicesOnVirtualBoard(szVB, &lNumSDs, 0);
   if (ESR_INSUFBUF == lRetVal )
   {
      lVBPtr = (SRLDEVICEINFO *)malloc(lNumSDs * sizeof(*lVBPtr));
      if (0 != lVBPtr)
      {
         lRetVal = SRLGetSubDevicesOnVirtualBoard(szVB, &lNumSDs, lVBPtr);
         if (ESR_NOERR == lRetVal)
         {
            mDeviceInfo.physicalBoard[brdCnt].virtualBoard[vbCnt].numberOfSubDevices = lNumSDs;
         }
      }
      else
      {
         TERR (gTrace, printf("%s: No Sub Devices Available\n", gProcessName););
         return false;
      }
      for(int lSdCnt = 1;lSdCnt <= lNumSDs;lSdCnt++)
      {
         mDeviceInfo.physicalBoard[brdCnt].virtualBoard[vbCnt].subDevice[lSdCnt].subDeviceIndex = lSdCnt ;
         strcpy(mDeviceInfo.physicalBoard[brdCnt].virtualBoard[vbCnt].subDevice[lSdCnt].subDeviceName, lVBPtr->szDevName);
         mDeviceInfo.physicalBoard[brdCnt].virtualBoard[vbCnt].subDevice[lSdCnt].subDeviceType = lVBPtr->iDevType;

         lVBPtr++;
      }
   }
   else
   {
      TERR (gTrace, printf("%s: No Sub Devices Available on Any Virtual Board\n", 
               gProcessName););
      return false;
   }
   return true;

}

//----------------------------------------------------------------------
// METHOD NAME : PrintDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
void IsdnDevice::PrintDevice()
{
   int  lIdx                                  = 0;
   int  lPrevDtiSubDevCount                   = 0;
   char lTmpDName[ISDN_DEVICE_NAME_LENGTH]    = "";
   char lTmpDVBrdNam[ISDN_DEVICE_NAME_LENGTH] = "";

   T(gTrace, printf("Number Of Physical Boards : %d\n", mDeviceInfo.numOfPhysicalBoards););
   for(int idxP = 1; idxP <= mDeviceInfo.numOfPhysicalBoards; idxP++)
   {
      T(gTrace, printf("\tPhysical Board : %d\n", mDeviceInfo.physicalBoard[idxP].physicalBoradIndex););
      T(gTrace, printf("\tPhysical Board Name : %s\n", mDeviceInfo.physicalBoard[idxP].nameofPhysicalBoards););
      T(gTrace, printf("\tNumber Of Virtual Board : %d\n", mDeviceInfo.physicalBoard[idxP].numberOfVirtualBoard););
      for(int idxV = 1; idxV <= mDeviceInfo.physicalBoard[idxP].numberOfVirtualBoard; idxV++)
      {
         T(gTrace, printf("\t\tVirtual Board : %d\n", mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].virtualBoardIndex););
         T(gTrace, printf("\t\tVirtual Board Name : %s\n", mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].virtualBoardName););
         T(gTrace, printf("\t\tVirtual Board Name : %d\n", mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].virtualBoardType););
         T(gTrace, printf("\t\tNumber Of subDevices : %d\n", mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].numberOfSubDevices););
         for(int idxS = 1; idxS <= mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].numberOfSubDevices; idxS++)
         {
            T(gTrace, printf("\t\t\tSubDevice : %d\n", mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].subDevice[idxS].subDeviceIndex););
            T(gTrace, printf("\t\t\tSubDevice Name : %s\n", mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].subDevice[idxS].subDeviceName););
            T(gTrace, printf("\t\t\tSubDevice Name : %d\n",mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].subDevice[idxS].subDeviceType););
            switch(mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].subDevice[idxS].subDeviceType)
            {
               case 502: //ISDN_VOICE_DEV
                  mNumOfDxDevice++;
                  mIsdnDx[mNumOfDxDevice].index = mNumOfDxDevice;
                  strcpy(mIsdnDx[mNumOfDxDevice].dxDeviceName, mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].subDevice[idxS].subDeviceName);
                  break;
               case 504: //LINE_DEV
                  mNumOfDtiDevice++;
                  strcpy(lTmpDVBrdNam, mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].virtualBoardName + strlen("dtiB"));
                  strcpy(lTmpDName, mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].subDevice[idxS].subDeviceName + strlen(mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].virtualBoardName) + 1);
                  lIdx = atoi(lTmpDName)+ (lPrevDtiSubDevCount * (atoi(lTmpDVBrdNam) - 1));
                  mIsdnDti[lIdx].index = mNumOfDtiDevice;
                  strcpy(mIsdnDti[lIdx].dtiDeviceName, mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].subDevice[idxS].subDeviceName);
                  break;
               default:
                  T(gTrace, printf("-----------------Sub Device Unknown---------------------\n"););
            }
         }
         if(503 == mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].virtualBoardType)
         {
            lPrevDtiSubDevCount = mDeviceInfo.physicalBoard[idxP].virtualBoard[idxV].numberOfSubDevices;
         }
      }
   }
   T (gTrace, printf("%s: Voice Device Count: %d Line Device Count: %d\n",
            gProcessName, mNumOfDxDevice, mNumOfDtiDevice););
}

//----------------------------------------------------------------------
// METHOD NAME : OpenDevices
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::OpenDevices()
{
   GC_INFO lErInfo;
   int neworkHandle                = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = mMinCkt; lIdx <= mMaxCkt; lIdx++)
   {
      snprintf(mIsdnChannel[lIdx].chnlDeviceName, ISDN_GC_DEVICE_NAME_LENGTH, 
            ":N_%s:P_ISDN",  mIsdnDti[lIdx].dtiDeviceName);
      if (gc_OpenEx((long int*)&mIsdnChannel[lIdx].chnlHandle, mIsdnChannel[lIdx].chnlDeviceName, 
               EV_SYNC, (void *)&mIsdnChannel[lIdx]) != GC_SUCCESS)
      {
         GetGcErrInfo(lErInfo);
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d Failed gc_OpenEx DeN:%s ErrVl:%d Err:%s",
               lIdx, mIsdnChannel[lIdx].chnlDeviceName, lErInfo.gcValue, lErInfo.gcMsg);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););

         if(0 < mIsdnChannel[lIdx].chnlHandle)
            gc_Close(mIsdnChannel[lIdx].chnlHandle);
         return false;
      }
      mIsdnChannel[lIdx].cic = lIdx;
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d gc_OpenEx Succ DeNe:%s DeH:%d",
         lIdx, mIsdnChannel[lIdx].chnlDeviceName, mIsdnChannel[lIdx].chnlHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

      if(gc_GetResourceH(mIsdnChannel[lIdx].chnlHandle, &neworkHandle, GC_NETWORKDEVICE) < 0)
      {
         GetGcErrInfo(lErInfo);
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "CIC:%03d Failed gc_GetResourceH DeN:%s ErrVl:%d Err:%s",
               lIdx, mIsdnChannel[lIdx].chnlDeviceName, lErInfo.gcValue, lErInfo.gcMsg);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););

         gc_Close(mIsdnChannel[lIdx].chnlHandle);
         return false;
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d gc_OpenEx Succ DeN:%s DeH:%d NwH:%d",
            lIdx, mIsdnChannel[lIdx].chnlDeviceName, mIsdnChannel[lIdx].chnlHandle, neworkHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      
      mIsdnDti[lIdx].dtiHandle = neworkHandle;
      mIsdnChannel[lIdx].indxIsdnDti = lIdx;
      if(true != OpenDxDevice(lIdx))
      {
         gc_Close(mIsdnChannel[lIdx].chnlHandle);
         return false;
      }
      mIsdnChannel[lIdx].indxIsdnDx = lIdx;
      mIsdnDx[lIdx].gcIndex = lIdx;

      if((nr_scroute(mIsdnDti[lIdx].dtiHandle, SC_DTI, mIsdnDx[lIdx].dxHandle, SC_VOX, SC_FULLDUP)) == -1)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "CIC:%03d Failed nr_scroute DeN:%s DtiN:%s DxN:%s DtiH:%d DxH:%d ErrVl:%ld Err:%s",
               lIdx, mIsdnChannel[lIdx].chnlDeviceName, mIsdnDti[lIdx].dtiDeviceName,
               mIsdnDx[lIdx].dxDeviceName, mIsdnDti[lIdx].dtiHandle, mIsdnDx[lIdx].dxHandle,
               ATDV_LASTERR(mIsdnDx[lIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[lIdx].dxHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         
         CloseDxDevice(lIdx);
         gc_Close(mIsdnChannel[lIdx].chnlHandle);
         return false;
      }

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CIC:%03d nr_scroute Succ DeN:%s DxN:%s",
            lIdx, mIsdnChannel[lIdx].chnlDeviceName, mIsdnDx[lIdx].dxDeviceName);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessEvent
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::ProcessEvent()
{
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Stop
// DESCRIPTION : 
// PARAMETERS  : INT32
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::Stop(INT32 seq, INT32 cicIdx)
{
   INT32       lSeq      = seq;
   INT32       lDxIndex  = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32       lCic    = mIsdnChannel[cicIdx].cic; 
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";


   if(lCic != cicIdx)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d cicIdx:%d Stop Not Matching Error in Internal CIC Maping",
            lSeq, lCic, cicIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return false;
   }


   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC:%03d DxH:%d CS:%s Recvd Stop",
         lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
         IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE08, lLogText);

   switch(mChannelStatus[cicIdx].voiceChannel.status)
   {
      case ISDN_VOICE_CHANNEL_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d CS:%s No Operation in Progress",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE09, lLogText);
         }
         break;
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d CS:%s Channel Already Stopping",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE09, lLogText);
         }
         break;
      case ISDN_VOICE_CHANNEL_PLAYING:
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d CS:%s Issuing Channel Stop",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE10, lLogText);

            pthread_mutex_lock(&Chnl_mutex[cicIdx]);
            if(true == StopChnlAct(lDxIndex))
            {
               if(ISDN_VOICE_CHANNEL_PLAYING == mChannelStatus[cicIdx].voiceChannel.status)
                  mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_PLAY_STOPING;
               else if(ISDN_VOICE_CHANNEL_RECORDING == mChannelStatus[cicIdx].voiceChannel.status)
                  mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_RECORD_STOPING;
               else if(ISDN_VOICE_CHANNEL_GET_DTMF == mChannelStatus[cicIdx].voiceChannel.status)
                  mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_GET_DTMF_STOPING;
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                        "RefNum:%08X CIC:%03d Abrupt Channel State Changed in Stop", lSeq, lCic);
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ISDN_DEVICE11, lLogText);
               }
               pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

               return true;
            }
            else
            {
               pthread_mutex_unlock(&Chnl_mutex[cicIdx]);
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "RefNum:%08X CIC:%03d DxH:%d CS:%s Stop Failed",
                     lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
                     IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE12, lLogText);
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d CS:%s Invalid Dx Channel Status",
                  lSeq, lCic, mIsdnDx[lDxIndex].dxHandle, 
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE05, lLogText);
         }
   }


   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : PlayFile
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : EnumIsdnProcRetType
//----------------------------------------------------------------------
EnumIsdnProcRetType IsdnDevice::PlayFile(MpRequestDetails &mpReqDetails)
{
   INT32       cicIdx  = mpReqDetails.mpReqData.playMsg.circuitId;
   INT32       lCic    = mIsdnChannel[cicIdx].cic; 
   INT32       lDx     = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32       lSeq    = mpReqDetails.mpReqData.playMsg.seqNum;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   if(lCic != cicIdx)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "RefNum:%08X CIC:%03d cicIdx:%d PlayFile Not Matching Error in Internal CIC Maping",
            lSeq, lCic, cicIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return ISDN_PROC_RETURN_FALSE;
   }


   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CIC:%03d Recvd Play CS:%s NonBl:%d", 
         lSeq, lCic, IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status),
            mpReqDetails.mpReqData.playMsg.nonBlocking);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE13, lLogText);

   switch(mChannelStatus[cicIdx].voiceChannel.status)
   {
      case ISDN_VOICE_CHANNEL_IDLE:
         {
            FillIsdnVoiceReq(rIsdnVD, mpReqDetails);
            pthread_mutex_lock(&Chnl_mutex[cicIdx]);
            if(true == Play(mpReqDetails.mpReqData.playMsg, lDx))
            {
               mChannelStatus[cicIdx].voiceChannel.dxHandleIdx    = lDx;
               mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_PLAYING;
               pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

               snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CIC:%03d DxH:%d CS:%d PlayFile Issued",
                     lSeq, lCic, mIsdnDx[lDx].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE49, lLogText);

               if( true == mpReqDetails.mpReqData.playMsg.nonBlocking)
                  return ISDN_PROC_RETURN_TRUE_WITH_RSP;
               else
                  return ISDN_PROC_RETURN_TRUE;
            }
            pthread_mutex_unlock(&Chnl_mutex[cicIdx]);
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d CS:%d Failed to Issue PlayFile",
                  lSeq, lCic, mIsdnDx[lDx].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE48, lLogText);
         }
         ChannelReset(cicIdx);
         break;
      case ISDN_VOICE_CHANNEL_PLAYING:
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d CS:%s Play, Hence Issuing Stop and Schedule Request", 
                  lSeq, lCic,
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE14, lLogText);

            if(true == StopAndScheduleRequest(lSeq, cicIdx, mpReqDetails, ISDN_NEXT_PLAY_REQUIRED))
               return ISDN_PROC_RETURN_TRUE;
         }
         break;
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d CS:%s Play, Hence Schedule Request", lSeq, lCic,
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE14, lLogText);

            rIsdnVD.lChnlReqStatus = ISDN_NEXT_PLAY_REQUIRED;
            rIsdnVD.nextReqDetails = mpReqDetails;
            return ISDN_PROC_RETURN_TRUE;
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d Dx Channel Invalid State", lSeq, lCic);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE05, lLogText);
         }

   }
   return ISDN_PROC_RETURN_FALSE;
}

//----------------------------------------------------------------------
// METHOD NAME : StopAndScheduleRequest
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::StopAndScheduleRequest(INT32 lSeq, INT32 cicIdx, MpRequestDetails &mpReqDetails, 
      EnumIsdnVoiceChannelReqStatus lChannelReqStatus)
{
   INT32       lCic   = mIsdnChannel[cicIdx].cic;
   INT32       lDx    = mIsdnChannel[cicIdx].indxIsdnDx;;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   rIsdnVD.lChnlReqStatus = lChannelReqStatus;
   rIsdnVD.nextReqDetails = mpReqDetails;

   pthread_mutex_lock(&Chnl_mutex[cicIdx]);
   if(true == StopChnlAct(lDx))
   {
      if(ISDN_VOICE_CHANNEL_PLAYING == mChannelStatus[cicIdx].voiceChannel.status)
         mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_PLAY_STOPING;
      else if(ISDN_VOICE_CHANNEL_RECORDING == mChannelStatus[cicIdx].voiceChannel.status)
         mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_RECORD_STOPING;
      else if(ISDN_VOICE_CHANNEL_GET_DTMF == mChannelStatus[cicIdx].voiceChannel.status)
         mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_GET_DTMF_STOPING;
      else
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "RefNum:%08X CIC:%03d Abrupt Channel State Changed in StopAndScheduleRequest", lSeq, lCic);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_DEVICE11, lLogText);
      }

      pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

      return true;
   }
   pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : RecordFile
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::RecordFile(MpRequestDetails &mpReqDetails)
{

   INT32       cicIdx  = mpReqDetails.mpReqData.recordMsg.circuitId;
   INT32       lCic    = mIsdnChannel[cicIdx].cic;
   INT32       lDx     = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32       lSeq    = mpReqDetails.mpReqData.recordMsg.seqNum;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   if(lCic != cicIdx)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d cicIdx:%d RecordFile Not Matching Error in Internal CIC Maping",
            lSeq, lCic, cicIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CIC:%03d Recvd Record CS:%s", 
         lSeq, lCic, IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE15, lLogText);

   switch(mChannelStatus[cicIdx].voiceChannel.status)
   {
      case ISDN_VOICE_CHANNEL_IDLE:
         {
            FillIsdnVoiceReq(rIsdnVD, mpReqDetails);
            pthread_mutex_lock(&Chnl_mutex[cicIdx]);
            if(true == Record(mpReqDetails.mpReqData.recordMsg, lDx))
            {

               mChannelStatus[cicIdx].voiceChannel.dxHandleIdx    = lDx;
               mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_RECORDING;
               pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

               snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CIC:%03d DxH:%d CS:%d Record Issued",
                     lSeq, lCic, mIsdnDx[lDx].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE49, lLogText);
               return true;
            }
            pthread_mutex_unlock(&Chnl_mutex[cicIdx]);
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d CS:%d Failed to Issue Record",
                  lSeq, lCic, mIsdnDx[lDx].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE48, lLogText);
         }
         ChannelReset(cicIdx);
         break;
      case ISDN_VOICE_CHANNEL_PLAYING:
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d CS:%s Record, Hence Issuing Stop and Schedule Request", 
                  lSeq, lCic,
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE14, lLogText);

            if(true == StopAndScheduleRequest(lSeq, cicIdx, mpReqDetails, ISDN_NEXT_RECORD_REQUIRED))
               return true;
         }
         break;
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d CS:%s Record, Hence Schedule Request", lSeq, lCic,
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE14, lLogText);

            lDx = mChannelStatus[cicIdx].voiceChannel.dxHandleIdx;

            rIsdnVD.lChnlReqStatus = ISDN_NEXT_RECORD_REQUIRED;
            rIsdnVD.nextReqDetails = mpReqDetails;
            return true;
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d Channel Invalid State", lSeq, lCic);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE15, lLogText);
         }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : FillIsdnVoiceReq
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::FillIsdnVoiceReq(IsdnVoiceDetails &isdnVD, MpRequestDetails &mpReqDetails)
{
   memset(&isdnVD, 0, sizeof(IsdnVoiceDetails));
   isdnVD.lChnlReqStatus = ISDN_NEXT_IDLE;
   isdnVD.requestDetails = mpReqDetails;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetDigit
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::GetDigit(MpRequestDetails &mpReqDetails)
{
   INT32       cicIdx  = mpReqDetails.mpReqData.getDigit.circuitId;
   INT32       lCic    = mIsdnChannel[cicIdx].cic;
   INT32       lDx     = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32       lSeq    = mpReqDetails.mpReqData.getDigit.seqNum;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   if(lCic != cicIdx)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC:%03d cicIdx:%d GetDigit Not Matching Error in Internal CIC Maping",
            lSeq, lCic, cicIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CIC:%03d Recvd GetDigit CS:%s", 
         lSeq, lCic, IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE16, lLogText);

   switch(mChannelStatus[cicIdx].voiceChannel.status)
   {
      case ISDN_VOICE_CHANNEL_IDLE:
         {
            FillIsdnVoiceReq(rIsdnVD, mpReqDetails);
            pthread_mutex_lock(&Chnl_mutex[cicIdx]);
            if(true == GetDigits(mpReqDetails.mpReqData.getDigit, lDx))
            {

               mChannelStatus[cicIdx].voiceChannel.dxHandleIdx    = lDx;
               mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_GET_DTMF;
               pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

               snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CIC:%03d DxH:%d CS:%d GetDigit Issued",
                     lSeq, lCic, mIsdnDx[lDx].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE49, lLogText);
               return true;
            }

            pthread_mutex_unlock(&Chnl_mutex[cicIdx]);
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d DxH:%d CS:%d Failed to Issue GetDigit",
                  lSeq, lCic, mIsdnDx[lDx].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE48, lLogText);
         }
         ChannelReset(cicIdx);
         break;
      case ISDN_VOICE_CHANNEL_PLAYING:
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d CS:%s GetDigit, Hence Issuing Stop and Schedule Request", 
                  lSeq, lCic,
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE14, lLogText);

            if(true == StopAndScheduleRequest(lSeq, cicIdx, mpReqDetails, ISDN_NEXT_GET_DTMF_REQUIRED))
               return true;
         }
         break;
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CIC:%03d CS:%s GetDigit, Hence Schedule Request", lSeq, lCic,
                  IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE14, lLogText);

            rIsdnVD.lChnlReqStatus = ISDN_NEXT_GET_DTMF_REQUIRED;
            rIsdnVD.nextReqDetails = mpReqDetails;
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CIC:%03d Channel Invalid State", lSeq, lCic);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE15, lLogText);
         }
   }

   return false;
}


//----------------------------------------------------------------------
// METHOD NAME : HandleNextRequest
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::HandleNextRequest(INT32 cicIdx, MpResponseDetails &rsp)
{
   INT32            lDxIndex = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32            lCic     = mIsdnChannel[cicIdx].cic;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   TEXT             lLogText[MAX_LOG_TEXT_LEN] = "";

   rsp.mpRspData           = rIsdnVD.nextReqDetails.mpReqData;

   if(ISDN_NEXT_PLAY_REQUIRED == rIsdnVD.lChnlReqStatus)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "RefNum:%08X CIC:%03d CS:%s NRS:%s NonBl:%d Hence Issue Play Request", 
            rIsdnVD.nextReqDetails.mpReqData.playMsg.seqNum, lCic,
            IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status),
            IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus),
            rIsdnVD.nextReqDetails.mpReqData.playMsg.nonBlocking);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE17, lLogText);

      rsp.cmdType             = MC_CNTTN;

      if(true == Play(rIsdnVD.nextReqDetails.mpReqData.playMsg, lDxIndex))
      {
         pthread_mutex_lock(&Chnl_mutex[cicIdx]);
         mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_PLAYING;
         pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

         rIsdnVD.lChnlReqStatus = ISDN_NEXT_IDLE;
         rIsdnVD.requestDetails = rIsdnVD.nextReqDetails;

         rsp.rspMsg              = MP_RSP_SUCCESS;
         rsp.voiceProcessErr     = MP_SUCCESS;


         if(true == rIsdnVD.nextReqDetails.mpReqData.playMsg.nonBlocking)
         {
            memset(&rIsdnVD.nextReqDetails, 0, sizeof(MpRequestDetails));
            return true; //send response when play is queued and only NonBlocking
         }
         else
         {
            memset(&rIsdnVD.nextReqDetails, 0, sizeof(MpRequestDetails));
            return false;
         }
      }

      rsp.voiceProcessErr     = MP_PLAY_CMD_ERR;
   }
   else if(ISDN_NEXT_RECORD_REQUIRED == rIsdnVD.lChnlReqStatus)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "RefNum:%08X CIC:%03d CS:%s NRS:%s, Hence Issue Record Request", 
            rIsdnVD.nextReqDetails.mpReqData.recordMsg.seqNum, lCic,
            IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status),
            IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE17, lLogText);

      rsp.cmdType              = MC_RECSTR;

      if(true == Record(rIsdnVD.nextReqDetails.mpReqData.recordMsg, lDxIndex))
      {
         pthread_mutex_lock(&Chnl_mutex[cicIdx]);
         mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_RECORDING;
         pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

         rIsdnVD.lChnlReqStatus = ISDN_NEXT_IDLE;
         rIsdnVD.requestDetails = rIsdnVD.nextReqDetails;

         memset(&rIsdnVD.nextReqDetails, 0, sizeof(MpRequestDetails));
         return false;
      }
      rsp.voiceProcessErr       = MP_RECORD_CMD_ERR;
   }
   else if(ISDN_NEXT_GET_DTMF_REQUIRED == rIsdnVD.lChnlReqStatus)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "RefNum:%08X CIC:%03d CS:%s NRS:%s, Hence Issue GetDigit Request", 
            rIsdnVD.nextReqDetails.mpReqData.getDigit.seqNum, lCic,
            IsdnUtil::ConvertVoiceChnlStatToStr(mChannelStatus[cicIdx].voiceChannel.status),
            IsdnUtil::ConvertNextReqStatToStr(rIsdnVD.lChnlReqStatus));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE17, lLogText);

      rsp.cmdType              = MC_GETDIG;

      if(true == GetDigits(rIsdnVD.nextReqDetails.mpReqData.getDigit, lDxIndex))
      {
         pthread_mutex_lock(&Chnl_mutex[cicIdx]);
         mChannelStatus[cicIdx].voiceChannel.status  = ISDN_VOICE_CHANNEL_GET_DTMF;
         pthread_mutex_unlock(&Chnl_mutex[cicIdx]);

         rIsdnVD.lChnlReqStatus = ISDN_NEXT_IDLE;
         rIsdnVD.requestDetails = rIsdnVD.nextReqDetails;

         memset(&rIsdnVD.nextReqDetails, 0, sizeof(MpRequestDetails));
         return false;
      }
      rsp.voiceProcessErr       = MP_GET_DIG_CMD_ERR;
   }

   rsp.rspMsg              = MP_RSP_FAILURE;
   ChannelReset(cicIdx); //Reset Channel As Cmd Req Failed

   return true; //Send Response As Cmd Req Failed
}

//----------------------------------------------------------------------
// METHOD NAME : CallInfoReset
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : None
//----------------------------------------------------------------------
void IsdnDevice::CallInfoReset(INT32 cicIdx)
{
   mIsdnChannel[cicIdx].crn = 0x00;
   memset(&mScheduledConnect[cicIdx], 0, sizeof(ScheduledConnect));
}

//----------------------------------------------------------------------
// METHOD NAME : ChannelReset
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
void IsdnDevice::ChannelReset(INT32 cicIdx)
{
   INT32            dxHdlIdx = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32            lCic     = mIsdnChannel[cicIdx].cic;
   IsdnVoiceDetails &rIsdnVD = mIsdnChannel[cicIdx].isdnVoiceDetails;
   TEXT             lLogText[MAX_LOG_TEXT_LEN]  = "";


   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "CIC:%03d DxIdx:%d Reset Voice Channel Info", 
         lCic, dxHdlIdx);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE50, lLogText);

   if(0 < dxHdlIdx)
   {
      memset(mIsdnDx[dxHdlIdx].iott, 0, sizeof(DX_IOTT) * ISDN_DX_MAXIMUM_FILES);
      memset(&mIsdnDx[dxHdlIdx].digits, 0, sizeof(DV_DIGIT));
   }

   if(0 < cicIdx)
   {
      pthread_mutex_lock(&Chnl_mutex[cicIdx]);
      memset(&mChannelStatus[cicIdx], 0, sizeof(ChannelStatus));
      pthread_mutex_unlock(&Chnl_mutex[cicIdx]);
   }

   memset(&rIsdnVD, 0, sizeof(IsdnVoiceDetails));
}


//----------------------------------------------------------------------
// METHOD NAME : GetGcErrInfo
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : void
//----------------------------------------------------------------------
void IsdnDevice::GetGcErrInfo(GC_INFO &errInfo)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(&errInfo, 0, sizeof(GC_INFO));
   if(gc_ErrorInfo(&errInfo) != GC_SUCCESS)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Get gc_ErrorInfo Failed");
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return;
   }
}

//----------------------------------------------------------------------
// METHOD NAME : GetGcResultInfo
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : void
//----------------------------------------------------------------------
void IsdnDevice::GetGcResultInfo(INT32 cic, METAEVENT *p_Metaevent, GC_INFO &errInfo)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(&errInfo, 0, sizeof(GC_INFO));
   if(0 > gc_ResultInfo(p_Metaevent, &errInfo))
   {
      GetGcErrInfo(errInfo);
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d Get gc_ResultInfo Failed ErrVl:%d Err:%s",
            cic, errInfo.gcValue, errInfo.gcMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return;
   }
}

//----------------------------------------------------------------------
// METHOD NAME : CloseDevices
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::CloseDevices()
{
   GC_INFO lErInfo;
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = mMinCkt; lIdx <= mMaxCkt; lIdx++)
   {
      ResetLineDevice(lIdx, EV_SYNC);

      if((nr_scunroute(mIsdnDti[lIdx].dtiHandle, SC_DTI, mIsdnDx[lIdx].dxHandle, SC_VOX, SC_FULLDUP)) == -1)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "CIC:%03d Failed nr_scunroute DeN:%s DtiN:%s DxN:%s DtiH:%d DxH:%d ErrVl:%ld Err:%s",
               lIdx, mIsdnChannel[lIdx].chnlDeviceName, mIsdnDti[lIdx].dtiDeviceName,
               mIsdnDx[lIdx].dxDeviceName, mIsdnDti[lIdx].dtiHandle, mIsdnDx[lIdx].dxHandle,
               ATDV_LASTERR(mIsdnDx[lIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[lIdx].dxHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      }

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CIC:%03d nr_scunroute Succ DeN:%s DxN:%s",
            lIdx, mIsdnChannel[lIdx].chnlDeviceName, mIsdnDx[lIdx].dxDeviceName);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

      CloseDxDevice(lIdx);

      if (gc_Close(mIsdnChannel[lIdx].chnlHandle) != GC_SUCCESS)
      {
         GetGcErrInfo(lErInfo);
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d Failed gc_Close DeN:%s ErrVl:%d Err:%s",
               lIdx, mIsdnChannel[lIdx].chnlDeviceName, lErInfo.gcValue, lErInfo.gcMsg);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d gc_Close Succ DeNe:%s",
         lIdx, mIsdnChannel[lIdx].chnlDeviceName);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendIsdnCallMsgToNw
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
INT16 IsdnDevice::SendIsdnCallMsgToNw(GenIsupMsg &genIsupMsg)
{
   INT32       cicIdx       = genIsupMsg.circuitId;
   INT32       lCic         = mIsdnChannel[cicIdx].cic;
   int         lGcCallState = 0x00;
   GC_INFO     lErInfo;
   TEXT        lStringCallState[51] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CIC:%03d IsupMsgType:%d Process Rcvd IsdnCall Msg ChSt:%d CalDir:%d",
         lCic, genIsupMsg.isupMsgType, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
         mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE51, lLogText);

   if(lCic != cicIdx)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CIC:%03d cicIdx:%d Send ALERTING, Not Matching Error in Internal CIC Maping",
            lCic, cicIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return 0;
   }

   if(ISDN_D_CHANNEL_INACTIVE == mIsdnChannel[cicIdx].isdnCallDetails.dChnlState)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
      "CIC:%03d IsupMsgType:%d ChSt:%d CalDir:%d Can't Proceed As D-Channel Inactive",
       lCic, genIsupMsg.isupMsgType, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
       mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE52, lLogText);
      return 0;
   }

   if(!(ISDN_NO_CALL == mIsdnChannel[cicIdx].isdnCallDetails.callDirection && 
            GEN_ISUP_IAM == genIsupMsg.isupMsgType))
   {
      if(0 == mIsdnChannel[cicIdx].crn)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "CIC:%03d Crn:%ld Error Null Call Reference Number Cannot Proceed, Failed",
               lCic, mIsdnChannel[cicIdx].crn);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_DEVICE18, lLogText);
         return 0;
      }

      lGcCallState = GetCallState(lCic, mIsdnChannel[cicIdx].crn);
      if( 0 > lGcCallState)
      {
         GetGcErrInfo(lErInfo);
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "CIC:%03d Crn:%ld Invalid Call State Cannot Proceed, Failed",
               lCic, mIsdnChannel[cicIdx].crn);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_DEVICE59, lLogText);
         return 0;
      }

      snprintf(lStringCallState, 50, "%s", IsdnUtil::ConvertDlgCalStToStr(lGcCallState));
   }

   switch(genIsupMsg.isupMsgType)
   {
      case GEN_ISUP_ACM:
         {
            if(GCST_OFFERED != lGcCallState)
            {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d Crn:%ld DCalSt:%s Can't Send ALERTING",
                     lCic, mIsdnChannel[cicIdx].crn, lStringCallState);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE20, lLogText);
               return 0;
            }

            if( 0 > gc_AcceptCall(mIsdnChannel[cicIdx].crn, 3, EV_ASYNC))
            {
               GetGcErrInfo(lErInfo);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d Crn:%ld DCalSt:%s gc_AcceptCall Send ALERTING Failed ErrVl:%d  Err:%s",
                     lCic, mIsdnChannel[cicIdx].crn, lStringCallState, lErInfo.gcValue, lErInfo.gcMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE20, lLogText);

               memset(&genIsupMsg, 0, sizeof(GenIsupMsg));
               //Send DISCONNECT to Application as ALERTING Failed
               genIsupMsg.isupMsgType = GEN_ISUP_REL;
               genIsupMsg.circuitId = cicIdx;
               return -1; //return as msg has to relayed back to App
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d Crn:%ld DCalSt:%s Sending ALERTING",
                  lCic, mIsdnChannel[cicIdx].crn, lStringCallState);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

            pthread_mutex_lock(&Call_mutex[cicIdx]);
            mIsdnChannel[cicIdx].isdnCallDetails.recvCallState = ISDN_RECV_ACCEPTED;
            pthread_mutex_unlock(&Call_mutex[cicIdx]);

            gLog.GenerateLog(ISDN_DEVICE21, lLogText);
         }
         break;
      case GEN_ISUP_ANM:
         {
            if(GCST_ACCEPTED != lGcCallState)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d Crn:%ld DCalSt:%s Scheduling CONNECT",
                  lCic, mIsdnChannel[cicIdx].crn, lStringCallState);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               mScheduledConnect[cicIdx].isScheduled = true;
               mScheduledConnect[cicIdx].schConnect  = genIsupMsg.isupAnm;
               gLog.GenerateLog(ISDN_DEVICE26, lLogText);
               return 0;
            }
            
            if(0 > gc_AnswerCall(mIsdnChannel[cicIdx].crn, 0, EV_ASYNC))
            {
               GetGcErrInfo(lErInfo);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d Crn:%ld DCalSt:%s gc_AnswerCall Send CONNECT Failed ErrVl:%d  Err:%s",
                     lCic, mIsdnChannel[cicIdx].crn, lStringCallState, 
                     lErInfo.gcValue, lErInfo.gcMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE27, lLogText);


               memset(&genIsupMsg, 0, sizeof(GenIsupMsg));
               //Send DISCONNECT to Application as CONNECT Failed
               genIsupMsg.isupMsgType = GEN_ISUP_REL;
               genIsupMsg.circuitId = cicIdx;
               return -1; //return as msg has to relayed back to App
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CIC:%03d Crn:%ld DCalSt:%s Sending CONNECT",
            lCic, mIsdnChannel[cicIdx].crn, lStringCallState);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

            pthread_mutex_lock(&Call_mutex[cicIdx]);
            mIsdnChannel[cicIdx].isdnCallDetails.recvCallState = ISDN_RECV_CONNECTED;
            pthread_mutex_unlock(&Call_mutex[cicIdx]);

            gLog.GenerateLog(ISDN_DEVICE28, lLogText);
         }
         break;
      case GEN_ISUP_REL:
         {
            if(GCST_NULL == lGcCallState || GCST_IDLE == lGcCallState)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d Crn:%ld DCalSt:%s Can't Send DISCONNECT",
                     lCic, mIsdnChannel[cicIdx].crn, lStringCallState);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE29, lLogText);

               return 0;
            }

            //if(0 > gc_DropCall(mIsdnChannel[cicIdx].crn, GC_NORMAL_CLEARING, EV_ASYNC))
            if(0 > gc_DropCall(mIsdnChannel[cicIdx].crn, genIsupMsg.isupRel.cause, EV_ASYNC))
            {
               GetGcErrInfo(lErInfo);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d Crn:%ld DCalSt:%s Cause:%d Send DISCONNECT Failed ErrVl:%d  Err:%s",
                     lCic, mIsdnChannel[cicIdx].crn, lStringCallState, 
                     genIsupMsg.isupRel.cause, lErInfo.gcValue, lErInfo.gcMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE29, lLogText);
               return 0;
            }

            pthread_mutex_lock(&Call_mutex[cicIdx]);
            if(ISDN_IN_BOUND_CALL == mIsdnChannel[cicIdx].isdnCallDetails.callDirection)
               mIsdnChannel[cicIdx].isdnCallDetails.recvCallState = ISDN_RECV_DISCONNECTED;
            else
               mIsdnChannel[cicIdx].isdnCallDetails.makeCallState = ISDN_MAKE_DISCONNECTED;
            pthread_mutex_unlock(&Call_mutex[cicIdx]);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d Crn:%ld DCalSt:%s Sending DISCONNECT Cause:%d",
                  lCic, mIsdnChannel[cicIdx].crn, lStringCallState, 
                  genIsupMsg.isupRel.cause);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE30, lLogText);
            return 1;
         }
         break;
      case GEN_ISUP_RLC:
         {
            if(GCST_DISCONNECTED != lGcCallState || true == mIsdnChannel[cicIdx].taskFail ||
                  ISDN_NO_CALL == mIsdnChannel[cicIdx].isdnCallDetails.callDirection)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d Crn:%ld DCalSt:%s TaskFail:%d CalDir:%d Can't Send RELEASE",
                     lCic, mIsdnChannel[cicIdx].crn, lStringCallState, 
                     mIsdnChannel[cicIdx].taskFail, mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE33, lLogText);

               return 0;
            }

            if(0 > gc_DropCall(mIsdnChannel[cicIdx].crn, genIsupMsg.isupRlc.cause, EV_ASYNC))
            {
               GetGcErrInfo(lErInfo);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d Crn:%ld DCalSt:%s Cause:%d Send RELEASE Failed ErrVl:%d  Err:%s",
                     lCic, mIsdnChannel[cicIdx].crn, lStringCallState, 
                     genIsupMsg.isupRlc.cause, lErInfo.gcValue, lErInfo.gcMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE33, lLogText);
               return 0;
            }


            pthread_mutex_lock(&Call_mutex[cicIdx]);
            if(ISDN_IN_BOUND_CALL == mIsdnChannel[cicIdx].isdnCallDetails.callDirection)
               mIsdnChannel[cicIdx].isdnCallDetails.recvCallState = ISDN_RECV_IDLE;
            else
               mIsdnChannel[cicIdx].isdnCallDetails.makeCallState = ISDN_MAKE_IDLE;
            pthread_mutex_unlock(&Call_mutex[cicIdx]);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d Crn:%ld DCalSt:%s Sending RELEASE Cause:%d",
                  lCic, mIsdnChannel[cicIdx].crn, lStringCallState, 
                  genIsupMsg.isupRlc.cause);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE34, lLogText);
            return 1;
         }
         break;
      case GEN_ISUP_IAM:
         {  
            GC_MAKECALL_BLK          lGcmkbl;
            GCLIB_MAKECALL_BLK       lGclib_mkbl;
            int                      lRet = 0;

            if(ISDN_D_CHANNEL_ACTIVE != mIsdnChannel[cicIdx].isdnCallDetails.dChnlState ||
                  ISDN_NO_CALL != mIsdnChannel[cicIdx].isdnCallDetails.callDirection)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d ChSt:%d CalDir:%d MaCalSt:%d can't snd SETUP DChnl Inactive/CalDir Invalid", 
                  lCic, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
                  mIsdnChannel[cicIdx].isdnCallDetails.callDirection,
                  mIsdnChannel[cicIdx].isdnCallDetails.makeCallState);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE58, lLogText);
            }
            
            memset(&lGclib_mkbl, 0, sizeof(GCLIB_MAKECALL_BLK));
            
            lGclib_mkbl.ext_datap = NULL;
            lGclib_mkbl.destination.address_type = GCADDRTYPE_NAT;
            lGclib_mkbl.destination.address_plan = GCADDRPLAN_ISDN;
            strcpy(lGclib_mkbl.destination.address, genIsupMsg.isupIam.calledNumber);

            strcpy(lGclib_mkbl.origination.address, genIsupMsg.isupIam.callingNumber);
            lGclib_mkbl.origination.address_type = GCADDRTYPE_NAT;
            lGclib_mkbl.origination.address_plan = GCADDRPLAN_ISDN;

            lGcmkbl.cclib = NULL; /* CCLIB pointer unused */
            lGcmkbl.gclib = &lGclib_mkbl;

            lRet = gc_MakeCall(mIsdnChannel[cicIdx].chnlHandle, &mIsdnChannel[cicIdx].crn, 
                  lGclib_mkbl.destination.address, &lGcmkbl, mIsdnConfigApi.mOutgoingCallWaitTime, EV_ASYNC);
            if(lRet < 0 || 0 == mIsdnChannel[cicIdx].crn)
            {
               GetGcErrInfo(lErInfo);
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "CIC:%03d DeH:%d Crn:%ld gc_MakeCall, Snd SETUP Fail ErrVl:%d Err:%s ClN:%s CdN:%s",
                     mIsdnChannel[cicIdx].cic, mIsdnChannel[cicIdx].chnlHandle, 
                     mIsdnChannel[cicIdx].crn, lErInfo.gcValue, lErInfo.gcMsg,
                     genIsupMsg.isupIam.callingNumber, genIsupMsg.isupIam.calledNumber);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE60, lLogText);

               memset(&genIsupMsg, 0, sizeof(GenIsupMsg));
               //Send DISCONNECT to Application as SETUP Failed
               genIsupMsg.isupMsgType = GEN_ISUP_REL;
               genIsupMsg.circuitId = cicIdx;
               return -1;
            }

            pthread_mutex_lock(&Call_mutex[cicIdx]);
            mIsdnChannel[cicIdx].isdnCallDetails.makeCallState = ISDN_MAKE_DIALING;
            mIsdnChannel[cicIdx].isdnCallDetails.callDirection = ISDN_OUT_BOUND_CALL;
            pthread_mutex_unlock(&Call_mutex[cicIdx]);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "CIC:%03d DeH:%d Crn:%ld Sending SETUP MaCalSt:%d CalDir:%d ClN:%s CdN:%s",
               mIsdnChannel[cicIdx].cic, mIsdnChannel[cicIdx].chnlHandle, mIsdnChannel[cicIdx].crn,
               mIsdnChannel[cicIdx].isdnCallDetails.makeCallState, 
               mIsdnChannel[cicIdx].isdnCallDetails.callDirection,
               genIsupMsg.isupIam.callingNumber, genIsupMsg.isupIam.calledNumber);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE61, lLogText);

            return 1;
         }
         break;
      case GEN_ISUP_RSC:
      case GEN_ISUP_CPG:
      case GEN_ISUP_UNKNOWN:
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d Crn:%ld DCalSt:%s Req Msg Not Handled",
                  lCic, mIsdnChannel[cicIdx].crn, lStringCallState);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE35, lLogText);
            return 0;
         }
   }
   return 1;
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessGlobalCallEvents
// DESCRIPTION : 
// PARAMETERS  : long, int, GenIsupMsg
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::ProcessGlobalCallEvents(METAEVENT &metaevent, GenIsupMsg &genIsupMsg)
{
   struct  IsdnChannel  *pIsdnChnl = NULL;
   GC_INFO lErInfo;
   TEXT        lLogText[MAX_LOG_TEXT_LEN]  = "";

   pIsdnChnl = (struct IsdnChannel *) metaevent.usrattr;
   if(NULL == pIsdnChnl || pIsdnChnl->chnlHandle != metaevent.evtdev)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Evt:%ld EvtH:%ld IsdnChnlPtr:%p DeH:%d Not Matching",
            metaevent.evttype, metaevent.evtdev, pIsdnChnl, pIsdnChnl->chnlHandle);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);

      return false;
   }

   switch(metaevent.evttype)
   {
      case GCEV_OPENEX:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d Recv GCEV_OPENEX Not Processed",
                  pIsdnChnl->cic);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);
         }
         break;
      case GCEV_BLOCKED:
         {
            BOOLEAN lBlockedReturn                 = false;

            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "CIC:%03d Recv GCEV_BLOCKED D-Channel InActive ChSt:%d CalDir:%d RcCalSt:%d MaCalSt:%d",
                  pIsdnChnl->cic, pIsdnChnl->isdnCallDetails.dChnlState, 
                  pIsdnChnl->isdnCallDetails.callDirection, pIsdnChnl->isdnCallDetails.recvCallState,
                  pIsdnChnl->isdnCallDetails.makeCallState);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            pIsdnChnl->isdnCallDetails.dChnlState = ISDN_D_CHANNEL_INACTIVE;

            if(ISDN_IN_BOUND_CALL == pIsdnChnl->isdnCallDetails.callDirection || 
                   ISDN_OUT_BOUND_CALL == pIsdnChnl->isdnCallDetails.callDirection)
            {
               switch(pIsdnChnl->isdnCallDetails.recvCallState)
               {
                  case ISDN_RECV_ACCEPTED:
                  case ISDN_RECV_CONNECTED:
                  case ISDN_RECV_OFFERED:
                     genIsupMsg.isupMsgType = GEN_ISUP_REL;
                     genIsupMsg.circuitId = pIsdnChnl->cic;
                     lBlockedReturn = true;
                     break;
                  default:
                  {
                     
                  }
               }

               pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
               pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECVCALL_UNKNOWN;
               pIsdnChnl->isdnCallDetails.callDirection = ISDN_NO_CALL;
               pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKECALL_UNKNOWN;
               pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);
            }
            return lBlockedReturn;
         }
         break;
      case GCEV_UNBLOCKED:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d Recv GCEV_UNBLOCKED D-Channel Active",
                  pIsdnChnl->cic);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            pIsdnChnl->isdnCallDetails.dChnlState = ISDN_D_CHANNEL_ACTIVE;
            //Channel Became Active Reset Line Device
            ResetLineDevice(pIsdnChnl->cic, EV_ASYNC);
         }
         break;
      case GCEV_OFFERED:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Recv GCEV_OFFERED Crn:%ld DCalSt:%s",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
                  IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            //Reset Any Scheduled Connect As Its New Call (New SETUP)
            memset(&mScheduledConnect[pIsdnChnl->cic], 0, sizeof(ScheduledConnect));
            pIsdnChnl->taskFail = false;

            pIsdnChnl->crn = metaevent.crn;
            
            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECV_OFFERED;
            pIsdnChnl->isdnCallDetails.callDirection = ISDN_IN_BOUND_CALL;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);

            HandleOfferedCall(pIsdnChnl, genIsupMsg);
            return true;
         }
         break;
      case GCEV_ACCEPT:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Recv GCEV_ACCEPT Crn:%ld DCalSt:%s",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
                  IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            if(true == mScheduledConnect[pIsdnChnl->cic].isScheduled)
            {
               if(0 > gc_AnswerCall(metaevent.crn, 0, EV_ASYNC))
               {
                  GetGcErrInfo(lErInfo);
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                        "CIC:%03d DeH:%d Crn:%ld Sending Scheduled CONNECT Failed ErrVl:%d Err:%s",
                        pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn, lErInfo.gcValue, lErInfo.gcMsg);
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ISDN_DEVICE27, lLogText);

                  memset(&mScheduledConnect[pIsdnChnl->cic], 0, sizeof(ScheduledConnect));

                  //Send DISCONNECT to Application as CONNECT Failed
                  genIsupMsg.isupMsgType = GEN_ISUP_REL;
                  genIsupMsg.circuitId = pIsdnChnl->cic;
                  return true; //return as msg has to relayed back to App
               }

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d DeH:%d Crn:%ld Sending Scheduled CONNECT",
                     pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE28, lLogText);
               memset(&mScheduledConnect[pIsdnChnl->cic], 0, sizeof(ScheduledConnect));
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d DeH:%d Crn:%ld No Scheduled CONNECT",
                     pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE27, lLogText);
            }
         }
         break;
      case GCEV_ANSWERED:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Recv GCEV_ANSWERED Crn:%ld DCalSt:%s",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
                  IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECV_CONNECTED;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);
         }
         break;
      case GCEV_PROCEEDING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Recv GCEV_PROCEEDING Crn:%ld DCalSt:%s",
            pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
            IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE62, lLogText);
         }
         break;
      case GCEV_ALERTING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Recv GCEV_ALERTING Crn:%ld DCalSt:%s",
            pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
            IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE63, lLogText);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKE_ALERTING;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);

            genIsupMsg.isupMsgType = GEN_ISUP_ACM;
            genIsupMsg.circuitId = pIsdnChnl->cic;
            return true;
         }
         break;
      case GCEV_CONNECTED:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Recv GCEV_CONNECTED Crn:%ld DCalSt:%s",
            pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
            IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE64, lLogText);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKE_CONNECTED;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);

            genIsupMsg.isupMsgType = GEN_ISUP_ANM;
            genIsupMsg.circuitId = pIsdnChnl->cic;
            return true;
         }
         break;
      case GCEV_DISCONNECTED:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d Recv GCEV_DISCONNECTED Crn:%ld DCalSt:%s Forward to App",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
                  IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
           gLog.GenerateLog(ISDN_DEVICE38, lLogText);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            if(ISDN_IN_BOUND_CALL == pIsdnChnl->isdnCallDetails.callDirection)
               pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECV_DISCONNECTED;
            else
               pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKE_DISCONNECTED;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);

            genIsupMsg.isupMsgType = GEN_ISUP_REL;
            genIsupMsg.circuitId = pIsdnChnl->cic;
            return true;
         }
         break;
         case GCEV_DROPCALL:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d Recv GCEV_DROPCALL Crn:%ld DCalSt:%s",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
                  IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            if(ISDN_IN_BOUND_CALL == pIsdnChnl->isdnCallDetails.callDirection)
               pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECV_IDLE;
            else
               pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKE_IDLE;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);

            if(0 > gc_ReleaseCallEx(metaevent.crn, EV_ASYNC))
            {
               GetGcErrInfo(lErInfo);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d DeH:%d Crn:%ld gc_ReleaseCallEx Failed ErrVl:%d Err:%s",
                     pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn, lErInfo.gcValue, lErInfo.gcMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE39, lLogText);
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d Crn:%ld gc_ReleaseCallEx Succ",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE40, lLogText);
         }
         break;
         case GCEV_RELEASECALL:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d Recv GCEV_RELEASECALL Crn:%ld DCalSt:%s",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn,
                  IsdnUtil::ConvertDlgCalStToStr(GetCallState(pIsdnChnl->cic, metaevent.crn)));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECVCALL_UNKNOWN;
            pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKECALL_UNKNOWN;
            pIsdnChnl->isdnCallDetails.callDirection = ISDN_NO_CALL;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);
            CallInfoReset(pIsdnChnl->cic);
         };
         break;
         case GCEV_TASKFAIL:
         {
            BOOLEAN lTaskFailReturn                 = false;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "CIC:%03d DeH:%d Recv GCEV_TASKFAIL Crn:%ld",
               pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE41, lLogText);

               if(ISDN_IN_BOUND_CALL == pIsdnChnl->isdnCallDetails.callDirection)
               {
                  switch(pIsdnChnl->isdnCallDetails.recvCallState)
                  {
                     case ISDN_RECV_ACCEPTED:
                     {
                           GetGcResultInfo(pIsdnChnl->cic, &metaevent, lErInfo);
                              snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                    "CIC:%03d Crn:%ld Evt gc_AcceptCall Send "
                                    "ALERTING Failed Send DISCONNECT to App ErrVl:%d  Err:%s",
                                    pIsdnChnl->cic, metaevent.crn,
                                    lErInfo.gcValue, lErInfo.gcMsg);
                           TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                           gLog.GenerateLog(ISDN_DEVICE20, lLogText);

                           //Send DISCONNECT to Application as ALERTING Failed
                           genIsupMsg.isupMsgType = GEN_ISUP_REL;
                           genIsupMsg.circuitId = pIsdnChnl->cic;

                           lTaskFailReturn = true;//return as msg has to relayed back to App*/
                     }
                     break;
                     case ISDN_RECV_CONNECTED:
                     {
                        GetGcResultInfo(pIsdnChnl->cic, &metaevent, lErInfo);
                         snprintf(lLogText, MAX_LOG_TEXT_LEN,
                         "CIC:%03d Crn:%ld Evt gc_AnswerCall Send "
                         "CONNECT Failed Send DISCONNECT to App ErrVl:%d  Err:%s",
                         pIsdnChnl->cic, metaevent.crn,
                         lErInfo.gcValue, lErInfo.gcMsg);
                         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                         gLog.GenerateLog(ISDN_DEVICE27, lLogText);

                         genIsupMsg.isupMsgType = GEN_ISUP_REL;
                         genIsupMsg.circuitId = pIsdnChnl->cic;

                         lTaskFailReturn = true;//return as msg has to relayed back to App*/
                     }
                     break;
                     case ISDN_RECV_DISCONNECTED:
                     {
                        GetGcResultInfo(pIsdnChnl->cic, &metaevent, lErInfo);
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "CIC:%03d Crn:%ld ISDN_IN_BOUND_CALL Evt gc_DropCall Send "
                        "DISCONNECT Failed ErrVl:%d  Err:%s",
                        pIsdnChnl->cic, metaevent.crn,
                        lErInfo.gcValue, lErInfo.gcMsg);
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(ISDN_DEVICE29, lLogText);
                     }
                     break;
                     default:
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "CIC:%03d Crn:%ld ISDN_IN_BOUND_CALL This TaskFail Event-rev not processed",
                        pIsdnChnl->cic, metaevent.crn);
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(ISDN_DEVICE42, lLogText);
                     }
                  }
               }
               else if(ISDN_OUT_BOUND_CALL == pIsdnChnl->isdnCallDetails.callDirection)
               {
                  switch(pIsdnChnl->isdnCallDetails.makeCallState)
                  {
                     case ISDN_MAKE_DIALING:
                     case ISDN_MAKE_ALERTING:
                     case ISDN_MAKE_CONNECTED:
                     {
                        GetGcResultInfo(pIsdnChnl->cic, &metaevent, lErInfo);
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "CIC:%03d Crn:%ld Evt gc_MakeCall Send "
                        "SETUP Failed Send DISCONNECT to App ErrVl:%d  Err:%s",
                        pIsdnChnl->cic, metaevent.crn, lErInfo.gcValue, lErInfo.gcMsg);
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(ISDN_DEVICE60, lLogText);

                        genIsupMsg.isupMsgType = GEN_ISUP_REL;
                        genIsupMsg.circuitId = pIsdnChnl->cic;

                        lTaskFailReturn = true;//return as msg has to relayed back to App*/
                     }
                     break;
                     case ISDN_MAKE_DISCONNECTED:
                     {
                        GetGcResultInfo(pIsdnChnl->cic, &metaevent, lErInfo);
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "CIC:%03d Crn:%ld ISDN_OUT_BOUND_CALL Evt gc_DropCall Send "
                              "DISCONNECT Failed ErrVl:%d  Err:%s",
                              pIsdnChnl->cic, metaevent.crn,
                              lErInfo.gcValue, lErInfo.gcMsg);
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(ISDN_DEVICE29, lLogText);
                     }
                     break;
                     default:
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "CIC:%03d Crn:%ld ISDN_OUT_BOUND_CALL This TaskFail Event-rev not processed",
                              pIsdnChnl->cic, metaevent.crn);
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(ISDN_DEVICE42, lLogText);
                     }
                  }
               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "CIC:%03d Crn:%ld This TaskFail Event-call dir not processed",
                        pIsdnChnl->cic, metaevent.crn);
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               }
      
               if(false == pIsdnChnl->taskFail)
               {
                  //TaskFail Occured for 1st time for this channel, So Reset Line Dev
                  pIsdnChnl->taskFail = true;
                  ResetLineDevice(pIsdnChnl->cic, EV_ASYNC);
               }
               else
               {
                  //TaskFail Occured for ResetLineDevice issued for previous TaskFail
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "CIC:%03d DeH:%d Crn:%ld TASKFAIL Already Processed Chnl For TaskFail, Trying Again",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn);
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ISDN_DEVICE42, lLogText);

                  ResetLineDevice(pIsdnChnl->cic, EV_ASYNC);
               }
               return lTaskFailReturn;
         }
         break;
         case GCEV_RELEASECALL_FAIL:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d Recv GCEV_RELEASECALL_FAIL Crn:%ld",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            if(ISDN_IN_BOUND_CALL ==  pIsdnChnl->isdnCallDetails.callDirection)
               pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECVCALL_UNKNOWN;
            else
               pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKECALL_UNKNOWN;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);

            pthread_mutex_lock(&Call_mutex[pIsdnChnl->cic]);
            pIsdnChnl->isdnCallDetails.recvCallState = ISDN_RECVCALL_UNKNOWN;
            pIsdnChnl->isdnCallDetails.makeCallState = ISDN_MAKECALL_UNKNOWN;
            pIsdnChnl->isdnCallDetails.callDirection = ISDN_NO_CALL;
            pthread_mutex_unlock(&Call_mutex[pIsdnChnl->cic]);
            CallInfoReset(pIsdnChnl->cic);
         }
         break;
         case GCEV_RESETLINEDEV:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "CIC:%03d DeH:%d Recv GCEV_RESETLINEDEV Crn:%ld TaskFail:%d",
               pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.crn, pIsdnChnl->taskFail);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            if(true == pIsdnChnl->taskFail)
               pIsdnChnl->taskFail = false;

            ReceiveCall(pIsdnChnl, EV_ASYNC);
         }
         break;
         case GCEV_RESTARTFAIL:
         {  
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d TaskFail:%d Recv GCEV_RESTARTFAIL Crn:%ld",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, pIsdnChnl->taskFail, metaevent.crn);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE36, lLogText);

            if(true == pIsdnChnl->taskFail)
               pIsdnChnl->taskFail = false;
         }
         break;
         default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeH:%d Recv Event:%ld TaskFail:%d Not Handled",
                  pIsdnChnl->cic, pIsdnChnl->chnlHandle, metaevent.evttype, pIsdnChnl->taskFail);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE43, lLogText);
            pIsdnChnl->taskFail = false;
         }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : HandleOfferedCall
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::HandleOfferedCall(IsdnChannel* const isdnChnl, GenIsupMsg &genIsupMsg)
{
   GC_INFO  lErInfo;
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   genIsupMsg.isupMsgType = GEN_ISUP_IAM;
   genIsupMsg.circuitId = isdnChnl->cic;

   if (gc_GetCallInfo(isdnChnl->crn, DESTINATION_ADDRESS, genIsupMsg.isupIam.calledNumber) != GC_SUCCESS)
   {
      GetGcErrInfo(lErInfo);
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Crn:%ld gc_GetCallInfo DA Failed ErrVl:%d Err:%s",
            isdnChnl->cic, isdnChnl->chnlHandle, isdnChnl->crn, lErInfo.gcValue, lErInfo.gcMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE39, lLogText);
   }

   if (gc_GetCallInfo(isdnChnl->crn, ORIGINATION_ADDRESS, genIsupMsg.isupIam.callingNumber) != GC_SUCCESS)
   {
      GetGcErrInfo(lErInfo);
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Crn:%ld gc_GetCallInfo OA Failed ErrVl:%d Err:%s",
            isdnChnl->cic, isdnChnl->chnlHandle, isdnChnl->crn, lErInfo.gcValue, lErInfo.gcMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE39, lLogText);
   }

   genIsupMsg.isupIam.callingNumberFormat = GEN_INTERNATIONAL_FMT;
   genIsupMsg.isupIam.calledNumberFormat = GEN_INTERNATIONAL_FMT;

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Crn:%ld Rcv SETUP ClN:%s CdN:%s",
         isdnChnl->cic, isdnChnl->chnlHandle, isdnChnl->crn,
         genIsupMsg.isupIam.callingNumber, genIsupMsg.isupIam.calledNumber);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE37, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReceiveCall
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::ReceiveCall(IsdnChannel* const isdnChnl, unsigned long mode)
{
   int      rc = -1;
   GC_INFO  lErInfo;
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   rc = gc_WaitCall(isdnChnl->chnlHandle, NULL, NULL, -1, mode);
   if( rc < 0)
   {  
      GetGcErrInfo(lErInfo);
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d gc_WaitCall Failed ErrVl:%d Err:%s",
            isdnChnl->cic, isdnChnl->chnlHandle, lErInfo.gcValue, lErInfo.gcMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE39, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%03d DeH:%d Issued gc_WaitCall Succ",
         isdnChnl->cic, isdnChnl->chnlHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE44, lLogText);
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : HandleTdxError
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::HandleTdxError(INT32 cicIdx, MpResponseDetails & rsp)
{
   INT32            lDxIndex = mIsdnChannel[cicIdx].indxIsdnDx;
   INT32            lCic     = mIsdnChannel[cicIdx].cic;
   long             lStpRes                 = -1;
   TEXT             lLogText[MAX_LOG_TEXT_LEN]  = "";

   lStpRes = ATDV_LASTERR(mIsdnDx[lDxIndex].dxHandle);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "CIC:%03d CmdType:%d DxH:%d Rcvd TDX_ERROR Event Reason:%ld CS:%d", 
         lCic, mIsdnChannel[cicIdx].isdnVoiceDetails.requestDetails.cmdType,
         mIsdnDx[lDxIndex].dxHandle, lStpRes,
         mChannelStatus[cicIdx].voiceChannel.status);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE45, lLogText);


   switch(mChannelStatus[cicIdx].voiceChannel.status)
   {
      case ISDN_VOICE_CHANNEL_RECORDING:
      case ISDN_VOICE_CHANNEL_RECORD_STOPING:
            return HandleRecordStopped(cicIdx, rsp);
      case ISDN_VOICE_CHANNEL_PLAYING:
      case ISDN_VOICE_CHANNEL_PLAY_STOPING:
            return HandlePlayStopped(cicIdx, rsp);
      case ISDN_VOICE_CHANNEL_GET_DTMF:
      case ISDN_VOICE_CHANNEL_GET_DTMF_STOPING:
            return HandleGetDigitStopped(cicIdx, rsp);
      case ISDN_VOICE_CHANNEL_IDLE:
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "CIC:%03d CmdType:%d DxH:%d Can't Handle TDX_ERROR CS:%d",
                  lCic, mIsdnChannel[cicIdx].isdnVoiceDetails.requestDetails.cmdType,
                  mIsdnDx[lDxIndex].dxHandle, mChannelStatus[cicIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE46, lLogText);
            ChannelReset(cicIdx);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : ResetLineDevice
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::ResetLineDevice(INT32 cicIdx, unsigned long mode)
{
   GC_INFO  lErInfo;
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   if (mIsdnChannel[cicIdx].chnlHandle)
   {
      if (0 > gc_ResetLineDev(mIsdnChannel[cicIdx].chnlHandle, mode))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CIC:%03d DevN:%s DeH:%d Mode:%s gc_ResetLineDev Failed ErrVl:%d Err:%s",
         mIsdnChannel[cicIdx].cic, mIsdnChannel[cicIdx].chnlDeviceName, 
         mIsdnChannel[cicIdx].chnlHandle, (EV_ASYNC == mode) ? "EV_ASYNC" : "EV_SYNC",
         lErInfo.gcValue, lErInfo.gcMsg);

         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_DEVICE39, lLogText);
         return false;
      }

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CIC:%03d DevN:%s DeH:%d Mode:%s gc_ResetLineDev",
            mIsdnChannel[cicIdx].cic, mIsdnChannel[cicIdx].chnlDeviceName, 
            mIsdnChannel[cicIdx].chnlHandle, (EV_ASYNC == mode) ? "EV_ASYNC" : "EV_SYNC");
       T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
       gLog.GenerateLog(ISDN_DEVICE44, lLogText);
   }

   pthread_mutex_lock(&Call_mutex[cicIdx]);
   mIsdnChannel[cicIdx].isdnCallDetails.recvCallState = ISDN_RECVCALL_UNKNOWN;
   mIsdnChannel[cicIdx].isdnCallDetails.callDirection = ISDN_NO_CALL;
   mIsdnChannel[cicIdx].isdnCallDetails.makeCallState = ISDN_MAKECALL_UNKNOWN;
   pthread_mutex_unlock(&Call_mutex[cicIdx]);
   CallInfoReset(mIsdnChannel[cicIdx].cic);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SendIsdnChnlMgmtMsgToNw
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::SendIsdnChnlMgmtMsgToNw(CktCmdReq &cktCmdReq, CktCmdRsp &cktCmdRsp)
{
   INT32       cicIdx       = cktCmdReq.channelId;
   INT32       lCic         = mIsdnChannel[cicIdx].cic;
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CIC:%03d ChnlMgmtMsgType:%d Process Rcvd Isdn Chnl Mgmt Msg ChSt:%d CalDir:%d",
         lCic, cktCmdReq.cktCmd, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
         mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE53, lLogText);

   if(lCic != cicIdx)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CIC:%03d cicIdx:%d Send ALERTING, Not Matching Error in Internal CIC Maping",
            lCic, cicIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return 0;
   }

   switch(cktCmdReq.cktCmd)
   {
      case CKT_CMND_BLOCK:
         {
            if(true == ResetLineDevice(lCic, EV_SYNC))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d ChSt:%d CalDir:%d Block Success",
                     lCic, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
                     mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE54, lLogText);

               cktCmdRsp.status = CKT_CMND_BLOCKED;
               cktCmdRsp.channelId = lCic;
               return true;
            }
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d ChSt:%d CalDir:%d Block Failed",
                  lCic, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
                  mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE55, lLogText);
         }
         break;
      case CKT_CMND_UNBLOCK:
         {
            if(true == ReceiveCall(&mIsdnChannel[cicIdx], EV_SYNC))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "CIC:%03d ChSt:%d CalDir:%d Unblock Success",
                     lCic, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
                     mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISDN_DEVICE56, lLogText);

               cktCmdRsp.status = CKT_CMND_UNBLOCKED;
               cktCmdRsp.channelId = lCic;
               return true;
            }
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d ChSt:%d CalDir:%d Unblock Failed",
                  lCic, mIsdnChannel[cicIdx].isdnCallDetails.dChnlState,
                  mIsdnChannel[cicIdx].isdnCallDetails.callDirection);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE57, lLogText);
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d Req Chnl Mgmt Msg Not Handled", lCic);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DEVICE35, lLogText);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : GetCallState
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int IsdnDevice::GetCallState(INT32 cic, long crn)
{
  int         lGcCallState = 0x00;
  GC_INFO     lErInfo;
  TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   if( 0 > gc_GetCallState(crn, &lGcCallState))
   {
      GetGcErrInfo(lErInfo);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CIC:%03d Crn:%ld Unable retrive Call State gc_GetCallState ErrVl:%d  Err:%s Failed",
            cic, crn, lErInfo.gcValue, lErInfo.gcMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE19, lLogText);
      return -1;
   }

   return lGcCallState;
}

//----------------------------------------------------------------------
// METHOD NAME : GetVoiceChannelStatus
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
EnumIsdnVoiceChannelState IsdnDevice::GetVoiceChannelStatus(INT32 cic)
{
   return mChannelStatus[cic].voiceChannel.status;
}

//----------------------------------------------------------------------
// METHOD NAME : ConnectSpeech
// DESCRIPTION :
// PARAMETERS  :
// RETURN      :
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::ConnectSpeech(MpRequestDetails mpReqDetails)
{

   INT32       cicIdx_1  = mpReqDetails.mpReqData.switchChnl.circuitId_1;
   INT32       cicIdx_2  = mpReqDetails.mpReqData.switchChnl.circuitId_2;
   INT32       lCic_1    = mIsdnChannel[cicIdx_1].cic;
   INT32       lCic_2    = mIsdnChannel[cicIdx_2].cic;
   INT32       lDtiHdl_1 = mIsdnDti[mIsdnChannel[cicIdx_1].indxIsdnDti].dtiHandle;
   INT32       lDtiHdl_2 = mIsdnDti[mIsdnChannel[cicIdx_2].indxIsdnDti].dtiHandle;
   INT32       lSeq      = mpReqDetails.mpReqData.switchChnl.seqNum;
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   if(lCic_1 != cicIdx_1 || lCic_2 != cicIdx_2)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC_1:%03d cicIdx_1:%d CIC_2:%03d cicIdx_2:%d ConnectSpeech "
            "Not Matching Error in Internal CIC Maping",
            lSeq, lCic_1, cicIdx_1, lCic_2, cicIdx_2);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC_1:%03d CIC_2:%03d Recvd ConnectSpeech CS_1:%d CS_2:%d", 
         lSeq, lCic_1, lCic_2, 
         mChannelStatus[cicIdx_1].voiceChannel.status, 
         mChannelStatus[cicIdx_2].voiceChannel.status);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE65, lLogText);


   if ((nr_scroute(lDtiHdl_1, SC_DTI, lDtiHdl_2, SC_DTI, SC_FULLDUP)) == -1 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC_1:%03d CIC_2:%03d Dti_1:%d Dti_2:%d ConnectSpeech Failed nr_scroute", 
         lSeq, lCic_1, lCic_2, lDtiHdl_1, lDtiHdl_2);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE66, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC_1:%03d CIC_2:%03d Dti_1:%d Dti_2:%d ConnectSpeech Failed nr_scroute", 
         lSeq, lCic_1, lCic_2, lDtiHdl_1, lDtiHdl_2);
   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE67, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DisconnectSpeech
// DESCRIPTION :
// PARAMETERS  :
// RETURN      :
//----------------------------------------------------------------------
BOOLEAN IsdnDevice::DisconnectSpeech(MpRequestDetails mpReqDetails)
{

   INT32       cicIdx_1  = mpReqDetails.mpReqData.switchChnl.circuitId_1;
   INT32       cicIdx_2  = mpReqDetails.mpReqData.switchChnl.circuitId_2;
   INT32       lCic_1    = mIsdnChannel[cicIdx_1].cic;
   INT32       lCic_2    = mIsdnChannel[cicIdx_2].cic;
   INT32       lDtiHdl_1 = mIsdnDti[mIsdnChannel[cicIdx_1].indxIsdnDti].dtiHandle;
   INT32       lDtiHdl_2 = mIsdnDti[mIsdnChannel[cicIdx_2].indxIsdnDti].dtiHandle;
   INT32       lSeq      = mpReqDetails.mpReqData.switchChnl.seqNum;

   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   if(lCic_1 != cicIdx_1 || lCic_2 != cicIdx_2)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CIC_1:%03d cicIdx_1:%d CIC_2:%03d cicIdx_2:%d DisconnectSpeech "
            "Not Matching Error in Internal CIC Maping",
            lSeq, lCic_1, cicIdx_1, lCic_2, cicIdx_2);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE22, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC_1:%03d CIC_2:%03d Recvd DisconnectSpeech CS_1:%d CS_2:%d", 
         lSeq, lCic_1, lCic_2, 
         mChannelStatus[cicIdx_1].voiceChannel.status, 
         mChannelStatus[cicIdx_2].voiceChannel.status);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE68, lLogText);


   if ((nr_scunroute(lDtiHdl_1, SC_DTI, lDtiHdl_2, SC_DTI, SC_FULLDUP)) == -1 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC_1:%03d CIC_2:%03d Dti_1:%d Dti_2:%d DisconnectSpeech Failed nr_scroute", 
         lSeq, lCic_1, lCic_2, lDtiHdl_1, lDtiHdl_2);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DEVICE69, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CIC_1:%03d CIC_2:%03d Dti_1:%d Dti_2:%d DisconnectSpeech Failed nr_scroute", 
         lSeq, lCic_1, lCic_2, lDtiHdl_1, lDtiHdl_2);
   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DEVICE70, lLogText);

   return true;
}
