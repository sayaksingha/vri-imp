static const char *id = "@(#) $Id: HmpBaseDeviceIpm.cc,v 1.1.2.1 2012/02/21 08:31:36 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpBaseDevice.cc
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
// AUTHOR         : Chetan V M                               12-JAN-2012
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "HmpBaseDevice.h"

pthread_mutex_t Chnl_mutex[MAX_SYNWAY_CHANNEL];

//----------------------------------------------------------------------
// METHOD NAME : HmpBaseDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpBaseDevice::HmpBaseDevice()
{
   mNumOfDxDevice    = 0;
   mNumOfDtiDevice   = 0;
   mNumOfIpmDevice   = 0;

   memset(mCircuitHandle, 0, sizeof(CircuitHandle) * MAX_DTI_DEVICE);
}

//----------------------------------------------------------------------
// METHOD NAME : ~HmpBaseDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpBaseDevice::~HmpBaseDevice()
{
}

//----------------------------------------------------------------------
// METHOD NAME : Init()
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::Init()
{
   if(true != GetDeviceInfo())
   {
      return false;
   }
   
   if(true != OpenDevices())
   {
      return false;
   }

   if(true != mHmpReqMapApi.Init())
   {
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : StartSrEvent
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : BOOLEAN 
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::StartSrEvent()
{
   int ret;
   for (;;) 
   {
      ret = sr_waitevt(500);            
      if (ret != -1) 
      {

      }
   }
}

//----------------------------------------------------------------------
// METHOD NAME : GetDeviceInfo
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::GetDeviceInfo()
{
   AUID*          lPhysicalBoard;
   long           lRetVal;
   int            lNumPylBds  = 0;
   int            lLen        = 0;
   TEXT           lPhyBrdName[MAX_BOARD_NAME_LEN] = "";

   lRetVal = SRLGetAllPhysicalBoards(&lNumPylBds, 0);

   if(ESR_INSUFBUF == lRetVal)
   {
      if(lNumPylBds != 0)
         lPhysicalBoard = (AUID *)malloc(lNumPylBds * sizeof(AUID));

      if(lPhysicalBoard)
      {
         lRetVal = SRLGetAllPhysicalBoards(&lNumPylBds,lPhysicalBoard);
         if(ESR_NOERR == lRetVal)
         {
            T(gTrace, printf("%s: Number of Physical Boards: %d\n",gProcessName, lNumPylBds););
         }
         else
         {
            TERR (gTrace, printf("%s: SRLGetAllPhysicalBoards Error .....\n", gProcessName););
            return false;
         }
      }
   }
   else
   {
      TERR (gTrace, printf("%s: SRLGetAllPhysicalBoards Error .....\n", gProcessName););
      return false;
   }

   for(int lBrdCnt = 0;lBrdCnt < lNumPylBds; lBrdCnt++)
   {
      lLen = MAX_BOARD_NAME_LEN;

      SRLGetPhysicalBoardName(lPhysicalBoard[lBrdCnt], &lLen, lPhyBrdName);
      T(gTrace, printf("%s: Name of Physical Board %d: %s\n", gProcessName, lBrdCnt + 1, lPhyBrdName););

      if(VirtualBoardCount(lPhysicalBoard[lBrdCnt]) == false)
      {
         TERR(gTrace, printf("%s: No Virtual Borads Vailable on Physical Board: %d\n", gProcessName, lBrdCnt + 1););
      }
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : VirtualBoardCount
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::VirtualBoardCount(AUID PhyBd)
{
   int                 lNumVB = 0;
   long                lRetVal;
   AUID                lPhyBd;
   SRLDEVICEINFO       *lVirBrdPtr;

   lPhyBd = PhyBd;

   lRetVal = SRLGetVirtualBoardsOnPhysicalBoard(lPhyBd, &lNumVB, 0);
   if(ESR_INSUFBUF == lRetVal)
   {
      lVirBrdPtr = (SRLDEVICEINFO *)malloc(lNumVB * sizeof(SRLDEVICEINFO));
      if(lVirBrdPtr)
      {
         lRetVal = SRLGetVirtualBoardsOnPhysicalBoard(lPhyBd, &lNumVB, lVirBrdPtr);
         if(ESR_NOERR == lRetVal)
            T(gTrace, printf("%s: Number of Virtual Boards: %d\n", gProcessName, lNumVB););
      }
      for(int lVbCnt = 1;lVbCnt <= lNumVB;lVbCnt++)
      {
         T (gTrace, printf("%s: Virtual Board: %d\n", gProcessName, lVbCnt););
         T (gTrace, printf("%s: Virtual Board Name: %s\n", gProcessName, lVirBrdPtr -> szDevName););
         T (gTrace, printf("%s: Virtual Board Type: %d\n", gProcessName, lVirBrdPtr -> iDevType););

         if(Subdevices(lVirBrdPtr -> szDevName) == false)
         {
            TERR(gTrace, printf("%s: No Subdevices Avilable on Virtual Board: %d\n", gProcessName, lVbCnt););
         }
         lVirBrdPtr++;
      }
   }
   else
   {
      TERR(gTrace, printf("%s: No SubDevices on Any Virtual Board\n", gProcessName););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Subdevices
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::Subdevices(TEXT* VBName)
{
   char             szVB[12];
   int              lNumSDs = 0;
   long             lRetVal;
   SRLDEVICEINFO    *lVBPtr;

   strcpy(szVB, VBName);

   lRetVal = SRLGetSubDevicesOnVirtualBoard(szVB, &lNumSDs, 0);
   if (ESR_INSUFBUF == lRetVal )
   {
      lVBPtr = (SRLDEVICEINFO *)malloc(lNumSDs * sizeof(*lVBPtr));
      if (0 != lVBPtr)
      {
         lRetVal = SRLGetSubDevicesOnVirtualBoard(szVB, &lNumSDs, lVBPtr);
         if (ESR_NOERR == lRetVal)
            T (gTrace, printf("%s: Num of Sub Devices: %d\n", gProcessName, lNumSDs););
      }
      else
      {
         TERR (gTrace, printf("%s: No Sub Devices Available\n", gProcessName););
         return false;
      }
      for(int lSdCnt = 1;lSdCnt <= lNumSDs;lSdCnt++)
      {
         T (gTrace, printf("%s: Sub Device     :%d\n", gProcessName, lSdCnt););
         T (gTrace, printf("%s: Sud Device Name: %s\n", gProcessName, lVBPtr->szDevName););
         T (gTrace, printf("%s: Sub Device Type: %d\n", gProcessName, lVBPtr->iDevType););

         switch(lVBPtr->iDevType)
         {
            case 502: //VOICE_DEV
               mHmpDxStruct[mNumOfDxDevice].index = mNumOfDxDevice;
               strcpy(mHmpDxStruct[mNumOfDxDevice].dxDeviceName, lVBPtr->szDevName);
               mNumOfDxDevice++;
               break;
            case 504: //LINE_DEV
               mHmpDtiStruct[mNumOfDtiDevice].index = mNumOfDtiDevice;
               strcpy(mHmpDtiStruct[mNumOfDtiDevice].dtiDeviceName, lVBPtr->szDevName);
               mNumOfDtiDevice++;
               break;
            case 528: //IPM_DEV
               mHmpIpmStruct[mNumOfIpmDevice].index = mNumOfIpmDevice;
               strcpy(mHmpIpmStruct[mNumOfIpmDevice].ipmDeviceName, lVBPtr->szDevName);
               mNumOfIpmDevice++;
         }

         lVBPtr++;
      }
   }
   else
   {
      TERR (gTrace, printf("%s: No Sub Devices Available on Any Virtual Board\n", gProcessName););
      return false;
   }
   T (gTrace, printf("%s: Voice Device Count: %ld Line Device Count: %ld Ipm Device Count: %ld\n",
            gProcessName, mNumOfDxDevice, mNumOfDtiDevice, mNumOfIpmDevice););
   return true;

}

//----------------------------------------------------------------------
// METHOD NAME : OpenDevices
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::OpenDevices()
{
   BOOLEAN lRetDti, lRetVoice, lRetIpm;
   lRetDti = lRetVoice = lRetIpm = false;

   lRetVoice = OpenDxDevice();
   lRetDti   = OpenDtiDevice();
   lRetIpm   = OpenIpmDevice();

   return (lRetVoice && lRetDti && lRetIpm);
}

//----------------------------------------------------------------------
// METHOD NAME : ProcessEvent
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::ProcessEvent()
{
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : PlayFile
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::PlayFile(VpRequestDetails &vpReqDetails)
{
   INT32       lFreeDx    = -1;
   INT32       lDtiIdx    = vpReqDetails.vpReqData.playMsg.circuitId - 1;
   HmpReqMap   lHmpReqMap;
   TEXT        lHashKey[20 + 1] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN] = "";


   snprintf(lLogText, MAX_LOG_TEXT_LEN, "CIC:%ld Recvd Play CS:%d", 
         lDtiIdx, mCircuitHandle[lDtiIdx].channelStatus);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   //gLog-Chethan

   switch(mCircuitHandle[lDtiIdx].channelStatus)
   {
      case VOICE_CHANNEL_IDLE:
         {
            if(-1 != (lFreeDx = GetFreeDxDevice()))
            {
               if(true == DtListen(lDtiIdx, mHmpDxStruct[lFreeDx].scts) && 
                     true == DxListen(lFreeDx, mHmpDtiStruct[lDtiIdx].scts))
               {
                     if(true == Play(vpReqDetails.vpReqData.playMsg, lFreeDx))
                     {

                        pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
                        mCircuitHandle[lDtiIdx].cicHdlType     = CIC_DX_HANDLE;
                        mCircuitHandle[lDtiIdx].dxHandleIdx    = lFreeDx;
                        mCircuitHandle[lDtiIdx].channelStatus  = VOICE_CHANNEL_PLAYING;
                        pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);
                        
                        memset(&lHmpReqMap, 0, sizeof(HmpReqMap));
                        lHmpReqMap.deviceType = HMP_DX_DEVICE;
                        lHmpReqMap.dtiHandleIdx[lHmpReqMap.totalDtiDevice] = lDtiIdx;
                        lHmpReqMap.totalDtiDevice++;
                        lHmpReqMap.lChnlReqStatus = NEXT_IDLE;
                        lHmpReqMap.requestDetails = vpReqDetails;
                        sprintf(lHmpReqMap.key, "%ld", mHmpDxStruct[lFreeDx].dxHandle);
                        
                        if(true != mHmpReqMapApi.InsertRequestIntoHash(lHmpReqMap))
                        {
                           snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                                 "CIC:%ld Insert to Hash Table Failed CS:VOICE_CHANNEL_IDLE", lDtiIdx);
                           TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                           //gLog-Chethan 
                        }
                        else
                           return true;
                     }
               }
            }
         }
         DeviceUnlisten(lFreeDx, lDtiIdx);
         break;
      case VOICE_CHANNEL_READY:
         {
            lFreeDx = mCircuitHandle[lDtiIdx].dxHandleIdx;
            if(true == Play(vpReqDetails.vpReqData.playMsg, lFreeDx))
            {
               pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
               mCircuitHandle[lDtiIdx].channelStatus  = VOICE_CHANNEL_PLAYING;
               pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);
               
               sprintf(lHashKey, "%ld", mHmpDxStruct[lFreeDx].dxHandle);
               if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReqMap))
               {
                  lHmpReqMap.lChnlReqStatus = NEXT_IDLE;
                  lHmpReqMap.requestDetails = vpReqDetails;
                  if(true != mHmpReqMapApi.UpdateRequestIntoHash(lHmpReqMap))
                  {
                     snprintf(lLogText, MAX_LOG_TEXT_LEN,
                           "CIC:%ld Update to Hash Table Failed CS:VOICE_CHANNEL_READY", lDtiIdx);
                     TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                     //gLog-Chethan
                  }
                  else
                     return true;

               }
               else
               {
                  snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                        "CIC:%ld Get Hash Failed CS:VOICE_CHANNEL_READY", lDtiIdx);
                  TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
                  //gLog-Chethan
               }

            }
         }
         break;
      case VOICE_CHANNEL_PLAYING:
         {
            lFreeDx = mCircuitHandle[lDtiIdx].dxHandleIdx;
            
            sprintf(lHashKey, "%ld", mHmpDxStruct[lFreeDx].dxHandle);
            if(true == mHmpReqMapApi.GetReqMapDetails(lHashKey, lHmpReqMap))
            {

            }
            if(true == PlayStop(lFreeDx))
            {
               pthread_mutex_lock(&Chnl_mutex[lDtiIdx]);
               mCircuitHandle[lDtiIdx].channelStatus  = VOICE_CHANNEL_PLAY_STOPING;
               pthread_mutex_unlock(&Chnl_mutex[lDtiIdx]);
            }
         }
         break;
   }

   return false;
}


//----------------------------------------------------------------------
// METHOD NAME : DeviceUnlisten
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
void HmpBaseDevice::DeviceUnlisten(int dxHdlIdx, int ipmHdlIdx, int dtiHdlIdx)
{
   if(-1 != dxHdlIdx)
   {
      FreeDxDevice(dxHdlIdx);
      DxUnlisten(dxHdlIdx);
   }

  if(-1 != dtiHdlIdx)
  {
     DtUnListen(dtiHdlIdx);
     pthread_mutex_lock(&Chnl_mutex[dtiHdlIdx]);
     memset(&mCircuitHandle[lDtiIdx], 0, sizeof(CircuitHandle));
     pthread_mutex_unlock(&Chnl_mutex[dtiHdlIdx]);

  }

  //if(-1 != ipmHdlIdx)
  //IpmUnListen(ipmHdlIdx);
}


//----------------------------------------------------------------------
// METHOD NAME : PlayStream
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpBaseDevice::PlayStream(VpRequestDetails &vpReqDetails)
{
   return true;
}
