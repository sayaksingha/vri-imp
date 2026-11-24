static const char* id="@(#) $Id: MediaAsyncRecorder.cc,v 1.1.4.3 2016/04/12 21:59:30 shijuc Exp $";
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
#include "MediaAsyncRecordHdlr.h"

extern MediaHandler gMediaHandlerObj;

tSMEventId     gEvent;
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

   memset(&lSe, 0, sizeof(lSe));
   memset(&lDcp, 0, sizeof(lDcp));

   lSe.channel      = gRcp[index].channel;
   lSe.event        = gEvent;
   lSe.event_type   = kSMEventTypeReadData;
   lSe.issue_events = kSMChannelNoEvent;

   lResult = sm_channel_set_event( &lSe );
   if( lResult != 0 )
   {
      snprintf(lLogText,MAX_LOG_TEXT_LEN,
            "Error: Could not associate channel and event <%d>", lResult );
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

      return false;
   }

   fclose(gRcp[index].rp.fd);

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
// METHOD      : RecordThread
// DESCRIPTION : Check Channel Status Thread
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *RecordThread (void *lPtr)
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
         int lIndex = 0;
         sm_record_status_parms lStatusParms;
         memset(&lStatusParms, 0, sizeof(lStatusParms));

         lStatusParms.channel = kSMNullChannelId;
         lResult = sm_record_status (&lStatusParms);
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
   }

   pthread_exit(NULLPTR);
   return NULLPTR;
}

//-------------------------------------------------------------
// METHOD        : rMediaPort.moduleIndxRecordChan
// DESCRIPTION   :
// PARAMETER     : NONE
// RETURN        : INT
//-------------------------------------------------------------
BOOLEAN MediaHandler::RecordChan(Channel & rMediaPort)
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
   ac.type   = kSMChannelTypeInput;
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
   memset(&lFileName, 0, sizeof(lFileName));
   gRcp[lIndex].channel = ac.channel;
   gRcp[lIndex].rp.record_parms.channel = ac.channel;
   gRcp[lIndex].rp.record_parms.type    = kSMDataFormat8KHzALawPCM;
   //gRcp[lIndex].rp.record_parms.max_silence = 0;
   gRcp[lIndex].rp.record_parms.max_silence = 5000;
   gRcp[lIndex].outPort.st = mCmdStream;
   gRcp[lIndex].outPort.ts = rMediaPort.ts;
   //gRcp[lIndex].rp.record_parms.max_elapsed_time = 40000;
   gRcp[lIndex].outPort.type = (kSMTimeslotType) rMediaPort.type;
   snprintf(lFileName, MAX_LOG_TEXT_LEN, "%s_%d_%d.wav", 
         mFileName, mCmdStream, rMediaPort.ts);

#if 0
   if(!lFileName)
      gRcp[lIndex].rp.fd = 0;
   else
   {
      gRcp[lIndex].rp.fd = fopen(lFileName, "w");
      if (!gRcp[lIndex].rp.fd) 
      {
         char *n;
         n = (char*) malloc(strlen(lFileName) + sizeof("File: ''"));
         if (n) sprintf(n, "File: '%s'", lFileName);
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s", "Cannot open file");
         T(gTrace, printf("%s: %s\n", gProcessName, lLogText););         
         free(n);
         return -1;
      }
   }
#endif

   memset(&sw, 0, sizeof(sw));
   sw.st = rMediaPort.st;
   sw.ts = rMediaPort.ts;
   sw.type = (kSMTimeslotType) rMediaPort.type;
   sw.channel = ac.channel;
   lResult = sm_switch_channel_input(&sw);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_switch_channel_output() failed <%d>", lResult);
      T(gTrace, printf("%s: %s\n", gProcessName, lLogText););

      this->ReleaseChannel (sw.channel);

      return lResult;
   }

   //if (gMediaHandlerObj.mOpenModule.card_id == gMediaHandlerObj.mOpenCardDet.card_id)
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
         printf ("Error could not make switch connection <%d>", lResult);
         return lResult;
      }
   }

   memset(&se, 0, sizeof(se));
   se.event = gEvent;
   se.issue_events = kSMAnyChannelEvent;
   se.event_type = kSMEventTypeReadData;
   se.channel = gRcp[lIndex].channel;
   lResult = sm_channel_set_event(&se);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "sm_channel_set_event() failed  <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return lResult;
   }

   gettimeofday(&ltimeval, NULL);
   ltmStruct = localtime(&(ltimeval.tv_sec));

   sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%06d",
         ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
         ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);

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
   int        lModuleIndx;
   INT16      lModules;

   Channel      lMediaPort;
   pthread_t    lResponseThread;
   TEXT         lLogText[MAX_LOG_TEXT_LEN + 1] = "";

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
               memset(&mOpenModule, 0, sizeof(mOpenModule) * lModules);

               for(lModuleIndx = 0; lModuleIndx < lModules; lModuleIndx++ )
               {
                  SM_OPEN_MODULE_PARMS lOpenModule;
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

               lModuleIndx = 0;

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
                     kSMEventTypeReadData, kSMAnyChannelEvent);
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
                     kSMEventTypeReadData, kSMAnyChannelEvent, gEvent.fd, gEvent.mode);
               T(gTrace, printf("%s: %s\n",gProcessName, lLogText););

               for(;;)
               {
                  if(mNumOfChann)
                  {
                     lResult = RecordChan(lMediaPort);
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

