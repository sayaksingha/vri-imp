// @(#) $Id: PromptApi.h,v 1.1.1.1.2.6.2.3 2024/03/01 09:54:14 mram Exp $
//-----------------------------------------------------------------------------
// NAME
// PromptApi.h
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

#ifndef INCL_PROMPT_API_H
#define INCL_PROMPT_API_H

//#include "IvrStructs.h"
#include "VXIheaderPrefix.h"
#include "VXItypes.h"                  /* For VXIprompt base interface */
#include "VXIvalue.h"                  /* For VXIprompt base interface */
#include "VXIlog.h"                    /* For VXIlog interface */
#include "VXIinet.h"                   /* For VXIinet interface */
#include "VXIcache.h"                  /* For VXIcache interface */
#include "VXIprompt.h"                 /* For VXIprompt base interface */
#include "MediaInterface.h"                 /* For VXIprompt base interface */
#include "Types.h"
#include "VXIrec.h"
#include "IvrConstDef.h"
#include "BillingApi.h"

#define MAX_NO_FILES              10 
#define MAX_CIC                   10 
#define MIN_DTMF_COLLECTED_DIGITS 1
#define MAX_DTMF_COLLECTED_DIGITS 50

typedef std::basic_string<VXIchar> vxistring;

struct FileDetails
{
	TEXT actualName[100]; 
	TEXT fetchedName[100];
	INT32 status;
};	

struct PlayDetails
{
  TEXT 	fileName[MAX_VOICE_FILES][MAX_VOICEFILE_LEN];
  //TEXT 	fileName[50][100];
  int    fileCount;
};	

struct cktStatus
{
	INT32 cktId;
	INT32 cktStauts;  
};

enum EnumResType 
{
	RES_TRUE,
	RES_FALSE,
	RES_TIME_OUT

};	

struct PlayResponse
{
	BOOLEAN  result;
	TEXT     dtmfcollected[10];   
};


typedef HashTable<TEXT, 200, struct FileDetails> fileHashTable;

class PromptApi
{
	private:

	MsgQueue             mReqMsgQue;	
	key_t						mReqMsgQKey;

	MsgQueue             mRspMsgQue;
   key_t 					mRspMsgQKey;

   key_t 					mHashTblShmKey;
	key_t                mFileShmKey;
	key_t                mCiruitShmKey;
	key_t                mShmKey;

   key_t 					mHashTblSemKey;
	TEXT*      				mHashTableMemory;
	fileHashTable        mHashTbl;
	INT32                mHashTableSize;
	
	cktStatus            *mCktStatus;
	INT32 					mShmSize;
	INT32                mShmId;



   TEXT					   mLogText[MAX_LOG_TEXT_LEN + 1];

	BOOLEAN              CreateSharedMemory();
   BOOLEAN              CreateHashTable();
								
	PlayDetails				mplayDetails;

	TEXT 						mBargeinFlag[10];
	TEXT 						mBargeinType[10];
	TEXT 						mTermchar[10];
	TEXT 						mTermTimeOut[10];
	TEXT 						mMaxRecordingTime[10];
	TEXT 						mBeep[10];
	TEXT 						mTermRecOnDtmf[10];
	TEXT 						mFinalSilence[10];
	TEXT 						mTermCharTimeOut[10];
	TEXT 						mTotalTimeOut[10];
   UINT8                mMaxDtmf;

   BillingRequest       mBillingRequest;
   BillingResponse      mBillingResponse;
   BillingApi           mBillingApi;
   BOOLEAN              mIsBillingRequested;
   BOOLEAN              mIsBillingInProgress;
	UINT32               mChargeContentId;
  
   TEXT                 mIvrBasePath[MAX_PATH];

   BOOLEAN ProcessCallCharge(EnumBillingEvents billingEvents);

	public:
		PromptApi();

		~PromptApi();

		BOOLEAN 					ReadConfig();
		INT32                mCfgSecs; //Read from Cfg
		PlayResponse         mPlayResponse; 

		void					 	ReloadConfig();

      void                 ClearStopFlag();

		BOOLEAN 					CreateQueues();
	
		BOOLEAN 					InitPromptApi( );

		BOOLEAN 					DeInitPromptApi();

		BOOLEAN 					InsertIntoHashTable(TEXT*, FileDetails);

		BOOLEAN 					DeleteFromHashTable(TEXT*, FileDetails);

		BOOLEAN	            GetDataFromHash(TEXT*, FileDetails);

		BOOLEAN           	ParseDataForText(TEXT*, TEXT*, TEXT*);

		BOOLEAN              ParseDataForFile( TEXT*);

		void                 SetCirciutId(int);

      BOOLEAN              GenerateEvr();
		
		INT16 	            mCircuitId;

		MediaInterface       mMediaLibrary;
	  	
		BOOLEAN              ProcessPrefetch(VXIpromptInterface*, const VXIchar*, const VXIchar*, const VXIchar*, const VXIMap*);
		
		BOOLEAN              ProcessQueue(VXIpromptInterface* vxip, const VXIchar *raw_type, const VXIchar *raw_src,  /* not used */
									 const VXIchar *raw_text, 	const VXIMap  *properties);

	  	BOOLEAN              ProcessPlayRequest();
	
      BOOLEAN              ProcessFillerPlayRequest(TEXT *src, BOOLEAN repeatEnable=true);

      BOOLEAN              ProcessFilePlayRequest(TEXT *src, BOOLEAN repeatEnable=true);

	  	BOOLEAN              ProcessEndRequest();

	   BOOLEAN              CollectDtmf( int ,int , int , TEXT* , TEXT *dtmf,
                                        TEXT *pauseDtmf=NULLPTR, TEXT *resumeDtmf=NULLPTR, 
                                        TEXT *fwdDtmf=NULLPTR, TEXT *rwndDtmf=NULLPTR);

		BOOLEAN              TSSVXIRecord(const VXIMap *props,  TEXT* recordFileName, int& recordingTime , TEXT *termChar);
	  	
		EnumResType          SetTimerAndWaitForReponse();

		BOOLEAN              mIsPlayInProgress;

		BOOLEAN              mIsDtmfInProgress;

		BOOLEAN              mIsRecInProgress;

      BOOLEAN              mIsStopRequested;

      BOOLEAN              mIsAudioOpSupported;
      TEXT                 mPauseDtmf[MAX_DTMF_LEN];
      TEXT                 mResumeDtmf[MAX_DTMF_LEN];
      TEXT                 mForwardDtmf[MAX_DTMF_LEN];
      TEXT                 mRewindDtmf[MAX_DTMF_LEN];

      static   TEXT                 mDtmf[30 + 1];
};

BOOLEAN 							TSSInitPromptApi();

void 								TSSSetCirciutId( int circiutId);

VXIint32 						TSSVXIpromptGetVersion(void);

VXIchar* 						TSSVXIpromptGetImplementationName(void);

VXIpromptResult 				TSSVXIpromptBeginSession(VXIpromptInterface * pThis, VXIMap *);

VXIpromptResult 				TSSVXIpromptEndSession(VXIpromptInterface *pThis, VXIMap *);

VXIpromptResult 				TSSVXIpromptPlay(VXIpromptInterface * vxip);

VXIpromptResult 				TSSVXIpromptPrefetch(VXIpromptInterface * vxip,
      const VXIchar *type,
      const VXIchar *src,
      const VXIchar *text,
      const VXIMap* properties);

VXIpromptResult 				TSSVXIpromptFillerPlay(VXIpromptInterface* vxip, unsigned char *src,
									const VXIMap  *properties);

VXIpromptResult 				TSSVXIpromptQueue(VXIpromptInterface* vxip,
									const VXIchar *raw_type,
									const VXIchar *raw_src,  /* not used */
									const VXIchar *raw_text,
									const VXIMap  *properties);

VXIpromptResult 				TSSVXIpromptWait(VXIpromptInterface* vxip,	VXIpromptResult* playResult);

BOOLEAN 							TSSCollectDtmf( int ,int , int , TEXT* , TEXT *dtmf); // Recognition

BOOLEAN 							TSSCollectDtmf( int ,int , int , TEXT* , TEXT *dtmf, TEXT *pauseDtmf, TEXT *resumeDtmf, TEXT *fwdDtmf, TEXT *rwndDtmf ); // Recognition

BOOLEAN 							TSSRecord(const VXIMap *props,  TEXT* recordFileName, int& recordingTime , TEXT *termChar); // Record


#endif
