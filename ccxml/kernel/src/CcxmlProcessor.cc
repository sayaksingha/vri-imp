static const char* id="@(#) $Id: CcxmlProcessor.cc,v 1.1.1.1.2.6.2.2 2016/05/12 22:16:17 mram Exp $";
//----------------------------------------------------------------------
// NAME
// CcxmlProcessorMain.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright (C) 2005
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated: Divya K                        Date: DEC-2010
//----------------------------------------------------------------------

#include "CcxmlProcessor.h"

UINT32   	gChannelId;
UINT32   	gSessionlId;
TEXT   		gLogText[MAX_LOG_TEXT_LEN + 1] = "";
IvrQueApi	gIvrQueApi;

extern INT32  gReferenceNum;
//-------------------------------------------------------------
// METHOD      :Constructor 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
CcxmlProcessor::CcxmlProcessor()
{
}

//-------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
CcxmlProcessor::~CcxmlProcessor()
{
}

//-------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN CcxmlProcessor::ReadConfig ()
{
   TEXT*    lenv;
   lenv     = new TEXT[100];
   //mXmlPath = new TEXT[100];

   lenv = getenv("XML_FILE_PATH");

   if(lenv != NULL)
   {
      strcpy(mXmlPath, lenv);
      T(gTrace, 
          printf("%s: XML_FILE_PATH = %s\n", gProcessName, mXmlPath););
   }
   else
   {
      snprintf( gLogText, MAX_LOG_TEXT_LEN, 
                "Environment variable XML_FILE_PATH not set");
      TERR( gTrace, 
            printf("%s: %s\n", gProcessName, gLogText););
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
BOOLEAN CcxmlProcessor::Init ()
{
	memset (gLogText, 0, sizeof (gLogText));

   if(false == ReadConfig ())
   {
		gLog.GenerateLog (GSYS04, gLogText);
      return false;
   }

	if(false == gIvrQueApi.Init())
	{
		snprintf (gLogText, MAX_LOG_TEXT_LEN,
				"IvrQueueApi Init failed. Exiting...");
		gLog.GenerateLog (GSYS04, gLogText);
		TERR (gTrace, 
				printf ("%s: %s\n", gProcessName, gLogText););
		return false;
	}

	return true;
}

//-------------------------------------------------------------
// METHOD      : ReadRequest 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN CcxmlProcessor::ReadRequest (CcxmlRequest& lccxmlrequest)
{
	memset(&mCcxmlRequest, 0, sizeof(CcxmlRequest));
	if(false == gIvrQueApi.ReadFromCcxmlReqQue(lccxmlrequest, 0))
	{
	   return false;
	}

	gChannelId                    = lccxmlrequest.channelId;
	gSessionlId                   = lccxmlrequest.sessionId;
	mCcxmlRequest.ccxmlUrlRequest = lccxmlrequest.ccxmlUrlRequest;
	mCcxmlRequest.channelId       = lccxmlrequest.channelId;
	mCcxmlRequest.sessionId       = lccxmlrequest.sessionId;
	mCcxmlRequest.refNum          = lccxmlrequest.refNum; 

	gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_REQ_RCV);
	return true;
}

//-------------------------------------------------------------
// METHOD      : ReadEvent
// DESCRIPTION : Reads Events from the Event Queue
// PARAMETER   : 
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN CcxmlProcessor::ReadEvent (CcxmlRequest& lccxmlrequest, INT16& lMsgType)
{
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	if(false == gIvrQueApi.ReadFromCcxmlEventQue(lccxmlrequest, lMsgType))
		return false;

		if(gReferenceNum != lccxmlrequest.refNum)
		{
			snprintf(lLogText, MAX_LOG_TEXT_LEN, 
					   "RefNum:%08X CID:%d OldRefNum:%08X SID:%d OA:%s DA:%s EVT:%s Clearing Old CCXML Event Message",
					   gReferenceNum,
    					lccxmlrequest.channelId, 
                  lccxmlrequest.refNum,
    					lccxmlrequest.sessionId, 
                  lccxmlrequest.ccxmlEventRequest.origAddr,
                  lccxmlrequest.ccxmlEventRequest.destAddr,
	    				lccxmlrequest.ccxmlEventRequest.event);
			gLog.GenerateLog(CCXML_IVR22, lLogText);
			T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		   return false;
		}

	gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_EVENT_RCV);
	switch(IvrUtil::GetEventNumber(lccxmlrequest.ccxmlEventRequest.event))
	{
		case EVT_CC_ALERTING:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_ALERT_EVT_RCV);
			break;
		case EVT_CC_CONNECTED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_CONNECTED_EVT_RCV);
			break;
		case EVT_CC_DISCONNECTED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_DISCONNECTED_EVT_RCV);
			break;
		case EVT_CC_ERROR:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_ERROR_EVT_RCV);
			break;
		case EVT_CC_FAILED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_FAILED_EVT_RCV);
			break;
		case EVT_CC_PROGRESSING:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_PROGRESS_EVT_RCV);
			break;
		case EVT_CC_REDIRECTED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_REDIRECTED_EVT_RCV);
			break;
		case EVT_CC_SIGNAL:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONN_SIGNAL_EVT_RCV);
			break;

		case EVT_CC_DLG_EXIT:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_DIALOG_EXITT_EVT_RCV);
			break;
		case EVT_CC_DLG_DISCONNECTED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_DIALOG_DISCONNECTED_EVT_RCV);
			break;
		case EVT_CC_DLG_PREPARED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_DIALOG_PREPARED_EVT_RCV);
			break;
		case EVT_CC_DLG_STARTED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_DIALOG_STARTED_EVT_RCV);
			break;
		case EVT_CC_DLG_TRANSFER:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_DIALOG_TRANSFER_EVT_RCV);
			break;

		case EVT_CC_CONF_CREATED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONF_CREATED_EVT_RCV);
			break;
		case EVT_CC_CONF_DESTROYED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONF_DESTROYED_EVT_RCV);
			break;
		case EVT_CC_CONF_JOINED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONF_JOINED_EVT_RCV);
			break;
		case EVT_CC_CONF_UNJOINED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CONF_UNJOINED_EVT_RCV);
			break;

		case EVT_CC_CCXML_CREATED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_CREATED_EVT_RCV);
			break;
		/*case EVT_CC_CCXML_JOINED:
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_JOINED_EVT_RCV);
			break;*/
		default:
			T(gTrace, printf("%s: UnKnown Event Received\n", gProcessName););
			break;

	gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_EVENT_RCV);
	}
	return true;
}

//-------------------------------------------------------------
// METHOD      : ProcessReq() 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//-------------------------------------------------------------
BOOLEAN CcxmlProcessor::ProcessReq()
{
	CcxmlRequest   lccxmlrequest;
   INT16          lMType = gChannelId;
   //INT16          lMType = gSessionlId;
   TEXT           lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	while (IvrUtil::KeepRunning ())
	{
		if (IvrUtil::ReloadConfig ())
		{
			IvrUtil::ResetConfigFlag ();
		}

		memset(&lccxmlrequest, 0, sizeof(CcxmlRequest));

		if (true == ReadEvent (lccxmlrequest, lMType))
		{
			snprintf(lLogText, MAX_LOG_TEXT_LEN, 
					   "RefNum:%08X CID:%d SID:%d OA:%s DA:%s EVT:%s Posted the Event",
                  lccxmlrequest.refNum,
    					lccxmlrequest.channelId, lccxmlrequest.sessionId, 
                  lccxmlrequest.ccxmlEventRequest.origAddr,
                  lccxmlrequest.ccxmlEventRequest.destAddr,
	    				lccxmlrequest.ccxmlEventRequest.event);
			mclient->PostEvent(lccxmlrequest);
			gLog.GenerateLog(CCXML_IVR09, 2, lLogText);
			T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		}
	}
	return true;
}

//-------------------------------------------------------------
// METHOD      : BuildRequest 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//-------------------------------------------------------------
BOOLEAN CcxmlProcessor::BuildRequest(TEXT* uri)
{
   TEXT     lUrl[100] = "";

   sprintf(lUrl, "%s/%s", mXmlPath, uri);
	//if(false == CreateChild(uri))
	if(false == CreateChild(lUrl))
		return false;
	
	return true;
}

//-------------------------------------------------------------
// METHOD      : SendNackRspToSmgr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//-------------------------------------------------------------
BOOLEAN CcxmlProcessor::SendNackRspToSmgr(CcxmlRequest& lCcxmlRequest,EnumExitReason  rsn)
{
	CcxmlResponse   lCcxmlResponse;
   INT16           lMType = gChannelId;
	TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	memset(&lCcxmlResponse, 0, sizeof(CcxmlResponse));

   lCcxmlResponse.ccxmlMsgType = CCXML_DISCONNECT;
   sprintf(lCcxmlResponse.conId,"%d", lCcxmlRequest.channelId);
   sprintf(lCcxmlResponse.sessionId, "%d", lCcxmlRequest.sessionId);
   lCcxmlResponse.exit.reason  = rsn;
	lCcxmlResponse.channelId = lCcxmlRequest.channelId;
   lCcxmlResponse.refNum = gReferenceNum;

   if(rsn != RSN_BUILD_CLIENT_FAILURE)
      ReadEvent (lCcxmlRequest, lMType);

	//if(false == gIvrQueApi.WriteToCcxmlRspQue(lCcxmlResponse, gChannelId))
	if(false == gIvrQueApi.WriteToEventChildQue(lCcxmlResponse, gChannelId))
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d (%s) SID:%s Rsn:%d",
				     lCcxmlResponse.refNum, 
                 lCcxmlResponse.channelId,
                 lCcxmlResponse.conId, 
					  lCcxmlResponse.sessionId, lCcxmlResponse.exit.reason);
		TERR(gTrace, 
				printf("%s: %s Failed to send the NACK Response to Session Manager\n", 
					gProcessName, lLogText););
		return false;
	}
	snprintf(lLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d SID:%s Rsn:%d",
				lCcxmlResponse.refNum, lCcxmlResponse.channelId, 
				lCcxmlResponse.sessionId, lCcxmlResponse.exit.reason);
	T(gTrace, printf("%s: %s Sent NACK Response to Session Manager successfull\n",
				gProcessName, lLogText););
	return true;
}

//-------------------------------------------------------------
// METHOD      : CreateChild
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//-------------------------------------------------------------
BOOLEAN CcxmlProcessor::CreateChild(TEXT* uri)
{
	TEXT    lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	TEXT    glLogText[MAX_LOG_TEXT_LEN + 1] = "";
	INT32   lChildPid;
	//CcxmlResponse   lCcxmlResponse;

	snprintf (mLogText, MAX_LOG_TEXT_LEN, "RefNum:%08X CID:%06d BUri:%s Url:%s SID:%d VarCnt:%d",
			    mCcxmlRequest.refNum, 
				 mCcxmlRequest.channelId, 
             uri, mCcxmlRequest.ccxmlUrlRequest.url, 
             mCcxmlRequest.sessionId,
				 mCcxmlRequest.ccxmlUrlRequest.varCount);

   gReferenceNum = mCcxmlRequest.refNum;
	lChildPid = fork();
	if (lChildPid  < 0)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s Fork Error", mLogText);
      snprintf(glLogText, MAX_LOG_TEXT_LEN, "Error: %s", gLogText);
		gLog.GenerateLog(CCXML_IVR03, glLogText);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return false;
	}
	else if (lChildPid == 0)
	{
		sprintf(gProcessName, "CP-%04d", mCcxmlRequest.sessionId);
		gLog.SetProcessName (gProcessName);
		//strcpy(argv[0], gProcessName);
      //memset(argv[1], 0, strlen(argv[1]));     
      //sprintf(argv[1], "%04d", mCcxmlRequest.sessionId)
      //snprintf(argv[1], "%d", mCcxmlRequest.sessionId);


		snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s Thread Creation success.", mLogText);
		T(gTrace, printf("%s: %s Fork success.\n", 
      					gProcessName, lLogText ););
		gLog.GenerateLog(CCXML_IVR04, 2, lLogText);

		mclient = new OktoClient();
		if(mclient->buildClient(uri, mCcxmlRequest) != OktoInterpreter::RESULT_SUCCESS)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "%s Failed to Build the Client", mLogText);
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(CCXML_IVR05, mLogText);
			SendNackRspToSmgr(mCcxmlRequest, RSN_BUILD_CLIENT_FAILURE);
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_NACK_SND);
			_exit(0);
		}
		else
		{
         sprintf(gProcessName, "CP-%04d", mCcxmlRequest.sessionId);
			//sprintf(gProcessName, "IVR_CCXML_%d", mCcxmlRequest.sessionId);
			snprintf(lLogText, MAX_LOG_TEXT_LEN, "%s Args:%s", 
					   mLogText, mCcxmlRequest.ccxmlUrlRequest.args);
			T(gTrace, printf("%s: %s Success to Build the Client\n", 
						gProcessName, lLogText););
			gLog.GenerateLog(CCXML_IVR06, 1, lLogText);

			/*if(strcmp(lCcxmlRequest.ccxmlUrlRequest.args, "") != 0
					&& strcmp(lCcxmlRequest.ccxmlUrlRequest.args, mCcxmlRequest.sessionId) != 0)*/
			/*memset(&lCcxmlResponse, 0, sizeof(CcxmlResponse));

			lCcxmlResponse.ccxmlMsgType = CCXML_LOADED;
			lCcxmlResponse.refNum       = mCcxmlRequest.refNum;
			sprintf(lCcxmlResponse.conId,"%d", mCcxmlRequest.channelId);
			sprintf(lCcxmlResponse.sessionId, "%d", mCcxmlRequest.sessionId);
			strcpy(lCcxmlResponse.args, mCcxmlRequest.ccxmlUrlRequest.args);
			lCcxmlResponse.channelId = mCcxmlRequest.channelId;

			//gIvrQueApi.WriteToCcxmlRspQue(lCcxmlResponse, gChannelId); //To be uncommented
			gIvrQueApi.WriteToEventChildQue(lCcxmlResponse, gChannelId); //To be uncommented*/
			gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_ACK_SND);
		}

		if (false == ProcessReq())
		{
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               " %s Failed to Process Request", mLogText);
			TERR(gTrace, 
					printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(CCXML_IVR07, mLogText);
		}
		strcat(lLogText, " Child Exiting...");
		T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(CCXML_IVR08, 1, lLogText);

		gIvrQueApi.WriteToIvrPegQueue(PEG_CCXML_EXIT_SND);
		_exit(0);
	}

	return true;
}

