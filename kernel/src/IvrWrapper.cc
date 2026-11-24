static const char* id="@(#) $Id: IvrWrapper.cc,v 1.1 2011/03/08 08:24:32 root Exp $" ;
//---------------------------------------------------------------------
// NAME      IvrWrapper.cc
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// DESCRIPTION
//
// Originated : Anupama R                    Date : 21-JAN-2011
//---------------------------------------------------------------------
#include "IvrWrapper.h"

extern TEXT gProcessName[IVR_MAX_PROCESS_NAME];

//---------------------------------------------------------------
// METHOD      : IvrWrapper() 
// DESCRIPTION : Constructor 
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
IvrWrapper::IvrWrapper()
{

}

//---------------------------------------------------------------
// METHOD      : IvrWrapper() 
// DESCRIPTION : Destructor 
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
IvrWrapper::~IvrWrapper()
{
}

//---------------------------------------------------------------
// METHOD      : Init()
// DESCRIPTION : Initializes DbManger,Reads Config data 
//               and Creates Queues
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN IvrWrapper::Init() 
{
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";
  
	if (false == ReadConfig())
   {
		sprintf(lLogText, "ReadConfig failed");
		gLog.GenerateLog(GSYS04, lLogText);
		TERR(gTrace, printf("%s: %s. Exiting...\n", gProcessName, lLogText);)
		return false;
   }

   if (false == CreateQueue())
   {
		sprintf(lLogText, "CreateQueue failed");
		gLog.GenerateLog(GSYS04, lLogText);
		TERR(gTrace, printf("%s: %s. Exiting...\n", gProcessName, lLogText);)
	   return false;
   }
		
   return true;
}
   
//-------------------------------------------------------------
// METHOD      : ReadConfig()
// DESCRIPTION : Reads Config Data
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//--------------------------------------------------------------
BOOLEAN IvrWrapper::ReadConfig() 
{
   TEXT         lLogText[MAX_LOG_TEXT_LEN];
   CfgRead      lCfgRead(gProcessName);
   
   if (false == lCfgRead.CfgInit(IVR_IPC_CFG))
	{
		sprintf (lLogText,"Could not open file %s", IVR_IPC_CFG);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
      gLog.GenerateLog(GSYS09, lLogText);
		return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum("MSG_WRAP_INPUT_Q_RCV", 
				mReadQKey, 
				IVR_MIN_MSG_Q_KEY, 
				IVR_MAX_MSG_Q_KEY))
	{
		TERR(gTrace, printf("%s: Config read failed for %s\n", gProcessName, "MSG_WRAP_INPUT_Q_RCV");)
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_WRAP_INPUT_Q_RCV = %d\n", gProcessName, mReadQKey);)
   }
   
   if (CFG_OK != lCfgRead.GetConfigNum("MSG_CALL_ALERT_Q_RCV", mWriteQKey, 
				IVR_MIN_MSG_Q_KEY, 
				IVR_MAX_MSG_Q_KEY))
	{
		TERR(gTrace, printf("%s: Config read failed for %s\n", gProcessName, "MSG_CALL_ALERT_Q_RCV");)
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_CALL_ALERT_Q_RCV = %d\n", gProcessName, mWriteQKey);)
   }
   lCfgRead.CfgDeInit();
	
   if (true != lCfgRead.CfgInit(IVR_KER_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s",
                         IVR_KER_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }

	INT32 lTempVal=0;

   if (CFG_OK != lCfgRead.GetConfigNum("PROTOCOL_TYPE",
                                       lTempVal,
                                       IVR_SIGNALING_ISUP,
                                       IVR_SIGNALING_SIP) )
   {
      TERR(gTrace, printf("%s: Config read failed for PROTOCOL_TYPE\n",
                          gProcessName);)
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
		mSignalMsgType = (EnumSignalingProtocol)lTempVal;
      T(gTrace, printf("%s: PROTOCOL_TYPE = %d\n",
                       gProcessName, mSignalMsgType);)
   }

	lCfgRead.CfgDeInit();

   return true;
}

//--------------------------------------------------------------
// METHOD      : CreateQueue()
// DESCRIPTION : Creates Queues
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN IvrWrapper::CreateQueue()
{
   msgQueueError lQErr = Q_SUCCESS;
   TEXT          lLogText[MAX_LOG_TEXT_LEN];

   lQErr = mReadQObj.Create((key_t)mReadQKey, IPC_CREAT | IPC_PERM);
	if (lQErr != Q_SUCCESS)
   {
      IvrUtil::HandleQueueCreateError(lQErr, mReadQObj);
      sprintf (lLogText, "Error creating queue %d", mReadQKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }
   
   lQErr = mWriteQObj.Create((key_t)mWriteQKey, IPC_CREAT | IPC_PERM);
   if (lQErr != Q_SUCCESS)
   {
      IvrUtil::HandleQueueCreateError(lQErr, mWriteQObj);
      sprintf (lLogText, "Error creating queue %d", mWriteQKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }
	
   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadRequest
// DESCRIPTION : 
// PARAMETER   : DBMsg structure
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrWrapper::ReadRequest()
{
   msgQueueError  lQErr = Q_SUCCESS;
   INT16          lMsgLen;
   TEXT           lLogText[MAX_LOG_TEXT_LEN];

	memset(&mGenIsupMsg, 0, sizeof(GenIsupMsg ));
	
   lMsgLen = sizeof(GenIsupMsg);
	
	lQErr   = mReadQObj.ReadMsg((void*)&mGenIsupMsg, lMsgLen, 0, BLOCKING);
   
	//Check for the read error
   if(lQErr != Q_SUCCESS)
   {
  		IvrUtil::HandleQueueReadError(lQErr, mReadQObj);
      return false;
   }
	snprintf(lLogText,MAX_LOG_TEXT_LEN,
			"ISUP Msg Read CID-%06d Imsg:%d Clg:%s Cld:%s",
			mGenIsupMsg.circuitId,
			mGenIsupMsg.isupMsgType,
			mGenIsupMsg.isupIam.callingNumber,
			mGenIsupMsg.isupIam.calledNumber);
	T(gTrace, printf("%s: %s\n", gProcessName, lLogText);)

	//gLog.GenerateLog(RHP_DBI1, lLogText);
	
   return true; 
}
//-------------------------------------------------------------
// METHOD      : WriteResponse
// DESCRIPTION : Sends req to ReqHdlr 
// PARAMETER   : DBMsg
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrWrapper:: WriteMsgToKernelQueue( KernelMsg lKernelMsg )
{
   msgQueueError   lQErr;
   TEXT            lLogText[MAX_LOG_TEXT_LEN] = "";
	INT16				 lMsgLen;
   
	lMsgLen = sizeof(lKernelMsg);
   lQErr   = mWriteQObj.WriteMsg((void*)&lKernelMsg, 
                                   lMsgLen, 
                                   1, 
                                   BLOCKING);
   if (Q_SUCCESS != lQErr)
   {
      lMsgLen = sizeof(lKernelMsg);
   	if (true != IvrUtil::HandleQueueWriteError(lQErr, 
		                                            mWriteQObj, 
	   														  &lKernelMsg, 
												              lMsgLen, 
																  1,
																  BLOCKING))
		{
      	snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: %s Error %s", gProcessName, 
                            lLogText, 
                            IvrUtil::ConvertQueErrToStr(lQErr));
   	//	gLog.GenerateLog( RHP_DBI4, lLogText);
			TERR(gTrace, printf("%s: %s Write Request to kernel Msg Queue failed.\n", 
                             gProcessName, lLogText);)
	   	return false;
		}
   }
	T(gTrace, printf("%s: Write Request to kernel Msg Queue\n", gProcessName);)
   //gLog.GenerateLog( RHP_DBI3, lLogText);
   return true;
}
//-------------------------------------------------------------
// METHOD      : ReadAndProcessRequest 
// DESCRIPTION : 
// PARAMETER   : DBMsg structure
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrWrapper::ReadAndProcessRequest()
{
	switch(mSignalMsgType)
	{
			case IVR_SIGNALING_ISUP:
				return this->ProcessISUPMsg();

		   case IVR_SIGNALING_SIP: 
			 //  return this->ProcessSIPMsg(); 
				break;

			default :
				break;	
	}
	
	return false;
}	
//-------------------------------------------------------------
// METHOD      : ReadRequest
// DESCRIPTION : 
// PARAMETER   : DBMsg structure
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrWrapper::ProcessISUPMsg()
{
	KernelMsg   lKernelMsg;
   TEXT            lLogText[MAX_LOG_TEXT_LEN] = "";

	memset ( &mGenIsupMsg, 0, sizeof(GenIsupMsg) );
	memset ( &lKernelMsg,  0, sizeof(KernelMsg ) );
	
	//Read  Isup Message
  if( false == this->ReadRequest())
  {
	  return false;
  }
  
  //Convert Isup Request
  this->ConvertToKernelMsg(mGenIsupMsg,  lKernelMsg);
  
  snprintf(lLogText,MAX_LOG_TEXT_LEN,
		  "CID-%06d Kmsg:%d Imsg:%d Clg:%s Cld:%s",
		  lKernelMsg.isupMsg.circuitId,
		  lKernelMsg.kernelMsgType,
		  lKernelMsg.isupMsg.isupMsgType,
		  lKernelMsg.isupMsg.isupIam.callingNumber,
		  lKernelMsg.isupMsg.isupIam.calledNumber);

	T(gTrace, printf("%s: Writing %s\n", gProcessName, lLogText);)
  //Write Request
  if( false == this->WriteMsgToKernelQueue( lKernelMsg ))
  {
	  return false;
  }

  return true;

}
//-------------------------------------------------------------
// METHOD      : ConvertToKernelMsg 
// DESCRIPTION : 
// PARAMETER   : DBMsg structure
// RETURN      : BOOLEAN
//-------------------------------------------------------------
void IvrWrapper::ConvertToKernelMsg(GenIsupMsg  &genIsupMsg , KernelMsg &kernelMsg)
{
   TEXT            lLogText[MAX_LOG_TEXT_LEN] = "";
	snprintf(lLogText,MAX_LOG_TEXT_LEN,
			"Conv Msg Read CID-%06d Imsg:%d Clg:%s Cld:%s",
			genIsupMsg.circuitId,
			genIsupMsg.isupMsgType,
			genIsupMsg.isupIam.callingNumber,
			genIsupMsg.isupIam.calledNumber);
	T(gTrace, printf("%s: %s\n", gProcessName, lLogText);)

		kernelMsg.kernelMsgType = ISUP_MSG;
	memcpy(&kernelMsg.isupMsg, &genIsupMsg, sizeof(genIsupMsg));
}

