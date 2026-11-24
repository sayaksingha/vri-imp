static const char *id = "@(#) $Id: HmpDevice.cc,v 1.1.2.4 2016/04/12 21:59:33 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpDevice.cc
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
// AUTHOR         : Ramprasad M                              12-JAN-2012
// AUTHOR         : Chetan V M                               12-JAN-2012
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "HmpDevice.h"

pthread_mutex_t Chnl_mutex[MAX_SYNWAY_CHANNEL];

//----------------------------------------------------------------------
// METHOD NAME : HmpDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpDevice::HmpDevice()
{
   mNumOfDxDevice    = 0;
   mNumOfDtiDevice   = 0;
   mNumOfIpmDevice   = 0;

   memset(mChannelStatus, 0, sizeof(ChannelStatus) * MAX_DTI_DEVICE);
}

//----------------------------------------------------------------------
// METHOD NAME : ~HmpDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpDevice::~HmpDevice()
{  
}

//----------------------------------------------------------------------
// METHOD NAME : DeInit
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN HmpDevice::DeInit()
{
   CloseDxDevice();
   CloseDtiDevice();
   CloseIpmDevice();

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Init()
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpDevice::Init()
{
   if(true != GetDeviceInfo())
   {
      return false;
   }
   
   if(true != OpenDevices())
   {
      return false;
   }

   if(true != mHmpReqMapApi.Init())
   {
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : StartSrEvent
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN HmpDevice::StartSrEvent(MpResponseDetails &mpResponse)
{
   int      lRet;
   long     lEvttype;
   int      lDevHandle;
   TEXT     lLogText[MAX_LOG_TEXT_LEN]  = "";

   for (;;) 
   {
      lRet = sr_waitevt(500);            
      if (lRet != HMP_FAILURE) 
      {
         lEvttype = sr_getevttype();
         lDevHandle = sr_getevtdev();
         
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "Recvd Event %s 0x%lx Handle:%d", 
               HmpUtil::ConvertHmpEventToStr(lEvttype), lEvttype, lDevHandle);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         ProcessEvents(lEvttype, lDevHandle, mpResponse);
      }
   }
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessEvents
// DESCRIPTION : Destructor
// PARAMETERS  : long, int
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::ProcessEvents(long evtTyp, int devHdl, 
                                              MpResponseDetails &mpRsp)
{
   INT32       lDtiDevIdx[MAX_CONNCURRENT_ACTION];
   HmpReqMap   lHmpReqMap;
   TEXT        lHashKey[20 + 1] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN]  = "";

   sprintf(lHashKey, "%d", devHdl);
   if(false == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReqMap))
      return false;

   for(int ldti = 0; ldti < lHmpReqMap.totalDtiDevice; ldti++)
      lDtiDevIdx[ldti] = lHmpReqMap.dtiHandleIdx[ldti];

   switch(evtTyp)
   {
      case TDX_PLAY:
         {
            return HandlePlayStopped(lHmpReqMap, lDtiDevIdx[0], mpRsp);
         }
         break;
      case TDX_RECORD:
         {
            return HandleRecordStopped(lHmpReqMap, lDtiDevIdx[0], mpRsp);
         }
         break;
      case TDX_GETDIG:
         {
            return HandleGetDigitStopped(lHmpReqMap, lDtiDevIdx[0], mpRsp);
         }
         break;
      case IPMEV_OPEN:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeviceHandle:%d IPMEV_OPEN Event :0x%lx",
                  lDtiDevIdx[0], devHdl, evtTyp);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         }
         break;
      case IPMEV_STARTMEDIA:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeviceHandle:%d IPMEV_STARTMEDIA Event :0x%lx",
                  lDtiDevIdx[0], devHdl, evtTyp);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         }
         break;
      case IPMEV_STOPPED:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "CIC:%03d DeviceHandle:%d IPMEV_STOPPED Event :0x%lx",
                  lDtiDevIdx[0], devHdl, evtTyp);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "CIC:%03d DeviceHandle:%d not processed for this Event:0x%lx", 
                  lDtiDevIdx[0], devHdl, evtTyp);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE01, lLogText);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : HandleGetDigitStopped
// DESCRIPTION : 
// PARAMETERS  : HmpReqMap, INT32
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::HandleGetDigitStopped(HmpReqMap &hmpReq, INT32 dtiHIdx, 
      MpResponseDetails &rsp)
{
   INT32        lDxIndex      = hmpReq.dxHandleIdx;
   INT32        lSeq          = hmpReq.requestDetails.mpReqData.getDigit.seqNum;
   INT16        lDigitLen   = 0;
   long         lPlayStpRes                 = -1;
   TEXT         lLogText[MAX_LOG_TEXT_LEN]  = "";
   
   lPlayStpRes = ATDX_TERMMSK(mHmpDxStruct[lDxIndex].dxHandle);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "SEQ:%06d CIC:%03d DxH:%d Rcvd GetDigit Stop Event Reason:%s CS:%d NxtReqStatus:%d", 
         lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle,
         HmpUtil::VoiceTermReason(lPlayStpRes),
         mChannelStatus[dtiHIdx].voiceChannel.status, hmpReq.lChnlReqStatus);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE02, lLogText);

   switch(mChannelStatus[dtiHIdx].voiceChannel.status)
   {
      case VOICE_CHANNEL_IDLE:
      case VOICE_CHANNEL_PLAYING:
      case VOICE_CHANNEL_PLAY_STOPING:
      case VOICE_CHANNEL_RECORDING:
      case VOICE_CHANNEL_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d was not waiting for digits CS:%d",
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle, hmpReq.lChnlReqStatus);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE03, lLogText);
         }
         break;
      case VOICE_CHANNEL_GET_DTMF:
      case VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d GetDigit Stopped CS:%d", 
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle, hmpReq.lChnlReqStatus);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

            
            if(NEXT_IDLE != hmpReq.lChnlReqStatus)
            {
               if(false == HandleNextRequest(hmpReq, lDxIndex, dtiHIdx, rsp))
                  DeviceUnlisten(lSeq, lDxIndex, dtiHIdx);

               return true;
            }
            else
            {
               rsp.rspMsg                       = MP_RSP_SUCCESS;
               rsp.cmdType                      = MC_GETDIGSTP;
               rsp.mpRspData.getDigit           = hmpReq.requestDetails.mpReqData.getDigit;

               lDigitLen = strlen(mHmpDxStruct[lDxIndex].digits.dg_value);

               if(0 == lDigitLen)
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                        "SEQ:%06d CIC:%03d DxH:%d GetDigit Stopped But No Digit Present In Buffer",
                        lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle);
               else
               {
                  if(0 == hmpReq.requestDetails.mpReqData.getDigit.includeTerminateChar && 
                        TM_DIGIT == lPlayStpRes)
                  {
                     strncpy(rsp.mpRspData.getDigit.rspDigits, 
                           mHmpDxStruct[lDxIndex].digits.dg_value, lDigitLen - 1);
                     rsp.mpRspData.getDigit.rspDigits[lDigitLen - 1] = '\0';
                  }
                  else
                  {
                     strcpy(rsp.mpRspData.getDigit.rspDigits, mHmpDxStruct[lDxIndex].digits.dg_value);
                  }

                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "SEQ:%06d CIC:%03d DxH:%d ITC:%d Digits:%s GetDigit Stopped",
                        lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle, 
                        hmpReq.requestDetails.mpReqData.getDigit.includeTerminateChar,
                        rsp.mpRspData.getDigit.rspDigits);
               }
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(HMP_DEVICE04, lLogText);

               DeviceUnlisten(lSeq, lDxIndex, dtiHIdx);

               return true;

            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d Invalid Dx Channel Status",
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE05, lLogText);
         }
   }
   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : HandleRecordStopped
// DESCRIPTION : 
// PARAMETERS  : HmpReqMap, INT32
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::HandleRecordStopped(HmpReqMap &hmpReq, INT32 dtiHIdx, 
      MpResponseDetails & rsp)
{
   long         lPlayStpRes                 = -1;
   INT32        lSeq                        = hmpReq.requestDetails.mpReqData.recordMsg.seqNum;
   INT32        lDxIndex = hmpReq.dxHandleIdx;
   TEXT         lLogText[MAX_LOG_TEXT_LEN]  = "";

   lPlayStpRes = ATDX_TERMMSK(mHmpDxStruct[lDxIndex].dxHandle);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "SEQ:%06d CIC:%03d DxH:%d Rcvd Record Stop Event Reason:%s CS:%d NxtReqStatus:%d", 
         lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle,
         HmpUtil::VoiceTermReason(lPlayStpRes),
         mChannelStatus[dtiHIdx].voiceChannel.status, hmpReq.lChnlReqStatus);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE06, lLogText);

   switch(mChannelStatus[dtiHIdx].voiceChannel.status)
   {
      case VOICE_CHANNEL_IDLE:
      case VOICE_CHANNEL_PLAYING:
      case VOICE_CHANNEL_GET_DTMF:
      case VOICE_CHANNEL_GET_DTMF_STOPING:
      case VOICE_CHANNEL_PLAY_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d was not recording CS:%d",
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle, hmpReq.lChnlReqStatus  );
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE03, lLogText);
         }
         break;
      case VOICE_CHANNEL_RECORDING:
      case VOICE_CHANNEL_RECORD_STOPING:
         {
            //close file open for recording 
            CloseDxIOFiles(lDxIndex, 1);

            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d Record Stopped CS:%d", 
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle, hmpReq.lChnlReqStatus  );
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

            if(NEXT_IDLE != hmpReq.lChnlReqStatus)
            {
               if(false == HandleNextRequest(hmpReq, lDxIndex, dtiHIdx, rsp))
                  DeviceUnlisten(lSeq, lDxIndex, dtiHIdx);

               return true;
            }
            else
            {
               DeviceUnlisten(lSeq, lDxIndex, dtiHIdx);

               rsp.rspMsg                       = MP_RSP_SUCCESS;
               rsp.cmdType                      = MC_RECSTP;
               rsp.mpRspData.stopMsg.seqNum     = hmpReq.requestDetails.mpReqData.recordMsg.seqNum;
               rsp.mpRspData.stopMsg.circuitId  = hmpReq.requestDetails.mpReqData.recordMsg.circuitId;

               return true;
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d Invalid Dx Channel Status",
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE05, lLogText);
         }
   }
   return false;
}
//----------------------------------------------------------------------
// METHOD NAME : HandlePlayStopped
// DESCRIPTION : 
// PARAMETERS  : HmpReqMap, INT32
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::HandlePlayStopped(HmpReqMap &hmpReq, INT32 dtiHIdx, 
      MpResponseDetails & rsp)
{
   INT32        lDxIndex = hmpReq.dxHandleIdx;
   INT32        lSeq     = hmpReq.requestDetails.mpReqData.playMsg.seqNum;
   long         lPlayStpRes                 = -1;
   TEXT         lLogText[MAX_LOG_TEXT_LEN]  = "";

   lPlayStpRes = ATDX_TERMMSK(mHmpDxStruct[lDxIndex].dxHandle);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "SEQ:%06d CIC:%03d DxH:%d Rcvd Play Stoped Event Reason:%s CS:%d NxtReqStatus:%d Repeat:%d", 
         lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle,
         HmpUtil::VoiceTermReason(lPlayStpRes),
         mChannelStatus[dtiHIdx].voiceChannel.status, 
         hmpReq.lChnlReqStatus, hmpReq.requestDetails.mpReqData.playMsg.repeatPlay);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE07, lLogText);


   switch(mChannelStatus[dtiHIdx].voiceChannel.status)
   {
      case VOICE_CHANNEL_IDLE:
      case VOICE_CHANNEL_RECORDING:
      case VOICE_CHANNEL_RECORD_STOPING:
      case VOICE_CHANNEL_GET_DTMF:
      case VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d was not playing CS:%d",
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle, hmpReq.lChnlReqStatus  );
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE03, lLogText);
         }
         break;

      case VOICE_CHANNEL_PLAYING:
         {
            //Close opened File for playing
            CloseDxIOFiles(lDxIndex, hmpReq.requestDetails.mpReqData.playMsg.vFileCount);

            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d Rcvd Play Stop Event CS:%d NxtReqStatus:%d Repeat:%d", 
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle,
                  mChannelStatus[dtiHIdx].voiceChannel.status, 
                  hmpReq.lChnlReqStatus, hmpReq.requestDetails.mpReqData.playMsg.repeatPlay);
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            
            if(1 == hmpReq.requestDetails.mpReqData.playMsg.repeatPlay && TM_EOD == lPlayStpRes)
            {
               if(true == Play(hmpReq.requestDetails.mpReqData.playMsg, lDxIndex))
               {
                  pthread_mutex_lock(&Chnl_mutex[dtiHIdx]);
                  mChannelStatus[dtiHIdx].voiceChannel.status  = VOICE_CHANNEL_PLAYING;
                  pthread_mutex_unlock(&Chnl_mutex[dtiHIdx]);

                  return true;
               }
               else
               {
                  rsp.rspMsg              = MP_RSP_FAILURE;
                  rsp.cmdType             = MC_CNTTN;
                  rsp.mpRspData           = hmpReq.requestDetails.mpReqData;
                  
                  return true;
               }
            }
            else
            {
               DeviceUnlisten(lSeq, lDxIndex, dtiHIdx);

               rsp.rspMsg                       = MP_RSP_SUCCESS;
               rsp.cmdType                      = MC_DISTN;
               rsp.mpRspData.stopMsg.seqNum     = hmpReq.requestDetails.mpReqData.playMsg.seqNum;
               rsp.mpRspData.stopMsg.circuitId  = hmpReq.requestDetails.mpReqData.playMsg.circuitId;

               return true;
            }
         }
         break;
      case VOICE_CHANNEL_PLAY_STOPING:
         {
            //Close opened File for playing
            CloseDxIOFiles(lDxIndex, hmpReq.requestDetails.mpReqData.playMsg.vFileCount);

            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d Play Stopped CS:VOICE_CHANNEL_PLAY_STOPING", 
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            
            if(NEXT_IDLE != hmpReq.lChnlReqStatus)
            {
               if(false == HandleNextRequest(hmpReq, lDxIndex, dtiHIdx, rsp))
                  DeviceUnlisten(lSeq, lDxIndex, dtiHIdx);

               return true;
            }
            else
            {
               DeviceUnlisten(lSeq, lDxIndex, dtiHIdx);

               rsp.rspMsg                       = MP_RSP_SUCCESS;
               rsp.cmdType                      = MC_DISTN;
               rsp.mpRspData.stopMsg.seqNum     = hmpReq.requestDetails.mpReqData.playMsg.seqNum;
               rsp.mpRspData.stopMsg.circuitId  = hmpReq.requestDetails.mpReqData.playMsg.circuitId;
               return true;
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d Invalid Dx Channel Status",
                  lSeq, dtiHIdx, mHmpDxStruct[lDxIndex].dxHandle);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE05, lLogText);
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
BOOLEAN HmpDevice::GetDeviceInfo()
{
   AUID*          lPhysicalBoard;
   long           lRetVal;
   int            lNumPylBds  = 0;
   int            lLen        = 0;
   TEXT           lPhyBrdName[MAX_BOARD_NAME_LEN] = "";

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
            T(gTrace, printf("%s: Number of Physical Boards: %d\n",
                     gProcessName, lNumPylBds););
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

   for(int lBrdCnt = 0;lBrdCnt < lNumPylBds; lBrdCnt++)
   {
      lLen = MAX_BOARD_NAME_LEN;

      SRLGetPhysicalBoardName(lPhysicalBoard[lBrdCnt], &lLen, lPhyBrdName);
      T(gTrace, printf("%s: Name of Physical Board %d: %s\n", 
               gProcessName, lBrdCnt + 1, lPhyBrdName););

      if(VirtualBoardCount(lPhysicalBoard[lBrdCnt]) == false)
      {
         TERR(gTrace, printf("%s: No Virtual Borads Vailable on Physical Board: %d\n", 
                  gProcessName, lBrdCnt + 1););
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
BOOLEAN HmpDevice::VirtualBoardCount(AUID PhyBd)
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
            T(gTrace, printf("%s: Number of Virtual Boards: %d\n", gProcessName, lNumVB););
      }
      for(int lVbCnt = 1;lVbCnt <= lNumVB;lVbCnt++)
      {
         T (gTrace, printf("%s: Virtual Board: %d\n", gProcessName, lVbCnt););
         T (gTrace, printf("%s: Virtual Board Name: %s\n", gProcessName, lVirBrdPtr -> szDevName););
         T (gTrace, printf("%s: Virtual Board Type: %d\n", gProcessName, lVirBrdPtr -> iDevType););

         if(Subdevices(lVirBrdPtr -> szDevName) == false)
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
BOOLEAN HmpDevice::Subdevices(TEXT* VBName)
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
            T (gTrace, printf("%s: Num of Sub Devices: %d\n", gProcessName, lNumSDs););
      }
      else
      {
         TERR (gTrace, printf("%s: No Sub Devices Available\n", gProcessName););
         return false;
      }
      for(int lSdCnt = 1;lSdCnt <= lNumSDs;lSdCnt++)
      {
         T (gTrace, printf("%s: Sub Device     :%d\n", gProcessName, lSdCnt););
         T (gTrace, printf("%s: Sud Device Name: %s\n", gProcessName, lVBPtr->szDevName););
         T (gTrace, printf("%s: Sub Device Type: %d\n", gProcessName, lVBPtr->iDevType););

         switch(lVBPtr->iDevType)
         {
            case 502: //VOICE_DEV
               mNumOfDxDevice++;
               mHmpDxStruct[mNumOfDxDevice].index = mNumOfDxDevice;
               strcpy(mHmpDxStruct[mNumOfDxDevice].dxDeviceName, lVBPtr->szDevName);
               break;
            case 504: //LINE_DEV
               mNumOfDtiDevice++;
               mHmpDtiStruct[mNumOfDtiDevice].index = mNumOfDtiDevice;
               strcpy(mHmpDtiStruct[mNumOfDtiDevice].dtiDeviceName, lVBPtr->szDevName);
               break;
            case 528: //IPM_DEV
               mNumOfIpmDevice++;
               mHmpIpmStruct[mNumOfIpmDevice].index = mNumOfIpmDevice;
               strcpy(mHmpIpmStruct[mNumOfIpmDevice].ipmDeviceName, lVBPtr->szDevName);
         }

         lVBPtr++;
      }
   }
   else
   {
      TERR (gTrace, printf("%s: No Sub Devices Available on Any Virtual Board\n", 
               gProcessName););
      return false;
   }
   T (gTrace, printf("%s: Voice Device Count: %d Line Device Count: %d Ipm Device Count: %d\n",
            gProcessName, mNumOfDxDevice, mNumOfDtiDevice, mNumOfIpmDevice););
   return true;

}

//----------------------------------------------------------------------
// METHOD NAME : OpenDevices
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::OpenDevices()
{
   BOOLEAN lRetDti, lRetVoice, lRetIpm;
   lRetDti = lRetVoice = lRetIpm = false;

   lRetVoice = OpenDxDevice();
   lRetDti   = OpenDtiDevice();
   lRetIpm   = OpenIpmDevice();

   return (lRetVoice && lRetDti && lRetIpm);
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessEvent
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::ProcessEvent()
{
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Stop
// DESCRIPTION : 
// PARAMETERS  : INT32
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::Stop(INT32 seq, INT32 cic)
{
   INT32       lDtiIdx    = cic;
   INT32        lSeq       = seq;
   INT32       lDxIndex;
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   lDxIndex = mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx;

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "SEQ:%06d CIC:%03d DxH:%d CS:%d Recvd Stop",
         lSeq, lDtiIdx, mHmpDxStruct[lDxIndex].dxHandle, mChannelStatus[lDtiIdx].voiceChannel.status);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE08, lLogText);

   switch(mChannelStatus[lDtiIdx].voiceChannel.status)
   {
      case VOICE_CHANNEL_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d CS:%d No Operation in Progress",
                  lSeq, lDtiIdx, mHmpDxStruct[lDxIndex].dxHandle, mChannelStatus[lDtiIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE09, lLogText);
         }
         break;
      case VOICE_CHANNEL_PLAY_STOPING:
      case VOICE_CHANNEL_RECORD_STOPING:
      case VOICE_CHANNEL_GET_DTMF_STOPING:
         {
             snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                   "SEQ:%06d CIC:%03d DxH:%d CS:%d Channel Already Stopping",
                   lSeq, lDtiIdx, mHmpDxStruct[lDxIndex].dxHandle, mChannelStatus[lDtiIdx].voiceChannel.status);
             TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
             gLog.GenerateLog(HMP_DEVICE09, lLogText);
         }
         break;
      case VOICE_CHANNEL_PLAYING:
      case VOICE_CHANNEL_RECORDING:
      case VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d CS:%d Issuing Channel Stop",
                   lSeq, lDtiIdx, mHmpDxStruct[lDxIndex].dxHandle, mChannelStatus[lDtiIdx].voiceChannel.status);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE10, lLogText);

            if(true == StopChnlAct(lDxIndex))
            {
               pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
               if(VOICE_CHANNEL_PLAYING == mChannelStatus[lDtiIdx].voiceChannel.status)
                  mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_PLAY_STOPING;
               else if(VOICE_CHANNEL_RECORDING == mChannelStatus[lDtiIdx].voiceChannel.status)
                  mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_RECORD_STOPING;
               else if(VOICE_CHANNEL_GET_DTMF == mChannelStatus[lDtiIdx].voiceChannel.status)
                  mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_GET_DTMF_STOPING;
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                        "SEQ:%06d CIC:%03d Abrupt Channel State Changed", lSeq, lDtiIdx);
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(HMP_DEVICE11, lLogText);
               }
               pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);

            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "SEQ:%06d CIC:%03d DxH:%d CS:%d Stop Failed",
                     lSeq, lDtiIdx, mHmpDxStruct[lDxIndex].dxHandle, 
                     mChannelStatus[lDtiIdx].voiceChannel.status);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(HMP_DEVICE12, lLogText);

               return false;
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d DxH:%d CS:%d Invalid Dx Channel Status",
                  lSeq, lDtiIdx, mHmpDxStruct[lDxIndex].dxHandle, 
                  mChannelStatus[lDtiIdx].voiceChannel.status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE05, lLogText);
         }
   }


   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : PlayFile
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::PlayFile(MpRequestDetails &mpReqDetails)
{
   INT32       lFreeDx    = -1;
   INT32       lDtiIdx    = mpReqDetails.mpReqData.playMsg.circuitId;
   INT32        lSeq       = mpReqDetails.mpReqData.playMsg.seqNum;
   HmpReqMap   lHmpReqMap;
   TEXT        lHashKey[20 + 1] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";


   snprintf(lLogText, MAX_LOG_TEXT_LEN, "SEQ:%06d CIC:%03d Recvd Play CS:%d", 
         lSeq, lDtiIdx, mChannelStatus[lDtiIdx].voiceChannel.status);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE13, lLogText);

   switch(mChannelStatus[lDtiIdx].voiceChannel.status)
   {
      case VOICE_CHANNEL_IDLE:
         {
            if(-1 != (lFreeDx = GetFreeDxDevice()))
            {
               if(true == DtListen(lDtiIdx, mHmpDxStruct[lFreeDx].scts) && 
                     true == DxListen(lFreeDx, mHmpDtiStruct[lDtiIdx].scts))
               {
                     if(true == Play(mpReqDetails.mpReqData.playMsg, lFreeDx))
                     {
                        pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
                        mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx    = lFreeDx;
                        mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_PLAYING;
                        pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);
                        
                        if(true == FillHmpReqAndInstToHash(lHmpReqMap, mpReqDetails, lFreeDx, lDtiIdx))
                           return true;
                     }
               }
            }
         }
         DeviceUnlisten(lSeq, lFreeDx, lDtiIdx);
         break;
      case VOICE_CHANNEL_PLAYING:
      case VOICE_CHANNEL_RECORDING:
      case VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d CS:%s Play, Hence Issuing Stop and Schedule Request", 
                  lSeq, lDtiIdx,
                  HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[lDtiIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE14, lLogText);

            if(true == StopAndScheduleRequest(lSeq, lDtiIdx, mpReqDetails, lHmpReqMap,
                     NEXT_PLAY_REQUIRED))
               return true;
         }
         break;
      case VOICE_CHANNEL_PLAY_STOPING:
      case VOICE_CHANNEL_RECORD_STOPING:
      case VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d CS:%s Play, Hence Schedule Request", lSeq, lDtiIdx,
                  HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[lDtiIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE14, lLogText);

            lFreeDx = mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx;

            sprintf(lHashKey, "%d", mHmpDxStruct[lFreeDx].dxHandle);
            if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReqMap))
            {
               lHmpReqMap.lChnlReqStatus = NEXT_PLAY_REQUIRED;
               lHmpReqMap.nextReqDetails = mpReqDetails;
               if(true == mHmpReqMapApi.UpdateRequestIntoHash(lHmpReqMap))
                  return true;
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d Dx Channel Invalid State", lSeq, lDtiIdx);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE05, lLogText);
         }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : StopAndScheduleRequest
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::StopAndScheduleRequest(INT32 lSeq, INT32 lDtiIdx, MpRequestDetails &mpReqDetails, 
      HmpReqMap &lHmpReqMap, EnumChannelReqStatus lChannelReqStatus)
{
   INT32       lFreeDx    = -1;
   TEXT        lHashKey[20 + 1] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   lFreeDx = mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx;

   sprintf(lHashKey, "%d", mHmpDxStruct[lFreeDx].dxHandle);
   if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReqMap))
   {
      lHmpReqMap.lChnlReqStatus = lChannelReqStatus;
      lHmpReqMap.nextReqDetails = mpReqDetails;
      if(true == mHmpReqMapApi.UpdateRequestIntoHash(lHmpReqMap))
      {
         if(true == StopChnlAct(lFreeDx))
         {
            pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
            if(VOICE_CHANNEL_PLAYING == mChannelStatus[lDtiIdx].voiceChannel.status)
               mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_PLAY_STOPING;
            else if(VOICE_CHANNEL_RECORDING == mChannelStatus[lDtiIdx].voiceChannel.status)
               mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_RECORD_STOPING;
            else if(VOICE_CHANNEL_GET_DTMF == mChannelStatus[lDtiIdx].voiceChannel.status)
               mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_GET_DTMF_STOPING;
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "SEQ:%06d CIC:%03d Abrupt Channel State Changed", lSeq, lDtiIdx);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(HMP_DEVICE11, lLogText);
            }

            pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);

            return true;
         }
      }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : RecordFile
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::RecordFile(MpRequestDetails &mpReqDetails)
{
   INT32       lFreeDx    = -1;
   INT32       lDtiIdx    = mpReqDetails.mpReqData.recordMsg.circuitId;
   INT32       lSeq       = mpReqDetails.mpReqData.recordMsg.seqNum;
   HmpReqMap   lHmpReqMap;
   TEXT        lHashKey[20 + 1] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";


   snprintf(lLogText, MAX_LOG_TEXT_LEN, "SEQ:%06d CIC:%03d Recvd Record CS:%d", 
         lSeq, lDtiIdx, mChannelStatus[lDtiIdx].voiceChannel.status);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE15, lLogText);

   switch(mChannelStatus[lDtiIdx].voiceChannel.status)
   {
      case VOICE_CHANNEL_IDLE:
         {
            if(-1 != (lFreeDx = GetFreeDxDevice()))
            {
               if(true == DtListen(lDtiIdx, mHmpDxStruct[lFreeDx].scts) && 
                     true == DxListen(lFreeDx, mHmpDtiStruct[lDtiIdx].scts))
               {
                  if(true == Record(mpReqDetails.mpReqData.recordMsg, lFreeDx))
                  {

                     pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
                     mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx    = lFreeDx;
                     mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_RECORDING;
                     pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);

                     if(true == FillHmpReqAndInstToHash(lHmpReqMap, mpReqDetails, lFreeDx, lDtiIdx))
                        return true;
                  }
               }
            }
         }
         DeviceUnlisten(lSeq, lFreeDx, lDtiIdx);
         break;
      case VOICE_CHANNEL_PLAYING:
      case VOICE_CHANNEL_RECORDING:
      case VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d CS:%s Record, Hence Issuing Stop and Schedule Request", lSeq, lDtiIdx,
                  HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[lDtiIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE14, lLogText);

            if(true == StopAndScheduleRequest(lSeq, lDtiIdx, mpReqDetails, lHmpReqMap, 
                     NEXT_RECORD_REQUIRED))
               return true;
         }
         break;
      case VOICE_CHANNEL_PLAY_STOPING:
      case VOICE_CHANNEL_RECORD_STOPING:
      case VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d CS:%s Record, Hence Schedule Request", lSeq, lDtiIdx,
                  HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[lDtiIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE14, lLogText);

            lFreeDx = mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx;

            sprintf(lHashKey, "%d", mHmpDxStruct[lFreeDx].dxHandle);
            if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReqMap))
            {
               lHmpReqMap.lChnlReqStatus = NEXT_RECORD_REQUIRED;
               lHmpReqMap.nextReqDetails = mpReqDetails;
               if(true == mHmpReqMapApi.UpdateRequestIntoHash(lHmpReqMap))
                  return true;
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d Channel Invalid State", lSeq, lDtiIdx);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE15, lLogText);
         }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : FillHmpReqAndInstToHash
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::FillHmpReqAndInstToHash(HmpReqMap &lHmpReqMap, 
      MpRequestDetails &mpReqDetails, INT32 lFreeDx, INT32 lDtiIdx)
{
   memset(&lHmpReqMap, 0, sizeof(HmpReqMap));
   lHmpReqMap.deviceType      = HMP_DX_DEVICE;
   lHmpReqMap.dxHandleIdx     = lFreeDx;
   lHmpReqMap.dtiHandleIdx[0] = lDtiIdx;
   lHmpReqMap.totalDtiDevice++;
   lHmpReqMap.lChnlReqStatus = NEXT_IDLE;
   lHmpReqMap.requestDetails = mpReqDetails;
   sprintf(lHmpReqMap.key, "%d", mHmpDxStruct[lFreeDx].dxHandle);

   if(true == mHmpReqMapApi.InsertRequestIntoHash(lHmpReqMap))
      return true;
   else
      return false;
}

//----------------------------------------------------------------------
// METHOD NAME : GetDigit
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::GetDigit(MpRequestDetails &mpReqDetails)
{
   INT32       lFreeDx    = -1;
   INT32       lSeq       = mpReqDetails.mpReqData.getDigit.seqNum;
   INT32       lDtiIdx    = mpReqDetails.mpReqData.getDigit.circuitId;
   HmpReqMap   lHmpReqMap;
   TEXT        lHashKey[20 + 1] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";


   snprintf(lLogText, MAX_LOG_TEXT_LEN, "SEQ:%06d CIC:%03d Recvd GetDigit CS:%d", 
         lSeq, lDtiIdx, mChannelStatus[lDtiIdx].voiceChannel.status);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE16, lLogText);

   switch(mChannelStatus[lDtiIdx].voiceChannel.status)
   {
      case VOICE_CHANNEL_IDLE:
         {
            if(-1 != (lFreeDx = GetFreeDxDevice()))
            {
               if(true == DtListen(lDtiIdx, mHmpDxStruct[lFreeDx].scts) && 
                     true == DxListen(lFreeDx, mHmpDtiStruct[lDtiIdx].scts))
               {
                  if(true == GetDigits(mpReqDetails.mpReqData.getDigit, lFreeDx))
                  {

                     pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
                     mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx    = lFreeDx;
                     mChannelStatus[lDtiIdx].voiceChannel.status  = VOICE_CHANNEL_GET_DTMF;
                     pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);

                     if(true == FillHmpReqAndInstToHash(lHmpReqMap, mpReqDetails, lFreeDx, lDtiIdx))
                        return true;
                  }
               }
            }
         }
         DeviceUnlisten(lSeq, lFreeDx, lDtiIdx);
         break;
      case VOICE_CHANNEL_PLAYING:
      case VOICE_CHANNEL_RECORDING:
      case VOICE_CHANNEL_GET_DTMF:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d CS:%s GetDigit, Hence Issuing Stop and Schedule Request", lSeq, lDtiIdx,
                  HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[lDtiIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE14, lLogText);

            if(true == StopAndScheduleRequest(lSeq, lDtiIdx, mpReqDetails, lHmpReqMap, 
                     NEXT_GET_DTMF_REQUIRED))
               return true;
         }
         break;
      case VOICE_CHANNEL_PLAY_STOPING:
      case VOICE_CHANNEL_RECORD_STOPING:
      case VOICE_CHANNEL_GET_DTMF_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d CS:%s GetDigit, Hence Schedule Request", lSeq, lDtiIdx,
                  HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[lDtiIdx].voiceChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE14, lLogText);

            lFreeDx = mChannelStatus[lDtiIdx].voiceChannel.dxHandleIdx;

            sprintf(lHashKey, "%d", mHmpDxStruct[lFreeDx].dxHandle);
            if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReqMap))
            {
               lHmpReqMap.lChnlReqStatus = NEXT_GET_DTMF_REQUIRED;
               lHmpReqMap.nextReqDetails = mpReqDetails;
               if(true == mHmpReqMapApi.UpdateRequestIntoHash(lHmpReqMap))
                  return true;
            }
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "SEQ:%06d CIC:%03d Channel Invalid State", lSeq, lDtiIdx);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE15, lLogText);
         }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : DeviceUnlisten
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
void HmpDevice::DeviceUnlisten(int lSeq, int dxHdlIdx, int dtiHdlIdx, int ipmHdlIdx)
{
   TEXT lHashKey[20 + 1] = "";
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "SEQ:%06d CIC:%03d DtiIdx:%d DxIdx:%d IpmIdx:%d", 
         lSeq, dtiHdlIdx, dtiHdlIdx, dxHdlIdx, ipmHdlIdx);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   if(-1 != dxHdlIdx)
   {
      sprintf(lHashKey, "%d", mHmpDxStruct[dxHdlIdx].dxHandle);
      mHmpReqMapApi.DeleteRequestFromHash(lHashKey);

      FreeDxDevice(dxHdlIdx);
      DxUnlisten(dxHdlIdx);

      mHmpDxStruct[dxHdlIdx].deviceState = DEVICE_IDLE;
      memset(mHmpDxStruct[dxHdlIdx].iott, 0, sizeof(DX_IOTT) * DX_MAXIMUM_FILES);
      memset(&mHmpDxStruct[dxHdlIdx].digits, 0, sizeof(DV_DIGIT));
   }

   if(-1 != dtiHdlIdx)
   {
      pthread_mutex_lock(&Chnl_mutex[dtiHdlIdx]);
      memset(&mChannelStatus[dtiHdlIdx], 0, sizeof(ChannelStatus));
      pthread_mutex_unlock(&Chnl_mutex[dtiHdlIdx]);

      DtUnListen(dtiHdlIdx);
   }

   if(-1 != ipmHdlIdx)
   {
      mHmpIpmStruct[ipmHdlIdx].streamType = IPM_NULL;
      SetIdleIpmDevice(ipmHdlIdx);
      sprintf(lHashKey, "%d", mHmpIpmStruct[ipmHdlIdx].ipmHandle);
      mHmpReqMapApi.DeleteRequestFromHash(lHashKey);
   }
}


//----------------------------------------------------------------------
// METHOD NAME : HandleNextRequest
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN HmpDevice::HandleNextRequest(HmpReqMap &hmpReq, INT32 dxIndex, 
      INT32 dtiHIdx, MpResponseDetails &rsp)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   rsp.mpRspData           = hmpReq.nextReqDetails.mpReqData;

   if(NEXT_PLAY_REQUIRED == hmpReq.lChnlReqStatus)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "SEQ:%06d CIC:%03d CS:%s NRS:%s, Hence Issue Play Request", 
            hmpReq.nextReqDetails.mpReqData.playMsg.seqNum,
            dtiHIdx, HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[dtiHIdx].voiceChannel.status),
            HmpUtil::ConvertNextReqStatToStr(hmpReq.lChnlReqStatus));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DEVICE17, lLogText);

      rsp.cmdType             = MC_CNTTN;

      if(true == Play(hmpReq.nextReqDetails.mpReqData.playMsg, dxIndex))
      {
         pthread_mutex_lock(&Chnl_mutex[dtiHIdx]);
         mChannelStatus[dtiHIdx].voiceChannel.status  = VOICE_CHANNEL_PLAYING;
         pthread_mutex_unlock(&Chnl_mutex[dtiHIdx]);

         hmpReq.lChnlReqStatus = NEXT_IDLE;
         hmpReq.requestDetails = hmpReq.nextReqDetails;

         rsp.rspMsg              = MP_RSP_SUCCESS;

         if(true == mHmpReqMapApi.UpdateRequestIntoHash(hmpReq))
            return true;
      }
   }
   else if(NEXT_RECORD_REQUIRED == hmpReq.lChnlReqStatus)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "SEQ:%06d CIC:%03d CS:%s NRS:%s, Hence Issue Record Request", 
            hmpReq.nextReqDetails.mpReqData.recordMsg.seqNum,
            dtiHIdx, HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[dtiHIdx].voiceChannel.status),
            HmpUtil::ConvertNextReqStatToStr(hmpReq.lChnlReqStatus));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DEVICE17, lLogText);

      rsp.cmdType             = MC_RECSTR;

      if(true == Record(hmpReq.nextReqDetails.mpReqData.recordMsg, dxIndex))
      {
         pthread_mutex_lock(&Chnl_mutex[dtiHIdx]);
         mChannelStatus[dtiHIdx].voiceChannel.status  = VOICE_CHANNEL_RECORDING;
         pthread_mutex_unlock(&Chnl_mutex[dtiHIdx]);

         hmpReq.lChnlReqStatus = NEXT_IDLE;
         hmpReq.requestDetails = hmpReq.nextReqDetails;

         rsp.rspMsg              = MP_RSP_SUCCESS;

         if(true == mHmpReqMapApi.UpdateRequestIntoHash(hmpReq))
            return true;
      }
   }
   else if(NEXT_GET_DTMF_REQUIRED == hmpReq.lChnlReqStatus)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "SEQ:%06d CIC:%03d CS:%s NRS:%s, Hence Issue GetDigit Request", 
            hmpReq.nextReqDetails.mpReqData.getDigit.seqNum,
            dtiHIdx, HmpUtil::ConvertVoiceChnlStatToStr(mChannelStatus[dtiHIdx].voiceChannel.status),
            HmpUtil::ConvertNextReqStatToStr(hmpReq.lChnlReqStatus));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DEVICE17, lLogText);

      rsp.cmdType             = MC_GETDIG;

      if(true == GetDigits(hmpReq.nextReqDetails.mpReqData.getDigit, dxIndex))
      {
         pthread_mutex_lock(&Chnl_mutex[dtiHIdx]);
         mChannelStatus[dtiHIdx].voiceChannel.status  = VOICE_CHANNEL_GET_DTMF;
         pthread_mutex_unlock(&Chnl_mutex[dtiHIdx]);

         hmpReq.lChnlReqStatus = NEXT_IDLE;
         hmpReq.requestDetails = hmpReq.nextReqDetails;

         rsp.rspMsg              = MP_RSP_SUCCESS;

         if(true == mHmpReqMapApi.UpdateRequestIntoHash(hmpReq))
            return true;
      }
   }

   rsp.rspMsg              = MP_RSP_FAILURE;
   return false;
}


//----------------------------------------------------------------------
// METHOD NAME : StreamReq
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::StreamReq(MpRequestDetails &mpReqDetails)
{
   INT32       lSeq     =  mpReqDetails.mpReqData.stream.seqNum;
   INT32       lDtiIdx  =  mpReqDetails.mpReqData.stream.circuitId;
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "SEQ:%06d CIC:%03d IpmCS:%s Url:%s Recvd Stream Req", 
         lSeq, lDtiIdx, 
         HmpUtil::ConvertIpmChnStatToStr(mChannelStatus[lDtiIdx].ipmChannel.status),
         mpReqDetails.mpReqData.stream.rtspUrl);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE18, lLogText);

   switch(mChannelStatus[lDtiIdx].ipmChannel.status)
   {
      case IPM_CHANNEL_IDLE:
         {
            if(true == PlayStream(mpReqDetails, true))
            {
               pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
               mChannelStatus[lDtiIdx].ipmChannel.ipmHandleIdx  = mFreeIpmIdx;
               mChannelStatus[lDtiIdx].ipmChannel.status        = IPM_CHANNEL_STREAM;
               pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);

               return true;
            }
         }
         break;
      case IPM_CHANNEL_STREAM:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "SEQ:%06d CIC:%03d IpmCS:%s Hence Stop Stream, PLay Again ",
                  lSeq, lDtiIdx, 
                  HmpUtil::ConvertIpmChnStatToStr(mChannelStatus[lDtiIdx].ipmChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE19, lLogText);
            
            if(true == StopStreamReq(lSeq, lDtiIdx))
            {
               return StreamReq(mpReqDetails);
            }
         }
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "SEQ:%06d CIC:%03d IpmCS:%s Invalid Ipm Channel Status",
                  lSeq, lDtiIdx,
                  HmpUtil::ConvertIpmChnStatToStr(mChannelStatus[lDtiIdx].ipmChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE05, lLogText);
         }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : StopStreamReq
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDevice::StopStreamReq(INT32 seq, INT32 lDtiIdx)
{
   HmpReqMap   lHmpReq;
   int         lIndex;
   TEXT        lHashKey[20 + 1 ] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";


   mFreeIpmIdx= mChannelStatus[lDtiIdx].ipmChannel.ipmHandleIdx;

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "SEQ:%06d CIC:%03d IpmH:%d ST:%s IpmCS:%s Recvd Stop Stream",
         seq, lDtiIdx, mHmpIpmStruct[mFreeIpmIdx].ipmHandle, 
         HmpUtil::ConvertIpmStreamToStr(mHmpIpmStruct[mFreeIpmIdx].streamType),
         HmpUtil::ConvertIpmChnStatToStr(mChannelStatus[lDtiIdx].ipmChannel.status));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DEVICE20, lLogText);

   switch(mChannelStatus[lDtiIdx].ipmChannel.status)
   {
      case IPM_CHANNEL_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "SEQ:%06d CIC:%03d IpmH:%d IpmCS:%d Not streaming To Stop",
                  seq, lDtiIdx, mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
                  mChannelStatus[lDtiIdx].ipmChannel.status);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE09, lLogText);
         }
         break;
      case IPM_CHANNEL_STREAM:
         {
            sprintf(lHashKey, "%d", mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
            if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReq))
            {
               for(lIndex = 0; lIndex < MAX_DTI_LSTN_IPM; lIndex++)
               {
                  if(lDtiIdx == lHmpReq.dtiHandleIdx[lIndex])
                     lHmpReq.dtiHandleIdx[lIndex] = 0;
                  break;
               }
               if(MAX_DTI_LSTN_IPM == lIndex)
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "SEQ:%06d CIC:%03d IpmH:%d DtiDevice Not Present in HT",
                        seq, lDtiIdx, mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
                  T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(HMP_DEVICE21, lLogText);
               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "SEQ:%06d CIC:%03d IpmH:%d ST:%s IpmCS:%s DtiAttachToIpm:%d Processing Stop",
                        seq, lDtiIdx, mHmpIpmStruct[mFreeIpmIdx].ipmHandle, 
                        HmpUtil::ConvertIpmStreamToStr(mHmpIpmStruct[mFreeIpmIdx].streamType),
                        HmpUtil::ConvertIpmChnStatToStr(mChannelStatus[lDtiIdx].ipmChannel.status),
                        lHmpReq.totalDtiDevice);
                  T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(HMP_DEVICE21, lLogText);

                  lHmpReq.totalDtiDevice--;
                  if(mHmpIpmStruct[mFreeIpmIdx].streamType == IPM_LIVE_STREAM)
                  {
                     if(0 == lHmpReq.totalDtiDevice)
                     {  
                        RtspStop();
                     }
                     DeviceUnlisten(seq, -1, lDtiIdx);
                     return mHmpReqMapApi.UpdateRequestIntoHash(lHmpReq);
                  }
                  else
                  {
                     StopStream();
                     DeviceUnlisten(seq, -1, lDtiIdx, mFreeIpmIdx);
                  }
                  pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
                  mChannelStatus[lDtiIdx].ipmChannel.ipmHandleIdx  = 0;
                  mChannelStatus[lDtiIdx].ipmChannel.status        = IPM_CHANNEL_IDLE;
                  pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);

                  return true;
               }
            }
            DeviceUnlisten(seq, -1, lDtiIdx);
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "SEQ:%06d CIC:%03d IpmCS:%s Invalid Ipm Channel Status",
                  seq, lDtiIdx,
                  HmpUtil::ConvertIpmChnStatToStr(mChannelStatus[lDtiIdx].ipmChannel.status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DEVICE05, lLogText);
         }
   }

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : PlayStream
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN HmpDevice::PlayStream(MpRequestDetails &mpReqDetails, BOOLEAN play)
{
   INT32       lSeq     =  mpReqDetails.mpReqData.stream.seqNum;
   INT32       lDtiIdx  =  mpReqDetails.mpReqData.stream.circuitId;
   HmpReqMap   lHmpReq;
   TEXT        lHashKey[20 + 1 ] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "SEQ:%06d CIC:%03d Url:%s",
         lSeq, lDtiIdx,
         mpReqDetails.mpReqData.stream.rtspUrl);
   mFreeIpmIdx = CheckLiveStream(mpReqDetails.mpReqData.stream.rtspUrl);

   if(-1 == mFreeIpmIdx)
   {
      snprintf(mLogText, MAX_LOG_TEXT_LEN,
            "%s No matching Urls of Live Stream", lLogText);
      TERR (gTrace, printf("%s: %s\n", gProcessName, mLogText););
      gLog.GenerateLog(HMP_DEVICE30, mLogText);

      if(false != SetFreeIpmDevice())
      {
         if(true != CreateRtspSession(mpReqDetails))
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s RTSP Session Creation Failed", lLogText);
            TERR (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE22, mLogText);

            return false;
         }
         else
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s IpmH:%d RTSP Session Creation Success", 
                  lLogText, mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
            T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE23, mLogText);
         }

         if(true != StartStream())
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s RTSP Start Stream Failed", lLogText);
            TERR (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE22, mLogText);

            return false;
         }
         else
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s IpmH:%d RTSP Start Stream Success", 
                  lLogText, mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
            T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE23, mLogText);
         }

         if(false != play)
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s IpmH:%d On Demand Stream Request", 
                  lLogText, mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
            T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE26, mLogText);

            mHmpIpmStruct[mFreeIpmIdx].streamType = IPM_ON_DEMAND_STREAM;
            if(true == DtListen(lDtiIdx, mHmpIpmStruct[mFreeIpmIdx].scts))
            {
               if(true == FillIpmHmpReqAndInstToHash(mFreeIpmIdx, mpReqDetails, lDtiIdx))
               {
                  if(true == RtspPlay())
                     return true;
               }
            }

            DeviceUnlisten(lSeq, -1, lDtiIdx, mFreeIpmIdx);
         }
         else
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s IpmH:%d Intialization of Live Stream", 
                  lLogText, mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
            T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE25, mLogText);

            mHmpIpmStruct[mFreeIpmIdx].streamType = IPM_LIVE_STREAM;
            strcpy(mHmpIpmStruct[mFreeIpmIdx].rtspUrl, 
                  mpReqDetails.mpReqData.stream.rtspUrl);

            if(true == FillIpmHmpReqAndInstToHash(mFreeIpmIdx, mpReqDetails, 0))
            {
               return true;
            }

         }
      }
   }
   else
   {
      snprintf(mLogText, MAX_LOG_TEXT_LEN,
            "%s Live Stream Request", lLogText);
      T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
      gLog.GenerateLog(HMP_DEVICE25, mLogText);

      sprintf(lHashKey, "%d", mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
      if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReq))
      {
         if(MAX_DTI_LSTN_IPM == lHmpReq.totalDtiDevice)
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s Maximu Dti On IpmDevice reached", lLogText);
            TERR (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE27, mLogText);
         }
         else
         {
            snprintf(mLogText, MAX_LOG_TEXT_LEN,
                  "%s CurrentDti:%d Initiating RTSP Play",
                  lLogText, lHmpReq.totalDtiDevice);
            T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
            gLog.GenerateLog(HMP_DEVICE28, mLogText);

            lHmpReq.dtiHandleIdx[lHmpReq.totalDtiDevice] = lDtiIdx;
            lHmpReq.totalDtiDevice++;
            if(true == DtListen(lDtiIdx, mHmpIpmStruct[mFreeIpmIdx].scts))
            {
               if(1 == lHmpReq.totalDtiDevice)
               {
                  if(true == RtspPlay())
                  {
                     snprintf(mLogText, MAX_LOG_TEXT_LEN,
                           "%s DtiDevice:%d RTSP Initial Live Stream Play Success",
                           lLogText, lHmpReq.totalDtiDevice);
                     T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
                     gLog.GenerateLog(HMP_DEVICE29, mLogText);

                     mHmpReqMapApi.UpdateRequestIntoHash(lHmpReq);

                     return true;
                  }
               }
               else
               {
                  snprintf(mLogText, MAX_LOG_TEXT_LEN,
                        "%s DtiDevice:%d RTSP Live Stream Play Success",
                        lLogText, lHmpReq.totalDtiDevice);
                  T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
                  gLog.GenerateLog(HMP_DEVICE29, mLogText);

                  mHmpReqMapApi.UpdateRequestIntoHash(lHmpReq);

                  return true;
               }
            }
            else
               DeviceUnlisten(lSeq, -1, lDtiIdx);
         }
      }
   }
   snprintf(mLogText, MAX_LOG_TEXT_LEN,
         "%s Play Stream Failed", lLogText);
   T (gTrace, printf("%s: %s\n", gProcessName, mLogText););
   gLog.GenerateLog(HMP_DEVICE24, mLogText);

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : FillIpmHmpReqAndInstToHash
// DESCRIPTION : 
// PARAMETERS  : 
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN HmpDevice::FillIpmHmpReqAndInstToHash(INT32 ipmIdx, MpRequestDetails &mpReqDetails, INT32 dtiIdx)
{
   HmpReqMap   lHmpReq;

   sprintf(lHmpReq.key, "%d",  mHmpIpmStruct[mFreeIpmIdx].ipmHandle);
   lHmpReq.deviceType = HMP_IPM_DEVICE;
   lHmpReq.ipmHandleIdx = mFreeIpmIdx;
   if(0 == dtiIdx)
      lHmpReq.totalDtiDevice = 0;
   else
   {
      lHmpReq.dtiHandleIdx[lHmpReq.totalDtiDevice] = dtiIdx;
      lHmpReq.totalDtiDevice++;
   }
   lHmpReq.requestDetails = mpReqDetails;


   if(true == mHmpReqMapApi.InsertRequestIntoHash(lHmpReq))
      return true;
   else
      return false;
}
