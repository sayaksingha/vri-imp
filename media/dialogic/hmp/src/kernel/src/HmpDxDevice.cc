static const char *id = "@(#) $Id: HmpDxDevice.cc,v 1.1.2.4 2016/04/12 21:59:34 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpDxDevice.cc
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
//                : Chetan V M
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "HmpDxDevice.h"

//----------------------------------------------------------------------
// METHOD NAME : HmpDxDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpDxDevice::HmpDxDevice()
{
   memset(mHmpDxStruct, 0, sizeof(HmpDxStruct) * MAX_DX_DEVICE);
}

//----------------------------------------------------------------------
// METHOD NAME : ~HmpDxDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpDxDevice::~HmpDxDevice()
{
}

//----------------------------------------------------------------------
// METHOD NAME : OpenDxDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::OpenDxDevice()
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";
   SC_TSINFO         lSc_tsinfo;

   for(UINT32 lIdx = 1; lIdx <= mNumOfDxDevice; lIdx++)
   {
      mHmpDxStruct[lIdx].dxHandle = dx_open(mHmpDxStruct[lIdx].dxDeviceName, 0);
      if(0 > mHmpDxStruct[lIdx].dxHandle)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_open Failed",
               mHmpDxStruct[lIdx].dxDeviceName, mHmpDxStruct[lIdx].dxHandle,
               ATDV_LASTERR(mHmpDxStruct[lIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[lIdx].dxHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DX01, lLogText);
         return false;
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_open Success",
            mHmpDxStruct[lIdx].dxDeviceName, mHmpDxStruct[lIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););


      lSc_tsinfo.sc_numts = 1;
      lSc_tsinfo.sc_tsarrayp = &mHmpDxStruct[lIdx].scts;

      if (HMP_FAILURE == dx_getxmitslot(mHmpDxStruct[lIdx].dxHandle, &lSc_tsinfo)) 
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_getxmitslot Failed",
               mHmpDxStruct[lIdx].dxDeviceName,  mHmpDxStruct[lIdx].dxHandle,
               ATDV_LASTERR(mHmpDxStruct[lIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[lIdx].dxHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DX02, lLogText);
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d scts:%ld dx_getxmitslot Success", 
            mHmpDxStruct[lIdx].dxDeviceName, mHmpDxStruct[lIdx].dxHandle, mHmpDxStruct[lIdx].scts);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : CloseDxDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::CloseDxDevice()
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = 1; lIdx <= mNumOfDxDevice; lIdx++)
   {
      if(HMP_FAILURE == dx_close(mHmpDxStruct[lIdx].dxHandle))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_close Failed", 
               mHmpDxStruct[lIdx].dxDeviceName, mHmpDxStruct[lIdx].dxHandle);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DX03, lLogText);
      }

      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_close Success", 
            mHmpDxStruct[lIdx].dxDeviceName, mHmpDxStruct[lIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }


   return true;
}


//----------------------------------------------------------------------
// METHOD NAME : GetFreeDxDevice
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : INT32
//----------------------------------------------------------------------
BOOLEAN  HmpDxDevice::FreeDxDevice(INT32 dxHdlIdx)
{
   mHmpDxStruct[dxHdlIdx].deviceState = DEVICE_IDLE;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetFreeDxDevice
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : INT32
//----------------------------------------------------------------------
INT32 HmpDxDevice::GetFreeDxDevice()
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = 1; lIdx <= mNumOfDxDevice; lIdx++)
   {
      if(DEVICE_IDLE == mHmpDxStruct[lIdx].deviceState)
      {
         mHmpDxStruct[lIdx].deviceState = DEVICE_BUSY;
         return mHmpDxStruct[lIdx].index;
      }
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "No Free Dx Device");
   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DX06, lLogText);


   return -1;
}

//----------------------------------------------------------------------
// METHOD NAME : DxListen
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::DxListen(int dxDevIdx, long &scts)
{
   SC_TSINFO         lSc_tsinfo_dt;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   lSc_tsinfo_dt.sc_numts = 1;
   lSc_tsinfo_dt.sc_tsarrayp = &scts;


   if (dx_listen(mHmpDxStruct[dxDevIdx].dxHandle, &lSc_tsinfo_dt) == HMP_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_listen Failed",
            mHmpDxStruct[dxDevIdx].dxDeviceName, mHmpDxStruct[dxDevIdx].dxHandle, 
            ATDV_LASTERR(mHmpDxStruct[dxDevIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[dxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX04, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_listen Success",
         mHmpDxStruct[dxDevIdx].dxDeviceName, mHmpDxStruct[dxDevIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DX05, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DxUnlisten
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::DxUnlisten(int dxDevIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if (dx_unlisten(mHmpDxStruct[dxDevIdx].dxHandle) == HMP_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_unlisten Failed",
            mHmpDxStruct[dxDevIdx].dxDeviceName,mHmpDxStruct[dxDevIdx].dxHandle,
            ATDV_LASTERR(mHmpDxStruct[dxDevIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[dxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX07, lLogText);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_unlisten Success",
            mHmpDxStruct[dxDevIdx].dxDeviceName, mHmpDxStruct[dxDevIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX08, lLogText);
   }

   return true;
}


//----------------------------------------------------------------------
// METHOD NAME : Record
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::Record(MpRecord &recordDetails, INT32 lDxDevIdx)
{
   DV_TPT            lTpt[20];
   DX_XPB            lXpb;
   INT16             lStopPlayCharLen           = 0;
   int               lIdx                       = 0;
   int               lTpLength                  = 0;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(lTpt, 0, sizeof(lTpt));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Voice Record TC:%s RecTime:%dms FF:%s",
         mHmpDxStruct[lDxDevIdx].dxHandle, recordDetails.dtmfStopRecCharSet,
         recordDetails.recTime, (0 == recordDetails.fileFormat) ? "WAV" : "VOX");
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DX09, lLogText);

   if(MEDIA_VOX == recordDetails.fileFormat)
   {
      lXpb.wFileFormat        = FILE_FORMAT_VOX;
      lXpb.wDataFormat        = DATA_FORMAT_MULAW;
      lXpb.nSamplesPerSec     = DRT_8KHZ;
      lXpb.wBitsPerSample     = 8;
   }
   else
   {
      lXpb.wFileFormat        = FILE_FORMAT_WAVE;
      lXpb.wDataFormat        = DATA_FORMAT_DIALOGIC_ADPCM;
      lXpb.nSamplesPerSec     = DRT_8KHZ;
      lXpb.wBitsPerSample     = 4;
   }

   if(0 != (lStopPlayCharLen = strlen(recordDetails.dtmfStopRecCharSet)))
   {
      //Terminate play for configure Dtmf
      for(int lRcIdx = 0; lRcIdx < lStopPlayCharLen; lRcIdx++)
      {
         lTpLength = lTpLength | gDMValues[(unsigned int) recordDetails.dtmfStopRecCharSet[lRcIdx]];
      }

      lTpt[lIdx].tp_type       = IO_CONT;
      lTpt[lIdx].tp_termno     = DX_DIGMASK;
      lTpt[lIdx].tp_length     = lTpLength;
      lTpt[lIdx].tp_flags      = TF_DIGMASK;
      lIdx++;
   }
   else
   {
      //Terminate play for all the Dtmf, so configure Termination condn at dtmf len = 1
      lTpt[lIdx].tp_type   = IO_CONT;
      lTpt[lIdx].tp_termno = DX_MAXDTMF;
      lTpt[lIdx].tp_length = 1;
      lTpt[lIdx].tp_flags  = TF_MAXDTMF;
      lIdx++;
   }

   if(0 == recordDetails.recTime)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Record Mam Time Not Specified Error",
            mHmpDxStruct[lDxDevIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX10, lLogText);

      return false;
   }

   if (dx_clrdigbuf(mHmpDxStruct[lDxDevIdx].dxHandle) != 0) 
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Clear Digit Buffer Failed ECo:%ld ESr:%s",
            mHmpDxStruct[lDxDevIdx].dxHandle, 
            ATDV_LASTERR (mHmpDxStruct[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[lDxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX11, lLogText);

      return false;
   }


   lTpt[lIdx].tp_type   = IO_EOT;
   lTpt[lIdx].tp_termno = DX_MAXTIME;
   lTpt[lIdx].tp_length = (int)(recordDetails.recTime/100);//2000 ms in dialogic is 2000 100ms so / 100
   lTpt[lIdx].tp_flags  = TF_MAXTIME;

   if(0 == mHmpDxStruct[lDxDevIdx].iott[0].io_fhandle)
   {
      if((mHmpDxStruct[lDxDevIdx].iott[0].io_fhandle = open(recordDetails.voiceFile, O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d open  Failed File:%s Errno:%d  ErrStr:%s",
               mHmpDxStruct[lDxDevIdx].dxHandle, recordDetails.voiceFile, errno, strerror(errno));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DX10, lLogText);

         return false;
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d open  Success File:%s",
            mHmpDxStruct[lDxDevIdx].dxHandle, recordDetails.voiceFile);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }

   mHmpDxStruct[lDxDevIdx].iott[0].io_type    = IO_DEV | IO_EOT ;
   mHmpDxStruct[lDxDevIdx].iott[0].io_offset  =  0 ;
   mHmpDxStruct[lDxDevIdx].iott[0].io_length  = -1 ;
   mHmpDxStruct[lDxDevIdx].iott[0].io_nextp = NULL;
   mHmpDxStruct[lDxDevIdx].iott[0].io_prevp = NULL;

   if(dx_reciottdata(mHmpDxStruct[lDxDevIdx].dxHandle, &mHmpDxStruct[lDxDevIdx].iott[0], lTpt, &lXpb, EV_ASYNC) != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d record File:%s Failed ECo:%ld ESr:%s",
            mHmpDxStruct[lDxDevIdx].dxHandle, recordDetails.voiceFile,
            ATDV_LASTERR (mHmpDxStruct[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[lDxDevIdx].dxHandle));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX10, lLogText);

      CloseDxIOFiles(lDxDevIdx, 1);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d record File:%s Success",
            mHmpDxStruct[lDxDevIdx].dxHandle, recordDetails.voiceFile);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX12, lLogText);
   }
   
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Play
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::Play(MpPlay &playDetails, INT32 lDxDevIdx)
{
   DV_TPT            lTpt[20];
   DX_XPB            lXpb;
   INT16             lStopPlayCharLen           = 0;
   int               lIdx                       = 0;
   int               lTpLength                  = 0;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(lTpt, 0, sizeof(lTpt));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Voice Play FC:%d TC:%s DE:%d RP:%d FF:%s",
         mHmpDxStruct[lDxDevIdx].dxHandle, playDetails.vFileCount, playDetails.dtmfStopPlayCharSet, playDetails.dtmfEnable,
         playDetails.repeatPlay, (0 == playDetails.fileFormat) ? "WAV" : "VOX");
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DX13, lLogText);

   if(MEDIA_VOX == playDetails.fileFormat)
   {
      lXpb.wFileFormat        = FILE_FORMAT_VOX;
      lXpb.wDataFormat        = DATA_FORMAT_MULAW;
      lXpb.nSamplesPerSec     = DRT_8KHZ;
      lXpb.wBitsPerSample     = 8;
   }
   else
   {
      lXpb.wFileFormat        = FILE_FORMAT_WAVE;
      lXpb.wDataFormat        = DATA_FORMAT_DIALOGIC_ADPCM;
      lXpb.nSamplesPerSec     = DRT_8KHZ;
      lXpb.wBitsPerSample     = 4;
   }

   if(0 != (lStopPlayCharLen = strlen(playDetails.dtmfStopPlayCharSet)))
   {
      //Terminate play for configure Dtmf
      for(int lPlIdx = 0; lPlIdx < lStopPlayCharLen; lPlIdx++)
      {
         lTpLength = lTpLength | gDMValues[(unsigned int)playDetails.dtmfStopPlayCharSet[lPlIdx]];
      }

      lTpt[lIdx].tp_type       = IO_EOT;
      lTpt[lIdx].tp_termno     = DX_DIGMASK;
      lTpt[lIdx].tp_length     = lTpLength;
      lTpt[lIdx].tp_flags      = TF_DIGMASK;
   }
   else
   {
      //Terminate play for all the Dtmf, so configure Termination condn at dtmf len = 1
      lTpt[lIdx].tp_type   = IO_EOT;
      lTpt[lIdx].tp_termno = DX_MAXDTMF;
      lTpt[lIdx].tp_length = 1;
      lTpt[lIdx].tp_flags  = TF_MAXDTMF;
   }
   
   if( 1 == playDetails.dtmfEnable)
   {
      if (dx_clrdigbuf(mHmpDxStruct[lDxDevIdx].dxHandle) != 0) 
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Clear Digit Buffer Failed ECo:%ld ESr:%s",
               mHmpDxStruct[lDxDevIdx].dxHandle, 
               ATDV_LASTERR (mHmpDxStruct[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[lDxDevIdx].dxHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_DX11, lLogText);

         return false;
      }
   }

   for(int lFile = 0; lFile < playDetails.vFileCount; lFile++)
   {
      if(0 == mHmpDxStruct[lDxDevIdx].iott[lFile].io_fhandle)
      {
         if((mHmpDxStruct[lDxDevIdx].iott[lFile].io_fhandle = 
                  open(playDetails.voiceFile[lFile], O_RDONLY, 0666)) == -1)
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "DxH:%d open Failed File:%s FileCount:%d Errno:%d ErrStr:%s", 
                  mHmpDxStruct[lDxDevIdx].dxHandle, playDetails.voiceFile[lFile], 
                  lFile, errno, strerror(errno));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(HMP_DX14, lLogText);

            for(int lFlOpn = 0; lFlOpn < lFile; lFlOpn++)
            {
               //close all previous opened files
               if(0 != mHmpDxStruct[lDxDevIdx].iott[lFlOpn].io_fhandle)
               {
                  close(mHmpDxStruct[lDxDevIdx].iott[lFlOpn].io_fhandle);
                  mHmpDxStruct[lDxDevIdx].iott[lFlOpn].io_fhandle = 0;
               }
            }

            return false;
         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "DxH:%d open Success File:%s FileCount:%d", 
                  mHmpDxStruct[lDxDevIdx].dxHandle, playDetails.voiceFile[lFile], 
                  lFile);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

            mHmpDxStruct[lDxDevIdx].iott[lFile].io_type    = 
               (lFile == playDetails.vFileCount-1) ? IO_DEV | IO_EOT : IO_DEV ;
            mHmpDxStruct[lDxDevIdx].iott[lFile].io_offset  = 0;
            mHmpDxStruct[lDxDevIdx].iott[lFile].io_length  = -1;
            mHmpDxStruct[lDxDevIdx].iott[lFile].io_nextp   = NULL;
            mHmpDxStruct[lDxDevIdx].iott[lFile].io_prevp   = NULL;   
         }
      }
   }

   if(dx_playiottdata (mHmpDxStruct[lDxDevIdx].dxHandle, &mHmpDxStruct[lDxDevIdx].iott[0], lTpt, &lXpb, EV_ASYNC) != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d play File:%s FileCount:%d Failed ECo:%ld ESr:%s", 
            mHmpDxStruct[lDxDevIdx].dxHandle, playDetails.voiceFile[0], playDetails.vFileCount,
            ATDV_LASTERR (mHmpDxStruct[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[lDxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX14, lLogText);
      
      CloseDxIOFiles(lDxDevIdx, playDetails.vFileCount);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d play File:%s FileCount:%d Success",
            mHmpDxStruct[lDxDevIdx].dxHandle, playDetails.voiceFile[0], playDetails.vFileCount);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX15, lLogText);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetDigits
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::GetDigits(MpGetDigit &getDigit, INT32 lDxDevIdx)
{
   DV_TPT            lTpt[20];
   INT16             lStopGetDtmfCharLen        = 0;
   int               lIdx                       = 0;
   int               lTpLength                  = 0;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(lTpt, 0, sizeof(lTpt));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d GetDigit NOD:%d TC:%s ITC:%d MaxTime:%ds IDTimeOut:%dms",
         mHmpDxStruct[lDxDevIdx].dxHandle, getDigit.numOfDig,getDigit.teminateChars,
         getDigit.includeTerminateChar, getDigit.maxTime, getDigit.interDigitTimeOut);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DX16, lLogText);

   if(0 != getDigit.numOfDig)
   {
      lTpt[lIdx].tp_type   = IO_CONT;
      lTpt[lIdx].tp_termno = DX_MAXDTMF;
      lTpt[lIdx].tp_length = getDigit.numOfDig;
      lTpt[lIdx].tp_flags  = TF_MAXDTMF;
      lIdx++;
   }

   if(0 != (lStopGetDtmfCharLen = strlen(getDigit.teminateChars)))
   {
      for(int lTCIdx = 0; lTCIdx < lStopGetDtmfCharLen; lTCIdx++)
      {
         lTpLength = lTpLength | gDMValues[(unsigned int)getDigit.teminateChars[lTCIdx]];
      }
      lTpt[lIdx].tp_type   = IO_CONT;
      lTpt[lIdx].tp_termno = DX_DIGMASK;
      lTpt[lIdx].tp_length = lTpLength;
      lTpt[lIdx].tp_flags  = TF_DIGMASK;
      lIdx++;
   }

   if(0 != getDigit.maxTime)
   {
      lTpt[lIdx].tp_type   = IO_CONT;
      lTpt[lIdx].tp_termno = DX_MAXTIME;
      lTpt[lIdx].tp_length = getDigit.maxTime * 10; //10 sec in dialogic is 10 100ms so * 10
      lTpt[lIdx].tp_flags  = TF_MAXTIME;
      lIdx++;
   }

   if(0 == getDigit.interDigitTimeOut)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d InterDigitTimeOut Not Specified Error",
            mHmpDxStruct[lDxDevIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX17, lLogText);
      return false;       
   }
   lTpt[lIdx].tp_type   = IO_EOT;
   lTpt[lIdx].tp_termno = DX_IDDTIME; //DX_MAXSIL;
   lTpt[lIdx].tp_length = (int)(getDigit.interDigitTimeOut/100);//2000 ms in dialogic is 2000 100ms so / 100
   lTpt[lIdx].tp_flags  = TF_IDDTIME; //TF_MAXSIL;

   if(dx_getdig(mHmpDxStruct[lDxDevIdx].dxHandle, lTpt, &mHmpDxStruct[lDxDevIdx].digits, EV_ASYNC) != HMP_SUCCESS)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Get Digit Failed ECo:%ld ESr:%s",
            mHmpDxStruct[lDxDevIdx].dxHandle,
            ATDV_LASTERR (mHmpDxStruct[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mHmpDxStruct[lDxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX17, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Get Digit Issue Success",
         mHmpDxStruct[lDxDevIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DX18, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : PlayStop
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpDxDevice::StopChnlAct(INT32 lDxDevIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if(HMP_FAILURE == dx_stopch(mHmpDxStruct[lDxDevIdx].dxHandle, EV_ASYNC))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Stop Voice Channel Action Failed", 
            mHmpDxStruct[lDxDevIdx].dxHandle);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_DX19, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Stop Voice Channel Action Success", 
         mHmpDxStruct[lDxDevIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_DX20, lLogText);

   return true;
}


//----------------------------------------------------------------------
// METHOD NAME :CloseDxIOFiles
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN  HmpDxDevice::CloseDxIOFiles(int dxIdx, int fileCnt)
{
   for(int lFlIdx = 0; lFlIdx < fileCnt; lFlIdx++)
   {
      if(0 != mHmpDxStruct[dxIdx].iott[lFlIdx].io_fhandle)
      {
         close(mHmpDxStruct[dxIdx].iott[lFlIdx].io_fhandle);
         mHmpDxStruct[dxIdx].iott[lFlIdx].io_fhandle = 0;
      }
   }

   return true;
}
