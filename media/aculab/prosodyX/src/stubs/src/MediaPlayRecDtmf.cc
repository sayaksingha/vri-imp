static const char* id="@(#) $Id: MediaPlayRecDtmf.cc,v 1.1.4.3 2016/04/12 21:59:31 shijuc Exp $";
//-----------------------------------------------------------------------------
// NAME
//   MediaRecDtmf.cc -- MediaHandler Main process
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
#include "MediaPlayRecDtmf.h"

extern MediaHandler gMediaHandlerObj;

tSMEventId     gRecEvent;
tSMEventId     gDtmfEvent;
tSMEventId     gPlayEvent;
runningChannel gRcp[256*TIMESLOTS_PER_STREAM];

//-------------------------------------------------------------
// METHOD        : MediaHandler
// DESCRIPTION   : Constructor
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
MediaHandler::MediaHandler()
{
   mCmdStream = 0;
   mCmdTimeSlot = 0;
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

   memset(&lDcp, 0, sizeof(lDcp));

   memset(&lSe, 0, sizeof(lSe));
   lSe.channel      = gRcp[index].channel;
   lSe.event        = gRecEvent;
   lSe.event_type   = kSMEventTypeReadData;
   lSe.issue_events = kSMChannelNoEvent;

   lResult = sm_channel_set_event( &lSe );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event REC <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }

   memset(&lSe, 0, sizeof(lSe));
   lSe.channel      = gRcp[index].channel;
   lSe.event        = gDtmfEvent;
   lSe.event_type   = kSMEventTypeRecog;
   lSe.issue_events = kSMChannelNoEvent;

   lResult = sm_channel_set_event( &lSe );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event DTMF <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }

   fclose(gRcp[index].rp.fd);

   if ( gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id )
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
// METHOD      : RecordThread
// DESCRIPTION : Check Channel Status Thread
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *RecordThread (void *lPtr)
{
   int      lResult = 0;
   struct   pollfd pfd[3];

   memset(&pfd, 0, sizeof(pfd));

   pfd[0].fd = gPlayEvent.fd;
   pfd[0].events = gPlayEvent.mode;
   pfd[1].fd = gDtmfEvent.fd;
   pfd[1].events = gDtmfEvent.mode;
   pfd[2].fd = gRecEvent.fd;
   pfd[2].events = gRecEvent.mode;

   T(gTrace, printf("%s: Play[Fd:%d Mode:%d] Dtmf[Fd:%d Mode:%d] Rec[Fd:%d Mode:%d]\n", gProcessName, 
            pfd[0].fd, pfd[0].events, pfd[1].fd, pfd[1].events, pfd[2].fd, pfd[2].events););

   for(;;)
   {
      lResult = poll(pfd, 3, -1);
      if (lResult < 0) 
      {
         TERR (gTrace, printf("%s: poll()failed Cannot wait for channels\n", 
                  gProcessName););
         pthread_exit(NULLPTR);
         return NULLPTR;
      }

      if (pfd[2].revents) 
      {
         int lIndex = 0;
         sm_record_status_parms lStatusParms;
         memset(&lStatusParms, 0, sizeof(lStatusParms));

         lStatusParms.channel = kSMNullChannelId;
         lResult = sm_record_status (&lStatusParms);
         if (lResult == ERR_SM_NO_SUCH_CHANNEL)  
            continue; 

         cout<<lResult; 
         if (lResult) 
         {
            TERR (gTrace, printf("%s: REC Cannot check channel status\n", 
                     gProcessName););
         }

         lIndex = sm_get_channel_ix(lStatusParms.channel);
         if(lIndex < 0)
            printf("%s: Index:%d\n", gProcessName, lIndex);

         T(gTrace, printf("%s: ChannelId:%d Status:%d Index:%d\n", gProcessName,
                  lStatusParms.channel, lStatusParms.status, lIndex););

         lResult = sm_record_file_progress_istatus(&gRcp[lIndex].rp, lStatusParms.status);
         if(lResult != ERR_SM_PENDING)
         {
            T(gTrace, printf("%s: Record Completed Successfully ChannelId:%d[\33[33m%d:%d\33[0m] "
                     "status:%d\n", gProcessName, gRcp[lIndex].channel, gRcp[lIndex].outPort.st, 
                     gRcp[lIndex].outPort.ts, lResult););

            gMediaHandlerObj.DisAssociateEvent(lIndex);
         }
      } 

      if (pfd[0].revents) 
      {
         int lIndex = 0;
         sm_replay_status_parms lStatusParms;
         memset(&lStatusParms, 0, sizeof(lStatusParms));

         lStatusParms.channel = kSMNullChannelId;
         lResult = sm_replay_status (&lStatusParms);
         if (lResult == ERR_SM_NO_SUCH_CHANNEL)  
            continue; 

         cout<<lResult; 
         if (lResult) 
         {
            TERR (gTrace, printf("%s: Cannot check PLAY channel status\n", 
                     gProcessName););
         }

         lIndex = sm_get_channel_ix(lStatusParms.channel);
         if(lIndex < 0)
            printf("%s: Index:%d\n", gProcessName, lIndex);

         T(gTrace, printf("%s: PLAY ChannelId:%d Status:%d Index:%d\n", gProcessName,
                  lStatusParms.channel, lStatusParms.status, lIndex););

         lResult = sm_replay_file_progress_istatus(&gRcp[lIndex].fp, lStatusParms.status);
         if(lResult != ERR_SM_PENDING)
         {
            T(gTrace, printf("%s: Play Completed Successfully ChannelId:%d[\33[33m%d:%d\33[0m] "
                     "status:%d\n", gProcessName, gRcp[lIndex].channel, gRcp[lIndex].outPort.st, 
                     gRcp[lIndex].outPort.ts, lResult););

            gMediaHandlerObj.DisAssociateEvent(lIndex);
         }
      } 

      if(pfd[1].revents)
      {
         int lIndex = 0;
         const char *type;
         SM_RECOGNISED_PARMS  rp;
         memset(&rp, 0, sizeof(rp));
         TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

         rp.channel = kSMNullChannelId;
         lResult = sm_get_recognised(&rp);

         if (lResult == ERR_SM_NO_SUCH_CHANNEL)
            continue;

         T(gTrace, printf("%s: DTMF ChannelId:%d Type:%d\n",
                  gProcessName, rp.channel, rp.type););

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
                     printf("%s: Index:%d\n", gProcessName, lIndex);

                  ci.channel = rp.channel;
                  lResult = sm_channel_info (&ci);
                  if ( lResult != 0 )
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "Error: Could not get channel info <%d>", lResult);
                     T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  }
                  T(gTrace, printf("\33[32m Digit: %d [%c] %d (%d:%d)\33[0m\n",
                           rp.param0, rp.param0, rp.param1, ci.ist, ci.its););
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
               printf("Error: sm_get_recognised produced event %d\n", rp.type);
               break;
         }

         if (!type)
            break;

         if(rp.type != kSMRecognisedDigit)
            printf("%s: \33[32m%d %d\33[0m\n", type, rp.param0, rp.param1);
         gMediaHandlerObj.DisAssociateEvent(lIndex);
      }

   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}

//-------------------------------------------------------------
// METHOD        : CreatePRDChannel
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : INT
//-------------------------------------------------------------
BOOLEAN MediaHandler::CreatePRDChannel(Channel & rMediaPort)
{
   int lIndex = 0;
   int lResult = 0;

   struct      tm* ltmStruct;
   char        lFileNameTime[100];
   struct      timeval    ltimeval;

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT lFileName[MAX_LOG_TEXT_LEN + 1] ="";

   SM_SWITCH_CHANNEL_PARMS       sw;
   SM_CHANNEL_SET_EVENT_PARMS    se;
   SM_CHANNEL_ALLOC_PLACED_PARMS ac;
   SM_CHANNEL_INFO_PARMS         ci;
   OUTPUT_PARMS                  cp; 

   memset(&ac, 0, sizeof(ac));
   ac.type   = kSMChannelTypeFullDuplex;
   ac.module = mOpenModule.module_id;
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
   memset(&lFileName, 0, sizeof(lFileName));

   gRcp[lIndex].channel = ac.channel;
   gRcp[lIndex].outPort.st = mCmdStream;
   gRcp[lIndex].outPort.ts = rMediaPort.ts;
   gRcp[lIndex].outPort.type = (kSMTimeslotType) rMediaPort.type;

   gRcp[lIndex].lp.grunt_latency = 1000;
   gRcp[lIndex].lp.enable_grunt_detection = 0;
   gRcp[lIndex].lp.tone_detection_mode = kSMToneDetectionNoMinDuration;
   gRcp[lIndex].lp.map_tones_to_digits = kSMDTMFToneSetDigitMapping;
   gRcp[lIndex].lp.channel = ac.channel;
   gRcp[lIndex].lp.enable_cptone_recognition = 0;
   
   gRcp[lIndex].fp.replay_parms.channel = ac.channel;
   gRcp[lIndex].fp.replay_parms.type    = kSMDataFormat8KHzALawPCM;

   gRcp[lIndex].rp.record_parms.channel = ac.channel;
   gRcp[lIndex].rp.record_parms.type    = kSMDataFormat8KHzALawPCM;
   gRcp[lIndex].rp.record_parms.max_silence = 5000;
   //gRcp[lIndex].rp.record_parms.max_elapsed_time = 40000;
   snprintf(lFileName, MAX_LOG_TEXT_LEN, "record_%d_%d.wav",
         mCmdStream, rMediaPort.ts);

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

   lResult = sm_listen_for(&gRcp[lIndex].lp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_listen_for() failed <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return lResult;
   }

   memset(&sw, 0, sizeof(sw));
   sw.st = rMediaPort.st;
   sw.ts = rMediaPort.ts;
   sw.type = (kSMTimeslotType) rMediaPort.type;
   sw.channel = ac.channel;
   lResult = sm_switch_channel_input(&sw);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_switch_channel_input() DTMF failed <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

      this->ReleaseChannel (sw.channel);

      return lResult;
   }

   memset(&sw, 0, sizeof(sw));
   sw.st = rMediaPort.st;
   sw.ts = rMediaPort.ts;
   sw.type = (kSMTimeslotType) rMediaPort.type;
   sw.channel = ac.channel;
   lResult = sm_switch_channel_input(&sw);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_switch_channel_input() REC failed <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

      this->ReleaseChannel (sw.channel);

      return lResult;
   }

   memset(&sw, 0, sizeof(sw));
   sw.st = rMediaPort.st;
   sw.ts = gRcp[lIndex].outPort.ts;
   sw.type = (kSMTimeslotType) gRcp[lIndex].outPort.type;
   sw.channel = gRcp[lIndex].fp.replay_parms.channel;
   lResult = sm_switch_channel_output(&sw);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_switch_channel_output() failed PLAY <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return lResult;
   }

   memset(&se, 0, sizeof(se));
   se.event = gPlayEvent;
   se.issue_events = kSMAnyChannelEvent;
   se.event_type = kSMEventTypeWriteData;
   se.channel = gRcp[lIndex].channel;
   lResult = sm_channel_set_event(&se);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_channel_set_event() failed PLAY <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   memset(&se, 0, sizeof(se));
   se.event = gDtmfEvent;
   se.issue_events = kSMAnyChannelEvent;
   se.event_type = kSMEventTypeRecog;
   se.channel = gRcp[lIndex].channel;
   lResult = sm_channel_set_event(&se);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_channel_set_event() failed DTMF <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   memset(&se, 0, sizeof(se));
   se.event = gRecEvent;
   se.issue_events = kSMAnyChannelEvent;
   se.event_type = kSMEventTypeReadData;
   se.channel = gRcp[lIndex].channel;
   lResult = sm_channel_set_event(&se);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_channel_set_event() failed REC <%d>", lResult);
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

   if ( gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id )
   {
      cp.size = sizeof(OUTPUT_PARMS);
      cp.ist = ci.ost;
      cp.its = ci.ots;
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
         printf ("Error could not make switch connection PLAY <%d>", lResult);
         return lResult;
      }
   }

   if (gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id)
   {
      memset(&cp, 0, sizeof(cp));
      cp.size = sizeof(cp);
      cp.ist = mCmdStream;
      cp.its = rMediaPort.ts;
      cp.mode = CONNECT_MODE;
      cp.ost = sw.st;
      cp.ots = sw.ts;

      printf("-----------------------------------------------------------------------------\n");
      printf("Connect Params: ist:%d its:%d mode:%d ost:%d ots:%d card:%d \n", 
            cp.ist, cp.its, cp.mode, cp.ost, cp.ots, gMediaHandlerObj.mOpenCardDet.card_id);
      printf("-----------------------------------------------------------------------------\n");

      lResult = sw_set_output(gMediaHandlerObj.mOpenCardDet.card_id, &cp);
      if ( lResult != 0)
      {
         printf ("Error could not make switch connection REC/DTMF <%d>", lResult);
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

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "Starting Record on %d:%d %d:%d\n",
         cp.ist, cp.its, cp.ost, cp.ots);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

   printf("StartTime:%s \33[32m File Record Started ChannelId:%d\33[0m[Status:%d]\n FileName:%s[fd:%d]\n",
         lFileNameTime, gRcp[lIndex].rp.record_parms.channel, gRcp[lIndex].rp.status, lFileName,
         gRcp[lIndex].rp.fd);
   printf("=========================================================================================\n");

   lResult = sm_record_wav_start (lFileName, &gRcp[lIndex].rp);
   if ( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not initiate wav %s record <%d>", lFileName, lResult );
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return lResult;
   }

   snprintf(lLogText,MAX_LOG_TEXT_LEN,
         "INFO: Success to initiate %s record <%d>", lFileName, lResult );
   T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

   if (! (++rMediaPort.ts & 0x1f)) 
   {
      rMediaPort.st ^= 1;
      rMediaPort.ts -= 32;
   }

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
   INT16      lModules;
   int        lModuleIndx;
   TEXT       lLogText[MAX_LOG_TEXT_LEN+1] = "";

   Channel      lMediaPort;
   pthread_t    lResponseThread;

   SM_CARD_INFO_PARMS            lSmCardInfo;
   SM_MODULE_INFO_PARMS          lModuleInfo;
   ACU_CARD_INFO_PARMS           lCardInfo;
   ACU_OPEN_PROSODY_PARMS        lOpenProsody;
   ACU_OPEN_SWITCH_PARMS         lOpenSwitch;

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
               for( lModuleIndx = 0; lModuleIndx < 1; lModuleIndx++ )
               {
                  mOpenModule.module_ix = lModuleIndx;

                  INIT_ACU_SM_STRUCT( &mOpenModule );

                  mOpenModule.module_ix = lModuleIndx;
                  mOpenModule.card_id   = mOpenCardDet.card_id;

                  lResult = sm_open_module ( &mOpenModule );
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
                           "Success to Open Prosody Module ModuleId:%d", mOpenModule.module_id);
                     T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  }

                  INIT_ACU_SM_STRUCT( &lModuleInfo );
                  lModuleInfo.module = mOpenModule.module_id;

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
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "Success to Obtain module Information ModuleId:%d Card ModuleIndx:%d",
                           lModuleInfo.module, mOpenModule.module_ix );
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                  }

                  lMediaPort.st = (mOpenModule.module_ix * 2) + lModuleInfo.min_stream; 
                  lMediaPort.st += (mCmdStream % 32);
                  lMediaPort.ts = mCmdTimeSlot;

                  T(gTrace, printf("%s: Open Module IX:%d Open Module Min Stream:%d Current Stream:%d\n",
                           gProcessName, mOpenModule.module_ix, lModuleInfo.min_stream, lMediaPort.st););

                  // Create Rec Event
                  lResult = smd_ev_create(&gRecEvent, kSMNullChannelId, 
                        kSMEventTypeReadData, kSMAnyChannelEvent);
                  if( lResult != 0 )
                  {
                     snprintf(lLogText,MAX_LOG_TEXT_LEN,
                           "Error: Could not create Prosody Event REC <%d>", lResult);
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                     return lResult;
                  }

                  // Create Dtmf Event
                  lResult = smd_ev_create(&gDtmfEvent, kSMNullChannelId, 
                        kSMEventTypeRecog, kSMAnyChannelEvent);
                  if( lResult != 0 )
                  {
                     snprintf(lLogText,MAX_LOG_TEXT_LEN,
                           "Error: Could not create Prosody Event DTMF <%d>", lResult);
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                     return lResult;
                  }

                  // Create Play Event
                  lResult = smd_ev_create(&gPlayEvent, kSMNullChannelId, 
                        kSMEventTypeWriteData, kSMAnyChannelEvent);
                  if( lResult != 0 )
                  {
                     snprintf(lLogText,MAX_LOG_TEXT_LEN,
                           "Error: Could not create Prosody Event PLAY <%d>", lResult);
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                     return lResult;
                  }


                  snprintf(lLogText,MAX_LOG_TEXT_LEN,
                        "Successfully created Prosody Event Channel:%d "
                        " IssueEvt:%d Rec[Fd:%d Mode:%d] Dtmf[Fd:%d Mode:%d] Play[Fd:%d Mode:%d]", 
                        kSMNullChannelId, kSMAnyChannelEvent, gRecEvent.fd, gRecEvent.mode,
                        gDtmfEvent.fd, gDtmfEvent.mode, gPlayEvent.fd, gPlayEvent.mode);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

                  for(;;)
                  {
                     if(mNumOfChann)
                     {
                        lResult = CreatePRDChannel(lMediaPort);
                        if (lResult != 0) 
                           return lResult;

                        mNumOfChann--;
                     }
                     else
                     {
                        int     lThreadid;
                        void*   (*lpResponseThread)(void *);

                        lpResponseThread = RecordThread;
                        lThreadid = pthread_create
                           (&lResponseThread, NULLPTR, lpResponseThread, (void *) (&gRcp));

                        if(0 != lThreadid)
                        {
                           snprintf( lLogText, MAX_LOG_TEXT_LEN, "Error: <%d> Could not create"
                                 "resource thread for Prosody Channel", lThreadid );
                           T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                           return -1;
                        }
                        else
                           pthread_join(lResponseThread, NULL);
                     }
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

