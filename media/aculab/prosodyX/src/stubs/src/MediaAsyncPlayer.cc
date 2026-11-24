static const char* id="@(#) $Id: MediaAsyncPlayer.cc,v 1.1.4.3 2016/04/12 21:59:30 shijuc Exp $";
//-----------------------------------------------------------------------------
// NAME
//   MediaHandlerMain.cc -- MediaHandler Main process
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
// Originated : Roopa A. M                                     DATE:01-SEP-2010
//------------------------------------------------------------------------------
#include "MediaAsyncHdlr.h"

extern MediaHandler gMediaHandlerObj;

tSMEventId     gEvent;
BOOLEAN        gPlayAction = false, gPlayCtrl = true;
runningChannel gRcp[256*TIMESLOTS_PER_STREAM];

//-------------------------------------------------------------
// METHOD        : MediaHandler
// DESCRIPTION   : Constructor
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
MediaHandler::MediaHandler()
{
}

//-------------------------------------------------------------
// METHOD        : MediaHandler
// DESCRIPTION   : Destructor
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
MediaHandler::~MediaHandler()
{
}

//-------------------------------------------------------------
// METHOD        : Init
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN MediaHandler::Init()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN+1] = "";

   if(false == this->InitializeCard())
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,"Failed to Initialize the Card and Ports");
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// METHOD        : DeInit
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
void MediaHandler::DeInit()
{
   ACU_CLOSE_CARD_PARMS       lCloseCardParms;
   ACU_CLOSE_PROSODY_PARMS    lCloseProsodyParms;
   ACU_CLOSE_SWITCH_PARMS     lCloseSwitchParms;

   for(UINT32 i = 0 ; i < mCardDetails.count; i++)
   {
      INIT_ACU_STRUCT(&lCloseProsodyParms);
      lCloseProsodyParms.card_id = mCardId[i];
      acu_close_prosody(&lCloseProsodyParms);
   }

   for(UINT32 i = 0 ; i < mCardDetails.count; i++)
   {
      INIT_ACU_STRUCT(&lCloseSwitchParms);
      lCloseSwitchParms.card_id = mCardId[i];
      acu_close_switch(&lCloseSwitchParms);
   }

   for(UINT32 i = 0 ; i < mCardDetails.count; i++)
   {
      INIT_ACU_STRUCT(&lCloseCardParms);
      lCloseCardParms.card_id = mCardId[i];
      acu_close_card(&lCloseCardParms);
   }
}

//-------------------------------------------------------------
// METHOD        : DisAssociateEvent
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        :
//-------------------------------------------------------------
BOOLEAN MediaHandler::DisAssociateEvent(int index)
{
   int      lResult = 0;
   TEXT     lLogText[MAX_LOG_TEXT_LEN+1] = "";

   SM_CHANNEL_SET_EVENT_PARMS lSe;
   OUTPUT_PARMS  lDcp; 

   memset(&lSe, 0, sizeof(lSe));
   memset(&lDcp, 0, sizeof(lDcp));

   lSe.channel      = gRcp[index].channel;
   lSe.event        = gEvent;
   lSe.event_type   = kSMEventTypeWriteData;
   lSe.issue_events = kSMChannelNoEvent;

   lResult = sm_channel_set_event( &lSe );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }

   fclose(gRcp[index].fp.fd);

   //if ( gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id )
   {
      lDcp.ost  = gRcp[index].outPort.st;
      lDcp.ots  = gRcp[index].outPort.ts;
      lDcp.mode = DISABLE_MODE;
      lDcp.size = sizeof(lDcp);

      lResult = sw_set_output(gMediaHandlerObj.mOpenCardDet.card_id, &lDcp);
      if ( lResult != 0)
      {
         printf ("Error could not break connection from call to Prosody <%d>", lResult);
         return false;
      }
   }

   this->ReleaseChannel (gRcp[index].channel);
   memset(&gRcp[index], 0, sizeof(runningChannel));
   T(gTrace, printf("%s: \33[32mDisconnected from Prosody, Released Channel "
            "and cleared m/y\33[0m\n", gProcessName););

   return true;
}

//-------------------------------------------------------------
// METHOD        : ReleaseChannel
// DESCRIPTION   : Release the channel associated with this
//                 Prosody resource. This function also releases
//                 the stream & timeslot used by the channel
// PARAMETER     : NONE
// RETURN        : true on success
//-------------------------------------------------------------
BOOLEAN MediaHandler::ReleaseChannel(tSMChannelId lChannelId)
{
   int lResult = 0;

   lResult = sm_channel_release(lChannelId);
   if(lResult != 0 )
   {
      printf("Error: Could not free Prosody channel <%d>", lResult);
      return false;
   }
   lChannelId = kSMNullChannelId;

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandlePause()
// DESCRIPTION : Pauses the play on current channel
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
BOOLEAN MediaHandler::HandlePause(int lIndex)
{
   int lResult = 0;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   //lResult = sm_replay_file_stop(&gRcp[2].fp, 0);
   if(lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_replay_file_stop() failed <%d>", lResult);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandleResume ()
// DESCRIPTION : Resumes the paused play on channel
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
BOOLEAN MediaHandler::HandleResume (int lIndex)
{
   int lResult = 0;
   tSM_UT32 lStartOffset = 0;
   FILE *lFp;
   BOOLEAN lRepeat = true;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lBuffer[kSMMaxReplayDataBufferSize] = "";
   SM_REPLAY_PARMS    lSmReplay;
   SM_REPLAY_STATUS_PARMS  lSmStatus;
   SM_TS_DATA_PARMS        lSmTsData;

   gRcp[lIndex].fp.fd = fopen(mFileName, "rb");
   if (!lFp)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s", "Cannot open file");
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
   }
   sleep(3);

   gRcp[lIndex].fp.offset = gRcp[lIndex].playCtrlPos;
   lResult = sm_replay_file_start(&gRcp[lIndex].fp);
   if(lResult)
   {
      printf("Error: Could not start replay in replay() %s <%d>, terminating\n", mFileName, lResult);
      fclose(lFp);
   }

   printf("%s: RESUME Completed Successfully\n", gProcessName);

   //fclose(lFp);
   //DisAssociateEvent(lIndex);

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandleRewind ()
// DESCRIPTION : Rewinds the play on channel
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
BOOLEAN MediaHandler::HandleRewind (int lIndex)
{
   SM_REPLAY_ADJUST_PARMS lRaParms;

   INIT_ACU_SM_STRUCT(&lRaParms);

   lRaParms.channel = gRcp[lIndex].channel;
   lRaParms.speed = -100;

   int lResult = sm_replay_adjust(&lRaParms);

   if(lResult != 0)
   {
      printf("%s: REWIND Error: sm_replay_adjust()\n", gProcessName);
      return false;
   }

   printf("%s: REWIND - Successfully Adjusted Replay Parms\n");

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandleForward ()
// DESCRIPTION : Forwards the play on channel
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
BOOLEAN MediaHandler::HandleForward (int lIndex)
{
   SM_REPLAY_ADJUST_PARMS lRaParms;

   INIT_ACU_SM_STRUCT(&lRaParms);

   lRaParms.channel = gRcp[lIndex].channel;
   lRaParms.speed = 200;

   int lResult = sm_replay_adjust(&lRaParms);

   if(lResult != 0)
   {
      printf("%s: FORWARD - Error: sm_replay_adjust()\n", gProcessName);
      return false;
   }

   printf("%s: FORWARD - Successfully Adjusted Replay Parms\n");
   return true;
}

//----------------------------------------------------------------------
// METHOD      : PlayThread
// DESCRIPTION : Check Channel Status Thread
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *PlayThread (void *lPtr)
{
   int      lResult = 0;
   struct   pollfd pfd;
   BOOLEAN     lPlayThreadFlg = false;

   memset(&pfd, 0, sizeof(pfd));

   pfd.fd = gEvent.fd;
   pfd.events = gEvent.mode;

   T(gTrace, printf("%s: Fd:%d Mode:%d gEvent:%d-%d\n", gProcessName, 
            pfd.fd, pfd.events, gEvent.fd, gEvent.mode););

   for(;;)
   {
      lResult = poll(&pfd, 1, -1);
      if (lResult < 0) 
      {
         TERR (gTrace, printf("%s: poll()failed Cannot wait for channels\n", 
                  gProcessName););
         pthread_exit(NULLPTR);
         return NULLPTR;
      }

      if (pfd.revents) 
      {
         int lIndex = 0;
         gRcp[lIndex].playCtrlPos = 0;
         sm_replay_status_parms lStatusParms;

         memset(&lStatusParms, 0, sizeof(lStatusParms));

         lStatusParms.channel = kSMNullChannelId;
         lResult = sm_replay_status (&lStatusParms);
         if (lResult == ERR_SM_NO_SUCH_CHANNEL)  
            continue; 

         if (lResult) 
         {
            TERR (gTrace, printf("%s: Cannot check channel status\n", 
                     gProcessName););
         }

         lIndex = sm_get_channel_ix(lStatusParms.channel);
         if(lIndex < 0)
            printf("%s: Index:%d\n", gProcessName, lIndex);

         T(gTrace, printf("%s: ChannelId:%d Status:%d\n", gProcessName, 
                  lStatusParms.channel, lStatusParms.status););

         lResult = sm_replay_file_progress_istatus(&gRcp[lIndex].fp, lStatusParms.status);

         //if(lStatusParms.status == kSMReplayStatusComplete && gPlayCtrl == true && gPlayAction == false)
         if(lStatusParms.status != kSMReplayStatusComplete)
         {
            //gRcp[lIndex].playCtrlPos = lStatusParms.offset;
            sleep(3);
            gMediaHandlerObj.HandleForward(lIndex);
            //T(gTrace, printf("PAUSE Success - Played Offset:%d\n", gRcp[lIndex].playCtrlPos););
            //gMediaHandlerObj.HandleResume(lIndex);
            sleep(10);
            gMediaHandlerObj.HandleRewind(lIndex);
            gPlayAction = true;
         }

         if(lResult != ERR_SM_PENDING && gPlayAction == true)
         {
            gPlayCtrl = false;
            T(gTrace, printf("%s: Play Completed Successfully ChannelId:%d status:%d\n",
                     gProcessName, gRcp[lIndex].channel, lResult););

            gMediaHandlerObj.DisAssociateEvent(lIndex);
         }
      } 
   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}

//-------------------------------------------------------------
// METHOD        : CreateCChan
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : INT
//-------------------------------------------------------------
BOOLEAN MediaHandler::CreateCChan(Channel & rMediaPort)
{
   int lIndex = 0;
   int lResult = 0;

   struct      tm* ltmStruct;
   char        lFileNameTime[100];
   struct      timeval    ltimeval;

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   SM_SWITCH_CHANNEL_PARMS       sw;
   SM_CHANNEL_SET_EVENT_PARMS    se;
   SM_CHANNEL_ALLOC_PLACED_PARMS ac;
   SM_CHANNEL_INFO_PARMS         ci;
   OUTPUT_PARMS                  cp; 

   memset(&ac, 0, sizeof(ac));
   memset(&sw, 0, sizeof(sw));
   memset(&se, 0, sizeof(se));
   memset(&cp, 0, sizeof(cp));
   memset(&ci, 0, sizeof(ci));

   ac.type   = kSMChannelTypeOutput;
   ac.module = mOpenModule[rMediaPort.moduleIndx].module_id;
   lResult = sm_channel_alloc_placed(&ac);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error: Unable to allocate channel <%d>", lResult);
      TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   lIndex = sm_get_channel_ix (ac.channel);
   if(lIndex < 0)
   {   
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error:sm_get_channel_ix() failed  <%d>", lIndex);
      TERR(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   memset(&gRcp[lIndex], 0, sizeof(runningChannel));

   gRcp[lIndex].play = PLAY_ONCE;
   gRcp[lIndex].channel = ac.channel;
   gRcp[lIndex].outPort.st = mCmdStream;
   gRcp[lIndex].outPort.ts = rMediaPort.ts;
   gRcp[lIndex].outPort.type = (kSMTimeslotType) rMediaPort.type;
   gRcp[lIndex].fp.replay_parms.channel = ac.channel;
   gRcp[lIndex].fp.replay_parms.type    = kSMDataFormat8KHzALawPCM;
   //gRcp[lIndex].fp.replay_parms.speed = 150;

   if(!mFileName)
      gRcp[lIndex].fp.fd = 0;
   else
   {
      gRcp[lIndex].fp.fd = fopen(mFileName, "rb");
      if (!gRcp[lIndex].fp.fd) 
      {
         char *n;
         n = (char*) malloc(strlen(mFileName) + sizeof("File: ''"));
         if (n) sprintf(n, "File: '%s'", mFileName);
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s", "Cannot open file");
         T(gTrace, printf("%s: %s\n", gProcessName, lLogText););         
         free(n);
         return -1;
      }
   }

   sw.st = rMediaPort.st;
   sw.ts = gRcp[lIndex].outPort.ts;
   sw.type = (kSMTimeslotType) gRcp[lIndex].outPort.type;
   sw.channel = gRcp[lIndex].fp.replay_parms.channel;
   lResult = sm_switch_channel_output(&sw);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_switch_channel_output() failed <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return lResult;
   }

   se.event = gEvent;
   se.issue_events = kSMAnyChannelEvent;
   se.event_type = kSMEventTypeWriteData;
   se.channel = gRcp[lIndex].fp.replay_parms.channel;
   lResult = sm_channel_set_event(&se);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_channel_set_event() failed  <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   ci.channel = gRcp[lIndex].fp.replay_parms.channel;
   lResult = sm_channel_info (&ci);
   if ( lResult != 0 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error: Could not get channel info <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   //if ( gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id )
   {
      cp.size = sizeof(OUTPUT_PARMS);
      cp.ist = rMediaPort.st;
      cp.its = rMediaPort.ts;
      cp.mode = CONNECT_MODE;
      cp.ost = mCmdStream;
      cp.ots = sw.ts;

      printf("-----------------------------------------------------------------------------\n");
      printf("Connect Params: ist:%ld its:%ld mode:%ld ost:%ld ots:%ld card:%ld \n", 
            cp.ist, cp.its, cp.mode, cp.ost, cp.ots, gMediaHandlerObj.mOpenCardDet.card_id);
      printf("-----------------------------------------------------------------------------\n");

      lResult = sw_set_output(gMediaHandlerObj.mOpenCardDet.card_id, &cp);
      if ( lResult != 0)
      {
         printf ("Error could not make switch connection <%d>", lResult);
         return lResult;
      }
   }

   gettimeofday(&ltimeval, NULL);
   ltmStruct = localtime(&(ltimeval.tv_sec));

   sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%06d",
         ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
         ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "starting replay on %d:%d %d:%d\n", 
         cp.ist, cp.its, cp.ost, cp.ots);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

   printf("StartTime:%s \33[32m File Play Started ChannelId:%d\33[0m[Status:%d]\n FileName:%s[fd:%d]\n", 
         lFileNameTime, gRcp[lIndex].fp.replay_parms.channel, gRcp[lIndex].fp.status, mFileName, 
         gRcp[lIndex].fp.fd);
   printf("=========================================================================================\n");

   lResult = sm_replay_file_start (&gRcp[lIndex].fp);
   if ( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not initiate wav %s replay <%d>", mFileName, lResult );
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return lResult;
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN,
         "INFO: Success to initiate %s replay <%d>", mFileName, lResult );
   T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

   if (! (++rMediaPort.ts & 0x1f)) 
   {
      rMediaPort.st ^= 1;
      rMediaPort.ts -= 32;
   }


#if 0
   int lStop = 0;

   printf("Enter Digit (1-9) to stop ");
   scanf("%d", &lStop);
   if(lStop > 0)
   {
      // Sending PLAY STOP CMD
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "sm_replay_file_stop Cmd Issued CktId:%d", lIndex);
      T (gTrace, printf("%s: %s Fp:%d\n", gProcessName, lLogText, gRcp[lIndex].fp););

      //lResult = sm_replay_file_stop(&gRcp[lIndex].fp, 0);
      lResult = sm_replay_file_stop(&gRcp[lIndex].fp, 1);
      if(lResult != 0)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "sm_replay_file_stop() failed <%d>", lResult);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         return false;
      }
      printf("TBR:=========== SENDING STOP ===============\n");
   }
#endif
   return lResult;
}

//-------------------------------------------------------------
// METHOD        : InitializeCard
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN MediaHandler::InitializeCard()
{
   int        lResult;
   int        lModuleIndx = 0;
   INT16      lModules;
   TEXT       lLogText[MAX_LOG_TEXT_LEN+1] = "";

   Channel      lMediaPort;
   pthread_t    lResponseThread;
   pthread_t    lPlayCtrlThread;

   SM_SWITCH_CHANNEL_PARMS       lSwitch;
   SM_CARD_INFO_PARMS            lSmCardInfo;
   SM_MODULE_INFO_PARMS          lModuleInfo;
   SM_CHANNEL_ALLOC_PLACED_PARMS lChannelAlloc;
   ACU_CARD_INFO_PARMS           lCardInfo;
   ACU_OPEN_PROSODY_PARMS        lOpenProsody;
   ACU_OPEN_SWITCH_PARMS         lOpenSwitch;

   tSMChannelId lAnyChann = kSMNullChannelId;

   if (COMPANDING == COMPANDING_ALAW)
      lMediaPort.type  = kSMTimeslotTypeALaw;
   else if (COMPANDING == COMPANDING_MULAW)
      lMediaPort.type  = kSMTimeslotTypeMuLaw;

   INIT_ACU_STRUCT (&mCardDetails);
   lResult = acu_get_system_snapshot(&mCardDetails);
   if (lResult)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed to get the System SnapShot...Error is %d", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);
      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Success to get the system snap shot Details.TotalCards:%d", mCardDetails.count);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);

      for(UINT32 i = 0 ; i < mCardDetails.count ; i++)
      {
         INIT_ACU_STRUCT(&mOpenCardDet);
         strncpy(mOpenCardDet.serial_no, mCardDetails.serial_no[i], ACU_MAX_SERIAL - 1);

         lResult = acu_open_card(&mOpenCardDet);
         if (lResult)
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "Failed to open the card for SerialNo:%s...Error is %d",
                  mCardDetails.serial_no[i],lResult);

            T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
            gLog.GenerateLog(GSYS04, lLogText);//logCode
            return false;
         }
         else
         {
            mCardId[i] = mOpenCardDet.card_id;
            snprintf(lLogText, MAX_LOG_TEXT_LEN,
                  "Success to open the card SerialNum:%s,CardId:%d",
                  mCardDetails.serial_no[i],mOpenCardDet.card_id);

            T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
            gLog.GenerateLog(GSYS04, lLogText);//logCode
         }

         INIT_ACU_STRUCT( &lCardInfo );
         lCardInfo.card_id = mOpenCardDet.card_id;

         lResult = acu_get_card_info( &lCardInfo );
         if(lResult == 0 )
         {
            INIT_ACU_STRUCT( &lOpenSwitch);
            lOpenSwitch.card_id = mOpenCardDet.card_id;

            lResult = acu_open_switch( &lOpenSwitch );
            if( lResult != 0 )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error: Unable to open switch res <%d>", lResult);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

               return lResult;
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Success to open the switch res <%d>", lResult);
               T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
            }

            if( lCardInfo.resources_available & ACU_RESOURCE_SPEECH )
            {
               INIT_ACU_STRUCT( &lOpenProsody );
               lOpenProsody.card_id = mOpenCardDet.card_id;

               lResult = acu_open_prosody( &lOpenProsody );

               if( lResult != 0 )
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error: Unable to open speech res <%d>", lResult);
                  T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

                  return lResult;
               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Success to open the speech res <%d>", lResult);
                  T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
               }

               INIT_ACU_SM_STRUCT( &lSmCardInfo );
               lSmCardInfo.card = mOpenCardDet.card_id;

               lResult = sm_get_card_info( &lSmCardInfo );

               if( lResult != 0 )
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "Error: Unable to get speech card information <%d>",
                        lResult);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                  return lResult;
               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "Successfully obtained the speech card information CardId:%d Module Cnt:%d",
                        lSmCardInfo.card, lSmCardInfo.module_count);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
               }

               lModules = lSmCardInfo.module_count;
               memset(&mOpenModule, 0, sizeof(mOpenModule) * lModules);

               for(lModuleIndx = 0; lModuleIndx < lModules; lModuleIndx++ )
               {
                  SM_OPEN_MODULE_PARMS  lOpenModule;
                  INIT_ACU_SM_STRUCT( &lOpenModule );

                  lOpenModule.module_ix = lModuleIndx;
                  lOpenModule.card_id   = mOpenCardDet.card_id;

                  lResult = sm_open_module ( &lOpenModule );
                  if( lResult != 0 )
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "Error:Unable to Open Prosody Module <%d>", lResult);
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

                     return lResult;
                  }
                  else
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "Success to Open Prosody Module, Index:%d ModuleId:%d", 
                           lOpenModule.module_ix, lOpenModule.module_id);
                     T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  }

                  mOpenModule[lModuleIndx].module_ix = lOpenModule.module_ix;
                  mOpenModule[lModuleIndx].module_id = lOpenModule.module_id;
                  mOpenModule[lModuleIndx].card_id = lOpenModule.card_id;
               }

               int lModIndx = (mCmdStream % 32);

               if(lModIndx == 0 || lModIndx == 1)
                  lModuleIndx = 0;
               else if(lModIndx == 2 || lModIndx == 3)
                  lModuleIndx = 1;
               else if(lModIndx == 4 || lModIndx == 5)
                  lModuleIndx = 2;
               else if(lModIndx == 6 || lModIndx == 7)
                  lModuleIndx = 3;
               else
                  return -1;

               INIT_ACU_SM_STRUCT( &lModuleInfo );
               lModuleInfo.module = mOpenModule[lModuleIndx].module_id;

               lResult = sm_get_module_info( &lModuleInfo );
               if( lResult != 0 )
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "Error: Failed to obtain module <%d>", lResult );
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

                  return lResult;
               }
               else
               {
                  memset(&lMediaPort, 0, sizeof(lMediaPort));

                  if(lModIndx == 0 || lModIndx == 2 || lModIndx == 4 || lModIndx == 6)
                     lMediaPort.st = lModuleInfo.min_stream;
                  else
                     lMediaPort.st = lModuleInfo.min_stream + 1;

                  lMediaPort.ts = mCmdTimeSlot;
                  lMediaPort.moduleIndx = lModuleIndx;

                  snprintf(lLogText, MAX_LOG_TEXT_LEN,
                        "Success to Obtain module Information ModuleId:%d Card ModuleIndx:%d"
                        " MinStream:%d TotalStream:%d\n", lModuleInfo.module, lMediaPort.moduleIndx,
                        lModuleInfo.min_stream, lMediaPort.st);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
               }

               // Create Event
               lResult = smd_ev_create(&gEvent, kSMNullChannelId, 
                     kSMEventTypeWriteData, kSMAnyChannelEvent);
               if( lResult != 0 )
               {
                  snprintf(lLogText,MAX_LOG_TEXT_LEN,
                        "Error: Could not create Prosody Event <%d>", lResult);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                  return lResult;
               }

               snprintf(lLogText,MAX_LOG_TEXT_LEN,
                     "Successfully created Prosody Event Channel:%d EvtType:%d"
                     " IssueEvt:%d EventFd:%d EventMode:%d", kSMNullChannelId,
                     kSMEventTypeWriteData, kSMAnyChannelEvent, gEvent.fd, gEvent.mode);
               T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

               int     lThreadid;
               void*   (*lpResponseThread)(void *);

               lpResponseThread = PlayThread;
               lThreadid = pthread_create
                  (&lResponseThread, NULLPTR, lpResponseThread, (void *) (&gRcp));

               if(0 != lThreadid)
               {
                  snprintf( lLogText, MAX_LOG_TEXT_LEN, "Error: <%d> Could not create"
                        "resource thread for Prosody Channel", lThreadid );
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                  return -1;
               }

               for(;;)
               {
                  if(mNumOfChann)
                  {
                     lResult = CreateCChan(lMediaPort);
                     if (lResult != 0) 
                        return lResult;

                     mNumOfChann--;
                  }
               }
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, " Error: Unable to obtain card info <%d>", lResult);
               TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
               return lResult;
            }
         }
      }
   }

   return true;
}

