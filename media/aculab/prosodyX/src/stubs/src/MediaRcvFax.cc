static const char* id="@(#) $Id: MediaRcvFax.cc,v 1.1.4.3 2016/04/12 21:59:32 shijuc Exp $";
//-----------------------------------------------------------------------------
// NAME
//   MediaRcvFax.cc -- MediaHandler Main process
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
// Originated : Roopa A. M                                     DATE:01-OCT-2012
//------------------------------------------------------------------------------
#include "MediaFax.h"

extern MediaHandler gMediaHandlerObj;

//-------------------------------------------------------------
// METHOD        : ReceiveFax
// DESCRIPTION   :
// PARAMETER     : ACU_FAX_PARAMS *
// RETURN        : VOID
//-------------------------------------------------------------
BOOLEAN MediaHandler::ReceiveFax(ACU_FAX_PARAMS * fax_param)
{
   char       lFileNameTime[100];
   struct     tm* ltmStruct;
   struct     timeval    ltimeval;
   int rc = 0, err = 0, page_count = 0;

   SMFAX_TRACE_PARMS trace_parms;
   SMFAX_SESSION fax_session;
   SMFAX_TRANSPORT_MEDIUM fax_trans_parms;
   SMFAX_NEGOTIATE_PARMS fax_neg_parms;
   SMFAX_PAGE_PROCESS_PARMS fax_pp_parms;
   SMFAX_PAGE_ACCESS_PARMS fax_pa_parms;

   ACTIFF_FILE *outF;
   ACTIFF_PAGE_HANDLE  pageH = 0;
   ACTIFF_PAGE_PROPERTIES page_props;

   BFILE *bfp = 0;

   int actiff_error = 0;

   INIT_ACU_SM_STRUCT(&fax_neg_parms);
   INIT_ACU_SM_STRUCT(&fax_trans_parms);
   INIT_ACU_SM_STRUCT(&fax_session);
   INIT_ACU_SM_STRUCT(&fax_pp_parms);
   INIT_ACU_SM_STRUCT(&fax_pa_parms);
   INIT_ACU_SM_STRUCT(&trace_parms);

   fax_session.channel = fax_param->TiNG_ChannelId;
   fax_session.module = fax_param->module_id;
   fax_session.fax_id = fax_param->fax_id;

   /* if changes are required refer to the Fax API Guide Section 4.1.8 SMFAX_USER_OPTIONS */
   memset(&fax_session.user_options, 0, sizeof(fax_session.user_options));
   smfax_get_recommended_user_options(&fax_session.user_options);
   fax_session.user_options.fax_mode =  kSMFaxModeCalled;

   /* if changes are required refer to the Fax API Guide Section 4.1.2 SMFAX_CAPS */
   memset(&fax_session.fax_caps, 0, sizeof(fax_session.fax_caps));
   smfax_get_recommended_fax_caps(&fax_session.fax_caps);

   fax_session.fax_caps.v8 = fax_session.fax_caps.v34 = 1;
   fax_session.fax_caps.MR2D = 0;
   //fax_session.fax_caps.MMRT6 = 1;
   //fax_session.fax_caps.ECM = 1;

#if 0
   fax_session.user_options.max_percent_badlines = 0.1f;
   fax_session.user_options.max_consec_badlines = 20;
   fax_session.user_options.max_modem_fb = 4;
   fax_session.user_options.page_retries = 3;
   fax_session.user_options.T38_ASN1_version = 2;
   fax_session.user_options.ecm_continue_to_correct = 0;
   fax_session.user_options.drop_speed_on_ctc       = 0;   // true or false
   fax_session.user_options.SkipToneGen             = 0;
   fax_session.user_options.SkipSubsId              = 1;
   fax_session.user_options.StrictT30               = 0;
#endif

#if 0
   strcpy(fax_session.fax_caps.subscriber_id, fax_param->subscriber);
   fax_session.fax_caps.data_rate = fax_param->max_data_rate;
   fax_session.fax_caps.v17 = fax_param->v17;
   fax_session.fax_caps.v29 = fax_param->v29;
   fax_session.fax_caps.v27ter = fax_param->v27ter;
   fax_session.fax_caps.v34 = fax_param->v34;
   fax_session.fax_caps.v8 = fax_param->v8;
   fax_session.fax_caps.MR2D = 0;
   fax_session.fax_caps.MMRT6 = 0;
   fax_session.fax_caps.ECM = fax_param->ECM;
   fax_session.fax_caps.Res200x200 = 1;
   fax_session.fax_caps.polling_mode = 0;
   fax_session.fax_caps.ECMsize64 =1;
   fax_session.fax_caps.NoRcvrOp = 1;
#endif

   outF = actiff_write_open(fax_param->tiff_file, 0, &actiff_error);
   if(!outF || actiff_error)
   {
      printf("Unable to open TIFF File for writing [%d:%d:1]\n", fax_param->stream, fax_param->ts);
      fax_param->return_code = kSMFaxNoSessionCreated;
      return false;
   }

   printf("Successfully Opened File:%s for Writing FAX\n", fax_param->tiff_file);

   if (0 != fax_param->trace)
   {
      rc = bfile(&bfp);
      rc = bfopen(bfp, fax_param->trace_file, "wtc");
      trace_parms.log_file = bfp;
      trace_parms.trace_level = TRACE_ALL;
      fax_session.trace_parms = &trace_parms;
   }

   gettimeofday(&ltimeval, NULL);
   ltmStruct = localtime(&(ltimeval.tv_sec));
   sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%06d",
         ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
         ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);

   printf("---------------------------------------------------------------------------------\n");
   printf("\33[33mFAX SESSION STARTTIME:%s CHANNELID:%d FILE:%s\33[0m\n", lFileNameTime,
         mChannelInfoParms.channel, fax_param->tiff_file);
   printf("---------------------------------------------------------------------------------\n");

   printf("FAX CREATE SESSION Parameters:\n");
   printf("==============================\n");
   printf("Channel-----------: %ld\n", fax_session.channel);
   printf("Module------------: %ld\n\n", fax_session.module);
   printf("======  USER OPTIONS ========\n");
   printf("MaxBadlines-------:%f\n", fax_session.user_options.max_percent_badlines);
   printf("MaxConsecBadlines-:%ld\n", fax_session.user_options.max_consec_badlines);
   printf("MaxModemFb--------:%ld\n", fax_session.user_options.max_modem_fb);
   printf("PageRetry---------:%ld\n", fax_session.user_options.page_retries);
   printf("T38ASN1_version---:%ld\n", fax_session.user_options.T38_ASN1_version);
   printf("ecm_cont2correct--:%ld\n", fax_session.user_options.ecm_continue_to_correct);
   printf("drop_speed_on_ctc-:%ld\n", fax_session.user_options.drop_speed_on_ctc);
   printf("SkipToneGen-------:%ld\n", fax_session.user_options.SkipToneGen);
   printf("SkipSubsId--------:%ld\n", fax_session.user_options.SkipSubsId);
   printf("StrictT30---------:%ld\n", fax_session.user_options.StrictT30);
   printf("FaxMode-----------:%ld\n", fax_session.user_options.fax_mode,
         AcuMediaUtil::ConvertFaxRcToStr(fax_session.user_options.fax_mode));
   printf("======  FAX CAPS  ==========\n");
   printf("SubsId------------:%s\n", fax_session.fax_caps.subscriber_id);
   printf("DataReate---------:%ld\n", fax_session.fax_caps.data_rate);
   printf("V17---------------:%ld\n", fax_session.fax_caps.v17);
   printf("V29---------------:%ld\n", fax_session.fax_caps.v29);
   printf("V27ter------------:%ld\n", fax_session.fax_caps.v27ter);
   printf("V34---------------:%ld\n", fax_session.fax_caps.v34);
   printf("V8----------------:%ld\n", fax_session.fax_caps.v8);
   printf("MR2D--------------:%ld\n", fax_session.fax_caps.MR2D);
   printf("MMRT6-------------:%ld\n", fax_session.fax_caps.MMRT6);
   printf("ECM---------------:%ld\n", fax_session.fax_caps.ECM);
   printf("Res200x200--------:%ld\n", fax_session.fax_caps.Res200x200);
   printf("pollingMode-------:%ld\n", fax_session.fax_caps.polling_mode);
   printf("ECMsize64---------:%ld\n", fax_session.fax_caps.ECMsize64);
   printf("NoRcvrOp----------:%ld\n", fax_session.fax_caps.NoRcvrOp);
   printf("FaxId-------------:%ld\n", fax_session.fax_id);
   printf("==============================\n");

   rc = smfax_create_session(&fax_session);

   if(rc == kSMFaxStateMachineRunning)
      printf( "\33[32msmfax_create_session Success [%d:%d:0] Status:%s[%d]\33[0m\n", 
            fax_param->stream, fax_param->ts, AcuMediaUtil::ConvertFaxRcToStr(rc), rc);
   else 
   {
      printf("smfax_create_session Failed [%d] ON [%d:%d:0]\n", rc, fax_param->stream, fax_param->ts);
      return false;
   }

   if(kSMFaxStateMachineRunning == rc)
   {
      printf("FAX SESSION RUNNING [%d:%d:0] Status:%s[%d]\n", fax_param->stream, fax_param->ts, 
            AcuMediaUtil::ConvertFaxRcToStr(rc), rc);

      while(rc == kSMFaxStateMachineRunning)
      {
         memset(&page_props, 0, sizeof(page_props));
         fax_neg_parms.fax_session = &fax_session;
         fax_neg_parms.page_props = &page_props;
         rc = smfax_rx_negotiate(&fax_neg_parms);

         if(rc != kSMFaxStateMachineRunning) 
            printf("smfax_rx_negotiate [%d] ON [%d:%d:0] Status:%s[%d]\n", 
                  rc, fax_param->stream, fax_param->ts, AcuMediaUtil::ConvertFaxRcToStr(rc), rc);

         printf("\33[32mFAX NEGOTIATE STATUS:%s[%d]\33[0m\n", AcuMediaUtil::ConvertFaxRcToStr(rc), rc);

         if(kSMFaxStateMachineRunning == rc)
         {
            if(kSMFaxPolling == fax_neg_parms.is_polling)
            {
               printf("WARNING: Polled session established. %d %d\n", fax_param->stream, fax_param->ts);
               printf("WARNING: Issueing polite interrupt.. %d %d\n", fax_param->stream, fax_param->ts);
               rc = smfax_polite_interrupt(&fax_session);
               if(rc) 
                  printf("smfax_polite_interrupt [%d] ON [%d:%d:0]\n", rc, 
                        fax_param->stream, fax_param->ts);

               printf("POLITE INTERRUPT [%d:%d:0] STATUS:%s[%d]\n", fax_param->stream, fax_param->ts,
                     AcuMediaUtil::ConvertFaxRcToStr(rc), rc);
            }

            printf("NEGOTIATE COMPLETED [%d:%d:0]\n", fax_param->stream, fax_param->ts);
            printf("NEGOTIATED MODEM: v27=%ld v29=%ld v17=%ld v34=%ld\n", 
                  fax_session.fax_caps.v27ter, fax_session.fax_caps.v29, fax_session.fax_caps.v17, 
                  fax_session.fax_caps.v34);
            printf("NEGOTIATED DATA RATE: %ld kbps\n", fax_session.fax_caps.data_rate);

            while(kSMFaxStateMachineRunning == rc && !fax_pp_parms.renegotiate)
            {
               printf("- Receiving page < %d > [%d:%d:1] \n", 
                     (page_count+1), fax_param->stream, fax_param->ts);

               *fax_param->faxpage = page_count+1;
               fax_pp_parms.fax_session = &fax_session;
               fax_pp_parms.page_handle = &pageH;
               rc = smfax_rx_page(&fax_pp_parms);
               if((rc != kSMFaxStateMachineTerminated) && (rc != kSMFaxStateMachineRunning))
                  printf("smfax_rx_page [%d] ON [%d:%d:1]", rc, fax_param->stream, fax_param->ts);

               printf("- Receive COMPLETE < %d > [%d:%d:1]", 
                     (page_count++)+1, fax_param->stream, fax_param->ts);

               fax_pa_parms.fax_session = &fax_session;
               fax_pa_parms.page_handle = fax_pp_parms.page_handle;
               fax_pa_parms.actiff      = outF;

               err = smfax_store_page(&fax_pa_parms);
               if(err != kSMFaxPageOK) 
                  printf("smfax_store_page [%d] ON [%d:%d:1]", rc, fax_param->stream, fax_param->ts);

               printf("STORE PAGE [%d:%d:1]", fax_param->stream, fax_param->ts);

               err = smfax_close_page(&pageH);
               if(err != kSMFaxPageOK) 
                  printf("smfax_close_page [%d] ON [%d:%d:1]", rc, fax_param->stream, fax_param->ts);

               printf("CLOSE PAGE [%d:%d:1]", fax_param->stream, fax_param->ts);
            }
         }
      }
   }
   rc = smfax_close_session(&fax_session);
   if(rc) 
      printf("smfax_close_session [%d] ON [%d:%d:0]\n", rc, fax_param->stream, fax_param->ts);

   printf("\33[33msmfax_close_session [%d:%d:0]\33[0m\n", fax_param->stream, fax_param->ts);

   rc = actiff_close(outF, &actiff_error);
   if(rc) 
      printf("actiff_close [%d] ON [%d:%d:0]\n", rc, fax_param->stream, fax_param->ts);

   printf("\33[33mactiff_close [%d:%d:0]\33[0m\n", fax_param->stream, fax_param->ts);

   gettimeofday(&ltimeval, NULL);
   ltmStruct = localtime(&(ltimeval.tv_sec));
   sprintf(lFileNameTime,"%04d/%02d/%02d %02d:%02d:%02d:%06d",
         ltmStruct->tm_year+1900,ltmStruct->tm_mon+1,ltmStruct->tm_mday,
         ltmStruct->tm_hour, ltmStruct->tm_min,ltmStruct->tm_sec,ltimeval.tv_usec);

   printf("---------------------------------------------------------------------------------\n");
   printf("\33[33mFAX SESSION CLOSE TIME:%s CHANNELID:%d FILE:%s\33[0m\n", lFileNameTime,
         mChannelInfoParms.channel, fax_param->tiff_file);
   printf("---------------------------------------------------------------------------------\n");

   if(bfp)
   {
      rc = bfile_dtor(bfp);
      if(rc) 
         printf("bfile_dtor [%d] ON [%d:%d:0]\n", rc, fax_param->stream, fax_param->ts);
   }

   fax_param->return_code = fax_session.exit_error_code;
   return true;
}

//-------------------------------------------------------------
// METHOD        : GetFaxDataRate
// DESCRIPTION   :
// PARAMETER     : int data_rate_const, int fax_modem
// RETURN        : INT 
//-------------------------------------------------------------
int MediaHandler::GetFaxDataRate(int data_rate_const, int fax_modem)
{
   int data_rate = 0;

   switch (data_rate_const)
   {
      case kSMFaxDataRate_Default:
         switch (fax_modem)
         {
            case MODEM_V34:
               data_rate = 33600;
               break;

            case MODEM_V17:
               data_rate = 14400;
               break;

            case MODEM_V29:
               data_rate = 9600;
               break;

            case MODEM_V27:
               data_rate = 4800;
               break;
         }
         break;

      case kSMFaxDataRate_2400:
         data_rate = 2400;
         break;

      case kSMFaxDataRate_4800:
         data_rate = 4800;
         break;

      case kSMFaxDataRate_7200:
         data_rate = 7200;
         break;

      case kSMFaxDataRate_9600:
         data_rate = 9600;
         break;

      case kSMFaxDataRate_12000:
         data_rate = 12000;
         break;

      case kSMFaxDataRate_14400:
         data_rate = 14400;
         break;

      case kSMFaxDataRate_16800:
         data_rate = 16800;
         break;

      case kSMFaxDataRate_19200:
         data_rate = 19200;
         break;

      case kSMFaxDataRate_21600:
         data_rate = 21600;
         break;

      case kSMFaxDataRate_24000:
         data_rate = 24000;
         break;

      case kSMFaxDataRate_26400:
         data_rate = 26400;
         break;

      case kSMFaxDataRate_28800:
         data_rate = 28800;
         break;

      case kSMFaxDataRate_31200:
         data_rate = 31200;
         break;

      case kSMFaxDataRate_33600:
         data_rate = 33600;
         break;
   }

   return data_rate;
}

//----------------------------------------------------------------------
// METHOD      : prosody_resource_thread
// DESCRIPTION : Start Channel Thread
// PARAMETER   :
// RETURN      : VOID
//----------------------------------------------------------------------
void *prosody_resource_thread (void *lPtr)
{
   struct     tm* ltmStruct;
   struct     timeval    ltimeval;
   int        lModem = 0;
   int        lNumFaxPages = 0;

   if(false == gMediaHandlerObj.ConnectToProsody (&gMediaHandlerObj.mSwitchChannelParms))
   {
      printf("Error: Unable to connect Call to Prosody\n");

      if(false == gMediaHandlerObj.ReleaseChannel())
      {
         printf("Failed to Release an allocated channel\n");
      }

      return NULLPTR;
   }

   ACU_FAX_PARAMS fax_param;
   memset(&fax_param, 0, sizeof(ACU_FAX_PARAMS));

   fax_param.trace = TRACE_ALL; //cfgRead

   if(gMediaHandlerObj.mFaxIterations == 5) // cfgRead
      gMediaHandlerObj.mFaxIterations = 1;
   else
      gMediaHandlerObj.mFaxIterations++;

   sprintf(fax_param.tiff_file, "/opt/ivr/bin/stubs/Fax_%d_%d_%d.tif", 
         gMediaHandlerObj.mCmdStream, gMediaHandlerObj.mCmdTimeSlot, gMediaHandlerObj.mFaxIterations);

   if(fax_param.trace)
   {
      gettimeofday(&ltimeval, NULL);
      ltmStruct = localtime(&(ltimeval.tv_sec));
      sprintf (fax_param.trace_file, "/opt/ivr/var/log/FAX-%d-%d-%d_%02d-%02d-%02d.log", 
            gMediaHandlerObj.mCmdStream, gMediaHandlerObj.mCmdTimeSlot, 
            gMediaHandlerObj.mFaxIterations, ltmStruct->tm_hour, ltmStruct->tm_min, 
            ltmStruct->tm_sec);
   }

   switch(lModem) // cfgRead
   {
      case MODEM_V34:
         fax_param.v34 = 1;
         fax_param.v8 = 1;
         fax_param.ECM = 1;
#if 0
      case MODEM_V17:
         fax_param.v17 = 1;
      case MODEM_V29:
         fax_param.v29 = 1;
      case MODEM_V27:
         fax_param.v27ter = 1;
#endif
   }

   fax_param.max_data_rate = gMediaHandlerObj.GetFaxDataRate(FAX_MAX_DATA_RATE, lModem);

   printf("MODEMS TO REQUEST DURING NEGOTIATION: v27=%d v29=%d v17=%d v34=%d\n", 
         fax_param.v27ter, fax_param.v29, fax_param.v17, fax_param.v34);
   printf("DATA RATE TO REQUEST DURING NEGOTIATION: %d kbps\n", fax_param.max_data_rate);

   if (lModem != MODEM_V34)
      fax_param.ECM = ENABLE_ECM;

   fax_param.return_code = 0;
   fax_param.trace = 3;
   fax_param.fax_id = 101;
   strcpy(fax_param.subscriber, "919480134444");
   fax_param.stream = gMediaHandlerObj.mCmdStream;
   fax_param.ts = gMediaHandlerObj.mCmdTimeSlot;
   fax_param.faxpage = &lNumFaxPages;
   fax_param.TiNG_ChannelId = gMediaHandlerObj.mChannelInfoParms.channel;
   fax_param.module_id = gMediaHandlerObj.mModuleInfo.module; 
   fax_param.rude_interrupt=0;

   if(false == gMediaHandlerObj.ReceiveFax(&fax_param))
      printf("FAILED To Receive FAX\n");

   if (kSMFaxDCNNormal == fax_param.return_code)
      printf("\33[32mFAX SUCCEEDED for [%d:%d:0]\33[0m\n", fax_param.stream, fax_param.ts);
   else
      printf("\33[31mFAX FAILED [%d:%d:0] RC:%s[%d]\33[0m\n", fax_param.stream, fax_param.ts, 
            AcuMediaUtil::ConvertFaxRcToStr(fax_param.return_code), fax_param.return_code);

   if(false == gMediaHandlerObj.DisConnectFromProsody
         (&gMediaHandlerObj.mSwitchChannelParms))
   {
      printf("Failed to Disconnect Call from Prosody\n");
   }

   if(false == gMediaHandlerObj.ReleaseChannel())
   {
      printf("Failed to Release an allocated channel\n");
   }

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
   SM_CLOSE_MODULE_PARMS      lCloseModuleParms;
   SM_VMPTX_DESTROY_TONESET_PARMS  lDesVmptxToneset;

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
      memset(&lCloseModuleParms, 0, sizeof(lCloseModuleParms));
      lCloseModuleParms.module_id = pModuleData[i].module_id;
      sm_close_module(&lCloseModuleParms);
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
   OUTPUT_PARMS  lCp;

   mChannelInfoParms.channel = switch_params->channel;
   lResult = sm_channel_info( &mChannelInfoParms );
   if ( lResult != 0 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error: Could not get channel info <%d>", lResult);
      T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
      return false;
   }

   if ( mOpenModule.card_id == mOpenCardDet.card_id )
   {
      INIT_ACU_STRUCT( &lCp );
      lCp.size = sizeof(lCp);
      lCp.ist  = switch_params->st;
      lCp.its  = switch_params->ts;
      lCp.mode = CONNECT_MODE;
      lCp.ost  = mCmdStream; 
      lCp.ots  = mCmdTimeSlot; 

      printf("-----------------------------------------------------------------------------\n");
      printf("Connect Params:\n ist:%d its:%d mode:%d ost:%d ots:%d CardId:%d\n", 
            lCp.ist, lCp.its, lCp.mode, lCp.ost, lCp.ots, mOpenModule.card_id);
      printf("-----------------------------------------------------------------------------\n");

      lResult = sw_set_output(mOpenCardDet.card_id, &lCp );
      if ( lResult != 0)
      {
         printf ("Error could not make switch connection <%d>", lResult);
         return false;
      }
   }

   if (mOpenModule.card_id == mOpenCardDet.card_id)
   {
      INIT_ACU_STRUCT( &lCp );
      lCp.size = sizeof(lCp);
      lCp.ist = mCmdStream;
      lCp.its = mCmdTimeSlot;
      lCp.mode = CONNECT_MODE;
      lCp.ost = switch_params->st;
      lCp.ots = switch_params->ts;

      printf("-----------------------------------------------------------------------------\n");
      printf("Connect Params: ist:%d its:%d mode:%d ost:%d ots:%d card:%d \n",
            lCp.ist, lCp.its, lCp.mode, lCp.ost, lCp.ots, mOpenCardDet.card_id);
      printf("-----------------------------------------------------------------------------\n");

      lResult = sw_set_output(mOpenCardDet.card_id, &lCp);
      if ( lResult != 0)
      {
         printf ("Error could not make switch connection FAX <%d>", lResult);
         return lResult;
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

         mDisConnectParms.ost  = mCmdStream;   
         mDisConnectParms.ots  = mCmdTimeSlot; 
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

         mDisConnectParms.ost  = mSwitchChannelParms.st;   
         mDisConnectParms.ots  = mSwitchChannelParms.ts; 
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
   mChannelSetEvtParms.event_type   = kSMEventTypeWriteData;
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
               pModuleData = (MODULE_DATA*) calloc(mModules, sizeof(MODULE_DATA));
               memset(pModuleData, 0, mModules * sizeof(MODULE_DATA));

               for( lModuleIndx = 1; lModuleIndx < 2; lModuleIndx++ )
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

                  INIT_ACU_SM_STRUCT( &mModuleInfo );
                  mModuleInfo.module = mOpenModule.module_id;

                  pModuleData[lModuleIndx].module = lModuleIndx;
                  pModuleData[lModuleIndx].module_id = mOpenModule.module_id;
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
                           "Success to Obtain module Information ModuleId:%d Card ModuleIndx:%ld",
                           mModuleInfo.module, mOpenModule.module_ix );
                     T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                  }

                  if(lModIndx == 0 || lModIndx == 2 || lModIndx == 4 || lModIndx == 6)
                     mStream = mModuleInfo.min_stream;
                  else
                     mStream = mModuleInfo.min_stream + 1;

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
                     mChannelAllocParms.type        = kSMChannelTypeFullDuplex;     //Full Duplex Channel
                     //mChannelAllocParms.type        = kSMChannelTypeInput;     //Full Duplex Channel

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

                     lResult = sm_switch_channel_output( &mSwitchChannelParms );
                     if( lResult != 0 )
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "Error: Could not setup channel's output switching <%d>", lResult );
                        T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                        return lResult;
                     }

                     lResult = sm_switch_channel_input( &mSwitchChannelParms );
                     if( lResult != 0 )
                     {
                        snprintf(lLogText, MAX_LOG_TEXT_LEN,
                              "Error: Could not setup channel's input switching <%d>", lResult );
                        T(gTrace, printf("%s: %s\n",gProcessName, lLogText););
                        return lResult;
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
      }
   }

   return true;
}
