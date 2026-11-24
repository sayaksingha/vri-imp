static const char *id = "@(#) $Id: HmpConfigApi.cc,v 1.1.2.1 2012/02/21 08:31:33 shijuc Exp $";
//----------------------------------------------------------------------
// NAME : HmpConfigApi.cc
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

// Originated:                                        Date: 17-JAN-2012
//----------------------------------------------------------------------
#include "HmpConfigApi.h"

//----------------------------------------------------------------------
// METHOD      : HmpConfigApi
// DESCRIPTION : Constructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
HmpConfigApi::HmpConfigApi ()
{
}

//----------------------------------------------------------------------
// METHOD      : ~HmpConfigApi
// DESCRIPTION : Destructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
HmpConfigApi::~HmpConfigApi ()
{
}

//----------------------------------------------------------------------
// METHOD NAME : Init
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpConfigApi::Init ()
{
   if(true != this->ReadConfig ())
   {
      T (gTrace,
            printf ("%s:\33[32m Read Config failed...\33[0m\n",
               gProcessName););
      return false;
   }

   if(0 != mHmpDb)
   {
      // Initialize DB object
      if(DB_NO_ERR != mDbObj.Init ())
      {
         printf("%s: Db object Init failed. Exiting.....\n", gProcessName);
         return false;
      }

      if(true != this->ReadConfigFromDb ())
      {
         T (gTrace,
               printf ("%s:\33[32m Read Config Db Init failed...\33[0m\n",
                  gProcessName););
         return false;
      }
   }

   T (gTrace,
         printf ("%s:\33[32m ConfigApi Init ok...\33[0m\n",
            gProcessName););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpConfigApi::ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

   if (lCfgRead.CfgInit (HMP_SYS_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, HMP_SYS_CFG););
      gLog.GenerateLog (GSYS09, HMP_SYS_CFG);
      return false;
   }

   if (CFG_OK !=
         lCfgRead.GetConfigNum ("HMP_DB", mHmpDb,
            0, 1))
   {
      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "HMP_DB", HMP_SYS_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: HMP_DB = %d\n",
               gProcessName, mHmpDb););
   }

   if(1 != mHmpDb)
   {
      if (CFG_OK !=
            lCfgRead.GetConfigNum ("HMP_LIVE_STREAM_COUNT", mIpmDeviceCount,
               1, 240))
      {
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Config read failed for %s. File:%s.",
               "HMP_LIVE_STREAM_COUNT", HMP_SYS_CFG);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
         lCfgRead.CfgDeInit ();
         mIpmDeviceCount = 0;
      }
      else
      {
         T (gTrace, printf ("%s: HMP_LIVE_STREAM_COUNT = %d\n",
                  gProcessName, mIpmDeviceCount););
      }

      if(0 != mIpmDeviceCount)
      {
         TEXT  lRtspUrl[240 + 1] = "";
         TEXT  lUrl[240 + 1] = "";

         for(int i = 1; i <= mIpmDeviceCount; i++)
         {
            sprintf(lRtspUrl, "HMP_LIVE_STREAM_URL_%d", i);
            printf("mIpmDeviceCount:%d %d\n",mIpmDeviceCount, i);
            if (CFG_OK !=
                  lCfgRead.GetConfigStr (lRtspUrl, lUrl,
                     10, 240))
            {
               snprintf (lLogText, MAX_LOG_TEXT_LEN,
                     "Config read failed for %s. File:%s.",
                     lRtspUrl, HMP_SYS_CFG);
               TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
               lCfgRead.CfgDeInit ();
               return false;
            }
            else
            {
               sprintf(mRtspUrls[i], "%s%s", "rtsp://", lUrl);
               T (gTrace, printf ("%s: %s = %s\n",
                        gProcessName, lRtspUrl, mRtspUrls[i]););
            }
         }
      }
   }

   lCfgRead.CfgDeInit ();
   return true;
}
//----------------------------------------------------------------------
// METHOD NAME : ReadConfigFromDb
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpConfigApi::ReadConfigFromDb ()
{
   INT32 lCfgValue = 0;
   TEXT  lCfgStr[200] = "";

   if (DB_NO_ERR != mDbObj.ReadCfgNum ("HMP_LIVE_STREAM_COUNT", 
            lCfgValue, 
            HMP_MIN_IPM_RESOURSE, HMP_MAX_IPM_RESOURSE))
   {
      T (gTrace, 
            printf ("%s:\33[32m No Live Stream Configured...\33[0m\n", 
               gProcessName););
      mIpmDeviceCount = 0;
   }
   else
   {
      mIpmDeviceCount = (INT16)lCfgValue;
      T (gTrace, 
            printf ("%s: HMP_LIVE_STREAM_COUNT = %d\n", 
               gProcessName, mIpmDeviceCount););
   }

   if(0 != mIpmDeviceCount)
   {
      TEXT  lRtspUrl[20 + 1] = "";

      for(int i = 1; i <= mIpmDeviceCount; i++)
      {
         sprintf(lRtspUrl, "HMP_LIVE_STREAM_URL_%d", i);

         if (DB_NO_ERR != mDbObj.ReadCfgStr (lRtspUrl, 
                  lCfgStr, 
                  HMP_MIN_IPM_RESOURSE, HMP_MAX_IPM_RESOURSE))
         {
            T (gTrace, 
                  printf ("%s:\33[32m Live Stream %d URL Not Configured...\33[0m\n", 
                     gProcessName, i););
            return false;
         }
         else
         {
            strcpy(mRtspUrls[i], lCfgStr);
            T (gTrace, 
                  printf ("%s: %s = %s\n", 
                     gProcessName, lRtspUrl, lCfgStr););
         }
      }
   }
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : ReloadConfig
// DESCRIPTION :
// PARAMETERS  :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpConfigApi::ReloadConfig()
{
   return this->ReadConfigFromDb();
}

