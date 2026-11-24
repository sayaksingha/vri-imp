static const char *id = "@(#) $Id: IsdnConfigApi.cc,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $";
//----------------------------------------------------------------------
// NAME : IsdnConfigApi.cc
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2008
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION:
//              This libreary used for reading Db configurations.
// message.

// Originated: Chethan V M                           23/07/2013
//----------------------------------------------------------------------
#include "IsdnConfigApi.h"

//----------------------------------------------------------------------
// METHOD      : IsdnConfigApi
// DESCRIPTION : Constructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
IsdnConfigApi::IsdnConfigApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : ~IsdnConfigApi
// DESCRIPTION : Destructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
IsdnConfigApi::~IsdnConfigApi ()
{
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnConfigApi::Init ()
{
   if(true != this->ReadConfig ())
   {
      TERR (gTrace, printf ("%s:\33[32m Read Config failed... Exiting\33[0m\n", gProcessName););
      return false;
   }

   T (gTrace, printf ("%s:\33[32m ConfigApi Init ok...\33[0m\n", gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnConfigApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (lCfgRead.CfgInit (ISDN_SYS_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, ISDN_SYS_CFG););
      gLog.GenerateLog (GSYS09, ISDN_SYS_CFG);
      return false;
   }

   lCfgRead.CfgDeInit ();

   if (lCfgRead.CfgInit (ISDN_IPC_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, ISDN_IPC_CFG););
      gLog.GenerateLog (GSYS09, ISDN_IPC_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("SHM_ISDN_CHANNEL_MGMT_KEY", mShmChnlMgmtKey,
   ISDN_MIN_MSG_Q_KEY, ISDN_MAX_MSG_Q_KEY))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
         "Config read failed for %s. File:%s.",
         "SHM_ISDN_CHANNEL_MGMT_KEY", ISDN_IPC_CFG);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         lCfgRead.CfgDeInit ();
         return false;
   }
   else
   {
      T (gTrace, printf ("%s: SHM_ISDN_CHANNEL_MGMT_KEY = %d\n",
               gProcessName,mShmChnlMgmtKey););
   }
   lCfgRead.CfgDeInit ();

   if (lCfgRead.CfgInit (ISDN_MEDIA_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
      gProcessName, ISDN_MEDIA_CFG););
      gLog.GenerateLog (GSYS09, ISDN_MEDIA_CFG);
      return false;
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("DIALOGIC_ISDN_CALL_HANDLE", mHandleIsdnCall,
            0, 1))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "DIALOGIC_ISDN_CALL_HANDLE", ISDN_MEDIA_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: DIALOGIC_ISDN_CALL_HANDLE = %d\n",
               gProcessName, mHandleIsdnCall););
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("MEDIA_SEND_RSP_FLAG", mSendRspFlag,
            0, 1))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MEDIA_SEND_RSP_FLAG", ISDN_MEDIA_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MEDIA_SEND_RSP_FLAG = %d\n",
               gProcessName, mSendRspFlag););
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("DIALOGIC_CHANNEL_MGMT_THREAD", mHandleChnlMgmt,
            0, 1))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "DIALOGIC_CHANNEL_MGMT_THREAD", ISDN_MEDIA_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: DIALOGIC_CHANNEL_MGMT_THREAD = %d\n",
               gProcessName, mHandleChnlMgmt););
   }

   if (CFG_OK != lCfgRead.GetConfigNum ("DIALOGIC_OUTGOING_CALL_WAIT_TIME", mOutgoingCallWaitTime,
            MIN_DIALOGIC_OUTGOING_CALL_WAIT_TIME,
            MAX_DIALOGIC_OUTGOING_CALL_WAIT_TIME))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "DIALOGIC_OUTGOING_CALL_WAIT_TIME", ISDN_MEDIA_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: DIALOGIC_OUTGOING_CALL_WAIT_TIME = %d\n",
               gProcessName, mOutgoingCallWaitTime););
   }

   lCfgRead.CfgDeInit ();

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnConfigApi::ReloadConfig()
{
   return this->ReadConfig();
}

