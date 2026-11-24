//----------------------------------------------------------------------
// NAME
// IvrScheduler.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright (C) 2005
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated: Divya K                        Date: 17-JAN-2012
//----------------------------------------------------------------------
#include "IvrScheduler.h"
TEXT        gLogText[MAX_LOG_TEXT_LEN];
TEXT        glLogText[MAX_LOG_TEXT_LEN];
extern UINT32 gNoc;
extern Log              gLog;
//extern CTrace           gTrace (TRACE_IVR_SCHEDULER_ENV);
//extern TEXT             gProcessName[MAX_PROCESS_NAME + 1] = "";
extern TEXT        gLogText[MAX_LOG_TEXT_LEN];
extern TEXT        glLogText[MAX_LOG_TEXT_LEN];
//-------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrScheduler::IvrScheduler()
{
}

//-------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrScheduler::~IvrScheduler()
{
}

//-------------------------------------------------------------
// METHOD      : Init 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrScheduler::Init()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(false == mIvrQueApi.Init())
   {
      sprintf(lLogText, "QueApi Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == mIvrConfigApi.Init())
   {
      sprintf(lLogText, "ConfigApi Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(DB_NO_ERR != mIvrDbManager.Init(mIvrConfigApi.mDbClient))
   {
      sprintf(lLogText, "DataBase Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == mIvrConfigApi.Init(mIvrDbManager, CONFIG_IVR_KERNEL))
   {
      sprintf(lLogText, "ConfigApi Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadRequest
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrScheduler::ReadRequest()
{
   return GetReq();
}

//-------------------------------------------------------------
// METHOD      : ProcessRequest
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrScheduler::ProcessRequest()
{
   KernelMsg   lKernelMsg;

   //Send Request to Session Mgr
   for(int lcnt = 0; lcnt < (int)mTotalCount; lcnt++)
   {
      memset(&lKernelMsg, 0, sizeof(KernelMsg));
      memset(&lKernelMsg.scheduleReq, 0, sizeof(ScheduleReq));
      
      lKernelMsg.kernelMsgType = SCHEDULE_REQ;
      lKernelMsg.scheduleReq = mScheduleReq[lcnt];
      T(gTrace, printf("%s: Schedule Request :(%d): %s, %s, %s\n",gProcessName, lcnt, lKernelMsg.scheduleReq.shortCode, lKernelMsg.scheduleReq.origAddr, lKernelMsg.scheduleReq.destAddr););
      mIvrQueApi.WriteToCallAlertQue(lKernelMsg);
   }
   return true; 
}

//-------------------------------------------------------------
// METHOD      : GetReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrScheduler::GetReq()
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   DbErrorCodes   lDbErr;
   mTotalCount    = 0;
   memset(&mScheduleReq, 0, sizeof(ScheduleReq));

   lDbErr = mIvrDbManager.FetchScheduledReq(mScheduleReq, mIvrConfigApi.mScheduleReTryTime, 
                              mIvrConfigApi.mScheduleReTryAttempts, mTotalCount, gNoc);
   //Fetch Request from Db
   if(lDbErr != DB_NO_ERR)
   {
      if(lDbErr == DB_NO_RECORD_FOUND_ERR)
         return false;
      else 
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Db Error occurred Err:%d", lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
         gLog.GenerateLog(DBP17, glLogText);
         return false;
      }
   }

      T(gTrace, printf("%s: GetRes Schedule Request :(%d): %s, %s, %s\n",gProcessName, mTotalCount, mScheduleReq[0].shortCode, mScheduleReq[0].origAddr, mScheduleReq[0].destAddr););
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Fetched Request - Total:%d", mTotalCount);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SCHD01, lLogText);
   return true; 
}

//-------------------------------------------------------------
// METHOD      : ReadRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrScheduler::ReadResponse()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   memset(&mScheduleRsp, 0, sizeof(ScheduleRsp));

   if(false == mIvrQueApi.ReadFromSchRspQ(mScheduleRsp)) 
   {
      TERR(gTrace, printf("%s: Que read failed\n", gProcessName););
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "ShId:%d RF:%s SHCODE:%s OA:%s DA:%s RSP:%d Err:%d ErrMsg:%s",
            mScheduleRsp.scheduleId, mScheduleRsp.callRefNum , mScheduleRsp.shortCode, 
            mScheduleRsp.origAddr, mScheduleRsp.destAddr, mScheduleRsp.status, 
            mScheduleRsp.errorCode, mScheduleRsp.errMsg);
   T(gTrace, printf("%s: Rcvd Rsp for %s\n", gProcessName, lLogText););
   gLog.GenerateLog(SCHD02, lLogText);
   return true;
}

//-------------------------------------------------------------
// METHOD      : ProcessRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrScheduler::ProcessResponse()
{
   return UpdateResponse(); 
}

//-------------------------------------------------------------
// METHOD      : 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrScheduler::UpdateResponse()
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   DbErrorCodes   lDbErr;
   mTotalCount    = 0;
//   memset(&mScheduleRsp, 0, sizeof(ScheduleRsp));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "ShId:%d RF:%s SHCODE:%s OA:%s DA:%s RSP:%d Err:%d ErrMsg:%s CT:%ld AT:%ld DT:%ld",
            mScheduleRsp.scheduleId, mScheduleRsp.callRefNum , mScheduleRsp.shortCode, 
            mScheduleRsp.origAddr, mScheduleRsp.destAddr, mScheduleRsp.status, 
            mScheduleRsp.errorCode, mScheduleRsp.errMsg, 
            mScheduleRsp.callTime, mScheduleRsp.answerTime, mScheduleRsp.disconnectTime);

   lDbErr = mIvrDbManager.UpdateScheduledRsp(mScheduleRsp); 
   if(lDbErr != DB_NO_ERR)
   {
      TERR(gTrace, printf("%s: Db Update Failed DbErr:%d for %s\n", 
               gProcessName, lDbErr, lLogText););
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", lLogText);
      gLog.GenerateLog(DBP15, glLogText);
      return false;
   }

   T(gTrace, printf("%s: Response Updated Successfuly for %s\n", 
              gProcessName, lLogText););
   gLog.GenerateLog(SCHD03, lLogText);
   return true;
}
