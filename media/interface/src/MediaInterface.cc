static const char* id="@(#) $Id: MediaInterface.cc,v 1.1.2.4.2.3 2018/02/26 23:18:00 mram Exp $" ;
//----------------------------------------------------------------------------
// NAME : 
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2010
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// ORIGINATED  
//----------------------------------------------------------------------------

#include "MediaInterface.h"
extern unsigned int gReferenceNum;
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
MediaInterface::MediaInterface()
{

}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
MediaInterface::~MediaInterface()
{

}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::Init()
{
	if(false == ReadConfig())
	{
		return false;
	}
	if(false == CreateMsgQ())
	{
		return false;
	}

   mStopRequested=false;
	return true;
}

//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::ReadConfig()
{
	CfgRead        lCfgRead("MEDIA_MEDIA_API");
	TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	if(false == lCfgRead.CfgInit(MEDIA_IPC_CFG))
	{
		printf("%s: CfgRead object(%s) intialization failed...\n",
				                        gProcessName,MEDIA_IPC_CFG);
		gLog.GenerateLog(GSYS09,(TEXT*) "MEDIA_IPC_CFG CfgRead object initializaton failed");
		return false;
	}
	
	if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_VOICE_PROCESS_RSP_Q", RdMsgQKey,
				                        MEDIA_MIN_MSG_Q_KEY, MEDIA_MAX_MSG_Q_KEY))
	{
		sprintf(lLogText,
				"Config read Err for MSG_VOICE_PROCESS_RSP_Q in %s",
				MEDIA_IPC_CFG);
		printf(":%s:%s\n", lLogText,gProcessName);
		lCfgRead.CfgDeInit();
		return false;
	}
	else
	{
		T(gTrace,printf("%s: MSG_VOICE_PROCESS_RSP_Q = %d \n", gProcessName,
					RdMsgQKey););
	}
	
	if( CFG_OK !=  lCfgRead.GetConfigNum("MSG_VOICE_PROCESS_REQ_Q", WrMsgQKey,
				MEDIA_MIN_MSG_Q_KEY, MEDIA_MAX_MSG_Q_KEY))
	{
		sprintf(lLogText,
				"Config read Err for MSG_VOICE_PROCESS_REQ_Q in %s",
				MEDIA_IPC_CFG);
		printf(":%s:%s\n", lLogText,gProcessName);
		lCfgRead.CfgDeInit();
		return false;
	}
	else
	{
		T(gTrace,printf("%s: MSG_VOICE_PROCESS_REQ_Q = %d \n", gProcessName,
					WrMsgQKey););
	}
	
	lCfgRead.CfgDeInit();
	
	if(false == lCfgRead.CfgInit(MEDIA_KER_CFG))
	{
		printf("%s: CfgRead object(%s) intialization failed...\n",
				gProcessName,MEDIA_KER_CFG);
		gLog.GenerateLog(GSYS09,(TEXT*) "MEDIA_KER_CFG CfgRead object initializaton failed");
		return false;
	}
	if(CFG_OK != lCfgRead.GetConfigNum("MAX_CHANNEL_PER_PROCESS",mMaxNoOfChanPerCard,
										MEDIA_MIN_CHAN_PER_CARD,MEDIA_MAX_CHAN_PER_CARD))
	{
		sprintf(lLogText,
				   "Config read Err for MAX_CHANNEL_PER_PROCESS in %s",
					 MEDIA_KER_CFG);
		printf(":%s:%s\n", lLogText,gProcessName);
		lCfgRead.CfgDeInit();
		return false;
	}
	else
	{
		T(gTrace,printf("%s: MAX_CHANNEL_PER_PROCESS = %d \n", gProcessName,
					mMaxNoOfChanPerCard););
	}

   lCfgRead.CfgDeInit();

   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::CreateMsgQ()
{
   msgQueueError lQueErr;

   lQueErr = RdResponseQ.Create (RdMsgQKey, IPC_CREAT | IPC_PERM);

   if(Q_SUCCESS != lQueErr)
   {
      MediaUtil::HandleQueueCreateError (lQueErr,  RdResponseQ);
      printf("%s: Error creating MSG_VOICE_PROCESS_RSP_Q queue key:%d\n",
            gProcessName,RdMsgQKey);
      return false;
   }

   lQueErr = WrRequestQ.Create (WrMsgQKey, IPC_CREAT | IPC_PERM);

   if(Q_SUCCESS != lQueErr)
   {
      MediaUtil::HandleQueueCreateError (lQueErr,  WrRequestQ);
      printf("%s: Error creating MSG_VOICE_PROCESS_REQ_Q queue key:%d\n",
            gProcessName, WrMsgQKey);
      return false;
   }

   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetMsgType 
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::GetMsgType(UINT16 & channelId, long &lMsgType)
{

   lMsgType = long((channelId -1) / mMaxNoOfChanPerCard);
   lMsgType += 1;
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::PlayBlockingMode(MpRequestDetails &lReq,MpResponseDetails &lResp)
{
   long lMsgType = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d SeqNo:%u File:%s",
         gReferenceNum,
         lReq.mpReqData.playMsg.circuitId,
         lReq.cmdType,
         lReq.mpReqData.playMsg.seqNum,
         &lReq.mpReqData.playMsg.voiceFile[0][0]);

   modeOption lMode = BLOCKING;
   if(true == GetMsgType(lReq.mpReqData.playMsg.circuitId, lMsgType))
   {
      //Write request and wait for Play response
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Blocking Mode Play Req send success\n",gProcessName,lLogText););
         lResp.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
         lResp.cmdType = lReq.cmdType;
         if (true == RxResponse(lReq.mpReqData.playMsg.circuitId, lResp,lMode))
         {
            T(gTrace,printf("%s: %s Blocking Mode Play Resp recv success\n",gProcessName,lLogText););
         }
         else
         {
            TERR(gTrace,printf("%s: %s Blocking Mode Play Resp recv fail\n",gProcessName,lLogText););
            return false;
         }
      }
      else
      {
         TERR(gTrace,printf("%s: %s Blocking Mode Play Req send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s Blocking Mode Play GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::PlayNonBlockingMode(MpRequestDetails &lReq)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   long lMsgType = 0;

   MpResponseDetails lResp;
   memset(&lResp, 0, sizeof(MpResponseDetails));

   modeOption lMode = BLOCKING;
   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d SeqNo:%u File:%s",
         gReferenceNum,
         lReq.mpReqData.playMsg.circuitId,
         lReq.cmdType,
         lReq.mpReqData.playMsg.seqNum,
         &lReq.mpReqData.playMsg.voiceFile[0][0]);

   lReq.mpReqData.playMsg.nonBlocking=true;
   if(true == GetMsgType(lReq.mpReqData.playMsg.circuitId, lMsgType))
   {
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Play Req (NB) send success\n",gProcessName,lLogText););
         lResp.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
         lResp.cmdType = lReq.cmdType;
         if (true == RxResponse(lReq.mpReqData.playMsg.circuitId, lResp,lMode))
         {
            T(gTrace,printf("%s: %s Blocking Mode Play Ack recv success\n",gProcessName,lLogText););
         }
         return true;
      }
      else
      {

         TERR(gTrace,printf("%s: %s Play Req (NB) send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::GetDtmfDigits(MpRequestDetails &lReq,MpResponseDetails &lResp)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   long lMsgType = 0;

   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d NumOfDig:%d InDigTmOut:%u TermChar:%s SeqNo:%u",
         gReferenceNum,
         lReq.mpReqData.getDigit.circuitId,
         lReq.cmdType,
         lReq.mpReqData.getDigit.numOfDig,lReq.mpReqData.getDigit.interDigitTimeOut,
         lReq.mpReqData.getDigit.teminateChars,
         lReq.mpReqData.getDigit.seqNum);

   modeOption lMode = BLOCKING;
   if(true == GetMsgType(lReq.mpReqData.getDigit.circuitId, lMsgType))
   {
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Req send success\n",gProcessName,lLogText););

         lResp.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
         lResp.cmdType = lReq.cmdType;
         if(true == RxResponse(lReq.mpReqData.playMsg.circuitId, lResp,lMode))
         {
            T(gTrace,printf("%s: %s Resp recv success\n",gProcessName,lLogText););
         }
         else
         {
            TERR(gTrace,printf("%s: %s Resp recv fail\n",gProcessName,lLogText););
            return false;
         }
      }
      else
      {
         TERR(gTrace,printf("%s: %s Req send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetDtmfDigits 
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::GetDtmfDigits(MpRequestDetails &lReq,MpResponseDetails &lResp, TEXT *pauseDtmf, TEXT *resumeDtmf, TEXT *forwardDtmf, TEXT *rewindDtmf)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   long lMsgType = 0;

   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d NumOfDig:%d InDigTmOut:%u TermChar:%s SeqNo:%u",
         gReferenceNum,
         lReq.mpReqData.getDigit.circuitId,
         lReq.cmdType,
         lReq.mpReqData.getDigit.numOfDig,lReq.mpReqData.getDigit.interDigitTimeOut,
         lReq.mpReqData.getDigit.teminateChars,
         lReq.mpReqData.getDigit.seqNum);

   modeOption lMode = BLOCKING;
   if(true == GetMsgType(lReq.mpReqData.getDigit.circuitId, lMsgType))
   {
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Req send success\n",gProcessName,lLogText););

         lResp.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
         lResp.cmdType = lReq.cmdType;
         if(true == RxResponse(lReq.mpReqData.playMsg.circuitId, lResp,lMode))
         {
            T(gTrace,printf("%s: %s Resp recv success\n",gProcessName,lLogText););

            //Compare DTMF for pause/resume/fwd/Rwnd 
            //Send Corresponding request
            //And collect the DTMF
            if( false == ValidateDtmfAndSetOperation( lReq, lResp, pauseDtmf, resumeDtmf, forwardDtmf, rewindDtmf) )
            {
               GetDtmfDigits(lReq, lResp);
            }
         }
         else
         {
            TERR(gTrace,printf("%s: %s Resp recv fail\n",gProcessName,lLogText););
            return false;
         }
      }
      else
      {
         TERR(gTrace,printf("%s: %s Req send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------

BOOLEAN MediaInterface::RecordVoice(MpRequestDetails &lReq,MpResponseDetails &lResp)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	long lMsgType = 0;
	
	sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d RecTime:%u SeqNo:%u",
			  gReferenceNum,
			  lReq.mpReqData.recordMsg.circuitId,
			  lReq.cmdType,
			  lReq.mpReqData.recordMsg.recTime,lReq.mpReqData.recordMsg.seqNum);
	
	modeOption lMode = BLOCKING;
	
	if(true == GetMsgType(lReq.mpReqData.recordMsg.circuitId, lMsgType))
	{
		if(true == TxRequest(lReq,lMsgType))
		{
			T(gTrace,printf("%s: %s Req send success\n",gProcessName,lLogText););

			lResp.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
			lResp.cmdType = lReq.cmdType;
			if(true == RxResponse(lReq.mpReqData.playMsg.circuitId, lResp,lMode))
			{
				T(gTrace,printf("%s: %s RECTIME:%d Resp recv success\n",gProcessName,lLogText, 
                     lResp.mpRspData.recordMsg.recTime););
			}
			else
			{
				TERR(gTrace,printf("%s: %s Resp recv fail\n",gProcessName,lLogText););
				return false;
			}
		}
		else
		{
         TERR(gTrace,printf("%s: %s Req send fail\n",gProcessName,lLogText););
			return false;
		}
		
	}
	else
	{
		TERR(gTrace,printf("%s: %s GetMsgType fail\n",gProcessName,lLogText););
		return false;
	}

	return true;

}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------

BOOLEAN MediaInterface::SwitchCalls(MpRequestDetails &lReq,MpResponseDetails &lResp)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	long lMsgType = 0;

	sprintf(lLogText,"CID_1:%06d CID_2:%06d CmdType:%d SeqNo:%u",
			   lReq.mpReqData.switchChnl.circuitId_1,
				lReq.mpReqData.switchChnl.circuitId_2,
				lReq.cmdType,
				lReq.mpReqData.switchChnl.seqNum);

	modeOption lMode = BLOCKING;
	if(true == GetMsgType(lReq.mpReqData.switchChnl.circuitId_1, lMsgType))
	{

		if(true == TxRequest(lReq,lMsgType))
		{
			T(gTrace,printf("%s: %s Req send success\n",gProcessName,lLogText););
			lResp.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
			lResp.cmdType = lReq.cmdType;
			if(true == RxResponse(lReq.mpReqData.playMsg.circuitId, lResp,lMode))
			{
				T(gTrace,printf("%s: %s Resp recv success\n",gProcessName,lLogText););
			}
			else
			{
				TERR(gTrace,printf("%s: %s Req recv fail\n",gProcessName,lLogText););
				return false;
			}
		}
		else
		{
			TERR(gTrace,printf("%s: %s Req send fail\n",gProcessName,lLogText););
			return false;
		}
	}
	else
	{
		TERR(gTrace,printf("%s: %s GetMsgType fail\n",gProcessName,lLogText););
		return false;
	}
	return true;
}

BOOLEAN MediaInterface::PrepareForNewCall() 
{
   mStopRequested = false;
   return true;
}

BOOLEAN MediaInterface::StopProcessing(MpRequestDetails &lReq,MpResponseDetails &lResp)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	long lMsgType = 0;
	 sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d SeqNo:%u",
			  gReferenceNum,
			  lReq.mpReqData.stopMsg.circuitId,
			  lReq.cmdType, lReq.mpReqData.stopMsg.seqNum);
	 //modeOption lMode = BLOCKING;
    // TBC : Komal : disabled mStopRequested on experimental basis to receive the Record Response from Media
    mStopRequested=true;
	 if(true == GetMsgType(lReq.mpReqData.stopMsg.circuitId,lMsgType))
	 {
		 if(true == TxRequest(lReq,lMsgType))
		 {
			 T(gTrace,printf("%s: %s Stop Request send success\n",gProcessName,lLogText););
			 lResp.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
			 lResp.cmdType = lReq.cmdType;
			 //if(true == RxResponse(lReq.mpReqData.playMsg.circuitId, lResp,lMode))
			 //{
			//	 T(gTrace,printf("%s: %s Resp recv success\n",gProcessName,lLogText););
			 //}
			 //else
			// {
			//	 TERR(gTrace,printf("%s: %s Req recv fail\n",gProcessName,lLogText););
			//	 return false;
			 //}
		 }
		 else
		 {
			 TERR(gTrace,printf("%s: %s Stop Req send fail\n",gProcessName,lLogText););
			 return false;
		 }
	 }
	 else
	 {
		 TERR(gTrace,printf("%s: %s GetMsgType fail\n",gProcessName,lLogText););
		 return false;
	 }
	 return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------

BOOLEAN MediaInterface::RxResponse(const UINT16 lMsgType, MpResponseDetails &lResp,modeOption &lMode)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	INT16 lMsgLen = sizeof(MpResponseDetails);
	msgQueueError      lQErr;

   EnumMediaCmdTypes lCmdType = lResp.cmdType;
   UINT32 lReqSeqNum = lResp.mpRspData.dummy.seqNum;

	while(false==mStopRequested)
	{
      memset(&lResp, 0, sizeof(MpResponseDetails));
		lMsgLen = sizeof(MpResponseDetails);
		lQErr = RdResponseQ.ReadMsg((void*)& lResp,lMsgLen, lMsgType, lMode);
		sprintf(lLogText,"RefNum:%08X CID:%06d ReqSeqNum:%08X RespSeqNum:%08X RspMsg:%d CmdType:%d(%d) MErr:%d QErr:%d",
				gReferenceNum,
				lMsgType,
            lReqSeqNum,
            lResp.mpRspData.dummy.seqNum,
				lResp.rspMsg,
            lResp.cmdType,
            lCmdType,
            lResp.voiceProcessErr, 
            lQErr);
		if(Q_SUCCESS == lQErr)
		{
         if((lResp.mpRspData.dummy.seqNum == lReqSeqNum) &&
		((lCmdType == lResp.cmdType) || ((lCmdType+1)== lResp.cmdType)))
            break;
         else
         {
            TERR(gTrace,printf("%s: %s Received Message in not valid in Current Context\n", gProcessName, lLogText););
            //gLog.GenerateLog(VXML_21, LOG_LEVEL_1, lLogText);
         }
		}
		else if(lQErr != Q_READ_SIGNAL_ERR)
		{	
			TERR(gTrace,printf("%s: %s Q Read Failed\n",gProcessName,lLogText););
			MediaUtil::HandleQueueReadError (lQErr,RdResponseQ);
			return false;
		}
      else 
      {
         T(gTrace,printf("%s: %s Signal Received\n",gProcessName,lLogText););
         if(mStopRequested)
         {
            T(gTrace,printf("%s: %s VXML Terminate Command Received\n",gProcessName,lLogText););
         }
      }
   }
   mStopRequested=false;
   T(gTrace,printf("%s: %s Q Read Success\n",gProcessName,lLogText););
   return true;
}


BOOLEAN MediaInterface::RxResponseNonBlocking(const UINT16 lMsgType, MpResponseDetails &lResp,int noOfAttempts)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	INT16 lMsgLen = sizeof(MpResponseDetails);
   modeOption lMode = NON_BLOCKING;

	msgQueueError      lQErr;

   EnumMediaCmdTypes lCmdType = lResp.cmdType;
   UINT32 lReqSeqNum = lResp.mpRspData.dummy.seqNum;

	while(noOfAttempts>0)
	{
      memset(&lResp, 0, sizeof(MpResponseDetails));
		lMsgLen = sizeof(MpResponseDetails);
		lQErr = RdResponseQ.ReadMsg((void*)& lResp,lMsgLen, lMsgType, lMode);
		sprintf(lLogText,"RefNum:%08X CID:%06d ReqSeqNum:%08X RespSeqNum:%08X RspMsg:%d CmdType:%d(%d) MErr:%d QErr:%d",
				gReferenceNum,
				lMsgType,
            lReqSeqNum,
            lResp.mpRspData.dummy.seqNum,
				lResp.rspMsg,
            lResp.cmdType,
            lCmdType,
            lResp.voiceProcessErr, 
            lQErr);
		if(Q_SUCCESS == lQErr)
		{
         if((lResp.mpRspData.dummy.seqNum == lReqSeqNum) && ((lCmdType == lResp.cmdType) || ((lCmdType+1)== lResp.cmdType)))
            break;
         else
         {
            TERR(gTrace,printf("%s: %s Received Message in not valid in Current Context\n", gProcessName, lLogText););
            //gLog.GenerateLog(VXML_21, LOG_LEVEL_1, lLogText);
         }
		}
		else if(lQErr == Q_NO_MSG_WITH_TYPE)
      {
         usleep(100000);
      }
      else if(lQErr != Q_READ_SIGNAL_ERR)
      {	
         TERR(gTrace,printf("%s: %s Q Read Failed\n",gProcessName,lLogText););
         MediaUtil::HandleQueueReadError (lQErr,RdResponseQ);
         return false;
      }
      else 
      {
         T(gTrace,printf("%s: %s Signal Received\n",gProcessName,lLogText););
         if(mStopRequested)
         {
            T(gTrace,printf("%s: %s VXML Terminate Command Received\n",gProcessName,lLogText););
         }
      }
      noOfAttempts--;
   }
   T(gTrace,printf("%s: %s Q Read Success\n",gProcessName,lLogText););
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------

BOOLEAN MediaInterface::TxRequest(MpRequestDetails &lReq,long &lMsgType)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   msgQueueError      lQErr;
	INT16 lMsgLen = sizeof(MpRequestDetails);

//	lReq.mpReqData.playMsg.circuitId = ((lReq.mpReqData.playMsg.circuitId -1) % mMaxNoOfChanPerCard);

	lQErr = WrRequestQ.WriteMsg((void*)& lReq,lMsgLen, lMsgType, NON_BLOCKING);
	sprintf(lLogText,"CmdType:%d SeqNum:%u clgNo:%s cldNo:%s msgType:%lu QErr:%d",
			lReq.cmdType, lReq.mpReqData.dummy.seqNum, lReq.clgNumber,lReq.cldNumber,lMsgType, lQErr);
	if(lQErr != Q_SUCCESS)
	{
		TERR(gTrace,printf("%s: %s Q Write Failed\n",gProcessName,lLogText););
		MediaUtil::HandleQueueWriteError(lQErr, WrRequestQ,(void*)& lReq,lMsgLen,lMsgType,NON_BLOCKING);
		return false;
  	}
   
	//T(gTrace,printf("%s: %s Q Write Success\n",gProcessName,lLogText););
	return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : GetModuleId
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------

BOOLEAN MediaInterface::ValidateDtmfAndSetOperation(MpRequestDetails lMpRequestDetails , MpResponseDetails lMpResponseDetails,TEXT* lDtmfForPause, TEXT* lDtmfForResume, TEXT* lDtmfForForward, TEXT* lDtmfForRewind)
{
	//TEXT              lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   MpRequestDetails  lTempMpRequestDetails;
   
   
   if( lMpResponseDetails.mpRspData.getDigit.rspDigits[0] == lDtmfForPause[0])
   {
      //PAUSE
      memset( &lTempMpRequestDetails, 0, sizeof(MpRequestDetails));
      lTempMpRequestDetails.cmdType           = MC_PAUSE;
      strcpy(lTempMpRequestDetails.cldNumber, lMpRequestDetails.cldNumber);
      strcpy(lTempMpRequestDetails.clgNumber, lMpRequestDetails.clgNumber);
      lTempMpRequestDetails.mpReqData.pauseMsg.seqNum = lMpRequestDetails.mpReqData.playMsg.seqNum;
      lTempMpRequestDetails.mpReqData.pauseMsg.circuitId= lMpRequestDetails.mpReqData.playMsg.circuitId;
      SendPauseReqAndRcvResp( lTempMpRequestDetails);
      return false;
   }
   else if( lMpResponseDetails.mpRspData.getDigit.rspDigits[0] == lDtmfForResume[0])
   {
     //RESUME 
      memset( &lTempMpRequestDetails, 0, sizeof(MpRequestDetails));
      lTempMpRequestDetails.cmdType           = MC_RESUME;
      strcpy(lTempMpRequestDetails.cldNumber, lMpRequestDetails.cldNumber);
      strcpy(lTempMpRequestDetails.clgNumber, lMpRequestDetails.clgNumber);
      lTempMpRequestDetails.mpReqData.resumeMsg.seqNum = lMpRequestDetails.mpReqData.playMsg.seqNum;
      lTempMpRequestDetails.mpReqData.resumeMsg.circuitId= lMpRequestDetails.mpReqData.playMsg.circuitId;
      SendResumeReqAndRcvResp( lTempMpRequestDetails);
      return false;
   } 
   else if( lMpResponseDetails.mpRspData.getDigit.rspDigits[0] == lDtmfForForward[0])
   {
     //FORWARD
      memset( &lTempMpRequestDetails, 0, sizeof(MpRequestDetails));
      lTempMpRequestDetails.cmdType           = MC_FORWARD;
      strcpy(lTempMpRequestDetails.cldNumber, lMpRequestDetails.cldNumber);
      strcpy(lTempMpRequestDetails.clgNumber, lMpRequestDetails.clgNumber);
      lTempMpRequestDetails.mpReqData.forwardMsg.seqNum = lMpRequestDetails.mpReqData.playMsg.seqNum;
      lTempMpRequestDetails.mpReqData.forwardMsg.circuitId= lMpRequestDetails.mpReqData.playMsg.circuitId;
      SendForwardReqAndRcvResp( lTempMpRequestDetails);
      return false;
   } 
   else if( lMpResponseDetails.mpRspData.getDigit.rspDigits[0] == lDtmfForRewind[0])
   {
      //REWIND
      memset( &lTempMpRequestDetails, 0, sizeof(MpRequestDetails));
      lTempMpRequestDetails.cmdType           = MC_REWIND;
      strcpy(lTempMpRequestDetails.cldNumber, lMpRequestDetails.cldNumber);
      strcpy(lTempMpRequestDetails.clgNumber, lMpRequestDetails.clgNumber);
      lTempMpRequestDetails.mpReqData.rewindMsg.seqNum = lMpRequestDetails.mpReqData.playMsg.seqNum;
      lTempMpRequestDetails.mpReqData.rewindMsg.circuitId= lMpRequestDetails.mpReqData.playMsg.circuitId;
      SendRewindReqAndRcvResp( lTempMpRequestDetails );
      return false;
   }
   
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : SendPauseReqAndRcvResp 
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::SendPauseReqAndRcvResp(MpRequestDetails lReq)
{
   long lMsgType = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   MpResponseDetails lMpResponseDetails;
   memset(&lMpResponseDetails, 0, sizeof( MpResponseDetails));
   
   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d SeqNo:%u",
         gReferenceNum,
         lReq.mpReqData.pauseMsg.circuitId,
         lReq.cmdType,
         lReq.mpReqData.pauseMsg.seqNum);

   modeOption lMode = BLOCKING;

   if(true == GetMsgType(lReq.mpReqData.pauseMsg.circuitId, lMsgType))
   {
      //Write request and wait for Play response
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Pause Req send success\n",gProcessName,lLogText););
         lMpResponseDetails.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
	 lMpResponseDetails.cmdType = lReq.cmdType;
         if (true == RxResponse(lReq.mpReqData.pauseMsg.circuitId, lMpResponseDetails, lMode))
         {
            T(gTrace,printf("%s: %s Pause Resp recv success\n",gProcessName,lLogText););
         }
         else
         {
            TERR(gTrace,printf("%s: %s Pause Resp recv fail\n",gProcessName,lLogText););
            return false;
         }
      }
      else
      {
         TERR(gTrace,printf("%s: %s Pause Req send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s Pause GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : SendResumeReqAndRcvResp 
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::SendResumeReqAndRcvResp(MpRequestDetails lReq)
{
   long lMsgType = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   MpResponseDetails lMpResponseDetails;
   memset(&lMpResponseDetails, 0, sizeof( MpResponseDetails));
   
   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d SeqNo:%u",
         gReferenceNum,
         lReq.mpReqData.resumeMsg.circuitId,
         lReq.cmdType,
         lReq.mpReqData.resumeMsg.seqNum);
   modeOption lMode = BLOCKING;

   if(true == GetMsgType(lReq.mpReqData.resumeMsg.circuitId, lMsgType))
   {
      //Write request and wait for Play response
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Resume Req send success\n",gProcessName,lLogText););
         lMpResponseDetails.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
	 lMpResponseDetails.cmdType = lReq.cmdType;
         if (true == RxResponse(lReq.mpReqData.resumeMsg.circuitId, lMpResponseDetails, lMode))
         {
            T(gTrace,printf("%s: %s Resume Resp recv success\n",gProcessName,lLogText););
         }
         else
         {
            TERR(gTrace,printf("%s: %s Resume Resp recv fail\n",gProcessName,lLogText););
            return false;
         }
      }
      else
      {
         TERR(gTrace,printf("%s: %s Resume Req send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s Resume GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : SendRewindReqAndRcvResp 
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::SendRewindReqAndRcvResp(MpRequestDetails lReq)
{
   long lMsgType = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   MpResponseDetails lMpResponseDetails;
   memset(&lMpResponseDetails, 0, sizeof( MpResponseDetails));
   
   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d SeqNo:%u",
         gReferenceNum,
         lReq.mpReqData.rewindMsg.circuitId,
         lReq.cmdType,
         lReq.mpReqData.rewindMsg.seqNum);

   modeOption lMode = BLOCKING;
   if(true == GetMsgType(lReq.mpReqData.rewindMsg.circuitId, lMsgType))
   {
      //Write request and wait for Play response
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Rewind Req send success\n",gProcessName,lLogText););
         lMpResponseDetails.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
	 lMpResponseDetails.cmdType = lReq.cmdType;
         if (true == RxResponse(lReq.mpReqData.rewindMsg.circuitId, lMpResponseDetails, lMode))
         {
            T(gTrace,printf("%s: %s Rewind Resp recv success\n",gProcessName,lLogText););
         }
         else
         {
            TERR(gTrace,printf("%s: %s Rewind Resp recv fail\n",gProcessName,lLogText););
            return false;
         }
      }
      else
      {
         TERR(gTrace,printf("%s: %s Rewind Req send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s Rewind GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
   return true;
}
//------------------------------------------------------------------------------
// METHOD NAME : SendForwardReqAndRcvResp 
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//------------------------------------------------------------------------------
BOOLEAN MediaInterface::SendForwardReqAndRcvResp(MpRequestDetails lReq)
{
   long lMsgType = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   MpResponseDetails lMpResponseDetails;
   memset(&lMpResponseDetails, 0, sizeof( MpResponseDetails));
   
   sprintf(lLogText,"RefNum:%08X CID:%06d CmdType:%d SeqNo:%u",
         gReferenceNum,
         lReq.mpReqData.forwardMsg.circuitId,
         lReq.cmdType,
         lReq.mpReqData.forwardMsg.seqNum);

   modeOption lMode = BLOCKING;
   if(true == GetMsgType(lReq.mpReqData.forwardMsg.circuitId, lMsgType))
   {
      //Write request and wait for Play response
      if(true == TxRequest(lReq,lMsgType))
      {
         T(gTrace,printf("%s: %s Forward Req send success\n",gProcessName,lLogText););
         lMpResponseDetails.mpRspData.dummy.seqNum = lReq.mpReqData.dummy.seqNum;
	 lMpResponseDetails.cmdType = lReq.cmdType;
         if (true == RxResponse(lReq.mpReqData.forwardMsg.circuitId, lMpResponseDetails, lMode))
         {
            T(gTrace,printf("%s: %s Forward Resp recv success\n",gProcessName,lLogText););
         }
         else
         {
            TERR(gTrace,printf("%s: %s Forward Resp recv fail\n",gProcessName,lLogText););
            return false;
         }
      }
      else
      {
         TERR(gTrace,printf("%s: %s Forward Req send fail\n",gProcessName,lLogText););
         return false;
      }
   }
   else
   {
      TERR(gTrace,printf("%s: %s Forward GetMsgType fail\n",gProcessName,lLogText););
      return false;
   }
   return true;
}
