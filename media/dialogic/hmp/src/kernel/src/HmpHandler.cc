static const char *id = "@(#) $Id: HmpHandler.cc,v 1.1.2.4 2016/04/12 21:59:34 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpHandler.cc
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
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "HmpHandler.h"

extern  HmpHandler gHmpHandler;
pthread_t      gThread;

void* PollEvts(void *pContext)
{
   HmpHandler* pHmpHandler = (HmpHandler*) pContext;

   while(HmpUtil::KeepRunning())
   {
      memset(&pHmpHandler->mMpResponse, 0, sizeof(MpResponseDetails));
      if( true == pHmpHandler->mHmpDevice.StartSrEvent(pHmpHandler->mMpResponse)) 
         pHmpHandler->mHmpQueApi.WriteToMediaCmdRspQue(pHmpHandler->mMpResponse);
   }

   return NULL;
}

//----------------------------------------------------------------------
// METHOD NAME : HmpHandler
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpHandler::HmpHandler()
{
}

//----------------------------------------------------------------------
// METHOD NAME : ~HmpHandler
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpHandler::~HmpHandler()
{
}

//----------------------------------------------------------------------
// METHOD NAME : DeInit
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpHandler::DeInit()
{
/*   if( 0 != pthread_join(gThread, NULL))
   {
      TERR (gTrace, printf("%s: Wait till Thread Killed Failed\n", gProcessName););
      return false;
   }
   else
   {
      T (gTrace, printf("%s: Wait till Thread Kill Success\n", gProcessName););
   }
*/
   DeInitRtspSession();

   mHmpDevice.DeInit();

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpHandler::Init()
{

   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if(true != mHmpConfigApi.Init())
   {
      sprintf(lLogText, "ConfigApi Init Failed" );
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }
   
   if(true != this->StartSrlThread())
   {
      sprintf(lLogText, "Start Srl Thread Failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(true != mHmpDevice.Init())
   {
      sprintf(lLogText, "Device Init Failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_HAND01, lLogText);
      return false;
   }
   
   if(true != mHmpQueApi.Init())
   {
      sprintf(lLogText, "HmpQueApi Init Failed");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_HAND02, lLogText);
      return false;
   }
   
   if(true != this->InitRtspSession())
   {
      sprintf(lLogText, "Creating RTSP Session Failed" );
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_HAND02, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpHandler::ReloadConfig()
{
   //mHmpConfigApi.ReadConfigFromDb(gDbObj);
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : StartSrlThread
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpHandler::StartSrlThread()
{
   int            lRet = 0;
   void * (*lpStartRoutine) (void *) = PollEvts;
   TEXT           lLogText[MAX_LOG_TEXT_LEN] = "";

   lRet = pthread_create(&gThread, NULL, lpStartRoutine, &gHmpHandler);
   if(0 != lRet)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Cannot Create Thread pthread_create Failed Ret:%d",
            lRet);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_HAND01, lLogText);
      return false;
   }
 
   T (gTrace, printf("%s: \33[32m Start Srl Thread Success. ...\33[0m\n", gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DeInitRtspSession
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpHandler::DeInitRtspSession()
{
   for(int i = 1; i <= mHmpConfigApi.mIpmDeviceCount; i++)
   {
      mHmpDevice.TearSessions(i);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : InitRtspSession
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpHandler::InitRtspSession()
{
   MpRequestDetails  lMpRequestDetails;

   memset(&lMpRequestDetails, 0, sizeof(MpRequestDetails));
   
   mHmpDevice.mIpmLiveStreamCount = mHmpConfigApi.mIpmDeviceCount;

   for(int i = 1; i <= mHmpConfigApi.mIpmDeviceCount; i++)
   {
      printf("TBD 0000 %d %s\n", i, mHmpConfigApi.mRtspUrls[i]);
      strcpy(lMpRequestDetails.mpReqData.stream.rtspUrl,
            mHmpConfigApi.mRtspUrls[i]);
      lMpRequestDetails.cmdType = MC_STREAM;
      lMpRequestDetails.mpReqData.stream.circuitId = 0;
      mHmpDevice.PlayStream(lMpRequestDetails, false);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadRequest
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpHandler::ReadRequest()
{
   return mHmpQueApi.ReadFromMediaCmdReqQue(mMpRequest);
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessRequest
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpHandler::ProcessRequest()
{
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "SEQ:%06d CIC:%03d COMMAND:%s Received Request", 
         mMpRequest.mpReqData.stream.seqNum, 
         mMpRequest.mpReqData.stream.circuitId,
         HmpUtil::ConvertCmdToStr(mMpRequest.cmdType));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_HAND02, lLogText);

   switch(mMpRequest.cmdType)
   {
      case MC_INVAL_CMD_TYPE:
         break;

      case MC_CNTTN:
         mHmpDevice.PlayFile(mMpRequest);
         break;

      case MC_DISTN:
         mHmpDevice.Stop(mMpRequest.mpReqData.stopMsg.seqNum,
               mMpRequest.mpReqData.stopMsg.circuitId);
         break;

      case MC_CNTSP:
         break;

      case MC_DISSP:
         break;

      case MC_GETDIG:
         mHmpDevice.GetDigit(mMpRequest);
         break;

      case MC_GETDIGSTP:
         mHmpDevice.Stop(mMpRequest.mpReqData.stopMsg.seqNum,
               mMpRequest.mpReqData.stopMsg.circuitId);
         break;

      case MC_RECSTR:
         mHmpDevice.RecordFile(mMpRequest);
         break;

      case MC_RECSTP:
         mHmpDevice.Stop(mMpRequest.mpReqData.stopMsg.seqNum,
               mMpRequest.mpReqData.stopMsg.circuitId);
         break;

      case MC_SETVOL:
         break;

      case MC_SETSPD:
         break;

      case MC_PAUSE:
         break;

      case MC_RESUME:
         break;

      case MC_FORWARD:
         break;

      case MC_REWIND:
         break;

      case MC_STREAM:
         mHmpDevice.StreamReq(mMpRequest);
         break;
      case MC_STREAM_STOP:
         mHmpDevice.StopStreamReq(mMpRequest.mpReqData.stopMsg.seqNum,
               mMpRequest.mpReqData.stopMsg.circuitId);
         break;
      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "SEQ:%06d CIC:%03d COMMAND:%s Invalid Command", 
                  mMpRequest.mpReqData.stream.seqNum, 
                  mMpRequest.mpReqData.stream.circuitId,
                  HmpUtil::ConvertCmdToStr(mMpRequest.cmdType));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_HAND03, lLogText);
         }
         return false;
   }

   return true;
}
