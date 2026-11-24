static const char* id="@(#) $Id: AculabMpMgr.cc,v 1.1.4.3 2016/04/12 21:59:28 shijuc Exp $";
//-----------------------------------------------------------------------------
// NAME           : AculabMPMgr.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2012 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Roopa A M                                Date: 30-JUL-2012
//----------------------------------------------------------------------------
#include "AculabMpMgr.h"

UINT16     gIndex = 1;
acuChannel gChannel[ACULAB_MAX_NUM_CARDS * ACULAB_CHANNELS_PER_CARD];
acuStatus  gAcuChannel[ACULAB_MAX_STREAMS * ACULAB_TIMESLOTS_PER_STREAM];
CardDetails gCardDetails;

extern AculabMPMgr   gAculabMpMgrObj;

//-------------------------------------------------------------
// FUNCTION    : AculabMPMgr
// DESCRIPTION : Constructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
AculabMPMgr::AculabMPMgr()
{
   mSendRspFlag = false;
   memset(&mMpReq, 0, sizeof(MpRequestDetails));
   memset(&mMpAcuReq, 0, sizeof(MpRequestDetails));
   memset(&mMpAcuRsp, 0, sizeof(MpResponseDetails));
   memset(&gChannel, 0, sizeof(gChannel[ACULAB_MAX_NUM_CARDS * ACULAB_CHANNELS_PER_CARD]));
   memset(&gAcuChannel, 0, sizeof(gAcuChannel[ACULAB_MAX_NUM_CARDS * ACULAB_TIMESLOTS_PER_STREAM]));
}

//-------------------------------------------------------------
// FUNCTION    : AculabMPMgr
// DESCRIPTION : Destructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
AculabMPMgr::~AculabMPMgr()
{
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::ReadConfig()
{
   ConfigReturn      lCfgRet;
   CfgRead  lCfgRead(gProcessName);
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == lCfgRead.CfgInit(ACULAB_MEDIA_CFG))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, ACULAB_MEDIA_CFG););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }

   if(CFG_OK != lCfgRead.GetConfigNum("MEDIA_SEND_RSP_FLAG", mSendRspFlag, 0, 1))
   {
      mSendRspFlag = 0;
   }
   T(gTrace, printf("%s: MEDIA_SEND_RSP_FLAG = %d\n", 
            gProcessName, mSendRspFlag););

   if(CFG_OK != lCfgRead.GetConfigNum("COMPANDING_TYPE", mCompType, 1 , 8))
   {
      mCompType = 1;
   }
   T(gTrace, printf("%s: COMPANDING_TYPE = %ld\n", gProcessName, mCompType););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MAX_CHANNEL_PER_PROCESS", 
               mMaxChannels, 0, MAX_CHANNEL_PER_PRS)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "MAX_CHANNEL_PER_PROCESS", AcuMediaUtil::GetConfigError(lCfgRet), ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
      T(gTrace, printf("%s: MAX_CHANNEL_PER_PROCESS = %d\n", gProcessName, mMaxChannels););

   if (CFG_OK != lCfgRead.GetConfigNum("MEDIA_TRACE", mMediaTrace, 
            MEDIA_TRACE_MIN, MEDIA_TRACE_MAX))
   {
      mMediaTrace = 0;
   }
   T(gTrace, printf("%s: MEDIA_TRACE = %d[\33[32m%s\33[0m]\n", gProcessName, 
            mMediaTrace, AcuMediaUtil::ConvertMediaTypeToStr((EnumAcuMediaTrace)mMediaTrace)););

   if (CFG_OK != lCfgRead.GetConfigNum("TING_TRACE", mTingTrace, 
            TING_TRACE_MIN, TING_TRACE_MAX))
   {
      mTingTrace = TiNGtrace = 1;
   }
   else
   {
      TiNGtrace = mTingTrace;
   }
   T(gTrace, printf("%s: TING_TRACE = %d[\33[32m%s\33[0m]\n", gProcessName,
            mTingTrace, AcuMediaUtil::ConvertTingTypeToStr((EnumAcuTingTrace)mTingTrace)););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("ACU_NUM_DSP_PER_CARD", mModules, 1, 4)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "ACU_NUM_DSP_PER_CARD", AcuMediaUtil::GetConfigError(lCfgRet), ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
      T(gTrace, printf("%s: ACU_NUM_DSP_PER_CARD = %d\n", gProcessName, mModules););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("ACU_FILE_FORMAT", mDataFormat, 
               kSMDataFormatNone, kSMDataFormatSpeex)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "ACU_FILE_FORMAT", AcuMediaUtil::GetConfigError(lCfgRet), ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else 
      T(gTrace, printf("%s: ACU_FILE_FORMAT = %s\n", gProcessName, 
               AcuMediaUtil::ConvertDataTypeToStr((kSMDataFormat)mDataFormat)););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("ACU_PLAY_SKIP_TIME", mSkipTime, 1, 60)))
   {
      mSkipTime = 1;
      mSkipOffset = ACULAB_OFFSET_PER_SEC;
   }
   else
      mSkipOffset = (mSkipTime * ACULAB_OFFSET_PER_SEC);

   T(gTrace, printf("%s: ACU_PLAY_SKIP_TIME = %ld[Offset:%ld]\n", 
            gProcessName, mSkipTime, mSkipOffset););

   lCfgRead.CfgDeInit();

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::ReloadConfig()
{
   ConfigReturn  lCfgRet;
   CfgRead  lCfgRead(gProcessName);
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == lCfgRead.CfgInit(ACULAB_MEDIA_CFG))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s", ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: Could not open file %s\n",
               gProcessName, ACULAB_MEDIA_CFG););
      gLog.GenerateLog(GSYS09, lLogText);
      return false;
   }

   if(CFG_OK != lCfgRead.GetConfigNum("MEDIA_SEND_RSP_FLAG", mSendRspFlag, 0, 1))
   {
      mSendRspFlag = 0;
   }
   T(gTrace, printf("%s: MEDIA_SEND_RSP_FLAG = %d\n",
            gProcessName, mSendRspFlag););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("MAX_CHANNEL_PER_PROCESS", 
               mMaxChannels, 0, MAX_CHANNEL_PER_PRS)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "MAX_CHANNEL_PER_PROCESS", AcuMediaUtil::GetConfigError(lCfgRet), ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
      T(gTrace, printf("%s: MAX_CHANNEL_PER_PROCESS = %d\n", gProcessName, mMaxChannels););

   if (CFG_OK != lCfgRead.GetConfigNum("MEDIA_TRACE", mMediaTrace,
            MEDIA_TRACE_MIN, MEDIA_TRACE_MAX))
   {
      mMediaTrace = 0;
   }
   T(gTrace, printf("%s: MEDIA_TRACE = %d[\33[32m%s\33[0m]\n", gProcessName, 
            mMediaTrace, AcuMediaUtil::ConvertMediaTypeToStr((EnumAcuMediaTrace)mMediaTrace)););

   if (CFG_OK != lCfgRead.GetConfigNum("TING_TRACE", mTingTrace,
            TING_TRACE_MIN, TING_TRACE_MAX))
   {
      mTingTrace = TiNGtrace = 1;
   }
   else
   {
      TiNGtrace = mTingTrace;
   }
   T(gTrace, printf("%s: TING_TRACE = %d[\33[32m%s\33[0m]\n", gProcessName, 
            mTingTrace, AcuMediaUtil::ConvertTingTypeToStr((EnumAcuTingTrace)mTingTrace)););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("ACU_NUM_DSP_PER_CARD", mModules, 2, 8)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "ACU_NUM_DSP_PER_CARD", AcuMediaUtil::GetConfigError(lCfgRet), ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else
      T(gTrace, printf("%s: ACU_NUM_DSP_PER_CARD = %d\n", gProcessName, mModules););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("ACU_FILE_FORMAT", mDataFormat, 
               kSMDataFormatNone, kSMDataFormatSpeex)))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error in Reading Parameter %s : %s in %s",
            "ACU_FILE_FORMAT", AcuMediaUtil::GetConfigError(lCfgRet), ACULAB_MEDIA_CFG);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS11, lLogText);
      lCfgRead.CfgDeInit();
      return false;
   }
   else 
      T(gTrace, printf("%s: ACU_FILE_FORMAT = %s\n", gProcessName, 
               AcuMediaUtil::ConvertDataTypeToStr((kSMDataFormat)mDataFormat)););

   if(CFG_OK != ( lCfgRet = lCfgRead.GetConfigNum("ACU_PLAY_SKIP_TIME", mSkipTime, 1, 60)))
   {
      mSkipTime = 1;
      mSkipOffset = ACULAB_OFFSET_PER_SEC;
   }
   else
      mSkipOffset = (mSkipTime * ACULAB_OFFSET_PER_SEC);

   T(gTrace, printf("%s: ACU_PLAY_SKIP_TIME = %ld[Offset:%ld]\n", 
            gProcessName, mSkipTime, mSkipOffset););

   lCfgRead.CfgDeInit();

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : InitialiseChannel
// DESCRIPTION : Initializes All the channels 
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
INT16 AculabMPMgr::InitialiseChannel(acuStream & rNetStream, acuStream & rMediaStream, INT16 lModuleIndx)
{
   INT16 lResult = 0;
   UINT16  lAcuIndex = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   SM_CHANNEL_ALLOC_PLACED_PARMS lAcp;
   SM_CHANNEL_INFO_PARMS         lCip;
   SM_SWITCH_CHANNEL_PARMS       lSwp;
   OUTPUT_PARMS                  lCp;

   gChannel[gIndex].mediaSt = rMediaStream;
   gChannel[gIndex].netSt =  rNetStream;
   gChannel[gIndex].circuitId = gIndex;
   gChannel[gIndex].status = CKT_IDLE;

   memset(&lAcp, 0, sizeof(lAcp));
   lAcp.type   = kSMChannelTypeFullDuplex;
   lAcp.module = gChannel[gIndex].moduleId;
   lResult = sm_channel_alloc_placed(&lAcp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Error Unable to allocate channel <%d>", lResult);
      TERR(gTrace, printf("%s: \33[32m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP01, lLogText);
      return lResult;
   }

   gChannel[gIndex].channelId = lAcp.channel;
   lAcuIndex = sm_get_channel_ix (lAcp.channel);
   if (lAcuIndex < 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error sm_get_channel_ix() failed  <%d>", lAcuIndex);
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n",gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP02, lLogText);
      return lAcuIndex;
   }

   // Validate lAcuIndex. within limits of card * channel
   gAcuChannel[lAcuIndex].mediaIndx = gIndex;

   memset(&lSwp, 0, sizeof(lSwp));
   lSwp.st = rMediaStream.st;
   lSwp.ts = rMediaStream.ts;
   lSwp.type = (kSMTimeslotType) rMediaStream.type;
   lSwp.channel = lAcp.channel;
   lResult = sm_switch_channel_input(&lSwp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error sm_switch_channel_input() failed <%d>", lResult);
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP03, lLogText);

      this->ReleaseChannel (lSwp.channel);
      return lResult;
   }

   memset(&lSwp, 0, sizeof(lSwp));
   lSwp.st = rMediaStream.st;
   lSwp.ts = rMediaStream.ts;
   lSwp.type = (kSMTimeslotType) rMediaStream.type;
   lSwp.channel = lAcp.channel;
   lResult = sm_switch_channel_output(&lSwp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error sm_switch_channel_output() failed <%d>", lResult);
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP04, lLogText);
      this->ReleaseChannel (lSwp.channel);
      return lResult;
   }

   memset(&lCip, 0, sizeof(lCip));
   lCip.channel = lAcp.channel;
   lResult = sm_channel_info (&lCip);
   if ( lResult != 0 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error Could not get channel info <%d>", lResult);
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP05, lLogText);
      this->ReleaseChannel (lCip.channel);
      return lResult;
   }

   memset(&lCp, 0, sizeof(lCp));
   lCp.size = sizeof(lCp);
   lCp.ist = lCip.ost;
   lCp.its = lCip.ots;
   lCp.mode = CONNECT_MODE;
   lCp.ost = rNetStream.st;
   lCp.ots = rNetStream.ts;

   lResult = sw_set_output(mOpenCardParms.card_id, &lCp);
   if ( lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error could not make switch connection PLAY <%d>", lResult);
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP05, lLogText);
      return lResult;
   }

   memset(&lCp, 0, sizeof(lCp));
   lCp.size = sizeof(lCp);
   lCp.ist = rNetStream.st;
   lCp.its = rNetStream.ts;
   lCp.mode = CONNECT_MODE;
   lCp.ost = lSwp.st;
   lCp.ots = lSwp.ts;

   lResult = sw_set_output(mOpenCardParms.card_id, &lCp);
   if ( lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error could not make switch connection REC/DTMF <%d>", lResult);
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP05, lLogText);
      return lResult;
   }

   if(mMediaTrace == ACU_MEDIA_TRACE_INIT || mMediaTrace == ACU_MEDIA_TRACE_ALL)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Media Details");
      T( gTrace, printf("%s: \33[32mMedia[st:%d ts:%d] Net[st:%d ts:%d]\33[0m CktId:%d Status:%d "
               "ChannlId:%lu lAcuIndex:%d MediaIndx:%ld[gIndex:%d]\n", gProcessName, 
               gChannel[gIndex].mediaSt.st, gChannel[gIndex].mediaSt.ts, gChannel[gIndex].netSt.st, 
               gChannel[gIndex].netSt.ts, gChannel[gIndex].circuitId, gChannel[gIndex].status, 
               gChannel[gIndex].channelId, lAcuIndex, 
               gAcuChannel[lAcuIndex].mediaIndx, gIndex););
      gLog.GenerateLog(ACU_MP06, lLogText);
   }

   gIndex= gIndex+1;

   return lResult;
}

//-------------------------------------------------------------
// FUNCTION    : InitializeCard
// DESCRIPTION : Initializes prosodyX card
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::InitializeCard()
{
   INT16      lResult = 0;
   INT16      lModules = 0;
   INT16      lModuleIndx = 0;
   TEXT       lLogText[MAX_LOG_TEXT_LEN+1] = "";

   acuStream     lNetStream;
   acuStream     lMediaStream;  
   INT16      lBasicSt = ACULAB_TIMESLOTS_PER_STREAM;

   SM_CARD_INFO_PARMS            lCardInfoParms;
   SM_MODULE_INFO_PARMS          lModuleInfoParms;
   ACU_CARD_INFO_PARMS           lAcuCardInfoParms;
   ACU_OPEN_PROSODY_PARMS        lOpenProsodyParms;
   ACU_OPEN_SWITCH_PARMS         lOpenSwitchParms;

   memset(&gChannel, 0, sizeof(gChannel[ACULAB_MAX_NUM_CARDS * ACULAB_CHANNELS_PER_CARD]));
   memset(&gCardDetails, 0, sizeof(gCardDetails));
   memset(&lMediaStream, 0, sizeof(lMediaStream));
   memset(&lNetStream, 0, sizeof(lNetStream));

   INIT_ACU_STRUCT (&mSnapshotParms);
   lResult = acu_get_system_snapshot(&mSnapshotParms);
   if (lResult)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed to get the System SnapShot...Error is %d", lResult);
      TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP07, lLogText);
      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Success to get the system snap shot Details.TotalCards:%d", 
            mSnapshotParms.count);
      T(gTrace, printf("%s: \33[35m%s\33[0m\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP08, lLogText);

      pid_t lPid;

      for(; lModuleIndx < mModules; )
      {
         lPid = fork ();
         if(lPid > 0)
         {
            lModuleIndx++;
         }
         else if (lPid == 0)
         {
            for(UINT32 i = mCardId -  1 ; i < mCardId ; i++)
            {
               INIT_ACU_STRUCT(&mOpenCardParms);
               strncpy(mOpenCardParms.serial_no, mSnapshotParms.serial_no[i], ACU_MAX_SERIAL - 1);
               strncpy(gCardDetails.cardSerialNum, mSnapshotParms.serial_no[i], ACU_MAX_SERIAL - 1);

               lResult = acu_open_card(&mOpenCardParms);
               if (lResult)
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "Failed to open the card for SerialNo:%s...Error is %d",
                        mSnapshotParms.serial_no[i], lResult);
                  TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP09, lLogText);
                  return false;
               }
               gCardDetails.cardId = mOpenCardParms.card_id;
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "Success to open the card SerialNum:%s CardId:%lu",
                     mSnapshotParms.serial_no[i], mOpenCardParms.card_id);

               T(gTrace, printf("%s: \33[36m%s\33[0m\n",gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP10, lLogText);

               INIT_ACU_STRUCT( &lAcuCardInfoParms );
               lAcuCardInfoParms.card_id = mOpenCardParms.card_id;

               lResult = acu_get_card_info( &lAcuCardInfoParms );
               if (lResult == 0)
               {
                  INIT_ACU_STRUCT( &lOpenSwitchParms);
                  lOpenSwitchParms.card_id = mOpenCardParms.card_id;

                  if (lAcuCardInfoParms.resources_available & ACU_RESOURCE_SWITCH)
                  {  
                     lResult = acu_open_switch(&lOpenSwitchParms);
                     if( lResult != 0 )
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                              "Error Unable to open switch res <%d>", lResult);
                        TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
                        gLog.GenerateLog(ACU_MP11, lLogText);
                        return false;
                     }
                     else
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                              "Success to open the switch res <%d>", lResult);
                        T(gTrace, printf("%s: \33[36m%s\33[0m\n", gProcessName, lLogText););
                        gLog.GenerateLog(ACU_MP12, lLogText);
                     }

                     if (lAcuCardInfoParms.resources_available & ACU_RESOURCE_SPEECH)
                     {
                        INIT_ACU_STRUCT( &lOpenProsodyParms );
                        lOpenProsodyParms.card_id = mOpenCardParms.card_id;

                        lResult = acu_open_prosody( &lOpenProsodyParms );

                        if( lResult != 0 )
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                                 "Error Unable to open speech res <%d>", lResult);
                           TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
                           gLog.GenerateLog(ACU_MP13, lLogText);
                           return false;
                        }

                        snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                              "Success to open the speech res <%d>", lResult);
                        T(gTrace, printf("%s: \33[36m%s\33[0m\n", gProcessName, lLogText););
                        gLog.GenerateLog(ACU_MP14, lLogText);

                        INIT_ACU_SM_STRUCT( &lCardInfoParms );
                        lCardInfoParms.card = mOpenCardParms.card_id;

                        lResult = sm_get_card_info( &lCardInfoParms );

                        if( lResult != 0 )
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                 "Error Unable to get speech card information <%d>", lResult);
                           TERR(gTrace, printf("%s: \33[31m%s\33[0m\n",gProcessName, lLogText););
                           gLog.GenerateLog(ACU_MP15, lLogText);
                           return false;
                        }
                        else
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN, "Successfully obtained the "
                                 "speech card information CardId:%lu Module Cnt:%d", 
                                 lCardInfoParms.card, lCardInfoParms.module_count);
                           T(gTrace, printf("%s: \33[36m%s\33[0m\n", gProcessName, lLogText););
                           gLog.GenerateLog(ACU_MP16, lLogText);
                        }

                        lModules = lCardInfoParms.module_count;

                        if(lModuleIndx < lModules)
                        {
                           {
                              sprintf(gProcessName, "%s-%ld", gProcessName, getpid());

                              SM_OPEN_MODULE_PARMS lOpenModuleParms;
                              INIT_ACU_SM_STRUCT( &lOpenModuleParms );

                              lOpenModuleParms.module_ix = lModuleIndx;
                              lOpenModuleParms.card_id   = lCardInfoParms.card;

                              lResult = sm_open_module ( &lOpenModuleParms );
                              if( lResult != 0 )
                              {
                                 snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                       "Error Unable to Open Prosody Module <%d>", lResult);
                                 TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
                                 gLog.GenerateLog(ACU_MP17, lLogText);
                                 return false;
                              }

                              snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                    "Success to Open Prosody Module ModuleId:%d Index:%d", 
                                    lOpenModuleParms.module_id, lOpenModuleParms.module_ix);
                              T(gTrace, printf("%s: \33[36m%s\33[0m\n", gProcessName, lLogText););
                              gLog.GenerateLog(ACU_MP18, lLogText);

                              INIT_ACU_SM_STRUCT(&lModuleInfoParms);
                              lModuleInfoParms.module = lOpenModuleParms.module_id;

                              lResult = sm_get_module_info( &lModuleInfoParms );
                              if( lResult != 0 )
                              {
                                 snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                       "Error Failed to obtain module <%d>", lResult );
                                 TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
                                 gLog.GenerateLog(ACU_MP19, lLogText);
                                 return false;
                              }

                              snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                    "Success to Obtain module Information "
                                    "ModuleId:%d ModuleIndx:%d MinStr:%d",
                                    lModuleInfoParms.module, lModuleIndx,
                                    lModuleInfoParms.min_stream);
                              T(gTrace, printf("%s: \33[36m%s\33[0m\n", gProcessName, lLogText););
                              gLog.GenerateLog(ACU_MP20, lLogText);

                              ModuleInfo lAcuModuleDetails;
                              memset(&lAcuModuleDetails, 0, sizeof(lAcuModuleDetails));

                              lAcuModuleDetails.moduleId = lOpenModuleParms.module_id;
                              lAcuModuleDetails.moduleIdx = lOpenModuleParms.module_ix;

                              memset(&lNetStream, 0, sizeof(lNetStream));
                              memset(&lMediaStream, 0, sizeof(lMediaStream));

                              int lChannels = ACULAB_CHANNELS_PER_MODULE;
                              lMediaStream.st = lModuleInfoParms.min_stream;
                              lNetStream.st = lBasicSt + (lModuleInfoParms.min_stream % lBasicSt);
                              lNetStream.ts = lMediaStream.ts = 0;

                              for(lNetStream.ts = 0, lMediaStream.ts = 0; 
                                    lNetStream.ts < lChannels; 
                                    lNetStream.ts++, lMediaStream.ts++)
                              {
                                 if(lNetStream.ts >= ACULAB_TIMESLOTS_PER_STREAM)
                                 {
                                    lNetStream.st += 1;
                                    lMediaStream.st += 1;
                                    lChannels -= ACULAB_TIMESLOTS_PER_STREAM;
                                    lNetStream.ts = lMediaStream.ts = 0;
                                 }

                                 if(lNetStream.ts == 16 || lNetStream.ts == 0)
                                 {
                                    lMediaStream.ts +=1;
                                    lNetStream.ts +=1;
                                 }

                                 if (mCompType == COMPANDING_ALAW)
                                 {
                                    lMediaStream.type  = kSMTimeslotTypeALaw;
                                    lNetStream.type = kSMTimeslotTypeALaw;
                                 }
                                 else if (mCompType == COMPANDING_MULAW)
                                 {
                                    lMediaStream.type  = kSMTimeslotTypeMuLaw;
                                    lNetStream.type = kSMTimeslotTypeMuLaw;
                                 }

                                 memset(&gChannel[gIndex], 0, sizeof(gChannel[gIndex]));
                                 gChannel[gIndex].cardId = i;
                                 gChannel[gIndex].moduleId = lModuleInfoParms.module;

                                 lResult = InitialiseChannel(lNetStream, lMediaStream, lModuleIndx);
                                 if(lResult != 0)
                                 {
                                    snprintf(lLogText, MAX_LOG_TEXT_LEN, " Channel initialization failed");
                                    TERR (gTrace, printf("%s: \33[31mFailed to allocate channels\33[0m", 
                                             gProcessName););
                                    gLog.GenerateLog(ACU_MP21, lLogText);
                                    return false;
                                 }
                              }
                           }
                        }

                        CardDetails lAcuCardDetails;
                        memset(&lAcuCardDetails , 0, sizeof(lAcuCardDetails));

                        lAcuCardDetails = gCardDetails;

                        if(false == mAcuCardDetails.InsertCardDetails(lAcuCardDetails))
                        {
                           sprintf(lLogText, "Failed to insert the card details in HT for cardId:%lu",
                                 lAcuCardDetails.cardId);
                           TERR(gTrace, printf("%s: \33[31m%s\33[0m\n", gProcessName, lLogText););
                           gLog.GenerateLog(ACU_MP24, lLogText);
                           return false;
                        }
                        else
                        {
                           sprintf(lLogText, "Success to insert the card details in HT for cardId:%lu" 
                                 " Num of Modules:%d", lAcuCardDetails.cardId, lCardInfoParms.module_count);
                           T(gTrace, printf("%s: \33[35m%s\33[0m\n", gProcessName, lLogText););
                           gLog.GenerateLog(ACU_MP25, lLogText);
                        }
                     }
                  }
               }
            }

            if (false == mAcuEventApi.CreateProsodyEvents())
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "EventApi Initialisation failed");
               TERR(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", 
                        gProcessName, lLogText);)
                  gLog.GenerateLog(ACU_MP59, lLogText);
               return false;
            }

            if (false == CreateEventThread())
            {
               sprintf(lLogText, "Event Thread Creation Failed");
               TERR(gTrace,printf("%s: \33[31m%s\33[0m. Exiting...\n", 
                        gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP61, lLogText);
               return false;
            }

            while(AcuMediaUtil::KeepRunning())
            {
               int lMsgType = lModuleIndx + 1;
               if (true == ReadModuleReq(lMsgType))
                  ProcessModuleReq();
            }

            if(false == DeInit(lModuleIndx))
               TERR(gTrace,printf("%s: Failed To DeInit Libraries. Exiting...\n", gProcessName););

            exit(2);   
         }
      }
   }//mSnapshotParms.count

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandlePlayCtrls
// DESCRIPTION : Handles PlayCtrls On DTMF
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::HandlePlayCtrls(UINT16 lCktId, int lParam)
{
   MpResponseDetails lAcuRsp;
   TEXT  lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(&lAcuRsp, 0, sizeof(lAcuRsp));

   if(lParam == (unsigned int) gChannel[lCktId].acuCurPlay.play.playCtrl.pauseDtmf)
   {
      MpPause lPause;
      lPause.seqNum = gChannel[lCktId].acuCurPlay.play.seqNum;
      lPause.circuitId = gChannel[lCktId].acuCurPlay.play.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] Status:%s DTMF:%c"
            "\33[33mProcessing Play Ctrl: Issue PAUSE\33[0m",
            lPause.seqNum, lPause.circuitId, 
            AcuMediaUtil::ConvertStatusToStr (gChannel[lCktId].status), lParam);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP179, lLogText);

      if(MP_SUCCESS != HandlePause(lPause))
         return false;
   }
   else if (lParam == (unsigned int) gChannel[lCktId].acuCurPlay.play.playCtrl.resumeDtmf)
   {
      MpResume lResume;
      lResume.seqNum = gChannel[lCktId].acuCurPlay.play.seqNum;
      lResume.circuitId = gChannel[lCktId].acuCurPlay.play.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] Status:%s DTMF:%c"
            "\33[33mProcessing Play Ctrl: Issue RESUME\33[0m",
            lResume.seqNum, lResume.circuitId,
            AcuMediaUtil::ConvertStatusToStr (gChannel[lCktId].status), lParam);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP180, lLogText);

      if(MP_SUCCESS != HandleResume(lResume))
         return false;
   }
   else if(lParam == (unsigned int) gChannel[lCktId].acuCurPlay.play.playCtrl.forwardDtmf)
   {
      MpForward lForward;

      lForward.seqNum = gChannel[lCktId].acuCurPlay.play.seqNum;
      lForward.circuitId = gChannel[lCktId].acuCurPlay.play.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] Status:%s DTMF:%c"
            "\33[33mProcessing Play Ctrl: Issue FORWARD\33[0m",
            lForward.seqNum, lForward.circuitId,
            AcuMediaUtil::ConvertStatusToStr (gChannel[lCktId].status), lParam);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP181, lLogText);

      if(MP_SUCCESS != HandleForward(lForward))
         return false;
   }
   else if (lParam == (unsigned int) gChannel[lCktId].acuCurPlay.play.playCtrl.rewindDtmf)
   {
      MpRewind lRewind;

      lRewind.seqNum = gChannel[lCktId].acuCurPlay.play.seqNum;
      lRewind.circuitId = gChannel[lCktId].acuCurPlay.play.circuitId;

      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] Status:%s DTMF:%c"
            "\33[33mProcessing Play Ctrl: Issue FORWARD\33[0m",
            lRewind.seqNum, lRewind.circuitId,
            AcuMediaUtil::ConvertStatusToStr (gChannel[lCktId].status), lParam);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP182, lLogText);

      if(MP_SUCCESS != HandleRewind(lRewind))
         return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] Status:%s DTMF:%c"
            "\33[31mPLAY CONTROL INVALID OPTION\33[0m",
            gChannel[lCktId].acuCurPlay.play.seqNum,
            gChannel[lCktId].acuCurPlay.play.circuitId,
            AcuMediaUtil::ConvertStatusToStr (gChannel[lCktId].status), lParam);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP183, lLogText);
      return false;
   }

   lAcuRsp.rspMsg = MP_RSP_SUCCESS;

   this->WriteRsp(lAcuRsp);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandlePlayStopOnDtmf
// DESCRIPTION : Handles Play Stop On Detecting DTMF Character Set
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::HandlePlayStopOnDtmf(UINT16 lCktId)
{
   int lLen = 0;
   TEXT *lDtmfPtr = NULL;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lpstrDtmfCharSet[MAX_DTMF_RSP_DIGITS + 1] = "";

   if(gChannel[lCktId].acuCurDtmf.dtmf.numOfDig == 0)
      strcpy(lpstrDtmfCharSet, "0123456789*#abcd");
   else
      strcpy(lpstrDtmfCharSet, 
            gChannel[lCktId].acuCurPlay.play.dtmfStopPlayCharSet);

   lLen = strlen(lpstrDtmfCharSet);

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d-%02d] Status:%s"
         "\33[36m HandlePlayStopOnDtmf Flg:%d CharSet:%s Len:%d[%d] Dtmf:%s\33[0m",
         gChannel[lCktId].acuCurDtmf.dtmf.seqNum, 
         gChannel[lCktId].acuCurDtmf.dtmf.circuitId, lCktId,
         AcuMediaUtil::ConvertStatusToStr(gChannel[lCktId].status),
         gChannel[lCktId].acuCurPlay.play.dtmfEnable, lpstrDtmfCharSet,
         strlen(gChannel[lCktId].acuCurDtmf.detectedDigits),
         gChannel[lCktId].acuCurDtmf.dtmf.numOfDig,
         gChannel[lCktId].acuCurDtmf.detectedDigits);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP177, lLogText);

   for(int i = 0; i < lLen; i++)
   {
      printf("TBR:--indx:%d detectedDigits:%s(%c) HandlePlayStopOnDtmf Len:%d(%d) DtmfSet:%s\n", 
            i, gChannel[lCktId].acuCurDtmf.detectedDigits, lpstrDtmfCharSet[i], 
            lLen, strlen(lpstrDtmfCharSet), gChannel[lCktId].acuCurPlay.play.dtmfStopPlayCharSet);

      if('\0' == gChannel[lCktId].acuCurPlay.play.dtmfStopPlayCharSet[0])
      {
         if(true != StopPlay(lCktId))
            return false;
      }
      else
      {
         lDtmfPtr = strstr(gChannel[lCktId].acuCurDtmf.detectedDigits, &lpstrDtmfCharSet[i]);

         printf("TBR:-222- HandlePlayStopOnDtmf Len:%d(%d) lDtmfPtr:%p\n", lLen, strlen(lpstrDtmfCharSet), lDtmfPtr);

         if(lDtmfPtr != NULL)
         {
            if(true != StopPlay(lCktId))
               return false; 
         }
      }
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d-%02d] Status:%s"
         "\33[32m HandlePlayStopOnDtmf Success\33[0m", 
         gChannel[lCktId].acuCurDtmf.dtmf.seqNum,
         gChannel[lCktId].acuCurDtmf.dtmf.circuitId, lCktId,
         AcuMediaUtil::ConvertStatusToStr(gChannel[lCktId].status));
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP178, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandleDtmfDetection
// DESCRIPTION : Handles Dtmf Detection on the specified channel
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::HandleDtmfDetection(SM_RECOGNISED_PARMS & rp, MpResponseDetails & rAcuRsp)
{
   INT16 lResult = 0;
   const TEXT *type;
   int lIndex = 0, lMediaIndx = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   switch(rp.type)
   {
      case kSMRecognisedNothing:
         type = 0;
         break;
      case kSMRecognisedTrainingDigit:
         type = "TrainingDigit";
         break;
      case kSMRecognisedDigit:
         {
            SM_CHANNEL_INFO_PARMS         ci;
            memset(&ci, 0, sizeof(ci));

            type = 0;
            lIndex = sm_get_channel_ix(rp.channel);
            if(lIndex < 0)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "EVT-THREAD DTMF: Invalid Channel Index:%d", lIndex);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP26, lLogText);
            }

            lMediaIndx = gAcuChannel[lIndex].mediaIndx;

            ci.channel = rp.channel;
            lResult = sm_channel_info (&ci);
            if (lResult != 0)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "EVT-THREAD DTMF: Error Could not get channel info <%d>", lResult);
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP27, lLogText);
            }

            // PlayCtrls
            if (gChannel[lMediaIndx].acuCurPlay.play.playCtrlEnable)
            {
               int lParam = rp.param0;

               T(gTrace, printf("RefNum:%08X CktId:[%03d] \33[33mMC_GETDIG "
                        " Detected Digit: [%c] (%d:%d) Param1:%dms\33[0m\n",
                        gChannel[lMediaIndx].acuCurDtmf.dtmf.seqNum, lIndex,
                        rp.param0, ci.ist, ci.its, rp.param1););

               if (true == HandlePlayCtrls(lMediaIndx, lParam))
                  return true;
            }

            printf("\33[32mTBR:================= DetectedLen:%d NumOfDig:%d\33[0m\n", 
                  gChannel[lMediaIndx].acuCurDtmf.detectedDigitsLen, 
                  gChannel[lMediaIndx].acuCurDtmf.dtmf.numOfDig);
            if(gChannel[lMediaIndx].acuCurDtmf.detectedDigitsLen < 
                  gChannel[lMediaIndx].acuCurDtmf.dtmf.numOfDig || 
                  gChannel[lMediaIndx].acuCurDtmf.dtmf.numOfDig == 0)
            {
               TEXT lTmpData[MAX_DTMF_RSP_DIGITS] = "";

               strncpy(lTmpData, gChannel[lMediaIndx].acuCurDtmf.detectedDigits, MAX_DTMF_RSP_DIGITS);
               gChannel[lMediaIndx].acuCurDtmf.detectedDigitsLen += 1;

               snprintf(gChannel[lMediaIndx].acuCurDtmf.detectedDigits, MAX_DTMF_RSP_DIGITS, 
                     "%s%c", lTmpData, rp.param0);

               T(gTrace, printf("RefNum:%08X CktId:[%03d] \33[33mMC_GETDIG "
                        " Buffer:%s Detected Digit: [%c] (%d:%d) Param1:%dms\33[0m\n",
                        gChannel[lMediaIndx].acuCurDtmf.dtmf.seqNum,
                        gChannel[lMediaIndx].acuCurDtmf.dtmf.circuitId,
                        gChannel[lMediaIndx].acuCurDtmf.detectedDigits,
                        rp.param0, ci.ist, ci.its, rp.param1););

               // PlayStopOnDtmf
               if (gChannel[lMediaIndx].acuCurPlay.play.dtmfEnable)
               {
                  gChannel[lMediaIndx].circuitId  = gChannel[lMediaIndx].acuCurDtmf.dtmf.circuitId;

                  return  HandlePlayStopOnDtmf (lMediaIndx);
               }

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] MC_GETDIG "
                     "DigitsToGet:%d Status:%s Wait for Dtmf",
                     gChannel[lMediaIndx].acuCurDtmf.dtmf.seqNum,
                     gChannel[lMediaIndx].acuCurDtmf.dtmf.circuitId, 
                     gChannel[lMediaIndx].acuCurDtmf.dtmf.numOfDig,
                     AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status));
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP30, lLogText);

               if(gChannel[lMediaIndx].acuCurDtmf.detectedDigitsLen == 
                     gChannel[lMediaIndx].acuCurDtmf.dtmf.numOfDig)
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] MC_GETDIG "
                        "Collected NoD:%d Dtmf:%s", gChannel[lMediaIndx].acuCurDtmf.dtmf.seqNum, 
                        lMediaIndx, gChannel[lMediaIndx].acuCurDtmf.dtmf.numOfDig, 
                        gChannel[lMediaIndx].acuCurDtmf.detectedDigits);
                  T (gTrace, printf("%s: \33[32m%s\33[0m\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP28, lLogText);

                  rAcuRsp.cmdType             = MC_GETDIGSTP;
                  rAcuRsp.mpRspData.getDigit  = gChannel[lMediaIndx].acuCurDtmf.dtmf;
                  rAcuRsp.rspMsg              = MP_RSP_SUCCESS;
                  rAcuRsp.mpRspData.getDigit.numOfDig = gChannel[lMediaIndx].acuCurDtmf.dtmf.numOfDig;
                  strcpy(rAcuRsp.mpRspData.getDigit.rspDigits, 
                        gChannel[lMediaIndx].acuCurDtmf.detectedDigits);

                  gChannel[lMediaIndx].playStatus = ACU_PLAY_NEXT_NOT_REQD;
                  gChannel[lMediaIndx].circuitId  = gChannel[lMediaIndx].acuCurDtmf.dtmf.circuitId;

                  if(CKT_GETDIG == gChannel[lMediaIndx].status)
                     gChannel[lMediaIndx].status          = CKT_IDLE;
                  else if(CKT_PLAYING_AND_GETDIG == gChannel[lMediaIndx].status)
                     gChannel[lMediaIndx].status          = CKT_PLAYING;
                  else if(CKT_PLAY_STOPING == gChannel[lMediaIndx].status)
                     gChannel[lMediaIndx].status          = CKT_IDLE;
                  else if(CKT_PLAY_STOPING_AND_GETDIG == gChannel[lMediaIndx].status)
                     gChannel[lMediaIndx].status          = CKT_PLAY_STOPING;
                  else if(CKT_RECORDING_AND_GETDIG == gChannel[lMediaIndx].status)
                     gChannel[lMediaIndx].status = CKT_RECORDING;
                  else if(CKT_RECORD_STOPING_AND_GETDIG == gChannel[lMediaIndx].status)
                     gChannel[lMediaIndx].status = CKT_RECORD_STOPING;
                  else
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "EVT-THREAD DTMF: RefNum:%08X CktId:[%03d] Status:%s"
                           "\33[33mState Changed HandlePlayStopped\33[0m",
                           gChannel[lIndex].acuCurDtmf.dtmf.seqNum, lMediaIndx, 
                           AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status));
                     TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(ACU_MP29, lLogText);
                  }

                  T(gTrace, printf("%s: EVT-THREAD DTMF: \33[35mRefNum:%08X CktId:[%03d] "
                           "Status:%s Success to Handle GETDTMF\33[0m\n", gProcessName,
                           rAcuRsp.mpRspData.getDigit.seqNum, rAcuRsp.mpRspData.getDigit.circuitId,
                           AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status)););
                  gLog.GenerateLog(ACU_MP56, 4);
                  this->WriteRsp(rAcuRsp);

                  gChannel[lMediaIndx].acuCurDtmf.detectedDigitsLen = 0;
                  memset(&(gChannel[lMediaIndx].acuCurDtmf.detectedDigits), 0, sizeof(MAX_DTMF_RSP_DIGITS));
                  memset(&(gChannel[lMediaIndx].acuCurDtmf.dtmf), 0, sizeof(MpGetDigit));
                  return true;
               }
            }
         }
         break;
      case kSMRecognisedTone:
         type = "Tone";
         break;
      case kSMRecognisedCPTone:
         type = "CPTone";
         break;
      case kSMRecognisedGruntStart:
         type = "GruntStart";
         break;
      case kSMRecognisedGruntEnd:
         type = "GruntEnd";
         break;
      case kSMRecognisedASRResult:
         type = "ASRResult";
         break;
      case kSMRecognisedASRUncertain:
         type = "ASRUncertain";
         break;
      case kSMRecognisedASRRejected:
         type = "ASRRejected";
         break;
      case kSMRecognisedASRTimeout:
         type = "ASRTimeout";
         break;
      case kSMRecognisedCatSig:
         type = "CatSig";
         break;
      case kSMRecognisedOverrun:
         type = "Overrun";
         break;
      default:
         TERR(gTrace, printf("EVT-THREAD DTMF Error: sm_get_recognised produced event %d\n", rp.type););
         gLog.GenerateLog(ACU_MP184, lLogText);
         break;
   }

   if(rp.type != kSMRecognisedDigit)
      printf("%s: \33[32m%d %d\33[0m\n", type, rp.param0, rp.param1);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandleRecordFinish
// DESCRIPTION : Handles Record Completion on the specified channel
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::HandleRecordFinish(UINT16 lChannlIndex, MpResponseDetails & rAcuRsp)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   acuChannel *pChannel = &gChannel[lChannlIndex];

   switch(pChannel->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "EVT-THREAD REC: RefNum:%08X CktId:[%03d-%02d] Status:%s",
                  pChannel->acuCurRecord.record.seqNum,
                  pChannel->acuCurRecord.record.circuitId,
                  lChannlIndex, AcuMediaUtil::ConvertStatusToStr(pChannel->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP32, lLogText);
            return false;
         }
         break;

      case CKT_RECORDING:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_RECORD_STOPING:
      case CKT_RECORD_STOPING_AND_GETDIG:
         {
            rAcuRsp.rspMsg   = MP_RSP_SUCCESS;
            rAcuRsp.cmdType  = MC_RECSTP;
            rAcuRsp.mpRspData.recordMsg   = pChannel->acuCurRecord.record;

            pChannel->playStatus = ACU_PLAY_NEXT_NOT_REQD;
            pChannel->circuitId  = lChannlIndex;

            if(CKT_RECORDING == pChannel->status)
               pChannel->status          = CKT_IDLE;
            else if(CKT_RECORDING_AND_GETDIG == pChannel->status)
               pChannel->status          = CKT_GETDIG;
            else if(CKT_RECORD_STOPING_AND_GETDIG == pChannel->status)
               pChannel->status          = CKT_GETDIG;
            else if(CKT_RECORD_STOPING == pChannel->status)
               pChannel->status          = CKT_IDLE;
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "EVT-THREAD REC: RefNum:%08X CktId:[%03d-%02d] Status:%s "
                     "\33[33mState Changed Abbrupt in HandleRecordFinish\33[0m\n",
                     pChannel->acuCurRecord.record.seqNum,
                     pChannel->acuCurRecord.record.circuitId,
                     lChannlIndex, AcuMediaUtil::ConvertStatusToStr(pChannel->status));
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP33, lLogText);
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "EVT-THREAD REC: RefNum:%08X CktId:[%03d-%02d] ChannelId:%lu[\33[33m%d:%d-%d:%d\33[0m]"
                  "Status:%s", pChannel->acuCurRecord.record.seqNum, 
                  pChannel->acuCurRecord.record.circuitId,
                  lChannlIndex, gChannel[lChannlIndex].channelId,
                  gChannel[lChannlIndex].netSt.st, gChannel[lChannlIndex].netSt.ts,
                  gChannel[lChannlIndex].mediaSt.st, gChannel[lChannlIndex].mediaSt.ts,
                  AcuMediaUtil::ConvertStatusToStr(gChannel[lChannlIndex].status));
            T(gTrace, printf("%s: %s Record Completed Successfully\n", 
                     gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP34, lLogText);
         }
         break;

      case CKT_IDLE:
      case CKT_GETDIG:
      case CKT_PLAYING:
      case CKT_PLAY_STOPING:
      case CKT_PLAYING_AND_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "EVT-THREAD REC: RefNum:%08X CktId:[%03d-%02d] Not Recording any thing Status:%s",
                  pChannel->acuCurRecord.record.seqNum, 
                  pChannel->acuCurRecord.record.circuitId, lChannlIndex, 
                  AcuMediaUtil::ConvertStatusToStr(pChannel->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP35, lLogText);
            return false;
         }

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "EVT-THREAD REC: RefNum:%08X CktId:[%03d-%02d] Status:%s STP Invalid Channel Status",
                  pChannel->acuCurRecord.record.seqNum, 
                  pChannel->acuCurRecord.record.circuitId, lChannlIndex, 
                  AcuMediaUtil::ConvertStatusToStr(pChannel->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP36, lLogText);
            return false;
         }
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandlePlayFinish
// DESCRIPTION : Handles play finish on the specified channel
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::HandlePlayFinish(UINT16 lChannlIndex, MpResponseDetails & rAcuRsp)
{
   INT16 lResult = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   acuChannel *pChannel = &gChannel[lChannlIndex];

   switch(pChannel->status)
   {
      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
         {
            if(ACU_PLAY_MULTIPLE_FILES == pChannel->playStatus && 
                  pChannel->acuCurPlay.fileCnt != 0)
            {
               int lFileIndx = pChannel->acuCurPlay.play.vFileCount - pChannel->acuCurPlay.fileCnt;

               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s "
                     "\33[32m Starting Next File \33[0m [%d:%s] to Play [Cnt:%d] PlayStatus:%d "
                     "[st:%d ts:%d]", pChannel->acuCurPlay.play.seqNum, 
                     pChannel->acuCurPlay.play.circuitId, lChannlIndex, 
                     AcuMediaUtil::ConvertStatusToStr(pChannel->status), pChannel->acuCurPlay.fileCnt, 
                     pChannel->acuCurPlay.play.voiceFile[lFileIndx],
                     pChannel->acuCurPlay.play.vFileCount, pChannel->playStatus, 
                     pChannel->mediaSt.st, pChannel->mediaSt.ts);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP37, lLogText);

               if(NULL != pChannel->acuCurPlay.fp.fd)
               {
                  fclose(pChannel->acuCurPlay.fp.fd);
                  pChannel->acuCurPlay.fp.fd = NULL;
               }
               memset(&pChannel->acuCurPlay.fp, 0, sizeof(SM_FILE_REPLAY_PARMS));

               if(!pChannel->acuCurPlay.play.voiceFile[lFileIndx])
                  pChannel->acuCurPlay.fp.fd = NULL;
               else
               {
                  pChannel->acuCurPlay.fp.fd = 
                     fopen(pChannel->acuCurPlay.play.voiceFile[lFileIndx], "rb");

                  if (NULL == pChannel->acuCurPlay.fp.fd)
                     return false;
               }

               pChannel->acuCurPlay.fp.replay_parms.channel = pChannel->channelId;
               pChannel->acuCurPlay.fp.replay_parms.type    = (kSMDataFormat) mDataFormat;

               lResult = sm_replay_file_start (&pChannel->acuCurPlay.fp);
               if ( lResult != 0 )
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] "
                        "CMD:sm_replay_file_start failed %s <%d>", 
                        pChannel->acuCurPlay.play.seqNum, 
                        pChannel->acuCurPlay.play.circuitId, lChannlIndex,
                        pChannel->acuCurPlay.play.voiceFile[lFileIndx], lResult);
                  TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP38, lLogText);
                  return false;
               }
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] "
                     "CMD:sm_replay_file_start Issued %s <%d>", 
                     pChannel->acuCurPlay.play.seqNum,
                     pChannel->acuCurPlay.play.circuitId, lChannlIndex,
                     pChannel->acuCurPlay.play.voiceFile[lFileIndx], lResult);
               T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP39, lLogText);
               return false;
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "\33[32mEVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] MC_CNTTN "
                     "Status:%s Play Completed Successfully RptPly:%d\33[0m", 
                     pChannel->acuCurPlay.play.seqNum, 
                     pChannel->acuCurPlay.play.circuitId, lChannlIndex, 
                     AcuMediaUtil::ConvertStatusToStr(pChannel->status), 
                     pChannel->acuCurPlay.play.repeatPlay);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP40, lLogText);

               if(NULL != pChannel->acuCurPlay.fp.fd)
               {
                  fclose(pChannel->acuCurPlay.fp.fd);
                  pChannel->acuCurPlay.fp.fd = NULL;
               }

               if(0 == pChannel->acuCurPlay.fileCnt && 
                     pChannel->playStatus == ACU_PLAY_MULTIPLE_FILES)
               {
                  pChannel->playStatus = ACU_PLAY_MULTIPLE_FILES_COMPLETED;
               }

               if(true == pChannel->acuCurPlay.play.repeatPlay)
               {
                  if(true != StartPlay(pChannel->acuCurPlay.play))
                  {
                     pChannel->playStatus     = ACU_PLAY_NEXT_NOT_REQD;
                     pChannel->circuitId      = lChannlIndex;

                     if(CKT_PLAYING == pChannel->status)
                        pChannel->status  = CKT_IDLE;
                     else if(CKT_PLAYING_AND_GETDIG == pChannel->status)
                        pChannel->status  = CKT_GETDIG;
                     else if(CKT_PLAY_STOPING == pChannel->status)
                        pChannel->status  = CKT_IDLE;
                     else if(CKT_PLAY_STOPING_AND_GETDIG == pChannel->status)
                        pChannel->status  = CKT_GETDIG;
                     else
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] Status:%s"
                              "\33[31mState Changed Abbrupt in HandlePlayFinish\33[0m\n",
                              pChannel->acuCurPlay.play.seqNum,
                              pChannel->acuCurPlay.play.circuitId, lChannlIndex, 
                              AcuMediaUtil::ConvertStatusToStr(pChannel->status));
                        TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                        gLog.GenerateLog(ACU_MP41, lLogText);
                     }
                     memset(&(pChannel->acuCurPlay), 0, sizeof(MpPlay));

                     rAcuRsp.rspMsg              = MP_RSP_FAILURE;
                     rAcuRsp.cmdType             = MC_CNTTN;
                     rAcuRsp.mpRspData.playMsg   = pChannel->acuCurPlay.play;
                     return true;
                  }
                  else
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] Status:%s "
                           "\33[32mRepeating Play FlCnt:%d\33[0m", 
                           pChannel->acuCurPlay.play.seqNum, 
                           pChannel->acuCurPlay.play.circuitId,
                           lChannlIndex,
                           AcuMediaUtil::ConvertStatusToStr(pChannel->status),
                           pChannel->acuCurPlay.play.vFileCount);
                     T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(ACU_MP42, lLogText);
                     return false;
                  }
               }
               else
               {
                  rAcuRsp.rspMsg                       = MP_RSP_SUCCESS;
                  rAcuRsp.cmdType                      = MC_DISTN;
                  rAcuRsp.mpRspData.stopMsg.seqNum     = pChannel->acuCurPlay.play.seqNum;
                  rAcuRsp.mpRspData.stopMsg.circuitId  = pChannel->acuCurPlay.play.circuitId;

                  pChannel->playStatus       = ACU_PLAY_NEXT_NOT_REQD;

                  if(NULL != pChannel->acuCurPlay.fp.fd)
                  {
                     fclose(pChannel->acuCurPlay.fp.fd);
                     pChannel->acuCurPlay.fp.fd = NULL;
                  }

                  pChannel->circuitId        = lChannlIndex;
                  if(CKT_PLAYING == pChannel->status)
                     pChannel->status          = CKT_IDLE;
                  else if(CKT_PLAYING_AND_GETDIG == pChannel->status)
                     pChannel->status          = CKT_GETDIG;
                  else if(CKT_PLAY_STOPING == pChannel->status)
                     pChannel->status          = CKT_IDLE;
                  else if(CKT_PLAY_STOPING_AND_GETDIG == pChannel->status)
                     pChannel->status          = CKT_GETDIG;
                  else
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] Status:%s"
                           "\33[33mState Changed HandlePlayStopped\33[0m\n",
                           pChannel->acuCurPlay.play.seqNum,
                           pChannel->acuCurPlay.play.circuitId,
                           lChannlIndex, AcuMediaUtil::ConvertStatusToStr(pChannel->status));
                     TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     gLog.GenerateLog(ACU_MP43, lLogText);
                  }
                  memset(&(pChannel->acuCurPlay.play), 0, sizeof(MpPlay));
               }
            }
         }
         break;

      case CKT_PLAY_STOPING:
      case CKT_PLAY_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] \33[32mPlay Stopped Successfully "
                  "RptPly:%d Status:%s PlaySt:%s\33[0m", pChannel->acuCurPlay.play.seqNum, 
                  pChannel->acuCurPlay.play.circuitId, lChannlIndex, 
                  pChannel->acuCurPlay.play.repeatPlay,
                  AcuMediaUtil::ConvertStatusToStr(pChannel->status), 
                  AcuMediaUtil::ConvertPlayStatusToStr(pChannel->playStatus));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP44, lLogText);

            if(pChannel->acuCurPlay.playCtrl == ACU_PLAY_RESUME)
            {
               if(NULL != pChannel->acuCurPlay.fp.fd)
               {
                  fclose(pChannel->acuCurPlay.fp.fd);
                  pChannel->acuCurPlay.fp.fd = NULL;
               }
               pChannel->acuCurPlay.playCtrl = ACU_PLAY_NONE;
            }

            if(pChannel->playStatus == ACU_PLAY_NEXT_IMMEDIATE || 
                  pChannel->playStatus == ACU_PLAY_NEXT_REQD)
            {
               rAcuRsp.mpRspData.playMsg   = pChannel->acuNxtPlay.play;
               rAcuRsp.rspMsg              = MP_RSP_SUCCESS;
               rAcuRsp.cmdType             = MC_CNTTN;

               memset(&(pChannel->acuCurPlay.play), 0, sizeof(MpPlay));
               pChannel->playStatus      = ACU_PLAY_NEXT_NOT_REQD;
               if(CKT_PLAY_STOPING == pChannel->status)
                  pChannel->status          = CKT_IDLE;
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] Status:%s "
                        "\33[33mState Changed HandlePlayStopped\33[0m",
                        pChannel->acuCurPlay.play.seqNum,
                        pChannel->acuCurPlay.play.circuitId,
                        lChannlIndex, AcuMediaUtil::ConvertStatusToStr(pChannel->status));
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP45, lLogText);
               }

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] Status:%s PlayStatus:%s"
                     "Starting the Handle for Next Play\n", pChannel->acuCurPlay.play.seqNum,
                     pChannel->acuCurPlay.play.circuitId, lChannlIndex, 
                     AcuMediaUtil::ConvertStatusToStr(pChannel->status),
                     AcuMediaUtil::ConvertPlayStatusToStr(pChannel->playStatus));
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP46, lLogText);

               if(MP_SUCCESS != this->HandlePlay(pChannel->acuNxtPlay.play))
               {
                  rAcuRsp.rspMsg              = MP_RSP_FAILURE;
                  rAcuRsp.cmdType             = MC_CNTTN;
                  rAcuRsp.mpRspData.playMsg   = pChannel->acuNxtPlay.play;
                  return true;
               }
            }
            else
            {
               rAcuRsp.rspMsg                       = MP_RSP_SUCCESS;
               rAcuRsp.cmdType                      = MC_DISTN;
               rAcuRsp.mpRspData.stopMsg.seqNum     = pChannel->acuCurPlay.play.seqNum;
               rAcuRsp.mpRspData.stopMsg.circuitId  = pChannel->acuCurPlay.play.circuitId;

               if(NULL != pChannel->acuCurPlay.fp.fd)
               {
                  fclose(pChannel->acuCurPlay.fp.fd);
                  pChannel->acuCurPlay.fp.fd = NULL;
               }

               if(CKT_PLAY_STOPING == pChannel->status)
                  pChannel->status = CKT_IDLE;
               else if(CKT_PLAY_STOPING_AND_GETDIG == pChannel->status)
                  pChannel->status = CKT_GETDIG;
            }
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "EVT-THREAD PLAY: RefNum:%08X CktId:[%03d-%02d] Status:%s"
                  "\33[31mPlay Stopped Invalid Channel Status\33[0m",
                  pChannel->acuCurPlay.play.seqNum,
                  pChannel->acuCurPlay.play.circuitId,
                  lChannlIndex, AcuMediaUtil::ConvertStatusToStr(pChannel->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP47, lLogText);
            return false;
         }
         break;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : StartEventThread
// DESCRIPTION : Check Channel Status Thread
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *StartEventThread (void *lPtr)
{
   int      lResult = 0;
   struct   pollfd pfd[3];
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT     lProcessName[ACULAB_MAX_PROCESS_NAME + 1] = "";

   snprintf(lProcessName, ACULAB_MAX_PROCESS_NAME, "%s-%ld", gProcessName, syscall(224));

   memset(&pfd, 0, sizeof(pfd));
   pfd[0].fd = gAculabMpMgrObj.mAcuEventApi.mPlayEvent.fd;
   pfd[0].events = gAculabMpMgrObj.mAcuEventApi.mPlayEvent.mode;
   pfd[1].fd = gAculabMpMgrObj.mAcuEventApi.mDtmfEvent.fd;
   pfd[1].events = gAculabMpMgrObj.mAcuEventApi.mDtmfEvent.mode;
   pfd[2].fd = gAculabMpMgrObj.mAcuEventApi.mRecEvent.fd;
   pfd[2].events = gAculabMpMgrObj.mAcuEventApi.mRecEvent.mode;

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "EVT-THREAD: \33[33mPlay[Fd:%d Mode:%d] Dtmf[Fd:%d Mode:%d]"
         " Record[Fd:%d Mode:%d]\33[0m", pfd[0].fd, pfd[0].events, pfd[1].fd, pfd[1].events,
         pfd[2].fd, pfd[2].events);
   T(gTrace, printf("%s: Thread Info %s\n", lProcessName, lLogText););
   gLog.GenerateLog(ACU_MP48, lLogText);

   for(;;)
   {
      lResult = poll(pfd, 3, -1);
      if (lResult < 0)
      {
         TERR (gTrace, printf("%s: \33[31m EVT-THREAD POLL() failed Cannot wait for channels\33[0m\n",
                  lProcessName););
         gLog.GenerateLog(ACU_MP49, lLogText);                  
         pthread_exit(NULLPTR);
         return NULLPTR;
      }

      if(pfd[0].revents)
      {
         int lIndex = 0;
         sm_replay_status_parms lStatusParms;
         memset(&lStatusParms, 0, sizeof(lStatusParms));

         lStatusParms.channel = kSMNullChannelId;
         lResult = sm_replay_status (&lStatusParms);
         if (lResult == ERR_SM_NO_SUCH_CHANNEL)
            continue;

         if (lResult)
         {
            TERR (gTrace, printf("%s: \33[31mEVT-THREAD PLAY: Cannot check PLAY status %d\33[0m\n",
                     lProcessName, lResult););
            gLog.GenerateLog(ACU_MP50, lLogText);                  
            return false;
         }

         lIndex = sm_get_channel_ix(lStatusParms.channel);
         if(lIndex < 0)
            T(gTrace, printf("%s: Index:%d\n", lProcessName, lIndex););

         int lMediaIndx = gAcuChannel[lIndex].mediaIndx;

         if(lStatusParms.status != kSMReplayStatusHasCapacity)
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CktId:[%03d-%02d] "
                  "ChannelId:%lu[\33[32m%d:%d-%d:%d\33[0m] Status:%d[%s]\n", 
                  gChannel[lMediaIndx].acuCurPlay.play.seqNum, 
                  gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx,
                  lStatusParms.channel, gChannel[lMediaIndx].netSt.st, 
                  gChannel[lMediaIndx].netSt.ts, gChannel[lMediaIndx].mediaSt.st, 
                  gChannel[lMediaIndx].mediaSt.ts, lStatusParms.status, 
                  AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status));
            T(gTrace, printf("%s: EVT-THREAD PLAY:%s\n", lProcessName, lLogText););
            gLog.GenerateLog(ACU_MP51, lLogText);                  
         }

         lResult = sm_replay_file_progress_istatus(&gChannel[lMediaIndx].acuCurPlay.fp, 
               lStatusParms.status);

         if((lResult != ERR_SM_PENDING && gChannel[lMediaIndx].acuCurPlay.playCtrl == ACU_PLAY_NONE))
         {
            MpResponseDetails lAcuRsp;
            memset(&lAcuRsp, 0, sizeof(lAcuRsp));

            gChannel[lMediaIndx].acuCurPlay.fileCnt -= 1;
            if(true == gAculabMpMgrObj.HandlePlayFinish(lMediaIndx, lAcuRsp))
            {
               if(lAcuRsp.cmdType == MC_DISTN)
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "RefNum:%08X CktId:[%03d-%02d] MC_DISTN \33[32mStopPlay Success\33[0m", 
                        lAcuRsp.mpRspData.stopMsg.seqNum, 
                        lAcuRsp.mpRspData.stopMsg.circuitId, lMediaIndx);
                  gLog.GenerateLog(ACU_MP98, lLogText);
               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "RefNum:%08X CktId:[%03d-%02d] \33[35mSuccess to Handle PlayFinish\33[0m", 
                        gChannel[lMediaIndx].acuCurPlay.play.seqNum,
                        gChannel[lMediaIndx].acuCurPlay.play.circuitId,
                        lMediaIndx);
                  gLog.GenerateLog(ACU_MP52, lLogText);
               }
               T (gTrace, printf("%s: EVT-THREAD PLAY:%s\n", lProcessName, lLogText););

               gAculabMpMgrObj.WriteRsp(lAcuRsp);
            }
         }

         if(lStatusParms.status == kSMReplayStatusComplete && 
               gChannel[lMediaIndx].acuCurPlay.playCtrl != ACU_PLAY_NONE)
         {
            if(gChannel[lMediaIndx].acuCurPlay.playCtrl == ACU_PLAY_PAUSE)
            {
               gChannel[lMediaIndx].status = CKT_IDLE;
               gChannel[lMediaIndx].acuCurPlay.playCtrlPos = lStatusParms.offset;

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d-%02d] Status:%s "
                     "Stop Received For PlayCtrl:\33[32m%s[offset:%ld]\33[0m", 
                     gChannel[lMediaIndx].acuCurPlay.play.seqNum,
                     gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx, 
                     AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status),
                     AcuMediaUtil::ConvertPlayCtrlsToStr(gChannel[lMediaIndx].acuCurPlay.playCtrl),
                     gChannel[lMediaIndx].acuCurPlay.playCtrlPos);
               T(gTrace, printf("%s: EVT-THREAD PAUSE %s\n", lProcessName, lLogText););
               gLog.GenerateLog(ACU_MP52, lLogText);
            }
            else if(gChannel[lMediaIndx].acuCurPlay.playCtrl == ACU_PLAY_RESUME)
            {
               MpResponseDetails lAcuRsp;
               memset(&lAcuRsp, 0, sizeof(lAcuRsp));

               gChannel[lMediaIndx].acuCurPlay.fileCnt -= 1;
               if(true == gAculabMpMgrObj.HandlePlayFinish(lMediaIndx, lAcuRsp))
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "RefNum:%08X CktId:[%03d-%02d] Status:%s "
                        "\33[32mSuccess to Handle Resume\33[0m\n", 
                        gChannel[lMediaIndx].acuCurPlay.play.seqNum,
                        gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx,
                        AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status));
                  T(gTrace, printf("%s: EVT-THREAD RESUME %s\n", lProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP176, lLogText);
               }
            }
            else if(gChannel[lMediaIndx].acuCurPlay.playCtrl == ACU_PLAY_FORWARD)
            {
               INT16 lResult = 0;

               MpResponseDetails lAcuRsp;
               memset(&lAcuRsp, 0, sizeof(lAcuRsp));

               gChannel[lMediaIndx].acuCurPlay.playCtrl = ACU_PLAY_NONE;
               gChannel[lMediaIndx].acuCurPlay.playCtrlPos = lStatusParms.offset;
               gChannel[lMediaIndx].acuCurPlay.fp.offset = 
                  gChannel[lMediaIndx].acuCurPlay.playCtrlPos + gAculabMpMgrObj.mSkipOffset;

               lResult = sm_replay_file_start (&gChannel[lMediaIndx].acuCurPlay.fp);
               if (lResult != 0)
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "Failed Err:%d RefNum:%08X CktId:[%03d-%02d] Status:%s Played:%ldsec Fwd:%ldsec "
                        "FwdOffset:%ld", lResult, gChannel[lMediaIndx].acuCurPlay.play.seqNum, 
                        gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx, 
                        AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status),
                        gChannel[lMediaIndx].acuCurPlay.playCtrlPos/gAculabMpMgrObj.mSkipTime,
                        gChannel[lMediaIndx].acuCurPlay.fp.offset/gAculabMpMgrObj.mSkipTime);
                  TERR(gTrace, printf("%s: \33[31mEVT-THREAD FORWARD %s\33[0m\n",
                           gProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP193, lLogText);
               }

               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "Issued RefNum:%08X CktId:[%03d-%02d] Status:%s Playes:%ldsec Fwd:%ld File:%s\n", 
                     gChannel[lMediaIndx].acuCurPlay.play.seqNum,
                     gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx,
                     AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaIndx].status),
                     gChannel[lMediaIndx].acuCurPlay.playCtrlPos/gAculabMpMgrObj.mSkipTime, 
                     gChannel[lMediaIndx].acuCurPlay.fp.offset/gAculabMpMgrObj.mSkipTime,
                     gChannel[lMediaIndx].acuCurPlay.play.voiceFile[
                     gChannel[lMediaIndx].acuCurPlay.fileCnt]); 
               T(gTrace, printf("%s: \33[33mEVT-THREAD FORWARD %s\33[0m\n", lProcessName, lLogText););
               gLog.GenerateLog (ACU_MP194, lLogText);
            }
            else if(gChannel[lMediaIndx].acuCurPlay.playCtrl == ACU_PLAY_REWIND)
            {
               MpResponseDetails lAcuRsp;
               memset(&lAcuRsp, 0, sizeof(lAcuRsp));

               gChannel[lMediaIndx].acuCurPlay.playCtrl = ACU_PLAY_NONE;
               gChannel[lMediaIndx].acuCurPlay.playCtrlPos = lStatusParms.offset;

               if(gAculabMpMgrObj.mSkipOffset > lStatusParms.offset)
               {
                  gChannel[lMediaIndx].acuCurPlay.fp.offset = lStatusParms.offset;
                  snprintf(lLogText,MAX_LOG_TEXT_LEN,
                        "Invalid RefNum:%08X CktId:[%03d-%02d] Played:%ldsec Rwd:%ldsec",
                        gChannel[lMediaIndx].acuCurPlay.play.seqNum,
                        gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx,
                        gChannel[lMediaIndx].acuCurPlay.playCtrlPos/gAculabMpMgrObj.mSkipTime,
                        gChannel[lMediaIndx].acuCurPlay.fp.offset/gAculabMpMgrObj.mSkipTime);
                  TERR(gTrace, printf("%s: \33[31mEVT-THREAD REWIND %s\33[0m", 
                           lProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP199, lLogText);
               }
               else
                  gChannel[lMediaIndx].acuCurPlay.fp.offset = 
                     gChannel[lMediaIndx].acuCurPlay.playCtrlPos - gAculabMpMgrObj.mSkipOffset;

               lResult = sm_replay_file_start (&gChannel[lMediaIndx].acuCurPlay.fp);
               if ( lResult != 0 )
               {
                  snprintf(lLogText,MAX_LOG_TEXT_LEN,
                        "Failed Err:%d RefNum:%08X CktId:[%03d-%02d] CtrlOffset:%ld RwdOffset:%ld ",
                        lResult, gChannel[lMediaIndx].acuCurPlay.play.seqNum,
                        gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx,
                        gChannel[lMediaIndx].acuCurPlay.playCtrlPos,
                        gChannel[lMediaIndx].acuCurPlay.fp.offset);
                  TERR(gTrace, printf("%s: \33[31mEVT-THREAD REWIND %s\33[0m\n", 
                           gProcessName, lLogText););
                  gLog.GenerateLog(ACU_MP195, lLogText);
               }

               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     "Issued RefNum:%08X CktId:[%03d-%02d] Played:%ldsec Rwn:%ldsec File:%s", 
                     gChannel[lMediaIndx].acuCurPlay.play.seqNum,
                     gChannel[lMediaIndx].acuCurPlay.play.circuitId, lMediaIndx,
                     gChannel[lMediaIndx].acuCurPlay.playCtrlPos/gAculabMpMgrObj.mSkipTime,
                     gChannel[lMediaIndx].acuCurPlay.fp.offset/gAculabMpMgrObj.mSkipTime,
                     gChannel[lMediaIndx].acuCurPlay.play.voiceFile[
                     gChannel[lMediaIndx].acuCurPlay.fileCnt]);
               T(gTrace, printf("%s: \33[33mEVT-THREAD REWIND %s\33[0m\n", lProcessName, lLogText););
               gLog.GenerateLog (ACU_MP196, lLogText);
            }
         }
      }

      if(pfd[2].revents)
      {
         int lRecIndex = 0;
         sm_record_status_parms lRecStatusParms;
         memset(&lRecStatusParms, 0, sizeof(lRecStatusParms));

         lRecStatusParms.channel = kSMNullChannelId;
         lResult = sm_record_status (&lRecStatusParms);

         if (lResult == ERR_SM_NO_SUCH_CHANNEL)
            continue;

         if (lResult)
         {
            TERR (gTrace, printf("%s: \33[31mEVT-THREAD REC: Cannot Check Channel status %d\33[0m\n",
                     lProcessName, lResult););
            gLog.GenerateLog(ACU_MP53, 1);                     
            return false;
         }

         lRecIndex = sm_get_channel_ix(lRecStatusParms.channel);
         if(lRecIndex < 0)
            T(gTrace, printf("%s: RecIndex:%d\n", gProcessName, lRecIndex););

         int lRecIndx = gAcuChannel[lRecIndex].mediaIndx;

         if(lRecStatusParms.status != kSMRecordStatusData)
         {
            T(gTrace, printf("%s: EVT-THREAD REC: RefNum:%08X CktId:[%03d-%02d] "
                     "ChannelId:%lu[\33[32m%d:%d-%d:%d\33[0m] Status:%d\n", 
                     lProcessName, gChannel[lRecIndx].acuCurRecord.record.seqNum, 
                     gChannel[lRecIndx].acuCurRecord.record.circuitId,
                     lRecIndx, lRecStatusParms.channel, gChannel[lRecIndx].netSt.st,
                     gChannel[lRecIndx].netSt.ts, gChannel[lRecIndx].mediaSt.st, 
                     gChannel[lRecIndx].mediaSt.ts, lRecStatusParms.status););
            gLog.GenerateLog(ACU_MP54, 2);                  
         }

         lResult = sm_record_file_progress_istatus(&gChannel[lRecIndx].acuCurRecord.rp, 
               lRecStatusParms.status);

         if(lResult != ERR_SM_PENDING)
         {
            MpResponseDetails lAcuRsp;
            memset(&lAcuRsp, 0, sizeof(lAcuRsp));

            if(true == gAculabMpMgrObj.HandleRecordFinish(lRecIndx, lAcuRsp))
            {
               T(gTrace, printf("%s: EVT-THREAD REC: \33[35m RefNum:%08X CktId:[%03d-%02d] "
                        "Success to Handle RecordFinish\33[0m\n", lProcessName,
                        gChannel[lRecIndx].acuCurRecord.record.seqNum, 
                        gChannel[lRecIndx].acuCurRecord.record.circuitId, lRecIndx););
               gLog.GenerateLog(ACU_MP55, 3);
               gAculabMpMgrObj.WriteRsp(lAcuRsp);
               memset(&gChannel[lRecIndx].acuCurRecord, 0, sizeof(MpRecord));
            }
         }
      }

      if(pfd[1].revents)
      {
         MpResponseDetails lAcuRsp;
         SM_RECOGNISED_PARMS  rp;

         memset(&rp, 0, sizeof(rp));

         rp.channel = kSMNullChannelId;
         lResult = sm_get_recognised(&rp);

         if (lResult == ERR_SM_NO_SUCH_CHANNEL)
            continue;

         T(gTrace, printf("%s: EVT-THREAD DTMF: ChannelId:%lu Type:%d\n",
                  gProcessName, rp.channel, rp.type););

         memset(&lAcuRsp, 0, sizeof(lAcuRsp));

         gAculabMpMgrObj.HandleDtmfDetection(rp, lAcuRsp);

         if (!rp.type)
            continue;
      }
   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}

#if 0
//----------------------------------------------------------------------
// METHOD      : StartDtmfThread
// DESCRIPTION : Thread checks the DTMF detection on any channel
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *StartDtmfThread (void *lPtr)
{
   int      lResult = 0;
   struct   pollfd lPfd;
   TEXT     lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT     lProcessName[ACULAB_MAX_PROCESS_NAME + 1] = "";

   snprintf(lProcessName, ACULAB_MAX_PROCESS_NAME, "%s-%ld", gProcessName, syscall(224));

   memset(&lPfd, 0, sizeof(lPfd));
   lPfd.fd = gAculabMpMgrObj.mAcuEventApi.mDtmfEvent.fd;
   lPfd.events = gAculabMpMgrObj.mAcuEventApi.mDtmfEvent.mode || POLLPRI;

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DTMF THREAD: \33[33m Dtmf[Fd:%d Mode:%d]\33[0m", 
         lPfd.fd, lPfd.events);
   T(gTrace, printf("%s: DTMF Thread Info %s\n", lProcessName, lLogText););
   //gLog.GenerateLog(ACU_MP48, lLogText);            

   for(;;)
   {
      lResult = poll(&lPfd, 1, -1);
      if (lResult < 0)
      {
         TERR (gTrace, printf("%s: poll()failed Cannot wait for channels\n",
                  lProcessName););
         //gLog.GenerateLog(ACU_MP49, lLogText);                  
         pthread_exit(NULLPTR);
         return NULLPTR;
      }

      if(lPfd.revents)
      {
         MpResponseDetails lAcuRsp;
         SM_RECOGNISED_PARMS  rp;

         memset(&rp, 0, sizeof(rp));

         rp.channel = kSMNullChannelId;
         lResult = sm_get_recognised(&rp);

         if (lResult == ERR_SM_NO_SUCH_CHANNEL)
            continue;

         T(gTrace, printf("%s: DTMF ChannelId:%lu Type:%d\n",
                  gProcessName, rp.channel, rp.type););

         memset(&lAcuRsp, 0, sizeof(lAcuRsp));

         if(true == gAculabMpMgrObj.HandleDtmfDetection(rp, lAcuRsp))
         {
            T(gTrace, printf("%s: Success to Handle GETDTMF\n", lProcessName););
            gLog.GenerateLog(ACU_MP56, 4);
            gAculabMpMgrObj.WriteRsp(lAcuRsp);
         }

         if (!rp.type)
            continue;
      }
   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}
#endif

//-------------------------------------------------------------
// FUNCTION    : CreateEventThread
// DESCRIPTION : Creates Event thread which handles the play, 
//               record and dtmf events
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::CreateEventThread()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   void* (*lpFuncPtr)(void * );
   lpFuncPtr = StartEventThread;

   mEventThread = pthread_create(&mEventMsgThread, NULLPTR, lpFuncPtr, (void *) &gChannel);
   if(mEventThread != 0)
   {
      sprintf(lLogText, "pthread_create failed for StartEventThread for %s", gProcessName);
      TERR(gTrace, printf("%s-%ld: \33[31m%s\33[0m\n", 
               gProcessName, syscall(224), lLogText););
      gLog.GenerateLog(ACU_MP57, lLogText);
      return false;
   }

#if 0
   void* (*lpDtmfFunc)(void * );
   lpDtmfFunc = StartDtmfThread;

   mDtmfThread = pthread_create(&mDtmfMsgThread, NULLPTR, lpDtmfFunc, (void *) &gChannel);
   if(mDtmfThread != 0)
   {
      sprintf(lLogText, "pthread_create failed for StartDtmfThread for %s", gProcessName);
      TERR(gTrace, printf("%s-%ld: %s\n", gProcessName, syscall(224), lLogText););
      //gLog.GenerateLog(ACU_MP57, lLogText);
      return false;
   }
#endif


   return true;
}

//-------------------------------------------------------------
// FUNCTION    : Init
// DESCRIPTION : Initializes member attributes
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::Init()
{
   TEXT      lLogText[MAX_LOG_TEXT_LEN];

   if( false == mMsgQueApi.Init())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "QueApi Init failed");
      TERR(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == ReadConfig())
   {  
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "ReadConfig failed");
      TERR(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", gProcessName, lLogText);)
         gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == mAcuCardDetails.Init(50))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Failed to initialize Acu Card Details");
      T(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == mAcuModuleInfo.Init(200))
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,"Failed to initialize Acu Module info");
      T(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == mAcuCardDetails.ClearCardDetails())
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,"Failed to clear Card Details");
      T(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   if(false == mAcuModuleInfo.ClearModuleDetails())
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,"Failed to Module Details");
      T(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP58, lLogText);
      return false;
   }

   if (false == InitializeCard())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Card Initialisation failed");
      TERR(gTrace, printf("%s: \33[31m%s\33[0m. Exiting...\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP60, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : ReleaseChannel 
// DESCRIPTION : Releases the channel for further use
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::ReleaseChannel(tSMChannelId & rChannel)
{
   INT16 lResult = 0;

   lResult = sm_channel_release(rChannel);
   if(lResult != 0 )
   {
      TERR(gTrace, printf("Error Could not free Prosody channel <%d>", lResult););
      gLog.GenerateLog(ACU_MP62, 1);
      return false;
   }
   rChannel = kSMNullChannelId;

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : DeinitChannel
// DESCRIPTION : Deinitialises the allocated channels
// PARAMETER   : None
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::DeinitChannel(UINT32 lTempCardId)
{
   int      lIndex = 0;
   INT16    lResult = 0;
   TEXT     lLogText[MAX_LOG_TEXT_LEN+1] = "";

   for(lIndex = 1; lIndex<gIndex; lIndex++)
   {
      SM_CHANNEL_SET_EVENT_PARMS lSe;
      OUTPUT_PARMS  lDcp;

      memset(&lSe, 0, sizeof(lSe));
      lSe.channel      = gChannel[lIndex].channelId;
      lSe.event        = mAcuEventApi.mRecEvent;
      lSe.event_type   = kSMEventTypeReadData;
      lSe.issue_events = kSMChannelNoEvent;
      lResult = sm_channel_set_event( &lSe );
      if( lResult != 0 )
      {
         snprintf(lLogText,MAX_LOG_TEXT_LEN,
               "Error Could not associate channel and event REC <%d>", lResult );
         TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););
         gLog.GenerateLog(ACU_MP63, lLogText);

         return false;
      }

      memset(&lSe, 0, sizeof(lSe));
      lSe.channel      = gChannel[lIndex].channelId;
      lSe.event        = mAcuEventApi.mDtmfEvent;
      lSe.event_type   = kSMEventTypeRecog;
      lSe.issue_events = kSMChannelNoEvent;
      lResult = sm_channel_set_event( &lSe );
      if( lResult != 0 )
      {
         snprintf(lLogText,MAX_LOG_TEXT_LEN,
               "Error Could not associate channel and event DTMF <%d>", lResult );
         TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););
         gLog.GenerateLog(ACU_MP64, lLogText);
         return false;
      }

      memset(&lSe, 0, sizeof(lSe));
      lSe.channel      = gChannel[lIndex].channelId;
      lSe.event        = mAcuEventApi.mPlayEvent;
      lSe.event_type   = kSMEventTypeWriteData;
      lSe.issue_events = kSMChannelNoEvent;
      lResult = sm_channel_set_event( &lSe );
      if( lResult != 0 )
      {
         snprintf(lLogText,MAX_LOG_TEXT_LEN,
               "Error Could not associate channel and event PLAY <%d>", lResult );
         T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
         gLog.GenerateLog(ACU_MP65, lLogText);
         return false;
      }

      memset(&lDcp, 0, sizeof(lDcp));
      lDcp.ost  = gChannel[lIndex].netSt.st;
      lDcp.ots  = gChannel[lIndex].netSt.ts;
      lDcp.mode = DISABLE_MODE;
      lDcp.size = sizeof(lDcp);
      lResult = sw_set_output(lTempCardId, &lDcp);
      if ( lResult != 0)
      {
         printf ("Error could not break connection to Prosody <%d>", lResult);
         return false;
      }

      memset(&lDcp, 0, sizeof(lDcp));
      lDcp.ost  = gChannel[lIndex].mediaSt.st;
      lDcp.ots  = gChannel[lIndex].mediaSt.ts;
      lDcp.mode = DISABLE_MODE;
      lDcp.size = sizeof(lDcp);
      lResult = sw_set_output(lTempCardId, &lDcp);
      if ( lResult != 0)
      {
         printf ("Error could not break connection to Prosody <%d>", lResult);
         return false;
      }

      this->ReleaseChannel (gChannel[lIndex].channelId);
      gChannel[lIndex].channelId = kSMNullChannelId;
      memset(&gChannel[lIndex], 0, sizeof(acuChannel));
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
BOOLEAN AculabMPMgr::DeInit(INT16 lModuleIndx)
{
   int                   lCount = 0;
   UINT32                lCardId[ACULAB_MAX_NUM_CARDS];
   TEXT                  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   ACU_CLOSE_CARD_PARMS       lCloseCardParms;
   ACU_CLOSE_PROSODY_PARMS    lCloseProsodyParms;
   ACU_CLOSE_SWITCH_PARMS     lCloseSwitchParms;

   CardDetails  lCardDetails;
   memset(&lCardDetails, 0, sizeof(CardDetails));

   lCount = mAcuCardDetails.GetTotalCardInfo(lCardId);

   if(false == mAcuCardDetails.GetCardDetails(lCardId[lModuleIndx], lCardDetails))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Failed to get the card Details "
            "for CardId:%lu", lCardId[lModuleIndx]);
      TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP66, lLogText);
      return false;
   }
   else
   {
      if(true != this->DeinitChannel(lCardId[lModuleIndx]))
         return false;

      INIT_ACU_STRUCT(&lCloseProsodyParms);
      lCloseProsodyParms.card_id = lCardId[lModuleIndx];
      acu_close_prosody(&lCloseProsodyParms);

      INIT_ACU_STRUCT(&lCloseSwitchParms);
      lCloseSwitchParms.card_id = lCardId[lModuleIndx];
      acu_close_switch(&lCloseSwitchParms);

      INIT_ACU_STRUCT(&lCloseCardParms);
      lCloseCardParms.card_id = lCardId[lModuleIndx];
      acu_close_card(&lCloseCardParms);
   }

   T(gTrace, printf("%s: Successfully Deinitialised the card\n", gProcessName););
   gLog.GenerateLog(ACU_MP67, 1);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : GetCircuitId
// DESCRIPTION :
// PARAMETER   :
// RETURN      : UINT16
//----------------------------------------------------------------------
UINT16 AculabMPMgr::GetCircuitId(UINT16 lCktId)
{
   UINT16 lCircuit = 0;

   lCircuit = (lCktId % 60); // max channles per module

   if(lCircuit == 0)
      lCircuit = 60;

   return lCircuit;
}

//----------------------------------------------------------------------
// METHOD      : CheckCircuitId
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : TEXT*
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::CheckCircuitId(EnumMediaCmdTypes lCmd, MpData &lMpData)
{
   BOOLEAN lValidCircuit = true;

   switch(lCmd)
   {
      case MC_INVAL_CMD_TYPE:
         lValidCircuit = false;
         break;

      case MC_CNTTN:
         if(lMpData.playMsg.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_DISTN:
         if(lMpData.stopMsg.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_CNTSP:
         if(lMpData.switchChnl.circuitId_1 > mMaxChannels) 
            lValidCircuit = false;
         if(lMpData.switchChnl.circuitId_2 > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_DISSP:
         if(lMpData.switchChnl.circuitId_1 > mMaxChannels) 
            lValidCircuit = false;
         if(lMpData.switchChnl.circuitId_2 > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_GETDIG:
         if(lMpData.getDigit.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_GETDIGSTP:
         if(lMpData.stopMsg.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_RECSTR:
         if(lMpData.recordMsg.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_RECSTP:
         if(lMpData.stopMsg.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_SETVOL:
         if(lMpData.setVolSpd.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      case MC_SETSPD:
         if(lMpData.setVolSpd.circuitId > mMaxChannels)
            lValidCircuit = false;
         break;

      default:
         break;
   }

   if(lValidCircuit != true)
      return false;

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : HandleRewind     
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandleRewind(const MpRewind rewindMsg)
{
   INT16  lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lCktId  = GetCircuitId(rewindMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] \33[32mRcvd play REWIND Req\33[0m",
         rewindMsg.seqNum, rewindMsg.circuitId, lCktId);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP189, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_REWIND Status:%s",
                  rewindMsg.seqNum, rewindMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP190, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
         break;

      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
         {
            pChnl->acuCurPlay.playCtrl = ACU_PLAY_REWIND;

            if(true != StopPlay(lCktId))
               return MP_REWIND_CMD_ERR;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_REWIND Status:%s PlayStatus:%s "
                  "Rewind File Success", rewindMsg.seqNum, rewindMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status),
                  AcuMediaUtil::ConvertPlayStatusToStr(pChnl->playStatus));
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP191, lLogText);
         }
         break;

      case  CKT_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_REWIND Status:%s "
                  "Not Playing to REWIND", rewindMsg.seqNum, rewindMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr( pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP197, lLogText);
            return MP_BADPARM_ERR;
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_REWIND Status:%s "
                  "Invalid Channel Status", rewindMsg.seqNum, rewindMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr( pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP192, lLogText);
            return MP_BADPARM_ERR;
         }
         break;
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : HandleForward   
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandleForward(const MpForward forwardMsg)
{
   INT16  lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lCktId  = GetCircuitId(forwardMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] \33[32mRcvd play FORWARD Req\33[0m",
         forwardMsg.seqNum, forwardMsg.circuitId, lCktId);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP185, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_FORWARD Status:%s",
                  forwardMsg.seqNum, forwardMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP186, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
         break;

      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
         {
            pChnl->acuCurPlay.playCtrl = ACU_PLAY_FORWARD;

            if(true != StopPlay(lCktId))
               return MP_FORWARD_CMD_ERR;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_FORWARD Status:%s PlayStatus:%s "
                  "Forward File Success", forwardMsg.seqNum, forwardMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status),
                  AcuMediaUtil::ConvertPlayStatusToStr(pChnl->playStatus));
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP187, lLogText);
         }
         break;

      case CKT_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_FORWARD Status:%s "
                  "Not Playing to FORWARD", forwardMsg.seqNum, forwardMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr( pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP198, lLogText);
            return MP_BADPARM_ERR;
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_FORWARD Status:%s "
                  "Invalid Channel Status", forwardMsg.seqNum, forwardMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr( pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP188, lLogText);
            return MP_BADPARM_ERR;
         }
         break;
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : HandleResume
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandleResume(const MpResume resumeMsg)
{
   int   lResult = 0;
   INT16 lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lCktId  = GetCircuitId(resumeMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] PlayCtrlPos:%ld \33[32mRcvd play RESUME Req\33[0m", 
         resumeMsg.seqNum, resumeMsg.circuitId, lCktId, pChnl->acuCurPlay.playCtrlPos);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP168, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RESUME Status:%s",
                  resumeMsg.seqNum, resumeMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP171,  lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
         break;

      case CKT_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RESUME Status:%s "
                  "So Issuing RESUME", resumeMsg.seqNum, resumeMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP172, lLogText);

            if(pChnl->acuCurPlay.playCtrlPos != 0)
               pChnl->acuCurPlay.fp.offset = pChnl->acuCurPlay.playCtrlPos;
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d-%02d] PlayCtrlPos:%ld "
                     "\33[33mPAUSE not RECEIVED to RESUME\33[0m",
                     resumeMsg.seqNum, resumeMsg.circuitId, lCktId, pChnl->acuCurPlay.playCtrlPos);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP169, lLogText);
               return MP_RSUME_CMD_ERR;
            }

            lResult = sm_replay_file_start (&pChnl->acuCurPlay.fp);
            if ( lResult != 0 )
            {
               snprintf(lLogText,MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d-%02d] CMD:RESUME Failed %s <%d>", 
                     resumeMsg.seqNum, resumeMsg.circuitId, lCktId,
                     pChnl->acuCurPlay.play.voiceFile[pChnl->acuCurPlay.fileCnt], lResult );
               TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP118, lLogText);
               return MP_RSUME_CMD_ERR;
            }

            pChnl->status = CKT_PLAYING;
            pChnl->acuCurPlay.playCtrl = ACU_PLAY_RESUME;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] \33[32m CMD:RESUME Issued\33[0m", 
                  resumeMsg.seqNum, resumeMsg.circuitId, lCktId);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP170, lLogText);
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RESUME Status:%s "
                  "Invalid Channel Status", resumeMsg.seqNum, resumeMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr( pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP173, lLogText);
            return MP_BADPARM_ERR;
         }
         break;
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : HandlePause       
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandlePause(const MpPause pauseMsg)
{
   INT16  lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lCktId  = GetCircuitId(pauseMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] \33[32mRcvd play PAUSE Req\33[0m", 
         pauseMsg.seqNum, pauseMsg.circuitId, lCktId);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP167, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_PAUSE Status:%s",
                  pauseMsg.seqNum, pauseMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP174, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
         break;

      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
         {
            pChnl->status = CKT_PLAY_STOPING;
            pChnl->acuCurPlay.playCtrl = ACU_PLAY_PAUSE;

            if(true != StopPlay(lCktId))
               return MP_PAUSE_CMD_ERR;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_PAUSE Status:%s PlayStatus:%s "
                  "Pause File Success", pauseMsg.seqNum, pauseMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status),
                  AcuMediaUtil::ConvertPlayStatusToStr(pChnl->playStatus));
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP166, lLogText);
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_PAUSE Status:%s "
                  "Invalid Channel Status", pauseMsg.seqNum, pauseMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr( pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP175, lLogText);
            return MP_BADPARM_ERR;
         }
         break;
   }

   return MP_SUCCESS;
}

//----------------------------------------------------------------------
// METHOD      : StopRecord
// DESCRIPTION : Stops Play and wait for stop completion
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::StopRecord(INT16 lIndex)
{
   int lResult = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X CktId:[%03d-%02d]",
         gChannel[lIndex].acuCurRecord.record.seqNum, 
         gChannel[lIndex].acuCurRecord.record.circuitId, lIndex);
   T (gTrace, printf("%s: %s CMD:sm_record_file_stop Issued\n", 
            gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP68, lLogText);

   lResult = sm_record_file_stop(&gChannel[lIndex].acuCurRecord.rp, 1);
   if(lResult != 0)
   {
      TERR (gTrace, printf("%s: %s CMD:sm_record_file_stop failed <%d>\n", 
               gProcessName, lLogText, lResult););
      gLog.GenerateLog(ACU_MP69, 2);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : HandleStopRecord
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandleStopRecord(const MpStop stopMsg)
{
   INT16          lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lCktId  = GetCircuitId(stopMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] \33[32mRcvd STOP RECORD Req\33[0m", 
         stopMsg.seqNum, stopMsg.circuitId, lCktId);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP70, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTP Status:%s",
                  stopMsg.seqNum, stopMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP71, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
         break;

      case CKT_IDLE:
      case CKT_GETDIG:
      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
      case CKT_PLAY_STOPING:
      case CKT_PLAY_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTP Status:%s "
                  "Not Recording To Stop", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP72, lLogText);
         }
         break;

      case CKT_RECORD_STOPING:
      case CKT_RECORD_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTP Status:%s"
                  "Stop Recording in Progress", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP73, lLogText);
         }
         break;

      case CKT_RECORDING:
      case CKT_RECORDING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTP Status:%s"
                  "Hence issuing StopRec", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP74, lLogText);

            if(pChnl->status != CKT_IDLE)
            {
               pChnl->playStatus = ACU_PLAY_NEXT_NOT_REQD;

               if(pChnl->status == CKT_RECORDING)
                  pChnl->status = CKT_RECORD_STOPING;
               else if(pChnl->status == CKT_RECORDING_AND_GETDIG)
                  pChnl->status = CKT_RECORD_STOPING_AND_GETDIG; 

               if(true != StopRecord(lCktId))
                  return MP_RECORD_STOP_CMD_ERR;

               pChnl->circuitId = lCktId;
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTP Status:%s"
                  "StopRec Success", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP75, lLogText);
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTP Status:%s "
                  "Invalid Channel Status", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP76, lLogText);
            return MP_BADPARM_ERR;
         }
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : CancelDtmf
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::CancelDtmf(INT16 lCktId)
{
   int      lResult = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN] = "";

   SM_CHANNEL_SET_EVENT_PARMS lSe;

   memset(&lSe, 0, sizeof(lSe));
   lSe.channel      = gChannel[lCktId].channelId;
   lSe.event        = mAcuEventApi.mDtmfEvent;
   lSe.event_type   = kSMEventTypeRecog;
   lSe.issue_events = kSMChannelNoEvent;

   lResult = sm_channel_set_event( &lSe );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error Could not associate channel and event DTMF <%d>", lResult );
      TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP77, lLogText);
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
EnumVoiceProcessErr AculabMPMgr::CancelGetDtmfDigits(const MpStop stopMsg, TEXT *lColDigit)
{
   INT16          lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";

   lCktId  = GetCircuitId(stopMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] Rcvd Cancel Get DtmfDigits", 
         stopMsg.seqNum, stopMsg.circuitId, lCktId);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP78, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIGSTP Status:%s", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP79, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }

      case CKT_IDLE:
      case CKT_PATCHED:
      case CKT_PLAYING:
      case CKT_RECORDING:
      case CKT_PLAY_STOPING:
      case CKT_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIGSTP Status:%s "
                  "Not waiting for Dtmf", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP80, lLogText);
            return MP_CKT_BUSY_ERR;
         }

      case CKT_GETDIG:
      case CKT_PLAYING_AND_GETDIG:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIGSTP Status:%s "
                  "Hence issuing cancel Dtmf", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP81, lLogText);

            if(true != CancelDtmf(lCktId))
               return MP_CANCEL_GET_DIG_CMD_ERR;

            memset(&(pChnl->acuCurDtmf.dtmf), 0, sizeof(MpGetDigit));
            pChnl->playStatus      = ACU_PLAY_NEXT_NOT_REQD;
            pChnl->circuitId       = lCktId;

            if(CKT_PLAYING_AND_GETDIG == pChnl->status)
               pChnl->status  = CKT_PLAYING;
            else if(CKT_PLAY_STOPING_AND_GETDIG == pChnl->status)
               pChnl->status  = CKT_PLAY_STOPING;
            else if(CKT_GETDIG == pChnl->status)
               pChnl->status  = CKT_IDLE;
            else if(CKT_RECORD_STOPING_AND_GETDIG == pChnl->status)
               pChnl->status  = CKT_RECORD_STOPING;
            else if(CKT_RECORDING_AND_GETDIG== pChnl->status)
               pChnl->status  = CKT_RECORDING;
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d-%02d] MC_GETDIGSTP Status:%s "
                     "State Changed Abbrupt\n", 
                     stopMsg.seqNum, stopMsg.circuitId, lCktId,
                     AcuMediaUtil::ConvertStatusToStr(pChnl->status));
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP82, lLogText);
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIGSTP Status:%s "
                  "CancelGetDtmfDigits Success", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP83, lLogText);
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIGSTP Status:%s "
                  "Invalid Channel Status", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP84, lLogText);
            return MP_BADPARM_ERR;
         }
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : ListenForDtmf
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN              
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::ListenForDtmf(MpGetDigit lDtmf, INT16 lCktId)
{
   INT16 lResult = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   SM_CHANNEL_SET_EVENT_PARMS lSep;

   memset(&gChannel[lCktId].acuCurDtmf.lp, 0, 
         sizeof(gChannel[lCktId].acuCurDtmf.lp));

   memset(&lSep, 0, sizeof(lSep));
   lSep.event = mAcuEventApi.mDtmfEvent;
   lSep.issue_events = kSMAnyChannelEvent;
   lSep.event_type = kSMEventTypeRecog;
   lSep.channel = gChannel[lCktId].channelId;
   lResult = sm_channel_set_event(&lSep);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CMD: sm_channel_set_event failed DTMF <%d>", lResult);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP85, lLogText);
      // Check this point
      if(gChannel[lCktId].status == CKT_GETDIG)
         this->ReleaseChannel (lSep.channel);

      return false;
   }

   gChannel[lCktId].acuCurDtmf.lp.grunt_latency = 1000;
   gChannel[lCktId].acuCurDtmf.lp.enable_grunt_detection = 0;

   if(lDtmf.numOfDig > 0)
      gChannel[lCktId].acuCurDtmf.lp.tone_detection_mode = kSMToneLenDetectionNoMinDuration;
   else
      gChannel[lCktId].acuCurDtmf.lp.tone_detection_mode = kSMToneDetectionNoMinDuration;

   gChannel[lCktId].acuCurDtmf.lp.map_tones_to_digits = kSMDTMFToneSetDigitMapping;
   gChannel[lCktId].acuCurDtmf.lp.channel = gChannel[lCktId].channelId;
   gChannel[lCktId].acuCurDtmf.lp.enable_cptone_recognition = 0;
   gChannel[lCktId].acuCurDtmf.dtmf.numOfDig = lDtmf.numOfDig;
   gChannel[lCktId].acuCurDtmf.lp.grunt_latency = 3000;

   lResult = sm_listen_for(&gChannel[lCktId].acuCurDtmf.lp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "CMD:sm_listen_for failed <%d>", lResult);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP86, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : HandleDtmf
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandleDtmf(const MpGetDigit dtmfMsg, MpGetDigit &dtmfRsp)
{
   INT16          lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1]      = "";

   lCktId  = GetCircuitId(dtmfMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] NumDig:%d IntrDgtTOut:%ld TermChar:%s "
         "\33[32mRcvd GetDTMF Digits Req\33[0m", 
         dtmfMsg.seqNum, dtmfMsg.circuitId, lCktId, dtmfMsg.numOfDig, 
         dtmfMsg.interDigitTimeOut, dtmfMsg.teminateChars);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP87, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIG Status:%s", 
                  dtmfMsg.seqNum, dtmfMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP88, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
         break;

      case CKT_GETDIG:
      case CKT_PATCHED:
      case CKT_PLAYING_AND_GETDIG:
      case CKT_PLAY_STOPING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
      case CKT_RECORDING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIG Status:%s"
                  "Hence Can not GetDigit", dtmfMsg.seqNum, 
                  dtmfMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP89, lLogText);
            return MP_CKT_BUSY_ERR;
         }

      case CKT_IDLE:
      case CKT_PLAYING:
      case CKT_RECORDING:
      case CKT_PLAY_STOPING:
      case CKT_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIG Status:%s"
                  " Setting status to GetDigit", dtmfMsg.seqNum, 
                  dtmfMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP90, lLogText);

            if(true != ListenForDtmf(dtmfMsg, lCktId))
               return MP_GET_DIG_CMD_ERR;

            pChnl->acuCurDtmf.dtmf = dtmfMsg;
            pChnl->playStatus      = ACU_PLAY_NEXT_NOT_REQD;
            pChnl->circuitId       = lCktId;
            pChnl->acuCurDtmf.detectedDigitsLen = 0;
            strcpy(pChnl->acuCurDtmf.detectedDigits, "");

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
                     "RefNum:%08X CktId:[%03d-%02d] MC_GETDIG Status:%s"
                     "State Changed Abbrupt\n", dtmfMsg.seqNum, 
                     dtmfMsg.circuitId, lCktId,
                     AcuMediaUtil::ConvertStatusToStr(pChnl->status));
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP91, lLogText);
            }

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_GETDIG "
                  "for DigitsToGet:%d Status:%s Listen for Success",
                  dtmfMsg.seqNum, dtmfMsg.circuitId, lCktId, dtmfMsg.numOfDig, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP92, lLogText);
         }
         break;
   }

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : DisconnectSpeech
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::DisconnectSpeech(const MpSwitch tear)
{
   INT16          lCktId1 = -1;
   INT16          lCktId2 = -1;
   acuChannel*    pChn1   = NULL;
   acuChannel*    pChn2   = NULL;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16          lResult = 0;

   SM_CONFERENCE_REMOVE_PARTY_PARMS lRemConfp;

   lCktId1 = GetCircuitId(tear.circuitId_1);
   lCktId2 = GetCircuitId(tear.circuitId_2);

   pChn1   = &gChannel[lCktId1];
   pChn2   = &gChannel[lCktId2];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] CktId2:[%03d-%02d] \33[32mRcvd Call Tear Req\33[0m",
         tear.seqNum, gChannel[lCktId1].acuConf.confId, tear.circuitId_1, lCktId1, 
         tear.circuitId_2, lCktId2);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   if (0 == gChannel[lCktId1].acuConf.confId)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] CktId2:[%03d-%02d]",
            tear.seqNum, gChannel[lCktId1].acuConf.confId, tear.circuitId_1, lCktId1, 
            tear.circuitId_2, lCktId2);
      TERR(gTrace, printf("%s: %s\33[31m Call Path Not Exists to Tear Connection\33[0m\n", 
               gProcessName, lLogText););
      //gLog.GenerateLog(ACU_MP01, lLogText);
      return MP_CONNECT_CMD_ERR;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] CktId2:[%03d-%02d]",
         tear.seqNum, gChannel[lCktId1].acuConf.confId, tear.circuitId_1, lCktId1, 
         tear.circuitId_2, lCktId2);
   T(gTrace, printf("%s: %s\33[32m CallPatch Exists\33[0m\n",
            gProcessName, lLogText););
   //gLog.GenerateLog(ACU_MP01, lLogText);

   INIT_ACU_SM_STRUCT(&lRemConfp);
   lRemConfp.conf = gChannel[lCktId1].acuConf.confId;
   lRemConfp.channelIn = gChannel[lCktId1].channelId;
   lRemConfp.channelOut = gChannel[lCktId1].channelId;

   lResult = sm_conference_remove_party (&lRemConfp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] ChannlId1:%ld",
            tear.seqNum, lRemConfp.conf, tear.circuitId_1, lCktId1, gChannel[lCktId1].channelId);
      TERR(gTrace, printf("%s: %s\33[31m Failed to Remove First Party\n", gProcessName, lLogText););
      //gLog.GenerateLog();
      return MP_CONNECT_CMD_ERR;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] ChannlId1:%lu",
         tear.seqNum, lRemConfp.conf, tear.circuitId_1, lCktId1, gChannel[lCktId1].channelId);
   T(gTrace, printf("%s: %s\33[32m Successfully Removed First Party\33[0m\n",
            gProcessName, lLogText););
   //gLog.GenerateLog();

   INIT_ACU_SM_STRUCT(&lRemConfp);
   lRemConfp.conf = gChannel[lCktId2].acuConf.confId;
   lRemConfp.channelIn = gChannel[lCktId2].channelId;
   lRemConfp.channelOut = gChannel[lCktId2].channelId;

   lResult = sm_conference_remove_party (&lRemConfp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X PatchId:%lu CktId2:[%03d-%02d] ChannlId2:%ld",
            tear.seqNum, lRemConfp.conf, tear.circuitId_2, lCktId2, gChannel[lCktId2].channelId);
      TERR(gTrace, printf("%s: %s\33[31m Failed to Remove Second Party\33[0m\n", 
               gProcessName, lLogText););
      //gLog.GenerateLog();
      return MP_CONNECT_CMD_ERR;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X PatchId:%lu CktId2:[%03d-%02d] ChannlId2:%lu", tear.seqNum, lRemConfp.conf, 
         tear.circuitId_2, lCktId2, gChannel[lCktId2].channelId);
   T(gTrace, printf("%s: %s\33[32m Successfully Removed Second Party\33[0m\n",
            gProcessName, lLogText););
   //gLog.GenerateLog();

   sm_conference_delete(lRemConfp.conf);

   gChannel[lCktId1].acuConf.confId = 0;
   gChannel[lCktId2].acuConf.confId = 0;
   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X PatchId:%ld CktId1:[%03d-%02d] CktId2:[%03d-%02d] "
         "ChannlId1:%ld ChannlId2:%ld", tear.seqNum, lRemConfp.conf, tear.circuitId_1, lCktId1, 
         tear.circuitId_2, lCktId2, gChannel[lCktId1].channelId, gChannel[lCktId2].channelId);
   T(gTrace, printf("%s: %s\33[33m Success Tear the Call\33[0m\n", gProcessName, lLogText););
   //gLog.GenerateLog();

   return MP_SUCCESS;
}

//-------------------------------------------------------------
// FUNCTION    : ConnectSpeech
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::ConnectSpeech(const MpSwitch patch)
{
   INT16          lCktId1 = -1; 
   INT16          lCktId2 = -1;
   acuChannel*    pChn1   = NULL;
   acuChannel*    pChn2   = NULL;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16          lResult = 0;

   SM_CHANNEL_ALLOC_PLACED_PARMS lAcp12, lRec;
   SM_SWITCH_CHANNEL_PARMS       lScp;
   SM_CONFERENCE_ADD_PARTY_PARMS lAddConfp;

   INIT_ACU_SM_STRUCT(&lAcp12);
   INIT_ACU_SM_STRUCT(&lScp);
   INIT_ACU_SM_STRUCT(&lRec);

   lCktId1 = GetCircuitId(patch.circuitId_1);
   lCktId2 = GetCircuitId(patch.circuitId_2);

   pChn1   = &gChannel[lCktId1];
   pChn2   = &gChannel[lCktId2];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId1:[%03d-%02d] CktId2:[%03d-%02d] \33[32mRcvd Call Patch Req\33[0m", 
         patch.seqNum, patch.circuitId_1, lCktId1, patch.circuitId_2, lCktId2);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   //create conference
   gChannel[lCktId1].acuConf.confId = sm_conference_create();
   gChannel[lCktId2].acuConf.confId = gChannel[lCktId1].acuConf.confId;
   if(0 == gChannel[lCktId1].acuConf.confId)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId1:[%03d-%02d] CktId2:[%03d-%02d] PatchId:%d",
            patch.seqNum, patch.circuitId_1, lCktId1, patch.circuitId_2, lCktId2, 
            gChannel[lCktId1].acuConf.confId);
      TERR(gTrace, printf("%s: %s\33[31m Call Patch Failed\33[0m\n", 
               gProcessName, lLogText););
      //gLog.GenerateLog(ACU_MP01, lLogText);
      return MP_CONNECT_CMD_ERR;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] CktId2:[%03d-%02d]",
         patch.seqNum, gChannel[lCktId1].acuConf.confId, patch.circuitId_1, 
         lCktId1, patch.circuitId_2, lCktId2);
   T(gTrace, printf("%s: %s\33[32m Successfully Created 2-party Conference\33[0m\n", 
            gProcessName, lLogText););
   //gLog.GenerateLog(ACU_MP01, lLogText);

   INIT_ACU_SM_STRUCT(&lAddConfp);
   lAddConfp.conf = gChannel[lCktId1].acuConf.confId;
   lAddConfp.channelIn = gChannel[lCktId1].channelId;
   lAddConfp.channelOut = gChannel[lCktId1].channelId;

   lResult = sm_conference_add_party (&lAddConfp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] ChannlId1:%ld Err:%d", patch.seqNum, 
            lAddConfp.conf, patch.circuitId_1, lCktId1, gChannel[lCktId1].channelId, lResult);
      TERR(gTrace, printf("%s: %s\33[31m Failed to Add First Party\33[0m\n", gProcessName, lLogText););
      //gLog.GenerateLog();
      return MP_CONNECT_CMD_ERR;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X PatchId:%lu CktId1:[%03d-%02d] ChannlId1:%lu", patch.seqNum, 
         lAddConfp.conf, patch.circuitId_1, lCktId1, gChannel[lCktId1].channelId);
   T(gTrace, printf("%s: %s\33[32m Successfully Added First Party to Conf\33[0m\n", 
            gProcessName, lLogText););
   //gLog.GenerateLog();

   INIT_ACU_SM_STRUCT(&lAddConfp);
   lAddConfp.conf = gChannel[lCktId2].acuConf.confId;
   lAddConfp.channelIn = gChannel[lCktId2].channelId;
   lAddConfp.channelOut = gChannel[lCktId2].channelId;

   lResult = sm_conference_add_party (&lAddConfp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X PatchId:%lu CktId2:[%03d-%02d] ChannlId2:%ld Err:%d", patch.seqNum, 
            lAddConfp.conf, patch.circuitId_2, lCktId2, gChannel[lCktId2].channelId, lResult);
      TERR(gTrace, printf("%s: %s\33[31m Failed to Add Second Party\33[0m\n", gProcessName, lLogText););
      //gLog.GenerateLog();
      return MP_CONNECT_CMD_ERR;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "RefNum:%08X PatchId:%lu CktId2:[%03d-%02d] ChannlId2:%ld", patch.seqNum, 
         lAddConfp.conf, patch.circuitId_2, lCktId2, gChannel[lCktId2].channelId);
   T(gTrace, printf("%s: %s\33[32m Successfully Added Second Party to Conf\33[0m\n", 
            gProcessName, lLogText););
   //gLog.GenerateLog();

   return MP_SUCCESS;
}

//----------------------------------------------------------------------
// METHOD      : IssueGetDigitStop
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::IssueGetDigitStop()
{
   TEXT                 lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   EnumVoiceProcessErr  lVpErr;
   MpResponseDetails    lAcuRsp;
   INT16 lCkt = GetCircuitId (mMpAcuReq.mpReqData.stopMsg.circuitId);

   memset(&lAcuRsp, 0, sizeof(lAcuRsp));

   lVpErr = CancelGetDtmfDigits(mMpAcuReq.mpReqData.stopMsg, lAcuRsp.mpRspData.getDigit.rspDigits);

   lAcuRsp.cmdType             = MC_GETDIGSTP;
   lAcuRsp.mpRspData.stopMsg   = mMpAcuReq.mpReqData.stopMsg;
   lAcuRsp.voiceProcessErr     = lVpErr;

   if(MP_SUCCESS != lVpErr )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] \33[31mCancelGetDtmfDigits Failed\33[0m",
            mMpAcuReq.mpReqData.stopMsg.seqNum,
            mMpAcuReq.mpReqData.stopMsg.circuitId, lCkt);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP93, lLogText);

      lAcuRsp.rspMsg              = MP_RSP_FAILURE;

      this->WriteRsp(lAcuRsp);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] \33[32mCancelGetDtmfDigits Success\33[0m",
            mMpAcuReq.mpReqData.stopMsg.seqNum,
            mMpAcuReq.mpReqData.stopMsg.circuitId, lCkt);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP94, lLogText);

      lAcuRsp.rspMsg              = MP_RSP_SUCCESS;

      this->WriteRsp(lAcuRsp);

      return true;
   }
}

//----------------------------------------------------------------------
// METHOD      : HandleStop   
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//----------------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandleStop(const MpStop stopMsg)
{
   INT16          lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN] = "";

   lCktId  = GetCircuitId(stopMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CktId:[%03d-%02d] Rcvd STOP", 
         stopMsg.seqNum, stopMsg.circuitId, lCktId);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP95, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_DISTN Status:%s", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP96, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }

      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_DISTN Status:%s"
                  " Hence issuing StopPlay", 
                  stopMsg.seqNum, stopMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP97, lLogText);

            if(pChnl->status != CKT_IDLE)
            {
               if(pChnl->status == CKT_PLAYING)
                  pChnl->status       = CKT_PLAY_STOPING;
               else if(pChnl->status == CKT_PLAYING_AND_GETDIG)
                  pChnl->status = CKT_PLAY_STOPING_AND_GETDIG;

               if(pChnl->acuCurPlay.playCtrl != ACU_PLAY_NONE)
                  pChnl->acuCurPlay.playCtrl = ACU_PLAY_NONE;

               pChnl->playStatus   = ACU_PLAY_NEXT_NOT_REQD;

               if(true != StopPlay(lCktId))
                  return MP_PLAY_STOP_CMD_ERR;
            }
         }
         break;

      case CKT_PLAY_STOPING:
      case CKT_PLAY_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_DISTN Status:%s"
                  " Stop in progress", stopMsg.seqNum, stopMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP99, lLogText);
         }
         break;

      case CKT_IDLE:
      case CKT_GETDIG:
      case CKT_PATCHED:
      case CKT_RECORDING:
      case CKT_RECORD_STOPING:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_RECORD_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_DISTN Status:%s"
                  " Not Playing To Stop", stopMsg.seqNum, stopMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP100, lLogText);
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_DISTN Status:%s" 
                  "Invalid Channel Status", stopMsg.seqNum, stopMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status)); 
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP101, lLogText);
            return MP_BADPARM_ERR;
         }
   }

   return MP_SUCCESS;
}

//----------------------------------------------------------------------
// METHOD      : IssuePlayStop
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::IssuePlayStop()
{
   TEXT                 lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   EnumVoiceProcessErr  lVpErr;
   MpResponseDetails    lAcuRsp;
   INT16 lCkt = GetCircuitId(mMpAcuReq.mpReqData.stopMsg.circuitId);

   memset(&lAcuRsp, 0, sizeof(lAcuRsp));

   lVpErr = HandleStop(mMpAcuReq.mpReqData.stopMsg);

   lAcuRsp.cmdType             = MC_DISTN;
   lAcuRsp.mpRspData.stopMsg   = mMpAcuReq.mpReqData.stopMsg;
   lAcuRsp.voiceProcessErr     = lVpErr;

   if (MP_SUCCESS != lVpErr)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] \33[3i1mPlay Stop Failed\33[0m",
            mMpAcuReq.mpReqData.stopMsg.seqNum,
            mMpAcuReq.mpReqData.stopMsg.circuitId, lCkt);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP102, lLogText);

      lAcuRsp.rspMsg              = MP_RSP_FAILURE;

      this->WriteRsp(lAcuRsp);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] \33[32mPlay Stop success\33[0m",
            mMpAcuReq.mpReqData.stopMsg.seqNum,
            mMpAcuReq.mpReqData.stopMsg.circuitId, lCkt);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP103, lLogText);

      lAcuRsp.rspMsg              = MP_RSP_SUCCESS;

      if(true == mSendRspFlag)
         this->WriteRsp(lAcuRsp);

      return true;
   }
}

//-------------------------------------------------------------
// FUNCTION    : GetMediaChannelStatus
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumChannelStatus
//-------------------------------------------------------------
EnumChannelStatus AculabMPMgr::GetMediaChannelStatus(INT16 lMediaChn, TEXT *lTxtStat)
{
   strcpy(lTxtStat, AcuMediaUtil::ConvertStatusToStr(gChannel[lMediaChn].status));
   return gChannel[lMediaChn].status;
}

//-------------------------------------------------------------
// FUNCTION    : StartRecord
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
// THROWS      : None
//-------------------------------------------------------------
BOOLEAN AculabMPMgr::StartRecord(const MpRecord recMsg)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1]  = "";
   TEXT           lFileName[MAX_LOG_TEXT_LEN + 1] = "";
   UINT16         lCktId = GetCircuitId(recMsg.circuitId);
   INT16          lResult = 0;

   SM_CHANNEL_SET_EVENT_PARMS   lSep;

   gChannel[lCktId].acuCurRecord.record = recMsg;
   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "%s: RefNum:%08X CktId:[%03d-%02d] RecFile", gProcessName,
         recMsg.seqNum, recMsg.circuitId, lCktId);

   memset(&gChannel[lCktId].acuCurRecord.rp, 0, sizeof(gChannel[lCktId].acuCurRecord.rp));

   memset(&gChannel[lCktId].acuCurRecord.rp, 0,
         sizeof(gChannel[lCktId].acuCurRecord.rp));
   gChannel[lCktId].acuCurRecord.rp.record_parms.channel = gChannel[lCktId].channelId;
   //gChannel[lCktId].acuCurRecord.rp.record_parms.type    = (kSMDataFormat) mDataFormat;
   gChannel[lCktId].acuCurRecord.rp.record_parms.type    = kSMDataFormat8KHzALawPCM;
   gChannel[lCktId].acuCurRecord.rp.record_parms.max_silence = recMsg.recTime;
   //gChannel[lCktId].acuCurRecord.rp.record_parms.max_silence = 2000;
   gChannel[lCktId].acuCurRecord.rp.record_parms.max_elapsed_time = recMsg.recTime;
   snprintf(lFileName, MAX_LOG_TEXT_LEN, "%s", recMsg.voiceFile);

   memset(&lSep, 0, sizeof(lSep));
   lSep.event = mAcuEventApi.mRecEvent;
   lSep.issue_events = kSMAnyChannelEvent;
   lSep.event_type = kSMEventTypeReadData;
   lSep.channel = gChannel[lCktId].channelId;
   lResult = sm_channel_set_event(&lSep);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] CMD:sm_channel_set_event failed REC <%d>", 
            gChannel[lCktId].acuCurRecord.record.seqNum,
            recMsg.circuitId, lCktId, lResult);
      TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP104, lLogText);
      return false;
   }

   lResult = sm_record_wav_start (lFileName, &gChannel[lCktId].acuCurRecord.rp);
   if ( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] CMD:sm_record_wav_start failed %s <%d>", 
            gChannel[lCktId].acuCurRecord.record.seqNum, recMsg.circuitId, lCktId,
            recMsg.voiceFile, lResult);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP105, lLogText);
      return false;
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN,
         "\33[32mRefNum:%08X CktId:[%03d-%02d] CMD: sm_record_wav_start Issued %s <%d>\33[0m", 
         gChannel[lCktId].acuCurRecord.record.seqNum, recMsg.circuitId, lCktId,
         recMsg.voiceFile, lResult);
   T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP106, lLogText);

   return true;
}

//-------------------------------------------------------------
// FUNCTION    : HandleStartRecord
// DESCRIPTION :
// PARAMETER   :
// RETURN      : EnumVoiceProcessErr
// THROWS      : None
//-------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandleStartRecord(const MpRecord recMsg)
{
   INT16          lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1]      = "";

   lCktId  = GetCircuitId(recMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] RecTime:%ld File:%s \33[32mRcvd RECORD Req\33[0m",
         recMsg.seqNum, recMsg.circuitId, lCktId, recMsg.recTime, recMsg.voiceFile);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP107, lLogText);

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTR Status:%s", 
                  recMsg.seqNum, recMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP108, lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }
         break;

      case CKT_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTR" 
                  " Status:%s Hence issuing StartRecord", 
                  recMsg.seqNum, recMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP109, lLogText);

            if(true != StartRecord(recMsg))
               return MP_RECORD_CMD_ERR;

            pChnl->acuCurRecord.record = recMsg;
            pChnl->playStatus      = ACU_PLAY_NEXT_NOT_REQD;
            pChnl->status          = CKT_RECORDING;
            pChnl->circuitId       = lCktId;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTR Status:%s Record Success",
                  recMsg.seqNum, recMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP110, lLogText);
         }
         break;

      case CKT_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTR Status:%s Hence issuing StartRecord",
                  recMsg.seqNum, recMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP111, lLogText);

            if(true != StartRecord(recMsg))
               return MP_RECORD_CMD_ERR;

            pChnl->acuCurRecord.record = recMsg;
            pChnl->playStatus      = ACU_PLAY_NEXT_NOT_REQD;
            pChnl->status          = CKT_RECORDING_AND_GETDIG;
            pChnl->circuitId       = lCktId;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTR "
                  "Status:%s RecToFile Success", recMsg.seqNum, recMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP112, lLogText);
         }
         break;

      case CKT_RECORDING:
      case CKT_RECORD_STOPING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTR Status:%s Cannot Start Record", 
                  recMsg.seqNum, recMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP113, lLogText);
            return MP_CKT_BUSY_ERR;
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_RECSTR Status:%s Invalid Channel Status",
                  recMsg.seqNum, recMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP114, lLogText);
            return MP_BADPARM_ERR;
         }
   }

   return MP_SUCCESS;
}

//----------------------------------------------------------------------
// METHOD      : StartPlay
// DESCRIPTION : Initiates play on the specified channel
// PARAMETER   : None
// RETURN      : EnumVoiceProcessErr
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::StartPlay(const MpPlay playMsg)
{
   INT16          lResult;
   TEXT           lLogText[MAX_LOG_TEXT_LEN]       = "";
   SM_CHANNEL_SET_EVENT_PARMS    lSep;
   UINT16         lCktId = GetCircuitId(playMsg.circuitId);

   gChannel[lCktId].acuCurPlay.play = playMsg;

   if(playMsg.vFileCount <= 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "%s: RefNum:%08X CktId:[%03d-%02d] MC_CNTTN "
            "PlayFile Failed FileCount Error", gProcessName, playMsg.seqNum,
            playMsg.circuitId, lCktId);
      gLog.GenerateLog(ACU_MP115, lLogText);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if(playMsg.vFileCount > 1)
      gChannel[lCktId].playStatus = ACU_PLAY_MULTIPLE_FILES;

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "%s: RefNum:%08X CktId:[%03d-%02d] PlayFile", gProcessName, 
         playMsg.seqNum, playMsg.circuitId, lCktId);

   memset(&gChannel[lCktId].acuCurPlay.fp, 0, sizeof(gChannel[lCktId].acuCurPlay.fp));
   memset(&gChannel[lCktId].acuCurDtmf.lp, 0, sizeof(gChannel[lCktId].acuCurDtmf.lp));

   if(!playMsg.voiceFile[0])
      gChannel[lCktId].acuCurPlay.fp.fd = NULL;
   else
   {
      gChannel[lCktId].acuCurPlay.fp.fd = fopen(playMsg.voiceFile[0], "rb");
      if (NULL == gChannel[lCktId].acuCurPlay.fp.fd)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s Err:%d", "Cannot open file", 
               errno);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACU_MP116, lLogText);
         return false;
      }
   }

   memset(&lSep, 0, sizeof(lSep));
   lSep.event = mAcuEventApi.mPlayEvent;
   lSep.issue_events = kSMAnyChannelEvent;
   lSep.event_type = kSMEventTypeWriteData;
   lSep.channel = gChannel[lCktId].channelId;
   lResult = sm_channel_set_event(&lSep);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] CMD:sm_channel_set_event PLAY failed <%d>", 
            playMsg.seqNum, playMsg.circuitId, lCktId, lResult);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP117, lLogText);
      this->ReleaseChannel (lSep.channel);
      return lResult;
   }

   gChannel[lCktId].acuCurPlay.fp.replay_parms.channel = gChannel[lCktId].channelId;
   gChannel[lCktId].acuCurPlay.fp.replay_parms.type    =  (kSMDataFormat) mDataFormat;
   gChannel[lCktId].acuCurPlay.fileCnt = playMsg.vFileCount;

   lResult = sm_replay_file_start (&gChannel[lCktId].acuCurPlay.fp);
   if ( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] CMD:sm_replay_file_start failed %s <%d>", 
            playMsg.seqNum, playMsg.circuitId, lCktId,
            playMsg.voiceFile[0], lResult );
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP118, lLogText);
      return false;
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] CMD:sm_replay_file_start Issued %s <%d>", 
         playMsg.seqNum, playMsg.circuitId, lCktId, playMsg.voiceFile[0], lResult);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP119, lLogText);

   if (true == playMsg.dtmfEnable)
   {
      #if 0
      TEXT lpstrDtmfCharSet[MAX_DTMF_RSP_DIGITS + 1] = "";

      if(gChannel[lCktId].acuCurDtmf.dtmf.numOfDig == 0)
      {
         strcpy(lpstrDtmfCharSet, "0123456789*#abcd");
         gChannel[lCktId].acuCurDtmf.dtmf.numOfDig = 1;
      }
      else
         strcpy(lpstrDtmfCharSet, 
               gChannel[lCktId].acuCurPlay.play.dtmfStopPlayCharSet);
   #endif

      memset(&lSep, 0, sizeof(lSep));
      lSep.event = mAcuEventApi.mDtmfEvent;
      lSep.issue_events = kSMAnyChannelEvent;
      lSep.event_type = kSMEventTypeRecog;
      lSep.channel = gChannel[lCktId].channelId;
      lResult = sm_channel_set_event(&lSep);
      if (lResult != 0)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "RefNum:%08X CktId:[%03d-%02d] CMD:sm_channel_set_event PLAY-DTMF failed <%d>", 
               playMsg.seqNum, playMsg.circuitId, lCktId, lResult);
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACU_MP120, lLogText);
         this->ReleaseChannel (lSep.channel);
         return lResult;
      }

      gChannel[lCktId].acuCurDtmf.lp.grunt_latency = 1000;
      gChannel[lCktId].acuCurDtmf.lp.enable_grunt_detection = 0;
      gChannel[lCktId].acuCurDtmf.lp.tone_detection_mode = kSMToneDetectionNoMinDuration;
      gChannel[lCktId].acuCurDtmf.lp.map_tones_to_digits = kSMDTMFToneSetDigitMapping;
      gChannel[lCktId].acuCurDtmf.lp.channel = gChannel[lCktId].channelId;
      gChannel[lCktId].acuCurDtmf.lp.enable_cptone_recognition = 0;
      gChannel[lCktId].acuCurDtmf.dtmf.numOfDig = strlen(playMsg.dtmfStopPlayCharSet);
      gChannel[lCktId].acuCurDtmf.dtmf.circuitId = gChannel[lCktId].acuCurPlay.play.circuitId;
      gChannel[lCktId].acuCurDtmf.dtmf.seqNum = gChannel[lCktId].acuCurPlay.play.seqNum;

      lResult = sm_listen_for(&gChannel[lCktId].acuCurDtmf.lp);
      if (lResult != 0)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "RefNum:%08X CktId:[%03d-%02d] CMD:sm_listen_for PLAY failed <%d>", 
               playMsg.seqNum, playMsg.circuitId, lCktId, lResult);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACU_MP121, lLogText);
         return false;
      }
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : StopPlay
// DESCRIPTION : Stops Play and wait for stop completion
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::StopPlay(INT16 lIndex)
{
   int lResult = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

      printf("TBR:-INSIDE StopPlay--------------------------------\n");
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CktId:[%03d-%02d] CMD:sm_replay_file_stop Issued", 
         gChannel[lIndex].acuCurPlay.play.seqNum, gChannel[lIndex].acuCurPlay.play.circuitId, lIndex);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP163, lLogText);

   lResult = sm_replay_file_stop(&gChannel[lIndex].acuCurPlay.fp, 1);  // nowait - enabled
   if(lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "RefNum:%08X CktId:[%03d-%02d] CMD:sm_replay_file_stop failed <%d>", 
            gChannel[lIndex].acuCurPlay.play.seqNum, gChannel[lIndex].acuCurPlay.play.circuitId,
            lIndex, lResult);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_MP166, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandlePlay
// DESCRIPTION : Initiates play on the specified channel
// PARAMETER   : None
// RETURN      : EnumVoiceProcessErr
//----------------------------------------------------------------------
EnumVoiceProcessErr AculabMPMgr::HandlePlay(const MpPlay playMsg)
{
   INT16          lCktId   = -1;
   acuChannel*    pChnl    = NULL;
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1]      = "";
   TEXT           lPlayCtrlText[MAX_LOG_TEXT_LEN + 1] = "";

   lCktId  = GetCircuitId(playMsg.circuitId);
   pChnl   = &gChannel[lCktId];

   snprintf(lLogText, MAX_LOG_TEXT_LEN,
         "RefNum:%08X CktId:[%03d-%02d] InTPly:%d RptPly:%d FlCnt:%d Tc:%s \33[32mRcvd PLAY Req\33[0m", 
         playMsg.seqNum, playMsg.circuitId, lCktId, playMsg.dtmfEnable, playMsg.repeatPlay, 
         playMsg.vFileCount, playMsg.dtmfStopPlayCharSet);

   if(true == playMsg.playCtrlEnable)
   {
      pChnl->acuCurPlay.playCtrl = ACU_PLAY_NONE;

      snprintf(lPlayCtrlText, MAX_LOG_TEXT_LEN,
            "Pas:%c Res:%c Fwd:%c Rwd:%c",
            playMsg.playCtrl.pauseDtmf, playMsg.playCtrl.resumeDtmf,
            playMsg.playCtrl.forwardDtmf, playMsg.playCtrl.rewindDtmf);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      strcat(lLogText, lPlayCtrlText);
   }

   T (gTrace, printf("%s: %s\n", gProcessName, lLogText);
         for(int idx = 0; idx < playMsg.vFileCount; idx++)
         printf("File:%d->%s\n", idx+1, playMsg.voiceFile[idx]););
   gLog.GenerateLog(ACU_MP165, lLogText);         

   switch(pChnl->status)
   {
      case CKT_UNAVAILABLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s",
                  playMsg.seqNum, playMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP124,  lLogText);
            return MP_CKT_UNAVAILABLE_ERR;
         }

      case CKT_IDLE:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s "
                  "So Issuing PlayFile", playMsg.seqNum, playMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP125, lLogText);

            if(true != StartPlay(playMsg))
               return MP_PLAY_CMD_ERR;

            pChnl->acuCurPlay.play = playMsg;

            if(pChnl->acuCurPlay.play.vFileCount > 1)
               pChnl->playStatus    = ACU_PLAY_MULTIPLE_FILES;
            else
               pChnl->playStatus    = ACU_PLAY_NEXT_NOT_REQD;

            pChnl->status        = CKT_PLAYING;
            pChnl->circuitId     = lCktId;

            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "%s: RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s"
                  " PlayFile Success", gProcessName, playMsg.seqNum, playMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP126,  lLogText);
         }
         break;

      case CKT_PLAYING:
      case CKT_PLAYING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s in-progress", 
                  playMsg.seqNum, playMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP127,  lLogText);

            if(pChnl->status != CKT_IDLE)
            {
               if(pChnl->status == CKT_PLAYING)
                  pChnl->status           = CKT_PLAY_STOPING;
               else if(pChnl->status == CKT_PLAYING_AND_GETDIG)
                  pChnl->status = CKT_PLAY_STOPING_AND_GETDIG;
               pChnl->playStatus       = ACU_PLAY_NEXT_IMMEDIATE;
               pChnl->acuNxtPlay.play  = playMsg;

               if(true != StopPlay(lCktId))
                  return MP_PLAY_STOP_CMD_ERR;

               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s PlayStatus:%s"
                     "StopPlayFile Success", playMsg.seqNum, playMsg.circuitId,
                     lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status),
                     AcuMediaUtil::ConvertPlayStatusToStr(pChnl->playStatus));
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP128, lLogText);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:CKT_IDLE "
                     "Changed from Playing Hence Playing", playMsg.seqNum,
                     playMsg.circuitId, lCktId);
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP129, lLogText);
               HandlePlay(playMsg);
            }
         }
         break;

      case CKT_PLAY_STOPING:
      case CKT_PLAY_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s STOP in-progress", 
                  playMsg.seqNum, playMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP130,  lLogText);

            if(pChnl->status != CKT_IDLE)
            {
               pChnl->playStatus       = ACU_PLAY_NEXT_REQD;
               pChnl->acuNxtPlay.play  = playMsg;
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s"
                     "Changed from stoping Hence Playing", playMsg.seqNum, playMsg.circuitId,
                     lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status));
               T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP131, lLogText);
               HandlePlay(playMsg);
            }
         }
         break;

      case CKT_RECORDING:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s Rec in-progress",
                  playMsg.seqNum, playMsg.circuitId, lCktId,
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP132, lLogText);
         }
         break;

      case CKT_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s"
                  "in-progress Hence issuing PlayFile", playMsg.seqNum, 
                  playMsg.circuitId, lCktId, 
                  AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP133,  lLogText);
         }
         break;

      case CKT_PATCHED:
      case CKT_RECORDING_AND_GETDIG:
      case CKT_RECORD_STOPING:
      case CKT_RECORD_STOPING_AND_GETDIG:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s "
                  "Start Play", playMsg.seqNum, playMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr(pChnl->status));
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP134,  lLogText);
            return MP_CKT_BUSY_ERR;
         }
         break;

      default:
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] MC_CNTTN Status:%s "
                  "Invalid Channel Status", playMsg.seqNum, playMsg.circuitId,
                  lCktId, AcuMediaUtil::ConvertStatusToStr( pChnl->status));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP135, lLogText);
            return MP_BADPARM_ERR;
         }
   }

   return MP_SUCCESS;
}

//----------------------------------------------------------------------
// METHOD      : ProcessModuleReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::ProcessModuleReq()
{
   EnumVoiceProcessErr	lVpErr;
   MpResponseDetails 	lAcuRsp;
   acuChannel           lAcuChannel;
   BOOLEAN 					lReturn = false;
   EnumChannelStatus    lTmpChnStatus;
   TEXT						lTxtStat[50] = "";
   TEXT            		lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   memset(&lAcuRsp, 0, sizeof(lAcuRsp));
   memset(&lAcuChannel, 0, sizeof(lAcuChannel));

   lAcuRsp.mpRspData.dummy.seqNum =  mMpAcuReq.mpReqData.dummy.seqNum;

   switch (mMpAcuReq.cmdType)
   {
      case MC_CNTTN:       
         {
            lVpErr = HandlePlay(mMpAcuReq.mpReqData.playMsg);

            lAcuRsp.cmdType				= MC_CNTTN;
            lAcuRsp.mpRspData.playMsg 	= mMpAcuReq.mpReqData.playMsg;
            lAcuRsp.voiceProcessErr	 	= lVpErr;

            if (MP_SUCCESS != lVpErr)
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CktId:[%03d] Play Failed",
                     mMpAcuReq.mpReqData.playMsg.seqNum, 
                     mMpAcuReq.mpReqData.playMsg.circuitId);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP136, lLogText);

               lAcuRsp.rspMsg 	= MP_RSP_FAILURE;
               lReturn				= false;

               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                     " RefNum:%08X CktId:[%03d] \33[32mPlay Success\33[0m",
                     mMpAcuReq.mpReqData.playMsg.seqNum,
                     mMpAcuReq.mpReqData.playMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP137, lLogText);

               lAcuRsp.rspMsg 				= MP_RSP_SUCCESS;
               lReturn							= true;

               //if(true == mSendRspFlag)
               if(true == mMpAcuReq.mpReqData.playMsg.nonBlocking)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_DISTN:
         {
            INT16 lCktId = GetCircuitId(mMpAcuReq.mpReqData.stopMsg.circuitId);
            lTmpChnStatus = 
               GetMediaChannelStatus (lCktId, lTxtStat);
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%08X CktId:[%03d-%02d] \33[33mStatus before issuing STOP cmd:%s\33[0m", 
                  mMpAcuReq.mpReqData.stopMsg.seqNum,
                  mMpAcuReq.mpReqData.stopMsg.circuitId, lCktId, lTxtStat);
            T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ACU_MP138, lLogText);

            switch(lTmpChnStatus)
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
            lVpErr = ConnectSpeech(mMpAcuReq.mpReqData.switchChnl);

            lAcuRsp.cmdType              = MC_CNTSP;
            lAcuRsp.mpRspData.switchChnl = mMpAcuReq.mpReqData.switchChnl;
            lAcuRsp.voiceProcessErr      = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] CktId:[%03d]\33[31m ConnectSpeech Failed\33[0m",
                     mMpAcuReq.mpReqData.switchChnl.seqNum,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_1,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_2);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP139, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] CktId:[%03d] \33[32mConnectSpeech success\33[0m",
                     mMpAcuReq.mpReqData.switchChnl.seqNum,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_1,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_2);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP140, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 						   = true;

               this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_DISSP:
         {
            lVpErr = DisconnectSpeech(mMpAcuReq.mpReqData.switchChnl);

            lAcuRsp.cmdType              = MC_DISSP;
            lAcuRsp.mpRspData.switchChnl = mMpAcuReq.mpReqData.switchChnl;
            lAcuRsp.voiceProcessErr      = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] CktId:[%03d] \33[31mDisconnectSpeech Failed\33[0m",
                     mMpAcuReq.mpReqData.switchChnl.seqNum,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_1,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_2);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP141, lLogText);

               lAcuRsp.rspMsg           = MP_RSP_FAILURE;
               lReturn 						 = false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] CktId:[%03d] \33[32mDisconnectSpeech success\33[0m",
                     mMpAcuReq.mpReqData.switchChnl.seqNum,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_1,
                     mMpAcuReq.mpReqData.switchChnl.circuitId_2);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP142, lLogText);

               lAcuRsp.rspMsg            = MP_RSP_SUCCESS;
               lReturn 						  = true;

               this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_GETDIG:
         {
            MpGetDigit rspDtmf;
            memset(&rspDtmf, 0, sizeof(MpGetDigit));
            lVpErr = HandleDtmf(mMpAcuReq.mpReqData.getDigit, rspDtmf);

            lAcuRsp.cmdType             = MC_GETDIG;
            lAcuRsp.mpRspData.getDigit  = mMpAcuReq.mpReqData.getDigit;
            lAcuRsp.voiceProcessErr     = lVpErr;

            if(MP_GET_DIG_CMD_ALREADY_DONE == lVpErr)
            {
               lAcuRsp.cmdType             = MC_GETDIGSTP;
               lAcuRsp.mpRspData.getDigit  = rspDtmf;
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mGetDtmfDigits Got Rsp with Digts:%s\33[0m",
                     mMpAcuReq.mpReqData.getDigit.seqNum,
                     mMpAcuReq.mpReqData.getDigit.circuitId,
                     rspDtmf.rspDigits);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP143, lLogText);
               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn                    = true;
               this->WriteRsp(lAcuRsp);
            }
            else if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mGetDtmfDigits Failed\33[0m",
                     mMpAcuReq.mpReqData.getDigit.seqNum,
                     mMpAcuReq.mpReqData.getDigit.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP144, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] HandleDtmf success",
                     mMpAcuReq.mpReqData.getDigit.seqNum,
                     mMpAcuReq.mpReqData.getDigit.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP145, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_GETDIGSTP:
         {
            lVpErr = CancelGetDtmfDigits(mMpAcuReq.mpReqData.stopMsg, 
                  lAcuRsp.mpRspData.getDigit.rspDigits);

            lAcuRsp.cmdType             = MC_GETDIGSTP;
            lAcuRsp.mpRspData.stopMsg   = mMpAcuReq.mpReqData.stopMsg;
            lAcuRsp.voiceProcessErr     = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mCancelGetDtmfDigits Failed\33[0m",
                     mMpAcuReq.mpReqData.stopMsg.seqNum,
                     mMpAcuReq.mpReqData.stopMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP146, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mCancelGetDtmfDigits Success\33[0m",
                     mMpAcuReq.mpReqData.stopMsg.seqNum,
                     mMpAcuReq.mpReqData.stopMsg.circuitId + 1);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP147, lLogText);

               lAcuRsp.rspMsg           = MP_RSP_SUCCESS;
               lReturn 						 = true;

               this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_RECSTR:
         {
            lVpErr = HandleStartRecord(mMpAcuReq.mpReqData.recordMsg);

            lAcuRsp.cmdType             = MC_RECSTR;
            lAcuRsp.mpRspData.recordMsg = mMpAcuReq.mpReqData.recordMsg;
            lAcuRsp.voiceProcessErr     = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mStartRecord Failed\33[0m",
                     mMpAcuReq.mpReqData.recordMsg.seqNum,
                     mMpAcuReq.mpReqData.recordMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP148, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 						   = false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mStartRecord Success\33[0m",
                     mMpAcuReq.mpReqData.recordMsg.seqNum,
                     mMpAcuReq.mpReqData.recordMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP149, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_RECSTP:
         {
            lVpErr = HandleStopRecord(mMpAcuReq.mpReqData.stopMsg);

            lAcuRsp.cmdType             = MC_RECSTP;
            lAcuRsp.mpRspData.stopMsg   = mMpAcuReq.mpReqData.stopMsg;
            lAcuRsp.voiceProcessErr     = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mStopRecord Failed\33[0m",
                     mMpAcuReq.mpReqData.stopMsg.seqNum,
                     mMpAcuReq.mpReqData.stopMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP150, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mStopRecord Success\33[0m",
                     mMpAcuReq.mpReqData.stopMsg.seqNum,
                     mMpAcuReq.mpReqData.stopMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP151, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_PAUSE:
         {
            lVpErr = HandlePause(mMpAcuReq.mpReqData.pauseMsg);

            lAcuRsp.cmdType              = MC_PAUSE;
            lAcuRsp.mpRspData.pauseMsg   = mMpAcuReq.mpReqData.pauseMsg;
            lAcuRsp.voiceProcessErr      = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mPause Play Failed\33[0m",
                     mMpAcuReq.mpReqData.pauseMsg.seqNum,
                     mMpAcuReq.mpReqData.pauseMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP152, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mPause Play Success\33[0m",
                     mMpAcuReq.mpReqData.pauseMsg.seqNum,
                     mMpAcuReq.mpReqData.pauseMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP153, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_RESUME:
         {
            lVpErr = HandleResume(mMpAcuReq.mpReqData.resumeMsg);

            lAcuRsp.cmdType              = MC_RESUME;
            lAcuRsp.mpRspData.resumeMsg  = mMpAcuReq.mpReqData.resumeMsg;
            lAcuRsp.voiceProcessErr      = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mResume Play  Failed\33[0m",
                     mMpAcuReq.mpReqData.resumeMsg.seqNum,
                     mMpAcuReq.mpReqData.resumeMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP154, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mResume Play Success\33[0m",
                     mMpAcuReq.mpReqData.resumeMsg.seqNum,
                     mMpAcuReq.mpReqData.resumeMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP155, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_FORWARD:
         {
            lVpErr = HandleForward(mMpAcuReq.mpReqData.forwardMsg);

            lAcuRsp.cmdType              = MC_FORWARD;
            lAcuRsp.mpRspData.forwardMsg = mMpAcuReq.mpReqData.forwardMsg;
            lAcuRsp.voiceProcessErr      = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mForward Play Failed\33[0m",
                     mMpAcuReq.mpReqData.forwardMsg.seqNum,
                     mMpAcuReq.mpReqData.forwardMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP156, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mForward Play Success\33[0m",
                     mMpAcuReq.mpReqData.forwardMsg.seqNum,
                     mMpAcuReq.mpReqData.forwardMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP157, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 							= true;

               if(true == mSendRspFlag)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      case MC_REWIND:
         {
            lVpErr = HandleRewind(mMpAcuReq.mpReqData.rewindMsg);

            lAcuRsp.cmdType              = MC_REWIND;
            lAcuRsp.mpRspData.rewindMsg  = mMpAcuReq.mpReqData.rewindMsg;
            lAcuRsp.voiceProcessErr      = lVpErr;

            if(MP_SUCCESS != lVpErr )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[31mRewind Play Failed\33[0m",
                     mMpAcuReq.mpReqData.rewindMsg.seqNum,
                     mMpAcuReq.mpReqData.rewindMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP158, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_FAILURE;
               lReturn 							= false;
               this->WriteRsp(lAcuRsp);
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN,
                     "RefNum:%08X CktId:[%03d] \33[32mRewind Play Success\33[0m",
                     mMpAcuReq.mpReqData.rewindMsg.seqNum,
                     mMpAcuReq.mpReqData.rewindMsg.circuitId);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               gLog.GenerateLog(ACU_MP159, lLogText);

               lAcuRsp.rspMsg             = MP_RSP_SUCCESS;
               lReturn 							= true;
               if(true == mSendRspFlag)
                  this->WriteRsp(lAcuRsp);
            }
         }
         break;

      default:
         {
            lAcuRsp.cmdType				= MC_INVAL_CMD_TYPE;
            lAcuRsp.rspMsg             = MP_RSP_FAILURE;
            lAcuRsp.voiceProcessErr		= MP_INVALID_CMD_ERR;
            gLog.GenerateLog(ACU_MP160, lLogText);

            lReturn                    = false;
            this->WriteRsp(lAcuRsp);
         }
   }

   return lReturn;
}

//----------------------------------------------------------------------
// METHOD      : SetMsgType
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
INT16 AculabMPMgr::SetMsgType(EnumMediaCmdTypes lCmd, MpData &lMpData)
{
   switch(lCmd)
   {
      case MC_INVAL_CMD_TYPE:
         return 0;

      case MC_CNTTN:
         return lMpData.playMsg.circuitId;

      case MC_DISTN:
         return lMpData.stopMsg.circuitId;

      case MC_CNTSP:
         return lMpData.switchChnl.circuitId_1;

      case MC_DISSP:
         return lMpData.switchChnl.circuitId_1;

      case MC_GETDIG:
         return lMpData.getDigit.circuitId;

      case MC_GETDIGSTP:
         return lMpData.stopMsg.circuitId;

      case MC_RECSTR:
         return lMpData.recordMsg.circuitId;

      case MC_RECSTP:
         return lMpData.stopMsg.circuitId;

      case MC_SETVOL:
         return lMpData.setVolSpd.circuitId;

      case MC_SETSPD:
         return lMpData.setVolSpd.circuitId;

      default:
         return 0;
   }
}

//----------------------------------------------------------------------
// METHOD      : WriteRsp
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::WriteRsp(MpResponseDetails &lAcuRsp)
{
   UINT16		  lCkt = 0;
   unsigned int  lSeq = 0;
   TEXT          lLogText[MAX_LOG_TEXT_LEN] = "";
   TEXT          lTxt[MAX_LOG_TEXT_LEN] = "";
   INT16         lMsgType = 0;

   snprintf(lTxt, MAX_LOG_TEXT_LEN, "CmdType:%s", 
         AcuMediaUtil::ConvertCmdToStr(lAcuRsp.cmdType, lSeq, lCkt, lAcuRsp.mpRspData));
   T(gTrace, printf("%s: %s\n", gProcessName, lTxt););
   gLog.GenerateLog(ACU_MP161, lTxt);

   lMsgType = SetMsgType(lAcuRsp.cmdType, lAcuRsp.mpRspData);

   if(false == mMsgQueApi.WriteToRspQue (lAcuRsp, lMsgType))
      return false;

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CircuitId:[%03d] Rsp:%s %s"
         " MsgType:%d Wrote to MP Respone Q", lSeq, lCkt, 
         (MP_RSP_SUCCESS == lAcuRsp.rspMsg) ? "MP_RSP_SUCCESS" : "MP_RSP_FAILURE", 
         lTxt, lMsgType); 
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP162, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadModuleReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::ReadModuleReq(INT16 lMsgType)
{
   UINT16  lCkt = 0;
   unsigned int lSeq = 0;
   MpResponseDetails lAcuRsp;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   memset(&mMpAcuReq, 0, sizeof (mMpAcuReq));

   if (false == mMsgQueApi.ReadFromModuleReqQue (mMpAcuReq, lMsgType))
      return false;

   lSeq = mMpAcuReq.mpReqData.dummy.seqNum;
   if(false == this->CheckCircuitId(mMpAcuReq.cmdType, mMpAcuReq.mpReqData))
   {
      memset(&lAcuRsp, 0, sizeof(lAcuRsp));
      lAcuRsp.cmdType    = mMpAcuReq.cmdType;
      lAcuRsp.mpRspData  = mMpAcuReq.mpReqData;
      lAcuRsp.rspMsg     = MP_RSP_FAILURE;
      lAcuRsp.voiceProcessErr = MP_INVALID_CHANNEL;

      this->WriteRsp(lAcuRsp);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "CmdType:%s",
         AcuMediaUtil::ConvertCmdToStr(mMpAcuReq.cmdType, lSeq, lCkt, mMpAcuReq.mpReqData));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Seq:%08X CktId:[%03d] "
         "MsgType:%d \33[33mRead Module Request\33[0m", lSeq, lCkt, lMsgType);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP200, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : GetMsgType
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
INT16 AculabMPMgr::GetMsgType(UINT16 lCktId)
{
   INT16 lMsgType = 0;

   switch(mModules)
   {
      case ACU_TWO_DSP_PER_CARD:
         if (lCktId >= 1 && lCktId <= 60)
            lMsgType = 1;
         else if(lCktId >= 61 && lCktId <= 120)
            lMsgType = 2;
         break;

      case ACU_FOUR_DSP_PER_CARD:
         if (lCktId >= 1 && lCktId <= 60)
            lMsgType = 1;
         else if (lCktId >= 61 && lCktId <= 120)
            lMsgType = 2;
         else if (lCktId >= 121 && lCktId <= 180)
            lMsgType = 3;
         else if (lCktId >= 181 && lCktId <= 240)
            lMsgType = 4;
         break;

      case ACU_EIGHT_DSP_PER_CARD:
         if (lCktId >= 1 && lCktId <= 60)
            lMsgType = 1;
         else if (lCktId >= 61 && lCktId <= 120)
            lMsgType = 2;
         else if (lCktId >= 121 && lCktId <= 180)
            lMsgType = 3;
         else if (lCktId >= 181 && lCktId <= 240)
            lMsgType = 4;
         else if (lCktId >= 241 && lCktId <= 300)
            lMsgType = 5;
         else if (lCktId >= 301 && lCktId <= 360)
            lMsgType = 6;
         else if (lCktId >= 361 && lCktId <= 420)
            lMsgType = 7;
         else if (lCktId >= 421 && lCktId <= 480)
            lMsgType = 8;
         break;

      default:
         break;
   }

   return lMsgType;
}

//----------------------------------------------------------------------
// METHOD      : DistributeReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::DistributeReq()
{
   UINT16 lCkt = 0;
   INT16  lMsgType = 0;
   unsigned int lSeq = 0;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   switch(mMpReq.cmdType)
   {
      case MC_INVAL_CMD_TYPE:
         break;

      case MC_CNTTN:
         lMsgType = GetMsgType(mMpReq.mpReqData.playMsg.circuitId);
         break;

      case MC_DISTN:
         lMsgType = GetMsgType(mMpReq.mpReqData.stopMsg.circuitId);
         break;

      case MC_CNTSP:
         lMsgType = GetMsgType(mMpReq.mpReqData.switchChnl.circuitId_1);
         break;

      case MC_DISSP:
         lMsgType = GetMsgType(mMpReq.mpReqData.switchChnl.circuitId_1);
         break;

      case MC_GETDIG:
         lMsgType = GetMsgType(mMpReq.mpReqData.getDigit.circuitId);
         break;

      case MC_GETDIGSTP:
         lMsgType = GetMsgType(mMpReq.mpReqData.stopMsg.circuitId);
         break;

      case MC_RECSTR:
         lMsgType = GetMsgType(mMpReq.mpReqData.recordMsg.circuitId);
         break;

      case MC_RECSTP:
         lMsgType = GetMsgType(mMpReq.mpReqData.stopMsg.circuitId);
         break;

      case MC_SETVOL:
         lMsgType = GetMsgType(mMpReq.mpReqData.setVolSpd.circuitId);
         break;

      case MC_SETSPD:
         lMsgType = GetMsgType(mMpReq.mpReqData.setVolSpd.circuitId);
         break;

      case MC_PAUSE:
         lMsgType = GetMsgType(mMpReq.mpReqData.pauseMsg.circuitId);
         break;

      case MC_RESUME:
         lMsgType = GetMsgType(mMpReq.mpReqData.resumeMsg.circuitId);
         break;

      case MC_FORWARD:
         lMsgType = GetMsgType(mMpReq.mpReqData.forwardMsg.circuitId);
         break;

      case MC_REWIND:
         lMsgType = GetMsgType(mMpReq.mpReqData.rewindMsg.circuitId);
         break;

      default:
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Invalid CmdType:%s", 
               AcuMediaUtil::ConvertCmdToStr(mMpReq.cmdType, lSeq, lCkt, mMpReq.mpReqData));
         TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ACU_MP201, lLogText);
         break;
   }
   if (false == mMsgQueApi.WriteToModuleReqQue (mMpReq, lMsgType))
      return false;

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadReq
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN AculabMPMgr::ReadReq()
{
   UINT16  lCkt = 0;
   unsigned int lSeq = 0;
   MpResponseDetails lAcuRsp;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   memset(&mMpReq, 0, sizeof (mMpReq));

   if (false == mMsgQueApi.ReadFromReqQue (mMpReq, mCardId))
      return false;

   lSeq = mMpReq.mpReqData.dummy.seqNum;
   if(false == this->CheckCircuitId(mMpReq.cmdType, mMpReq.mpReqData))
   {
      memset(&lAcuRsp, 0, sizeof(lAcuRsp));
      lAcuRsp.cmdType    = mMpReq.cmdType;
      lAcuRsp.mpRspData  = mMpReq.mpReqData;
      lAcuRsp.rspMsg     = MP_RSP_FAILURE;
      lAcuRsp.voiceProcessErr = MP_INVALID_CHANNEL;

      this->WriteRsp(lAcuRsp);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "CmdType:%s",
         AcuMediaUtil::ConvertCmdToStr(mMpReq.cmdType, lSeq, lCkt, mMpReq.mpReqData));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Seq:%08X CktId:[%03d] %s \33[33mRead Request\33[0m",
         lSeq, lCkt, lLogText);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_MP163, lLogText);

   return true;
}
