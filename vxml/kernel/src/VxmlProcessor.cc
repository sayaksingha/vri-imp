static const char* id="@(#) $Id: VxmlProcessor.cc,v 1.1.1.1.2.6.2.2 2017/01/06 20:35:18 mram Exp $" ;
//----------------------------------------------------------------------
// NAME
// VxmlProcessor.cc
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
// Originated : KOTHARI KOAML                           DATE:03-JAN-2011
//----------------------------------------------------------------------

#include "ConfigFile.h"
#include "IvrUtil.h"
#include "IvrQueApi.h"
#include "PromptApi.h"
#include "IvrChannelApi.h"

#include "limits.h"

#include "VXItypes.h"
#include "VXItrd.h"
#include "VXIinterpreter.h"
#include "VXIplatform.h"
#include "VXIlog.h"
#include "VXIclient.h"
#include "VXIclientUtils.h"
#include "VXIclientConfig.h"

#include "SBlog.h"
#include "SBlogListeners.h"
#include "SWIprintf.h"


#define DEFAULT_CONFIG_FILENAME       "SBclient.cfg"
#define STRINGLEN                      4096

// Structure for controlling channel threads //
typedef struct ChannelThreadArgs {
	VXIunsigned    channelNum;
	long           maxCalls;
	const VXIMap  *configArgs;
	const VXIchar *vxmlURL;
	VXItrdThread  *threadHandle;
	long           delayCallSec;
//	int				refNum;
	VxmlRequest 	vxmlReqMsg;
} ChannelThreadArgs;

typedef struct ChildThreadArgs {
	INT16		       channelNum;
	VXIplatform    *platform;
	pthread_mutex_t count_mutex; //     = PTHREAD_MUTEX_INITIALIZER;
} ChildThreadArgs;


TEXT 				gProcessName[IVR_MAX_PROCESS_NAME + 1] = "";
Log 				gLog;
CTrace 			gTrace (TRACE_IVR_VXML_ENV);
TEXT 				gLogText[MAX_LOG_TEXT_LEN + 1] = "";
INT32          gReferenceNum;

BOOLEAN        gIsAudioOpSupported = false;
TEXT           gPauseDtmf[MAX_DTMF_LEN] = "";
TEXT           gResumeDtmf[MAX_DTMF_LEN] = "";
TEXT           gForwardDtmf[MAX_DTMF_LEN] = "";
TEXT           gRewindDtmf[MAX_DTMF_LEN] = "";

//HealthCheckApi gHealthCheckObj;
IvrQueApi      gIvrQueApi;
VxmlRequest 	gVxReqMsg;
VxmlResponse	gVxResMsg; 

ChannelThreadArgs *channelArgs;
ChildThreadArgs   *childArgs     = NULL;
BOOLEAN            gNewCall;

int                gArrChildPid[1000];
BOOLEAN            gAllLogsReqd = false;

IvrChannelApi      gChannelApi;

TEXT            PromptApi::mDtmf[];
TEXT            gXmlPath[MAX_PATH];
TEXT            gGlobalResourcePath[MAX_PATH];

IvrEvrApi         gIvrEvrApi;
IvrEvrMsg         gIvrEvrMsg;
EnumIvrEvent      gIvrEvrEvent;
EnumIvrCause      gIvrEvrCause;
EnumIvrReason     gIvrEvrReason;

TEXT              gCallingNumber[MAX_MSISDN_LEN + 1];
TEXT              gCalledNumber[MAX_MSISDN_LEN + 1];

//Function taken from VXIclient.c - Komal
static const VXIchar * AppendStringAlloc(VXIchar ** dst, const VXIchar * src)
{
	VXIchar * temp, *_dst = NULL;
	int srclen = 0, dstlen = 0;
	if( !dst ) return NULL;
	if( !src ) return *dst;
	srclen = wcslen(src) + 1;
	_dst = *dst;
	if( !_dst )
	{
		_dst = (VXIchar*)malloc(sizeof(VXIchar)*srclen);
		if( !_dst ) return NULL;
		wcscpy(_dst, src);
		*dst = _dst;
		return _dst;
	}
	else
	{
		dstlen = wcslen(_dst);
		temp = (VXIchar*)realloc(_dst, (dstlen + srclen + 1) * sizeof(VXIchar));
		if( !temp ) return NULL;
		_dst = temp;
		wcscpy(&_dst[dstlen], src);
		*dst = _dst;
		return _dst;
	}
	/* shoud never reach here */
	return NULL;
}
//--------------------------------------------------------------------------------------------------
// METHOD      : GenerateEvr
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : BOOLEAN
//--------------------------------------------------------------------------------------------------
BOOLEAN GenerateEvr(INT16 chnlNum, unsigned int refNum)
{
   memset(&gIvrEvrMsg, 0, sizeof(IvrEvrMsg));

   gIvrEvrMsg.evrEvent    =  gIvrEvrEvent;
   gIvrEvrMsg.evrCause    =  gIvrEvrCause;
   gIvrEvrMsg.evrReason   =  gIvrEvrReason;
   strcpy(gIvrEvrMsg.cldNum, gCalledNumber);
   strcpy(gIvrEvrMsg.clgNum, gCallingNumber);
   gIvrEvrMsg.refId        =  chnlNum;
   sprintf(gIvrEvrMsg.callRefNum, "%d", refNum);

   if(true != gIvrEvrApi.GenerateEvr (gIvrEvrMsg))
   {
      return false;
   }
   return true;
}
//-------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN Init ()
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	memset (lLogText, 0, sizeof (lLogText));

	if (false ==  gIvrQueApi.Init())
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN, "Queue API Init failed. Exiting...");
		gLog.GenerateLog (GSYS04, lLogText);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		return false;
	}

   if(true != gIvrEvrApi.Init())
   {
      return false;
   }

/*	if(false == gChannelApi.Init())
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN, "Channel API Init failed. Exiting...");
		gLog.GenerateLog (GSYS04, lLogText);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		return false;
	}
*/
	gNewCall = false;
	return true;
}
//-------------------------------------------------------------
// METHOD      : HandleStopRequest
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
void HandleStopRequest (int sig)
{
	VXIplatform     *platform	=  childArgs->platform;
	TEXT             lLogText[MAX_LOG_TEXT_LEN + 1];

	if(gNewCall == true)
	{
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d", gVxResMsg.refNum, childArgs->channelNum);

      //T(gTrace, printf("%s: %s Stop Signal Received\n", gProcessName, lLogText););
      snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Stop Signal Received", lLogText);
      gLog.GenerateLog(VXML_09, LOG_LEVEL_3, gLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_TERM_EVT_RCV);

		//T(gTrace, printf("%s: %s Channel In-Call, Stoping VXI Platform\n", gProcessName, lLogText););
      snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Channel In-Call, Stoping VXI Platform", lLogText);
      gLog.GenerateLog(VXML_10, LOG_LEVEL_1, gLogText);

		platform->VXIinterpreter->RequestStop(platform->VXIinterpreter, true);

		/* Now end the recognizer, prompt, and tel session */
		//VXIrecResult recResult;
		//T(gTrace, printf("%s: %s Terminating Recognizer Session\n", gProcessName, lLogText););
		//recResult = platform->VXIrec->EndSession(platform->VXIrec, NULL);

		VXIpromptResult promptResult;
		//T(gTrace, printf("%s: %s Terminating Prompt Session\n", gProcessName, lLogText););
		promptResult = platform->VXIprompt->EndSession(platform->VXIprompt, NULL);

		//VXItelResult telResult;
		//T(gTrace, printf("%s: %s Terminating Telephony Session\n", gProcessName, lLogText););
		//telResult = platform->VXItel->EndSession(platform->VXItel, NULL);
	}
   else
   {
      gVxResMsg.refNum = 0;
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d", gVxResMsg.refNum, childArgs->channelNum);
      //gVxResMsg.result = VXIplatform_RESULT_SUCCESS;
      strcpy(gVxResMsg.dtmf, "");

      //T(gTrace, printf("%s: %s Stop Signal Received\n", gProcessName, lLogText););
      snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Stop Signal Received", lLogText);
      gLog.GenerateLog(VXML_09, LOG_LEVEL_3, gLogText);
      gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_TERM_EVT_RCV);

      //T(gTrace, printf("%s: %s Channel Idle, Sending VXML Response\n", gProcessName, lLogText););
      gIvrQueApi.WriteToEventChildQue (gVxResMsg, childArgs->channelNum);
   }

   //T(gTrace, printf("%s: %s Stop RequestlProcessed.\n", gProcessName, lLogText););
   snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Stop RequestlProcessed", lLogText);
	gLog.GenerateLog(VXML_11, LOG_LEVEL_1, gLogText);
	gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_TERM_EVT_PROCESSED);

	//gNewCall = false;

	signal(sig, HandleStopRequest);
	return;
}

/*
//-------------------------------------------------------------
// METHOD      : ReadExternalRequest
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
void *ReadExternalRequest (void * msgptr)
{

	ChildThreadArgs *childArgs = (ChildThreadArgs *)msgptr;
	INT16	           lMsgType	=	childArgs->channelNum;
	VXIplatform     *platform	=  childArgs->platform;
	VxmlRequest		  lVxmlRequest;
	VXIString       *vxmlURL   = NULL;
	VXIchar         *tempstr   = NULL;
	const char      *vxmlURLptr= NULL;
	int              len       = 0;

	T(gTrace, printf("%s: CID:%06d: External Request Read Thread Created.\n", gProcessName, lMsgType););

	while(IvrUtil::KeepRunning())
	{
		//T(gTrace, printf("%s: Thread Waiting to Acquired Lock...\n", gProcessName););
		pthread_mutex_lock( &childArgs->count_mutex );
		//T(gTrace, printf("%s: Thread Acquired Lock...\n", gProcessName););
		//if(true == gIvrQueApi.ReadFromVxmlChildReqQue(lVxmlRequest, lMsgType))
		if(true == gIvrQueApi.ReadFromVxmlReqQue(lVxmlRequest, lMsgType))
		{
			if(EVT_VX_TERM == lVxmlRequest.vxmlEvent)
			{
				T(gTrace, printf("%s: RefNum:%08X: Stop Request Received\n", gProcessName, lVxmlRequest.refNum););
				gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_TERM_EVT_RCV);
				if(gNewCall == true)
				{
					platform->VXIinterpreter->RequestStop(platform->VXIinterpreter, true);

					//VXItelResult telResult;
					VXIpromptResult promptResult;
					VXIrecResult recResult;

					// Now end the recognizer, prompt, and tel session //
					recResult = platform->VXIrec->EndSession(platform->VXIrec, NULL);

					promptResult = platform->VXIprompt->EndSession(platform->VXIprompt, NULL);
				}
				//telResult = platform->VXItel->EndSession(platform->VXItel, NULL);

				T(gTrace, printf("%s: RefNum:%08X: Stop Request Processed.\n", gProcessName, lVxmlRequest.refNum););
				gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_TERM_EVT_PROCESSED);
				gNewCall = false;
				pthread_mutex_unlock( &childArgs->count_mutex );
				//usleep(1000);
			}
			else if(EVT_VX_URL == lVxmlRequest.vxmlEvent)
			{
				T(gTrace, printf("%s: RefNum:%08X: New Call Req Received\n", gProcessName, lVxmlRequest.refNum););
				gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_URL_EVT_RCV);
				if( vxmlURL ) 
					VXIStringDestroy(&vxmlURL);

				vxmlURLptr = lVxmlRequest.vxmlUrl;

				len = strlen(vxmlURLptr);
				tempstr = (VXIchar*)malloc(sizeof(VXIchar)*(len+1));

				if( !tempstr )
					printf("System memory exhausted!\n");

				if ( char2wchar(tempstr, vxmlURLptr, len) != 0 )
					printf("Unable to convert char to wchar_t\n");
				vxmlURL = VXIStringCreate(tempstr);

				FREE(tempstr);

				channelArgs->vxmlURL = VXIStringCStr(vxmlURL);
				gReferenceNum = lVxmlRequest.refNum;
				channelArgs->vxmlReqMsg = lVxmlRequest;

				// Trigger the call processing threads 
				gNewCall = true;
				//T(gTrace, printf("%s: Condition Signaled...\n", gProcessName););
				//pthread_cond_signal( &condition_var );
				T(gTrace, printf("%s: Thread Release Lock...\n", gProcessName););
				pthread_mutex_unlock( &childArgs->count_mutex );
				usleep(1000);
			}
			else
			{
				pthread_mutex_unlock( &childArgs->count_mutex );
			}
		}
		else
		{
			pthread_mutex_unlock( &childArgs->count_mutex );
		}
		//T(gTrace, printf("%s: Thread Release Lock...\n", gProcessName););
	}
	T(gTrace, printf("%s: External Read Thread Exiting...\n", gProcessName););
	return NULLPTR;
}
*/

//-------------------------------------------------------------
// METHOD      : Channel Thread/Function
// DESCRIPTION : Channel Function to be execute per Channel
//               This will initialize the VXI platform and wait 
//               for the new incoming call on specified Channel
// PARAMETER   : ChannelThreadArgs
// RETURN      : INT16 Error/Status 
//-------------------------------------------------------------
INT16 ExecuteChannelFunc(ChannelThreadArgs *rChannelArgs)
{
	VXIplatform      *platform;
	VXIMap           *channelConfig;
	int               lChannel      = channelArgs->channelNum;
	VXIplatformResult platformResult;
//	VXItrdTimer      *delayTimer    = NULL;
	VXIString        *vxmlURL   = NULL;
	VXIchar          *tempstr   = NULL;
	const char       *vxmlURLptr= NULL;
	int               len       = 0;
	TEXT              lLogText[MAX_LOG_TEXT_LEN + 1];
   TEXT              lVxmlFullUrl[MAX_PATH];

//	ChildThreadArgs  *childArgs     = NULL;

	/* Create delay timer */
//	VXItrdTimerCreate(&delayTimer);

	if ( channelArgs->configArgs )
		channelConfig = VXIMapClone (channelArgs->configArgs);
	else
		channelConfig = NULL;

	//TEXT	   key[10 + 1] = "";
	VXIchar *wc_key = NULL;
	VXIchar *wc_val = NULL;
   wc_key = new VXIchar[10 + 1];
   wc_val = new VXIchar[50 + 1];
   size_t  result;

   // Registering HandleStopRequest to handle External Stop Requests
   signal(30, HandleStopRequest);

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "CID:%06d", lChannel);

   /* Set the ANI (Automatic Number Identification) & DNIS (Dialled Number Identification Service) 
		Set ANI as Calling (Origination) Number, and
		Set DNI as Called (Destination) Number */

	platformResult = VXIplatformCreateResources(lChannel, channelConfig, &platform);

	if( 0 != platformResult )
   {
      //gIvrEvrEvent   = EVT_IVR_VXML;
      //gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
      //gIvrEvrReason  = RSN_IVR_VXML_RESOURCE_UNAVAILABLE;
      //GenerateEvr(lChannel, channelArgs->vxmlReqMsg.refNum);
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "CID:%06d Result-%d Error Creating Platform Resource. Exiting", lChannel, platformResult);

      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(VXML_12, LOG_MENDATE, lLogText);

		CHECK_RESULT_NO_RETURN(NULL, "VXIplatformCreateResources()", platformResult);
		return platformResult;
	}
	
	platform->VXIprompt->properties = VXIMapCreate();

	childArgs             = (ChildThreadArgs*)malloc(sizeof(ChildThreadArgs));
	childArgs->channelNum = lChannel;
	childArgs->platform   =	platform;
/*	
	pthread_t	lThreadId;
	UINT32		lRetVal;

   pthread_mutex_init(&childArgs->count_mutex, NULL);

	lRetVal = pthread_create(&lThreadId, NULL, ReadExternalRequest, (void*)childArgs);
	
	if (lRetVal == -1)
	{
		TERR(gTrace, printf ("%s: Failed to Create External Request Thread. Exiting...\n", gProcessName););
		return -1;
	}
//	pthread_detach (lProvThreadId);
*/

	while(IvrUtil::KeepRunning ())
	{
		memset(&channelArgs->vxmlReqMsg, 0, sizeof(channelArgs->vxmlReqMsg));
		if(true == gIvrQueApi.ReadFromVxmlChildReqQue(channelArgs->vxmlReqMsg, lChannel))
		{
         snprintf(lVxmlFullUrl, MAX_PATH, "%s/%s", gXmlPath, channelArgs->vxmlReqMsg.vxmlUrl);
         snprintf(gCallingNumber, MAX_MSISDN_LEN, "%s", channelArgs->vxmlReqMsg.origAddr);
         snprintf(gCalledNumber, MAX_MSISDN_LEN, "%s",  channelArgs->vxmlReqMsg.destAddr);

			snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d EVT-%s OA-%s DA-%s URL-%s", channelArgs->vxmlReqMsg.refNum, 
					channelArgs->vxmlReqMsg.channelId,
					IvrUtil::ConvertVxmlEventToString(channelArgs->vxmlReqMsg.vxmlEvent),
               channelArgs->vxmlReqMsg.origAddr,
               channelArgs->vxmlReqMsg.destAddr,
               lVxmlFullUrl);
               //channelArgs->vxmlReqMsg.vxmlUrl);

         memset(&gVxResMsg, 0, sizeof(gVxResMsg));
         snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Request Received", lLogText);
         T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
         gLog.GenerateLog(VXML_06, LOG_MENDATE, gLogText);

         if(EVT_VX_URL == channelArgs->vxmlReqMsg.vxmlEvent)
         {
            gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_URL_EVT_RCV);
            if( vxmlURL ) 
               VXIStringDestroy(&vxmlURL);

            //vxmlURLptr = channelArgs->vxmlReqMsg.vxmlUrl;
            vxmlURLptr = lVxmlFullUrl;

            len = strlen(vxmlURLptr);
            tempstr = (VXIchar*)malloc(sizeof(VXIchar)*(len+1));

            if( !tempstr )
            {
               gIvrEvrEvent   = EVT_IVR_VXML;
               gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
               gIvrEvrReason  = RSN_IVR_VXML_RESOURCE_UNAVAILABLE;
               GenerateEvr(lChannel, channelArgs->vxmlReqMsg.refNum);
               snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Call Failed - System memory exhausted!", lLogText);
               TERR(gTrace, printf("%s: %s\n", gProcessName, gLogText););
               gLog.GenerateLog(VXML_17, LOG_LEVEL_1, gLogText);
               gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_CALL_FAILED);
            }
            else
            {
               if ( char2wchar(tempstr, vxmlURLptr, len) != 0 )
               {
                  TERR(gTrace, printf("%s: %s Unable to convert char to wchar_t\n", gProcessName, lLogText););
               }
               VXIclientDiag(platform, 60001, L"testClient::ChannelThread", L"In a Call");
               {
                  vxmlURL = VXIStringCreate(tempstr);

                  FREE(tempstr);

                  channelArgs->vxmlURL = VXIStringCStr(vxmlURL);
                  gReferenceNum  = channelArgs->vxmlReqMsg.refNum;

                  memset(&gVxResMsg, 0, sizeof(gVxResMsg));
                  gVxResMsg.channelId = lChannel; 
                  gVxResMsg.refNum    = gReferenceNum;

                  gNewCall = true;

                  snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s New Call/Play Request Received", lLogText);
                  T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
                  if(gAllLogsReqd)
                     gLog.GenerateLog(VXML_14, LOG_LEVEL_2, gLogText);

                  /*
                  // Setting TSS Input Variables for Prompt API.
                  for(int i = 0; i < channelArgs->vxmlReqMsg.varCount; i++)
                  {
                  sprintf(key, "tss_var%d", i);

                  //if(wc_key)
                  //   delete wc_key;
                  //if(wc_val)
                  //   delete wc_val;

                  //wc_key = new VXIchar[10 + 1];
                  result = mbstowcs(wc_key, key, 11);

                  //wc_val = new VXIchar[50 + 1];
                  result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.var[i], 51);

                  VXIMapSetProperty(platform->VXIprompt->properties, wc_key, (VXIValue *) VXIStringCreate(wc_val));
                  }
                  */

                  VXIchar *sessionArgScript = NULL;
                  VXIValue *lpResult = NULL ;

                  /* Put the input/output map into the session ECMAScript so they will be
                     referenced as session.input.* and session.output.*. */
                  VXIchar tempScript[STRINGLEN];
                  memset(tempScript, 0, sizeof(tempScript));
                  // create application object //
                  AppendStringAlloc(&sessionArgScript, L"var application = new Object();\n");

                  result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.vxmlAppl, MAX_URL_LEN);
                  SWIswprintf(tempScript, STRINGLEN, L"application.name = '%s';\n", wc_val);
                  AppendStringAlloc(&sessionArgScript, tempScript);

                  TEXT lApplPath[MAX_URL_LEN] = "";
                  snprintf(lApplPath, MAX_URL_LEN, "%s/%s", gXmlPath, channelArgs->vxmlReqMsg.vxmlAppl);
                  result = mbstowcs(wc_val, lApplPath, MAX_URL_LEN);
                  SWIswprintf(tempScript, STRINGLEN, L"application.path = '%s';\n", wc_val);
                  AppendStringAlloc(&sessionArgScript, tempScript);

                  result = mbstowcs(wc_val, gGlobalResourcePath, MAX_PATH);
                  SWIswprintf(tempScript, STRINGLEN, L"application.globalpath = '%s';\n", wc_val);
                  AppendStringAlloc(&sessionArgScript, tempScript);

                  TEXT lRefNum[MAX_URL_LEN] = "";
                  snprintf(lRefNum, MAX_URL_LEN, "%08X", channelArgs->vxmlReqMsg.refNum);
                  result = mbstowcs(wc_val, lRefNum, MAX_URL_LEN);
                  SWIswprintf(tempScript, STRINGLEN, L"application.callReference = '%s';\n", wc_val);
                  AppendStringAlloc(&sessionArgScript, tempScript);

                  // Added Last Record FileName in session.application.lastrecordedfile
                  //SWIswprintf(tempScript, STRINGLEN, L"application.lastrecordedfile = '';\n");
                  //AppendStringAlloc(&sessionArgScript, tempScript);

                  // create input object //
                  AppendStringAlloc(&sessionArgScript, L"var input = new Object();\n");

                  SWIswprintf(tempScript, STRINGLEN, L"input.scheduleId = '%d';\n", channelArgs->vxmlReqMsg.scheduleId);
                  AppendStringAlloc(&sessionArgScript, tempScript);

						//wc_val = new VXIchar[50 + 1];  // Value Length : 50 + 1 NULL char
                  SWIswprintf(tempScript, STRINGLEN, L"input.varCount = '%d';\n", channelArgs->vxmlReqMsg.varCount);
                  AppendStringAlloc(&sessionArgScript, tempScript);

						if(strlen(channelArgs->vxmlReqMsg.var[0]) > 0)
						{
							result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.var[0], 51);
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar1 = '%s';\n", wc_val);
							AppendStringAlloc(&sessionArgScript, tempScript);
						}
                  else
                  {
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar1 = '';\n");
							AppendStringAlloc(&sessionArgScript, tempScript);
                  }
						if(strlen(channelArgs->vxmlReqMsg.var[1]) > 0)
						{
							result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.var[1], 51);
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar2 = '%s';\n", wc_val);
							AppendStringAlloc(&sessionArgScript, tempScript);
						}
                  else
                  {
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar2 = '';\n");
							AppendStringAlloc(&sessionArgScript, tempScript);
                  }
						if(strlen(channelArgs->vxmlReqMsg.var[2]) > 0)
						{
							result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.var[2], 51);
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar3 = '%s';\n", wc_val);
							AppendStringAlloc(&sessionArgScript, tempScript);
						}
                  else
                  {
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar3 = '';\n");
							AppendStringAlloc(&sessionArgScript, tempScript);
                  }
						if(strlen(channelArgs->vxmlReqMsg.var[3]) > 0)
						{
							result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.var[3], 51);
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar4 = '%s';\n", wc_val);
							AppendStringAlloc(&sessionArgScript, tempScript);
						}
                  else
                  {
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar4 = '';\n");
							AppendStringAlloc(&sessionArgScript, tempScript);
                  }
						if(strlen(channelArgs->vxmlReqMsg.var[4]) > 0)
						{
							result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.var[4], 51);
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar5 = '%s';\n", wc_val);
							AppendStringAlloc(&sessionArgScript, tempScript);
						}
                  else
                  {
							SWIswprintf(tempScript, STRINGLEN, L"input.tssvar5 = '';\n");
							AppendStringAlloc(&sessionArgScript, tempScript);
                  }
						/*
							for(int i = 0; i < channelArgs->vxmlReqMsg.varCount; i++)
							{
							SWIswprintf(tempScript, STRINGLEN,
							L"input.tssvar%d = '%s';\n", (i+1), channelArgs->vxmlReqMsg.var[i]);

							AppendStringAlloc(&sessionArgScript, tempScript);
							}
						 */
						AppendStringAlloc(&sessionArgScript, L"var output = new Object();\n"
								L"output.tssvar1 = '';\n"
								L"output.tssvar2 = '';\n"
								L"output.tssvar3 = '';\n" 
								L"output.tssvar4 = '';\n"
								L"output.tssvar5 = '';\n");

						/* local uri */
                  //TBR
						AppendStringAlloc(&sessionArgScript, platform->connectionPropScript);

                  //printf("###############################################\n");
                  //printf("LOCAL  URI : %s\n", channelArgs->vxmlReqMsg.origAddr);
                  //printf("REMOTE URI : %s\n", channelArgs->vxmlReqMsg.destAddr);
                  //printf("###############################################\n");

						result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.origAddr, (MAX_MSISDN_LEN + 1));

						SWIswprintf(tempScript, STRINGLEN, L"connection.local = new Object();\n"
								L"connection.local.uri = '%s';\n", wc_val);
						//AppendStringAlloc(&platform->connectionPropScript, tempScript);
						AppendStringAlloc(&sessionArgScript, tempScript);

                  // Originator
						SWIswprintf(tempScript, STRINGLEN, L"connection.originator = '%s';\n", wc_val);
						//AppendStringAlloc(&platform->connectionPropScript, tempScript);
						AppendStringAlloc(&sessionArgScript, tempScript);

						// remote uri
						result = mbstowcs(wc_val, channelArgs->vxmlReqMsg.destAddr, (MAX_MSISDN_LEN + 1));

						SWIswprintf(tempScript, STRINGLEN, L"connection.remote = new Object();\n"
								L"connection.remote.uri = '%s';\n", wc_val);
						//AppendStringAlloc(&platform->connectionPropScript, tempScript);
						AppendStringAlloc(&sessionArgScript, tempScript);

						//Peg the Process Document Event
                  snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Invoking VXML Parser", lLogText);
						T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
						if(gAllLogsReqd)
							gLog.GenerateLog(VXML_15, LOG_LEVEL_3, gLogText);
						gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_PARSER_INVOKED);

						// Call the VXIplatformProcessDocument to parse the VXML file from given URL
						platformResult = VXIplatformProcessDocument(channelArgs->vxmlURL,
								&sessionArgScript,
								&lpResult, platform);

						gVxResMsg.result = platformResult;

                  strcpy(gVxResMsg.dtmf, PromptApi::mDtmf);
						snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d EVT-%s Result-%d DTMF:%s", 
                  channelArgs->vxmlReqMsg.refNum,
                  channelArgs->vxmlReqMsg.channelId,
					   IvrUtil::ConvertVxmlEventToString(channelArgs->vxmlReqMsg.vxmlEvent),
					   platformResult,
                  gVxResMsg.dtmf);

						T(gTrace, printf("%s: %s VXML Document Parsing Over\n", gProcessName, lLogText);); 
						//gIvrQueApi.WriteToVxmlRspQue (gVxResMsg, lChannel);
						gIvrQueApi.WriteToEventChildQue (gVxResMsg, lChannel);

						if((VXIint) platformResult != 0)
                  {
                     gIvrEvrEvent   = EVT_IVR_VXML;
                     gIvrEvrCause   = CAS_IVR_CALL_REJECTED;
                     gIvrEvrReason  = RSN_IVR_VXML_INVALID_DOC;
                     GenerateEvr(channelArgs->vxmlReqMsg.channelId, channelArgs->vxmlReqMsg.refNum);
                     snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Call Failed Error - ProcessDocument returned error code", lLogText);
                     TERR(gTrace, printf("%s: %s\n",gProcessName, gLogText););
                     gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_CALL_FAILED);
							gLog.GenerateLog(VXML_16, LOG_MENDATE, gLogText);
						}
						else
						{
                     gIvrEvrEvent   = EVT_IVR_VXML;
                     gIvrEvrCause   = CAS_IVR_CALL_COMPLETE;
                     gIvrEvrReason  = RSN_IVR_NONE;
                     GenerateEvr(channelArgs->vxmlReqMsg.channelId, channelArgs->vxmlReqMsg.refNum);
							// Peg the Call Complete Event
							gIvrQueApi.WriteToIvrPegQueue(PEG_VXML_CALL_COMPLETE);
							gLog.GenerateLog(VXML_19, LOG_MENDATE, lLogText);
						}

                  if(lpResult)
                  {
                     //ShowResult(lpResult, channelArgs->channelNum, platform);
                     VXIValueDestroy(&lpResult);  /* Notes: must deallocate result */
                  }
                  else
                  {
                     VXIclientDiag(platform, 60001, L"testClient::ChannelThread", L"NULL result");
                  }

						/* FREE session ECMAScript */
						if( sessionArgScript )
						{
							FREE(sessionArgScript);
							sessionArgScript = NULL;
						}
					}
				}
				VXIclientDiag(platform, 60001, L"testClient::ChannelThread", L"Call Terminated");
				T(gTrace, printf("%s: %s Call Terminated.\n", gProcessName, lLogText););
            memset(&PromptApi::mDtmf, '\0', sizeof(PromptApi::mDtmf));
            VXIpromptResult promptResult;
            promptResult = childArgs->platform->VXIprompt->BeginSession(childArgs->platform->VXIprompt, NULL);

				gNewCall = false;
			}
			else
         {
            snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Unknown Request, Discarding", lLogText);
				T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
            gLog.GenerateLog(VXML_13, LOG_OPTIONAL, gLogText);
			}
		}
	}

   if(platform->VXIprompt->properties)
      VXIMapDestroy(&platform->VXIprompt->properties);

	platformResult = VXIplatformDestroyResources(&platform);

	//	if( delayTimer ) VXItrdTimerDestroy(&delayTimer);

   if(wc_key)
      delete wc_key;
   if(wc_val)
      delete wc_val;

//   if(tempstr)
//      FREE(tempstr);

   if( vxmlURL ) 
      VXIStringDestroy(&vxmlURL);

	if (channelConfig)
		VXIMapDestroy(&channelConfig);


	if( platformResult != 0 )
	{
		CHECK_RESULT_NO_RETURN(platform, "VXIplatformDestroyResources()", platformResult);
		return platformResult;
	}

	T(gTrace, printf("%s: Channel Process Exiting...\n", gProcessName););
	gLog.GenerateLog(VXML_20);

	return 0;
}


//-------------------------------------------------------------
// METHOD      :
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
int main(int argc, char **argv)
{
	ProcessLock lProcessLock;
	// VXML
	VXIplatformResult pfResult;
	VXIMap           *configArgs = NULL;
	char             *cfgURL     = NULL;
	int               nbChannels = 1;
	int               maxCalls   = 1;
	int               nbSeconds  = 0;
	TEXT              lLogText[MAX_LOG_TEXT_LEN+1];
	//VXIString      *vxmlURL    = NULL;
   BOOLEAN           lIsMain = true;

	strcpy (gProcessName, basename (argv[0]));

	if (argc != 3)
	{
		printf ("Usage %s <instance number> <NumOfChannels> \n", gProcessName);
		printf ("Trace: export %s=1\n", TRACE_IVR_VXML_ENV);
      //printf ("Version: %s", id);
		return 1;
	}
	else
	{
		//memset(argv[0], 0, strlen(argv[0]));
		//sprintf(argv[0], "IVR_VXI_P%02d", atoi(argv[1]));
		//strcpy(gProcessName, argv[0]);
		sprintf(gProcessName, "IVR_VP%d", atoi(argv[1]));
		nbChannels = atoi(argv[2]);
	}

	IvrUtil::LoadSignals ();

	// setting name of executable for log
	gLog.SetProcessName (gProcessName);

	printf ("%s: Starting...\n", gProcessName);

	// Initialize log object
	if (true != gLog.Init ())
	{
		printf("%s:\33[32m Failed to initialize Log Obj. Exiting...\33[0m\n", gProcessName);
		return 1;
	}

	lProcessLock.SetProdProcName ((TEXT*)IVR_NAME, gProcessName);
	if (LOCK_SUCCESS != lProcessLock.Lock ())
	{
		gLog.GenerateLog (GSYS16);
		printf ("%s: Process already running. Exiting...\n", gProcessName);
		return 1;
	}

	// Startup log
	gLog.GenerateLog (GSYS01);

	// Set Trace Key
	gTrace.SetTraceKey (TRACE_IVR_VXML_ENV);

//	if (false == gHealthCheckObj.Init (IVR_VXML_PROC))
//	{
//		printf ("%s: Health Check Object Init Failed\n", gProcessName);
//		return -1;
//	}

	if (false == Init ())
		return 1;

	/* Creating an object of Prompt API , which will be utilized by Prompt library to 
		interface with the underlying media */
	//if (false == TSSInitPromptApi())
	{
		// Proper Error code has to be returned - TBD
		// return VXIplatform_RESULT_NOT_INITIALIZED;
	}

	const char* lProdHome = getenv("PRODUCT_HOME");
	const char* lCfgPath = getenv("PRODUCT_CFG_PATH");

   TEXT *lpTempPtr;

   lpTempPtr = getenv("IVR_XML_PATH");
   if (NULLPTR == lpTempPtr)
      strncpy (gXmlPath, IVR_XML_PATH, MAX_PATH);
   else
      strncpy (gXmlPath, lpTempPtr, MAX_PATH);

   lpTempPtr = getenv("GLOBAL_RESOURCE_PATH");
   if (NULLPTR == lpTempPtr)
      strncpy (gGlobalResourcePath, IVR_GLOBAL_RESOURCE_PATH, MAX_PATH);
   else
      strncpy (gGlobalResourcePath, lpTempPtr, MAX_PATH);

	cfgURL = (char*)malloc
		(sizeof(char)*(strlen(DEFAULT_CONFIG_FILENAME)+strlen(lProdHome)+strlen(lCfgPath)+10));

	sprintf(cfgURL, "%s%s/%s", lProdHome, lCfgPath, DEFAULT_CONFIG_FILENAME);
	T(gTrace, printf ("%s: Default Configuration URL: %s\n", gProcessName, cfgURL););

	/* Parse the configuration file */
	pfResult = ParseConfigFile(&configArgs, cfgURL);
	CHECK_RESULT_RETURN(NULL, "ParseConfigFile()", pfResult);

	//pfResult = VXIplatformInit(configArgs, (VXIunsigned*)&nbChannels);
	//CHECK_RESULT_RETURN(NULL, "VXIplatformInit()", pfResult);

	gLog.GenerateLog (GSYS03);
   printf("%s:\33[32m Initialization Okay...\33[0m\n",gProcessName);

	if (nbChannels > 0) 
	{
		channelArgs = (ChannelThreadArgs *)malloc(sizeof(ChannelThreadArgs));
		CHECK_MEMALLOC_RETURN(NULL, channelArgs, "ChannelThreadArgs array");

		memset(gArrChildPid, 0, sizeof(gArrChildPid));

		for (int i = 0; i < nbChannels ; i++) 
		{
			channelArgs->channelNum = (i+1);
			channelArgs->maxCalls = maxCalls;
			channelArgs->configArgs = configArgs;
			channelArgs->delayCallSec = nbSeconds;
			//channelArgs->vxmlURL = VXIStringCStr(vxmlURL);
			//gReferenceNum = gVxReqMsg.refNum;

			int lChdId;
			lChdId = fork();
			snprintf(lLogText, MAX_LOG_TEXT_LEN, "CID:%06d PID:%d", channelArgs->channelNum, lChdId);
			if(lChdId < 0)
			{
            snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s VP Channel Process Creation -Failed", lLogText);
				TERR(gTrace, printf("%s: %s\n", gProcessName, gLogText););
				gLog.GenerateLog(VXML_02, LOG_LEVEL_1, gLogText);
				gArrChildPid[i] = lChdId;
			}
			else if(lChdId > 0)
			{
            snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s VP Channel Process Created", lLogText);
				T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
				gLog.GenerateLog(VXML_01, LOG_LEVEL_1, gLogText);
				gArrChildPid[i] = lChdId;
            usleep(100);
			}
			else
         {
            lIsMain = false;
            int lNumOfChannels=1;
            pfResult = VXIplatformInit(configArgs, (VXIunsigned*)&lNumOfChannels);
            CHECK_RESULT_RETURN(NULL, "VXIplatformInit()", pfResult);

				//memset(argv[0], 0, strlen(argv[0]));
				memset(argv[1], 0, strlen(argv[1]));
				//memset(argv[2], 0, strlen(argv[1]));
				sprintf(argv[1], "%04d", channelArgs->channelNum);
				//sprintf(argv[0], "VP-%04d", channelArgs->channelNum);
				//strcpy(gProcessName, argv[0]);
				sprintf(gProcessName, "VP-%04d", channelArgs->channelNum);
				gLog.SetProcessName(gProcessName);
				ExecuteChannelFunc(channelArgs);

            // Shut down the platform 
            pfResult = VXIplatformShutdown();
            CHECK_RESULT_NO_RETURN(NULL, "VXIplatformShutdown()", pfResult);

            if (pfResult == VXIplatform_RESULT_SUCCESS)
            {
               T(gTrace, printf("%s: Successfully exiting\n", gProcessName););
            }
            else
            {
               TERR(gTrace, printf("%s: Exiting with errors, rc = %d\n", gProcessName, pfResult););
            }

            if( configArgs ) VXIMapDestroy(&configArgs);
            if( channelArgs ) FREE(channelArgs);
            if(cfgURL) FREE(cfgURL);
            exit(0);
			}
		}
      
      if(lIsMain)
      {
         while (IvrUtil::KeepRunning ())
         {
            if (IvrUtil::ReloadConfig ())
            {
               //ReloadConfig ();
               IvrUtil::ResetConfigFlag ();
            }
            if (true == gIvrQueApi.ReadFromVxmlReqQue (gVxReqMsg))
            {
               snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d EVT-%s", gVxReqMsg.refNum, 
                     gVxReqMsg.channelId,
                     IvrUtil::ConvertVxmlEventToString(gVxReqMsg.vxmlEvent));

               snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Request Received", lLogText);
               T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
               gLog.GenerateLog(VXML_06, LOG_MENDATE, gLogText);

               if(EVT_VX_TERM == gVxReqMsg.vxmlEvent)
               {
                  snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Sending Stop Signal to Channel Process", lLogText);
                  T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
                  if(gAllLogsReqd)
                     gLog.GenerateLog(VXML_07, LOG_MENDATE, gLogText);
                  kill(gArrChildPid[gVxReqMsg.channelId -1], 30);
               }
               else
               {
                  gIvrEvrEvent   = EVT_IVR_VXML;
                  gIvrEvrCause   = CAS_IVR_CALL_RECEIVED;
                  gIvrEvrReason  = RSN_IVR_NONE;
                  GenerateEvr(gVxReqMsg.channelId, gVxReqMsg.refNum);

                  snprintf(gLogText, MAX_LOG_TEXT_LEN, "%s Request Forwarding to Channel Process", lLogText);
                  T(gTrace, printf("%s: %s\n", gProcessName, gLogText););
                  if(gAllLogsReqd)
                     gLog.GenerateLog(VXML_08, LOG_LEVEL_1, gLogText);
                  gIvrQueApi.WriteToVxmlChildReqQue(gVxReqMsg);
               }
            }
         }// end of while loop (main process)
      }
   }
   else 
   {
      T(gTrace, printf("%s: ERROR - No channels available.\n", gProcessName););
      gLog.GenerateLog(VXML_04);
   }

	T(gTrace, printf("%s: Shutting Down the VXML Platform\n", gProcessName););
	gLog.GenerateLog(VXML_05);

	// Shut down the platform 
	//pfResult = VXIplatformShutdown();
	//CHECK_RESULT_NO_RETURN(NULL, "VXIplatformShutdown()", pfResult);

	// clean up memory 
	//if( vxmlURL ) VXIStringDestroy(&vxmlURL);
   
	if( configArgs ) VXIMapDestroy(&configArgs);
	if( channelArgs ) FREE(channelArgs);
   if(cfgURL) FREE(cfgURL);

	printf ("%s: Shutting down...\n", gProcessName);
	gLog.GenerateLog (GSYS02);

	return (int)pfResult;
}
