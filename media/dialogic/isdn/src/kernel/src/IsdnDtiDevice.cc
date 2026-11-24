static const char *id = "@(#) $Id: IsdnDtiDevice.cc,v 1.1.4.3 2016/04/12 21:59:36 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       IsdnDtiDevice.cc
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
// AUTHOR         : Chethan V M                     25/07/2013
//----------------------------------------------------------------------
#include "IsdnDtiDevice.h"

//----------------------------------------------------------------------
// METHOD NAME : IsdnDtiDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnDtiDevice::IsdnDtiDevice()
{
   memset(mIsdnDti, 0, sizeof(IsdnDti) * ISDN_MAX_DTI_DEVICE);
   mNumOfDtiDevice   = 0;
}

//----------------------------------------------------------------------
// METHOD NAME : ~IsdnDtiDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnDtiDevice::~IsdnDtiDevice()
{
}

//----------------------------------------------------------------------
// METHOD NAME : OpenDtiDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDtiDevice::OpenDtiDevice(INT32 dtiHdlIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   mIsdnDti[dtiHdlIdx].dtiHandle = dt_open(mIsdnDti[dtiHdlIdx].dtiDeviceName, 0);
   if(0 > mIsdnDti[dtiHdlIdx].dtiHandle)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_open Failed",
            mIsdnDti[dtiHdlIdx].dtiDeviceName, mIsdnDti[dtiHdlIdx].dtiHandle,
            ATDV_LASTERR(mIsdnDti[dtiHdlIdx].dtiHandle), ATDV_ERRMSGP(mIsdnDti[dtiHdlIdx].dtiHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DTI01, lLogText);

      return false;
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_open Success", 
         mIsdnDti[dtiHdlIdx].dtiDeviceName, mIsdnDti[dtiHdlIdx].dtiHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetDtiXmitslot
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDtiDevice::GetDtiXmitslot(INT32 dtiHdlIdx)
{
   SC_TSINFO         lSc_tsinfo;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   lSc_tsinfo.sc_numts = 1;
   lSc_tsinfo.sc_tsarrayp = &mIsdnDti[dtiHdlIdx].scts;

   if (ISDN_DTI_FAILURE == dt_getxmitslot(mIsdnDti[dtiHdlIdx].dtiHandle, &lSc_tsinfo))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_getxmitslot Failed",
            mIsdnDti[dtiHdlIdx].dtiDeviceName, mIsdnDti[dtiHdlIdx].dtiHandle,
            ATDV_LASTERR(mIsdnDti[dtiHdlIdx].dtiHandle), ATDV_ERRMSGP(mIsdnDti[dtiHdlIdx].dtiHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DTI02, lLogText); 
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d scts:%ld dt_getxmitslot Success", 
         mIsdnDti[dtiHdlIdx].dtiDeviceName, mIsdnDti[dtiHdlIdx].dtiHandle, mIsdnDti[dtiHdlIdx].scts);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : CloseDtiDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDtiDevice::CloseDtiDevice(INT32 dtiHdlIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if(ISDN_DTI_FAILURE == dt_close(mIsdnDti[dtiHdlIdx].dtiHandle))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_close Failed", 
            mIsdnDti[dtiHdlIdx].dtiDeviceName, mIsdnDti[dtiHdlIdx].dtiHandle);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DTI03, lLogText);
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_close Success", 
         mIsdnDti[dtiHdlIdx].dtiDeviceName, mIsdnDti[dtiHdlIdx].dtiHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DtListen
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDtiDevice::DtListen(int dtiDevIdx, long &scts)
{
   SC_TSINFO         lSc_tsinfo;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   lSc_tsinfo.sc_numts = 1;
   lSc_tsinfo.sc_tsarrayp = &scts;

   if (dt_listen(mIsdnDti[dtiDevIdx].dtiHandle, &lSc_tsinfo) == ISDN_DTI_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_listen Failed",
            mIsdnDti[dtiDevIdx].dtiDeviceName, mIsdnDti[dtiDevIdx].dtiHandle, 
            ATDV_LASTERR(mIsdnDti[dtiDevIdx].dtiHandle), ATDV_ERRMSGP(mIsdnDti[dtiDevIdx].dtiHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DTI04, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_listen Success",
         mIsdnDti[dtiDevIdx].dtiDeviceName, mIsdnDti[dtiDevIdx].dtiHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DTI05, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DtUnListen
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDtiDevice::DtUnListen(int dtiDevIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if (dt_unlisten(mIsdnDti[dtiDevIdx].dtiHandle) == ISDN_DTI_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_unlisten Failed",
            mIsdnDti[dtiDevIdx].dtiDeviceName, mIsdnDti[dtiDevIdx].dtiHandle, 
            ATDV_LASTERR(mIsdnDti[dtiDevIdx].dtiHandle), ATDV_ERRMSGP(mIsdnDti[dtiDevIdx].dtiHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DTI06, lLogText);
      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_unlisten Success",
            mIsdnDti[dtiDevIdx].dtiDeviceName, mIsdnDti[dtiDevIdx].dtiHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DTI08, lLogText);
   }

   return true;
}
