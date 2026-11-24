static const char *id = "@(#) $Id: AcuEventApi.cc,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
// AcuEventApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2012
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Roopa A M                                    31-JUL-2012
//----------------------------------------------------------------------
#include "AcuEventApi.h"

//----------------------------------------------------------------------
// METHOD      : Constructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
AcuEventApi::AcuEventApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
AcuEventApi::~AcuEventApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN AcuEventApi::Init ()
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (false == ReadConfig ())
   {
      strncpy (lLogText, "ReadConfig failed, Exiting ....", MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   if (false == CreateProsodyEvents ())
   {
      strncpy (lLogText, "Prosody Events Creation failed, Exiting ....",
            MAX_LOG_TEXT_LEN);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS04, lLogText);
      return false;
   }

   T (gTrace, printf ("%s:\33[32m AcuEventApi::Initialization ok...\33[0m\n",
            gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN AcuEventApi::ReadConfig ()
{
   return true;
}

//----------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION : Reload configuration parameters
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
BOOLEAN AcuEventApi::ReloadConfig ()
{
   return true;
}

//----------------------------------------------------------------------
// METHOD      : CreateProsodyEvents ()
// DESCRIPTION : Creates Play, record and Dtmf prosody events
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
BOOLEAN AcuEventApi::CreateProsodyEvents ()
{
   INT16 lResult = 0;
   TEXT  lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   lResult = smd_ev_create(&mPlayEvent, kSMNullChannelId,
         kSMEventTypeWriteData, kSMAnyChannelEvent);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error Could not create Prosody Event PLAY <%d>", lResult);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API01, lLogText);
      return false;
   }

   lResult = smd_ev_create(&mRecEvent, kSMNullChannelId,
         kSMEventTypeReadData, kSMAnyChannelEvent);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error Could not create Prosody Event REC <%d>", lResult);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API02, lLogText);
      return false;
   }

   lResult = smd_ev_create(&mDtmfEvent, kSMNullChannelId,
         kSMEventTypeRecog, kSMAnyChannelEvent);
   if (lResult != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "Error Could not create Prosody Event DTMF <%d>", lResult);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ACU_API03, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "\33[32mSuccessfully Created Events "
         "PlayEvt[Fd:%d Mode:%d] RecEvt[Fd:%d Mode:%d] DtmfEvt[Fd:%d Mode:%d]\33[0m",
         mPlayEvent.fd, mPlayEvent.mode, mRecEvent.fd, mRecEvent.mode, 
         mDtmfEvent.fd, mDtmfEvent.mode);
   T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ACU_API04, lLogText);

   return true;
}
