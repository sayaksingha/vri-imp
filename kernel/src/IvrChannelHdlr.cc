//----------------------------------------------------------------------
// NAME
// IvrChannelHdlr.cc
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
#include "IvrChannelHdlr.h"
TEXT        gLogText[MAX_LOG_TEXT_LEN];
extern IvrQueApi  gIvrQueApi;
extern EnumChannelCmd   gChannelCmd;
extern UINT32           gMinChannelId;
//-------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrChannelHdlr::IvrChannelHdlr()
{
}

//-------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrChannelHdlr::~IvrChannelHdlr()
{
}

//-------------------------------------------------------------
// METHOD      : Init 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrChannelHdlr::Init()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(false == gIvrQueApi.Init())
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

   if(false == mIvrChannelApi.Init())
   {
      sprintf(lLogText, "ChannelApi Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

/*   if(DB_SUCCESS != mIvrDbManager.Connect())
   {
      sprintf(lLogText, "DataBase Initialization failed.");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }
*/
   return true;
}

//-------------------------------------------------------------
// METHOD      : ProcessRequest
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN IvrChannelHdlr::ProcessRequest(UINT32& lTotalCkts)
{
   EnumChannelCmd   lCktCmd = gChannelCmd;
   UINT16           lChannelId = gMinChannelId;

   for(UINT32 i = 1; i <= lTotalCkts; i++)
   {
      if(false == SendCmd(lCktCmd, lChannelId))
      {
         TERR(gTrace, printf("%s: Send Command Failed for Ckt:%d",
                  gProcessName, mChannelMgmt[i].channelId););
         return false;
      }
      lChannelId++;
   }
   return true; 
}

//-------------------------------------------------------------
// METHOD      : FetchReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
/*BOOLEAN IvrChannelHdlr::FetchReq(UINT32& lMinChannelId, UINT32& lMaxChannelId, UINT32& lTotalCkts)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   DbErrorCodes   lDbErr;

   memset(&mChannelMgmt, 0, (sizeof(ChannelMgmt) * MAX_NUM_OF_CKT_CMDS));

   lDbErr = mIvrDbManager.FetchChannelCmd(mChannelMgmt, lMinChannelId, lMaxChannelId) ;
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

   //snprintf(lLogText, MAX_LOG_TEXT_LEN, 
   //           "Fetched Request InCId:%ld patchCId:%ld Cmd:%d Status:%d",
   //           mChannelMgmt.channelId, mChannelMgmt.patchChannelId, 
   //           mChannelMgmt.cmd, mChannelMgmt.status);
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "Fetched Request TotalCnt:%ld", lTotalCkts);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(CHMG03, lLogText);
   return true; 
}*/

//--------------------------------------------------------------
// METHOD      : SendCmd
// DESCRIPTION : 
// PARAMETER   : 
// RETURN      : BOOLEAN
// THROWS      : none
//---------------------------------------------------------------
BOOLEAN IvrChannelHdlr::SendCmd(EnumChannelCmd& lCmd, UINT16& lChannelId)
{
   TEXT                    lLogText[MAX_LOG_TEXT_LEN] = "";
   BOOLEAN                 lSendRel  = false;
   //BOOLEAN                 lChStatus = false;
   EnumIVRChannelStatus    lstatus;
   EnumCktCmdStatus        lCktCmdStatus;

   memset(&mCktCmdMsg,  0, sizeof(CktCmdMsg));
   
   //mIvrChannelApi.SetChannelStatus(lChannelId, IVR_CHANNEL_CMD_IN_PROGRESS);
             
   switch(lCmd)
   {
      case CMD_BLOCK:
           {
              mCktCmdMsg.cktCmd = CKT_CMND_BLOCK;
              lstatus = IVR_CHANNEL_BLOCKED;
              lCktCmdStatus = CKT_CMND_BLOCKED;
              lSendRel = true;
           }
           break;
      case CMD_UN_BLOCK:
           {
              mCktCmdMsg.cktCmd = CKT_CMND_UNBLOCK;
              lstatus = IVR_CHANNEL_UN_BLOCKED;
              lCktCmdStatus = CKT_CMND_UNBLOCKED;
              lSendRel = false;
           }
           break;
      case CMD_RELEASE:
           {
              lstatus = IVR_CHANNEL_RELEASED;
              lCktCmdStatus = CKT_CMND_RELEASED;
              lSendRel = true;
           }
           break;
      case CMD_CHK_STATUS:
           {
              //Check Status - ss7
              lstatus = mIvrChannelApi.GetChannelStatus(lChannelId);
              lSendRel = false;
           }
           break;
      case CMD_UNKNOWN:
      default:
           snprintf(lLogText, MAX_LOG_TEXT_LEN, "CktId:%d Cmd:%d not Supported",
                       lChannelId, gChannelCmd);
           TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
           gLog.GenerateLog(CHMG05, lLogText);
           return false;
           break;
   }

   if(lSendRel == true)
   {
      KernelMsg   lKernelMsg;
      memset(&lKernelMsg, 0, sizeof(KernelMsg));

      mGenIsupMsg.isupMsgType      = GEN_ISUP_REL;
      mGenIsupMsg.circuitId        = lChannelId;
      mGenIsupMsg.isupRel.optLen   = 0 ;

      lKernelMsg.kernelMsgType = ISUP_MSG;
      lKernelMsg.isupMsg       = mGenIsupMsg;
      
      if(false == gIvrQueApi.WriteToCallAlertQue(lKernelMsg))
      {
         sprintf(lLogText, "CktId:%d Sending ISUP_REL to SessionMgr failed", 
               lKernelMsg.isupMsg.circuitId);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(CHMG02, lLogText);
         return false;
      }
      if(true != gIvrQueApi.WriteToIsupEncQue (mGenIsupMsg))
      {
         return false;
      }
      sprintf(lLogText, "CktId:%d Cmd:%d Sent ISUP REL to SessionMgr and N/W",
            mGenIsupMsg.circuitId, lCmd);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(CHMG01, lLogText);

      if(lCmd == CMD_RELEASE)
      {
         return true;
      }
   }

   mCktCmdMsg.channelId = lChannelId;
   if(true != gIvrQueApi.WriteToCktCmd(mCktCmdMsg))
   {
      T(gTrace, printf("%s: %s Sending Cmd:%d to ISUP Failed\n",
               gProcessName, lLogText, lCmd););
      gLog.GenerateLog(CHMG02, lLogText);
      return false;
   }
   T(gTrace, printf("%s: %s Sending Cmd:%d to ISUP Success\n",
            gProcessName, lLogText, lCmd););
   gLog.GenerateLog(CHMG01, lLogText);

   
   if(true != gIvrQueApi.ReadCktCmdRsp(mCktCmdRspMsg))
   {
      return false;
   }
   T(gTrace, printf("%s: %s Rcvd Rsp for Cmd:%d\n",
            gProcessName, lLogText, lCmd););
   gLog.GenerateLog(CHMG04, lLogText);

   //Check the CMD Status
   //

   if(mCktCmdRspMsg.status != lCktCmdStatus)
   {
      if(lSendRel == true)
      {
         lstatus = IVR_CHANNEL_FREE;
         lSendRel = false;
      }
   }
   mIvrChannelApi.SetChannelStatus(lChannelId, lstatus);

   return true;
}






