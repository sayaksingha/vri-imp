static const char* id="@(#) $Id: MediaDtmfDetection.cc,v 1.1.4.3 2016/04/12 21:59:31 shijuc Exp $";
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
#include "MediaHandler.h"

extern MediaHandler gMediaHandlerObj;

//----------------------------------------------------------------------
// METHOD      : prosody_resource_thread
// DESCRIPTION : Start Channel Thread
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *prosody_resource_thread (void *lPtr)
{
   int        lResult = 0;
   tSMEventId lEvent;
   TEXT       lLogText[MAX_LOG_TEXT_LEN+1] = "";
   
   unsigned   lGruntlatency  = 1000; 
   unsigned   lGrunt  = ~0;  
   unsigned   lMap      = kSMDTMFToneSetDigitMapping; 
   unsigned   lToneMode = kSMToneDetectionNoMinDuration; 

   if(false == gMediaHandlerObj.ConnectToProsody (&gMediaHandlerObj.mSwitchChannelParms))
   {
      printf("Error: Unable to connect Call to Prosody\n");

      if(false == gMediaHandlerObj.ReleaseChannel())
      {
         printf("Failed to Release an allocated channel\n");
      }
      
      return NULLPTR;
   }

   INIT_ACU_SM_STRUCT( &gMediaHandlerObj.mListenParms );

   //if (lGrunt != ~0U) 
   {
      gMediaHandlerObj.mListenParms.enable_grunt_detection = 0; 
      gMediaHandlerObj.mListenParms.grunt_latency = lGruntlatency; 
   }
   
   gMediaHandlerObj.mListenParms.channel             = gMediaHandlerObj.mSwitchChannelParms.channel;
   gMediaHandlerObj.mListenParms.tone_detection_mode = kSMToneDetectionNoMinDuration; 
   gMediaHandlerObj.mListenParms.map_tones_to_digits = kSMDTMFToneSetDigitMapping; 

   lResult = sm_listen_for( &gMediaHandlerObj.mListenParms );
   if( lResult != 0 )
   {
      printf("Error: Could not set-up detection parameters, Terminating <%d>", lResult );
      return NULLPTR;
   }
   
   lResult = smd_ev_create(&lEvent, gMediaHandlerObj.mSwitchChannelParms.channel,
        kSMEventTypeRecog, kSMChannelSpecificEvent );

   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not create Prosody Event <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return NULLPTR;
   }
   snprintf(lLogText,MAX_LOG_TEXT_LEN,
         "Successfully created Prosody Event EventId:%ld Channel:%d EvtType:%d"
         "IssueEvt:%d Mode:%lu fd:%lu", lEvent, gMediaHandlerObj.mSwitchChannelParms.channel,
         kSMEventTypeWriteData, kSMChannelSpecificEvent, lEvent.fd, lEvent.mode);
   T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

   gMediaHandlerObj.mChannelSetEvtParms.channel      = gMediaHandlerObj.mSwitchChannelParms.channel;
   gMediaHandlerObj.mChannelSetEvtParms.event        = lEvent;
   gMediaHandlerObj.mChannelSetEvtParms.event_type   = kSMEventTypeRecog;
   gMediaHandlerObj.mChannelSetEvtParms.issue_events = kSMChannelSpecificEvent;

   lResult = sm_channel_set_event( &gMediaHandlerObj.mChannelSetEvtParms );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      exit(0);
   }
   else
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Successfully associated channel:%d and event:%d",
            gMediaHandlerObj.mChannelSetEvtParms.channel, gMediaHandlerObj.mChannelSetEvtParms.event);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      lResult = gMediaHandlerObj.DetectLoop(gMediaHandlerObj.mSwitchChannelParms.channel, 
            gMediaHandlerObj.mSwitchChannelParms.st, gMediaHandlerObj.mSwitchChannelParms.ts, 
            lEvent);
   }

   if(lResult == true)
      smd_ev_free(gMediaHandlerObj.mChannelSetEvtParms.event);

   if(false == gMediaHandlerObj.DisAssociateEvent(lEvent))
   {
      printf("Failed to Disassociate an event from Channel\n");
      return NULLPTR;
   }

   if(false == gMediaHandlerObj.DisConnectFromProsody
         (&gMediaHandlerObj.mSwitchChannelParms))
      printf("Failed to Disconnect Call from Prosody\n");

   if(false == gMediaHandlerObj.ReleaseChannel())
      printf("Failed to Release an allocated channel\n");

   pthread_exit(NULLPTR);
   return NULLPTR;
}

//-------------------------------------------------------------
// METHOD        : DetectLoop
// DESCRIPTION   : 
// PARAMETER     : NONE
// RETURN        : BOOLEAN Value
//-------------------------------------------------------------
BOOLEAN MediaHandler::DetectLoop(tSMChannelId &chan, tSM_INT &stream, tSM_INT &slot, tSMEventId &event)
{
   bool loopVar = 1;

   for(;;) 
   {
      int res;
      struct pollfd pfd;
      pfd.fd = event.fd;
      pfd.events = event.mode;
      res = poll(&pfd, 1, -1);
      if (res != 1) 
      {
         if (res) {
            perror("poll() error");
            return 1;
         }
      } 
      else 
      {
         if (pfd.revents) 
         {
            fprintf(stderr, "%d.%d: %d  %d\n", stream, slot, pfd.revents, pfd.events);
         }
         else
         {
            printf("EROOR__________________\n");
            exit(0);
         }
         if (HandleRecog(stream, slot, chan)) 
            return 1;
      }
   }
}

//-------------------------------------------------------------
// METHOD        : HandleRecog
// DESCRIPTION   : 
// PARAMETER     : NONE
// RETURN        : BOOLEAN Value
//-------------------------------------------------------------
BOOLEAN MediaHandler::HandleRecog(int &stream, int &slot, tSMChannelId &chan)
{
   for(;;)
   {
      const char *type;
      struct      tm* ltmStruct;
      char        lFileNameTime[100];
      struct      timeval    ltimeval;
      int         lResult = 0;

      SM_RECOGNISED_PARMS  rp;
      mRecogParms.channel = chan;

      lResult = sm_get_recognised(&mRecogParms);
      if ( lResult != 0 )
      {
         printf("Error: Could not collect digit <%d>", lResult);
         return false;
      }

      gettimeofday(&ltimeval, NULL);
      ltmStruct = localtime(&(ltimeval.tv_sec));
      sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%06d",
            ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
            ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);

      switch (mRecogParms.type) 
      {
         case kSMRecognisedNothing: 
            type = 0; 
            break;
         case kSMRecognisedTrainingDigit: 
            type = "TrainingDigit"; 
            break;
         case kSMRecognisedDigit:
            {
               type = 0;
               printf("\33[32mDigit: %d [%c]\33[0m %d\n", 
                     mRecogParms.param0, mRecogParms.param0, mRecogParms.param1);
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
            printf("Error: %d.%d: sm_get_recognised produced event %d\n", 
                  stream, slot, mRecogParms.type);
            return 1;
      }

      if (!type) 
         break;

      if(mRecogParms.type != kSMRecognisedDigit)
         printf("%s: \33[32m%d %d\33[0m\n", type, mRecogParms.param0, mRecogParms.param1);
   }

   return false;
}

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

//----------------------------------------------------------------------
// METHOD      : ConnectToProsody
// DESCRIPTION : Make a connection between a Prosody channel and a call
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MediaHandler::ConnectToProsody (SM_SWITCH_CHANNEL_PARMS *lPtr)
{
   int         lResult = 0;
   TEXT        lLogText[MAX_LOG_TEXT_LEN+1] = "";

   INIT_ACU_SM_STRUCT( &mChannelInfoParms );
   SM_SWITCH_CHANNEL_PARMS *switch_params = (SM_SWITCH_CHANNEL_PARMS *) lPtr;

   mChannelInfoParms.channel = switch_params->channel;
   lResult = sm_channel_info( &mChannelInfoParms );
   if ( lResult != 0 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error: Could not get channel info <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return false;
   }

   if ( mChannelInfoParms.ist != -1 )
   {
      if ( gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id )
      {
         INIT_ACU_STRUCT( &mConnectParms );

         mConnectParms.ist  = gMediaHandlerObj.mCmdStream; 
         mConnectParms.its  = gMediaHandlerObj.mCmdTimeSlot;
         mConnectParms.mode = CONNECT_MODE;
         mConnectParms.ost  = mChannelInfoParms.ist;
         mConnectParms.ots  = mChannelInfoParms.its;

         printf("-----------------------------------------------------------------------------\n");
         printf("Connect Params: mConnectParms.ist:%d\t  mConnectParms.its:%d\t mConnectParms.mode:%d\t"
               "mConnectParms.ost:%d\t mConnectParms.ots:%d\t\n"
               "gMediaHandlerObj.mOpenModule.card_id:%d\t gMediaHandlerObj.mOpenCardDet.card_id:%d\t\n", 
               mConnectParms.ist, mConnectParms.its, mConnectParms.mode, mConnectParms.ost, 
               mConnectParms.ots,gMediaHandlerObj.mOpenModule.card_id, 
               gMediaHandlerObj.mOpenCardDet.card_id);
         printf("-----------------------------------------------------------------------------\n");

         lResult = sw_set_output(gMediaHandlerObj.mOpenCardDet.card_id, &mConnectParms );
         if ( lResult != 0)
         {
            printf ("Error could not make switch connection <%d>", lResult);
            return false;
         }
      }
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD      : DisConnectFromProsody
// DESCRIPTION : Removes a connection between a Prosody channel and a call
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MediaHandler::DisConnectFromProsody(void *lPtr)
{
   int         lResult = 0;
   TEXT        lLogText[MAX_LOG_TEXT_LEN+1] = "";

   INIT_ACU_SM_STRUCT( &mChannelInfoParms );
   SM_SWITCH_CHANNEL_PARMS *switch_params = (SM_SWITCH_CHANNEL_PARMS *) lPtr;

   mChannelInfoParms.channel = switch_params->channel;
   lResult = sm_channel_info( &mChannelInfoParms );
   if ( lResult != 0 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error: Could not get channel info <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return false;
   }

   if ( mChannelInfoParms.ist != -1 )
   {
      if ( mOpenModule.card_id == mOpenCardDet.card_id )
      {
         INIT_ACU_STRUCT( &mDisConnectParms );

         mDisConnectParms.ost  = mChannelInfoParms.ist;
         mDisConnectParms.ots  = mChannelInfoParms.its;
         mDisConnectParms.mode = DISABLE_MODE;

         lResult = sw_set_output(mOpenCardDet.card_id, &mDisConnectParms);
         if ( lResult != 0)
         {
            printf ("Error could not break connection from call to Prosody <%d>", lResult);
            return false;
         }
      }
   }

   if ( mChannelInfoParms.ost != -1 )
   {
      if ( mOpenModule.card_id == mOpenCardDet.card_id )
      {
         INIT_ACU_STRUCT( &mDisConnectParms );

         mDisConnectParms.ost  = mChannelInfoParms.ist;
         mDisConnectParms.ots  = mChannelInfoParms.its;
         mDisConnectParms.mode = DISABLE_MODE;

         lResult = sw_set_output(mOpenCardDet.card_id, &mDisConnectParms);
         if ( lResult != 0 )
         {
            printf("Error could not break connection from Prosody to call <%d>", lResult);
            return false;
         }
      }
   }
   return true;
}

//-------------------------------------------------------------
// METHOD        : DisAssociateEvent
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        :
//-------------------------------------------------------------
BOOLEAN MediaHandler::DisAssociateEvent(tSMEventId &lEvent)
{
   int      lResult = 0;
   TEXT     lLogText[MAX_LOG_TEXT_LEN+1] = "";

   mChannelSetEvtParms.channel      = mSwitchChannelParms.channel;
   mChannelSetEvtParms.event        = lEvent;
   mChannelSetEvtParms.event_type   = kSMEventTypeReadData;
   mChannelSetEvtParms.issue_events = kSMChannelNoEvent;

   lResult = sm_channel_set_event( &mChannelSetEvtParms );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }
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
BOOLEAN MediaHandler::ReleaseChannel()
{
   int lResult = 0;
   lResult = sm_channel_release( mChannelInfoParms.channel );
   if(lResult != 0 )
   {
      printf("Error: Could not free Prosody channel <%d>", lResult);
      return false;
   }
   mChannelInfoParms.channel = kSMNullChannelId;

   return true;
}

//-------------------------------------------------------------
// METHOD        : InitializeCard
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : NONE
//-------------------------------------------------------------
BOOLEAN MediaHandler::InitializeCard()
{
   TEXT       lLogText[MAX_LOG_TEXT_LEN+1] = "";
   int        lResult;
   int        lModuleIndx;
   pthread_t     lChannelThread;

   //mChannels = CHANNELS_PER_MODULE;
   mChannels = 1;
   
   INIT_ACU_STRUCT(&mCardDetails);

   lResult = acu_get_system_snapshot(&mCardDetails);

   if (lResult)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Failed to get the System SnapShot...Error is %d",lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);//logCode
      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Success to get the system snap shot Details.TotalCards:%d",mCardDetails.count);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      gLog.GenerateLog(GSYS04, lLogText);//logCode

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

         INIT_ACU_STRUCT( &mCardInfo );
         mCardInfo.card_id = mOpenCardDet.card_id;

         lResult = acu_get_card_info( &mCardInfo );

         if(lResult == 0 )
         {
            INIT_ACU_STRUCT( &mOpenSwitch);
            mOpenSwitch.card_id = mOpenCardDet.card_id;
            lResult = acu_open_switch( &mOpenSwitch );
            if( lResult != 0 )
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error: Unable to open switch res <%d>", lResult);
               T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

               return lResult;
            }
            else
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "Success to open the switch res <%d>", lResult);
               T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
            }

            if( mCardInfo.resources_available & ACU_RESOURCE_SPEECH )
            {
               INIT_ACU_STRUCT( &mOpenProsody );

               mOpenProsody.card_id = mOpenCardDet.card_id;

               lResult = acu_open_prosody( &mOpenProsody );

               if( lResult != 0 )
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error: Unable to open speech res <%d>", lResult);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

                  return lResult;
               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, "Success to open the speech res <%d>", lResult);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
               }

               INIT_ACU_SM_STRUCT( &mSmCardInfo );
               mSmCardInfo.card = mOpenCardDet.card_id;

               lResult = sm_get_card_info( &mSmCardInfo );

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
                        mSmCardInfo.card, mSmCardInfo.module_count);
                  T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
               }

               mModules = mSmCardInfo.module_count;
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

                  INIT_ACU_SM_STRUCT( &mModuleInfo );
                  mModuleInfo.module = mOpenModule.module_id;

                  lResult = sm_get_module_info( &mModuleInfo );

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
                           mModuleInfo.module, mOpenModule.module_ix );
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                  }

                  mStream = (mOpenModule.module_ix * 2) + mModuleInfo.min_stream;

                  mStream+= 1;  // for recording on next stream.
                  mTimeSlot = mCmdTimeSlot;

                  T(gTrace, printf("%s: Open Module IX:%d Open Module Min Stream:%d Total Stream:%d\n",
                           gProcessName, mOpenModule.module_ix, mModuleInfo.min_stream, mStream););

                  T(gTrace, printf("%s: Number of Channels per Module:%d \n",
                           gProcessName,CHANNELS_PER_MODULE););
                  T(gTrace, printf("%s: Number of Timeslots per Stream:%d \n",
                           gProcessName,TIMESLOTS_PER_STREAM););

                  for ( mTimeSlot = 0; mTimeSlot < mChannels; mTimeSlot++ )
                  {
                     if( mTimeSlot >= TIMESLOTS_PER_STREAM )
                     {
                        mStream += 1;                      
                        mChannels -= TIMESLOTS_PER_STREAM; 
                        mTimeSlot = 0;                     
                     }
                     INIT_ACU_SM_STRUCT( &mChannelAllocParms );
                     INIT_ACU_SM_STRUCT( &mSwitchChannelParms );

                     mChannelAllocParms.module      = mOpenModule.module_id;
                     mChannelAllocParms.type        = kSMChannelTypeInput;

                     lResult = sm_channel_alloc_placed( &mChannelAllocParms );
                     if( lResult != 0 )
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "Error: Unable to allocate channel <%d>",lResult);
                        T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                        return lResult;
                     }
                     else
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "Successfully allocated channel:%lu Stream:%d TimeSlot:%d",
                              mChannelAllocParms.channel,  mStream, mTimeSlot);
                        T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                     }

                     mSwitchChannelParms.channel     = mChannelAllocParms.channel;
                     mSwitchChannelParms.st          = mStream;
                     mSwitchChannelParms.ts          = mTimeSlot;

                     if( COMPANDING == COMPANDING_ALAW )
                     {
                        mSwitchChannelParms.type  = kSMTimeslotTypeALaw;
                     }
                     else if( COMPANDING == COMPANDING_MULAW )
                     {
                        mSwitchChannelParms.type  = kSMTimeslotTypeMuLaw;
                     }
                     else
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN, "Error: Unable to set companding type");
                        T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                     }

                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "Successfully set Companding type:%d ChannelType:%d",
                           mSwitchChannelParms.type, mChannelAllocParms.type);

                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

                     if( mChannelAllocParms.type != kSMChannelTypeOutput )
                     {
                        lResult = sm_switch_channel_input( &mSwitchChannelParms );
                        if( lResult != 0 )
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                 "Error: Could not setup channel's input switching <%d>", lResult );
                           T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                           return lResult;
                        }
                     }

                     int           lThreadid;
                     void*   (*lpChannelThread)(void *);

                     lpChannelThread = prosody_resource_thread;
                     lThreadid = pthread_create
                        (&lChannelThread, NULLPTR, lpChannelThread, (void *) (&mSwitchChannelParms));

                     if(0 != lThreadid)
                     {
                        snprintf( lLogText, MAX_LOG_TEXT_LEN, "Error: <%d> Could not create"
                              "resource thread for Prosody Channel", lThreadid );
                        T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                        return -1;
                     }
                     else
                        pthread_join(lChannelThread, NULL);
                  }
               }

            }
         }
         else
         {
            snprintf(lLogText,MAX_LOG_TEXT_LEN," Error: Unable to obtain card info <%d>", lResult);
            return lResult;
         }
         return true;
      }
   }
}



