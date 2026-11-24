//----------------------------------------------------------------------
// NAME
// IvrFeedback.cc
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
#include "IvrFeedback.h"
TEXT        gLogText[MAX_LOG_TEXT_LEN];
//-------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrFeedback::IvrFeedback()
{
}

//-------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrFeedback::~IvrFeedback()
{
}

//-------------------------------------------------------------
// METHOD      : Init 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrFeedback::Init()
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

   if(DB_NO_ERR != mIvrDbManager.Init(1))
   {
      sprintf(lLogText, "DataBase Initialization failed.");
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
BOOLEAN IvrFeedback::ReadRequest()
{
   return GetReq();
}

//-------------------------------------------------------------
// METHOD      : ProcessRequest
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrFeedback::ProcessRequest()
{
   KernelMsg   lKernelMsg;

   //Send Request to Session Mgr
   for(int lcnt = 0; lcnt < (int)mTotalCount; lcnt++)
   {
      memset(&lKernelMsg, 0, sizeof(KernelMsg));
      memset(&lKernelMsg.scheduleReq, 0, sizeof(ScheduleReq));
      
      lKernelMsg.kernelMsgType = SCHEDULE_REQ;
      lKernelMsg.scheduleReq = mScheduleReq[lcnt];

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
BOOLEAN IvrFeedback::GetReq()
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   DbErrorCodes   lDbErr;
   mTotalCount    = 0;
   memset(&mScheduleReq, 0, sizeof(ScheduleReq));

   lDbErr = mIvrDbManager.FetchFeedbackRsp();

   //Fetch Request from Db
   if(lDbErr != DB_NO_ERR)
   {
      if(lDbErr == DB_NO_RECORD_FOUND_ERR)
         return false;
      else 
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Db Error occurred Err:%d", lDbErr);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(DBP17, lLogText);
         return false;
      }
   }
   return true; 
}

//-------------------------------------------------------------
// METHOD      : ReadRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrFeedback::ReadResponse()
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
BOOLEAN IvrFeedback::ProcessResponse()
{
   return UpdateResponse(); 
}

//-------------------------------------------------------------
// METHOD      : 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrFeedback::UpdateResponse()
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   DbErrorCodes   lDbErr;
   mTotalCount    = 0;
//   memset(&mScheduleRsp, 0, sizeof(ScheduleRsp));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "ShId:%d RF:%s SHCODE:%s OA:%s DA:%s RSP:%d Err:%d ErrMsg:%s",
            mScheduleRsp.scheduleId, mScheduleRsp.callRefNum , mScheduleRsp.shortCode, 
            mScheduleRsp.origAddr, mScheduleRsp.destAddr, mScheduleRsp.status, 
            mScheduleRsp.errorCode, mScheduleRsp.errMsg);

   lDbErr = mIvrDbManager.UpdateScheduledRsp(mScheduleRsp); 
   if(lDbErr != DB_NO_ERR)
   {
      TERR(gTrace, printf("%s: Db Update Failed DbErr:%d for %s\n", 
               gProcessName, lDbErr, lLogText););
      gLog.GenerateLog(DBP15, lLogText);
      return false;
   }

   T(gTrace, printf("%s: Response Updated Successfuly for %s\n", 
              gProcessName, lLogText););
   gLog.GenerateLog(SCHD03, lLogText);
   return true;
}
