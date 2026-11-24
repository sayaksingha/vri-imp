static const char *id = "@(#) $Id: HmpEvrApi.cc,v 1.1.2.1 2012/02/21 08:31:33 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpEvrApi.cc
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
#include "HmpEvrApi.h"
#include "HmpUtil.h"
//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
HmpEvrApi::HmpEvrApi ()
{
}

//----------------------------------------------------------------------
// METHOD      :
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpEvrApi::Init ()
{
   if (false == this->ReadConfig ())
   {
      return false;
   }

   if (false == this->CreateMsgQueue ())
   {
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
BOOLEAN HmpEvrApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (lCfgRead.CfgInit (HMP_IPC_CFG) != true)
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN, "Could not open file %s",
            HMP_IPC_CFG);
      gLog.GenerateLog (GSYS09, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   if (lCfgRead.GetConfigNum ("MSG_HMP_EVR_Q_RCV",
            mEvrHdlrQueKey,
            HMP_MIN_MSG_Q_KEY,
            HMP_MAX_MSG_Q_KEY) != CFG_OK)
   {
      snprintf (lLogText,MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_HMP_EVR_Q_RCV", HMP_IPC_CFG);
      gLog.GenerateLog (GSYS11, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MSG_HMP_EVR_Q_RCV = %d\n",
               gProcessName, mEvrHdlrQueKey););
   }
   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateMsgQueue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpEvrApi::CreateMsgQueue ()
{
   msgQueueError lQueErr;
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lQueErr = mEvrHdlrQue.Create (mEvrHdlrQueKey, IPC_CREAT | IPC_PERM);
   if (lQueErr != Q_SUCCESS)
   {
      sprintf (lLogText,
            "Error creating MSG_HMP_EVR_Q_RCV queue with %d",
            mEvrHdlrQueKey);
      gLog.GenerateLog (GSYS06, lLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : GenerateEvr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpEvrApi::GenerateEvr (HmpEvrMsg & rEvrMsg, TEXT * logText)
{
   INT16 lEvrMsgLen = sizeof (rEvrMsg);
   msgQueueError lQueErr = Q_SUCCESS;

   T (gTrace, HmpUtil::DisplayEvr (rEvrMsg););

   lQueErr = mEvrHdlrQue.WriteMsg (&rEvrMsg, lEvrMsgLen, 1, BLOCKING);

   if (lQueErr != Q_SUCCESS)
   {
      lEvrMsgLen = sizeof (rEvrMsg);
      if (true != HmpUtil::HandleQueueWriteError (lQueErr,
               mEvrHdlrQue, &rEvrMsg, lEvrMsgLen, 1, BLOCKING))
      {
         gLog.GenerateLog (HMP_EAPI02, logText);
         TERR (gTrace, printf ("%s: %s: Failed to send evr message\n",
                  gProcessName, logText););

         return false;
      }
   }

   gLog.GenerateLog (HMP_EAPI01, 2, logText);
   T (gTrace, printf ("%s: %s: Evr wrote to evr Queue\n",
            gProcessName, logText););

   return true;
}
//----------------------------------------------------------------------
// METHOD      : GenerateProvEvr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpEvrApi::GenerateProvEvr (EnumHmpEvrEvt evrEvent,
      EnumHmpEvrCause   evrCause,
      TEXT *             msisdn,
      HmpProvEvrMsg &   provEvr)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   HmpEvrMsg lEvrMsg;

   memset (&lEvrMsg, 0, sizeof (lEvrMsg));

   lEvrMsg.evrType = EVR_TYPE_HMP_PROV;
   lEvrMsg.evrEvent = evrEvent;
   lEvrMsg.evrCause = evrCause;

   strcpy (lEvrMsg.msisdn, msisdn);

   lEvrMsg.gentime = time (NULL);

   lEvrMsg.provEvr = provEvr;

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "Msisdn:%s EvrType:%s(%d) EvrEvent:%s(%d) EvrCause:%s(%d) ",
         lEvrMsg.msisdn,
         HmpUtil::ConvertEvrTypeToStr (lEvrMsg.evrType), lEvrMsg.evrType,
         HmpUtil::ConvertEvrEventToStr (lEvrMsg.evrEvent), lEvrMsg.evrEvent,
         HmpUtil::ConvertEvrCauseToStr (lEvrMsg.evrCause), lEvrMsg.evrCause);

   return this->GenerateEvr (lEvrMsg, lLogText);
}
//-------------------------------------------------------------
// METHOD      : ReadEvrMessage
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN HmpEvrApi::ReadEvrMessage (HmpEvrMsg & evrMsg)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lEvrMsgLen = sizeof (HmpEvrMsg);
   msgQueueError lQueErr;

   lQueErr = mEvrHdlrQue.ReadMsg (&evrMsg, lEvrMsgLen, 0, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      TERR (gTrace, printf ("%s: ErrNo:%d  ErrText:%s\n",
               gProcessName, errno, strerror(errno)););
      HmpUtil::HandleQueueReadError (lQueErr, mEvrHdlrQue);
      return false;
   }

   if (0 == evrMsg.evrCause && 0 == evrMsg.evrEvent)
   {
      gHealthCheckObj.WriteMsgToHealthCheck(HC_HEART_BEAT_RSP, HC_INFO);
      return false;
   }

   snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "Msisdn:%s EvrEvent:%s(%d) EvrCause:%s(%d)",
         evrMsg.msisdn,
         HmpUtil::ConvertEvrEventToStr (evrMsg.evrEvent), evrMsg.evrEvent,
         HmpUtil::ConvertEvrCauseToStr (evrMsg.evrCause), evrMsg.evrCause);


   T (gTrace, printf ("%s: %s Evr message read\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_EAPI03, 2, lLogText);

   T (gTrace, HmpUtil::DisplayEvr (evrMsg););

   return true;
}

