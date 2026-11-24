static const char *id = "@(#) $Id: HmpDtiDevice.cc,v 1.1.2.4 2016/04/12 21:59:34 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpDtiDevice.cc
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
// AUTHOR         : Ramprasad M                              12-JAN-2012
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "HmpDtiDevice.h"

//----------------------------------------------------------------------
// METHOD NAME : HmpDtiDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpDtiDevice::HmpDtiDevice()
{
   memset(mHmpDtiStruct, 0, sizeof(HmpDtiStruct) * MAX_DTI_DEVICE);
}

//----------------------------------------------------------------------
// METHOD NAME : ~HmpDtiDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpDtiDevice::~HmpDtiDevice()
{
}

//----------------------------------------------------------------------
// METHOD NAME : OpenDtiDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDtiDevice::OpenDtiDevice()
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";
   SC_TSINFO         lSc_tsinfo;

   for(UINT32 lIdx = 1; lIdx <= mNumOfDtiDevice; lIdx++)
   {
      mHmpDtiStruct[lIdx].dtiHandle = dt_open(mHmpDtiStruct[lIdx].dtiDeviceName, 0);
      if(0 > mHmpDtiStruct[lIdx].dtiHandle)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_open Failed",
               mHmpDtiStruct[lIdx].dtiDeviceName, mHmpDtiStruct[lIdx].dtiHandle,
               ATDV_LASTERR(mHmpDtiStruct[lIdx].dtiHandle), ATDV_ERRMSGP(mHmpDtiStruct[lIdx].dtiHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DTI01, lLogText);

         return false;
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_open Success", 
            mHmpDtiStruct[lIdx].dtiDeviceName, mHmpDtiStruct[lIdx].dtiHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

      lSc_tsinfo.sc_numts = 1;
      lSc_tsinfo.sc_tsarrayp = &mHmpDtiStruct[lIdx].scts;

      if (HMP_FAILURE == dt_getxmitslot(mHmpDtiStruct[lIdx].dtiHandle, &lSc_tsinfo))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_getxmitslot Failed",
               mHmpDtiStruct[lIdx].dtiDeviceName, mHmpDtiStruct[lIdx].dtiHandle,
               ATDV_LASTERR(mHmpDtiStruct[lIdx].dtiHandle), ATDV_ERRMSGP(mHmpDtiStruct[lIdx].dtiHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DTI02, lLogText); 
      }

      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d scts:%ld dt_getxmitslot Success", 
            mHmpDtiStruct[lIdx].dtiDeviceName, mHmpDtiStruct[lIdx].dtiHandle, mHmpDtiStruct[lIdx].scts);
   }


   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : CloseDtiDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDtiDevice::CloseDtiDevice()
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = 1; lIdx <= mNumOfDtiDevice; lIdx++)
   {
      if(HMP_FAILURE == dt_close(mHmpDtiStruct[lIdx].dtiHandle))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_close Failed", 
               mHmpDtiStruct[lIdx].dtiDeviceName, mHmpDtiStruct[lIdx].dtiHandle);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DTI03, lLogText);
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_close Success", 
            mHmpDtiStruct[lIdx].dtiDeviceName, mHmpDtiStruct[lIdx].dtiHandle);
       T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DtListen
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDtiDevice::DtListen(int dtiDevIdx, long &scts)
{
   SC_TSINFO         lSc_tsinfo;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   lSc_tsinfo.sc_numts = 1;
   lSc_tsinfo.sc_tsarrayp = &scts;

   if (dt_listen(mHmpDtiStruct[dtiDevIdx].dtiHandle, &lSc_tsinfo) == HMP_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_listen Failed",
            mHmpDtiStruct[dtiDevIdx].dtiDeviceName, mHmpDtiStruct[dtiDevIdx].dtiHandle, 
            ATDV_LASTERR(mHmpDtiStruct[dtiDevIdx].dtiHandle), ATDV_ERRMSGP(mHmpDtiStruct[dtiDevIdx].dtiHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DTI04, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_listen Success",
         mHmpDtiStruct[dtiDevIdx].dtiDeviceName, mHmpDtiStruct[dtiDevIdx].dtiHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DTI05, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DtUnListen
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDtiDevice::DtUnListen(int dtiDevIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if (dt_unlisten(mHmpDtiStruct[dtiDevIdx].dtiHandle) == HMP_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d ECo:%ld ESr:%s dt_unlisten Failed",
            mHmpDtiStruct[dtiDevIdx].dtiDeviceName, mHmpDtiStruct[dtiDevIdx].dtiHandle, 
            ATDV_LASTERR(mHmpDtiStruct[dtiDevIdx].dtiHandle), ATDV_ERRMSGP(mHmpDtiStruct[dtiDevIdx].dtiHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DTI06, lLogText);
      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DtiName:%s DtiH:%d dt_unlisten Success",
            mHmpDtiStruct[dtiDevIdx].dtiDeviceName, mHmpDtiStruct[dtiDevIdx].dtiHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DTI08, lLogText);
   }

   return true;
}
