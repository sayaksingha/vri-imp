static const char* id="@(#) $Id: MediaAsyncDtmf.cc,v 1.1.4.3 2016/04/12 21:59:29 shijuc Exp $";
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
#include "MediaAsyncDtmfHdlr.h"

extern MediaHandler gMediaHandlerObj;

tSMEventId     gEvent;
dtmfChannel    gDtmf[256*TIMESLOTS_PER_STREAM];

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
   OUTPUT_PARMS               lDcp;
   
   memset(&lSe, 0, sizeof(lSe));
   memset(&lDcp, 0, sizeof(lDcp));

   lSe.channel      = gDtmf[index].chan;
   lSe.event        = gEvent;
   lSe.event_type   = kSMEventTypeRecog;
   lSe.issue_events = kSMChannelNoEvent;

   lResult = sm_channel_set_event( &lSe );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }

   if (gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id)
   {
      lDcp.ost  = gDtmf[index].port.st;
      lDcp.ots  = gDtmf[index].port.ts;
      lDcp.mode = DISABLE_MODE;
      lDcp.size = sizeof(lDcp);

      lResult = sw_set_output(gMediaHandlerObj.mOpenCardDet.card_id, &lDcp);
      if ( lResult != 0)
      {
         printf ("Error could not break connection from call to Prosody <%d>", lResult);
         return false;
      }
   }

   this->ReleaseChannel (gDtmf[index].chan);
   memset(&gDtmf[index], 0, sizeof(dtmfChannel));

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
// METHOD      : DetectThread
// DESCRIPTION : Check Channel Status Thread
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *DetectThread (void *lPtr)
{
   int      lResult = 0;
   struct   pollfd pfd;

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
         for(;;)
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

            T(gTrace, printf("%s: ChannelId:%d Type:%d\n", 
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
                              rp.param0, rp.param0, rp.param1, ci.ist, ci.its, rp.channel););
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
   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}

//-------------------------------------------------------------
// METHOD        : DetectChannel
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : INT
//-------------------------------------------------------------
BOOLEAN MediaHandler::DetectChannel(Channel & rMediaPort)
{
   int lIndex = 0;
   int lResult = 0;

   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   SM_SWITCH_CHANNEL_PARMS       sw;
   SM_CHANNEL_ALLOC_PLACED_PARMS ac;
   SM_CHANNEL_SET_EVENT_PARMS    se;
   SM_CHANNEL_INFO_PARMS         ci;
   OUTPUT_PARMS                  cp;
   Channel lPort;

   lPort = rMediaPort;
   memset(&ac, 0, sizeof(ac));
   ac.type   = kSMChannelTypeInput;
   ac.module = mOpenModule.module_id;
   lResult = sm_channel_alloc_placed(&ac);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error: Unable to allocate channel <%d>", lResult);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
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

   memset(&gDtmf[lIndex], 0, sizeof(lIndex)); 
   gDtmf[lIndex].chan = ac.channel;
   gDtmf[lIndex].port = lPort;
   gDtmf[lIndex].lp.grunt_latency = 1000;
   gDtmf[lIndex].lp.enable_grunt_detection = 0;
   gDtmf[lIndex].lp.tone_detection_mode = kSMToneDetectionNoMinDuration;
   gDtmf[lIndex].lp.map_tones_to_digits = kSMDTMFToneSetDigitMapping;
   gDtmf[lIndex].lp.channel = ac.channel;
   gDtmf[lIndex].lp.enable_cptone_recognition = 0;

   lResult = sm_listen_for(&gDtmf[lIndex].lp);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_listen_for() failed <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return lResult;
   }

   memset(&sw, 0, sizeof(sw));
   sw.st = lPort.st;
   sw.ts = lPort.ts;
   sw.type = (kSMTimeslotType) lPort.type;
   sw.channel = ac.channel;
   lResult = sm_switch_channel_input(&sw);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_switch_channel_input() failed <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

      this->ReleaseChannel (sw.channel);

      return lResult;
   }

   if ( mOpenModule.card_id == mOpenCardDet.card_id )
   {
      memset(&cp, 0, sizeof(cp));
      cp.size = sizeof(cp);
      cp.ist  = mCmdStream;
      cp.its  = rMediaPort.ts;
      cp.mode = CONNECT_MODE;
      cp.ost  = sw.st;
      cp.ots  = sw.ts;

      printf("-----------------------------------------------------------------------------\n");
      printf("Connect Params: ist:%d its:%d mode:%d ost:%d ots:%d CardId:%d\n",
            cp.ist, cp.its, cp.mode, cp.ost, cp.ots, mOpenModule.card_id);
      printf("-----------------------------------------------------------------------------\n");

      lResult = sw_set_output(mOpenCardDet.card_id, &cp);
      if ( lResult != 0)
      {
         printf ("Error could not make switch connection <%d>", lResult);
         return false;
      }
   }

   memset(&se, 0, sizeof(se));
   se.channel = gDtmf[lIndex].chan;
   se.event = gEvent;
   se.issue_events = kSMAnyChannelEvent;
   se.event_type = kSMEventTypeRecog;
   lResult = sm_channel_set_event(&se);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_channel_set_event() failed  <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   if (rMediaPort.ts == 32) 
   {
      rMediaPort.ts = 0;
      rMediaPort.st++;
   }
   else
      rMediaPort.ts++;

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

   memset(&lMediaPort, 0, sizeof(lMediaPort));

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

                  T(gTrace, printf("%s: Open Module IX:%d Open Module Min Stream:%d Total Stream:%d\n",
                           gProcessName, mOpenModule.module_ix, lModuleInfo.min_stream, lMediaPort.st););

                  // Create Event
                  lResult = smd_ev_create(&gEvent, kSMNullChannelId,
                        kSMEventTypeRecog, kSMAnyChannelEvent);
                  if( lResult != 0 )
                  {
                     snprintf(lLogText,MAX_LOG_TEXT_LEN,
                           "Error: Could not create Prosody Event <%d>", lResult);
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                     return lResult;
                  }

                  snprintf(lLogText,MAX_LOG_TEXT_LEN,
                        "\33[32mSuccessfully created Prosody Event EventFd:%d EventMode:%d\33[0m"
                        " EvtType:%d IssueEvt:%d", gEvent.fd, gEvent.mode, 
                        kSMEventTypeRecog, kSMAnyChannelEvent);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

                  for(;;)
                  {
                     if(mNumOfChann)
                     {
                        lResult = DetectChannel(lMediaPort);
                        if (lResult != 0) 
                           return lResult;

                        mNumOfChann--;
                     }
                     else
                     {
                        int     lThreadid;
                        void*   (*lpResponseThread)(void *);

                        lpResponseThread = DetectThread;
                        lThreadid = pthread_create
                           (&lResponseThread, NULLPTR, lpResponseThread, (void *) (&gDtmf));

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

