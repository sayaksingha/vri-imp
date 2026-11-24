static const char* id="@(#) $Id: PromptApi.cc,v 1.1.1.1.2.6.2.5 2018/02/26 23:19:43 mram Exp $" ;
//-----------------------------------------------------------------------------
// NAME
// PromptApi.cc
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
// Originated : 21-SEPT-2010            Anupama R
//------------------------------------------------------------------------------
#include  "PromptApi.h"
#include "IvrChannelApi.h"
#include "IvrEvrApi.h"

extern TEXT           gProcessName[MAX_PROCESS_NAME];
extern CTrace         gTrace;
extern Log            gLog;
extern unsigned int   gReferenceNum;
extern TEXT           gCallingNumber[MAX_MSISDN_LEN + 1];
extern TEXT           gCalledNumber[MAX_MSISDN_LEN + 1];

extern BOOLEAN        gIsAudioOpSupported;
extern TEXT           gPauseDtmf[MAX_DTMF_LEN];
extern TEXT           gResumeDtmf[MAX_DTMF_LEN];
extern TEXT           gForwardDtmf[MAX_DTMF_LEN];
extern TEXT           gRewindDtmf[MAX_DTMF_LEN];

PromptApi		       gPromptApi; 
extern IvrChannelApi  gChannelApi;
extern IvrQueApi      gIvrQueApi;

TEXT              glLogText[MAX_LOG_TEXT_LEN];

IvrEvrApi         gIvrEvrApi;
IvrEvrMsg         gIvrEvrMsg;
EnumIvrEvent      gIvrEvrEvent;
EnumIvrCause      gIvrEvrCause;
EnumIvrReason     gIvrEvrReason;
//---------------------------------------------------------------
// METHOD      : PromptApi 
// DESCRIPTION : Constructor
// PARAMETER   : no arguments
// RETURN      : 
//---------------------------------------------------------------
PromptApi::PromptApi()
{
	mIsPlayInProgress = false;
   mIsRecInProgress  = false;
   mIsStopRequested = false;
   mIsDtmfInProgress = false;
}
//---------------------------------------------------------------
// METHOD      : PromptApi 
// DESCRIPTION : Constructor
// PARAMETER   : no arguments
// RETURN      : 
//---------------------------------------------------------------
PromptApi::~PromptApi()
{
}	
//---------------------------------------------------------------
// METHOD      : SetCirciutId 
// DESCRIPTION : to set the circuit id for playing on it
// PARAMETER   : int  
// RETURN      : void
//---------------------------------------------------------------
void PromptApi::SetCirciutId( int circiutId)
{
	TEXT lLogText[MAX_LOG_TEXT_LEN+1] = "";

	mCircuitId = circiutId;
	snprintf( lLogText, MAX_LOG_TEXT_LEN, "CID:%06d ChannelId Successfully Set", mCircuitId);
	T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
}
//---------------------------------------------------------------
// METHOD      : InitPromptApi 
// DESCRIPTION : Api called by VXML processor to Initialize MediaLibrary API 
// PARAMETER   : no arguments
// RETURN      : bool 
//---------------------------------------------------------------
BOOLEAN PromptApi::InitPromptApi( )
{
	TEXT lLogText[MAX_LOG_TEXT_LEN+1] = "";

   TEXT *lpTempPtr;

   lpTempPtr = getenv("IVR_BASE_PATH");
   if (NULLPTR == lpTempPtr)
      strncpy (mIvrBasePath, IVR_BASE_PATH, MAX_PATH);
   else
      strncpy (mIvrBasePath, lpTempPtr, MAX_PATH);

   if( false == mMediaLibrary.Init())
   {
      snprintf( lLogText, MAX_LOG_TEXT_LEN, "Media Library Initialization failed");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, mLogText);	
      return false;
   }
   snprintf( lLogText, MAX_LOG_TEXT_LEN, "Media Library Initialization Success");

   if (false == mBillingApi.Init(IVR_VXML_PROC))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Ivr Billing API Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   mIsBillingInProgress = false;
   mIsStopRequested     = false;
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   return true;

}	
//---------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::ReadConfig()
{
	CfgRead        lCfgRead(gProcessName);
	TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	ConfigReturn   lCfgRet;

	//open file 
	if (true != lCfgRead.CfgInit(IVR_IPC_CFG) )
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", IVR_IPC_CFG);
		gLog.GenerateLog(GSYS09, lLogText);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("PLAY_FILE_SHM_KEY",
					mFileShmKey,
					IVR_MIN_SHM_KEY,
					IVR_MAX_SHM_KEY)))
	{
		snprintf(mLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s \n",
				"PLAY_FILE_SHM_KEY");
		TERR(gTrace, printf("%s: Config read failed for %s\n",
					gProcessName, "PLAY_FILE_SHM_KEY"););
		gLog.GenerateLog(GSYS11, mLogText);
		lCfgRead.CfgDeInit();
		return false;
	}
	else
	{
		T(gTrace, printf("%s: PLAY_FILE_SHM_KEY=%d\n", gProcessName, mFileShmKey);)
   }
   lCfgRead.CfgDeInit();

	return true;
}	
//---------------------------------------------------------------
// METHOD      : ClearStopFlag
// DESCRIPTION :
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
void PromptApi::ClearStopFlag()
{
   //this->ProcessEndRequest();
   mIsStopRequested = false;
   mMediaLibrary.PrepareForNewCall();
}	

//---------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION :
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
void PromptApi::ReloadConfig()
{
	//Reload all the configurations
}	
//---------------------------------------------------------------
// METHOD      : CreateHashTable 
// DESCRIPTION :
// PARAMETER   : no arguments
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::CreateHashTable()
{
	// Create the Hash Table
	TEXT        lLogText[MAX_LOG_TEXT_LEN];
	INT32       lShmId;
	INT32       lSemId;
	UINT32      lBucketCount     = 100;
	BOOLEAN     lShmCreated      = false;
	
	lShmCreated = false;  //make it true when Shared memory created Success

	mHashTbl.SetBucketCount(lBucketCount);

	mHashTableSize = mHashTbl.GetHashTableSize();

	lShmId = shmget(mFileShmKey, mHashTableSize, IPC_PERM);

	if(lShmId < 0)
	{
		lShmId = shmget((key_t)mFileShmKey, mHashTableSize, IPC_PERM|IPC_CREAT);

		if (lShmId < 0)
		{
			sprintf(lLogText, "Shmget Failed ShmId:%d ShmKey:%d Size:%d",
					lShmId, 
					mFileShmKey, 
					mHashTableSize);

			TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
			//	gLog.GenerateLog(L0106, lLogText);
			return  false;
		}
		else
		{
			sprintf(lLogText, "Shmget Sucess ShmId:%d ShmKey:%d",
					lShmId, mFileShmKey);
			TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
			//	gLog.GenerateLog(L0105, lLogText);
		}
		lShmCreated = true;
	}

	lSemId = semget((key_t)mHashTblSemKey, 1, IPC_PERM);
	if(lSemId < 0)
	{
		lSemId = semget(mHashTblSemKey, 1, IPC_PERM|IPC_CREAT);

		if(lSemId < 0)
		{
			sprintf(lLogText,"Semget Failed SemId:%d SemKey:%d",lSemId, mHashTblSemKey);
			TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
			//	gLog.GenerateLog(L0106, lLogText);
			return  false;
		}
		else
		{
			sprintf(lLogText, "Semget Sucess SemId:%d ShmKey:%d", lShmId, mHashTblSemKey);
			TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
			//	gLog.GenerateLog(L0105, lLogText );
		}
	}

	mHashTableMemory = (TEXT*) shmat((key_t)lShmId, (void*)0, 0);
	if(mHashTableMemory == NULL)
	{
		sprintf(lLogText,"Error attaching to Shm memory ShmId=%d", lShmId);
		TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
	//	gLog.GenerateLog(L0120, lLogText);
		return  false;
	}

	mHashTbl.SetSemaphoreId(lSemId);
	if(mHashTbl.SetHashMemoryPtr((void*)mHashTableMemory, lShmCreated) != SHT_SUCCESS)
	{
		//	gLog.GenerateLog(GSYS06, lLogText );
		sprintf(lLogText,"Failed to set KW hash table memory pointer ");
		TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
		return false;
	}

	return true;
}	
//---------------------------------------------------------------
// METHOD      : InsertIntoHashTable 
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::InsertIntoHashTable(TEXT *hashKey , FileDetails fileDetails )
{
	INT16          lReturn;
	TEXT           lLogText[MAX_LOG_TEXT_LEN]    = "";


	lReturn = mHashTbl.Insert( hashKey,  fileDetails  );

	if(SHT_SUCCESS == lReturn )
	{
		sprintf(lLogText,"CID:%06d SC:%s HKY:%s Insert Pull keyword Details Into PullHashTable Success",
				mCircuitId,
				fileDetails.fetchedName, 
				hashKey);
		gLog.GenerateLog(IVR_PLIB01, 2, lLogText );
		T(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
		return true;
	}
	else if( SHT_KEY_ALREADY_EXIST   == lReturn  )
	{
		sprintf(lLogText,"CID:%06d SC:%s HKY:%s Key Already Exists",
				mCircuitId,
				fileDetails.fetchedName, 
				hashKey) ;
		gLog.GenerateLog(IVR_PLIB02, 2, lLogText );
		T(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
		return true;
	}
	else
	{
		sprintf(lLogText,"CID:%06d, HKY:%s Insert Pull keyword Details Into HashTable Failed ,Error:%d" ,
				mCircuitId,
				hashKey,
				lReturn);
		gLog.GenerateLog(IVR_PLIB03, lLogText );
		TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
		return false;
	}

	return true;
}
//---------------------------------------------------------------
// METHOD      : GetDataFromHash 
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::GetDataFromHash( TEXT * hashKey, FileDetails fileDetails)
{
	INT16          lReturn;
	TEXT           lLogText[MAX_LOG_TEXT_LEN] = "";

	lReturn = mHashTbl.Get(hashKey, fileDetails);	
	if (SHT_SUCCESS == lReturn)
	{
		sprintf(lLogText,"CID:%06d Fetching Pull keyword Details Success HKY:%s", mCircuitId, hashKey);
		gLog.GenerateLog(IVR_PLIB04,  2 ,lLogText );
		return true;
	}
	else
	{
		sprintf(lLogText, "CID:%06d HKY:%s Fetch Pull keyword from Hashtable failed Error:%d", mCircuitId, hashKey, lReturn);
	 	gLog.GenerateLog(IVR_PLIB05, lLogText );
		return false;
	}
}	
//---------------------------------------------------------------
// METHOD      : DeleteFromHashTable 
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::DeleteFromHashTable(TEXT* hashKey, FileDetails fileDetails)
{
	INT16          lReturn;
	TEXT           lLogText[MAX_LOG_TEXT_LEN]    = "";

	lReturn = mHashTbl.Delete( hashKey  );
	if(SHT_SUCCESS == lReturn )
	{
		sprintf(lLogText,"CID:%06d SC:%s HKY:%s delete Pull keyword Details Into PullHashTable Success",
				mCircuitId,
				fileDetails.fetchedName,
				hashKey );
		gLog.GenerateLog(IVR_PLIB06,  2, lLogText );
		T(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
		return true;

	}
	else
	{
		sprintf(lLogText,"CID:%06d SC:%s HKY:%s Delete Pull keyword Details Into HashTable Failed,Error:%d" ,
				mCircuitId,
				fileDetails.fetchedName,
				hashKey,
				lReturn);
		gLog.GenerateLog(IVR_PLIB07, lLogText );
		TERR(gTrace ,printf("%s: %s\n",gProcessName , lLogText););
		return false;
	}

	return true;
}

//---------------------------------------------------------------
// METHOD      : ProcessPlayRequest 
// DESCRIPTION : Send Play request to underlying media
// PARAMETER   : no arguments  
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::ProcessPlayRequest( )
{
   if(mIsStopRequested)
      return false;
   MpRequestDetails      lMpRequestDetails;
   MpResponseDetails     lMpResponseDetails;
   TEXT                  lLogText[MAX_LOG_TEXT_LEN + 1]="";

   memset( &lMpRequestDetails, 0, sizeof(MpRequestDetails) );
   memset( &lMpResponseDetails, 0, sizeof(MpResponseDetails) );

   lMpRequestDetails.cmdType                            = MC_CNTTN; 
   lMpRequestDetails.mpReqData.playMsg.seqNum           = gReferenceNum;
   lMpRequestDetails.mpReqData.playMsg.circuitId        = mCircuitId;

   if( strcmp( mBargeinFlag, "true")==0)
   {
      lMpRequestDetails.mpReqData.playMsg.dtmfEnable    = true;  //interupt play on dtmf Detection
   }
   else
      lMpRequestDetails.mpReqData.playMsg.dtmfEnable    = false; //Continue Play on dtmf detection

   lMpRequestDetails.mpReqData.playMsg.repeatPlay       = false;
   lMpRequestDetails.mpReqData.playMsg.vFileCount       = mplayDetails.fileCount; 
   strcpy(lMpRequestDetails.mpReqData.playMsg.dtmfStopPlayCharSet, mTermchar);
   memcpy(lMpRequestDetails.mpReqData.playMsg.voiceFile, mplayDetails.fileName, 	sizeof(mplayDetails.fileName) );

   snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Nof:%d", gReferenceNum,
         mCircuitId, 
         mplayDetails.fileCount);

   if( 0 == mplayDetails.fileCount )
   {
      T(gTrace, printf("%s: %s No files for playing returning\n", gProcessName, lLogText););
      return true;
   }	

   //if(true == TSSCollectDtmf( 0, 1, 10, "", dtmf) )
   if(mIsAudioOpSupported)
   {
      lMpRequestDetails.mpReqData.playMsg.playCtrlEnable       = true;  //interupt play on dtmf Detection
      lMpRequestDetails.mpReqData.playMsg.playCtrl.pauseDtmf   = mPauseDtmf[0];
      lMpRequestDetails.mpReqData.playMsg.playCtrl.resumeDtmf  = mResumeDtmf[0];
      lMpRequestDetails.mpReqData.playMsg.playCtrl.forwardDtmf = mForwardDtmf[0];
      lMpRequestDetails.mpReqData.playMsg.playCtrl.rewindDtmf  = mRewindDtmf[0];     
   }

   /*
      if( false == mMediaLibrary.PlayNonBlockingMode( lMpRequestDetails ) )
      {
      snprintf( lLogText, MAX_LOG_TEXT_LEN, "CIC:%ld Sending Play request Played", mCircuitId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB08, lLogText);	
      return false;
      }*/

   mIsPlayInProgress = true;
   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_REQUESTED);
   gChannelApi.SendChannelStatus(mCircuitId, IVR_CHANNEL_PLAYING, gReferenceNum);

   gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
   gIvrEvrCause   =  CAS_IVR_REQ_RECEIVED;
   gIvrEvrReason  =  RSN_IVR_NONE;
   this->GenerateEvr();
   if(mIsBillingInProgress)
   {
      if(true != this->ProcessCallCharge(BILLING_CONTENT_START))
      {
         return false;
      }
   }

   if( false == mMediaLibrary.PlayBlockingMode( lMpRequestDetails , lMpResponseDetails) )
   {
      mIsPlayInProgress = false;
      T(gTrace, printf("%s: %s Play Request failed\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, lLogText);	
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_FAILED);
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_FAILED;
      gIvrEvrReason  =  RSN_IVR_MEDIA_REQ_RESOURCE_UNAVAILABLE; //// need to verify
      this->GenerateEvr();
      if(mIsBillingInProgress)
      {
         this->ProcessCallCharge(BILLING_CONTENT_STOP);
      }
      return false;
   } 
   mIsPlayInProgress = false;
   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_OVER);

   if(MP_RSP_SUCCESS == lMpResponseDetails.rspMsg )
   {
      memset ( &mplayDetails, 0, sizeof(PlayDetails)); //Played Queued Files memset struct for new request
      T(gTrace, printf("%s: %s Play Req sent and successful response recvd\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB03, LOG_LEVEL_1, lLogText);	
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_COMPLETE;
      gIvrEvrReason  =  RSN_IVR_NONE;
      this->GenerateEvr();
      if(mIsBillingInProgress)
      {
         if(true != this->ProcessCallCharge(BILLING_CONTENT_STOP))
         {
            return false;
         }
      }
      return true;
   }
   else
   {	
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_FAILED);
      snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Nof:%d RspStatus:%d", gReferenceNum,
            mCircuitId, 
            mplayDetails.fileCount, 
            lMpResponseDetails.rspMsg);

      memset( &mplayDetails, 0, sizeof(PlayDetails)); //Played Queued Files memset struct for new request
      T(gTrace, printf("%s: %s Play Req sent but NACK respose recvd\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, lLogText);	
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_FAILED;
      gIvrEvrReason  =  RSN_IVR_MEDIA_REQ_INVALID_FILE;  // need to verify
      this->GenerateEvr();
      if(mIsBillingInProgress)
      {
         this->ProcessCallCharge(BILLING_CONTENT_STOP);
      }
      return false;
   }	
   return true;
}
//---------------------------------------------------------------
// METHOD      : ProcessEndRequest 
// DESCRIPTION : Send Stop request to underlying media library
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------

BOOLEAN  PromptApi::ProcessEndRequest( )
{
   //No need to send Stop for Every EndSession
   //to be sent only when call terminated and play in progress
   TEXT lLogText[MAX_LOG_TEXT_LEN +1 ] ="";

   snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d", gReferenceNum, mCircuitId);

   mIsStopRequested = true;

   memset ( &mplayDetails, 0, sizeof(PlayDetails));

   if(mIsPlayInProgress)
   {
      MpRequestDetails  lMpRequestDetails;
      MpResponseDetails lMpResponseDetails;
      memset( &lMpResponseDetails, 0, sizeof(MpResponseDetails));
      memset(&lMpRequestDetails, 0, sizeof(MpRequestDetails) );

      T(gTrace, printf("%s: %s End Session CallBack \n", gProcessName, lLogText););

      lMpRequestDetails.cmdType    = MC_DISTN; 
      lMpRequestDetails.mpReqData.stopMsg.seqNum = gReferenceNum;
      lMpRequestDetails.mpReqData.stopMsg.circuitId = gPromptApi.mCircuitId;

      // Give Stop Request to Voice Manager
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_STOP_IN_PROGRESS);

      if( false == mMediaLibrary.StopProcessing( lMpRequestDetails, lMpResponseDetails))
      {
         T(gTrace, printf("%s: %s Sending Stop Request Failed\n", gProcessName, lLogText););
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
         gLog.GenerateLog(IVR_PLIB13, glLogText);
         return false;
      }
      else
      {	
         if(  MP_RSP_SUCCESS == lMpResponseDetails.rspMsg)
         {
            T(gTrace, printf("%s: %s Sent Stop Request and recv ACK response\n", gProcessName, lLogText););
            return true;
         }	
      }

      return false;
   }
   else if(mIsDtmfInProgress)
   {
      MpRequestDetails  lMpRequestDetails;
      MpResponseDetails lMpResponseDetails;
      memset( &lMpResponseDetails, 0, sizeof(MpResponseDetails));
      memset(&lMpRequestDetails, 0, sizeof(MpRequestDetails) );

      //T(gTrace, printf("%s: End Session CallBack \n", gProcessName););

      lMpRequestDetails.cmdType    = MC_GETDIGSTP; 
      lMpRequestDetails.mpReqData.stopMsg.seqNum = gReferenceNum;
      lMpRequestDetails.mpReqData.stopMsg.circuitId = gPromptApi.mCircuitId;

      // Give Stop Request to Voice Manager
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_STOP_IN_PROGRESS);

      if( false == mMediaLibrary.StopProcessing( lMpRequestDetails, lMpResponseDetails))
      {
         T(gTrace, printf("%s: %s Sending DTMF Stop Request Failed\n", gProcessName, lLogText););
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
         gLog.GenerateLog(IVR_PLIB14, glLogText);
         return false;
      }
      else
      {	
         if(  MP_RSP_SUCCESS == lMpResponseDetails.rspMsg)
         {
            T(gTrace, printf("%s: %s Sent DTMF Stop Request and recv ACK response\n", gProcessName, lLogText););
            return true;
         }	
      }

      return false;
   }
   else if(mIsRecInProgress)
   {
      MpRequestDetails  lMpRequestDetails;
      MpResponseDetails lMpResponseDetails;
      memset( &lMpResponseDetails, 0, sizeof(MpResponseDetails));
      memset(&lMpRequestDetails, 0, sizeof(MpRequestDetails) );

      //T(gTrace, printf("%s: End Session CallBack \n", gProcessName););

      lMpRequestDetails.cmdType    = MC_RECSTP; 
      lMpRequestDetails.mpReqData.stopMsg.seqNum = gReferenceNum;
      lMpRequestDetails.mpReqData.stopMsg.circuitId = gPromptApi.mCircuitId;

      // Give Stop Request to Voice Manager
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_STOP_IN_PROGRESS);

      if( false == mMediaLibrary.StopProcessing( lMpRequestDetails, lMpResponseDetails))
      {
         T(gTrace, printf("%s: %s Sending Record Stop Request Failed\n", gProcessName, lLogText););
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
         gLog.GenerateLog(IVR_PLIB15, glLogText);
         return false;
      }
      else
      {	
         if(  MP_RSP_SUCCESS == lMpResponseDetails.rspMsg)
         {
            T(gTrace, printf("%s: %s Sent Record Stop Request and recv ACK response\n", gProcessName, lLogText););
            return true;
         }	
      }

      return false;
   }
   else
   {
      T(gTrace, printf("%s: %s Play/Record Not in Progress\n", gProcessName, lLogText););
      return true;
   }
}
//---------------------------------------------------------------
// METHOD      : ProcessPrefetch
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------

BOOLEAN  PromptApi::ProcessPrefetch( VXIpromptInterface * vxip, const VXIchar *type,
      const VXIchar *src, const VXIchar *text,  const VXIMap* properties)
{
   if(mIsStopRequested)
      return false;
   return VXIprompt_RESULT_SUCCESS;
}
//---------------------------------------------------------------
// METHOD      : ProcessQueue 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------

BOOLEAN PromptApi::ProcessQueue(VXIpromptInterface* vxip, const VXIchar *raw_type, const VXIchar *raw_src, const VXIchar *raw_text, const VXIMap  *properties )
{
   TEXT                  lLogText[MAX_LOG_TEXT_LEN + 1]="";
   TEXT 						 mystr[5000]= "";

   if(mIsStopRequested)
      return false;
   //mIsStopRequested = false;

   wcstombs ( mystr , raw_text , 5000) ;

   vxistring bargein;
   const VXIValue *val = VXIMapGetProperty(properties, L"bargein");
   if (val != NULL)
      bargein = VXIStringCStr(reinterpret_cast<const VXIString*>(val));

   wcstombs (mBargeinFlag , bargein.c_str() , 10) ;

   vxistring bargeintype;
   val = VXIMapGetProperty(properties, L"bargeintype");
   if (val != NULL)
      bargeintype = VXIStringCStr(reinterpret_cast<const VXIString*>(val));

   wcstombs (mBargeinType , bargeintype.c_str() , 20) ;

   vxistring lTermChar;
   val = VXIMapGetProperty(properties, L"termchar");
   if (val != NULL)
      lTermChar = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
   

   wcstombs (mTermchar , lTermChar.c_str() , 10) ;

   vxistring lTermTimeOut;
   val = VXIMapGetProperty(properties, L"termtimeout");
   if (val != NULL)
      lTermTimeOut = VXIStringCStr(reinterpret_cast<const VXIString*>(val));

   wcstombs (mTermTimeOut , lTermTimeOut.c_str() , 10) ;

   mIsAudioOpSupported = false;
   //---------------------------------------------------------------------------
   //Get the Audio Operation Supported Properties
   vxistring lVsPauseDtmf;
   val = VXIMapGetProperty(properties, L"pausedtmf");
   if (val != NULL && VXIValueGetType(val) == VALUE_STRING)
   {
      lVsPauseDtmf = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
      wcstombs(mPauseDtmf, lVsPauseDtmf.c_str(), 10);
      mIsAudioOpSupported = true;
   }

   vxistring lVsResumeDtmf;
   val = VXIMapGetProperty(properties, L"resumedtmf");
   if (val != NULL && VXIValueGetType(val) == VALUE_STRING)
   {
      lVsResumeDtmf = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
      wcstombs(mResumeDtmf, lVsResumeDtmf.c_str(), 10);
      mIsAudioOpSupported = true;
   }

   vxistring lVsRewindDtmf;
   val = VXIMapGetProperty(properties, L"rewinddtmf");
   if (val != NULL && VXIValueGetType(val) == VALUE_STRING)
   {
      lVsRewindDtmf = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
      wcstombs(mRewindDtmf, lVsRewindDtmf.c_str(), 10);
      mIsAudioOpSupported = true;
   }

   vxistring lVsForwardDtmf;
   val = VXIMapGetProperty(properties, L"forwarddtmf");
   if (val != NULL && VXIValueGetType(val) == VALUE_STRING)
   {
      lVsForwardDtmf = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
      wcstombs(mForwardDtmf, lVsForwardDtmf.c_str(), 10);
      mIsAudioOpSupported = true;
   }

   //---------------------------------------------------------------------------




   vxistring lChargeContent;
   TEXT      lChargeContentId[20];
   val = VXIMapGetProperty(properties, L"chargecontent");
   if (val != NULL)
   {
      lChargeContent = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
      wcstombs (lChargeContentId, lChargeContent.c_str(), 20) ;
      mIsBillingInProgress = true;
      mChargeContentId = atol((const char*)lChargeContentId);
      T(gTrace, printf("%s: ChargeContnetId : %s (%d)\n", gProcessName, lChargeContentId, mChargeContentId););
      if(true != this->ProcessCallCharge(BILLING_CONTENT_PLAY))
      {
         return false;
      }
   }
   else
   {
      mIsBillingInProgress = false;
      mChargeContentId = 0;
   }



   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d BargeinFlg:%s BargeinType:%s", 
         gReferenceNum,
         mCircuitId, 
         mBargeinFlag, 
         mBargeinType);

   T(gTrace, printf("%s: %s Queue request\n", gProcessName, lLogText););

   vxistring testProp;
   const VXIValue *testValProp = VXIMapGetProperty(vxip->properties,  L"KEY_PARAM1");
   if (testValProp != NULL)
      testProp = VXIStringCStr(reinterpret_cast<const VXIString*>(testValProp));

   //T(gTrace, printf("FileName in properties: %ls\n" , testProp.c_str() ););

   TEXT  lTempchar[100]="";

   wcstombs(lTempchar, testProp.c_str(), 100 );

   if( lTempchar[0] != '\0')
   {
      strcpy(mplayDetails.fileName[0], lTempchar);
      mplayDetails.fileCount++;
      T(gTrace, printf("%s: %s File to Queue:%d-%s\n", gProcessName, lLogText, mplayDetails.fileCount, mplayDetails.fileName[0]););
   }
   else
   {
      //T(gTrace, printf("%s: %s FileName in properties is NULL hence fetching from data\n", gProcessName, lLogText););

      // check for audio tag
      if( false == gPromptApi.ParseDataForFile( mystr))
      {
         //T(gTrace, printf("%s: %s FileName not found Fetching from mark name\n", gProcessName, lLogText););
         //gLog.GenerateLog(IVR_PLIB02, LOG_OPTIONAL, lLogText);	

         //check for mark name
         VXIValue *val = (VXIValue*) VXIMapGetProperty(properties, PROMPT_AUDIO_REFS);
         if (val != NULL)
         {
            const VXIchar *key;
            const VXIValue *keyvalue;
            VXIMap* valueMap;
            valueMap = reinterpret_cast<VXIMap*>( val);
            VXIMapGetFirstProperty( valueMap, &key, &keyvalue);
            if ( (key!=NULL) && (keyvalue!=NULL))
            {
               const VXIchar *type;
               const VXIbyte *data;
               VXIulong size;
               VXIContentValue(( reinterpret_cast<const VXIContent*>(keyvalue)), &type, &data, &size);
               //printf("In Prompt Lib appendContent :%s size:%ld \n", data, size);
               char mychar[200];
               strcpy( mychar,(char*)data);
               strcpy(mplayDetails.fileName[0], mychar);
               mplayDetails.fileCount++;
               //printf(" Print my char %s \n", mychar);
               return true;
            }
         }
         else
         {
            //T(gTrace, printf("%s: %s Mark Name Not Found\n", gProcessName, lLogText););
            //gLog.GenerateLog(IVR_PLIB02, LOG_OPTIONAL, lLogText);	
            return false;

         }

      }
      else
      {
         for( int n=0; n<mplayDetails.fileCount; n++ )
         {
            TEXT *lStrPtr;
            TEXT	lFileName[100];
            T(gTrace, printf("%s: FileSeq: %d/%d %s Checking for Properties:%s\n", 
                     gProcessName, n, mplayDetails.fileCount, lLogText,
                     mplayDetails.fileName[n]););

            lStrPtr= strstr( mplayDetails.fileName[n], "@" );

            if( NULL != lStrPtr)
            {	
               VXIchar* lPropName = NULL;
               size_t   lsize;
               strcpy(lFileName, lStrPtr+1 );


               //lPropName   = new VXIchar[ strlen ( mplayDetails.fileName[n] ) + 1  ];
               lPropName   = new VXIchar[ 101];
               //lsize = mbstowcs(lPropName, mplayDetails.fileName[n], strlen(mplayDetails.fileName[n]));
               lsize = mbstowcs(lPropName, lFileName, 100);

               //printf(" PropertyKeyName=%ls \n", lPropName );

               vxistring lVarFileName;
               TEXT  lTempFilName[100]="";
               //const VXIValue *val = VXIMapGetProperty(properties, lPropName);
               const VXIValue *val = VXIMapGetProperty(vxip->properties, lPropName);
               //VXIPrintValue( val );
               if (val != NULL)
               {
                  //printf("1------------------ :%ls\n",  lVarFileName.c_str());
                  lVarFileName = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
               }
               else
                  //printf("2------------------:%ls\n", lVarFileName.c_str());

                  //wcstombs (lTempFilName, lVarFileName.c_str(), lsize) ;
                  wcstombs (lTempFilName, lVarFileName.c_str(), 100) ;


               // Ram To Be Continued
               //TEXT	lTemp[100] = "";
               //sprintf(lTemp, "/opt/ivr/bin/%s", lTempFilName);
               //strcpy(lTempFilName, lTemp);

               T(gTrace, printf("%s: %s Audio FileName=%s PropertyName:%s\n", gProcessName, lLogText,
                        lFileName,
                        lTempFilName););
               strcpy( mplayDetails.fileName[n] ,lTempFilName );
            }
         }

      }	  
   }
   gLog.GenerateLog(IVR_PLIB01, LOG_LEVEL_2, lLogText);	
   return true;

}
//---------------------------------------------------------------
// METHOD      : ParseDataForFile 
// DESCRIPTION : parse Data to get the filename from rawdata
// PARAMETER   : TEXT  
// RETURN      : bool 
//---------------------------------------------------------------
BOOLEAN PromptApi::ParseDataForFile( TEXT * rawData )
{

   if(mIsStopRequested)
      return false;
   TEXT	 *lStrptr  = NULL;
   TEXT	 *lStrptr1 = NULL;
   TEXT	 *lStrptr3 = NULL;
   TEXT 	 lTempStrArray[50][400];
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] ="";

   memset( lTempStrArray, 0, sizeof(lTempStrArray) );

   //memset ( &mplayDetails, 0, sizeof(PlayDetails));

   int q=0;

   snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d", gReferenceNum, mCircuitId);

   //T(gTrace, printf("%s: FileNames Raw Data: %s\n", gProcessName, rawData););
   lStrptr = strstr( rawData , "<audio src");

   if( lStrptr == NULL )
   {
      T(gTrace, printf("%s: %s No audio tag found hence not queuing the file \n", gProcessName, lLogText););
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
      gLog.GenerateLog(IVR_PLIB16, glLogText);
      return false;
   }
   else
   {	
      while( lStrptr != NULL )
      {
         lStrptr1 = strstr( lStrptr, "\"");

         if(lStrptr1 == NULL)
         {
            return false;
         }
         else
         {
            int i = 0;
            lStrptr3 = lStrptr1+1 ;

            while( lStrptr3[i] != '\"' )
            {
               lTempStrArray[q][i] = lStrptr3[i];
               i++;
            }
            lTempStrArray[q][i] = '\0';
            lStrptr = strstr(lStrptr1 , "<audio src");
            q++;
         }
      }
   }
   //T(gTrace, printf("%s: Parsing data For File Name Total Prompts: %d \n", gProcessName, q););
   //T(gTrace, printf("%s: -------------------------------------------------------- \n", gProcessName););
   for( int n=0; n<q; n++ )
   {
      //strcpy(mplayDetails.fileName[n], lTempStrArray[n]);
      strcpy(mplayDetails.fileName[mplayDetails.fileCount], lTempStrArray[n]);
      mplayDetails.fileCount++ ;
      //T(gTrace, printf("String :%s Index:%d\n", lTempStrArray[n], n););
      T(gTrace, printf("%s: %s Audio FileName=%d-%s FileIndex=%d \n", gProcessName, lLogText, mplayDetails.fileCount, mplayDetails.fileName[n] , n););
   }
      T(gTrace, printf("%s: Total FileNames to be played: %d\n", gProcessName, mplayDetails.fileCount););
   //T(gTrace, printf("%s: -------------------------------------------------------- \n", gProcessName););

   return true;

}
//---------------------------------------------------------------
// ETHOD       :ParseDataForText 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------

BOOLEAN PromptApi::ParseDataForText( TEXT *searchStr, TEXT *rawData,TEXT *textToPlay  ) 
{
   if(mIsStopRequested)
      return false;
   TEXT *strptr;
   TEXT *strptr1;
   TEXT *strptr2;
   TEXT substr1[100];

   strptr = strstr(rawData , searchStr);

   if( strptr == NULL )
   {
      //T(gTrace, printf("TBR %s TAG not found\n", searchStr););
      return false;
   }
   else
   {
      //T(gTrace, printf("TBR %s TAG found\n", searchStr););
      strptr1= strstr( strptr , ">");
      if( strptr1 == NULL )
      {
         //T(gTrace, printf("TBR > not found\n"););
         return false;
      }

      //T(gTrace, printf("TBR > found\n"););
      strptr2 = strstr( strptr1 ,"<");
      if( strptr2 == NULL )
      {
         //cout<<" not found "<<endl;
         //T(gTrace, printf("TBR < not found\n"););
         return false;
      }
      //T(gTrace, printf("TBR < found\n"););

      puts(strptr2);
      int i =0;

      while( strptr1[i] != '<' || strptr1[i] != '\n')
      {
         substr1[i] = strptr1[i];
         i++;
      }
      substr1[i] = '\0';
      strcpy(  textToPlay, substr1  );
      //T(gTrace, printf("TBR Text to Play :%s \n", textToPlay););
   }

   return true;

}	
//---------------------------------------------------------------
// METHOD      : SetTimerAndWaitForReponse() 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
EnumResType PromptApi::SetTimerAndWaitForReponse()
{
   time_t            lExpiryTime;

   lExpiryTime = time(NULL) + mCfgSecs;

   while(lExpiryTime <= time(NULL))
   {
      //if( true == ReadResponseFromVM(lTsResponseDetails))
      {
         return RES_TRUE;
      }
      //else
      return RES_FALSE;	
   }
   //Send Nack to VM

   return RES_TIME_OUT;
}
//---------------------------------------------------------------
// METHOD      : CollectDtmf 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
BOOLEAN PromptApi::CollectDtmf( int cic, int noOfDigit, int interDigitTimeout, TEXT *termChars , TEXT *dtmf, 
      TEXT *pauseDtmf, TEXT *resumeDtmf, TEXT *fwdDtmf, TEXT *rwndDtmf)
{
   TEXT               lLogText[MAX_LOG_TEXT_LEN + 1]="";
   MpRequestDetails   lMpRequestDetails;
   MpResponseDetails  lMpResponseDetails;
   BOOLEAN            lRetVal=false;

   //ChangeNote: Komal - Commented so that DTMF gets collected after Play, even in case of Hangup
   //if(mIsStopRequested)
   //   return false;

   if(mIsStopRequested)
      interDigitTimeout = 1;

   // TBC - CIC is not required, as object already have channel number stored in mCircuitId -- Komal
   cic = mCircuitId;
   memset( &lMpRequestDetails,  0, sizeof(MpRequestDetails) );
   memset( &lMpResponseDetails, 0, sizeof(MpResponseDetails) );

   lMpRequestDetails.cmdType                                    = MC_GETDIG; 
   lMpRequestDetails.mpReqData.getDigit.circuitId               = cic;
   lMpRequestDetails.mpReqData.getDigit.interDigitTimeOut       = interDigitTimeout;
   strcpy(lMpRequestDetails.mpReqData.getDigit.teminateChars,    termChars);            
   lMpRequestDetails.mpReqData.getDigit.seqNum                  = gReferenceNum;
   lMpRequestDetails.mpReqData.getDigit.numOfDig                = noOfDigit;
   if(noOfDigit == 1)
	   lMpRequestDetails.mpReqData.getDigit.includeTerminateChar    = true;
   else
	   lMpRequestDetails.mpReqData.getDigit.includeTerminateChar    = false;

   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_DTMF_REQUESTED);
   gChannelApi.SendChannelStatus(mCircuitId, IVR_CHANNEL_DTMF, gReferenceNum);
   snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d NOD:%d TermChars:%s", 
         gReferenceNum, mCircuitId, noOfDigit, termChars);

   T(gTrace, printf("%s: %s GetDTMF Request\n", gProcessName, lLogText););
   gLog.GenerateLog(IVR_PLIB09, LOG_LEVEL_2, lLogText);
   mIsDtmfInProgress = true;

   if(NULLPTR == pauseDtmf)
      lRetVal = mMediaLibrary.GetDtmfDigits( lMpRequestDetails, lMpResponseDetails );
   else
      lRetVal = mMediaLibrary.GetDtmfDigits( lMpRequestDetails, lMpResponseDetails , pauseDtmf, resumeDtmf, fwdDtmf, rwndDtmf);

   if( false == lRetVal)
   {
      T(gTrace, printf("%s: %s Failed to Get the Digits \n", gProcessName, lLogText););
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
      gLog.GenerateLog(IVR_PLIB06, LOG_LEVEL_1, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_DTMF_FAILED);
      mIsDtmfInProgress = false;
      return false;
   }
   else
   {
      mIsDtmfInProgress = false;
      if( MP_RSP_SUCCESS == lMpResponseDetails.rspMsg )
      {
         strcpy( dtmf, lMpResponseDetails.mpRspData.getDigit.rspDigits) ;
         strcpy( mDtmf, lMpResponseDetails.mpRspData.getDigit.rspDigits) ;

         //snprintf( lLogText, MAX_LOG_TEXT_LEN , "CIC:%ld Success respose recvd for CollectDtmf, DTMF collected:%s\n", 
         //		mCircuitId , 
         //		dtmf);

         snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d DTMF:%s", gReferenceNum, mCircuitId, dtmf);
         gLog.GenerateLog(IVR_PLIB05, LOG_LEVEL_1, lLogText);
         if('\0' == lMpResponseDetails.mpRspData.getDigit.rspDigits[0])
         {
            T(gTrace, printf("%s: %s Success respose recvd, But No Dtmf Collected\n", gProcessName, lLogText););
            gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_DTMF_FAILED);
            snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
            gLog.GenerateLog(IVR_PLIB17, glLogText);
            return false;
         }
         else
         {
            T(gTrace, printf("%s: %s Success respose recvd for CollectDtmf\n", gProcessName, lLogText););
            gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_DTMF_COLLECTED);
         }
         return true;
      }	
      T(gTrace, printf("%s: %s Failed to Get the Digits \n", gProcessName, lLogText););
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
      gLog.GenerateLog(IVR_PLIB06, LOG_LEVEL_1, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_DTMF_FAILED);
      return false;
   }	
}
//---------------------------------------------------------------
// METHOD      : TSSVXIRecord 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
BOOLEAN PromptApi::TSSVXIRecord(const VXIMap *properties,  TEXT* recordFileName, int& recordingTime , TEXT *termChar)
{
   TEXT               lLogText[MAX_LOG_TEXT_LEN + 1]  = "";
   MpRequestDetails   lMpRequestDetails;
   MpResponseDetails  lMpResponseDetails;

   if(mIsStopRequested)
      return false;

   memset( &lMpRequestDetails, 0, sizeof( MpRequestDetails ) );
   memset( &lMpResponseDetails, 0, sizeof( MpResponseDetails) );

   //printf( "4---------------- RECORD----------\n" ) ;
   // Max Recording time  


   VXIlong lMaxTime = 20000;  //Defaulting to 20 Second
   VXIlong lBeep = 0;  //Defaulting to false
   VXIlong lDtmf = 1;  //Defaulting to true
   //VXIlong lTerminateDtmf;
   //VXIlong lTimeOutComplete;
   //VXIlong  lDtmfTimeOut;
   vxistring lTemp; 

   const VXIValue* val ;
   val = VXIMapGetProperty(properties, REC_MAX_RECORDING_TIME);
   if (val != NULL && VXIValueGetType(val) == VALUE_INTEGER )
   {
      lMaxTime = VXIIntegerValue(reinterpret_cast<const VXIInteger *>(val));
      printf( "1---------------- RECORD Val Integer  ---- %ld \n", lMaxTime);
   }
   else if (val == NULL)
   {
      printf( "1---------------- Property Not Defined  ---- %ld \n", lMaxTime);
   }
   else
   {
      printf( "1---------------- Property Value Type ---- %ld \n", VXIValueGetType(val));
      VXIPrintValue( val );
      lTemp = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
      TEXT      lTempStr[101]="";
      wcstombs (lTempStr , lTemp.c_str() , 10) ;
      printf( "1---------------- RECORD Val  ---- %s \n", lTempStr);
   }

   printf( "1---------------- RECORD---- REC_MAX_RECORDING_TIME ------:%ld\n", lMaxTime ) ;
   //lTemp = VXIStringCStr(reinterpret_cast<const VXIString*>(val));

   //whether to append Beep at the end of recording

   //val = VXIMapGetProperty(properties, REC_BEEP);
   //if (val != NULL && VXIValueGetType(val) == VALUE_INTEGER )
   //   lBeep = VXIIntegerValue(reinterpret_cast<const VXIInteger *>(val));

   vxistring lTempBeep;
   TEXT      lBeepFlag[20]="";
   val = VXIMapGetProperty(properties, REC_BEEP);
   if (val != NULL && VXIValueGetType(val) == VALUE_STRING )
      lTempBeep = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
   else
      lTempBeep = L"false";

   wcstombs (lBeepFlag , lTempBeep.c_str() , 10) ;

   if( strcmp( lBeepFlag, "true")==0)
      lBeep = 1;

   //If initial beep has to be played
   if(lBeep == 1)
   {
      TEXT lRecordBeepFile[100]="";
      snprintf(lRecordBeepFile, 100, "%s/global/audio/beep.wav", mIvrBasePath);
      if(false == ProcessFilePlayRequest(lRecordBeepFile, false))
      {
         TERR(gTrace, printf("%s: %s Playing Record Beep - Failed\n", gProcessName, lLogText););
      }
      else
      {
         T(gTrace, printf("%s: %s Playing Record Beep - Success\n", gProcessName, lLogText););
      }
   }

   //printf( "2---------------- RECORD----- REC_BEEP -----:%ld\n", lBeep ) ;

   //whether to terminate recording on DTMF Detection
   val = VXIMapGetProperty(properties, REC_TERMINATED_ON_DTMF);
   if (val != NULL && VXIValueGetType(val) == VALUE_INTEGER )
      lDtmf = VXIIntegerValue(reinterpret_cast<const VXIInteger *>(val));
   //printf( "3---------------- RECORD-----REC_TERMINATED_ON_DTMF-----:%ld\n", lDtmf ) ;

   if(lDtmf)
      sprintf(lMpRequestDetails.mpReqData.recordMsg.dtmfStopRecCharSet, "0123456789*#");
   else
      sprintf(lMpRequestDetails.mpReqData.recordMsg.dtmfStopRecCharSet, "");
   //printf( "4---------------- RECORD------REC_DTMF_TERMINATOR_CHAR----:%s\n", 
   //      lMpRequestDetails.mpReqData.recordMsg.dtmfStopRecCharSet ) ;
#if 0   
//which char would terminate the recording
   vxistring lTermChar;
   val = VXIMapGetProperty(properties, REC_DTMF_TERMINATOR_CHAR);
   if (val != NULL && VXIValueGetType(val) == VALUE_STRING )
      lTermChar = VXIStringCStr(reinterpret_cast<const VXIString*>(val));
   else
      lTermChar = L"#";

   wcstombs(lMpRequestDetails.mpReqData.recordMsg.dtmfStopRecCharSet, lTermChar.c_str(), 10);
   printf( "6---------------- RECORD------REC_DTMF_TERMINATOR_CHAR----:%s\n", 
         lMpRequestDetails.mpReqData.recordMsg.dtmfStopRecCharSet ) ;

   //Final Silence
   val = VXIMapGetProperty(properties, REC_TIMEOUT_COMPLETE);
   if (val != NULL && VXIValueGetType(val) == VALUE_INTEGER )
      lTimeOutComplete = VXIIntegerValue(reinterpret_cast<const VXIInteger *>(val));
   printf( "6---------------- RECORD-----REC_TIMEOUT_COMPLETE-----:%ld\n", lTimeOutComplete ) ;

   //how long to wait for terminating char 
   /*
      const VXIValue* val = VXIMapGetProperty(properties, REC_DTMF_TIMEOUT_TERMINATOR);
      if (val != NULL && VXIValueGetType(val) == VALUE_INTEGER )
      lDtmfTimeOut = VXIIntegerValue(reinterpret_cast<const VXIInteger *>(val));
      printf( "6---------------- RECORD----------:%ld\n", lTimeOutComplete ) ;
    */
   //total time to wait for user input
   val = VXIMapGetProperty(properties, REC_DTMF_TIMEOUT);
   if (val != NULL && VXIValueGetType(val) == VALUE_INTEGER )
      lDtmfTimeOut = VXIIntegerValue(reinterpret_cast<const VXIInteger *>(val));
   printf( "6---------------- RECORD-----REC_DTMF_TIMEOUT-----:%ld\n", lDtmfTimeOut ) ;

#endif

   //fill the request structure
   lMpRequestDetails.cmdType                            = MC_RECSTR;
   lMpRequestDetails.mpReqData.recordMsg.seqNum         = gReferenceNum;
   lMpRequestDetails.mpReqData.recordMsg.circuitId      = mCircuitId;
   lMpRequestDetails.mpReqData.recordMsg.recTime        = lMaxTime;

   if(strlen(recordFileName)<= 0)
   {
      struct tm      *time_tm;
      struct timeval  timePtr;

      gettimeofday(&timePtr, 0);
      time_t  t = timePtr.tv_sec;

      time_tm = localtime(&t);
      sprintf(recordFileName, "%s/recordings/Record-%04d-%02d%02d%06ld.wav",
            mIvrBasePath,
            mCircuitId,
            time_tm->tm_min,
            time_tm->tm_sec,
            timePtr.tv_usec);     // Date & Time
   }

   strcpy( lMpRequestDetails.mpReqData.recordMsg.voiceFile, recordFileName);
   snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d", gReferenceNum, mCircuitId);

   //printf( "7---------------- RECORD----------\n" ) ;
   gChannelApi.SendChannelStatus(mCircuitId, IVR_CHANNEL_RECORDING, gReferenceNum);
   mIsRecInProgress = true;
   if( false == mMediaLibrary.RecordVoice( lMpRequestDetails , lMpResponseDetails) )
   {
      //snprintf( lLogText, MAX_LOG_TEXT_LEN, "CIC:%ld Record Request failed", mCircuitId);
      T(gTrace, printf("%s: %s Record Request failed\n", gProcessName, lLogText););
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, glLogText);
      mIsRecInProgress = false;
      return false;
   }
   mIsRecInProgress = false;

   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_RECORD_REQUESTED);

   // TBC : Komal 
   // Adding additional ReadMediaResponse to read the Record Response, if pending in Queue
   // Start
   if(mIsStopRequested && lMpResponseDetails.cmdType != MC_RECSTR)
   {
      //RxResponse(const UINT16 lMsgType, MpResponseDetails &lResp,modeOption &lMode)
      usleep(5000);
      lMpResponseDetails.mpRspData.dummy.seqNum = lMpRequestDetails.mpReqData.dummy.seqNum;
      lMpResponseDetails.cmdType = lMpRequestDetails.cmdType;
      mMediaLibrary.RxResponseNonBlocking(lMpRequestDetails.mpReqData.playMsg.circuitId, lMpResponseDetails, 10);
   }
   //End

   if( MP_RSP_SUCCESS == lMpResponseDetails.rspMsg )
   {
      //snprintf( lLogText, MAX_LOG_TEXT_LEN, "CIC:%ld Record Req sent and successful response recvd", mCircuitId );
      recordingTime = lMpResponseDetails.mpRspData.recordMsg.recTime;
      T(gTrace, printf("%s: %s Record Req sent and successful response recvd RecTime:%d\n", gProcessName, lLogText, recordingTime););
      gLog.GenerateLog(IVR_PLIB03, LOG_LEVEL_1, lLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_RECORD_COMPLETED);
      return true;
   }
   else
   {
      snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d RspStatus:%d RecTime:%d ", gReferenceNum, mCircuitId, lMpResponseDetails.rspMsg, lMpResponseDetails.mpRspData.recordMsg.recTime);

      //snprintf( lLogText, MAX_LOG_TEXT_LEN, "CIC:%ld Record Req sent but NACK respose recvd RespStatus:%d",
      //		mCircuitId,
      //		lMpResponseDetails.rspMsg);
      T(gTrace, printf("%s: %s Record Req sent but NACK respose recvd\n", gProcessName, lLogText););
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, glLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_RECORD_FAILED);
      return false;
   }

}	
//---------------------------------------------------------------
// METHOD      : GenerateEvr 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
BOOLEAN PromptApi::GenerateEvr()
{
   memset(&gIvrEvrMsg, 0, sizeof(IvrEvrMsg));

   gIvrEvrMsg.evrEvent    =  gIvrEvrEvent;
   gIvrEvrMsg.evrCause    =  gIvrEvrCause;
   gIvrEvrMsg.evrReason   =  gIvrEvrReason;
   //strcpy(gIvrEvrMsg.shortCode, gShortCode);
   strcpy(gIvrEvrMsg.cldNum, gCalledNumber);
   strcpy(gIvrEvrMsg.clgNum, gCallingNumber);
   gIvrEvrMsg.refId   =  mCircuitId;
   sprintf(gIvrEvrMsg.callRefNum, "%d", gReferenceNum);

   if(true != gIvrEvrApi.GenerateEvr (gIvrEvrMsg))
   {
      return false;
   }
   return true;
}

/*
//----------------------------------------------------------------------
// METHOD NAME : ProcessCallCharge
// DESCRIPTION : Blocks Amount
// PARAMETERS  : NONE
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN PromptApi::ProcessCallCharge(EnumBillingEvents billingEvents)
{
   TEXT       lLogText[MAX_LOG_TEXT_LEN] = "";

   mBillingRequest.appId = IVR_VXML_PROC;
   mBillingRequest.mode     = CHARGE_MODE_UNKNOWN;
   mBillingRequest.referenceNum = gReferenceNum;
   mBillingRequest.expiryTime =  BILLING_DEFAULT_EXPIRY_TIME;
   mBillingRequest.event = billingEvents;

   //strcpy(mBillingRequest.msisdn, gCallingNumber);
   strcpy(mBillingRequest.purpose, "Prompt");

   //mBillingRequest.subsType = mProvDetails.subsInfo.subsType;
   //mBillingRequest.shortCode,
   //mBillingRequest.imsi
   //mBillingRequest.chargeId;
   mBillingRequest.contentId  = mChargeContentId;
   mBillingRequest.channelId  = mCircuitId;

   if(true != mBillingApi.ProcessBillingRequest (mBillingRequest, mBillingResponse))
   {
      sprintf(lLogText, "%s Status:%d",
            mLogText, mBillingResponse.status);
      TERR (gTrace, printf ("%s: %s Billing Failed\n",
               gProcessName, lLogText););
      //   gLog.GenerateLog(lLogText);
      return false;
   }

   return true;
}
*/

//----------------------------------------------------------------------
// METHOD NAME : ProcessCallCharge
// DESCRIPTION : Blocks Amount
// PARAMETERS  : NONE
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN PromptApi::ProcessCallCharge(EnumBillingEvents billingEvents)
{
   TEXT       lLogText[MAX_LOG_TEXT_LEN] = "";
   BOOLEAN    lRetVal = true;

   mBillingRequest.appId = IVR_VXML_PROC;
   mBillingRequest.mode     = CHARGE_MODE_UNKNOWN;
   mBillingRequest.referenceNum = gReferenceNum;
   mBillingRequest.expiryTime =  BILLING_DEFAULT_EXPIRY_TIME;
   mBillingRequest.event = billingEvents;

   strcpy(mBillingRequest.msisdn, gCallingNumber);
   strcpy(mBillingRequest.shortCode, gCalledNumber);
   strcpy(mBillingRequest.purpose, "Content");

   //mBillingRequest.subsType = mProvDetails.subsInfo.subsType;
   //mBillingRequest.shortCode,
   //mBillingRequest.imsi
   //mBillingRequest.chargeId;
   mBillingRequest.contentId  =  mChargeContentId;
   mBillingRequest.channelId = mCircuitId;

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X Msisdn:%s Event:%d(%s) SubsType:%d(%s) ChargeId:%u Expiry:%u",
         mBillingRequest.referenceNum,
         mBillingRequest.msisdn, mBillingRequest.event,
         BillingUtil::ConvertBillingEventTypeToStr (mBillingRequest.event),
         mBillingRequest.subsType,
         BillingUtil::ConvertSubsTypeToStr (mBillingRequest.subsType),
         mBillingRequest.chargeId, mBillingRequest.expiryTime);

   if(billingEvents == BILLING_CONTENT_PLAY)
   {
      mIsBillingInProgress = false;

      if(true != mBillingApi.ProcessBillingRequest (mBillingRequest, mBillingResponse))
      {
         //sprintf(lLogText, "%s Status:%d",
         //      gLogText, mBillingResponse.status);
         TERR (gTrace, printf ("%s: %s Billing Failed\n",
                  gProcessName, lLogText););
         mIsBillingInProgress = false;
         //   gLog.GenerateLog(lLogText);
         lRetVal = false;
         //return false;
      }
      else
      {
         if(mBillingResponse.status == BILLING_SUCCESS)
         {
            mIsBillingInProgress = true;
            T(gTrace, printf("%s: %s Billing Success\n", gProcessName, lLogText););
         }
         else
         {
            T(gTrace, printf("%s: %s Billing Disabled\n", gProcessName, lLogText););
         }
      }
   }
   else if(mIsBillingInProgress)
   {
      if(true != mBillingApi.ProcessBillingRequest (mBillingRequest, mBillingResponse))
      {
         //sprintf(lLogText, "%s Status:%d",
         //      gLogText, mBillingResponse.status);
         TERR (gTrace, printf ("%s: %s Billing Failed\n",
                  gProcessName, lLogText););
         mIsBillingInProgress = false;
         //   gLog.GenerateLog(lLogText);
         lRetVal = false;
      }
   }
   else
   {
      T(gTrace, printf("%s: %s Billing Disabled\n", gProcessName, lLogText););
   }


   if(billingEvents == BILLING_CONTENT_STOP)
   {
      mIsBillingInProgress = false;
   }
   return lRetVal;
}


//---------------------------------------------------------------
// METHOD      : ProcessFillerPlayRequest 
// DESCRIPTION : Send Filler Play request to underlying media
// PARAMETER   : no arguments  
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::ProcessFillerPlayRequest(TEXT *src, BOOLEAN repeatEnable)
{
   if(mIsStopRequested)
      return false;
   MpRequestDetails      lMpRequestDetails;
   MpResponseDetails     lMpResponseDetails;
   TEXT                  lLogText[MAX_LOG_TEXT_LEN + 1]="";
   int                   lFileCount = 1;

   memset( &lMpRequestDetails, 0, sizeof(MpRequestDetails) );
   memset( &lMpResponseDetails, 0, sizeof(MpResponseDetails) );

   lMpRequestDetails.cmdType                            = MC_CNTTN; 
   lMpRequestDetails.mpReqData.playMsg.seqNum           = gReferenceNum;
   lMpRequestDetails.mpReqData.playMsg.circuitId        = mCircuitId;

   //Barge-in is always disabled as it is filler Play
   lMpRequestDetails.mpReqData.playMsg.dtmfEnable    = false; //Continue Play on dtmf detection

   //Repeat is always enabled as it is filler play
   lMpRequestDetails.mpReqData.playMsg.repeatPlay       = repeatEnable;
   lMpRequestDetails.mpReqData.playMsg.vFileCount       = lFileCount; 
   strcpy(lMpRequestDetails.mpReqData.playMsg.dtmfStopPlayCharSet, mTermchar);
   strcpy(lMpRequestDetails.mpReqData.playMsg.voiceFile[0], src);

   snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Nof:%d", gReferenceNum,
         mCircuitId, 
         lFileCount);

   // Commented, as always there is a Filler play (Background Audio to be played
   //if( 0 == lFileCount )
   //{
   //   T(gTrace, printf("%s: %s No files for playing returning\n", gProcessName, lLogText););
   //   return true;
   //}	

   //Play Control is disabled as it is filler play
   lMpRequestDetails.mpReqData.playMsg.playCtrlEnable       = false;  //interupt play on dtmf Detection


   mIsPlayInProgress = true;
   if( false == mMediaLibrary.PlayNonBlockingMode( lMpRequestDetails ) )
   {
      snprintf( lLogText, MAX_LOG_TEXT_LEN, "CIC:%d Sending Play request Played", mCircuitId);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB08, lLogText);	
      mIsPlayInProgress = false;
      return false;
   }

   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_REQUESTED);
   gChannelApi.SendChannelStatus(mCircuitId, IVR_CHANNEL_PLAYING, gReferenceNum);

   gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
   gIvrEvrCause   =  CAS_IVR_REQ_RECEIVED;
   gIvrEvrReason  =  RSN_IVR_NONE;
   this->GenerateEvr();
/*   if(mIsBillingInProgress)
   {
      if(true != this->ProcessCallCharge(BILLING_CONTENT_START))
      {
         return false;
      }
   }

   if( false == mMediaLibrary.PlayBlockingMode( lMpRequestDetails , lMpResponseDetails) )
   {
      mIsPlayInProgress = false;
      T(gTrace, printf("%s: %s Play Request failed\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, lLogText);	
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_FAILED);
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_FAILED;
      gIvrEvrReason  =  RSN_IVR_MEDIA_REQ_RESOURCE_UNAVAILABLE; //// need to verify
      this->GenerateEvr();
      if(mIsBillingInProgress)
      {
         this->ProcessCallCharge(BILLING_CONTENT_STOP);
      }
      return false;
   } 
   mIsPlayInProgress = false;
   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_OVER);

   if(MP_RSP_SUCCESS == lMpResponseDetails.rspMsg )
   {
      memset ( &mplayDetails, 0, sizeof(PlayDetails)); //Played Queued Files memset struct for new request
      T(gTrace, printf("%s: %s Play Req sent and successful response recvd\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB03, LOG_LEVEL_1, lLogText);	
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_COMPLETE;
      gIvrEvrReason  =  RSN_IVR_NONE;
      this->GenerateEvr();
      if(mIsBillingInProgress)
      {
         if(true != this->ProcessCallCharge(BILLING_CONTENT_STOP))
         {
            return false;
         }
      }
      return true;
   }
   else
   {	
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_FAILED);
      snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Nof:%d RspStatus:%d", gReferenceNum,
            mCircuitId, 
            mplayDetails.fileCount, 
            lMpResponseDetails.rspMsg);

      memset( &mplayDetails, 0, sizeof(PlayDetails)); //Played Queued Files memset struct for new request
      T(gTrace, printf("%s: %s Play Req sent but NACK respose recvd\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, lLogText);	
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_FAILED;
      gIvrEvrReason  =  RSN_IVR_MEDIA_REQ_INVALID_FILE;  // need to verify
      this->GenerateEvr();
      if(mIsBillingInProgress)
      {
         this->ProcessCallCharge(BILLING_CONTENT_STOP);
      }
      return false;
   }	
   */
   return true;
}
//---------------------------------------------------------------
// METHOD      : ProcessFilePlayRequest 
// DESCRIPTION : Send Filler Play request to underlying media
// PARAMETER   : no arguments  
// RETURN      : BOOLEAN
//---------------------------------------------------------------
BOOLEAN PromptApi::ProcessFilePlayRequest(TEXT *src, BOOLEAN repeatEnable)
{
   if(mIsStopRequested)
      return false;
   MpRequestDetails      lMpRequestDetails;
   MpResponseDetails     lMpResponseDetails;
   TEXT                  lLogText[MAX_LOG_TEXT_LEN + 1]="";

   memset( &lMpRequestDetails, 0, sizeof(MpRequestDetails) );
   memset( &lMpResponseDetails, 0, sizeof(MpResponseDetails) );

   lMpRequestDetails.cmdType                            = MC_CNTTN; 
   lMpRequestDetails.mpReqData.playMsg.seqNum           = gReferenceNum;
   lMpRequestDetails.mpReqData.playMsg.circuitId        = mCircuitId;

   //Barge-in is always disabled as it is filler Play
   lMpRequestDetails.mpReqData.playMsg.dtmfEnable    = false; //Continue Play on dtmf detection

   //Repeat is always enabled as it is filler play
   lMpRequestDetails.mpReqData.playMsg.repeatPlay       = repeatEnable;
   lMpRequestDetails.mpReqData.playMsg.vFileCount       = 1; 
   strcpy(lMpRequestDetails.mpReqData.playMsg.dtmfStopPlayCharSet, mTermchar);
   strcpy(lMpRequestDetails.mpReqData.playMsg.voiceFile[0], src);

   snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Nof:1", gReferenceNum,
         mCircuitId);

   //Play Control is disabled as it is filler play
   lMpRequestDetails.mpReqData.playMsg.playCtrlEnable       = false;  //interupt play on dtmf Detection

   mIsPlayInProgress = true;
   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_REQUESTED);
   gChannelApi.SendChannelStatus(mCircuitId, IVR_CHANNEL_PLAYING, gReferenceNum);

   gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
   gIvrEvrCause   =  CAS_IVR_REQ_RECEIVED;
   gIvrEvrReason  =  RSN_IVR_NONE;
   this->GenerateEvr();

   if( false == mMediaLibrary.PlayBlockingMode( lMpRequestDetails , lMpResponseDetails) )
   {
      mIsPlayInProgress = false;
      T(gTrace, printf("%s: %s Play Request failed\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, lLogText);	
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_FAILED);
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_FAILED;
      gIvrEvrReason  =  RSN_IVR_MEDIA_REQ_RESOURCE_UNAVAILABLE; //// need to verify
      this->GenerateEvr();
      return false;
   } 
   mIsPlayInProgress = false;
   gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_OVER);

   if(MP_RSP_SUCCESS == lMpResponseDetails.rspMsg )
   {
      //memset ( &mplayDetails, 0, sizeof(PlayDetails)); //Played Queued Files memset struct for new request
      T(gTrace, printf("%s: %s Play Req sent and successful response recvd\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB03, LOG_LEVEL_1, lLogText);	
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_COMPLETE;
      gIvrEvrReason  =  RSN_IVR_NONE;
      this->GenerateEvr();
      return true;
   }
   else
   {	
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PLAY_FAILED);
      snprintf( lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d Nof:1 RspStatus:%d", gReferenceNum,
            mCircuitId, 
            lMpResponseDetails.rspMsg);

      //memset( &mplayDetails, 0, sizeof(PlayDetails)); //Played Queued Files memset struct for new request
      T(gTrace, printf("%s: %s Play Req sent but NACK respose recvd\n", gProcessName, lLogText););
      gLog.GenerateLog(IVR_PLIB04, LOG_LEVEL_1, lLogText);	
      gIvrEvrEvent   =  EVT_IVR_MEDIA_PLAY;
      gIvrEvrCause   =  CAS_IVR_REQ_FAILED;
      gIvrEvrReason  =  RSN_IVR_MEDIA_REQ_INVALID_FILE;  // need to verify
      this->GenerateEvr();
   }	
   return true;
}

//*********************************************************************
//  "C" Functions accessed by other libraries and Process defined below 
//*********************************************************************

//---------------------------------------------------------------
// METHOD      : TSSInitPromptApi
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
BOOLEAN TSSInitPromptApi( )
{
   return gPromptApi.InitPromptApi();
}
//---------------------------------------------------------------
// METHOD      : TSSSetCirciutId 
// DESCRIPTION : Constructor
// PARAMETER   : no arguments
// RETURN      : 
//---------------------------------------------------------------
void TSSSetCirciutId( int circiutId)
{
   gPromptApi.SetCirciutId(circiutId);

}	
//---------------------------------------------------------------
// METHOD      : TSSVXIpromptGetVersion
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIint32 TSSVXIpromptGetVersion(void)
{
   return 1;
}
//---------------------------------------------------------------
// METHOD      : TSSVXIpromptGetImplementationName 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIchar* TSSVXIpromptGetImplementationName()
{
   return (VXIchar*) VXI_CURRENT_VERSION_STR;
}
//---------------------------------------------------------------
// METHOD      : TSSVXIpromptBeginSession 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIpromptResult TSSVXIpromptBeginSession(VXIpromptInterface * pThis, VXIMap *properties)
{
  	T(gTrace, printf("%s: Begin Session CallBack \n", gProcessName);)
   //printf("##############promptBeginSession##############0\n");
   //VXIPrintMap(properties);
   //printf("##############promptBeginSession###############\n");
   gPromptApi.ClearStopFlag();

   return VXIprompt_RESULT_SUCCESS;
}
//---------------------------------------------------------------
// METHOD      :  TSSVXIpromptPrefetch 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIpromptResult TSSVXIpromptPrefetch(VXIpromptInterface * vxip,
      const VXIchar *type,
      const VXIchar *src,
      const VXIchar *text,
      const VXIMap* properties)
{
   //	T(gTrace, printf("%s: prompt Prefetch CallBack \n", gProcessName););

   //printf("##########TSSVXIpromptPrefetch################0\n");
   //VXIPrintMap(properties);
   //printf("##########TSSVXIpromptPrefetch#################\n");

   if(false == gPromptApi.ProcessPrefetch( vxip, type, src ,text, properties ))
   {
      return VXIprompt_RESULT_FAILURE;
   }	
   return VXIprompt_RESULT_SUCCESS;
}	
//---------------------------------------------------------------
// METHOD      : TSSVXIpromptQueue 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIpromptResult TSSVXIpromptQueue(VXIpromptInterface* vxip,
      const VXIchar *raw_type,
      const VXIchar *raw_src,  /* not used */
      const VXIchar *raw_text,
      const VXIMap  *properties)
{
   //printf("################ProcessQueue##################0\n");
   //VXIPrintMap(properties);
   //printf("################ProcessQueue###################\n");

   if( true 	== gPromptApi.ProcessQueue(vxip, raw_type, raw_src, raw_text,properties))
   {
      return VXIprompt_RESULT_SUCCESS;
   }
   else
      return VXIprompt_RESULT_FAILURE;
}	

VXIpromptResult TSSVXIpromptFillerPlay(VXIpromptInterface* vxip, unsigned char *src,
									const VXIMap  *properties)
{
   //printf("################ProcessFillerPlay#############0\n");
   //VXIPrintMap(properties);
   //printf("################ProcessQueue###################\n");

   TEXT tempTxt[1000]= "";
   //SWIwcstoutf8(raw_src, tempTxt, 500);
   //wcstombs ( mystr , raw_src, 1000) ;
   snprintf(tempTxt, 1000, "%s", src); 
   if( true 	== gPromptApi.ProcessFillerPlayRequest(tempTxt))
   {
      return VXIprompt_RESULT_SUCCESS;
   }
   else
      return VXIprompt_RESULT_FAILURE;
}	


//---------------------------------------------------------------
// METHOD      : TSSVXIpromptPlay 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIpromptResult TSSVXIpromptPlay(VXIpromptInterface * vxip)
{
   //Play the Queued Prompts
   //T(gTrace, printf("%s: Play CallBack \n", gProcessName););

   if( false == gPromptApi.ProcessPlayRequest())
   {
      return VXIprompt_RESULT_FAILURE;
   }	
   return VXIprompt_RESULT_SUCCESS;
}
//---------------------------------------------------------------
// METHOD      : TSSVXIpromptWait 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIpromptResult TSSVXIpromptWait(VXIpromptInterface* vxip,
      VXIpromptResult* playResult)
{
   //T(gTrace, printf("%s: prompt Wait CallBack \n", gProcessName););
   //mIsStopRequested = false;
   // SetAndCheckTimer
   //gPromptApi.SetTimerAndWaitForReponse();

   //gPromptApi.ClearStopFlag();
   return VXIprompt_RESULT_SUCCESS;
}
//---------------------------------------------------------------
// METHOD      : TSSVXIpromptEndSession 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
VXIpromptResult TSSVXIpromptEndSession(VXIpromptInterface *pThis, VXIMap *properties)
{
   //printf("##############promptEndSession################0\n");
   //VXIPrintMap(properties);
   //printf("##############promptEndSession#################\n");

   if(false  == gPromptApi.ProcessEndRequest())
   {
      return VXIprompt_RESULT_FAILURE;
   }
   return VXIprompt_RESULT_SUCCESS;
}

//---------------------------------------------------------------
// METHOD      : CollectDtmf 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
BOOLEAN TSSCollectDtmf( int cic, int noOfDigit, int interDigitTimeout, TEXT *termChars , TEXT *dtmf)
{
   //interDigitTimeout = interDigitTimeout*1000;
   return gPromptApi.CollectDtmf(cic, noOfDigit, interDigitTimeout, termChars, dtmf);
}

//---------------------------------------------------------------
// METHOD      : CollectDtmf 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
BOOLEAN TSSCollectDtmf( int cic, int noOfDigit, int interDigitTimeout, TEXT *termChars , TEXT *dtmf, 
      TEXT *pauseDtmf, TEXT *resumeDtmf, TEXT *fwdDtmf, TEXT *rwndDtmf ) // Recognition
{
   //interDigitTimeout = interDigitTimeout*1000;
   return gPromptApi.CollectDtmf(cic, noOfDigit, interDigitTimeout, termChars, dtmf, pauseDtmf, resumeDtmf, fwdDtmf, rwndDtmf);
}

//---------------------------------------------------------------
// METHOD      : TSSRecord 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//---------------------------------------------------------------
BOOLEAN TSSRecord(const VXIMap *props,  TEXT* recordFileName, int& recordingTime , TEXT *termChar)
{
   //strcpy( recordFileName , "/opt/ivr/audio/Record1.wav" ) ;
   return gPromptApi.TSSVXIRecord( props, recordFileName , recordingTime , termChar );
   //printf("***************Record File Name : %s\n", recordFileName);
   //return gPromptApi.TSSVXIRecord( props, recordFileName , recordingTime , termChar );
   //return true;
}

