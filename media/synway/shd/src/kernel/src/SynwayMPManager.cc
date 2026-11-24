static const char* id="@(#) $Id: SynwayMPManager.cc,v 1.1.4.3 2016/04/12 21:59:39 shijuc Exp $" ;
//-----------------------------------------------------------------------------
// NAME           : SynwayMPManager.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2009 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Chethan V M           Date:
//----------------------------------------------------------------------------

#include "SynwayMPManager.h"

extern SynwayMPManager   gSynwayMPManagerObj;
SynwayVoiceMgrApi			 gSynwayVoiceMgrApiObj;

void* StartEventThread(void* ptr);
void* StartSignalingThread(void* ptr);
void* StartChnlMgmtThread(void* ptr);
void* StartChnlTimerThread(void* ptr);

//-------------------------------------------------------------
// FUNCTION    : SynwayMPManager
// DESCRIPTION : Constructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
SynwayMPManager::SynwayMPManager()
{
   mSendRspFlag = false;
   memset(&mMpReq, 			0, sizeof(MpRequestDetails));
	memset(mInPutShCfgFile, 0, sizeof(mInPutShCfgFile));
	memset(mInPutShIdxFile, 0, sizeof(mInPutShIdxFile));

}

//-------------------------------------------------------------
// FUNCTION    : SynwayMPManager
// DESCRIPTION : Destructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
SynwayMPManager::~SynwayMPManager()
{
}

//-------------------------------------------------------------
// FUNCTION    : Init
// DESCRIPTION : Initializes member attributes
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::Init()
{
   TEXT      lLogText[MAX_LOG_TEXT_LEN];

   // Read the configuration
   if( false == ReadConfig())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "ReadConfig failed");
      TERR(gTrace, printf("%s: %s. Exiting...\n", gProcessName, lLogText);)
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   // Create Queues
   if( false == CreateQueues())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CreateQueues failed");
      TERR(gTrace, printf("%s: %s. Exiting...\n", gProcessName, lLogText);)
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }
   // Create Queues

   // Initialize Channels
   if ( false == InitChannel())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Initialising Channels failed");
      TERR(gTrace, printf("%s: %s. Exiting...\n", gProcessName, lLogText);)
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ReadConfig()
{
   CfgRead           lCfgRead(gProcessName);
   ConfigReturn      lCfgRet;
	TEXT 				   lLogText[MAX_LOG_TEXT_LEN] = "";

   if (false == lCfgRead.CfgInit(SYNWAY_IPC_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n",
                           gProcessName, lLogText););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }
   
   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MSG_VOICE_PROCESS_REQ_Q",
                                                   mReqMsgQKey,
                                                   SYNWAY_MIN_SEM_KEY,
                                                   SYNWAY_MAX_SEM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Error in Reading Parameter %s : %s in %s", 
            "MSG_VOICE_PROCESS_REQ_Q", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_VOICE_PROCESS_REQ_Q = %d\n", gProcessName, mReqMsgQKey););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MSG_ISUP_HDLR_REQ_Q_RCV",
               mIsupMsgReqKey,
               SYNWAY_MIN_SEM_KEY,
               SYNWAY_MAX_SEM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Error in Reading Parameter %s : %s in %s",
            "MSG_ISUP_HDLR_REQ_Q_RCV", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_ISUP_HDLR_REQ_Q_RCV = %d\n", gProcessName, mIsupMsgReqKey););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MSG_CHANNEL_MGMT_HDLR_REQ_Q_RCV",
               mChnlMgmtMsgReqKey,
               SYNWAY_MIN_SEM_KEY,
               SYNWAY_MAX_SEM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Error in Reading Parameter %s : %s in %s",
            "MSG_CHANNEL_MGMT_HDLR_REQ_Q_RCV", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_CHANNEL_MGMT_HDLR_REQ_Q_RCV = %d\n", gProcessName, mChnlMgmtMsgReqKey););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MSG_ISUP_HDLR_RSP_Q_RCV",
               mIsupMsgRspKey,
               SYNWAY_MIN_SEM_KEY,
               SYNWAY_MAX_SEM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Error in Reading Parameter %s : %s in %s",
            "MSG_ISUP_HDLR_RSP_Q_RCV", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_ISUP_HDLR_RSP_Q_RCV = %d\n", gProcessName, mIsupMsgRspKey););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MSG_CHANNEL_MGMT_HDLR_RSP_Q_RCV",
               mChnlMgmtRspKey,
               SYNWAY_MIN_SEM_KEY,
               SYNWAY_MAX_SEM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Error in Reading Parameter %s : %s in %s",
            "MSG_CHANNEL_MGMT_HDLR_RSP_Q_RCV", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_CHANNEL_MGMT_HDLR_RSP_Q_RCV = %d\n", gProcessName, mChnlMgmtRspKey););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MSG_VOICE_PROCESS_RSP_Q",
               mRspMsgQKey,
               SYNWAY_MIN_SEM_KEY,
               SYNWAY_MAX_SEM_KEY)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s", 
            "MSG_VOICE_PROCESS_RSP_Q", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_IPC_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MSG_VOICE_PROCESS_RSP_Q = %d\n", gProcessName, mRspMsgQKey);)
   }

   lCfgRead.CfgDeInit();

   // Read all Tone server configurations
   if (false == lCfgRead.CfgInit(SYNWAY_MEDIA_CFG) )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, SYNWAY_MEDIA_CFG););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }

   // Read Flag to send back response   
   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MEDIA_SEND_RSP_FLAG", mSendRspFlag, 0, 1)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s", 
            "MEDIA_SEND_RSP_FLAG", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MEDIA_SEND_RSP_FLAG = %d\n", gProcessName, mSendRspFlag););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MAX_CHANNEL_PER_PROCESS", mMaxChannel, 0, MAX_CHANNEL_PER_PRS)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "MAX_CHANNEL_PER_PROCESS", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: MAX_CHANNEL_PER_PROCESS = %d\n", gProcessName, mMaxChannel););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("RUN_SYNWAY_SIGNALLING_THREAD", mRunSigThread, 0, 1)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "RUN_SYNWAY_SIGNALLING_THREAD", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: RUN_SYNWAY_SIGNALLING_THREAD = %d\n", gProcessName, mRunSigThread););
   }

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("RUN_SYNWAY_CHANNEL_MGMT_THREAD", mRunChnlMgmtThread, 0, 1)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "RUN_SYNWAY_CHANNEL_MGMT_THREAD", SynwayUtil::GetConfigError(lCfgRet), SYNWAY_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
   {
      T(gTrace, printf("%s: RUN_SYNWAY_CHANNEL_MGMT_THREAD = %d\n", gProcessName, mRunChnlMgmtThread););
   }

   lCfgRead.CfgDeInit();

   return true;
}
//----------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ReloadConfig()
{
   //CfgRead           lCfgRead(gProcessName);
   //ConfigReturn      lCfgRet;

   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateQueues
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::CreateQueues()
{
   msgQueueError lQueueError;
   TEXT			lLogText[MAX_LOG_TEXT_LEN] = "";

   lQueueError = mReqMsgQue.Create(mReqMsgQKey, IPC_CREAT| IPC_PERM);
   if (Q_SUCCESS != lQueueError)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: Queue Creation with key %u failed\n", 
            gProcessName, mReqMsgQKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         SynwayUtil::HandleQueueCreateError(lQueueError, mReqMsgQue);
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }

   lQueueError = mIsupMsgReqQ.Create(mIsupMsgReqKey, IPC_CREAT| IPC_PERM);
   if (Q_SUCCESS != lQueueError)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: Queue Creation with key %u failed\n",
            gProcessName, mIsupMsgReqKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         SynwayUtil::HandleQueueCreateError(lQueueError, mIsupMsgReqQ);
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }

   lQueueError = mChnlMgmtMsgReqQ.Create(mChnlMgmtMsgReqKey, IPC_CREAT| IPC_PERM);
   if (Q_SUCCESS != lQueueError)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: Queue Creation with key %u failed\n",
            gProcessName, mChnlMgmtMsgReqKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      SynwayUtil::HandleQueueCreateError(lQueueError, mChnlMgmtMsgReqQ);
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }

   lQueueError = mIsupMsgRspQ.Create(mIsupMsgRspKey, IPC_CREAT| IPC_PERM);
   if (Q_SUCCESS != lQueueError)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: Queue Creation with key %u failed\n",
            gProcessName, mIsupMsgRspKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         SynwayUtil::HandleQueueCreateError(lQueueError, mIsupMsgRspQ);
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }

   lQueueError = mChnlMgmtMsgRspQ.Create(mChnlMgmtRspKey, IPC_CREAT| IPC_PERM);
   if (Q_SUCCESS != lQueueError)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: Queue Creation with key %u failed\n",
            gProcessName, mChnlMgmtRspKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      SynwayUtil::HandleQueueCreateError(lQueueError, mChnlMgmtMsgRspQ);
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }

   lQueueError = mRspMsgQue.Create(mRspMsgQKey, IPC_CREAT| IPC_PERM);
   if (Q_SUCCESS != lQueueError)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s: Queue Creation with key %u failed\n", 
            gProcessName, mRspMsgQKey);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         SynwayUtil::HandleQueueCreateError(lQueueError, mRspMsgQue);
      gLog.GenerateLog(GSYS06, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : StartEventThread
// DESCRIPTION : Wrapper function to access member function
// PARAMETER   :
// RETURN      :
// THROWS      :
//-------------------------------------------------------------
void* StartEventThread(void* ptr)
{
   SynwayMPManager * lpSynwayMPManager = (SynwayMPManager * ) ptr;

#ifdef __x86_64__
   T(gTrace, printf("%s: EventMsg Thread started  %ld \n", gProcessName, syscall(186)););
#else
   T(gTrace, printf("%s: EventMsg Thread started  %ld \n", gProcessName, syscall(224)););
#endif   
   MESSAGE_INFO 		lEvent;

   while(SynwayUtil::KeepRunning())
   {
      memset(&lEvent, 0, sizeof(MESSAGE_INFO));
      if(true == lpSynwayMPManager->ReadEventRsp(lEvent))
      {
         switch(lEvent.wEventCode)
         {
            case 0x000F:/*E_PROC_PlayEnd*/
            case 0x0010:/*E_PROC_PlayFile*/
            case 0x0013:/*E_PROC_RecordEnd*/
            case 0x0014:/*E_PROC_RecordFile*/
            case 0x000B:/*E_PROC_WaitDTMF*/
            case 0x002E:/*E_SYS_BargeIn*/
            case 0x002F:/*E_SYS_NoSound*/
            case 0x000C:/*E_CHG_RcvDTMF*/
               lpSynwayMPManager->ProcessEventRsp(lpSynwayMPManager, lEvent);
               break;
            case 0x001A:/*E_CHG_RemoteChBlock*/
            case 0x001B:/*E_CHG_RemotePCMBlock*/
               lpSynwayMPManager->ProcessChnlMgmtrsp(lpSynwayMPManager, lEvent);
               break;
            default:
               lpSynwayMPManager->ProcessSigEventRsp(lpSynwayMPManager, lEvent);
         }
      }
   }

   return 0;
}

//-------------------------------------------------------------
// FUNCTION    : StartSignalingThread
// DESCRIPTION : 
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
void* StartSignalingThread(void* ptr)
{
   SynwayMPManager * lpSynwayMPManager = (SynwayMPManager * ) ptr;

#ifdef __x86_64__
   T(gTrace, printf("%s: Signaling Thread started  %ld \n", gProcessName, syscall(186)););
#else
   T(gTrace, printf("%s: Signaling Thread started  %ld \n", gProcessName, syscall(224)););
#endif
   GenIsupMsg			lGenIsupMsg;

   while(SynwayUtil::KeepRunning())
   {
      memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
      if(true == lpSynwayMPManager->ReadIsupMsg(lGenIsupMsg))
      {
         lpSynwayMPManager->ProcessIsupMsg(lGenIsupMsg);
      }
   }

   return 0;
}

//-------------------------------------------------------------
// FUNCTION    : StartChnlMgmtThread
// DESCRIPTION : 
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
void* StartChnlMgmtThread(void* ptr)
{
   SynwayMPManager * lpSynwayMPManager = (SynwayMPManager * ) ptr;

#ifdef __x86_64__
   T(gTrace, printf("%s: Chnl Mgmt Thread started  %ld \n", gProcessName, syscall(186)););
#else
      T(gTrace, printf("%s: Chnl Mgmt Thread started  %ld \n", gProcessName, syscall(224)););
#endif
   CktCmdReq 			lCktCmdReq;


   while(SynwayUtil::KeepRunning())
   {
      memset(&lCktCmdReq, 0, sizeof(CktCmdReq));
      if(true == lpSynwayMPManager->ReadChnlMgmtMsg(lCktCmdReq))
      {
         lpSynwayMPManager->ProcessChnlMgmtMsg(lCktCmdReq);
      }
   }

   return 0;
}
//-------------------------------------------------------------
// FUNCTION    : StartChnlTimerThread
// DESCRIPTION : 
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
void* StartChnlTimerThread(void* ptr)
{
   SynwayMPManager * lpSynwayMPManager = (SynwayMPManager * ) ptr;

#ifdef __x86_64__
   T(gTrace, printf("%s: Chnl Timer Thread started  %d \n", gProcessName, syscall(186)););
#else
   T(gTrace, printf("%s: Chnl Timer Thread started  %ld \n", gProcessName, syscall(224)););
#endif

   while(SynwayUtil::KeepRunning())
   {
      lpSynwayMPManager->CheckChnlStatus();

      sleep(1);
   }

   return 0;
}

//-------------------------------------------------------------
// FUNCTION    : ProcessIsupMsg
// DESCRIPTION : 
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::ProcessIsupMsg(GenIsupMsg &lGenIsupMsg)
{
   INT16 lRet = 0;

   lRet = gSynwayVoiceMgrApiObj.SendIsupMsgToNw(lGenIsupMsg);
   if(SNW_RET_ERR_FRW_APP == lRet)
      return WriteIsupMsg(lGenIsupMsg,(TEXT*) "TH:SIG");
   else if(SNW_RET_ERR == lRet)
      return false;

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : ProcessChnlMgmtMsg
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::ProcessChnlMgmtMsg(CktCmdReq &lCktCmdReq)
{
   if(true != gSynwayVoiceMgrApiObj.SendChnlMgmtMsg(lCktCmdReq))
      return false;

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : InitChannel
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      :
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::InitChannel()
{
   TEXT       lLogText[MAX_LOG_TEXT_LEN] = "";

   mMinChannel = 0;
   if (MP_SUCCESS != gSynwayVoiceMgrApiObj.Init(0, mMinChannel, mMaxChannel-1, mInPutShCfgFile, mInPutShIdxFile))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,  "Failed to Init Synway Library");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP01, lLogText);
      return false;
   }
   // Create thread to monitor events
   if (true != CreateEventThread())
   {
      sprintf(lLogText, "Event Thread Creation Failed. ");
      TERR(gTrace,printf("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP02, lLogText);
      return false;
   }

   if(true == mRunSigThread)
   {
      if  (true != CreateSignalingThread())
      {
         sprintf(lLogText, "Signaling Thread Creation Failed. ");
         TERR(gTrace,printf("%s: %s. Exiting...\n", gProcessName, lLogText););
         gLog.GenerateLog(ISMP02, lLogText);
         return false;
      }
   }

   if(true == mRunChnlMgmtThread)
   {
      if  (true != CreateChnlMgmtThread())
      {
         sprintf(lLogText, "Channel Management Thread Creation Failed. ");
         TERR(gTrace,printf("%s: %s. Exiting...\n", gProcessName, lLogText););
         gLog.GenerateLog(ISMP02, lLogText);
         return false;
      }
   }

   if  (true != CreateChnlTimerThread())
   {
      sprintf(lLogText, "Channel Timer Thread Creation Failed. ");
      TERR(gTrace,printf("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP02, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : CreateEventThread
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
// THROWS      :
//-------------------------------------------------------------

BOOLEAN SynwayMPManager::CreateEventThread()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   void* (*lpFuncPtr)(void * );

   lpFuncPtr = StartEventThread;

   mEventThread = pthread_create(&mEventMsgThread, NULLPTR, lpFuncPtr, &gSynwayMPManagerObj);
   if(mEventThread != 0)
   {
      sprintf(lLogText, "pthread_create failed for StartEventThread for %s",gProcessName);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// FUNCTION    : CreateSignalingThread
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::CreateSignalingThread()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   void* (*lpFuncPtr)(void * );

   lpFuncPtr = StartSignalingThread;

   mSignalingThread = pthread_create(&mSignalingMsgThread, NULLPTR, lpFuncPtr, &gSynwayMPManagerObj);
   if(mSignalingThread != 0)
   {
      sprintf(lLogText, "pthread_create failed for StartSignalingThread for %s",gProcessName);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// FUNCTION    : CreateChnlMgmtThread
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::CreateChnlMgmtThread()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   void* (*lpFuncPtr)(void * );

   lpFuncPtr = StartChnlMgmtThread;

   mChnlMgmtThread = pthread_create(&mChnlMgmtMsgThread, NULLPTR, lpFuncPtr, &gSynwayMPManagerObj);
   if(mChnlMgmtThread != 0)
   {
      printf(lLogText, "pthread_create failed for StartChnlMgmtThread for %s",gProcessName);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// FUNCTION    : CreateChnlTimerThread
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::CreateChnlTimerThread()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   void* (*lpFuncPtr)(void * );

   lpFuncPtr = StartChnlTimerThread;

   mChnlTimerThread = pthread_create(&mChnlTimerMsgThread, NULLPTR, lpFuncPtr, &gSynwayMPManagerObj);
   if(mChnlTimerThread != 0)
   {
      printf(lLogText, "pthread_create failed for StartChnlTimerThread for %s",gProcessName);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   return true;
}

//-------------------------------------------------------------
// FUNCTION    : DeInit
// DESCRIPTION : DeInitializes member attributes
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN SynwayMPManager::DeInit()
{
   TEXT      lLogText[MAX_LOG_TEXT_LEN] = "";

   if ( MP_SUCCESS != gSynwayVoiceMgrApiObj.Deinit() )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DeInit Lib Api's Failed");
      TERR(gTrace,printf("%s: %s. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP03, lLogText);
   }

   if(-1 ==  kill(0, SIGTERM))
   {
      printf("%s: Threads Kill Failed .....Errno:%d  %s\n", gProcessName, errno, strerror(errno));
   }

#if 0
   if(0 != pthread_join(mSignalingMsgThread, NULL))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Join Signaling Thread Failed......");
      TERR(gTrace,printf("%s: %s. Exiting...\n", gProcessName, lLogText););
   }

   if(0 != pthread_join(mEventMsgThread, NULL))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Join EventThread Failed......");
      TERR(gTrace,printf("%s: %s. Exiting...\n", gProcessName, lLogText););
   }
#endif
   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadEventRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ReadEventRsp(MESSAGE_INFO &lEvent)
{
   if (true != gSynwayVoiceMgrApiObj.WaitForEvt(lEvent))
      return false;

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadIsupMsg
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ReadIsupMsg(GenIsupMsg &lGenIsupMsg)
{
   msgQueueError lQueueErr;
   INT16			  lMsgType = 0;
   INT16         lMsgLen = sizeof(GenIsupMsg);
   TEXT          lLogText[MAX_LOG_TEXT_LEN] = "";

   if(Q_SUCCESS != (lQueueErr = mIsupMsgReqQ.ReadMsg((void*)&lGenIsupMsg, lMsgLen, lMsgType, BLOCKING)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "TH:SIG Read IsupMsg Request Failed");
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP25, lLogText);
      SynwayUtil::HandleQueueReadError(lQueueErr, mIsupMsgReqQ);
      return false;
   }

   if(lGenIsupMsg.circuitId < 1 || lGenIsupMsg.circuitId > mMaxChannel)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:SIG CKTID:[%03d] Type:%d(%s) Invalid Channel, Cannot Proceed Further",
            lGenIsupMsg.circuitId, lGenIsupMsg.isupMsgType, 
            SynwayUtil::ConvertIsupMsgToStr(lGenIsupMsg.isupMsgType));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP41, lLogText);
      return false;
   }

   lGenIsupMsg.circuitId = lGenIsupMsg.circuitId - 1;

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "TH:SIG Read IsupReq CKTID:[%03d] Type:%d(%s)",
         lGenIsupMsg.circuitId + 1, lGenIsupMsg.isupMsgType, 
         SynwayUtil::ConvertIsupMsgToStr(lGenIsupMsg.isupMsgType));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISMP26, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadChnlMgmtMsg
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ReadChnlMgmtMsg(CktCmdReq &lCktCmdReq)
{
   msgQueueError lQueueErr;
   INT16			  lMsgType = 0;
   INT16         lMsgLen = sizeof(CktCmdReq);
   TEXT          lLogText[MAX_LOG_TEXT_LEN] = "";


   if(Q_SUCCESS != (lQueueErr = mChnlMgmtMsgReqQ.ReadMsg((void*)&lCktCmdReq, lMsgLen, lMsgType, BLOCKING)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "TH:CM Read Channel Mgmt Request Failed");
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP37, lLogText);
      SynwayUtil::HandleQueueReadError(lQueueErr, mChnlMgmtMsgReqQ);
      return false;
   }

   if(lCktCmdReq.channelId < 1 || lCktCmdReq.channelId > mMaxChannel)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "TH:CM CKTID:[%03d] CMDType:%d(%s) Invalid Channel, Cannot Proceed Further",
            lCktCmdReq.channelId + 1, lCktCmdReq.cktCmd, 
            SynwayUtil::ConvertCktCmdReqToStr(lCktCmdReq.cktCmd));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP41, lLogText);
      return false;
   }

   lCktCmdReq.channelId = lCktCmdReq.channelId - 1;

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "TH:CM Read Channel Mgmt Request CKTID:[%03d] CMDType:%d(%s)",
         lCktCmdReq.channelId + 1, lCktCmdReq.cktCmd, 
         SynwayUtil::ConvertCktCmdReqToStr(lCktCmdReq.cktCmd));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISMP38, lLogText);

   return true;
}
//----------------------------------------------------------------------
// METHOD      : ProcessEventRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ProcessEventRsp(SynwayMPManager * lpSynwayMPManager,
      MESSAGE_INFO &lEvent)
{	
   MpResponseDetails lMpRsp;

   memset(&lMpRsp, 0, sizeof(MpResponseDetails));
   if(true == gSynwayVoiceMgrApiObj.HandleMediaEvent(lMpRsp, lEvent))
      return lpSynwayMPManager->WriteRsp(lMpRsp,(TEXT*) "TH:EV:M");

   return false;
}

//----------------------------------------------------------------------
// METHOD      : ProcessSigEventRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ProcessSigEventRsp(SynwayMPManager * lpSynwayMPManager, 
      MESSAGE_INFO &lEvent)
{
   GenIsupMsg lGenIsupMsg;
   
   memset(&lGenIsupMsg, 0, sizeof(GenIsupMsg));
   if(true == gSynwayVoiceMgrApiObj.HandleSigEvent(lGenIsupMsg, lEvent))
      return lpSynwayMPManager->WriteIsupMsg(lGenIsupMsg,(TEXT*) "TH:EV:S");

   return false;
}

//----------------------------------------------------------------------
// METHOD      : ProcessChnlMgmtrsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ProcessChnlMgmtrsp(SynwayMPManager * lpSynwayMPManager, 
      MESSAGE_INFO &lEvent)
{
   CktCmdRsp      lCktCmdRsp;

   if(true == gSynwayVoiceMgrApiObj.HandleChnlMgmtEvent(lCktCmdRsp, lEvent))
      return lpSynwayMPManager->WriteChnlMgmtMsg(lCktCmdRsp,(TEXT*) "TH:EV:C");

   return false;
}

//----------------------------------------------------------------------
// METHOD      : ConvtCmdToStr
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* SynwayMPManager::ConvtCmdToStr(EnumMediaCmdTypes lCmd, unsigned int &lseq, UINT16 &lCkt, MpData &lMpData)
{
   switch(lCmd)
   {
      case MC_INVAL_CMD_TYPE:
         lseq = 0;
         lCkt = 0;
         return (TEXT*)"MC_INVAL_CMD_TYPE";
      case MC_CNTTN:
         lseq = lMpData.playMsg.seqNum;
         lCkt = lMpData.playMsg.circuitId;
         return (TEXT*)"MC_CNTTN";
      case MC_DISTN:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_DISTN";
      case MC_CNTSP:
         lseq = lMpData.switchChnl.seqNum;
         lCkt = lMpData.switchChnl.circuitId_1;
         return (TEXT*)"MC_CNTSP";
      case MC_DISSP:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_DISSP";
      case MC_GETDIG:
         lseq = lMpData.getDigit.seqNum;
         lCkt = lMpData.getDigit.circuitId;
         return (TEXT*)"MC_GETDIG";
      case MC_GETDIGSTP:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_GETDIGSTP";
      case MC_RECSTR:
         lseq = lMpData.recordMsg.seqNum;
         lCkt = lMpData.recordMsg.circuitId;
         return (TEXT*)"MC_RECSTR";
      case MC_RECSTP:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_RECSTP";
      case MC_SETVOL:
         lseq = lMpData.setVolSpd.seqNum;
         lCkt = lMpData.setVolSpd.circuitId;
         return (TEXT*)"MC_SETVOL";
      case MC_SETSPD:
         lseq = lMpData.setVolSpd.seqNum;
         lCkt = lMpData.setVolSpd.circuitId;
         return (TEXT*)"MC_SETSPD";
      default:
         lseq = 0;
         lCkt = 0;
         return (TEXT*)"CMD_CORRUPT";
   }
}

//----------------------------------------------------------------------
// METHOD      : ReadReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ReadReq()
{
   msgQueueError lQueueErr;
   INT16         lMsgLen = sizeof(MpRequestDetails);
   TEXT          lLogText[MAX_LOG_TEXT_LEN] = "";
   TEXT          lTxt[MAX_LOG_TEXT_LEN] = "";
   INT16			  lMsgType = mBoardId + 1;
   UINT16 		  lCkt;
   unsigned int  lSeq = 0;
   MpResponseDetails lMpRsp;

   memset(&mMpReq, 0, sizeof(MpRequestDetails) );

   if(Q_SUCCESS != (lQueueErr = mReqMsgQue.ReadMsg((void*)&mMpReq, lMsgLen, lMsgType, BLOCKING)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Read Voice Process Request Failed");
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP04, lLogText);
      SynwayUtil::HandleQueueReadError(lQueueErr, mReqMsgQue);

      return false;
   }

   lSeq = mMpReq.mpReqData.dummy.seqNum;
   if( -1 == this->GetChannelId(mMpReq.cmdType, mMpReq.mpReqData))
   {
      memset(&lMpRsp, 0 , sizeof(MpResponseDetails));
      lMpRsp.cmdType 	= mMpReq.cmdType;
      lMpRsp.mpRspData 	= mMpReq.mpReqData;
      lMpRsp.rspMsg     = MP_RSP_FAILURE;
      lMpRsp.voiceProcessErr = MP_INVALID_CHANNEL;

      this->WriteRsp(lMpRsp);
      return false;
   }

   snprintf(lTxt, MAX_LOG_TEXT_LEN, "CMDTYPE:%s", 
         ConvtCmdToStr(mMpReq.cmdType, lSeq, lCkt, mMpReq.mpReqData));

   if(lCkt < 0 || lCkt > mMaxChannel-1)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CKTID:[%03d] %s Invalid Channel, Cannot Proceed Further",
            lSeq, lCkt + 1, lTxt);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISMP41, lLogText);
      return false;
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN,"RefNum:%08X CKTID:[%03d] %s Read Request", 
         lSeq, lCkt + 1, lTxt);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISMP05, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ProcessReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::ProcessReq()
{
   MpResponseDetails 	lMpRsp;
   EnumVoiceProcessErr	lVoiceProcessErr;
   BOOLEAN 					lReturn = false;
   EnumChannelStatus    lTmpChnlSt;
   TEXT						lTxtStat[50] = "";
   TEXT            		lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   memset(&lMpRsp, 0, sizeof(MpResponseDetails));
   lMpRsp.mpRspData.dummy.seqNum =  mMpReq.mpReqData.dummy.seqNum;
   switch(mMpReq.cmdType)
   {
      case MC_CNTTN:       
         {
            lVoiceProcessErr = gSynwayVoiceMgrApiObj.Play(mMpReq.mpReqData.playMsg,(TEXT*) "MAIN:MEDIA");

            lMpRsp.cmdType					= MC_CNTTN;
            lMpRsp.mpRspData.playMsg 	= mMpReq.mpReqData.playMsg;
            lMpRsp.voiceProcessErr	 	= lVoiceProcessErr;

            if ( MP_SUCCESS != lVoiceProcessErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "RefNum:%08X CKTID:[%03d] Play Failed",
                     mMpReq.mpReqData.playMsg.seqNum,
                     mMpReq.mpReqData.playMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP06, lLogText);

               lMpRsp.rspMsg 					= MP_RSP_FAILURE;
               lReturn							= false;
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               this->WriteRsp(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     " RefNum:%08X CKTID:[%03d] Play Success",
                     mMpReq.mpReqData.playMsg.seqNum,
                     mMpReq.mpReqData.playMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP07, lLogText);

               lMpRsp.rspMsg 					= MP_RSP_SUCCESS;
               lReturn							= true;
               //if(true == mSendRspFlag || true == mMpReq.mpReqData.playMsg.nonBlocking)
               //For Non Blocking Response is Sent Later When the Play Issue is Success
               if(true == mSendRspFlag)
                  this->WriteRsp(lMpRsp);
            }
         }
         break;
      case MC_DISTN:      // Stop Command
         {
            lTmpChnlSt = gSynwayVoiceMgrApiObj.GetMediaChannelStatus(
                  mMpReq.mpReqData.stopMsg.circuitId, lTxtStat);
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CKTID:[%03d] Status before issuing STOP cmd:%s", 
                  mMpReq.mpReqData.stopMsg.seqNum,
                  mMpReq.mpReqData.stopMsg.circuitId + 1,
                  lTxtStat);
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            //gLog.GenerateLog
            switch(lTmpChnlSt)
            {
               case CKT_PLAYING_AND_GETDIG:
               case CKT_PLAY_STOPING_AND_GETDIG:
                  lReturn = IssuePlayStop();
                  lReturn = IssueGetDigitStop();
                  break;
               case CKT_GETDIG:
                  lReturn = IssueGetDigitStop();
                  break;
               default:
                  lReturn = IssuePlayStop();
            }
         }
         break;
      case MC_CNTSP:
         {
            lVoiceProcessErr = gSynwayVoiceMgrApiObj.ConnectSpeech(mMpReq.mpReqData.switchChnl,(TEXT*) "MAIN:MEDIA");

            lMpRsp.cmdType              = MC_CNTSP;
            lMpRsp.mpRspData.switchChnl = mMpReq.mpReqData.switchChnl;
            lMpRsp.voiceProcessErr      = lVoiceProcessErr;

            if(MP_SUCCESS != lVoiceProcessErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] CKTID:[%03d] ConnectSpeech Failed",
                     mMpReq.mpReqData.switchChnl.seqNum,
                     mMpReq.mpReqData.switchChnl.circuitId_1 + 1,
                     mMpReq.mpReqData.switchChnl.circuitId_2 + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP10, lLogText);

               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] CKTID:[%03d] ConnectSpeech success",
                     mMpReq.mpReqData.switchChnl.seqNum,
                     mMpReq.mpReqData.switchChnl.circuitId_1 + 1,
                     mMpReq.mpReqData.switchChnl.circuitId_2 + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP11, lLogText);

               lMpRsp.rspMsg              = MP_RSP_SUCCESS;
               lReturn 						   = true;

               this->WriteRsp(lMpRsp);
            }
         }
         break;
      case MC_DISSP:
         {
            lVoiceProcessErr = gSynwayVoiceMgrApiObj.DisconnectSpeech(mMpReq.mpReqData.switchChnl,(TEXT*) "MAIN:MEDIA");

            lMpRsp.cmdType              = MC_DISSP;
            lMpRsp.mpRspData.switchChnl = mMpReq.mpReqData.switchChnl;
            lMpRsp.voiceProcessErr      = lVoiceProcessErr;

            if(MP_SUCCESS != lVoiceProcessErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] CKTID:[%03d] DisconnectSpeech Failed",
                     mMpReq.mpReqData.switchChnl.seqNum,
                     mMpReq.mpReqData.switchChnl.circuitId_1 + 1,
                     mMpReq.mpReqData.switchChnl.circuitId_2 + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP12, lLogText);

               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               lReturn 						 = false;
               this->WriteRsp(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] CKTID:[%03d] DisconnectSpeech success",
                     mMpReq.mpReqData.switchChnl.seqNum,
                     mMpReq.mpReqData.switchChnl.circuitId_1 + 1,
                     mMpReq.mpReqData.switchChnl.circuitId_2 + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP13, lLogText);

               lMpRsp.rspMsg              = MP_RSP_SUCCESS;
               lReturn 						  = true;

               this->WriteRsp(lMpRsp);
            }
         }
         break;
      case MC_GETDIG:
         {
            MpGetDigit rspDtmf;
            memset(&rspDtmf, 0, sizeof(MpGetDigit));
            lVoiceProcessErr = gSynwayVoiceMgrApiObj.GetDtmfDigits(mMpReq.mpReqData.getDigit, 
                  rspDtmf,(TEXT*) "MAIN:MEDIA");

            lMpRsp.cmdType             = MC_GETDIG;
            lMpRsp.mpRspData.getDigit  = mMpReq.mpReqData.getDigit;
            lMpRsp.voiceProcessErr     = lVoiceProcessErr;

            if(MP_GET_DIG_CMD_ALREADY_DONE == lVoiceProcessErr)
            {
               lMpRsp.cmdType             = MC_GETDIGSTP;
               lMpRsp.mpRspData.getDigit  = rspDtmf;
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] GetDtmfDigits Got Rsp with Digts:%s",
                     mMpReq.mpReqData.getDigit.seqNum,
                     mMpReq.mpReqData.getDigit.circuitId + 1,
                     rspDtmf.rspDigits);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP15, lLogText);
               lMpRsp.rspMsg              = MP_RSP_SUCCESS;
               lReturn                    = true;
               this->WriteRsp(lMpRsp);
            }
            else if(MP_SUCCESS != lVoiceProcessErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] GetDtmfDigits Failed",
                     mMpReq.mpReqData.getDigit.seqNum,
                     mMpReq.mpReqData.getDigit.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP14, lLogText);

               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] GetDtmfDigits success",
                     mMpReq.mpReqData.getDigit.seqNum,
                     mMpReq.mpReqData.getDigit.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP15, lLogText);

               lMpRsp.rspMsg              = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lMpRsp);
            }
         }
         break;
      case MC_GETDIGSTP:
         {
            lVoiceProcessErr = gSynwayVoiceMgrApiObj.CancelGetDtmfDigits(mMpReq.mpReqData.stopMsg ,(TEXT*) "MAIN:MEDIA", lMpRsp.mpRspData.getDigit.rspDigits);

            lMpRsp.cmdType             = MC_GETDIGSTP;
            lMpRsp.mpRspData.stopMsg   = mMpReq.mpReqData.stopMsg;
            lMpRsp.voiceProcessErr     = lVoiceProcessErr;

            if(MP_SUCCESS != lVoiceProcessErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] CancelGetDtmfDigits Failed",
                     mMpReq.mpReqData.stopMsg.seqNum,
                     mMpReq.mpReqData.stopMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP16, lLogText);

               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] CancelGetDtmfDigits Success",
                     mMpReq.mpReqData.stopMsg.seqNum,
                     mMpReq.mpReqData.stopMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP17, lLogText);

               lMpRsp.rspMsg              = MP_RSP_SUCCESS;
               lReturn 						 = true;

               this->WriteRsp(lMpRsp);
            }
         }
         break;
      case MC_RECSTR:
         {
            lVoiceProcessErr = gSynwayVoiceMgrApiObj.StartRecord(mMpReq.mpReqData.recordMsg,(TEXT*) "MAIN:MEDIA");

            lMpRsp.cmdType             = MC_RECSTR;
            lMpRsp.mpRspData.recordMsg = mMpReq.mpReqData.recordMsg;
            lMpRsp.voiceProcessErr     = lVoiceProcessErr;

            if(MP_SUCCESS != lVoiceProcessErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] StartRecord Failed",
                     mMpReq.mpReqData.recordMsg.seqNum,
                     mMpReq.mpReqData.recordMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP18, lLogText);

               lMpRsp.rspMsg              = MP_RSP_FAILURE;
               lReturn 						   = false;
               this->WriteRsp(lMpRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CKTID:[%03d] StartRecord Success",
                     mMpReq.mpReqData.recordMsg.seqNum,
                     mMpReq.mpReqData.recordMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ISMP19, lLogText);

               lMpRsp.rspMsg              = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lMpRsp);
            }
         }
         break;
      case MC_RECSTP:
         {
            lVoiceProcessErr = gSynwayVoiceMgrApiObj.StopRecord(mMpReq.mpReqData.stopMsg,(TEXT*) "MAIN:MEDIA");

            lMpRsp.cmdType             = MC_RECSTP;
            lMpRsp.mpRspData.stopMsg   = mMpReq.mpReqData.stopMsg;
            lMpRsp.voiceProcessErr     = lVoiceProcessErr;

            if(MP_SUCCESS != lVoiceProcessErr )
            {
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] StopRecord Failed",
							mMpReq.mpReqData.stopMsg.seqNum,
							mMpReq.mpReqData.stopMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP20, lLogText);

					lMpRsp.rspMsg              = MP_RSP_FAILURE;
					lReturn 							= false;
					this->WriteRsp(lMpRsp);
				}
				else
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] StopRecord Success",
							mMpReq.mpReqData.stopMsg.seqNum,
							mMpReq.mpReqData.stopMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP21, lLogText);

					lMpRsp.rspMsg              = MP_RSP_SUCCESS;
					lReturn 							= true;
					if(true == mSendRspFlag)
						 this->WriteRsp(lMpRsp);
				}
			}
			break;
      case MC_PAUSE:
			{
				lVoiceProcessErr = gSynwayVoiceMgrApiObj.Pause(mMpReq.mpReqData.pauseMsg, (TEXT*) "MAIN:MEDIA");

				lMpRsp.cmdType              = MC_PAUSE;
				lMpRsp.mpRspData.pauseMsg   = mMpReq.mpReqData.pauseMsg;
				lMpRsp.voiceProcessErr      = lVoiceProcessErr;

				if(MP_SUCCESS != lVoiceProcessErr )
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Pause Play Failed",
							mMpReq.mpReqData.pauseMsg.seqNum,
							mMpReq.mpReqData.pauseMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP29, lLogText);

					lMpRsp.rspMsg              = MP_RSP_FAILURE;
					lReturn 							= false;
					this->WriteRsp(lMpRsp);
				}
				else
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Pause Play Success",
							mMpReq.mpReqData.pauseMsg.seqNum,
							mMpReq.mpReqData.pauseMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP30, lLogText);

					lMpRsp.rspMsg              = MP_RSP_SUCCESS;
					lReturn 							= true;
					if(true == mSendRspFlag)
						 this->WriteRsp(lMpRsp);
				}
         }
         break;
      case MC_RESUME:
			{
				lVoiceProcessErr = gSynwayVoiceMgrApiObj.Resume(mMpReq.mpReqData.resumeMsg,(TEXT*) "MAIN:MEDIA");

				lMpRsp.cmdType              = MC_RESUME;
				lMpRsp.mpRspData.resumeMsg  = mMpReq.mpReqData.resumeMsg;
				lMpRsp.voiceProcessErr      = lVoiceProcessErr;

				if(MP_SUCCESS != lVoiceProcessErr )
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Resume Play  Failed",
							mMpReq.mpReqData.resumeMsg.seqNum,
							mMpReq.mpReqData.resumeMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP31, lLogText);

					lMpRsp.rspMsg              = MP_RSP_FAILURE;
					lReturn 							= false;
					this->WriteRsp(lMpRsp);
				}
				else
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Resume Play Success",
							mMpReq.mpReqData.resumeMsg.seqNum,
							mMpReq.mpReqData.resumeMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP32, lLogText);

					lMpRsp.rspMsg              = MP_RSP_SUCCESS;
					lReturn 							= true;
					if(true == mSendRspFlag)
						 this->WriteRsp(lMpRsp);
				}
         }
         break;
      case MC_FORWARD:
			{
				lVoiceProcessErr = gSynwayVoiceMgrApiObj.Forward (mMpReq.mpReqData.forwardMsg,(TEXT*) "MAIN:MEDIA");

				lMpRsp.cmdType              = MC_FORWARD;
				lMpRsp.mpRspData.forwardMsg = mMpReq.mpReqData.forwardMsg;
				lMpRsp.voiceProcessErr      = lVoiceProcessErr;

				if(MP_SUCCESS != lVoiceProcessErr )
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Forward Play  Failed",
							mMpReq.mpReqData.forwardMsg.seqNum,
							mMpReq.mpReqData.forwardMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP33, lLogText);

					lMpRsp.rspMsg              = MP_RSP_FAILURE;
					lReturn 							= false;
					this->WriteRsp(lMpRsp);
				}
				else
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Forward Play Success",
							mMpReq.mpReqData.forwardMsg.seqNum,
							mMpReq.mpReqData.forwardMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP34, lLogText);

					lMpRsp.rspMsg              = MP_RSP_SUCCESS;
					lReturn 							= true;
					if(true == mSendRspFlag)
						 this->WriteRsp(lMpRsp);
				}
         }
         break;
      case MC_REWIND:
			{
				lVoiceProcessErr = gSynwayVoiceMgrApiObj.Rewind(mMpReq.mpReqData.rewindMsg,(TEXT*) "MAIN:MEDIA");

				lMpRsp.cmdType              = MC_REWIND;
				lMpRsp.mpRspData.rewindMsg  = mMpReq.mpReqData.rewindMsg;
				lMpRsp.voiceProcessErr      = lVoiceProcessErr;

				if(MP_SUCCESS != lVoiceProcessErr )
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Forward Play  Failed",
							mMpReq.mpReqData.rewindMsg.seqNum,
							mMpReq.mpReqData.rewindMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP35, lLogText);

					lMpRsp.rspMsg              = MP_RSP_FAILURE;
					lReturn 							= false;
					this->WriteRsp(lMpRsp);
				}
				else
				{
					snprintf(lLogText, MAX_LOG_TEXT_LEN,
							"RefNum:%08X CKTID:[%03d] Forward Play Success",
							mMpReq.mpReqData.rewindMsg.seqNum,
							mMpReq.mpReqData.rewindMsg.circuitId + 1);
					T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
					gLog.GenerateLog(ISMP36, lLogText);

					lMpRsp.rspMsg              = MP_RSP_SUCCESS;
					lReturn 							= true;
					if(true == mSendRspFlag)
						 this->WriteRsp(lMpRsp);
				}
         }
         break;
		default:
			{
				lMpRsp.cmdType					= MC_INVAL_CMD_TYPE;
				lMpRsp.rspMsg              = MP_RSP_FAILURE;
				lMpRsp.voiceProcessErr		= MP_INVALID_CMD_ERR;
				snprintf(lLogText, MAX_LOG_TEXT_LEN,
						"Wrong Command:%d", mMpReq.cmdType);
				T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
				gLog.GenerateLog(ISMP22, lLogText);

				lReturn                    = false;
				this->WriteRsp(lMpRsp);
			}
	}
	return lReturn;
}

//----------------------------------------------------------------------
// METHOD      : WriteRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::WriteRsp(MpResponseDetails &lMpRsp, TEXT *lType)
{
	msgQueueError lQueueErr;
	INT16         lMsgLen = sizeof(MpResponseDetails);
	unsigned int  lSeq = 0;
	UINT16			  lCkt = 0;
	TEXT          lTxt[MAX_LOG_TEXT_LEN] = "";
	TEXT          lLogText[MAX_LOG_TEXT_LEN] = "";
	long          lMsgType = 0;

	
	snprintf(lTxt, MAX_LOG_TEXT_LEN, "CMDTYPE:%s", 
			ConvtCmdToStr(lMpRsp.cmdType, lSeq, lCkt, lMpRsp.mpRspData));

	lMsgType = this->SetChannelId(lMpRsp.cmdType, lMpRsp.mpRspData);

	lQueueErr = mRspMsgQue.WriteMsg(&lMpRsp, lMsgLen, lMsgType, BLOCKING);
	if (Q_SUCCESS != lQueueErr)
	{
		lMsgLen = sizeof(MpResponseDetails);
		if (false == SynwayUtil::HandleQueueWriteError(lQueueErr, mRspMsgQue, &lMpRsp, lMsgLen, 1, BLOCKING))
		{
			snprintf(lLogText, MAX_LOG_TEXT_LEN, 
					"%s: RefNum:%08X CKTID:[%03d] %s MsgType:%ld Write to MP Respone Q Failed", 
					lType, lSeq, lCkt + 1, lTxt, lMsgType); 
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(ISMP24, lLogText);
			return false;
		}
	}

	snprintf(lLogText, MAX_LOG_TEXT_LEN, 
			"%s: RefNum:%08X CKTID:[%03d] RSP:%s %s MsgType:%ld Wrote to MP Respone Q", 
			lType, lSeq, lCkt + 1, (MP_RSP_SUCCESS == lMpRsp.rspMsg) ? 
			"MP_RSP_SUCCESS" : "MP_RSP_FAILURE", lTxt, lMsgType); 
	T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
	gLog.GenerateLog(ISMP23, lLogText);
	return true;
}

//----------------------------------------------------------------------
// METHOD      : GetChannelId
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : TEXT*
//----------------------------------------------------------------------
long SynwayMPManager::GetChannelId(EnumMediaCmdTypes lCmd, MpData &lMpData)
{
	switch(lCmd)
	{
		case MC_INVAL_CMD_TYPE:
			return 0;
		case MC_CNTTN:
			lMpData.playMsg.circuitId = ((lMpData.playMsg.circuitId - 1)  % mMaxChannel);
			return lMpData.playMsg.circuitId;
		case MC_DISTN:
			lMpData.stopMsg.circuitId = ((lMpData.stopMsg.circuitId - 1)  % mMaxChannel);
			return lMpData.stopMsg.circuitId;
		case MC_CNTSP:
			lMpData.switchChnl.circuitId_1 = ((lMpData.switchChnl.circuitId_1 - 1)  % mMaxChannel);
			lMpData.switchChnl.circuitId_2 = ((lMpData.switchChnl.circuitId_2 - 1)  % mMaxChannel);
			
			return lMpData.switchChnl.circuitId_1;
		case MC_DISSP:
			lMpData.switchChnl.circuitId_1 = ((lMpData.switchChnl.circuitId_1 - 1)  % mMaxChannel);
			lMpData.switchChnl.circuitId_2 = ((lMpData.switchChnl.circuitId_2 - 1)  % mMaxChannel);

			return lMpData.switchChnl.circuitId_1;
		case MC_GETDIG:
			lMpData.getDigit.circuitId = ((lMpData.getDigit.circuitId - 1)  % mMaxChannel);

			return lMpData.getDigit.circuitId;
		case MC_GETDIGSTP:
			lMpData.stopMsg.circuitId = ((lMpData.stopMsg.circuitId - 1)  % mMaxChannel);

			return lMpData.stopMsg.circuitId;
		case MC_RECSTR:
			lMpData.recordMsg.circuitId = ((lMpData.recordMsg.circuitId - 1)  % mMaxChannel);

			return lMpData.stopMsg.circuitId;
		case MC_RECSTP:
			lMpData.stopMsg.circuitId = ((lMpData.stopMsg.circuitId - 1)  % mMaxChannel);

			return lMpData.stopMsg.circuitId;
		case MC_SETVOL:
			lMpData.setVolSpd.circuitId = ((lMpData.setVolSpd.circuitId - 1)  % mMaxChannel);

			return lMpData.setVolSpd.circuitId;
		case MC_SETSPD:
			lMpData.setVolSpd.circuitId = ((lMpData.setVolSpd.circuitId - 1)  % mMaxChannel);

			return lMpData.setVolSpd.circuitId;
		default:
			return 0;
	}
}

//----------------------------------------------------------------------
// METHOD      : SetChannelId
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : TEXT*
//----------------------------------------------------------------------
long SynwayMPManager::SetChannelId(EnumMediaCmdTypes lCmd, MpData &lMpData)
{
	switch(lCmd)
	{
		case MC_INVAL_CMD_TYPE:
			return INV_CHANNEL_NUMBER;
		case MC_CNTTN:
			lMpData.playMsg.circuitId = (lMpData.playMsg.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.playMsg.circuitId;
		case MC_DISTN:
			lMpData.stopMsg.circuitId = (lMpData.stopMsg.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.stopMsg.circuitId;
		case MC_CNTSP:
			lMpData.switchChnl.circuitId_1 = (lMpData.switchChnl.circuitId_1 + 1) +(mBoardId * mMaxChannel);
			lMpData.switchChnl.circuitId_2 = (lMpData.switchChnl.circuitId_2 + 1) +(mBoardId * mMaxChannel);
			return lMpData.switchChnl.circuitId_1;
		case MC_DISSP:
         lMpData.switchChnl.circuitId_1 = (lMpData.switchChnl.circuitId_1 + 1) +(mBoardId * mMaxChannel);
         lMpData.switchChnl.circuitId_2 = (lMpData.switchChnl.circuitId_2 + 1) +(mBoardId * mMaxChannel);
         return lMpData.switchChnl.circuitId_1;
		case MC_GETDIG:
			lMpData.getDigit.circuitId = (lMpData.getDigit.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.getDigit.circuitId;
		case MC_GETDIGSTP:
			lMpData.stopMsg.circuitId = (lMpData.stopMsg.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.stopMsg.circuitId;
		case MC_RECSTR:
			lMpData.recordMsg.circuitId = (lMpData.recordMsg.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.recordMsg.circuitId;
		case MC_RECSTP:
			lMpData.stopMsg.circuitId = (lMpData.stopMsg.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.stopMsg.circuitId;
		case MC_SETVOL:
			lMpData.setVolSpd.circuitId = (lMpData.setVolSpd.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.setVolSpd.circuitId;
		case MC_SETSPD:
			lMpData.setVolSpd.circuitId = (lMpData.setVolSpd.circuitId + 1) +(mBoardId * mMaxChannel);
			return lMpData.setVolSpd.circuitId;
		default:
			return INV_CHANNEL_NUMBER;
	}
}


//----------------------------------------------------------------------
// METHOD      : IssuePlayStop
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : 
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::IssuePlayStop()
{
	EnumVoiceProcessErr  lVoiceProcessErr;
	TEXT                 lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	MpResponseDetails    lMpRsp;
	memset(&lMpRsp, 0, sizeof(MpResponseDetails));

	lVoiceProcessErr = gSynwayVoiceMgrApiObj.Stop(mMpReq.mpReqData.stopMsg,(TEXT*) "MAIN:MEDIA");

	lMpRsp.cmdType					= MC_DISTN;
	lMpRsp.mpRspData.stopMsg 	= mMpReq.mpReqData.stopMsg;
	lMpRsp.voiceProcessErr	 	= lVoiceProcessErr;

	if ( MP_SUCCESS != lVoiceProcessErr )
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"RefNum:%08X CKTID:[%03d] Stop Failed",
				mMpReq.mpReqData.stopMsg.seqNum,
				mMpReq.mpReqData.stopMsg.circuitId + 1);
		T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(ISMP08, lLogText);

		lMpRsp.rspMsg 					= MP_RSP_FAILURE;
		this->WriteRsp(lMpRsp);
		return false;
	}
	else
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"RefNum:%08X CKTID:[%03d] Stop success",
				mMpReq.mpReqData.stopMsg.seqNum,
				mMpReq.mpReqData.stopMsg.circuitId + 1);
		T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(ISMP09, lLogText);

		lMpRsp.rspMsg 					= MP_RSP_SUCCESS;
		if(true == mSendRspFlag)
			this->WriteRsp(lMpRsp);

		return true;
	}
}

//----------------------------------------------------------------------
// METHOD      : IssueGetDigitStop
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : 
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::IssueGetDigitStop()
{
	EnumVoiceProcessErr  lVoiceProcessErr;
	TEXT                 lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	MpResponseDetails    lMpRsp;
	memset(&lMpRsp, 0, sizeof(MpResponseDetails));

	lVoiceProcessErr = gSynwayVoiceMgrApiObj.CancelGetDtmfDigits(mMpReq.mpReqData.stopMsg ,(TEXT*) "MAIN:MEDIA", lMpRsp.mpRspData.getDigit.rspDigits);

	lMpRsp.cmdType             = MC_GETDIGSTP;
	lMpRsp.mpRspData.stopMsg   = mMpReq.mpReqData.stopMsg;
	lMpRsp.voiceProcessErr     = lVoiceProcessErr;

	if(MP_SUCCESS != lVoiceProcessErr )
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"RefNum:%08X CKTID:[%03d] CancelGetDtmfDigits Failed",
				mMpReq.mpReqData.stopMsg.seqNum,
				mMpReq.mpReqData.stopMsg.circuitId + 1);
		T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(ISMP16, lLogText);

		lMpRsp.rspMsg              = MP_RSP_FAILURE;
		this->WriteRsp(lMpRsp);
		return false;
	}
	else
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"RefNum:%08X CKTID:[%03d] CancelGetDtmfDigits Success",
				mMpReq.mpReqData.stopMsg.seqNum,
				mMpReq.mpReqData.stopMsg.circuitId + 1);
		T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(ISMP17, lLogText);

		lMpRsp.rspMsg              = MP_RSP_SUCCESS;
		this->WriteRsp(lMpRsp);
		return true;
	}
}

//----------------------------------------------------------------------
// METHOD      : WriteIsupMsg
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : 
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::WriteIsupMsg(GenIsupMsg &lGenIsupMsg, TEXT *lType)
{
	msgQueueError lQueueErr;
	INT16         lMsgLen = sizeof(GenIsupMsg);
	TEXT          lLogText[MAX_LOG_TEXT_LEN] = "";
	long          lMsgType = 1;

	lGenIsupMsg.circuitId = lGenIsupMsg.circuitId + 1; 

	lQueueErr = mIsupMsgRspQ.WriteMsg(&lGenIsupMsg, lMsgLen, lMsgType, BLOCKING);
	if (Q_SUCCESS != lQueueErr)
	{
		lMsgLen = sizeof(GenIsupMsg);
		if (false == SynwayUtil::HandleQueueWriteError(lQueueErr, mIsupMsgRspQ, &lGenIsupMsg, lMsgLen, 1, BLOCKING))
		{
			snprintf(lLogText, MAX_LOG_TEXT_LEN, 
					"CKTID:[%03d] Type:%d Write to IsupRsp Q Failed", 
					lGenIsupMsg.circuitId, lGenIsupMsg.isupMsgType);
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(ISMP27, lLogText);
			return false;
		}
	}

	snprintf(lLogText, MAX_LOG_TEXT_LEN, 
			"CKTID:[%03d] Type:%d(%s) Wrote to IsupRsp Q ", 
			lGenIsupMsg.circuitId, lGenIsupMsg.isupMsgType,
         SynwayUtil::ConvertIsupMsgToStr(lGenIsupMsg.isupMsgType));
	T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
	gLog.GenerateLog(ISMP28, lLogText);
	return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteChnlMgmtMsg
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : 
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::WriteChnlMgmtMsg(CktCmdRsp &lCktCmdRsp, TEXT *lType)
{
	msgQueueError lQueueErr;
	INT16         lMsgLen = sizeof(CktCmdRsp);
	TEXT          lLogText[MAX_LOG_TEXT_LEN] = "";
	long          lMsgType = 1;

   lCktCmdRsp.channelId = lCktCmdRsp.channelId+ 1;

	lQueueErr = mChnlMgmtMsgRspQ.WriteMsg(&lCktCmdRsp, lMsgLen, lMsgType, BLOCKING);
	if (Q_SUCCESS != lQueueErr)
	{
		lMsgLen = sizeof(CktCmdRsp);
		if (false == SynwayUtil::HandleQueueWriteError(lQueueErr, mIsupMsgRspQ, &lCktCmdRsp, lMsgLen, 1, BLOCKING))
		{
			snprintf(lLogText, MAX_LOG_TEXT_LEN, 
					"CKTID:[%03d] CmdStatus:%d Write to Channel Mgmt Rsp Q Failed", 
					lCktCmdRsp.channelId, lCktCmdRsp.status);
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(ISMP39, lLogText);
			return false;
		}
	}

	snprintf(lLogText, MAX_LOG_TEXT_LEN, 
			"CKTID:[%03d] CmdStatus:%d(%s) Wrote to Channel Mgmt Rsp Q ", 
			lCktCmdRsp.channelId, lCktCmdRsp.status,
         SynwayUtil::ConvertCktCmdStToStr(lCktCmdRsp.status));
	T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
	gLog.GenerateLog(ISMP40, lLogText);
	return true;
}

//----------------------------------------------------------------------
// METHOD      : CheckChnlStatus
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SynwayMPManager::CheckChnlStatus()

{
   MpResponseDetails  rsp;
   GenIsupMsg         genIsupMsg;

   for(int chnl = 0; chnl < mMaxChannel; chnl++)
   {
      memset(&rsp, 0, sizeof(rsp));
      if(true == gSynwayVoiceMgrApiObj.CheckMediaChannelStatus(chnl, rsp))
         WriteRsp(rsp, "TH:TM");

      memset(&genIsupMsg, 0, sizeof(GenIsupMsg));
      if(true == gSynwayVoiceMgrApiObj.CheckSignalStatus(chnl, genIsupMsg))
         WriteIsupMsg(genIsupMsg, "TH:TM");
   }
   return true;
}
