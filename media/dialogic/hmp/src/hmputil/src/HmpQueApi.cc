static const char *id = "@(#) $Id: HmpQueApi.cc,v 1.1.2.1 2012/02/21 08:31:34 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpQueApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Hmp                                  23-Nov-2011
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "HmpQueApi.h"
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
HmpQueApi::HmpQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
HmpQueApi::~HmpQueApi ()
{

}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpQueApi::Init ()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   // Read Configuration
   if (false == this->ReadConfig ())
   {
      strncpy (lLogText, "ReadConfig failed, Exiting ....", MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
}

   // Creating Queues
   if (false == this->CreateQueues ())
   {
      strncpy (lLogText, "CreateQueue failed, Exiting ....",
            MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpQueApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);

   // Read message queue key values
   if (lCfgRead.CfgInit (HMP_IPC_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, HMP_IPC_CFG););
      gLog.GenerateLog (GSYS09, HMP_IPC_CFG);
      return false;
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_REQ_Q", mMediaCmdReqQueKey,
            HMP_MIN_MSG_Q_KEY, HMP_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_VOICE_PROCESS_REQ_Q = %d\n",
               gProcessName, mMediaCmdReqQueKey););
   }
   
   if (CFG_OK !=
         lCfgRead.GetConfigNum ("MSG_VOICE_PROCESS_RSP_Q", mMediaCmdRspQueKey,
            HMP_MIN_MSG_Q_KEY, HMP_MAX_MSG_Q_KEY))
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_VOICE_PROCESS_RSP_Q = %d\n",
               gProcessName, mMediaCmdRspQueKey););
   }

   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateQueues
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpQueApi::CreateQueues ()
{
   msgQueueError lQueErr;

   // Creating Cap Handler Queue
   lQueErr = mMediaCmdReqQue.Create (mMediaCmdReqQueKey, IPC_CREAT | IPC_PERM);

   if (Q_SUCCESS != lQueErr)
   {
      HmpUtil::HandleQueueCreateError (lQueErr, mMediaCmdReqQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VOICE_PROCESS_REQ_Q",
               mMediaCmdReqQueKey););
      return false;
   }

   if (Q_SUCCESS != lQueErr)
   {
      HmpUtil::HandleQueueCreateError (lQueErr, mMediaCmdRspQue);
      TERR (gTrace, printf ("%s: Error creating %s queue with %d\n",
               gProcessName, "MSG_VOICE_PROCESS_RSP_Q",
               mMediaCmdRspQueKey););
      return false;
   }

   return true;
}
//----------------------------------------------------------------------
// METHOD      : ReadFromMediaCmdReqQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpQueApi::ReadFromMediaCmdReqQue (MpRequestDetails & vpRequest, INT16 msgType)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (vpRequest);
   msgQueueError lQueErr;

   memset (&vpRequest, 0, sizeof (vpRequest));

   // Read Messages from the Hmp'1x' Queue
   lQueErr = mMediaCmdReqQue.ReadMsg ((void *) &vpRequest, lMsgLen, msgType, BLOCKING);
   if (Q_SUCCESS != lQueErr)
   {
      return HmpUtil::HandleQueueReadError (lQueErr, mMediaCmdReqQue);
   }

   //if (0 == vpRequest.)
   //{
   //   gHealthCheckObj.WriteMsgToHealthCheck (HC_HEART_BEAT_RSP, HC_INFO);
   //   return false;
   //}

   //snprintf (lLogText, MAX_LOG_TEXT_LEN,
   //      "",
   //      vpRequest.);

   gLog.GenerateLog (HMP_QAPI01, 2, lLogText);
   T (gTrace, printf ("%s: %s QueKey:%d Message read from 1x Queue\n",
                     gProcessName, lLogText, mMediaCmdReqQue.GetQueueKey ()););
   
   return true;
}

//----------------------------------------------------------------------
// METHOD      : WriteToMediaCmdRspQue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpQueApi::WriteToMediaCmdRspQue (MpResponseDetails & MpResponse)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lMsgLen = sizeof (MpResponse);
   msgQueueError lQueErr;

   lQueErr = mMediaCmdRspQue.WriteMsg ((void *) &MpResponse, lMsgLen, 1, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (MpResponse);
      if (false ==
            HmpUtil::HandleQueueWriteError (lQueErr, mMediaCmdRspQue,
               &MpResponse, lMsgLen, 1, BLOCKING))
      {
         gLog.GenerateLog (HMP_QAPI03, lLogText);
         TERR (gTrace,
               printf ("%s: %s Write to 1x Queue Failed\n",
                  gProcessName, lLogText););
         return false;
      }
   }

   //snprintf (lLogText, MAX_LOG_TEXT_LEN,
   //      "",
   //      MpResponse.);

   gLog.GenerateLog (HMP_QAPI02, 2, lLogText);
   T (gTrace, printf ("%s: %s Wrote to 1x Queue\n",
            gProcessName, lLogText););

   return true;
}
