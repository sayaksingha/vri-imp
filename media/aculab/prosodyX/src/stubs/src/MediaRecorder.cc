static const char* id="@(#) $Id: MediaRecorder.cc,v 1.1.4.3 2016/04/12 21:59:32 shijuc Exp $";
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

void *PromtRecording (void *ptr)
{
   struct      tm* ltmStruct;
   char        lFileNameTime[100];
   struct      timeval    ltimeval;
   int         lResult = 0;

   SM_FILE_RECORD_PARMS   *prompt_parms = (SM_FILE_RECORD_PARMS *)ptr; 

   lResult = sm_record_file_complete( prompt_parms );

   if(lResult != 0 )
   {
      T(gTrace, printf("Error: Could not complete wav record <%d>\n", lResult);)
   }

   gettimeofday(&ltimeval, NULL);
   ltmStruct = localtime(&(ltimeval.tv_sec));
   sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%10d",
         ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
         ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);
   printf("*****************************************************************************\n");
   printf("Record finished Time: %s ChannelId:%d Status:%d Result:<%d>\n",
         lFileNameTime, prompt_parms->record_parms.channel, prompt_parms->status, lResult);
   printf("*****************************************************************************\n");
   
   lResult = sm_record_wav_close( prompt_parms );

   if(lResult != 0 )
   {
      T(gTrace, printf("Error: Could not complete wav record <%d>\n", lResult);)
   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}

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
   char       lFileNameTime[100];
   struct     tm* ltmStruct;
   struct     timeval    ltimeval;
   TEXT       lLogText[MAX_LOG_TEXT_LEN+1] = "";
   TEXT       lFilename[20] = "record.wav";

   if(false == gMediaHandlerObj.ConnectToProsody (&gMediaHandlerObj.mSwitchChannelParms))
   {
      printf("Error: Unable to connect Call to Prosody\n");

      if(false == gMediaHandlerObj.ReleaseChannel())
      {
         printf("Failed to Release an allocated channel\n");
      }
      
      return NULLPTR;
   }

   lResult = smd_ev_create(&lEvent, gMediaHandlerObj.mSwitchChannelParms.channel,
         kSMEventTypeReadData, kSMChannelSpecificEvent );

   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not create Prosody Event <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
   }
   else
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Successfully created Prosody Event EventFd:%d Mode:%d Channel:%d EvtType:%d"
            "IssueEvt:%d", lEvent.fd, lEvent.mode, gMediaHandlerObj.mSwitchChannelParms.channel,
            kSMEventTypeReadData, kSMChannelSpecificEvent);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
   }

   gMediaHandlerObj.mChannelSetEvtParms.channel      = gMediaHandlerObj.mSwitchChannelParms.channel;
   gMediaHandlerObj.mChannelSetEvtParms.event        = lEvent;
   gMediaHandlerObj.mChannelSetEvtParms.event_type   = kSMEventTypeReadData;
   gMediaHandlerObj.mChannelSetEvtParms.issue_events = kSMChannelSpecificEvent;

   lResult = sm_channel_set_event( &gMediaHandlerObj.mChannelSetEvtParms );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
   }
   else
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Successfully associated channel:%d and event:%d",
            gMediaHandlerObj.mChannelSetEvtParms.channel, gMediaHandlerObj.mChannelSetEvtParms.event);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
   }


   gMediaHandlerObj.mFileRecordParms.record_parms.channel = gMediaHandlerObj.mChannelAllocParms.channel;
   gMediaHandlerObj.mFileRecordParms.record_parms.type    =  kSMDataFormat8KHzALawPCM;
   gMediaHandlerObj.mFileRecordParms.record_parms.max_silence =  7000;
   //gMediaHandlerObj.mFileRecordParms.record_parms.max_silence =  5000;

   gettimeofday(&ltimeval, NULL);
   ltmStruct = localtime(&(ltimeval.tv_sec));

   sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%10d",
         ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
         ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);
   
   char x;
   printf("Please enter any key to start\n");
   scanf("%c",&x);
   
   printf("******************************************************************\n");
   printf("Record Start Time: %s ChannelId:%d Status:%d\n",lFileNameTime,
         gMediaHandlerObj.mFileRecordParms.record_parms.channel,
         gMediaHandlerObj.mFileRecordParms.status );
   printf("******************************************************************\n");

   lResult = sm_record_wav_start( lFilename, &gMediaHandlerObj.mFileRecordParms);
   if ( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not initiate wav %s record <%d>", lFilename, lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
   }
   snprintf(lLogText,MAX_LOG_TEXT_LEN,
         "INFO: Success to initiate %s record <%d>", lFilename, lResult );
   T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

   pthread_t     lEventThread;
   int           lRetVal;
   void*   (*lpThreadPtr)(void *);

   lpThreadPtr = PromtRecording;
   lRetVal = pthread_create(&lEventThread,
         NULLPTR, lpThreadPtr, (void *) (&gMediaHandlerObj.mFileRecordParms));

   if(0 != lRetVal)
   {
      return NULLPTR;
   }
   else
   {
      pthread_join(lEventThread, NULL);
   }

   gettimeofday(&ltimeval, NULL);
   ltmStruct = localtime(&(ltimeval.tv_sec));

   sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%10d",
         ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
         ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);
   printf("------- Time: %s ChannelId:%d Status:%d\n",lFileNameTime,
         gMediaHandlerObj.mFileRecordParms.record_parms.channel, gMediaHandlerObj.mFileRecordParms.status );


   #if 0
   if(false == gMediaHandlerObj.DisAssociateEvent(lEvent))
   {
      printf("Failed to Disassociate an event from Channel\n");
   }

   if(false == gMediaHandlerObj.DisConnectFromProsody
         (&gMediaHandlerObj.mSwitchChannelParms))
   {
      printf("Failed to Disconnect Call from Prosody\n");
   }

   if(false == gMediaHandlerObj.ReleaseChannel())
   {
      printf("Failed to Release an allocated channel\n");
   }
   #endif

   pthread_exit(NULLPTR);
   return NULLPTR;
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

   if ( gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id )
   {

      INIT_ACU_STRUCT( &mConnectParms );

      mConnectParms.ist  = gMediaHandlerObj.mCmdStream;   
      mConnectParms.its  = gMediaHandlerObj.mCmdTimeSlot; 
      mConnectParms.mode = CONNECT_MODE;
      mConnectParms.ost  = mChannelInfoParms.ist;
      mConnectParms.ots  = mChannelInfoParms.its;

      printf("-----------------------------------------------------------------------------\n");
      printf("Connect Params: ist:%d its:%d mode:%d ost:%d ots:%d cardId:%d\n", 
            mConnectParms.ist, mConnectParms.its, mConnectParms.mode, mConnectParms.ost, 
            mConnectParms.ots,gMediaHandlerObj.mOpenModule.card_id);
      printf("-----------------------------------------------------------------------------\n");

      lResult = sw_set_output(gMediaHandlerObj.mOpenCardDet.card_id, &mConnectParms );
      if ( lResult != 0)
      {
         printf ("Error could not make switch connection <%d>", lResult);
         return false;
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

         mDisConnectParms.ost  = gMediaHandlerObj.mCmdStream;   
         mDisConnectParms.ots  = gMediaHandlerObj.mCmdTimeSlot; 
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

                  mStream = (mOpenModule.module_ix * 2) + mModuleInfo.min_stream; //mOpenModule.min_stream

                  mStream+= 1;  // for recording on next stream
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
                        mStream += 1;                      //2nd stream
                        mChannels -= TIMESLOTS_PER_STREAM; //numbers of channels remaining
                        mTimeSlot = 0;                     //reset timeslot to 0 (1st ts on the 2nd stream
                     }
                     INIT_ACU_SM_STRUCT( &mChannelAllocParms );
                     INIT_ACU_SM_STRUCT( &mSwitchChannelParms );

                     mChannelAllocParms.module      = mOpenModule.module_id;
                     mChannelAllocParms.type        = kSMChannelTypeInput;     //Full Duplex Channel
                     //mChannelAllocParms.type        = kSMChannelTypeFullDuplex;     //Full Duplex Channel

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
                     mSwitchChannelParms.ts          = mCmdTimeSlot;

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

                     if( mChannelAllocParms.type != kSMChannelTypeInput )
                     {
                        lResult = sm_switch_channel_output( &mSwitchChannelParms );
                        if( lResult != 0 )
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN,
                                 "Error: Could not setup channel's output switching <%d>", lResult );
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
