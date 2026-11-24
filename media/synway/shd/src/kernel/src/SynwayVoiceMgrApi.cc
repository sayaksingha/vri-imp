static const char* id="@(#) $Id: SynwayVoiceMgrApi.cc,v 1.1.4.3 2016/04/12 21:59:39 shijuc Exp $" ;
//-----------------------------------------------------------------------------
// NAME           : SynwayVoiceMgrApi.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Chethan V M           Date:
//----------------------------------------------------------------------------

#include "SynwayVoiceMgrApi.h"

INT32 gMaxCfgBoard, gMaxUsableBoard;

static ScheduledAnm gScheduledAnm[MAX_SYNWAY_CHANNEL];
static ScheduledIam gScheduledIam[MAX_SYNWAY_CHANNEL];
//-------------------------------------------------------------
// FUNCTION    : SynwayVoiceMgrApi
// DESCRIPTION : Constructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
SynwayVoiceMgrApi::SynwayVoiceMgrApi()
{
   mpSynwaySystem = NULL;
   memset(gScheduledAnm, 0, sizeof(gScheduledAnm));
   memset(gScheduledIam, 0, sizeof(gScheduledIam));
}

//-------------------------------------------------------------
// FUNCTION    : SynwayVoiceMgrApi
// DESCRIPTION : Destructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
SynwayVoiceMgrApi::~SynwayVoiceMgrApi()
{ 
}
   
//-------------------------------------------------------------
// FUNCTION    : Init
// DESCRIPTION : Initializes library
// PARAMETER   : None
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Init(INT16 boardId, INT16 minChnl, INT16 maxChnl, 
TEXT *cfgFile, TEXT *idxFile)
{
	TEXT 			lLogText[MAX_LOG_TEXT_LEN] = "";

   T(gTrace, printf("%s: Initialising Synway Libraray BoardId:%d \n", 
				gProcessName, boardId););
   
   if (true != ReadConfig())
   {  
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "ReadConfig Failed MP_CFG_READ_ERR");
      TERR(gTrace, printf("%s: %s Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return MP_CFG_READ_ERR;
   }

   if( true != InitChnlMgmt())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "InitChnlMgmt Failed");
      TERR(gTrace, printf("%s: %s Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return MP_INIT_ERR;
   }
   
   if(true != InitSynway(cfgFile, idxFile))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Initialising synway Library");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI01, lLogText);
		return MP_BOARD_INIT_ERR;
	}   

	mpSynwaySystem->boardID 		= boardId;
	mpSynwaySystem->minChannel 	= minChnl;
	mpSynwaySystem->maxChannel 	= maxChnl;

   if(mpSynwaySystem->maxChannel > mpSynwaySystem->mMaxChannel-1)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Channel Configued Per Process in More than Available ChnlPerProc:%d-%d ChnlAvailable:0-%d",
            mpSynwaySystem->minChannel, mpSynwaySystem->maxChannel, mpSynwaySystem->mMaxChannel);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI01, lLogText);
      return MP_INIT_ERR;
   }

   for(int i = 0; i < mpSynwaySystem->mMaxChannel; i++)
   {
      mpSynwaySystem->channel[i].status = CKT_IDLE;
   }

	T(gTrace, printf("%s: Synway Initialization Success\n", gProcessName););

	return MP_SUCCESS;
}
//-------------------------------------------------------------
// FUNCTION    : ReadConfig
// DESCRIPTION : Reads the configuration file 
// PARAMETER   :   
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::ReadConfig()
{

	CfgRead      lCfgReadObj(gProcessName);
   ConfigReturn lCfgRet;
	INT32		    lTemp_Cfg = 0;
	TEXT		    lTempTxt[25] = "";
	TEXT 		    lLogText[MAX_LOG_TEXT_LEN] = "";

   // Read config values from media.cfg
   if (false == lCfgReadObj.CfgInit(SYNWAY_MEDIA_CFG) )
   {
		snprintf(lLogText, MAX_LOG_TEXT_LEN,"Could not open file %s",SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }
  
   //Signaling Link Status Log File Path
   if (CFG_OK != lCfgReadObj.GetConfigStr("SYNWAY_LINK_STATUS_LOG_FILE",
            mLinkStatusFile,
            SYNWAY_MIN_FILE_PATH_LEN,
            SYNWAY_MAX_FILE_PATH_LEN))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Config read failed for SYNWAY_LINK_STATUS_LOG_FILE in  File:%s",
            SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s:  %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
   T(gTrace, printf("%s: SYNWAY_LINK_STATUS_LOG_FILE = %s\n", gProcessName, mLinkStatusFile););

   //File Play format
   if (CFG_OK != lCfgReadObj.GetConfigStr("SYNWAY_PLAY_VOICE_FILE_FORMAT",
            lTempTxt,
                                          SYNWAY_MIN_FILE_FORMAT_LEN,
                                          SYNWAY_MAX_FILE_FORMAT_LEN))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Config read failed for SYNWAY_PLAY_VOICE_FILE_FORMAT in  File:%s",
				SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s:  %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
	mPlayFileFormat_Cfg = atoi(lTempTxt);
	T(gTrace, printf("%s: SYNWAY_PLAY_VOICE_FILE_FORMAT = %d\n", gProcessName, mPlayFileFormat_Cfg););

   //File Record format
   if (CFG_OK != lCfgReadObj.GetConfigStr("SYNWAY_RECORD_VOICE_FILE_FORMAT",
                                          lTempTxt,
                                          SYNWAY_MIN_FILE_FORMAT_LEN,
                                          SYNWAY_MAX_FILE_FORMAT_LEN))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"Config read failed for SYNWAY_RECORD_VOICE_FILE_FORMAT in  File:%s", SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s:  %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
	mRecordFileFormat_Cfg = atoi(lTempTxt);
	T(gTrace, printf("%s: SYNWAY_RECORD_VOICE_FILE_FORMAT = %d\n", gProcessName, mRecordFileFormat_Cfg););

   //Timeout for wait event
   if (CFG_OK != lCfgReadObj.GetConfigNum("SYNWAY_TIME_OUT_FOR_WAIT_EVENT",
														lTemp_Cfg,
                                          SYNWAY_MIN_TIMEOUT_VAL,
                                          SYNWAY_MAX_TIMEOUT_VAL))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"Config read failed for SYNWAY_TIME_OUT_FOR_WAIT_EVENT in File:%s",
				SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s:  %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }

	if(lTemp_Cfg == 4000000)
		mTimeOutEvt_Cfg = 0xffffffff;
	else
		mTimeOutEvt_Cfg = lTemp_Cfg;

	T(gTrace, printf("%s: SYNWAY_TIME_OUT_FOR_WAIT_EVENT = %d\n", gProcessName, mTimeOutEvt_Cfg););

   if (CFG_OK != lCfgReadObj.GetConfigNum("SYNWAY_FILE_PLAY_PROGRESS_CHECK_INETRVAL",
            mFilePlayPrfChkIntr,
            MIN_FILE_PROGRESS_CHECK,
            MAX_FILE_PROGRESS_CHECK))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for SYNWAY_FILE_PLAY_PROGRESS_CHECK_INETRVAL in File:%s",
            SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s:  %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
   T(gTrace, printf("%s: SYNWAY_FILE_PLAY_PROGRESS_CHECK_INETRVAL = %d\n", gProcessName, mFilePlayPrfChkIntr););

   if (CFG_OK != lCfgReadObj.GetConfigNum("SYNWAY_FILE_RECORD_PROGRESS_CHECK_INETRVAL",
            mFileRecordPrfChkIntr,
            MIN_FILE_PROGRESS_CHECK,
            MAX_FILE_PROGRESS_CHECK))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for SYNWAY_FILE_RECORD_PROGRESS_CHECK_INETRVAL in File:%s",
            SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s:  %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
   T(gTrace, printf("%s: SYNWAY_FILE_RECORD_PROGRESS_CHECK_INETRVAL = %d\n", gProcessName, mFileRecordPrfChkIntr););

   if (CFG_OK != lCfgReadObj.GetConfigNum("SYNWAY_MAX_CALL_DURATION",
            mMaxCallDur,
            MIN_SYNWAY_MAX_CALL_DURATION,
            MAX_SYNWAY_MAX_CALL_DURATION))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for SYNWAY_MAX_CALL_DURATION in File:%s",
            SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s:  %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgReadObj.CfgDeInit();
      return false;
   }
   T(gTrace, printf("%s: SYNWAY_MAX_CALL_DURATION = %d\n", gProcessName, mMaxCallDur););

   lCfgReadObj.CfgDeInit();
   if (false == lCfgReadObj.CfgInit(SYNWAY_IPC_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n",
                           gProcessName, lLogText););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }

   if(CFG_OK != ( lCfgRet = lCfgReadObj.GetConfigNum("SNW_CHNL_STATUS_SHM_KEY",
                                                   mChnlStShmKey,
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
      T(gTrace, printf("%s: SNW_CHNL_STATUS_SHM_KEY = %d\n", gProcessName, mChnlStShmKey););
   }

   if(CFG_OK != ( lCfgRet = lCfgReadObj.GetConfigNum("SNW_CHNL_STATUS_SEM_KEY",
                                                   mChnlStSemKey,
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
      T(gTrace, printf("%s: SNW_CHNL_STATUS_SEM_KEY = %d\n", gProcessName, mChnlStSemKey););
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
BOOLEAN SynwayVoiceMgrApi::InitChnlMgmt()
{
   UINT32           lSharedMemSize = 0;
   BOOLEAN          lShmCreated    = false;
   TEXT             lLogText[MAX_LOG_TEXT_LEN] = "";

   lSharedMemSize = sizeof(synwaySystem);

   mShmChnlMgmtId = shmget (mChnlStShmKey, lSharedMemSize, IPC_PERM);
   snprintf (lLogText,MAX_LOG_TEXT_LEN, "ShmChnlMgmtKey:%u Size:%u",
         mChnlStShmKey, lSharedMemSize);
   if (mShmChnlMgmtId < 0)
   {
      // Share memory Not present. Creating....
      mShmChnlMgmtId = shmget (mChnlStShmKey, lSharedMemSize, IPC_PERM | IPC_CREAT);
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

   mpSynwaySystem = (synwaySystem*) shmat (mShmChnlMgmtId, (void *) 0, 0);
   if (NULL == mpSynwaySystem)
   {
      TERR (gTrace, printf ("%s: %s Error Attaching to Shared Memory Err:%d ErrStr:%s\n", 
               gProcessName, lLogText, errno, strerror(errno)););
      return false;
   }
   T (gTrace, printf ("%s: %s Shared Memory Attached\n", gProcessName, lLogText););

   //Clear Shared Memory 
   memset(mpSynwaySystem, 0, sizeof(synwaySystem));

   mSemChnlMgmtId = semget(mChnlStSemKey, MAX_SYNWAY_CHANNEL, IPC_PERM|IPC_CREAT);
   snprintf (lLogText,MAX_LOG_TEXT_LEN, "SemChnlMgmtKey:%u nsems:%u",
         mChnlStShmKey, MAX_SYNWAY_CHANNEL);
   if (-1 == mSemChnlMgmtId)
   {
      TERR (gTrace, printf("%s: %s Semaphore Creation  Failed Err:%d ErrStr:%s\n", 
               gProcessName, lLogText, errno, strerror(errno)););
      return false;
   }
   T (gTrace, printf("%s: %s SemId:%d Semaphore Creation Success.\n", 
            gProcessName, lLogText, mSemChnlMgmtId););

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : ChnlUnlock
// DESCRIPTION : Unlock Particular Channel
// PARAMETER   : int
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::ChnlUnlock(int semNum)
{
   struct sembuf   lsemLocker[1];
   int             lReturnVal;
   int             lnsops;
   TEXT            lLogText[MAX_LOG_TEXT_LEN +1] = "";

   lsemLocker[0].sem_num = semNum ;
   lsemLocker[0].sem_op  = -1;
   lsemLocker[0].sem_flg = SEM_UNDO | IPC_NOWAIT ;

   lnsops = 1;

   lReturnVal = semop(mSemChnlMgmtId, &lsemLocker[0], lnsops);
   if (-1 == lReturnVal)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CKTID:[%03d] SemUnlock Failed SemId:%d Err:%d ErrStr:%s",
            semNum, mSemChnlMgmtId, errno, strerror(errno));
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI88, lLogText);

      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : ChnlLock
// DESCRIPTION : Lock Particular Channel
// PARAMETER   : int
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::ChnlLock(int semNum)
{
   struct sembuf   lsemLocker[2];
   int             lReturnVal;
   int             lnsops;
   TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lsemLocker[0].sem_num = semNum;
   lsemLocker[0].sem_op  = 0;
   lsemLocker[0].sem_flg = SEM_UNDO;

   lsemLocker[1].sem_num = semNum;
   lsemLocker[1].sem_op  = 1;
   lsemLocker[1].sem_flg = SEM_UNDO | IPC_NOWAIT;

   lnsops = 2;

   lReturnVal = semop(mSemChnlMgmtId, &lsemLocker[0], lnsops);
   if (-1 == lReturnVal)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CKTID:[%03d] SemLock Failed SemId:%d Err:%d ErrStr:%s",
            semNum, mSemChnlMgmtId, errno, strerror(errno));
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI87, lLogText);

      return false;
   }

   return true;
}


//-------------------------------------------------------------
// FUNCTION    : InitSynway
// DESCRIPTION : Initializes library
// PARAMETER   : NONE
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::InitSynway(TEXT *cfgFile, TEXT *idxFile)
{
	TEXT 			lLogText[MAX_LOG_TEXT_LEN] 		= "";
	TEXT			lErrMsg[MAX_SYNWAY_ERR_MSG]		= "";


	if (-1 == access(cfgFile , F_OK))
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"%s Shconfiguration file not present not present", cfgFile);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;;
	}
	if(-1 == access(idxFile , F_OK))
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"%s Shindex file not present not present", idxFile);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	//Load configuration file and initialize system
	if(-1 == SsmStartCti(cfgFile, idxFile))
	{
		SsmGetLastErrMsg(lErrMsg);//Get error message
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmStartCti Failed EC:%d EM:%s", 
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI02, lLogText);
		return false;
	}
	mpSynwaySystem->boardType = SsmGetBoardModel(mpSynwaySystem->boardID);

	T (gTrace, printf("%s: SsmStartCti  Success BoardModel:%d\n", 
				gProcessName, mpSynwaySystem->boardType););

	mpSynwaySystem->mMaxChannel = SsmGetMaxCh();
	if(-1 == mpSynwaySystem->mMaxChannel)
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmGetMaxCh Failed EC:%d EM:%s",
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI03, lLogText);
		return false;
	}
	gMaxCfgBoard 		= SsmGetMaxCfgBoard();
	gMaxUsableBoard 	= SsmGetMaxUsableBoard();

	T (gTrace, printf("%s:\n***************System Details***************\n"
				"MaxCfgBoard      : %d\n"
				"MaxUsableBoard   : %d\n"
				"MaxChannel       : %d\n"
				"************************************************\n",
				gProcessName, gMaxCfgBoard, gMaxUsableBoard, SsmGetMaxCh()););

	T (gTrace, 
			printf("%s:\n***************Board Details*******************\n", gProcessName);
			for(INT16 brd = 0; brd < gMaxUsableBoard; brd++)
			printf(
				"BoardId                : %d\n"
				"Board Model            : 0x%02x\n"
				"SerialNumber           : %d\n"
				"***********************************************\n",
				brd,
				SsmGetBoardModel(brd),
				SsmGetPciSerialNo(brd));
			printf("\n"););

	T(gTrace,
			printf("%s:\n***************Channel Type*******************\n", gProcessName);
			for(INT16 chl = 0; chl < mpSynwaySystem->mMaxChannel; chl++)
			printf("Channel [%03d] Type:%d State:%d\n", chl + 1, SsmGetChType(chl), SsmGetChState(chl));
			printf("\n*************************************************\n"););

	if(true != SetEvents())
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SetEvents Failed" );
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI04, lLogText);
		return false;
	}

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : Deinit
// DESCRIPTION : De initialises library
// PARAMETER   : 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Deinit()
{
	TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";

	if(1 != SsmCloseCti())
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN, "SsmCloseCti Failed BoardId:%d", 
				mpSynwaySystem->boardID);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI05, lLogText);
		return MP_DEINIT_ERR;
	}

	snprintf(lLogText, MAX_LOG_TEXT_LEN, "SsmCloseCti Success BoardId:%d",
			mpSynwaySystem->boardID);
	T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
	
	sleep(2); //wait to disconnect channels and close system

   //Clear Shared Memory 
   memset(mpSynwaySystem, 0, sizeof(synwaySystem));
   shmdt(mpSynwaySystem);

	return MP_SUCCESS;
}



//-------------------------------------------------------------
// FUNCTION    : ConvertStatusToStr
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
TEXT* SynwayVoiceMgrApi::ConvertStatusToStr(EnumChannelStatus chnlStatus)
{
	switch(chnlStatus)
	{
		case CKT_UNAVAILABLE:
			return (TEXT*) "CKT_UNAVAILABLE";
		case CKT_IDLE:
			return (TEXT*) "CKT_IDLE";
		case CKT_PLAYING:
			return (TEXT*) "CKT_PLAYING";
		case CKT_PLAY_STOPING:
			return (TEXT*)"CKT_PLAY_STOPING";
		case CKT_RECORD_STOPING:
			return (TEXT*)"CKT_RECORD_STOPING";
		case CKT_GETDIG:
			return (TEXT*)"CKT_GETDIG";
		case CKT_RECORDING:
			return (TEXT*)"CKT_RECORDING";
		case CKT_PATCHED:
			return (TEXT*)"CKT_PATCHED";
		case CKT_PLAYING_AND_GETDIG:
			return (TEXT*)"CKT_PLAYING_AND_GETDIG";
		case CKT_RECORDING_AND_GETDIG:
			return (TEXT*)"CKT_RECORDING_AND_GETDIG";
		case CKT_PLAY_STOPING_AND_GETDIG:
			return (TEXT*)"CKT_PLAY_STOPING_AND_GETDIG";
		case CKT_RECORD_STOPING_AND_GETDIG:
			return (TEXT*)"CKT_RECORD_STOPING_AND_GETDIG";
		default:
			return (TEXT*)"CKT_CURRUPT";
	}
}

//-------------------------------------------------------------
// FUNCTION    : Play
// DESCRIPTION : plays the passed file as arguments on channel specified
// PARAMETER   : request and response structure 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Play(const MpPlay req, TEXT *lType)
{
	INT16			   lCktId   = -1;
	synwayChannel*	pChnl 	= NULL;
	TEXT        	lLogText[MAX_LOG_TEXT_LEN]      = "";
   TEXT           lPlayCtrlTEXT[MAX_LOG_TEXT_LEN] = "";
	
	lCktId  = req.circuitId;
	pChnl   = &mpSynwaySystem->channel[lCktId];

	snprintf(lLogText, MAX_LOG_TEXT_LEN, 
			"%s: RefNum:%08X CKTID:[%03d] InTPly:%d RptPly:%d FlCnt:%d Tc:%s NonBl:%d Rcvd PLAY", lType,
			req.seqNum, lCktId + 1, req.dtmfEnable, req.repeatPlay, req.vFileCount, req.dtmfStopPlayCharSet, req.nonBlocking);
   if(true == req.playCtrlEnable)
   {
      snprintf(lPlayCtrlTEXT, MAX_LOG_TEXT_LEN,
         " Pas:%c Res:%c Fwd:%c Rwd:%c", 
         req.playCtrl.pauseDtmf, req.playCtrl.resumeDtmf,
         req.playCtrl.forwardDtmf, req.playCtrl.rewindDtmf);
      strcat(lLogText, lPlayCtrlTEXT);
   }

   T (gTrace, printf("%s: %s\n", gProcessName, lLogText);
         for(int idx = 0; idx < req.vFileCount; idx++)
         printf(" Fl:%d->%s\n", idx+1, req.voiceFile[idx]););

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_CNTTN ST:CKT_UNAVAILABLE", lType, 
                  req.seqNum, lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, LOG_LEVEL_2, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
      case CKT_PLAYING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
                  "ST:CKT_PLAYING PLAY in-progress Hence issuing SsmStopPlayFile", lType,
						req.seqNum, lCktId + 1);
				TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI07, LOG_LEVEL_2, lLogText);
				
				//3rd argumnet to clear dtmf buffer if in current playing enabled
				if(true != StopPlaying(lCktId, req.seqNum, pChnl->currentPlay.dtmfEnable, lType))
					return MP_PLAY_STOP_CMD_ERR;

            ChnlLock(lCktId);
				if(pChnl->status != CKT_IDLE)
				{
					pChnl->playStatus  = PLAY_NEXT_IMMEDIATE;
					pChnl->nextPlay    = req;
					pChnl->status      = CKT_PLAY_STOPING;
               pChnl->mediaCompTime = time(NULL);
               ChnlUnlock(lCktId);

					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
							"ST:%s SsmStopPlayFile Success", lType, req.seqNum,
							lCktId + 1,
							SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(SAPI08, LOG_LEVEL_2, lLogText);
				}
				else
				{
               ChnlUnlock(lCktId);
					snprintf(lLogText, MAX_LOG_TEXT_LEN, 
							"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
							"ST:CKT_IDLE Changed from Playing Hence Playing", lType, req.seqNum,
							lCktId + 1);
					T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(SAPI09, LOG_LEVEL_2, lLogText);
					Play(req, lType);
				}
			}
			break;
		case CKT_PLAY_STOPING:
		case CKT_PLAY_STOPING_AND_GETDIG:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN, 
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:%s STOP in-progress", lType, req.seqNum,
						lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI10, LOG_LEVEL_2, lLogText);

            ChnlLock(lCktId);
				if(pChnl->status != CKT_IDLE)
				{
					pChnl->playStatus  	= PLAY_NEXT_REQD;
					pChnl->nextPlay     	= req;
               pChnl->mediaCompTime = time(NULL);
               ChnlUnlock(lCktId);
				}
				else
				{
               ChnlUnlock(lCktId);
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
							"ST:CKT_IDLE Changed from stoping Hence Playing", lType, req.seqNum,
							lCktId + 1);
					T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(SAPI09, LOG_LEVEL_2, lLogText);
					Play(req, lType);
				}
			}
			break;
		case CKT_GETDIG:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:CKT_GETDIG in-progress Hence issuing SsmPlayFile", lType, req.seqNum,
						lCktId + 1);
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI11, LOG_LEVEL_2, lLogText);

				if(true != StartPlay(req, lType))
					return MP_PLAY_CMD_ERR;

				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:%d SsmPlayFile Success", lType,req.seqNum,
						lCktId + 1, pChnl->status);
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI12, LOG_LEVEL_2, lLogText);

            ChnlLock(lCktId);
				if(pChnl->status != CKT_IDLE)
				{
					pChnl->currentPlay   = req;
					pChnl->playStatus    = PLAY_NEXT_NOT_REQD;
					pChnl->status 			= CKT_PLAYING_AND_GETDIG;
               pChnl->mediaCompTime = time(NULL);
					pChnl->cktId			= lCktId;
               ChnlUnlock(lCktId);
				}
				else
				{
               ChnlUnlock(lCktId);
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
							"ST:CKT_IDLE Changed From GetDigitHence Playing", lType, req.seqNum,
							lCktId + 1);
					T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(SAPI09, LOG_LEVEL_2, lLogText);
					Play(req, lType);
				}
			}
			break;
		case CKT_IDLE:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:CKT_IDLE So Issuing SsmPlayFile", lType, req.seqNum,
						lCktId + 1);
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI13, lLogText);

				if(true != StartPlay(req, lType))
					return MP_PLAY_CMD_ERR;

            ChnlLock(lCktId);
				pChnl->currentPlay   = req;
				pChnl->playStatus 	= PLAY_NEXT_NOT_REQD;
				pChnl->status 			= CKT_PLAYING;
				pChnl->cktId			= lCktId;
            pChnl->mediaCompTime = time(NULL);
            ChnlUnlock(lCktId);

				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:%d SsmPlayFile Success", lType, req.seqNum,
						lCktId + 1, pChnl->status);
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI12, LOG_LEVEL_2, lLogText);
			}
			break;
		case CKT_RECORDING:
		case CKT_PATCHED:
		case CKT_RECORDING_AND_GETDIG:
		case CKT_RECORD_STOPING:
		case CKT_RECORD_STOPING_AND_GETDIG:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:%s Hence Can not Start Play", lType, req.seqNum,
						lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI06, LOG_LEVEL_2, lLogText);
				return MP_CKT_BUSY_ERR;
			}
		case CKT_PLAYING_AND_GETDIG:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN, 
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:CKT_PLAYING_AND_GETDIG PLAY "
						"and GETDIG in-progress Hence issuing SsmStopPlayFile", lType, req.seqNum,
						lCktId + 1);
				TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI06, LOG_LEVEL_2, lLogText);
				
				//3rd argumnet to clear dtmf buffer if in current playing enabled
				if(true != StopPlaying(lCktId, req.seqNum, pChnl->currentPlay.dtmfEnable, lType))
					return MP_PLAY_STOP_CMD_ERR;

            ChnlLock(lCktId);
				if(pChnl->status != CKT_IDLE && pChnl->status != CKT_GETDIG)
				{
					pChnl->playStatus  = PLAY_NEXT_IMMEDIATE;
					pChnl->nextPlay    = req;
					pChnl->status      = CKT_PLAY_STOPING_AND_GETDIG;
               pChnl->mediaCompTime = time(NULL);
               ChnlUnlock(lCktId);

					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
							"ST:%s SsmStopPlayFile Success", lType, req.seqNum,
							lCktId + 1,
							SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(SAPI08, lLogText);
				}
				else
				{
               ChnlUnlock(lCktId);
					snprintf(lLogText, MAX_LOG_TEXT_LEN, 
							"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
							"ST:%s Changed from Playing Hence Playing", lType, req.seqNum,
							lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
					T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(SAPI09, LOG_LEVEL_2, lLogText);
					Play(req, lType);
				}
			}
		default:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"ST:%d Invalid Channel Status", lType, req.seqNum,
						lCktId + 1, pChnl->status);
				TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI14, lLogText);
				return MP_BADPARM_ERR;
			}
	}

	return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : StopPlaying
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::StopPlaying(UINT16 lChnl, unsigned int lseq, BOOLEAN lClrDtmfBuf, TEXT *lType)
{
	TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
	TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";

	//if(-1 == SsmStopPlayFile(lChnl))
	if(-1 == SsmStopPlay(lChnl))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"%s: RefNum:%08X CKTID:[%03d] "
				"SsmStopPlayFile Failed EC:%d EM:%s", lType, lseq,
				lChnl + 1, SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI15, lLogText);
		return false;;
	}

	if(true == lClrDtmfBuf)
	{
/*		if(SsmEnableRxDtmf(lChnl, false))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] "
					"SsmEnableRxDtmf Failed in Stopping EC:%d EM:%s", lType, lseq,
					lChnl + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI26, lLogText);
			return false;
		}

		if(SsmClearRxDtmfBuf(lChnl))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] "
					"SsmClearRxDtmfBuf Failed in Stopping EC:%d EM:%s", lType, lseq,
					lChnl + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI17, lLogText);
			return false;
		}
*/	}

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : StartPlay
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::StartPlay(MpPlay playTone, TEXT *lType)
{
	TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
	TEXT           lLogText[MAX_LOG_TEXT_LEN] 		= "";
	TEXT				lpstrDtmfCharSet[25] 				= "";

	if(playTone.vFileCount <= 0)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
				"SsmPlayFile Failed FileCount Error", lType, playTone.seqNum,
				playTone.circuitId + 1);
		gLog.GenerateLog(SAPI18, lLogText);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	if(true == playTone.dtmfEnable)
	{
		if(SsmEnableRxDtmf(playTone.circuitId, true))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmEnableRxDtmf Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI16, lLogText);
			return false;
		}

		if(SsmClearRxDtmfBuf(playTone.circuitId))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmClearRxDtmfBuf Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI17, lLogText);
			return false;
		}

		if(0 == strlen(playTone.dtmfStopPlayCharSet))
			strcpy(lpstrDtmfCharSet, "0123456789*#abcd");
		else
			strcpy(lpstrDtmfCharSet,playTone.dtmfStopPlayCharSet);

		if( 1 != SsmSetDTMFStopPlayCharSet(playTone.circuitId, lpstrDtmfCharSet))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmSetDTMFStopPlayCharSet Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI23, lLogText);
			return false;
		}

		if(SsmSetDtmfStopPlay(playTone.circuitId, true))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmSetDtmfStopPlay Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI24, lLogText);
			return false;
		}
	}
	else
	{
		if(SsmEnableRxDtmf(playTone.circuitId, true))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmEnableRxDtmf Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI16, lLogText);
			return false;
		}


		if(SsmSetDtmfStopPlay(playTone.circuitId, false))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmSetDtmfStopPlay Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI25, lLogText);
			return false;
		}
	}

	if(1 == playTone.vFileCount)
	{
		//if(SsmPlayFile(playTone.circuitId, const_cast<char*>(playTone.voiceFile[0]), 
		//			mPlayFileFormat_Cfg, 0, 0xffffffff))
		if(SsmPlayFile(playTone.circuitId, playTone.voiceFile[0], 
					mPlayFileFormat_Cfg, 0, 0xffffffff))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmPlayFile Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			gLog.GenerateLog(SAPI19, lLogText);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			return false;
		}
	}
	else
	{
		if(SsmClearFileList(playTone.circuitId))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmClearFileList Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			gLog.GenerateLog(SAPI20, lLogText);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			return false;
		}

		for(INT16 lCnt = 1; lCnt <= playTone.vFileCount; lCnt++)
		{
			//if(SsmAddToFileList(playTone.circuitId, const_cast<char*>(playTone.voiceFile[lCnt-1]), mPlayFileFormat_Cfg, 0, 0xffffffff))
			if(SsmAddToFileList(playTone.circuitId, playTone.voiceFile[lCnt-1], mPlayFileFormat_Cfg, 0, 0xffffffff))
			{
				SsmGetLastErrMsg(lErrMsg);
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
						"SsmAddToFileList Failed Num:%d Nam:%s EC:%d EM:%s", lType, playTone.seqNum,
						playTone.circuitId + 1, lCnt, playTone.voiceFile[lCnt-1], 
						SsmGetLastErrCode(), lErrMsg);
				gLog.GenerateLog(SAPI21, lLogText);
				TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				return false;
			}
		}

		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
				"Playing File List", lType, playTone.seqNum,
				playTone.circuitId + 1);
		T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

		if(SsmPlayFileList(playTone.circuitId))
		{
			SsmGetLastErrMsg(lErrMsg);
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"%s: RefNum:%08X CKTID:[%03d] MC_CNTTN "
					"SsmPlayFileList Failed EC:%d EM:%s", lType, playTone.seqNum,
					playTone.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
			gLog.GenerateLog(SAPI22, lLogText);
			TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			return false;
		}
	}

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : Stop
// DESCRIPTION : stops all media processing functions on the specified channel
// PARAMETER   : request and response structure
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Stop(const MpStop req, TEXT *lType)
{
	INT16			   lCktId   = -1;
	synwayChannel*	pChnl 	= NULL;
	TEXT        	lLogText[MAX_LOG_TEXT_LEN] = "";

	lCktId  = req.circuitId;
	pChnl   = &mpSynwaySystem->channel[lCktId];

	snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: RefNum:%08X CKTID:[%03d] Rcvd STOP", lType, req.seqNum,
			lCktId + 1);
	T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

	switch(pChnl->status)
	{
		case CKT_UNAVAILABLE:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN, 
						"%s: RefNum:%08X CKTID:[%03d] MC_DISTN ST:CKT_UNAVAILABLE", lType, req.seqNum,
						lCktId + 1);
				TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI06, lLogText);
				return MP_CKT_UNAVAILABLE_ERR;
			}
		case CKT_PLAYING:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN, 
						"%s: RefNum:%08X CKTID:[%03d] MC_DISTN "
						"ST:CKT_PLAYING Hence issuing SsmStopPlayFile", lType,
						req.seqNum, lCktId + 1);
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI07, LOG_LEVEL_2, lLogText);

				//3rd argumnet to clear dtmf buffer if in current playing enabled
				if(true != StopPlaying(lCktId, req.seqNum, pChnl->currentPlay.dtmfEnable, lType))
					return MP_PLAY_STOP_CMD_ERR;

            ChnlLock(lCktId);
				if(pChnl->status != CKT_IDLE)
				{
					pChnl->status 		  = CKT_PLAY_STOPING;
               pChnl->mediaCompTime = time(NULL);
					pChnl->playStatus	  = PLAY_NEXT_NOT_REQD;
				}
            ChnlUnlock(lCktId);

				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] "
						"MC_DISTN SsmStopPlayFile Success", lType,
						req.seqNum, lCktId + 1);
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText);); 
				gLog.GenerateLog(SAPI08, lLogText);
			}
			break;
		case CKT_PLAYING_AND_GETDIG:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN, 
						"%s: RefNum:%08X CKTID:[%03d] MC_DISTN "
						"ST:CKT_PLAYING_AND_GETDIG Hence issuing SsmStopPlayFile", lType,
						req.seqNum, lCktId + 1);
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI27, LOG_LEVEL_2, lLogText);

				//3rd argumnet to clear dtmf buffer if in current playing enabled
				if(true != StopPlaying(lCktId, req.seqNum, pChnl->currentPlay.dtmfEnable, lType))
					return MP_PLAY_STOP_CMD_ERR;

            ChnlLock(lCktId);
				if(pChnl->status != CKT_IDLE && pChnl->status != CKT_GETDIG)
				{
					pChnl->status 		  = CKT_PLAY_STOPING_AND_GETDIG;
               pChnl->mediaCompTime = time(NULL);
					pChnl->playStatus	  = PLAY_NEXT_NOT_REQD;
				}
            ChnlUnlock(lCktId);

				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] "
						"MC_DISTN ST:%s SsmStopPlayFile Success", lType,
						req.seqNum, lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText);); 
				gLog.GenerateLog(SAPI08, lLogText);
			}
			break;
		case CKT_PLAY_STOPING:
		case CKT_PLAY_STOPING_AND_GETDIG:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] "
						"MC_DISTN ST:%s Stop in progress", lType, req.seqNum,
						lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI10, lLogText);
			}
			break;
		case CKT_RECORDING_AND_GETDIG:
		case CKT_RECORD_STOPING:
		case CKT_RECORD_STOPING_AND_GETDIG:
		case CKT_GETDIG:
		case CKT_IDLE:
		case CKT_PATCHED:
		case CKT_RECORDING:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] "
						"MC_DISTN ST:%s Not Playing To Stop", lType, req.seqNum,
						lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI06, lLogText);
			}
			break;	
		default:
			{
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"%s: RefNum:%08X CKTID:[%03d] "
						"MC_DISTN ST:%d Invalid Channel Status", lType, req.seqNum,
						lCktId + 1, pChnl->status);
				TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI14, lLogText);
				return MP_BADPARM_ERR;
			}
	}
	return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : SetEvents
// DESCRIPTION : Set the Events required to process
// PARAMETER   : 
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::SetEvents()
{
	TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
	TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

	static 			EVENT_SET_INFO EventTmp;

	EventTmp.dwUser 				= 0;
	EventTmp.dwWorkMode 			= EVENT_POLLING;
	EventTmp.lpHandlerParam 	= 0;
	EventTmp.dwOutCondition 	= 0;
	EventTmp.dwOutParamVal 		= 0;

	//Init Event System
	if(-1 == SsmSetEvent(0xffff/*all event*/, -1/*all ch set event*/, true, &EventTmp))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmSetEvent All Failed EC:%d EM:%s",
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}
	
	if(-1 == SsmSetEvent(E_CHG_ChState, -1, true, &EventTmp))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmSetEvent E_CHG_ChState Failed EC:%d EM:%s",
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	if(-1 == SsmSetEvent(E_CHG_PcmLinkStatus, -1, true, &EventTmp))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmSetEvent E_CHG_PcmLinkStatus Failed EC:%d EM:%s", 
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	if(-1 == SsmSetEvent(E_PROC_WaitDTMF, -1, true, &EventTmp))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmSetEvent E_PROC_WaitDTMF Failed EC:%d EM:%s", 
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	if(-1 == SsmSetEvent(E_CHG_RcvDTMF, -1, true, &EventTmp))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmSetEvent E_CHG_RcvDTMF Failed EC:%d EM:%s", 
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	if(-1 == SsmSetEvent(E_PROC_RecordEnd, -1, true, &EventTmp))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmSetEvent E_PROC_RecordEnd Failed EC:%d EM:%s", 
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
   }

   if(-1 == SsmSetEvent(E_CHG_RemotePCMBlock, -1, true, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_CHG_RemotePCMBlock Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   if(-1 == SsmSetEvent(E_CHG_RemoteChBlock, -1, true, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_CHG_RemoteChBlock Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   if(-1 == SsmSetEvent(E_SYS_NoSound, -1, false, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_SYS_NoSound to false Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   if(-1 == SsmSetEvent(E_PROC_PlayFileList, -1, true, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_PROC_PlayFileList to false Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   if(-1 == SsmSetEvent(E_CHG_ISDNStatus, -1, true, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_CHG_ISDNStatus to false Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   if(-1 == SsmSetEvent(E_CHG_Mtp2Status, -1, true, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_CHG_Mtp2Status to false Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   if(-1 == SsmSetEvent(E_CHG_Mtp3State, -1, true, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_CHG_Mtp3State to false Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   EventTmp.dwWorkMode 		= EVENT_POLLING;
   EventTmp.lpHandlerParam = 0;
   EventTmp.dwOutCondition = (mFilePlayPrfChkIntr * 1000); // 20 * 1000 //90000;
   EventTmp.dwOutParamVal	= 1;

   if(-1 == SsmSetEvent(E_PROC_PlayFile/*event*/
            , -1/*ch*/
            , true, &EventTmp))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "SsmSetEvent E_PROC_PlayFile Failed EC:%d EM:%s",
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
	}

	EventTmp.dwWorkMode = EVENT_POLLING;
	EventTmp.lpHandlerParam = 0;
	EventTmp.dwOutCondition = (mFileRecordPrfChkIntr * 1000); // 20 * 1000 //50000;
	EventTmp.dwOutParamVal = 1;

	if(-1 == SsmSetEvent(E_PROC_RecordFile/*event*/
				, -1/*ch*/
				, true, &EventTmp))
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"SsmSetEvent E_PROC_RecordFile Failed EC:%d EM:%s",
				SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : WaitForEvt
// DESCRIPTION : waits for event to happen in channel
// PARAMETER   : response structures
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::WaitForEvt(MESSAGE_INFO &lEvent)
{
	INT16				lRtVal 	= -1;
	TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
	TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

	snprintf(lLogText, MAX_LOG_TEXT_LEN, "TH:EVT Wait For Event To occur");
	T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
	memset(&lEvent, 0, sizeof(MESSAGE_INFO));

	lRtVal = SsmWaitForEvent (mTimeOutEvt_Cfg, &lEvent);
	if (lRtVal == -2)            //wait for some event
	{
		SsmGetLastErrMsg(lErrMsg);
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"TH:EVT SsmWaitForEvent Return val:%d Failed EC:%d EM:%s",
				lRtVal, SsmGetLastErrCode(), lErrMsg);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI28, LOG_LEVEL_2, lLogText);
		return false;
	}
	else if(lRtVal == -1)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"TH:EVT SsmWaitForEvent Return val:%d TimeOut Occured",lRtVal);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI29, LOG_LEVEL_2,lLogText);
		return false;
	}

#if 0
	snprintf(lLogText, MAX_LOG_TEXT_LEN, 
			"TH:EVT EVENT GENERATED: EC:0x%04X nReference:%d dwParam:%u", 
			lEvent.wEventCode , lEvent.nReference, lEvent.dwParam);
	T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
#endif

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : HandleMediaEvent
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::HandleMediaEvent(MpResponseDetails &rsp, MESSAGE_INFO &lEvent)
{
	INT16				lRtValue = -1;
	TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

	lRtValue = HandleEvent(rsp, lEvent);
	if(-1 == lRtValue)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"TH:EV:M Handle Event Failed CKTID:[%03d] EC:0x%04X dwParam:%u",
				lEvent.nReference + 1, lEvent.wEventCode , lEvent.dwParam);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI30, lLogText);
		return false;
	}
	else if(0 == lRtValue)
		return false;

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : HandleSigEvent
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::HandleSigEvent(GenIsupMsg &lGenIsupMsg, MESSAGE_INFO &lEvent)
{
	INT16          lRtValue = -1;
	TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";

	lRtValue = SignalEvents(lGenIsupMsg, lEvent);
	if(-1 == lRtValue)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"TH:EV:S Handle sig Event Failed CKTID:[%03d] EC:0x%04X dwParam:%u",
				lEvent.nReference + 1, lEvent.wEventCode , lEvent.dwParam);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI56, lLogText);
		return false;
	}
	else if(0 == lRtValue)
		return false;

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : HandleChnlMgmtEvent
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::HandleChnlMgmtEvent(CktCmdRsp &lCktCmdRsp, MESSAGE_INFO &lEvent)
{
	INT16          lRtValue = -1;
	TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";

	lRtValue = ChnlMgmtEvents(lCktCmdRsp, lEvent);
	if(-1 == lRtValue)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"TH:EV:C Handle Chnl  Event Failed CKTID:[%03d] EC:0x%04X dwParam:%u",
				lEvent.nReference + 1, lEvent.wEventCode , lEvent.dwParam);
		TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(SAPI56, lLogText);
		return false;
	}
	else if(0 == lRtValue)
		return false;

	return true;
}

//-------------------------------------------------------------
// FUNCTION    : SignalEvents
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::SignalEvents(GenIsupMsg &lGenIsupMsg, MESSAGE_INFO &lEvent)
{
	unsigned int 	      lTemp_1;
	unsigned int 	      lTemp_2;
   int                  lTemp_3;
	INT16				      lPenRes;
   EnumSigChannelStatus lSigChnlSt  = SNW_CHNL_NOT_AVAILABLE;
   EnumSnwCallDirection lSnwCallDir = SNW_NO_CALL;
	TEXT                 lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
	TEXT        	      lLogText[MAX_LOG_TEXT_LEN] 		= "";


	switch(lEvent.wEventCode)
	{
      case 0x0001:/*E_CHG_ISDNStatus*/
         {
            int pL3Start    = 0;
            int pL2DStatus  = 0;
            int pL2D_L3Atom = 0;
            int pL3_L2DAtom = 0;
            int pRef_ind    = 0;

            if(-1 == SsmISDNGetStatus(lEvent.nReference, &pL3Start, &pL2DStatus, &pL2D_L3Atom,
                     &pL3_L2DAtom, &pRef_ind))
            {
               memset(lErrMsg, 0, sizeof(lErrMsg));
               SsmGetLastErrMsg(lErrMsg);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:EV:S Event Signaling Get ISDNStatus Change Failed Trunk:%d",
                     lEvent.nReference);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:EV:S Event Signaling ISDNStatus Change Trunk:%d "
                     "L3Start:%d L2DStatus:%d L2D_L3Atom:%d L3_L2DAtom:%d Ref_ind:%d",
                     lEvent.nReference, pL3Start, pL2DStatus, pL2D_L3Atom, pL3_L2DAtom, pRef_ind);
            }
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI31, lLogText);
         }
         return 0;
      case 0x0003:/*E_CHG_Mtp3State*/
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:S Event Signaling Mtp3 Status change DpcNum:%d Status:%s",
               lEvent.nReference, (1 == lEvent.dwParam) ? "Support" : "UnSupport");
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return 0;
      case 0x0040:/*E_CHG_Mtp2Status*/
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:S Event Signaling Link Status Change Link:%d Description:%s",
               lEvent.nReference, SynwayUtil::ConvertSnwSigLinkStToStr(lEvent.dwParam));
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         GetSignalingLinkStatus();         
         return 0;
		case 0x0007:/*E_CHG_PcmLinkStatus*/
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"TH:EV:S Event status of the digital trunk %d changed Description:%u",
					lEvent.nReference, lEvent.dwParam);
			T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(SAPI31, lLogText);
			return 0;//break;
		case 0x0018:/*E_CHG_ChState*/
			{
				lTemp_1 = lEvent.dwParam & 0x00FF;
				lTemp_2 = lEvent.dwParam >> 16 ;

            lSigChnlSt  = mpSynwaySystem->sigChannel[lEvent.nReference].status;
            lSnwCallDir = mpSynwaySystem->sigChannel[lEvent.nReference].callDirection;

				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"TH:EV:S Event ChnlStChng CKTID:[%03d] Type:%d ProcCS:%d(%s-%s) PrvSt:%d(%s) CurSt:%d(%s)", 
						lEvent.nReference + 1, SsmGetChType(lEvent.nReference), 
                  lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt),
                  SynwayUtil::ConvertProcCallDirToStr(lSnwCallDir),
                  lTemp_2, SynwayUtil::ConvertChnlStToStr(lTemp_2), 
                  lTemp_1, SynwayUtil::ConvertChnlStToStr(lTemp_1));
				T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(SAPI31, lLogText);

				switch(lTemp_1)
				{  
               case 0:/*S_CALL_STANDBY*/
                  {
                     WORD lChRelRes = SsmGetReleaseReason(lEvent.nReference);

                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].cic = lEvent.nReference;
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_CHNL_IDLE;
                     mpSynwaySystem->sigChannel[lEvent.nReference].callDirection = SNW_NO_CALL;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);

                     switch(lSigChnlSt)
                     {
                        case SNW_IAM_SENT:
                        case SNW_WAIT_DAIL_ANS:
                        case SNW_DIALING:
                        case SNW_WAIT_REMOTE_PICKUP:
                        case SNW_ANM_RCVD:
                           {
                              if(SNW_OUT_BOUND_CALL == lSnwCallDir)
                              {
                                 FillRelAndClearScheduled(lEvent.nReference, lGenIsupMsg, lChRelRes);
                                 ChnlUnlock(lEvent.nReference);
                                 return 1;
                              }
                           }
                           break;
                        case SNW_TALKING:
                           {
                              if(SNW_NO_CALL != lSnwCallDir)
                              {
                                 FillRelAndClearScheduled(lEvent.nReference, lGenIsupMsg, lChRelRes);
                                 ChnlUnlock(lEvent.nReference);
                                 return 1;
                              }
                           }
                           break;
                        case SNW_IAM_RCVD:
                        case SNW_WAIT_ACM_SND:
                        case SNW_RINGING:
                           {
                              if(SNW_IN_BOUND_CALL == lSnwCallDir)
                              {
                                 FillRelAndClearScheduled(lEvent.nReference, lGenIsupMsg, lChRelRes);
                                 ChnlUnlock(lEvent.nReference);
                                 return 1;
                              }
                           }
                           break;
                        case SNW_CHNL_NOT_AVAILABLE:
                        case SNW_CHNL_IDLE:
                        case SNW_WAIT_RLC:
                        case SNW_REMOTE_HANGUP:
                        case SNW_WAIT_REL_SND:
                        default:
                           {
                           }
                     }

                     gScheduledAnm[lEvent.nReference].isScheduled = false;
                     memset(&gScheduledAnm[lEvent.nReference].schAnm, 0 ,sizeof(GenIsupAnm));

                     //Send Scheduled IAM if Present
                     if(true == gScheduledIam[lEvent.nReference].isScheduled)
                     {
                        ChnlUnlock(lEvent.nReference);

                        memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
                        lGenIsupMsg.isupMsgType = GEN_ISUP_IAM;
                        lGenIsupMsg.circuitId   = lEvent.nReference;
                        lGenIsupMsg.isupIam     = gScheduledIam[lEvent.nReference].schIam;

                        gScheduledIam[lGenIsupMsg.circuitId].isScheduled = false;
                        memset(&gScheduledIam[lGenIsupMsg.circuitId].schIam, 0 ,sizeof(GenIsupIam));

                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "TH:EV:S CKTID:[%03d] ChnlSt:S_CALL_STANDBY Atempting Scheduled IAM OA:%s DA:%s", 
                              lEvent.nReference + 1, lGenIsupMsg.isupIam.callingNumber, lGenIsupMsg.isupIam.calledNumber);
                        T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(SAPI95, lLogText);

                        if(SNW_RET_ERR_FRW_APP == DialNum(lGenIsupMsg))
                           return 1;
                     }
                     else
                        ChnlUnlock(lEvent.nReference);
                  }
                  break;
               case 2:/*S_CALL_RINGING*/
                  {
                     int nANX;

                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_RINGING;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);

                     if(true == gScheduledAnm[lEvent.nReference].isScheduled)
                     {
                        ChnlUnlock(lEvent.nReference);

                        if(TOA_NO_CHARGE == gScheduledAnm[lEvent.nReference].schAnm.typeOfAns)
                           nANX = 2;
                        else if(TOA_CHARGE == gScheduledAnm[lEvent.nReference].schAnm.typeOfAns)
                           nANX = 1;
                        else
                           nANX = 0;

                        //Clear Scheduled ANM
                        gScheduledAnm[lEvent.nReference].isScheduled = false;
                        memset(&gScheduledAnm[lEvent.nReference].schAnm, 0 ,sizeof(GenIsupAnm));

                        if(-1 == SsmPickupANX(lEvent.nReference, nANX))
                        {
                           lErrMsg[0] = '\0';
                           SsmGetLastErrMsg(lErrMsg);
                           snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                 "TH:EV:S CKTID:[%03d] ChnlSt  S_CALL_RINGING Sch ANM Snd Fail EC:%d EM:%s",
                                 lEvent.nReference + 1, SsmGetLastErrCode(), lErrMsg);
                           TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                           gLog.GenerateLog(SAPI61, lLogText);
                           return 0;
                        }


                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "TH:EV:S CKTID:[%03d] Channel State S_CALL_RINGING Scheduled ANM Send Success", 
                              lEvent.nReference + 1);
                        T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(SAPI62, lLogText);
                     }
                     else
                        ChnlUnlock(lEvent.nReference);
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:S CKTID:[%03d] Channel State S_CALL_RINGING, but Not Scheduled ANM",
                           lEvent.nReference + 1);
                     TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     //gLog.GenerateLog(
                  }
                  break;
               case 3:/*S_CALL_TALKING*/
                  {
                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_TALKING;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                     ChnlUnlock(lEvent.nReference);
                  }
                  break;
               case 7:/*S_CALL_PENDING*/
                  {
                     lPenRes = SsmGetPendingReason(lEvent.nReference);
                     lSigChnlSt = mpSynwaySystem->sigChannel[lEvent.nReference].status;

                     if(-1 == lPenRes)
                     {
                        lTemp_3 = SsmGetChState(lEvent.nReference);
                        SsmGetLastErrMsg(lErrMsg);
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "TH:EV:S CKTID:[%03d] CS:S_CALL_PENDING Get PenReason Failed CS:%d(%s) EC:%d EM:%s",
                              lEvent.nReference + 1, lTemp_3, SynwayUtil::ConvertChnlStToStr(lTemp_3),
                              SsmGetLastErrCode(), lErrMsg);
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(SAPI31, lLogText);
                        return -1;
                     }
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:S CKTID:[%03d] ChnlSt:S_CALL_PENDING ProcCS:%d Res:%d(%s)",
                           lEvent.nReference + 1, lSigChnlSt, lPenRes, 
                           SynwayUtil::ConvertCallPendingResToStr(lPenRes));
                     TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI31, lLogText);

                     switch(lPenRes)
                     {
                        case 13:/*PEND_AutoDialFailed*/
                           {
                              int lAutoDialFailRes = SsmGetAutoDialFailureReason(lEvent.nReference);
                              int lRelRes          = SsmGetReleaseReason(lEvent.nReference);;

                              lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
                              lGenIsupMsg.circuitId = lEvent.nReference;

                              if(lAutoDialFailRes > 0)
                              {
                                 switch(lAutoDialFailRes)
                                 {
                                    case 0:/*ATDL_NULL*/
                                       lGenIsupMsg.isupRel.cause = OUTGOING_CALL_BARRED;
                                       break;;
                                    case 10:/*ATDL_Mtp3Unusable*/
                                       lGenIsupMsg.isupRel.cause = NETWORK_OUTOF_ORDER;
                                       break;
                                    case 65:/* ATDL_IllegalMessage*/
                                       lGenIsupMsg.isupRel.cause = NVALID_MSG;
                                    case 66:/*ATDL_RcvREL*/
                                       lGenIsupMsg.isupRel.cause = lRelRes;
                                       break;
                                    default:
                                       if(0 >=  lRelRes)
                                          lGenIsupMsg.isupRel.cause = TEMP_FAILURE;
                                       else
                                          lGenIsupMsg.isupRel.cause = lRelRes;
                                 }
                              }
                              else
                                 lGenIsupMsg.isupRel.cause = TEMP_FAILURE;

                              snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                    "TH:EV:S Event Rcv AutoDialFailed Snd IAM/SETUP in CKTID:[%03d] Reason:%d RELRes:%d", 
                                    lEvent.nReference + 1, lAutoDialFailRes, lRelRes);
                              TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                              gLog.GenerateLog(SAPI31, lLogText);

                              ChnlLock(lEvent.nReference);
                              mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_WAIT_REL_SND;
                              mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                              ChnlUnlock(lEvent.nReference);

                              switch(lSigChnlSt)
                              {
                                 case SNW_IAM_SENT:
                                 case SNW_WAIT_DAIL_ANS:
                                 case SNW_DIALING:
                                 case SNW_WAIT_REMOTE_PICKUP:
                                 case SNW_ANM_RCVD:
                                 case SNW_TALKING:
                                    return 1; //Return 1, As for Above State Send REL to App
                                 default:
                                    return 0; //Retun 0, Rest Dont Send REL To App
                              }
                           }
                           break;
                        case 12:/*PEND_RemoteHangupOnTalking*/
                        case 20:/*PEND_CalleeHangupOnTalking*/
                        case 41:/*ISDN_CALLOVER*/
                        case 43:/*ISDN_HANGING*/
                        case 48:/*PEND_CalleeHangupOnWaitRemotePickUp*/
                        case 50:/*ISUP_RemoteSuspend*/
                        case 54:/*PEND_RemoteHangupOnRinging*/
                        case 63:/*PEND_RemoteHangupOnSuspend*/
                        case 64:/*PEND_CalleeHangupOnSuspend*/
                        case 53:/*ISDN_CALL_REJ*/
                        case 42:/*ISDN_WAIT_RELEASE*/
                        case 44:/*ISDN_RELEASING*/
                        case 45:/*ISDN_UNALLOCATED_NUMBER*/
                        case 46:/*ISDN_NETWORK_BUSY*/
                        case 47:/*ISDN_CIRCUIT_NOT_AVAILABLE*/
                        case 52:/*ISDN_NO_ANSWER*/
                        case 55:/*ISDN_NO_ROUTE*/
                        case 56:/*ISDN_NO_ROUTE_TO_DEST*/
                        case 65:/*ISDN_NORMAL_UNSPEC*/
                        case 66:/*ISDN_USER_ABSENCE*/
                        case 67:/*ISDN_INVALID_NUMBER_FOEMAT*/
                        case 68:/*ISDN_NO_CIRCUIT_AVAILABLE*/
                           {
                              ChnlLock(lEvent.nReference);
                              mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_REMOTE_HANGUP;
                              mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                              ChnlUnlock(lEvent.nReference);

                              WORD lRedirRes;
                              lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
                              lGenIsupMsg.circuitId = lEvent.nReference;
                              //lGenIsupMsg.isupRel
                              lRedirRes = SsmGetReleaseReason(lEvent.nReference);
                              lGenIsupMsg.isupRel.cause =  lRedirRes;
                              snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                    "TH:EV:S Event Rcv REL in CKTID:[%03d] Reason:%d", 
                                    lEvent.nReference + 1, lRedirRes);
                              T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                              gLog.GenerateLog(SAPI31, lLogText);

                              /*         
                              //Here REL is forwared to IVR, RLC will be sent from the from IVR 

                              if(-1 == SsmHangup(lEvent.nReference))
                              {
                              lErrMsg[0] = '\0';
                              SsmGetLastErrMsg(lErrMsg);
                              snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "TH:EV:S CKTID:[%03d] SsmHangup Failed EC:%d EM:%s",
                              lEvent.nReference + 1, SsmGetLastErrCode(), lErrMsg);
                              TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                              gLog.GenerateLog(SAPI57, lLogText);
                              }
                              snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "TH:EV:S CKTID:[%03d] SsmHangup snd:RLC Success",
                              lEvent.nReference + 1);
                              T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                              gLog.GenerateLog(SAPI58, lLogText);

                              //Clear if any Scheduled ANM in present in that channel, before sending REl to app
                              if(true == gScheduledAnm[lEvent.nReference].isScheduled)
                              {
                              gScheduledAnm[lEvent.nReference].isScheduled = false;
                              memset(&gScheduledAnm[lEvent.nReference].schAnm, 0 ,sizeof(GenIsupAnm));
                              }
                               */                              
                              return 1;//break;
                           }
                           break;
                        case 10:/*PEND_WaitBckStpMsg*/
                           {
                              int nANX;
                              int lChTyp = SsmGetChType(lEvent.nReference);
                              if(7 == lChTyp || 8 == lChTyp) // 7 - user end ISDN 8 - Network side ISDN
                              {
                                 ChnlLock(lEvent.nReference);
                                 mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_IAM_RCVD;
                                 mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                                 ChnlUnlock(lEvent.nReference);

                                 lGenIsupMsg.isupMsgType = GEN_ISUP_IAM;
                                 lGenIsupMsg.circuitId = lEvent.nReference;
                                 strcpy(lGenIsupMsg.isupIam.callingNumber, SsmGetCallerIdA(lEvent.nReference));
                                 strcpy(lGenIsupMsg.isupIam.calledNumber, SsmGetPhoNumStrA(lEvent.nReference));
                                 lGenIsupMsg.isupIam.callingNumberFormat = GEN_INTERNATIONAL_FMT;
                                 lGenIsupMsg.isupIam.calledNumberFormat = GEN_INTERNATIONAL_FMT;
                                 lGenIsupMsg.isupIam.redirectReason =  GEN_RI_UNKNOWN;
                                 snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                       "TH:EV:S CKTID:[%03d] Rcv SETUP in ClgNum:%s",
                                       lEvent.nReference + 1, lGenIsupMsg.isupIam.callingNumber);
                                 T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                                 gLog.GenerateLog(SAPI85, lLogText);
                                 return 1;
                              }
                              else if(true == gScheduledAnm[lEvent.nReference].isScheduled)
                              {
                                 if(TOA_NO_CHARGE == gScheduledAnm[lEvent.nReference].schAnm.typeOfAns)
                                    nANX = 2;
                                 else if(TOA_CHARGE == gScheduledAnm[lEvent.nReference].schAnm.typeOfAns)
                                    nANX = 1;
                                 else
                                    nANX = 0;

                                 //Clear Scheduled ANM
                                 gScheduledAnm[lEvent.nReference].isScheduled = false;
                                 memset(&gScheduledAnm[lEvent.nReference].schAnm, 0 ,sizeof(GenIsupAnm));

                                 if(-1 == SsmPickupANX(lEvent.nReference, nANX))
                                 {
                                    lErrMsg[0] = '\0';
                                    SsmGetLastErrMsg(lErrMsg);
                                    snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                          "TH:EV:S CKTID:[%03d] CS:S_CALL_PENDING(PEND_WaitBckStpMsg) Sch ANM Snd Fail EC:%d EM:%s",
                                          lEvent.nReference + 1, SsmGetLastErrCode(), lErrMsg);
                                    TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                                    gLog.GenerateLog(SAPI61, lLogText);

                                    lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
                                    lGenIsupMsg.circuitId = lEvent.nReference;
                                    return 1; //Send REL Application as Send ANM Failed
                                 }

                                 snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                       "TH:EV:S CKTID:[%03d] CS:S_CALL_PENDING(PEND_WaitBckStpMsg)  Scheduled ANM Send Success", 
                                       lEvent.nReference + 1);
                                 T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                                 gLog.GenerateLog(SAPI62, lLogText);
                              }
                              else
                              {
                                 ChnlLock(lEvent.nReference);
                                 mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_WAIT_ACM_SND;
                                 mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                                 ChnlUnlock(lEvent.nReference);
                              }
                           }
                           break;
                        default:
                           {
                           }
                     }
                  }
                  break;
               case 11:/*S_CALL_UNAVAILABLE*/
               case 19:/*S_CALL_RemoteBlock*/
               case 20:/*S_CALL_LocalBlock*/
               case 123:/*S_ISUP_LocallyBlocked*/
               case 124:/*S_ISUP_RemotelyBlocked*/
               case 129:/*S_ISUP_Suspend*/
                  {
                     WORD lChRelRes = SsmGetReleaseReason(lEvent.nReference);

                     lSigChnlSt = mpSynwaySystem->sigChannel[lEvent.nReference].status;

                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_CHNL_NOT_AVAILABLE;
                     mpSynwaySystem->sigChannel[lEvent.nReference].callDirection = SNW_NO_CALL;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);

                     switch(lSigChnlSt)
                     {
                        case SNW_IAM_SENT:
                        case SNW_WAIT_DAIL_ANS:
                        case SNW_DIALING:
                        case SNW_WAIT_REMOTE_PICKUP:
                        case SNW_ANM_RCVD:
                           {
                              if(SNW_OUT_BOUND_CALL == lSnwCallDir)
                              {
                                 FillRelAndClearScheduled(lEvent.nReference, lGenIsupMsg, lChRelRes);
                                 ChnlUnlock(lEvent.nReference);
                                 return 1;
                              }
                              return 1;
                           }
                           break;
                        case SNW_TALKING:
                           {
                              if(SNW_NO_CALL != lSnwCallDir)
                              {
                                 FillRelAndClearScheduled(lEvent.nReference, lGenIsupMsg, lChRelRes);
                                 ChnlUnlock(lEvent.nReference);
                                 return 1;
                              }
                           }
                           break;
                        case SNW_IAM_RCVD:
                        case SNW_WAIT_ACM_SND:
                        case SNW_RINGING:
                           {
                              if(SNW_IN_BOUND_CALL == lSnwCallDir)
                              {
                                 FillRelAndClearScheduled(lEvent.nReference, lGenIsupMsg, lChRelRes);
                                 ChnlUnlock(lEvent.nReference);
                                 return 1;
                              }
                           }
                           break;
                        case SNW_CHNL_NOT_AVAILABLE:
                        case SNW_CHNL_IDLE:
                        case SNW_WAIT_RLC:
                        case SNW_REMOTE_HANGUP:
                        case SNW_WAIT_REL_SND:
                        default:
                           {
                           }
                     }

                     if(true == gScheduledIam[lEvent.nReference].isScheduled)
                     {
                        ChnlUnlock(lEvent.nReference);
                        FillRelAndClearScheduled(lEvent.nReference, lGenIsupMsg, lChRelRes);
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "TH:EV:S CKTID:[%03d] Event S_CALL_UNAVAILABLE Clear Schedule IAM/SETUP OA:%s DA:%s", 
                              lEvent.nReference + 1, lGenIsupMsg.isupIam.callingNumber, 
                              lGenIsupMsg.isupIam.calledNumber);
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(SAPI100, lLogText);
                        return 1;
                     }
                     else
                        ChnlUnlock(lEvent.nReference);
                  }
                  break;
               case 125: /*S_ISUP_WaitDialAnswer*/
                  {
                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_WAIT_DAIL_ANS;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                     ChnlUnlock(lEvent.nReference);
                  }
                  break;
               case 121: /*S_ISUP_WaitRLC*/
                  {
#if 0
                     lSigChnlSt = mpSynwaySystem->sigChannel[lEvent.nReference].status;

                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_WAIT_REL_SND;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                     ChnlUnlock(lEvent.nReference);
                     switch(lSigChnlSt)
                     {
                        case SNW_IAM_SENT:
                        case SNW_WAIT_DAIL_ANS:
                        case SNW_DIALING:
                        case SNW_WAIT_REMOTE_PICKUP:
                        case SNW_ANM_RCVD:
                        case SNW_TALKING:
                           {
                              //Send Release to Application
                              lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
                              lGenIsupMsg.circuitId = lEvent.nReference;

                              return 1;
                           }
                           break;
                        case SNW_CHNL_NOT_AVAILABLE:
                        case SNW_CHNL_IDLE:
                        case SNW_IAM_RCVD:
                        case SNW_WAIT_ACM_SND:
                        case SNW_RINGING:
                        case SNW_WAIT_RLC:
                        case SNW_REMOTE_HANGUP:
                        case SNW_WAIT_REL_SND:
                        default:
                           {
                           }
                     }
#endif                           
                  }
                  break;
               default:
                  {
                  }
            }
            return 0;//break;
         }
      case 0x0019:/*E_PROC_AutoDial*/
         {
            lSigChnlSt = mpSynwaySystem->sigChannel[lEvent.nReference].status;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:S Event E_PROC_AutoDial in CKTID:[%03d] AutoDial progress:%u(%s) ProcCS:%d(%s)", 
                  lEvent.nReference + 1, lEvent.dwParam, SynwayUtil::ConvertSnwDialStToStr(lEvent.dwParam),
                  lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI90, lLogText);

            switch(lEvent.dwParam)
            {
               case 1:/*DIAL_DIALING*/
                  {
                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_DIALING;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                     ChnlUnlock(lEvent.nReference);

                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:S CKTID:[%03d] Call is Dialing", lEvent.nReference + 1);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI91, lLogText);
                  }
                  break;
               case 2:/*DIAL_ECHOTONE*/
                  {
                     ChnlLock(lEvent.nReference);
                     mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_WAIT_REMOTE_PICKUP;
                     mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                     ChnlUnlock(lEvent.nReference);

                     lGenIsupMsg.isupMsgType = GEN_ISUP_ACM;
                     lGenIsupMsg.circuitId = lEvent.nReference;

                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:S CKTID:[%03d] ACM received", lEvent.nReference + 1);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI92, lLogText);

                     return 1;
                  }
               case 7:/*DIAL_VOICE*/
                  {
                     //ChnlLock(lEvent.nReference);
                     //mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_ANM_RCVD;
                     //mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                     //ChnlUnlock(lEvent.nReference);

                     lGenIsupMsg.isupMsgType = GEN_ISUP_ANM;
                     lGenIsupMsg.circuitId = lEvent.nReference;

                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:S CKTID:[%03d] ANM received", lEvent.nReference + 1);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI93, lLogText);

                     return 1;
                  }
               case 3:/*DIAL_NO_DIALTONE*/
               case 4:/*DIAL_BUSYTONE*/
               case 10:/*DIAL_NOANSWER*/
               case 11:/*DIAL_FAILURE*/
               case 12:/*DIAL_INVALID_PHONUM*/
               default:
                  {
                     int lDialFailRes = SsmGetAutoDialFailureReason(lEvent.nReference);
                     int lRelRes      = SsmGetReleaseReason(lEvent.nReference);

                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:S CKTID:[%03d] AutoDial Failed Snd IAM/SETUP Reason:%d RELRes:%d\n", 
                           lEvent.nReference + 1, lDialFailRes, lRelRes);
                     TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI66, lLogText);

                     //if(66/*ATDL_RcvREL*/ == lDialFailRes)
                     int lChTyp = SsmGetChType(lEvent.nReference);
                     // 7 - user end ISDN 8 - Network side ISDN
                     if(66/*ATDL_RcvREL*/ == lDialFailRes ||
                         ((7 == lChTyp || 8 == lChTyp )&& 121 == lDialFailRes/*ISDN_REJECT*/))
                     {
                        ChnlLock(lEvent.nReference);
                        mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_WAIT_REL_SND;
                        mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
                        ChnlUnlock(lEvent.nReference);

                        switch(lSigChnlSt)
                        {
                           case SNW_IAM_SENT:
                           case SNW_WAIT_DAIL_ANS:
                           case SNW_DIALING:
                           case SNW_WAIT_REMOTE_PICKUP:
                           case SNW_ANM_RCVD:
                           case SNW_TALKING:
                              {
                                 //Send REL/DISCONNECT to Application as IAM/SETUP Failed
                                 lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
                                 lGenIsupMsg.circuitId = lEvent.nReference;

                                 if(0 >= lRelRes)
                                    lGenIsupMsg.isupRel.cause = TEMP_FAILURE;
                                 else
                                    lGenIsupMsg.isupRel.cause = lRelRes;

                                 return 1;
                              }
                              break;
                           case SNW_CHNL_NOT_AVAILABLE:
                           case SNW_CHNL_IDLE:
                           case SNW_IAM_RCVD:
                           case SNW_WAIT_ACM_SND:
                           case SNW_RINGING:
                           case SNW_WAIT_RLC:
                           case SNW_REMOTE_HANGUP:
                           case SNW_WAIT_REL_SND:
                           default:
                              {
                              }
                        }
                     }
                  }
            }
            return 0;
         }
      case 0x0020: /*E_CHG_RxPhoNumBuf*/
         {
            INT16 lRedirRes;

            ChnlLock(lEvent.nReference);
            mpSynwaySystem->sigChannel[lEvent.nReference].status = SNW_IAM_RCVD;
            mpSynwaySystem->sigChannel[lEvent.nReference].callDirection = SNW_IN_BOUND_CALL;
            mpSynwaySystem->sigChannel[lEvent.nReference].statusUpdateTime = time(NULL);
            ChnlUnlock(lEvent.nReference);

            lGenIsupMsg.isupMsgType = GEN_ISUP_IAM;
            lGenIsupMsg.circuitId = lEvent.nReference;
            strcpy(lGenIsupMsg.isupIam.callingNumber, SsmGetCallerIdA(lEvent.nReference));
            strcpy(lGenIsupMsg.isupIam.calledNumber, SsmGetPhoNumStrA(lEvent.nReference));
            lGenIsupMsg.isupIam.callingNumberFormat = GEN_INTERNATIONAL_FMT;
            lGenIsupMsg.isupIam.calledNumberFormat = GEN_INTERNATIONAL_FMT;
            lRedirRes = SsmGetRedirectionInfReason(lEvent.nReference);
            if(lRedirRes > 0)
            {
               //TBD To be Modified
            }
            else
               lGenIsupMsg.isupIam.redirectReason =  GEN_RI_UNKNOWN;
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:S Event E_CHG_RxPhoNumBuf Rcv IAM in CKTID:[%03d] ClgNum:%s CldNum:%s", 
                  lEvent.nReference + 1, lGenIsupMsg.isupIam.callingNumber, 
                  lGenIsupMsg.isupIam.calledNumber);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI31, lLogText);
            return 1;//break;
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:S Event EC:0x%04X CKTID:[%03d] dwParam:%u Not Processed",
                  lEvent.wEventCode , lEvent.nReference + 1, lEvent.dwParam);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI31, lLogText);

            return 0;
         }
   }

   return 1;
}

//-------------------------------------------------------------
// FUNCTION    : HandleEvent
// DESCRIPTION : Process Event
// PARAMETER   : response structures, MESSAGE_INFO
// RETURN      : INT16
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::HandleEvent(MpResponseDetails &rsp, MESSAGE_INFO &lEvent)
{
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";
   unsigned int 	lTemp_1;
   unsigned int 	lTemp_2;

   switch(lEvent.wEventCode)
   {
      case 0x000F:/*E_PROC_PlayEnd*/
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated Play Stoped in CKTID:[%03d] Reason:%u", 
               lEvent.nReference + 1, lEvent.dwParam);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return HandlePlayStopped(lEvent, rsp);
      case 0x0010:/*E_PROC_PlayFile*/
         ChnlLock(lEvent.nReference);
         mpSynwaySystem->channel[lEvent.nReference].mediaCompTime = time(NULL);
         ChnlUnlock(lEvent.nReference);

         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated Play in Progress in CKTID:[%03d] PlayTime:%d", 
               lEvent.nReference + 1, lEvent.dwParam);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return 0;//break;
      case 0x0011:/*E_PROC_PlayFileList*/
         ChnlLock(lEvent.nReference);
         mpSynwaySystem->channel[lEvent.nReference].mediaCompTime = time(NULL);
         ChnlUnlock(lEvent.nReference);

         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated Play in Progress From File List CKTID:[%03d] FileNum:%d",
               lEvent.nReference + 1, lEvent.dwParam);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return 0;
      case 0x0013:/*E_PROC_RecordEnd*/
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated Record Stoped in CKTID:[%03d] Reason:%u", 
               lEvent.nReference + 1, lEvent.dwParam);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return HandleRecordStopped(lEvent, rsp);
      case 0x0014:/*E_PROC_RecordFile*/
         ChnlLock(lEvent.nReference);
         mpSynwaySystem->channel[lEvent.nReference].mediaCompTime = time(NULL);
         ChnlUnlock(lEvent.nReference);

         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated Record File in Progress in CKTID:[%03d] RecordTime:%d", 
               lEvent.nReference + 1, lEvent.dwParam);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return 0;
      case 0x000B:/*E_PROC_WaitDTMF*/
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated GetDtmf Stopped in CKTID:[%03d] Reason:%d", 
               lEvent.nReference + 1, lEvent.dwParam);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return HandleGetDtmfStopped(lEvent, rsp);
      case 0x002E:/*E_SYS_BargeIn*/
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated BargeIn in CKTID:[%03d] Voice:%d",
               lEvent.nReference + 1, lEvent.dwParam);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return 0;
      case 0x002F:/*E_SYS_NoSound*/
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M Event Generated No sound in CKTID:[%03d]",
               lEvent.nReference + 1);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI31, lLogText);
         return 0;
      case 0x000C:/*E_CHG_RcvDTMF*/
         {
            lTemp_1 = lEvent.dwParam & 0x00FF;
            lTemp_2 = lEvent.dwParam >> 16 ;
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M Event Generated Dtfm Detected in CKTID:[%03d] len:%d  lastRcd:%d  lastchar:%c",
                  lEvent.nReference + 1, lTemp_2, lTemp_1, lTemp_1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI31, lLogText);

            //----Added For play Controls---------------------------
            if(true == mpSynwaySystem->channel[lEvent.nReference].currentPlay.playCtrlEnable)
               return HandlePlayControls(lEvent);
            //------------------------------------------------------
            /*for(unsigned int k = 0; k < lTemp_2; k++)
              {
              if(-1 == SsmGet1stDtmfClr(lEvent.nReference, &pcDtmf))
              {
              SsmGetLastErrMsg(lErrMsg);
              snprintf(lLogText, MAX_LOG_TEXT_LEN,
              "TH:EV:M SsmGet1stDtmfClr Failed CKTID:[%03d] EC:%d EM:%s",
              lEvent.nReference + 1, SsmGetLastErrCode(), lErrMsg);
              TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
              return -1;
              }
              }*/
            return 0;
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M Event Generated EC:0x%04X CKTID:[%03d] dwParam:%u Not Processed",
                  lEvent.wEventCode , lEvent.nReference + 1, lEvent.dwParam);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI31, lLogText);
            return 0;
         }
   }

   return 1;
}


//-------------------------------------------------------------
// FUNCTION    : HandlePlayControls
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::HandlePlayControls(MESSAGE_INFO &lEvent)
{
   unsigned int   lTemp_1;
   unsigned int   lTemp_2;
   char  			pcDtmf;
   synwayChannel* pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";

   lTemp_1 = lEvent.dwParam & 0x00FF;
   lTemp_2 = lEvent.dwParam >> 16 ;

   for(unsigned int k = 0; k < lTemp_2; k++)
   {
      if(-1 == SsmGet1stDtmfClr(lEvent.nReference, &pcDtmf))
      {
         SsmGetLastErrMsg(lErrMsg);
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:EV:M SsmGet1stDtmfClr Failed in PlayControls CKTID:[%03d] EC:%d EM:%s",
               lEvent.nReference + 1, SsmGetLastErrCode(), lErrMsg);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         return -1;
      }
   }

   pChnl   = &mpSynwaySystem->channel[lEvent.nReference];

   if(lTemp_1 == (unsigned int)pChnl->currentPlay.playCtrl.pauseDtmf)
   {
      MpPause lpause;
      lpause.seqNum = pChnl->currentPlay.seqNum;
      lpause.circuitId = pChnl->currentPlay.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:EV:M Processing Play Control:Issue PAUSE "
            "Play CKTID:[%03d] len:%d DTMF:%c",
            lEvent.nReference + 1, lTemp_2, lTemp_1);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI83, lLogText);

      if(MP_SUCCESS != Pause(lpause,(TEXT*) "TH:EV:M"))
         return 0;
   }
   else if(lTemp_1 == (unsigned int)pChnl->currentPlay.playCtrl.resumeDtmf)
   {
      MpResume lResume;
      lResume.seqNum = pChnl->currentPlay.seqNum;
      lResume.circuitId = pChnl->currentPlay.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:EV:M Processing Play Control:Issue RESUME "
            "Play CKTID:[%03d] len:%d DTMF:%c",
            lEvent.nReference + 1, lTemp_2, lTemp_1);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI83, lLogText);

      if(MP_SUCCESS != Resume(lResume,(TEXT*) "TH:EV:M"))
         return 0;
   }
   else if(lTemp_1 ==  (unsigned int)pChnl->currentPlay.playCtrl.forwardDtmf)
   {
      MpForward lForward;
      lForward.seqNum = pChnl->currentPlay.seqNum;
      lForward.circuitId = pChnl->currentPlay.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:EV:M Processing Play Control:Issue FORWARD "
            "Play CKTID:[%03d] len:%d DTMF:%c",
            lEvent.nReference + 1, lTemp_2, lTemp_1);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI83, lLogText);

      if(MP_SUCCESS != Forward(lForward,(TEXT*) "TH:EV:M"))
         return 0;
   }
   else if(lTemp_1 == (unsigned int)pChnl->currentPlay.playCtrl.rewindDtmf)
   {
      MpRewind lRewind;
      lRewind.seqNum = pChnl->currentPlay.seqNum;
      lRewind.circuitId = pChnl->currentPlay.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:EV:M Processing Play Control:Issue REWIND "
            "Play CKTID:[%03d] len:%d DTMF:%c",
            lEvent.nReference + 1, lTemp_2, lTemp_1);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI83, lLogText);

      if(MP_SUCCESS != Rewind(lRewind,(TEXT*) "TH:EV:M"))
         return 0;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:EV:M Processing Play Control: INVALID"
            "PLAY CONTROL OPTION CKTID:[%03d] len:%d DTMF:%c",
            lEvent.nReference + 1, lTemp_2, lTemp_1);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI84, lLogText);

      return 0;
   }

   return 0;
}

//-------------------------------------------------------------
// FUNCTION    : HandleGetDtmfStopped
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::HandleGetDtmfStopped(MESSAGE_INFO &lEvent, MpResponseDetails &rsp)
{
   INT16          lCktId   = -1;
   synwayChannel* pChnl    = NULL;
   INT16          lReturn  = -1;
   TEXT				lTmpDtmfDgts[45] = "";
   TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";

   lCktId  = lEvent.nReference;
   pChnl   = &mpSynwaySystem->channel[lEvent.nReference];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "TH:EV:M CKTID:[%03d] GetDtmf Stopped ST:%d Reason:%d(%s)", 
         lCktId + 1, pChnl->status, lEvent.dwParam,
         SynwayUtil::ConvertGetDtmfStopResToStr(lEvent.dwParam));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SAPI98, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] GetDtmf Stoped, ST:CKT_UNAVAILABLE",
                  lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, lLogText);
            return -1;
         }
      case CKT_PATCHED:
      case CKT_IDLE:
      case CKT_PLAYING:
      case CKT_PLAY_STOPING:
      case CKT_RECORDING:
      case CKT_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] Not in GetDtmf ST:%s",
                  lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI32, lLogText);
            return -1;
         }
      case CKT_PLAYING_AND_GETDIG:
      case CKT_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
         {
            rsp.cmdType = MC_GETDIGSTP;
            rsp.mpRspData.getDigit = pChnl->currentGetdigit;
            if(true != CancelWaitDtmf(lCktId, pChnl->currentGetdigit.seqNum,(TEXT*) "TH:EV:M",
                     lTmpDtmfDgts, false))
            {
               rsp.rspMsg = MP_RSP_FAILURE;
               lReturn = 0;
            }
            else
            {
               rsp.rspMsg = MP_RSP_SUCCESS;
               strcat(rsp.mpRspData.getDigit.rspDigits, lTmpDtmfDgts);
               lReturn = 1;
            }
            ChnlLock(lCktId);
            pChnl->playStatus       = PLAY_NEXT_NOT_REQD;
            pChnl->cktId            = lCktId;
            pChnl->mediaCompTime = time(NULL);
            if(CKT_GETDIG == pChnl->status)
               pChnl->status          = CKT_IDLE;
            else if(CKT_PLAY_STOPING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_PLAY_STOPING;
            else if(CKT_RECORDING_AND_GETDIG  == pChnl->status)
               pChnl->status          = CKT_RECORDING;
            else if(CKT_PLAYING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_PLAYING;
            else if(CKT_RECORD_STOPING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_RECORD_STOPING;
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:EV:M CKTID:[%03d]"
                     "ST:%s State Changed Abbrupt in HandleGetDtmfStopped\n",
                     lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI33, lLogText);
            }
            memset(&(pChnl->currentGetdigit), 0, sizeof(MpGetDigit));
            ChnlUnlock(lCktId);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M RefNum:%08X CKTID:[%03d] GetDtmf Stopped For ST:%s",
                  pChnl->currentGetdigit.seqNum,
                  lCktId + 1, 
                  SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI34, lLogText);
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] ST:%d  GetDtmfStopped Invalid Channel Status",
                  lCktId + 1, pChnl->status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI14, lLogText);
            return -1;
         }
   }
   return 1;
}

//-------------------------------------------------------------
// FUNCTION    : HandleRecordStopped
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::HandleRecordStopped(MESSAGE_INFO &lEvent, MpResponseDetails &rsp)
{
   INT16          lCktId   = -1;
   synwayChannel* pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";

   lCktId  = lEvent.nReference;
   pChnl   = &mpSynwaySystem->channel[lEvent.nReference];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "TH:EV:M CKTID:[%03d] Record Stopped ST:%d Reason:%d(%s)", 
         lCktId + 1, pChnl->status, lEvent.dwParam,
         SynwayUtil::ConvertRecordStopResToStr(lEvent.dwParam));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SAPI97, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] Record Stoped, ST:CKT_UNAVAILABLE",
                  lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, lLogText);
            return -1;
         }
      case CKT_GETDIG:
      case CKT_PATCHED:
      case CKT_IDLE:
      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
      case CKT_PLAY_STOPING:
      case CKT_PLAY_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] Not Recording any thing ST:%s",
                  lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI32, lLogText);
            return -1;
         }
      case CKT_RECORDING:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING:
         {
            rsp.rspMsg              	= MP_RSP_SUCCESS;
            rsp.cmdType        	= MC_RECSTP;
            rsp.mpRspData.recordMsg   = pChnl->currentRecord;
            ChnlLock(lCktId);
            pChnl->mediaCompTime = time(NULL);
            pChnl->playStatus       = PLAY_NEXT_NOT_REQD;
            pChnl->cktId            = lCktId;
            if(CKT_RECORDING == pChnl->status)
               pChnl->status          = CKT_IDLE;
            else if(CKT_RECORDING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_GETDIG;
            else if(CKT_RECORD_STOPING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_GETDIG;
            else if(CKT_RECORD_STOPING == pChnl->status)
               pChnl->status          = CKT_IDLE;
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:EV:M CKTID:[%03d]"
                     "ST:%s State Changed Abbrupt in HandleRecordStopped\n",
                     lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI33, lLogText);
            }
            memset(&(pChnl->currentRecord), 0, sizeof(MpRecord));
            ChnlUnlock(lCktId);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M RefNum:%08X CKTID:[%03d] Record Stopped For ST:%s",
                  pChnl->currentRecord.seqNum, 
                  lCktId + 1, 
                  SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI34, lLogText);
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] ST:%d Record Stopped Invalid Channel Status",
                  lCktId + 1, pChnl->status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI14, lLogText);
            return -1;
         }
   }
   return 1;
}
//-------------------------------------------------------------
// FUNCTION    : HandlePlayStopped
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::HandlePlayStopped(MESSAGE_INFO &lEvent, MpResponseDetails &rsp)
{
   INT16			   lCktId   = -1;
   synwayChannel*	pChnl 	= NULL;
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   lCktId  = lEvent.nReference;
   pChnl   = &mpSynwaySystem->channel[lEvent.nReference];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "TH:EV:M CKTID:[%03d] Play Stopped ST:%d PST:%d Reason:%d(%s)", 
         lCktId + 1, pChnl->status, pChnl->playStatus, lEvent.dwParam,
         SynwayUtil::ConvertPlayStopResToStr(lEvent.dwParam));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SAPI96, lLogText);

   if(6 == lEvent.dwParam) //Retrun as its play Pause event
      return 0;

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "TH:EV:M CKTID:[%03d] Play Stoped, ST:CKT_UNAVAILABLE", 
                  lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, lLogText);
            return -1;
         }
      case CKT_RECORD_STOPING_AND_GETDIG:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_GETDIG:
      case CKT_PATCHED:
      case CKT_IDLE:
      case CKT_RECORD_STOPING:
      case CKT_RECORDING:
         {

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] Not Playing any thing ST:%s",
                  lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI32, lLogText);
            return -1;
         }
      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "TH:EV:M RefNum:%08X CKTID:[%03d] Play Stopped for RptPly:%d ST:%s",
                  pChnl->currentPlay.seqNum, 
                  lCktId + 1, 
                  pChnl->currentPlay.repeatPlay, 
                  SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI34, lLogText);

            //rsp.mpRspData.playMsg 	= pChnl->currentPlay;

            if(1 == pChnl->currentPlay.repeatPlay && lEvent.dwParam == 1/*All voice data played out*/)
            {
               if(true != StartPlay(pChnl->currentPlay,(TEXT*) "TH:EV:M"))
               {
                  ChnlLock(lCktId);
                  pChnl->mediaCompTime = time(NULL);
                  pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
                  pChnl->cktId           = lCktId;
                  if(CKT_PLAYING == pChnl->status)
                     pChnl->status          = CKT_IDLE;
                  else if(CKT_PLAYING_AND_GETDIG == pChnl->status)
                     pChnl->status 			  = CKT_GETDIG;
                  else if(CKT_PLAY_STOPING == pChnl->status)
                     pChnl->status          = CKT_IDLE;
                  else if(CKT_PLAY_STOPING_AND_GETDIG == pChnl->status)
                     pChnl->status          = CKT_GETDIG;
                  else
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:M CKTID:[%03d]"
                           "ST:%s State Changed Abbrupt in HandlePlayStopped\n", 
                           lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
                     TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI33, lLogText);
                  }
                  memset(&(pChnl->currentPlay), 0, sizeof(MpPlay));
                  ChnlUnlock(lCktId);
                  rsp.rspMsg              = MP_RSP_FAILURE;
                  rsp.cmdType        		= MC_CNTTN;
                  rsp.mpRspData.playMsg   = pChnl->currentPlay;

                  return 1;
               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "TH:EV:M RefNum:%08X CKTID:[%03d] ST:%s Repeating Play FlCnt:%d",
                        pChnl->currentPlay.seqNum, 
                        pChnl->currentPlay.circuitId + 1, 
                        SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status),
                        pChnl->currentPlay.vFileCount);
                  T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(SAPI35, lLogText);
                  return 0;
               }
            }
            else
            {
               rsp.rspMsg 							  	= MP_RSP_SUCCESS;
               rsp.cmdType 							= MC_DISTN;
               rsp.mpRspData.stopMsg.seqNum    	= pChnl->currentPlay.seqNum;
               rsp.mpRspData.stopMsg.circuitId 	= pChnl->currentPlay.circuitId;

               ChnlLock(lCktId);
               pChnl->mediaCompTime = time(NULL);
               pChnl->playStatus      	= PLAY_NEXT_NOT_REQD;
               pChnl->cktId           	= lCktId;
               if(CKT_PLAYING == pChnl->status)
                  pChnl->status          = CKT_IDLE;
               else if(CKT_PLAYING_AND_GETDIG == pChnl->status)
                  pChnl->status          = CKT_GETDIG;
               else if(CKT_PLAY_STOPING == pChnl->status)
                  pChnl->status          = CKT_IDLE;
               else if(CKT_PLAY_STOPING_AND_GETDIG == pChnl->status)
                  pChnl->status          = CKT_GETDIG;
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "TH:EV:M CKTID:[%03d]"
                        "ST:%s State Changed Abbrupt in HandlePlayStopped\n",
                        lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(SAPI33, lLogText);
               }
               memset(&(pChnl->currentPlay), 0, sizeof(MpPlay));
               ChnlUnlock(lCktId);
            }

         }
         break;
      case CKT_PLAY_STOPING:
      case CKT_PLAY_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "TH:EV:M RefNum:%08X CKTID:[%03d] Play Stopped for RptPly:%d ST:%s PlayST:%d",
                  pChnl->currentPlay.seqNum, lCktId + 1, pChnl->currentPlay.repeatPlay, 
                  SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status), pChnl->playStatus);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI34, lLogText);

            if(pChnl->playStatus == PLAY_NEXT_IMMEDIATE || pChnl->playStatus == PLAY_NEXT_REQD)
            {
               rsp.mpRspData.playMsg   = pChnl->nextPlay;
               rsp.rspMsg              = MP_RSP_SUCCESS;
               rsp.cmdType        		= MC_CNTTN;

               ChnlLock(lCktId);
               pChnl->mediaCompTime = time(NULL);
               memset(&(pChnl->currentPlay), 0, sizeof(MpPlay));
               pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
               if(CKT_PLAY_STOPING == pChnl->status)
                  pChnl->status          = CKT_IDLE;
               else if(CKT_PLAY_STOPING_AND_GETDIG == pChnl->status)
                  pChnl->status          = CKT_GETDIG;
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "TH:EV:M CKTID:[%03d]"
                        "ST:%s State Changed Abbrupt in HandlePlayStopped\n",
                        lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(SAPI33, lLogText);
               }
               ChnlUnlock(lCktId);
               if(MP_SUCCESS != this->Play(pChnl->nextPlay,(TEXT*) "TH:EV:M"))
               {
                  rsp.rspMsg              = MP_RSP_FAILURE;
                  rsp.cmdType        		= MC_CNTTN;
                  rsp.mpRspData.playMsg   = pChnl->nextPlay;
                  return 1;
               }
               
               if(true == pChnl->nextPlay.nonBlocking)
                  return 1; //Added Chethan 26/06/13 Not to send response when play is queued 
               else
                  return 0;
            }
            else
            {
               rsp.rspMsg 							  	= MP_RSP_SUCCESS;
               rsp.cmdType 							= MC_DISTN;
               rsp.mpRspData.stopMsg.seqNum    	= pChnl->currentPlay.seqNum;
               rsp.mpRspData.stopMsg.circuitId 	= pChnl->currentPlay.circuitId;

               ChnlLock(lCktId);
               pChnl->mediaCompTime = time(NULL);
               pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
               if(CKT_PLAY_STOPING == pChnl->status)
                  pChnl->status          = CKT_IDLE;
               else if(CKT_PLAY_STOPING_AND_GETDIG == pChnl->status)
                  pChnl->status          = CKT_GETDIG;
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "TH:EV:M CKTID:[%03d]"
                        "ST:%s State Changed Abbrupt in HandlePlayStopped\n",
                        lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(SAPI33, lLogText);
               }
               ChnlUnlock(lCktId);
            }
         }
         break;	
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:M CKTID:[%03d] ST:%d Play Stopped Invalid Channel Status",
                  lCktId + 1, pChnl->status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI14, lLogText);
            return -1;
         }
   }
   return 1;
}

//-------------------------------------------------------------
// FUNCTION    : AdjSpdVol
// DESCRIPTION : Adjust Speed and volume
// PARAMETER   : Channel Number, string  that specifies ADD or SUB of Volume
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::AdjSpdVol(const MpSetVolSpd req)
{
   synwayChannel*				pChnl   = NULL;
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   pChnl   = &mpSynwaySystem->channel[req.circuitId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CKTID:[%03d] AdjApdVol Inavlid cmd",
         req.seqNum, req.circuitId + 1);

   if(-1 == SsmSetPlayVolume(req.circuitId, req.adjSize))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CKTID:[%03d] Val:%d SsmSetPlayVolume Failed EC:%d EM:%s",
            req.seqNum, req.circuitId + 1, req.adjSize, SsmGetLastErrCode(), lErrMsg);
      gLog.GenerateLog(SAPI35, lLogText);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return MP_ADJ_VOL_SPD_ERR;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "CKTID:[%03d] RefNum:%08X SsmSetPlayVolume Success",
         req.circuitId + 1, req.seqNum);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SAPI36, lLogText);

   return MP_SUCCESS;	
}

//-------------------------------------------------------------
// FUNCTION    : ShowChnlStatus
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
void SynwayVoiceMgrApi::ShowChnlStatus()
{
   T (gTrace, printf("%s: Channels Status :\n", gProcessName);
         for(INT16 i = 0; i < mpSynwaySystem->mMaxChannel; i++)
         printf("CKTID:[%03d] ST:%d PlayST:%d\n", i + 1,
            mpSynwaySystem->channel[i].status,
            mpSynwaySystem->channel[i].playStatus););
}

//-------------------------------------------------------------
// FUNCTION    : RecToFile
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::RecToFile(MpRecord recFile, TEXT *lType)
{
   TEXT           lpstrDtmfCharSet[25]             = "";
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";


   if(SsmEnableRxDtmf(recFile.circuitId, true))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR "
            "SsmEnableRxDtmf Failed EC:%d EM:%s", lType, recFile.seqNum,
            recFile.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI16, lLogText);
      return false;
   }

   if(SsmClearRxDtmfBuf(recFile.circuitId))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR "
            "SsmClearRxDtmfBuf Failed EC:%d EM:%s", lType, recFile.seqNum,
            recFile.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI17, lLogText);
      return false;
   }

   if(0 == strlen(recFile.dtmfStopRecCharSet))
      strcpy(lpstrDtmfCharSet, "abcd");
      //strcpy(lpstrDtmfCharSet, "0123456789*#abcd");
   else
      strcpy(lpstrDtmfCharSet,recFile.dtmfStopRecCharSet);

   if(SsmSetDTMFStopRecCharSet(recFile.circuitId, lpstrDtmfCharSet))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR "
            "SsmSetDTMFStopRecCharSet Failed EC:%d EM:%s", lType, recFile.seqNum,
            recFile.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI86, lLogText);
      return false;
   }


   if(SsmRecToFile(recFile.circuitId, recFile.voiceFile, 
            mRecordFileFormat_Cfg, 0xfffffffe, 0xffffffff, recFile.recTime, 1))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "SsmRecToFile Failed Tm:%d Nam:%s EC:%d EM:%s", lType,
            recFile.seqNum, recFile.circuitId + 1, recFile.recTime, recFile.voiceFile, 
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI37, lLogText);
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// FUNCTION    : StartRecord
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::StartRecord(const MpRecord req, TEXT *lType)
{
   INT16			   lCktId   = -1;
   synwayChannel*	pChnl 	= NULL;
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   lCktId  = req.circuitId;
   pChnl   = &mpSynwaySystem->channel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: RefNum:%08X CKTID:[%03d] RecTm:%d FL:%s Tc:%s Rcvd Record Start", 
         lType, req.seqNum, lCktId + 1, req.recTime, req.voiceFile, req.dtmfStopRecCharSet);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR ST:CKT_UNAVAILABLE", lType,
                  req.seqNum, lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
      case CKT_PATCHED:
      case CKT_RECORDING:
      case CKT_PLAY_STOPING:
      case CKT_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR "
                  "ST:%s Hence Can not Start Record", lType, req.seqNum,
                  lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI32, lLogText);
            return MP_CKT_BUSY_ERR;
         }
      case CKT_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:CKT_GETDIG Hence issuing SsmRecToFile", lType,
                  req.seqNum, lCktId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI38, lLogText);

            if(true != RecToFile(req, lType))
               return MP_RECORD_CMD_ERR;

            ChnlLock(lCktId);
            pChnl->currentRecord   = req;
            pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
            pChnl->status          = CKT_RECORDING_AND_GETDIG;
            pChnl->cktId           = lCktId;
            pChnl->mediaCompTime = time(NULL);
            ChnlUnlock(lCktId);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR "
                  "ST:%d SsmRecToFile Success", lType, req.seqNum,
                  lCktId + 1, pChnl->status);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI39, lLogText);
         }
         break;
      case CKT_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:CKT_IDLE Hence issuing SsmRecToFile", lType,
                  req.seqNum, lCktId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI13, lLogText);

            if(true != RecToFile(req, lType))
               return MP_RECORD_CMD_ERR;

            ChnlLock(lCktId);
            pChnl->currentRecord   = req;
            pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
            pChnl->status          = CKT_RECORDING;
            pChnl->cktId           = lCktId;
            pChnl->mediaCompTime = time(NULL);
            ChnlUnlock(lCktId);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR "
                  "ST:%d SsmRecToFile Success", lType,
                  req.seqNum, lCktId + 1, pChnl->status);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI39, lLogText);
         }
         break;	
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:%d Invalid Channel Status", lType,
                  req.seqNum, lCktId + 1, pChnl->status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI14, lLogText);
            return MP_BADPARM_ERR;
         }
   }
   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : StopRecToFile
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::StopRecToFile(UINT16 lChnl, unsigned int lSeq, TEXT *lType)
{
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   if(SsmStopRecToFile(lChnl))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "StopRecToFile Failed EC:%d EM:%s", lType,
            lSeq, lChnl + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI40, lLogText);
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// FUNCTION    : StopRecord
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::StopRecord(const MpStop req, TEXT *lType)
{
   INT16			   lCktId   = -1;
   synwayChannel*	pChnl 	= NULL;
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   lCktId  = req.circuitId;
   pChnl   = &mpSynwaySystem->channel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: RefNum:%08X CKTID:[%03d] Rcvd Record Stop", lType,
         req.seqNum, lCktId + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTP ST:CKT_UNAVAILABLE", lType,
                  req.seqNum, lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_PLAY_STOPING:
      case CKT_PATCHED:
      case CKT_GETDIG:
      case CKT_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTP "
                  "ST:%s Not Recording To Stop", lType,
                  req.seqNum, lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI32, lLogText);
         }
         break;
      case CKT_RECORD_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTP "
                  "ST:%s Stop Recording in Progress", lType,
                  req.seqNum, lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI41, lLogText);
         }
         break;
      case CKT_RECORDING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:CKT_RECORDING Hence issuing SsmStopRecToFile", lType,
                  req.seqNum, lCktId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI42, lLogText);

            if(true != StopRecToFile(lCktId, req.seqNum, lType))
               return MP_RECORD_STOP_CMD_ERR;
            if(pChnl->status != CKT_IDLE)
            {
               ChnlLock(lCktId);
               pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
               pChnl->status          = CKT_RECORD_STOPING;
               pChnl->cktId           = lCktId;
               pChnl->mediaCompTime = time(NULL);
               ChnlUnlock(lCktId);
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTP "
                  "ST:%d SsmStopRecToFile Success", lType,
                  req.seqNum, lCktId + 1, pChnl->status);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI43, lLogText);
         }
         break;
      case CKT_RECORDING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:CKT_RECORDING_AND_GETDIG Hence issuing SsmRecToFile", lType,
                  req.seqNum, lCktId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI44, lLogText);

            if(true != StopRecToFile(lCktId, req.seqNum, lType))
               return MP_RECORD_STOP_CMD_ERR;

            if(pChnl->status != CKT_IDLE && pChnl->status != CKT_GETDIG)
            {
               ChnlLock(lCktId);
               pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
               pChnl->status          = CKT_RECORD_STOPING_AND_GETDIG;
               pChnl->cktId           = lCktId;
               pChnl->mediaCompTime = time(NULL);
               ChnlUnlock(lCktId);
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_RECSTR "
                  "ST:%d SsmRecToFile Success", lType, req.seqNum,
                  lCktId + 1, pChnl->status);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI39, lLogText);
         }
         break;	
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:%d Invalid Channel Status", lType, req.seqNum,
                  lCktId + 1, pChnl->status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI14, lLogText);
            return MP_BADPARM_ERR;
         }
   }
   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : CollectDtmfBuf
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::CollectDtmfBuf(MpGetDigit getDigit, TEXT *lTmpDgt, TEXT *lType)
{
   TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";
   INT16				lRet = -1;

   lRet = SsmGetDtmfStr(getDigit.circuitId, lTmpDgt);
   if(0 > lRet)
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "SsmGetDtmfStr Failed EC:%d EM:%s", lType, getDigit.seqNum, 
            getDigit.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI45, lLogText);
      return -1;
   }

   T(gTrace, printf("%s: %s: RefNum:%08X CKTID:[%03d] RetVal:%d SsmGetDtmfStr:%s\n", 
            gProcessName, lType, getDigit.seqNum,
            getDigit.circuitId + 1, lRet, lTmpDgt););

   if(SsmClearRxDtmfBuf(getDigit.circuitId))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "SsmClearRxDtmfBuf Failed in CollectDtmfBuf EC:%d EM:%s", lType, getDigit.seqNum,
            getDigit.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI17, lLogText);
      return -1;
   }


   return lRet;
}

//-------------------------------------------------------------
// FUNCTION    : WaitDtmf
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::WaitDtmf(MpGetDigit getDigit, TEXT *lType)
{
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   if(SsmEnableRxDtmf(getDigit.circuitId, true))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "SsmEnableRxDtmf true Failed EC:%d EM:%s", lType, getDigit.seqNum,
            getDigit.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI16, lLogText);
      return false;
   }

   for(UINT16 j = 0; j < strlen(getDigit.teminateChars); j++)
      getDigit.teminateChars[j] = tolower(getDigit.teminateChars[j]);

   if(SsmSetWaitDtmfExA(getDigit.circuitId, getDigit.interDigitTimeOut, 
            getDigit.numOfDig, getDigit.teminateChars, getDigit.includeTerminateChar))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "SsmSetWaitDtmfExA Failed EC:%d EM:%s", lType, getDigit.seqNum,
            getDigit.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI46, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : GetDtmfDigits
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::GetDtmfDigits(const MpGetDigit req, MpGetDigit &rsp, TEXT *lType)
{
   INT16			   lCktId   = -1;
   synwayChannel*	pChnl 	= NULL;
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";
   INT16				lRetDtmf = -1;

   lCktId  = req.circuitId;
   pChnl   = &mpSynwaySystem->channel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "%s: RefNum:%08X CKTID:[%03d] NumDig:%d InterDigitTo:%d TermChars:%s Rcvd Get DtmfDigits", lType,
         req.seqNum, lCktId + 1, req.numOfDig, req.interDigitTimeOut, req.teminateChars);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_GETDIG ST:CKT_UNAVAILABLE", lType,
                  req.seqNum, lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
      case CKT_PLAYING_AND_GETDIG:
      case CKT_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
      case CKT_PATCHED:
      case CKT_RECORDING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_GETDIG "
                  "ST:%s Hence Can not GetDigit", lType,
                  req.seqNum, lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI32, lLogText);
            return MP_CKT_BUSY_ERR;
         }
      case CKT_PLAYING:
      case CKT_PLAY_STOPING:
      case CKT_IDLE:
      case CKT_RECORD_STOPING:
      case CKT_RECORDING:
         {

            rsp = req;
            lRetDtmf = CollectDtmfBuf(req, rsp.rspDigits, lType);
            if( 0 > lRetDtmf)
               return MP_GET_DIG_CMD_ERR;
            else if(lRetDtmf == req.numOfDig)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "%s: RefNum:%08X CKTID:[%03d] MC_GETDIG "
                     "Collected NoD:%d Dtmf:%s", lType,
                     req.seqNum, lCktId + 1, req.numOfDig, rsp.rspDigits);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI47, lLogText);
               return MP_GET_DIG_CMD_ALREADY_DONE;
            }
            else if(lRetDtmf < req.numOfDig)
            {
               rsp.numOfDig = rsp.numOfDig - lRetDtmf;

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "%s: RefNum:%08X CKTID:[%03d] MC_GETDIG "
                     "DigitsToGet:%d ST:%s issuing SsmSetWaitDtmfExA", lType,
                     req.seqNum, lCktId + 1, rsp.numOfDig, 
                     SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI48, lLogText);

               if(true != WaitDtmf(rsp, lType))
                  return MP_GET_DIG_CMD_ERR;

               ChnlLock(lCktId);
               pChnl->currentGetdigit = req;
               strcpy(pChnl->currentGetdigit.rspDigits, rsp.rspDigits);
               pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
               pChnl->cktId           = lCktId;

               pChnl->mediaCompTime = time(NULL) + ((req.interDigitTimeOut / 1000) * req.numOfDig);

               if(CKT_PLAYING == pChnl->status)
                  pChnl->status          = CKT_PLAYING_AND_GETDIG;
               else if(CKT_PLAY_STOPING == pChnl->status)
                  pChnl->status          = CKT_PLAY_STOPING_AND_GETDIG;
               else if(CKT_IDLE == pChnl->status)
                  pChnl->status          = CKT_GETDIG;
               else if(CKT_RECORD_STOPING == pChnl->status)
                  pChnl->status          = CKT_RECORD_STOPING_AND_GETDIG;
               else if(CKT_RECORDING == pChnl->status)
                  pChnl->status          = CKT_RECORDING_AND_GETDIG;
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "%s: RefNum:%08X CKTID:[%03d] MC_GETDIG "
                        "ST:%s State Changed Abbrupt\n", lType, req.seqNum, lCktId + 1, 
                        SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(SAPI33, lLogText);
               }

               ChnlUnlock(lCktId);

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "%s: RefNum:%08X CKTID:[%03d] MC_GETDIG "
                     "for DigitsToGet:%d ST:%d SsmSetWaitDtmfExA Success", lType,
                     req.seqNum, lCktId + 1, rsp.numOfDig, pChnl->status);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

               return MP_SUCCESS;
            }
            else 
            {
               rsp.rspDigits[req.numOfDig] = '\0';

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "%s: RefNum:%08X CKTID:[%03d] MC_GETDIG "
                     "Collected NoD:%d Dtmf:%s is more Actual NoD:%d", lType,
                     req.seqNum, lCktId + 1, rsp.numOfDig, rsp.rspDigits, req.numOfDig);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI47, lLogText);
               return MP_GET_DIG_CMD_ALREADY_DONE;
            }

         }
         break;	
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:%d Invalid Channel Status", lType,
                  req.seqNum, lCktId + 1, pChnl->status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI14, lLogText);
            return MP_BADPARM_ERR;
         }
   }
   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : CancelWaitDtmf
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::CancelWaitDtmf(UINT16 lChnl, unsigned int lseq, TEXT *lType, 
      TEXT *lColDigit, BOOLEAN lCancelWait)
{
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";
   INT16				lRet = -1;

   if(true == lCancelWait)
   {
      if(SsmCancelWaitDtmf(lChnl))
      {
         SsmGetLastErrMsg(lErrMsg);
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s: RefNum:%08X CKTID:[%03d] "
               "SsmCancelWaitDtmf Failed EC:%d EM:%s", lType,
               lseq, lChnl + 1, SsmGetLastErrCode(), lErrMsg);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI49, lLogText);
         return false;
      }
   }

   lRet = SsmChkWaitDtmf(lChnl, lColDigit);
   if(-1 == lRet)
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "SsmChkWaitDtmf lRtVal:%d EC:%d EM:%s", lType,
            lseq, lChnl + 1, lRet, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI50, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "%s: RefNum:%08X CKTID:[%03d] "
         "Cancel WaitDtmf ColDigits:%s", lType,
         lseq, lChnl + 1, lColDigit);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SAPI47, lLogText);

   if(SsmEnableRxDtmf(lChnl, false))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] "
            "SsmEnableRxDtmf false Failed EC:%d EM:%s", lType,
            lseq, lChnl + 1, SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI26, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : CancelGetDtmfDigits
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::CancelGetDtmfDigits(const MpStop req, TEXT *lType, TEXT *lColDigit)
{
   INT16			   lCktId   = -1;
   synwayChannel*	pChnl 	= NULL;
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   lCktId  = req.circuitId;
   pChnl   = &mpSynwaySystem->channel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "%s: RefNum:%08X CKTID:[%03d] Rcvd Cancel Get DtmfDigits", lType,
         req.seqNum, lCktId + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "%s: RefNum:%08X CKTID:[%03d] MC_GETDIGSTP ST:CKT_UNAVAILABLE", lType,
                  req.seqNum, lCktId + 1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI06, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
      case CKT_PLAYING:
      case CKT_PATCHED:
      case CKT_PLAY_STOPING:
      case CKT_IDLE:
      case CKT_RECORD_STOPING:
      case CKT_RECORDING:
         {

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_GETDIGSTP "
                  "ST:%s Not waiting for Dtmf", lType,
                  req.seqNum, lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI32, lLogText);
            return MP_CKT_BUSY_ERR;
         }
      case CKT_PLAYING_AND_GETDIG:
      case CKT_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
      case CKT_RECORDING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_GETDIGSTP "
                  "ST:%s Hence issuing cancel Dtmf", lType,
                  req.seqNum, lCktId + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI38, lLogText);

            if(true != CancelWaitDtmf(lCktId, req.seqNum, lType, lColDigit, true))
               return MP_CANCEL_GET_DIG_CMD_ERR;

            ChnlLock(lCktId);
            memset(&(pChnl->currentGetdigit), 0, sizeof(MpGetDigit));
            pChnl->playStatus      = PLAY_NEXT_NOT_REQD;
            pChnl->cktId           = lCktId;
            pChnl->mediaCompTime = time(NULL);

            if(CKT_PLAYING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_PLAYING;
            else if(CKT_PLAY_STOPING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_PLAY_STOPING;
            else if(CKT_GETDIG == pChnl->status)
               pChnl->status          = CKT_IDLE;
            else if(CKT_RECORD_STOPING_AND_GETDIG == pChnl->status)
               pChnl->status          = CKT_RECORD_STOPING;
            else if(CKT_RECORDING_AND_GETDIG== pChnl->status)
               pChnl->status          = CKT_RECORDING;
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "%s: RefNum:%08X CKTID:[%03d] MC_GETDIGSTP "
                     "ST:%s State Changed Abbrupt\n", lType, req.seqNum, lCktId + 1, 
                     SynwayVoiceMgrApi::ConvertStatusToStr(pChnl->status));
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI33, lLogText);
            }

            ChnlUnlock(lCktId);

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] MC_GETDIGSTP "
                  "ST:%d CancelGetDtmfDigits Success", lType,
                  req.seqNum, lCktId + 1, pChnl->status);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI51, lLogText);
         }
         break;	
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] "
                  "MC_RECSTR ST:%d Invalid Channel Status", lType,
                  req.seqNum, lCktId + 1, pChnl->status);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI14, lLogText);
            return MP_BADPARM_ERR;
         }
   }
   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : ConnectSpeech
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::ConnectSpeech(const MpSwitch req, TEXT *lType)
{
   synwayChannel*	pChnl_1 	= NULL;
   synwayChannel*	pChnl_2 	= NULL;
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   pChnl_1   = &mpSynwaySystem->channel[req.circuitId_1];
   pChnl_2   = &mpSynwaySystem->channel[req.circuitId_2];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "%s: RefNum:%08X CKTID:[%03d] CKTID:[%03d] Rcvd Establishing Two-way Connection", lType, req.seqNum,
         req.circuitId_1 + 1, req.circuitId_2 + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););


   //if(pChnl_1->status == CKT_IDLE && pChnl_2->status == CKT_IDLE)
   //{
      if(SsmTalkWith(req.circuitId_1, req.circuitId_2))
      {
         SsmGetLastErrMsg(lErrMsg);
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
               "SsmTalkWith EC:%d EM:%s", lType, req.seqNum,
               req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
               req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status),
               SsmGetLastErrCode(), lErrMsg);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI52, lLogText);
         return MP_CONNECT_CMD_ERR;
      }

      //ChnlLock(req.circuitId_1);
      //pChnl_1->playStatus      = PLAY_NEXT_NOT_REQD;
      //pChnl_1->cktId           = req.circuitId_1;
      //pChnl_1->status          = CKT_PATCHED;
      //ChnlUnlock(req.circuitId_1);

      //ChnlLock(req.circuitId_2);
      //pChnl_2->playStatus      = PLAY_NEXT_NOT_REQD;
      //pChnl_2->cktId           = req.circuitId_2;
      //pChnl_2->status          = CKT_PATCHED;
      //ChnlUnlock(req.circuitId_2);


      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
            "SsmTalkWith Success", lType, req.seqNum,
            req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
            req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI53, lLogText);

      if(0 != strlen(req.recordFile))
      {
         if(SsmSetRecBack(req.circuitId_2, 1))  
         {
            SsmGetLastErrMsg(lErrMsg);
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
                  "SsmSetRecBack Record Patch Fail EC:%d EM:%s", lType, req.seqNum,
                  req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
                  req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status),
                  SsmGetLastErrCode(), lErrMsg);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI52, lLogText);
            return MP_CONNECT_CMD_ERR;
         }

         if(SsmSetRecMixer(req.circuitId_1, true, 0))    
         {
            SsmGetLastErrMsg(lErrMsg);
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
                  "SsmSetRecMixer Record Patch Fail EC:%d EM:%s", lType, req.seqNum,
                  req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
                  req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status),
                  SsmGetLastErrCode(), lErrMsg);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI52, lLogText);
            return MP_CONNECT_CMD_ERR;
         }

         if(-1 == SsmRecToFile(req.circuitId_1, req.recordFile, 7, 0xfffffffe, 0xffffffff, 0xffffffff, 1))
         {
            SsmGetLastErrMsg(lErrMsg);
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
                  "SsmRecToFile Record Patch Fail EC:%d EM:%s", lType, req.seqNum,
                  req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
                  req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status),
                  SsmGetLastErrCode(), lErrMsg);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI52, lLogText);
            return MP_CONNECT_CMD_ERR;
         }

         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
               "SsmTalkWith Record Conversion Intiated", lType, req.seqNum,
               req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
               req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status));
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI53, lLogText);
      }
      //}
      //else
      //{
      //snprintf(lLogText, MAX_LOG_TEXT_LEN,
      //"%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s MC_CNTSP Can't Connect", lType, req.seqNum,
      //req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status), 
      //req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status));
      //TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      //gLog.GenerateLog(SAPI32, lLogText);
      //return MP_CKT_BUSY_ERR;
      //}

      return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : DisconnectSpeech
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::DisconnectSpeech(const MpSwitch req, TEXT *lType)
{
   synwayChannel*	pChnl_1 	= NULL;
   synwayChannel*	pChnl_2 	= NULL;
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";

   pChnl_1   = &mpSynwaySystem->channel[req.circuitId_1];
   pChnl_2   = &mpSynwaySystem->channel[req.circuitId_2];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "%s: RefNum:%08X CKTID:[%03d] CKTID:[%03d] Rcvd Tearing off Two-way Connection", lType,
         req.seqNum, req.circuitId_1 + 1, req.circuitId_2 + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););


   //if(pChnl_1->status == CKT_PATCHED && pChnl_2->status == CKT_PATCHED)
  //{
      if(SsmStopTalkWith(req.circuitId_1, req.circuitId_2))
      {
         SsmGetLastErrMsg(lErrMsg);
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
               "SsmStopTalkWith EC:%d EM:%s", lType, req.seqNum,
               req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
               req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status),
               SsmGetLastErrCode(), lErrMsg);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI54, lLogText);
         return MP_CONNECT_CMD_ERR;
      }

      if(0 == SsmCheckRecord (req.circuitId_1))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
               "SsmStopTalkWith Record Stopped in CIC:%03d", lType, req.seqNum,
               req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
               req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status),
               req.circuitId_1 + 1);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI101, lLogText);
      }

      if(0 == SsmCheckRecord (req.circuitId_2))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
               "SsmStopTalkWith Record Stopped in CIC:%03d", lType, req.seqNum,
               req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
               req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status),
               req.circuitId_2 + 1);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI101, lLogText);
      }

      //ChnlLock(req.circuitId_1);
      //pChnl_1->playStatus      = PLAY_NEXT_NOT_REQD;
      //pChnl_1->cktId           = req.circuitId_1;
      //pChnl_1->status          = CKT_IDLE;
      //ChnlUnlock(req.circuitId_1);

      //ChnlLock(req.circuitId_2);
      //pChnl_2->playStatus      = PLAY_NEXT_NOT_REQD;
      //pChnl_2->cktId           = req.circuitId_2;
      //pChnl_2->status          = CKT_IDLE;
      //ChnlUnlock(req.circuitId_2);


      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s "
            "SsmStopTalkWith Success", lType, req.seqNum,
            req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status),
            req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI55, lLogText);
   //}
   //else
   //{
      //snprintf(lLogText, MAX_LOG_TEXT_LEN,
            //"%s: RefNum:%08X CKTID:[%03d] ST:%s CKTID:[%03d] ST:%s MC_CNTSP Can't Disconnect", lType, req.seqNum,
            //req.circuitId_1 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_1->status), 
            //req.circuitId_2 + 1, SynwayVoiceMgrApi::ConvertStatusToStr(pChnl_2->status));
      //TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      //gLog.GenerateLog(SAPI32, lLogText);
      //return MP_CKT_BUSY_ERR;
   //}

   return MP_SUCCESS;
}


//-------------------------------------------------------------
// FUNCTION    : GetMediaChannelStatus
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : EnumChannelStatus
//-------------------------------------------------------------
EnumChannelStatus SynwayVoiceMgrApi::GetMediaChannelStatus(INT16 lMediaChnl, TEXT *lTxtStat)
{
   strcpy(lTxtStat, SynwayVoiceMgrApi::ConvertStatusToStr(mpSynwaySystem->channel[lMediaChnl].status));
   return mpSynwaySystem->channel[lMediaChnl].status;
}

//-------------------------------------------------------------
// FUNCTION    : SendIsupMsgToNw
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::SendIsupMsgToNw(GenIsupMsg &lGenIsupMsg)
{
   INT32                 lCic                         = lGenIsupMsg.circuitId;
   int                   lChnlState                   = -1;
   EnumSigChannelStatus  lSigChnlSt                   = SNW_CHNL_NOT_AVAILABLE;
   TEXT        	       lErrMsg[MAX_SYNWAY_ERR_MSG]  = "";
   TEXT        	       lLogText[MAX_LOG_TEXT_LEN] 	= "";

   switch(lGenIsupMsg.isupMsgType)
   {
      case GEN_ISUP_ACM:
         {
            BYTE btSigKB = 1;

            lSigChnlSt = mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].status;
            lChnlState = SsmGetChState(lGenIsupMsg.circuitId);

            if(0 /*S_CALL_STANDBY*/ ==  lChnlState)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Snd ACM Failed Inappropriate ChnlState CS:%d(%s) ProcCS:%d(%s) Snd REL to App",
                     lGenIsupMsg.circuitId + 1, lChnlState, SynwayUtil::ConvertChnlStToStr(lChnlState),
                     lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI59, lLogText);

               memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
               //Send REL/DISCONNECT to Application as ACM/ALERTING Failed
               lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
               lGenIsupMsg.circuitId = lCic;
               lGenIsupMsg.isupRel.cause = TEMP_FAILURE;

               return SNW_RET_ERR_FRW_APP; //return as msg has to relayed back to App
            }

            if(-1 == SsmSetKB(lGenIsupMsg.circuitId, btSigKB))
            {
               SsmGetLastErrMsg(lErrMsg);//Get error message
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Snd ACM/ALERTING(SsmSetKB) Failed, "
                     "Snd REL to App CS:%d EC:%d EM:%s", 
                     lGenIsupMsg.circuitId + 1, SsmGetChState(lGenIsupMsg.circuitId),
                     SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI59, lLogText);

               memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
               //Send REL/DISCONNECT to Application as ACM/ALERTING Failed
               lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
               lGenIsupMsg.circuitId = lCic;
               lGenIsupMsg.isupRel.cause = TEMP_FAILURE;

               return SNW_RET_ERR_FRW_APP; //return, msg has to relayed back to App
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:SIG CKTID:[%03d] Send ACM/ALERTING Success", lGenIsupMsg.circuitId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI60, lLogText);
         }
         break;
      case GEN_ISUP_ANM:
         {
            int nANX;

            ChnlLock(lGenIsupMsg.circuitId);
            lSigChnlSt = mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].status;
            lChnlState = SsmGetChState(lGenIsupMsg.circuitId);

            if(0 /*S_CALL_STANDBY*/ ==  lChnlState)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Snd ANM Failed Inappropriate ChnlState CS:%d(%s) ProcCS:%d(%s) Snd REL to App",
                     lGenIsupMsg.circuitId + 1, lChnlState, SynwayUtil::ConvertChnlStToStr(lChnlState),
                     lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI61, lLogText);

               memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
               //Send REL/DISCONNECT to Application as ANM/CONNECT Failed
               lGenIsupMsg.isupMsgType   = GEN_ISUP_REL;
               lGenIsupMsg.circuitId     = lCic;
               lGenIsupMsg.isupRel.cause = TEMP_FAILURE;

               ChnlUnlock(lGenIsupMsg.circuitId);
               return SNW_RET_ERR_FRW_APP; //return as msg has to relayed back to App
            }

            if(2/*S_CALL_RINGING*/ != lChnlState && 7/*S_CALL_PENDING*/ != lChnlState)
            {
               //Schedule the ANM Message
               gScheduledAnm[lGenIsupMsg.circuitId].isScheduled = true;
               gScheduledAnm[lGenIsupMsg.circuitId].schAnm = lGenIsupMsg.isupAnm;

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] ANM/CONNECT is Scheduled as Channel State is not Update State:%d",
                     lGenIsupMsg.circuitId + 1, lChnlState);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI89, lLogText);

               ChnlUnlock(lGenIsupMsg.circuitId);
               return SNW_RET_SUCCESS;
            }
            else
               ChnlUnlock(lGenIsupMsg.circuitId);

            if(TOA_NO_CHARGE == lGenIsupMsg.isupAnm.typeOfAns)
               nANX = 2;
            else if(TOA_CHARGE == lGenIsupMsg.isupAnm.typeOfAns)
               nANX = 1;
            else
               nANX = 0;
            if(-1 == SsmPickupANX(lGenIsupMsg.circuitId, nANX))
            {
               lErrMsg[0] = '\0';
               SsmGetLastErrMsg(lErrMsg);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Send ANM/CONNECT(SsmPickupANX) Failed, "
                     "Snd REL to App CS:%d EC:%d EM:%s",
                     lGenIsupMsg.circuitId + 1, SsmGetChState(lGenIsupMsg.circuitId), 
                     SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI61, lLogText);

               memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
               //Send REL/DISCONNECT to Application as ANM/CONNECT Failed
               lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
               lGenIsupMsg.circuitId = lCic;
               lGenIsupMsg.isupRel.cause = TEMP_FAILURE;

               return SNW_RET_ERR_FRW_APP; //return as msg has to relayed back to App
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:SIG CKTID:[%03d] Send ANM/CONNECT Success", lGenIsupMsg.circuitId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI62, lLogText);
         }
         break;
      case GEN_ISUP_REL:
         {
            DWORD dwValue;
            PVOID  lvoid = NULL;
            dwValue = lGenIsupMsg.isupRel.cause;

            lSigChnlSt = mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].status;
            lChnlState = SsmGetChState(lGenIsupMsg.circuitId);

            if(0 /*S_CALL_STANDBY*/ ==  lChnlState)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Snd REL Failed Inappropriate Chnl State CS:%d(%s) ProcCS:%d(%s)",
                     lGenIsupMsg.circuitId + 1, lChnlState, SynwayUtil::ConvertChnlStToStr(lChnlState),
                     lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI64, lLogText);

               return SNW_RET_ERR;
            }

            if(-1 == SsmSetIsupFlag(lGenIsupMsg.circuitId, 100 /*ISUP_REL_DENY_SetToOther*/, 
                     dwValue, &lvoid))
            {
               lErrMsg[0] = '\0';
               SsmGetLastErrMsg(lErrMsg);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] SsmSetIsupFlag Failed EC:%d EM:%s",
                     lGenIsupMsg.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI63, lLogText);
            }
            if(-1 == SsmHangup(lGenIsupMsg.circuitId))
            {
               lErrMsg[0] = '\0';
               SsmGetLastErrMsg(lErrMsg);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] (SsmHangup)REL Failed CS:%d EC:%d EM:%s",
                     lGenIsupMsg.circuitId + 1, SsmGetChState(lGenIsupMsg.circuitId), 
                     SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI64, lLogText);

               return SNW_RET_ERR;
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:SIG CKTID:[%03d] Send REL Success", lGenIsupMsg.circuitId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI65, lLogText);

            //Clear if any Scheduled ANM in present in that channel, after sending REL to n/w
            if(true == gScheduledAnm[lGenIsupMsg.circuitId].isScheduled)
            {
               gScheduledAnm[lGenIsupMsg.circuitId].isScheduled = false;
               memset(&gScheduledAnm[lGenIsupMsg.circuitId].schAnm, 0 ,sizeof(GenIsupAnm));
            }

            ChnlLock(lGenIsupMsg.circuitId);
            mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].status = SNW_WAIT_RLC;
            mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].statusUpdateTime = time(NULL);
            ChnlUnlock(lGenIsupMsg.circuitId);
         }
         break;
      case GEN_ISUP_IAM:
         {
            ChnlLock(lGenIsupMsg.circuitId);
            lChnlState = SsmGetChState(lGenIsupMsg.circuitId);

            lSigChnlSt = mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].status;

            if(SNW_CHNL_NOT_AVAILABLE == lSigChnlSt)
            {
               ChnlUnlock(lGenIsupMsg.circuitId);

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] IAM/SETUP Failed Chnl N/A ChnlSt:%d(%s) ProcCS:%d(%s) OA:%s DA:%s",
                     lGenIsupMsg.circuitId + 1, lChnlState, SynwayUtil::ConvertChnlStToStr(lChnlState),
                     lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt),
                     lGenIsupMsg.isupIam.callingNumber, lGenIsupMsg.isupIam.calledNumber);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI06, lLogText);

               FillRelAndClearScheduled(lCic, lGenIsupMsg, 0x22);/*No cic/chn available*/

               return SNW_RET_ERR_FRW_APP;
            }

            //if(121/*S_ISUP_WaitRLC*/ == lChnlState)
            if(SNW_CHNL_IDLE != lSigChnlSt)
            {
               //Schedule the IAM Message
               gScheduledIam[lGenIsupMsg.circuitId].isScheduled = true;
               gScheduledIam[lGenIsupMsg.circuitId].schIam = lGenIsupMsg.isupIam;

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] IAM/SETUP Scheduled ChnlSt:%d(%s) ProcCS:%d(%s) OA:%s DA:%s",
                     lGenIsupMsg.circuitId + 1, lChnlState, SynwayUtil::ConvertChnlStToStr(lChnlState),
                     lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt),
                     lGenIsupMsg.isupIam.callingNumber, lGenIsupMsg.isupIam.calledNumber);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI94, lLogText);

               ChnlUnlock(lGenIsupMsg.circuitId);
               return SNW_RET_SUCCESS;
            }
            else
            {
               ChnlUnlock(lGenIsupMsg.circuitId);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Before Iam/Setup Attempt ChnlSt:%d(%s) ProcCS:%d(%s) OA:%s DA:%s",
                     lGenIsupMsg.circuitId + 1, lChnlState, SynwayUtil::ConvertChnlStToStr(lChnlState),
                     lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt),
                     lGenIsupMsg.isupIam.callingNumber, lGenIsupMsg.isupIam.calledNumber);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI99, lLogText);
            }

            //Clear if any Scheduled ANM in present in that channel, before sending IAM to n/w
            if(true == gScheduledAnm[lGenIsupMsg.circuitId].isScheduled)
            {
               gScheduledAnm[lGenIsupMsg.circuitId].isScheduled = false;
               memset(&gScheduledAnm[lGenIsupMsg.circuitId].schAnm, 0 ,sizeof(GenIsupAnm));
            }

            if(SNW_RET_ERR_FRW_APP == DialNum(lGenIsupMsg))
            {
               return SNW_RET_ERR_FRW_APP;
            }
         }
         break;
      case GEN_ISUP_RLC:
         {
            lSigChnlSt = mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].status;
            lChnlState = SsmGetChState(lGenIsupMsg.circuitId);

            if(0 /*S_CALL_STANDBY*/ ==  lChnlState)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Snd RLC Failed Inappropriate Chnl State CS:%d(%s) ProcCS:%d(%s)",
                     lGenIsupMsg.circuitId + 1, lChnlState, SynwayUtil::ConvertChnlStToStr(lChnlState),
                     lSigChnlSt, SynwayUtil::ConvertProcSigChnlStToStr(lSigChnlSt));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI57, lLogText);

               return SNW_RET_ERR;
            }

            if(-1 == SsmHangup(lGenIsupMsg.circuitId))
            {
               lErrMsg[0] = '\0';
               SsmGetLastErrMsg(lErrMsg);
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:SIG CKTID:[%03d] Snd RLC Failed(SsmHangup) Failed CS:%d EC:%d EM:%s",
                     lGenIsupMsg.circuitId + 1, SsmGetChState(lGenIsupMsg.circuitId),
                     SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI57, lLogText);

               return SNW_RET_ERR;
            }
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:SIG CKTID:[%03d] SsmHangup snd:RLC Success",
                  lGenIsupMsg.circuitId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI58, lLogText);

            //Clear if any Scheduled ANM in present in that channel, after sending RLC to n/w
            if(true == gScheduledAnm[lGenIsupMsg.circuitId].isScheduled)
            {
               gScheduledAnm[lGenIsupMsg.circuitId].isScheduled = false;
               memset(&gScheduledAnm[lGenIsupMsg.circuitId].schAnm, 0 ,sizeof(GenIsupAnm));
            }
         }
         break;
      case GEN_ISUP_RSC:
      case GEN_ISUP_CPG:
      case GEN_ISUP_UNKNOWN:
      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:SIG CKTID:[%03d] IsupMsg Not Handled", lGenIsupMsg.circuitId + 1);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI68, lLogText);
         return SNW_RET_ERR;
   }

   return SNW_RET_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : Pause
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Pause(MpPause lpause, TEXT *lType)
{
   TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT           lLogText[MAX_LOG_TEXT_LEN] 		= "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "%s: RefNum:%08X CKTID:[%03d] Rcvd MC_PAUSE ", lType,
         lpause.seqNum, lpause.circuitId + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   if(-1 == SsmPausePlay(lpause.circuitId))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] MC_PAUSE "
            "SsmPausePlay Failed EC:%d EM:%s", lType,
            lpause.seqNum, lpause.circuitId + 1, 
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI69, lLogText);

      return MP_PAUSE_CMD_ERR;
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : Resume
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Resume(MpResume lresume, TEXT *lType)
{
   TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT           lLogText[MAX_LOG_TEXT_LEN] 		= "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "%s: RefNum:%08X CKTID:[%03d] Rcvd MC_RESUME", lType,
         lresume.seqNum, lresume.circuitId + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   if(-1 == SsmRestartPlay(lresume.circuitId))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] MC_RESUME "
            "SsmRestartPlay Failed EC:%d EM:%s", lType,
            lresume.seqNum, lresume.circuitId + 1,
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI70, lLogText);

      return MP_RSUME_CMD_ERR;
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : Forward
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Forward(MpForward lforward, TEXT *lType)
{
   TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT           lLogText[MAX_LOG_TEXT_LEN] 		= "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "%s: RefNum:%08X CKTID:[%03d] Rcvd MC_FORWARD", lType,
         lforward.seqNum, lforward.circuitId + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   if(-1 == SsmFastFwdPlay(lforward.circuitId))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] MC_FORWARD "
            "SsmFastFwdPlay Failed EC:%d EM:%s", lType,
            lforward.seqNum, lforward.circuitId + 1, 
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI71, lLogText);

      return MP_FORWARD_CMD_ERR;
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : Rewind
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : 
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr SynwayVoiceMgrApi::Rewind(MpRewind lRewind, TEXT *lType)
{
   TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT           lLogText[MAX_LOG_TEXT_LEN] 		= "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "%s: RefNum:%08X CKTID:[%03d] Rcvd MC_FORWARD", lType,
         lRewind.seqNum, lRewind.circuitId + 1);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   if(-1 == SsmFastBwdPlay(lRewind.circuitId))
   {
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CKTID:[%03d] MC_FORWARD "
            "SsmFastBwdPlay Failed EC:%d EM:%s", lType,
            lRewind.seqNum, lRewind.circuitId + 1,
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI72, lLogText);
      return MP_REWIND_CMD_ERR;
   }

   return MP_SUCCESS;
}


//-------------------------------------------------------------
// FUNCTION    : SendChnlMgmtMsg
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::SendChnlMgmtMsg(CktCmdReq &lCktCmdReq)
{
   TEXT        	lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";
   int            lCurChnlstate = -1;


   switch(lCktCmdReq.cktCmd)
   {
      case CKT_CMND_BLOCK:
         {
            if(-1 == (lCurChnlstate = SsmGetChState(lCktCmdReq.channelId)))
            {
               SsmGetLastErrMsg(lErrMsg);//Get error message
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:CM CKTID:[%03d] Error Getting Channel State Failed EC:%d EM:%s",
                     lCktCmdReq.channelId + 1, SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI80, lLogText);
               return false;
            }
            if(123 /*S_ISUP_LocallyBlocked*/ == lCurChnlstate)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:CM CKTID:[%03d] Alread in Block State", lCktCmdReq.channelId + 1);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI81, lLogText);
               return false;
            }

            if(-1 == SsmBlockRemoteCh(lCktCmdReq.channelId)) 
            {
               SsmGetLastErrMsg(lErrMsg);//Get error message
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:CM CKTID:[%03d] Issue Block Channel(SsmBlockRemoteCh) Failed EC:%d EM:%s", 
                     lCktCmdReq.channelId + 1, SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI73, lLogText);
               return false;;
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:CM CKTID:[%03d] Issue Block Channel Cmd Success", lCktCmdReq.channelId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI74, lLogText);
         }
         break;
      case CKT_CMND_UNBLOCK:
         {
            if(-1 == (lCurChnlstate = SsmGetChState(lCktCmdReq.channelId)))
            {
               SsmGetLastErrMsg(lErrMsg);//Get error message
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:CM CKTID:[%03d] Error Getting Channel State Failed EC:%d EM:%s",
                     lCktCmdReq.channelId + 1, SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI80, lLogText);
               return false;
            }

            if(123 /*S_ISUP_LocallyBlocked*/ != lCurChnlstate)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:CM CKTID:[%03d] Alread in Unblock State", lCktCmdReq.channelId + 1);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI82, lLogText);
               return false;
            }

            if(-1 == SsmUnblockRemoteCh(lCktCmdReq.channelId))
            {
               SsmGetLastErrMsg(lErrMsg);//Get error message
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:CM CKTID:[%03d] Issue Unblock Channel(SsmUnbockRemoteCh) Failed EC:%d EM:%s",
                     lCktCmdReq.channelId + 1, SsmGetLastErrCode(), lErrMsg);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI76, lLogText);
               return false;
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:CM CKTID:[%03d] Issue Unblock Channel Cmd Success", lCktCmdReq.channelId + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI76, lLogText);
         }
         break;
      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "TH:CM CKTID:[%03d] Channel Mgmt Msg Not Handled", lCktCmdReq.channelId + 1);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(SAPI77, lLogText);
         return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : ChnlMgmtEvents
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::ChnlMgmtEvents(CktCmdRsp &lCktCmdRsp, MESSAGE_INFO &lEvent)
{
   unsigned int   lTemp_1;
   unsigned int   lTemp_2;
   TEXT        	lLogText[MAX_LOG_TEXT_LEN] 		= "";


   switch(lEvent.wEventCode)
   {
      case 0x001A:/*E_CHG_RemoteChBlock*/
         {
            switch(lEvent.dwParam)
            {
               case 0:/*The block caused by the local end successfully unblocked*/
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:C Event Generated CKTID:[%03d] Channel Unblocked", 
                           lEvent.nReference + 1);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI78, lLogText);

                     lCktCmdRsp.status = CKT_CMND_UNBLOCKED;
                     lCktCmdRsp.channelId = lEvent.nReference;
                  }
                  return 1;
               case 1:/*The remote end successfully blocked*/
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:C Event Generated CKTID:[%03d] Channel Blocked",
                           lEvent.nReference + 1);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(SAPI79, lLogText);

                     lCktCmdRsp.status = CKT_CMND_BLOCKED;
                     lCktCmdRsp.channelId = lEvent.nReference;
                  }
                  return 1;
               case 2:/*Waiting for the block-verified signal from the remote PBX*/
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:C Event Generated CKTID:[%03d] Channel "
                           "Waiting for the block from the remote PBX", 
                           lEvent.nReference + 1);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  }
                  break;
               case 3:/*Waiting for the unblock-verified signal from the remote PBX*/
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "TH:EV:C Event Generated CKTID:[%03d] Channel "
                           "Waiting for the unblock from the remote PBX",
                           lEvent.nReference + 1);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  }
                  break;
            }
         }
         break;
      case 0x001B:/*E_CHG_RemotePCMBlock*/
         {
            lTemp_1 = lEvent.dwParam & 0x00FF;
            lTemp_2 = lEvent.dwParam >> 16 ;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:C Event Generated PCM:%d Block/Unblock Status Changed status:%d",
                  lEvent.nReference, lTemp_1);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         }
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "TH:EV:C Event Generated EC:0x%04X CKTID:[%03d] dwParam:%u Not Processed",
                  lEvent.wEventCode , lEvent.nReference + 1, lEvent.dwParam);
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(SAPI31, lLogText);
         }
   }

   return 0;
}

//-------------------------------------------------------------
// FUNCTION    : DialNum
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : INT16
// THROWS      : None
//-------------------------------------------------------------
INT16 SynwayVoiceMgrApi::DialNum(GenIsupMsg &lGenIsupMsg)
{
   INT32          lCic                         = lGenIsupMsg.circuitId;
   CHAR           szPhoNum[25]                     = "";
   CHAR           pszTxCallerId[25]                = "";
// DWORD          dwValue;
// PVOID          lvoid                            = NULL;
   TEXT           lErrMsg[MAX_SYNWAY_ERR_MSG]      = "";
   TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";

   strcpy(szPhoNum, lGenIsupMsg.isupIam.calledNumber);
   strcpy(pszTxCallerId, lGenIsupMsg.isupIam.callingNumber);

   // TBD: To be derived from IAM Calling Number Format
   //dwValue = 0x1004;
   //if(-1 == SsmSetIsupFlag(lGenIsupMsg.circuitId, 1/*ISUP_CallerParam*/, 
   //         dwValue, &lvoid))
   //{	
   //   lErrMsg[0] = '\0';
   //   SsmGetLastErrMsg(lErrMsg);
   //   snprintf(lLogText, MAX_LOG_TEXT_LEN,
   //         "TH:SIG CKTID:[%03d] SsmSetIsupFlag for ISUP_CallerParam Failed EC:%d EM:%s",
   //         lGenIsupMsg.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
   //   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   //   gLog.GenerateLog(SAPI63, lLogText);
   //}
   if(-1 == SsmSetTxCallerId(lGenIsupMsg.circuitId, pszTxCallerId))
   {
      lErrMsg[0] = '\0';
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:SIG CKTID:[%03d] SsmSetTxCallerId Failed OA:%s EC:%d EM:%s",
            lGenIsupMsg.circuitId + 1, pszTxCallerId,
            SsmGetLastErrCode(), lErrMsg);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI63, lLogText);
   }

   // TBD: To be derived from IAM Called Number Format
   //dwValue = 0x1004;
   //if(-1 == SsmSetIsupFlag(lGenIsupMsg.circuitId, 2/*ISUP_PhoNumParam*/, 
   //         dwValue, &lvoid))
   //{	
   //   lErrMsg[0] = '\0';
   //   SsmGetLastErrMsg(lErrMsg);
   //   snprintf(lLogText, MAX_LOG_TEXT_LEN,
   //         "TH:SIG CKTID:[%03d] SsmSetIsupFlag for ISUP_PhoNumParam Failed EC:%d EM:%s",
   //         lGenIsupMsg.circuitId + 1, SsmGetLastErrCode(), lErrMsg);
   //   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   //   gLog.GenerateLog(SAPI63, lLogText);
   //}

   if(-1 == SsmAutoDial(lGenIsupMsg.circuitId, szPhoNum))
   {
      lErrMsg[0] = '\0';
      SsmGetLastErrMsg(lErrMsg);
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:SIG CKTID:[%03d] (SsmAutoDial) Send IAM Failed, "
            "Snd REL to App DA:%s CS:%d EC:%d EM:%s OA:%s DA:%s",
            lGenIsupMsg.circuitId + 1, szPhoNum, SsmGetChState(lGenIsupMsg.circuitId),
            SsmGetLastErrCode(), lErrMsg,
            pszTxCallerId, szPhoNum);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(SAPI66, lLogText);

      memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
      //Send REL/DISCONNECT to Application as IAM/SETUP Failed
      lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
      lGenIsupMsg.circuitId = lCic;
      lGenIsupMsg.isupRel.cause = 0x22;/*No cic/chn available*/

      return SNW_RET_ERR_FRW_APP;
   }

   ChnlLock(lGenIsupMsg.circuitId);
   mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].status = SNW_IAM_SENT;
   mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].callDirection = SNW_OUT_BOUND_CALL;
   mpSynwaySystem->sigChannel[lGenIsupMsg.circuitId].statusUpdateTime = time(NULL);
   ChnlUnlock(lGenIsupMsg.circuitId);

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "TH:SIG CKTID:[%03d] OA:%s DA:%s Send IAM Success", 
         lGenIsupMsg.circuitId + 1, pszTxCallerId, szPhoNum);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SAPI67, lLogText);

   return SNW_RET_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : FillRelAndClearScheduled
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
void SynwayVoiceMgrApi::FillRelAndClearScheduled(int cic, GenIsupMsg &genIsupMsg, int cause)
{
   //Send Release to Application
   genIsupMsg.isupMsgType = GEN_ISUP_REL;
   genIsupMsg.circuitId = cic;

   if(0 >= cause)
      genIsupMsg.isupRel.cause = TEMP_FAILURE;
   else
      genIsupMsg.isupRel.cause = cause;

   gScheduledIam[cic].isScheduled = false;
   memset(&gScheduledIam[cic].schIam, 0,sizeof(GenIsupIam));

   gScheduledAnm[cic].isScheduled = false;
   memset(&gScheduledAnm[cic].schAnm, 0 ,sizeof(GenIsupAnm));
}

//-------------------------------------------------------------
// FUNCTION    : GetSignalingLinkStatus
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
void SynwayVoiceMgrApi::GetSignalingLinkStatus()
{
   int linkState=0;
   ofstream mReportFile;
   TEXT  lLinkstatusFileName[SYNWAY_MAX_FILE_PATH_LEN+50];
   TEXT  lTimeString[20];
   struct timeval time_ptr;
   struct tm      *time_tm;
   time_t         t;

   gettimeofday(&time_ptr, 0);
   t = time_ptr.tv_sec;
   time_tm = localtime(&t);

   sprintf(lLinkstatusFileName, "%s/LinkStatus_%04d_%02d_%02d", mLinkStatusFile, 1900 + time_tm->tm_year, 1 + time_tm->tm_mon, time_tm->tm_mday);
   sprintf(lTimeString, "%02d:%02d:%02d ",  time_tm->tm_hour, time_tm->tm_min, time_tm->tm_sec);

   mReportFile.open(lLinkstatusFileName, ios::out | ios::app);

   if (mReportFile == NULL)
   {
      mReportFile.close();
      TERR(gTrace, printf("%s: %s unable to open file\n", gProcessName, lLinkstatusFileName););
      return ;
   }
   else
   {
      T(gTrace, printf("%s: Status File %s open successfully \n", gProcessName, lLinkstatusFileName););
   }

   for (int i=0; i < 8; i++)
   {
      linkState = SsmGetMtp2Status(i);
      if (linkState != -1)
      {
         mReportFile << lTimeString << "Signaling Link [" << i << "] : State - " << ConvertLinkStateToStr(linkState) << endl;
      }
   }
   mReportFile.close();
}

//-------------------------------------------------------------
// FUNCTION    : ConvertLinkStateToStr
// DESCRIPTION :
// PARAMETER   : linkState
// RETURN      : TEXT *
// THROWS      : None
//-------------------------------------------------------------
TEXT* SynwayVoiceMgrApi::ConvertLinkStateToStr(int linkState)
{
   switch (linkState)
   {
      case 0:
         return (TEXT*)"DSP Uploaded";
      case 1:
         return (TEXT*)"Service Interrupted";
      case 2:
         return (TEXT*)"Initial Framing";
      case 3:
         return (TEXT*)"Framed/Ready";
      case 4:
         return (TEXT*)"Framed/NotReady";
      case 5:
         return (TEXT*)"Service Open";
      case 6:
         return (TEXT*)"Processor Failure";
      case -1:
         return (TEXT*)"Failed";
   }
}

//-------------------------------------------------------------
// FUNCTION    : CheckMediaChannelStatus
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOELAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::CheckMediaChannelStatus(int chnl, MpResponseDetails &rsp)
{
   TEXT        lTmpDtmfDgts[45] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";
   time_t      nowTime = time(NULL);

   if(chnl > mpSynwaySystem->mMaxChannel)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:TM CKTID:[%03d] Channel Error", chnl);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   MpData lTempMpData;
   memset(&lTempMpData, 0, sizeof(MpData));

   switch(mpSynwaySystem->channel[chnl].status)
   {
      case CKT_PLAYING:
         {
            if((nowTime - mpSynwaySystem->channel[chnl].mediaCompTime) > (mFilePlayPrfChkIntr + SYNWAY_MEDIA_TIMEDOUT_BUFFER))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] CKT_PLAYING BLOCKED CompTm:%lu NowTm:%lu Issuing PlayStop",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               lTempMpData.stopMsg.seqNum = mpSynwaySystem->channel[chnl].currentPlay.seqNum;
               lTempMpData.stopMsg.circuitId = chnl;

               if(MP_SUCCESS != Stop(lTempMpData.stopMsg, "TH:TM"))
               {
                  rsp.rspMsg              = MP_RSP_FAILURE;
                  rsp.cmdType             = MC_DISTN;
                  rsp.mpRspData.stopMsg.seqNum     = mpSynwaySystem->channel[chnl].currentPlay.seqNum;
                  rsp.mpRspData.stopMsg.circuitId  = mpSynwaySystem->channel[chnl].currentPlay.circuitId;

                  ChnlLock(chnl);
                  mpSynwaySystem->channel[chnl].status = CKT_IDLE;
                  mpSynwaySystem->channel[chnl].playStatus   = PLAY_NEXT_NOT_REQD;
                  mpSynwaySystem->channel[chnl].mediaCompTime = 0;
                  ChnlUnlock(chnl);

                  return true;
               }
            }
         }
         break;
      case CKT_PLAY_STOPING:
         {
            if((nowTime - mpSynwaySystem->channel[chnl].mediaCompTime) > SYNWAY_STOP_EVENT_TIMEDOUT)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] CKT_PLAY_STOPING BLOCKED CompTm:%lu NowTm:%lu",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               rsp.rspMsg              = MP_RSP_FAILURE;
               rsp.cmdType             = MC_DISTN;
               rsp.mpRspData.stopMsg.seqNum     = mpSynwaySystem->channel[chnl].currentPlay.seqNum;
               rsp.mpRspData.stopMsg.circuitId  = mpSynwaySystem->channel[chnl].currentPlay.circuitId;

               ChnlLock(chnl);
               mpSynwaySystem->channel[chnl].status = CKT_IDLE;
               mpSynwaySystem->channel[chnl].playStatus   = PLAY_NEXT_NOT_REQD;
               mpSynwaySystem->channel[chnl].mediaCompTime = 0;
               ChnlUnlock(chnl);

               return true;
            }
         }
         break;
      case CKT_RECORD_STOPING:
         {
            if((nowTime - mpSynwaySystem->channel[chnl].mediaCompTime) > SYNWAY_STOP_EVENT_TIMEDOUT)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] CKT_RECORD_STOPING BLOCKED CompTm:%lu NowTm:%lu",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               rsp.rspMsg              = MP_RSP_FAILURE;
               rsp.cmdType             = MC_RECSTP;
               rsp.mpRspData.recordMsg   = mpSynwaySystem->channel[chnl].currentRecord;

               ChnlLock(chnl);
               mpSynwaySystem->channel[chnl].status = CKT_IDLE;
               mpSynwaySystem->channel[chnl].mediaCompTime = 0;
               ChnlUnlock(chnl);

               return true;
            }
         }
         break;
      case CKT_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
         {
            if(nowTime > (mpSynwaySystem->channel[chnl].mediaCompTime + SYNWAY_GETDIG_TIMEDOUT_BUFFER))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] CKT_GETDIG BLOCKED CompTm:%lu NowTm:%lu",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               rsp.cmdType = MC_GETDIGSTP;
               rsp.mpRspData.getDigit = mpSynwaySystem->channel[chnl].currentGetdigit;
               if(true != CancelWaitDtmf(chnl, mpSynwaySystem->channel[chnl].currentGetdigit.seqNum,(TEXT*) "TH:TM",
                        lTmpDtmfDgts, true))
               {
                  rsp.rspMsg = MP_RSP_FAILURE;
               }
               else
               {
                  rsp.rspMsg = MP_RSP_SUCCESS;
                  strcat(rsp.mpRspData.getDigit.rspDigits, lTmpDtmfDgts);
               }

               ChnlLock(chnl);
               mpSynwaySystem->channel[chnl].status = CKT_IDLE;
               mpSynwaySystem->channel[chnl].mediaCompTime = 0;
               ChnlUnlock(chnl);

               return true;
            }
         }
         break;
      case CKT_RECORDING:
         {
            if((nowTime - mpSynwaySystem->channel[chnl].mediaCompTime) > (mFileRecordPrfChkIntr + SYNWAY_MEDIA_TIMEDOUT_BUFFER))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] CKT_PLAYING BLOCKED CompTm:%lu NowTm:%lu Issuing RecordStop",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               lTempMpData.stopMsg.seqNum = mpSynwaySystem->channel[chnl].currentRecord.seqNum;
               lTempMpData.stopMsg.circuitId = chnl;

               if(MP_SUCCESS != StopRecord(lTempMpData.stopMsg, "TH:TM"))
               {
                  rsp.rspMsg              = MP_RSP_FAILURE;
                  rsp.cmdType             = MC_RECSTP;
                  rsp.mpRspData.recordMsg   = mpSynwaySystem->channel[chnl].currentRecord;

                  ChnlLock(chnl);
                  mpSynwaySystem->channel[chnl].status = CKT_IDLE;
                  mpSynwaySystem->channel[chnl].playStatus   = PLAY_NEXT_NOT_REQD;
                  mpSynwaySystem->channel[chnl].mediaCompTime = 0;
                  ChnlUnlock(chnl);

                  return true;
               }
            }
         }
         break;
      case CKT_PLAYING_AND_GETDIG:
         {
            if(nowTime > (mpSynwaySystem->channel[chnl].mediaCompTime + SYNWAY_GETDIG_TIMEDOUT_BUFFER))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] CKT_PLAYING_AND_GETDIG BLOCKED CompTm:%lu NowTm:%lu",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               lTempMpData.stopMsg.seqNum = mpSynwaySystem->channel[chnl].currentPlay.seqNum;
               lTempMpData.stopMsg.circuitId = chnl;

               Stop(lTempMpData.stopMsg, "TH:TM");

               rsp.cmdType = MC_GETDIGSTP;
               rsp.mpRspData.getDigit = mpSynwaySystem->channel[chnl].currentGetdigit;
               if(true != CancelWaitDtmf(chnl, mpSynwaySystem->channel[chnl].currentGetdigit.seqNum,(TEXT*) "TH:TM",
                        lTmpDtmfDgts, true))
               {
                  rsp.rspMsg = MP_RSP_FAILURE;
               }
               else
               {
                  rsp.rspMsg = MP_RSP_SUCCESS;
                  strcat(rsp.mpRspData.getDigit.rspDigits, lTmpDtmfDgts);
               }

               ChnlLock(chnl);
               mpSynwaySystem->channel[chnl].status = CKT_IDLE;
               mpSynwaySystem->channel[chnl].mediaCompTime = 0;
               ChnlUnlock(chnl);

               return true;
            }
         }
         break;
      case CKT_RECORDING_AND_GETDIG:
         {
            if(nowTime > (mpSynwaySystem->channel[chnl].mediaCompTime + SYNWAY_GETDIG_TIMEDOUT_BUFFER))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] CKT_RECORDING_AND_GETDIG BLOCKED CompTm:%lu NowTm:%lu",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               lTempMpData.stopMsg.seqNum = mpSynwaySystem->channel[chnl].currentRecord.seqNum;
               lTempMpData.stopMsg.circuitId = chnl;

               Stop(lTempMpData.stopMsg, "TH:TM");

               rsp.cmdType = MC_GETDIGSTP;
               rsp.mpRspData.getDigit = mpSynwaySystem->channel[chnl].currentGetdigit;
               if(true != CancelWaitDtmf(chnl, mpSynwaySystem->channel[chnl].currentGetdigit.seqNum,(TEXT*) "TH:TM",
                        lTmpDtmfDgts, true))
               {
                  rsp.rspMsg = MP_RSP_FAILURE;
               }
               else
               {
                  rsp.rspMsg = MP_RSP_SUCCESS;
                  strcat(rsp.mpRspData.getDigit.rspDigits, lTmpDtmfDgts);
               }

               ChnlLock(chnl);
               mpSynwaySystem->channel[chnl].status = CKT_IDLE;
               mpSynwaySystem->channel[chnl].mediaCompTime = 0;
               ChnlUnlock(chnl);

               return true;
            }
         }
         break;
      case CKT_PATCHED:
      case CKT_UNAVAILABLE:
      case CKT_IDLE:
         break;
      default:
            if(nowTime > (5 * SYNWAY_MEDIA_TIMEDOUT_BUFFER))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] UNKNOWN STATE BLOCKED CompTm:%lu NowTm:%lu",
                     chnl + 1, mpSynwaySystem->channel[chnl].mediaCompTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               ChnlLock(chnl);
               mpSynwaySystem->channel[chnl].status = CKT_IDLE;
               mpSynwaySystem->channel[chnl].mediaCompTime = 0;
               ChnlUnlock(chnl);
            }
   }

   return false;
}

//-------------------------------------------------------------
// FUNCTION    : CheckSignalStatus
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOELAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayVoiceMgrApi::CheckSignalStatus(int chnl, GenIsupMsg &lGenIsupMsg)
{
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";
   time_t      nowTime = time(NULL);

   switch(mpSynwaySystem->sigChannel[chnl].status)
   {
      case SNW_IAM_SENT:
      case SNW_WAIT_DAIL_ANS:
      case SNW_DIALING:
      case SNW_WAIT_REMOTE_PICKUP:
      case SNW_ANM_RCVD:
      case SNW_IAM_RCVD:
      case SNW_WAIT_ACM_SND:
      case SNW_RINGING:
      case SNW_WAIT_RLC:
      case SNW_REMOTE_HANGUP:
      case SNW_WAIT_REL_SND:
         {
            if(nowTime > (mpSynwaySystem->sigChannel[chnl].statusUpdateTime + (mMaxCallDur * 60)))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "TH:TM Event CKTID:[%03d] %s CALL STATE BLOCKED StUpdateTm:%lu NowTm:%lu",
                     chnl + 1, SynwayUtil::ConvertProcSigChnlStToStr(mpSynwaySystem->sigChannel[chnl].status),
                     mpSynwaySystem->sigChannel[chnl].statusUpdateTime, nowTime);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(SAPI102, lLogText);

               lGenIsupMsg.isupMsgType = GEN_ISUP_REL;
               lGenIsupMsg.isupRel.cause = TEMP_FAILURE;

               ChnlLock(chnl);
               mpSynwaySystem->sigChannel[chnl].status = SNW_CHNL_NOT_AVAILABLE;
               mpSynwaySystem->sigChannel[chnl].callDirection = SNW_NO_CALL;
               mpSynwaySystem->sigChannel[chnl].statusUpdateTime = 0;
               ChnlUnlock(chnl);

               return true;
            }
         }
         break;
      case SNW_TALKING:
      case SNW_CHNL_NOT_AVAILABLE:
      case SNW_CHNL_IDLE:
      default:
         {
            ;
         }
   }

   return false;
}

