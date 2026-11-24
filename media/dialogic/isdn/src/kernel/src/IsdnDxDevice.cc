static const char *id = "@(#) $Id: IsdnDxDevice.cc,v 1.1.4.3 2016/04/12 21:59:36 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       IsdnDxDevice.cc
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
#include "IsdnDxDevice.h"

//----------------------------------------------------------------------
// METHOD NAME : IsdnDxDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnDxDevice::IsdnDxDevice()
{
   memset(mIsdnDx, 0, sizeof(IsdnDx) * ISDN_MAX_DX_DEVICE);
   mNumOfDxDevice    = 0;
}

//----------------------------------------------------------------------
// METHOD NAME : ~IsdnDxDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
IsdnDxDevice::~IsdnDxDevice()
{
}

//----------------------------------------------------------------------
// METHOD NAME : OpenDxDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::OpenDxDevice(INT32 dxHdlIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   mIsdnDx[dxHdlIdx].dxHandle = dx_open(mIsdnDx[dxHdlIdx].dxDeviceName, 0);
   if(0 > mIsdnDx[dxHdlIdx].dxHandle)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_open Failed",
            mIsdnDx[dxHdlIdx].dxDeviceName, mIsdnDx[dxHdlIdx].dxHandle,
            ATDV_LASTERR(mIsdnDx[dxHdlIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[dxHdlIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX01, lLogText);

      if(0 < mIsdnDx[dxHdlIdx].dxHandle)
         dx_close(mIsdnDx[dxHdlIdx].dxHandle);
      return false;
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_open Success",
         mIsdnDx[dxHdlIdx].dxDeviceName, mIsdnDx[dxHdlIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetDxXmitslot
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::GetDxXmitslot(INT32 dxHdlIdx)
{
   SC_TSINFO         lSc_tsinfo;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   lSc_tsinfo.sc_numts = 1;
   lSc_tsinfo.sc_tsarrayp = &mIsdnDx[dxHdlIdx].scts;

   if (ISDN_DX_FAILURE == dx_getxmitslot(mIsdnDx[dxHdlIdx].dxHandle, &lSc_tsinfo)) 
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_getxmitslot Failed",
            mIsdnDx[dxHdlIdx].dxDeviceName,  mIsdnDx[dxHdlIdx].dxHandle,
            ATDV_LASTERR(mIsdnDx[dxHdlIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[dxHdlIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX02, lLogText);
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d scts:%ld dx_getxmitslot Success", 
         mIsdnDx[dxHdlIdx].dxDeviceName, mIsdnDx[dxHdlIdx].dxHandle, mIsdnDx[dxHdlIdx].scts);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : CloseDxDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::CloseDxDevice(INT32 dxHdlIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if(ISDN_DX_FAILURE == dx_close(mIsdnDx[dxHdlIdx].dxHandle))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_close Failed", 
            mIsdnDx[dxHdlIdx].dxDeviceName, mIsdnDx[dxHdlIdx].dxHandle);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX03, lLogText);
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_close Success", 
         mIsdnDx[dxHdlIdx].dxDeviceName, mIsdnDx[dxHdlIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   return true;
}


//----------------------------------------------------------------------
// METHOD NAME : GetFreeDxDevice
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : INT32
//----------------------------------------------------------------------
BOOLEAN  IsdnDxDevice::FreeDxDevice(INT32 dxHdlIdx)
{
   mIsdnDx[dxHdlIdx].deviceState = ISDN_DX_DEVICE_IDLE;

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetFreeDxDevice
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : INT32
//----------------------------------------------------------------------
INT32 IsdnDxDevice::GetFreeDxDevice()
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = 1; lIdx <= mNumOfDxDevice; lIdx++)
   {
      if(ISDN_DX_DEVICE_IDLE == mIsdnDx[lIdx].deviceState)
      {
         mIsdnDx[lIdx].deviceState = ISDN_DX_DEVICE_BUSY;
         return mIsdnDx[lIdx].index;
      }
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "No Free Dx Device");
   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DX06, lLogText);


   return -1;
}

//----------------------------------------------------------------------
// METHOD NAME : DxListen
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::DxListen(int dxDevIdx, long &scts)
{
   SC_TSINFO         lSc_tsinfo_dt;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   lSc_tsinfo_dt.sc_numts = 1;
   lSc_tsinfo_dt.sc_tsarrayp = &scts;


   if (dx_listen(mIsdnDx[dxDevIdx].dxHandle, &lSc_tsinfo_dt) == ISDN_DX_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_listen Failed",
            mIsdnDx[dxDevIdx].dxDeviceName, mIsdnDx[dxDevIdx].dxHandle, 
            ATDV_LASTERR(mIsdnDx[dxDevIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[dxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX04, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_listen Success",
         mIsdnDx[dxDevIdx].dxDeviceName, mIsdnDx[dxDevIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DX05, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : DxUnlisten
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::DxUnlisten(int dxDevIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if (dx_unlisten(mIsdnDx[dxDevIdx].dxHandle) == ISDN_DX_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d ECo:%ld ESr:%s dx_unlisten Failed",
            mIsdnDx[dxDevIdx].dxDeviceName,mIsdnDx[dxDevIdx].dxHandle,
            ATDV_LASTERR(mIsdnDx[dxDevIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[dxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX07, lLogText);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxDevice:%s DxH:%d dx_unlisten Success",
            mIsdnDx[dxDevIdx].dxDeviceName, mIsdnDx[dxDevIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX08, lLogText);
   }

   return true;
}


//----------------------------------------------------------------------
// METHOD NAME : Record
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::Record(MpRecord &recordDetails, INT32 lDxDevIdx)
{
   DV_TPT            lTpt[20];
   DX_XPB            lXpb;
   INT16             lStopPlayCharLen           = 0;
   int               lIdx                       = 0;
   int               lTpLength                  = 0;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(lTpt, 0, sizeof(lTpt));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Voice Record TC:%s RecTime:%dms FF:%s",
         mIsdnDx[lDxDevIdx].dxHandle, recordDetails.dtmfStopRecCharSet,
         recordDetails.recTime, (0 == recordDetails.fileFormat) ? "WAV" : "VOX");
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DX09, lLogText);

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
      //lXpb.wDataFormat        = DATA_FORMAT_DIALOGIC_ADPCM;
      lXpb.wDataFormat        = DATA_FORMAT_MULAW;
      lXpb.nSamplesPerSec     = DRT_8KHZ;
      lXpb.wBitsPerSample     = 8;
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
            mIsdnDx[lDxDevIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX10, lLogText);

      return false;
   }

   if (dx_clrdigbuf(mIsdnDx[lDxDevIdx].dxHandle) != 0) 
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Clear Digit Buffer Failed ECo:%ld ESr:%s",
            mIsdnDx[lDxDevIdx].dxHandle, 
            ATDV_LASTERR (mIsdnDx[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[lDxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX11, lLogText);

      return false;
   }

   lTpt[lIdx].tp_type   = IO_EOT;
   lTpt[lIdx].tp_termno = DX_MAXTIME;
   lTpt[lIdx].tp_length = (int)(recordDetails.recTime/100);//2000 ms in dialogic is 2000 100ms so / 100
   lTpt[lIdx].tp_flags  = TF_MAXTIME;

   if(0 == mIsdnDx[lDxDevIdx].iott[0].io_fhandle)
   {
      if((mIsdnDx[lDxDevIdx].iott[0].io_fhandle = open(recordDetails.voiceFile, O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d open  Failed File:%s Errno:%d  ErrStr:%s",
               mIsdnDx[lDxDevIdx].dxHandle, recordDetails.voiceFile, errno, strerror(errno));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_DX10, lLogText);

         return false;
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d open  Success File:%s FilePtr:%d",
            mIsdnDx[lDxDevIdx].dxHandle, recordDetails.voiceFile, mIsdnDx[lDxDevIdx].iott[0].io_fhandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX21, lLogText);
   }

   mIsdnDx[lDxDevIdx].iott[0].io_type    = IO_DEV | IO_EOT ;
   mIsdnDx[lDxDevIdx].iott[0].io_offset  =  0 ;
   mIsdnDx[lDxDevIdx].iott[0].io_length  = -1 ;
   mIsdnDx[lDxDevIdx].iott[0].io_nextp = NULL;
   mIsdnDx[lDxDevIdx].iott[0].io_prevp = NULL;


   if(dx_reciottdata(mIsdnDx[lDxDevIdx].dxHandle, &mIsdnDx[lDxDevIdx].iott[0], lTpt, &lXpb, EV_ASYNC) != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d record File:%s Failed ECo:%ld ESr:%s",
            mIsdnDx[lDxDevIdx].dxHandle, recordDetails.voiceFile,
            ATDV_LASTERR (mIsdnDx[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[lDxDevIdx].dxHandle));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX10, lLogText);

      CloseDxIOFiles(lDxDevIdx, 1);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d record File:%s Success",
            mIsdnDx[lDxDevIdx].dxHandle, recordDetails.voiceFile);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX12, lLogText);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : Play
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::Play(MpPlay &playDetails, INT32 lDxDevIdx)
{
   DV_TPT            lTpt[20];
   DX_XPB            lXpb;
   INT16             lStopPlayCharLen           = 0;
   int               lIdx                       = 0;
   int               lTpLength                  = 0;
   DV_TPT*           pTpt                       = NULL;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(lTpt, 0, sizeof(lTpt));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Voice Play FC:%d TC:%s DE:%d RP:%d FF:%s NonBl:%d",
         mIsdnDx[lDxDevIdx].dxHandle, playDetails.vFileCount, 
         playDetails.dtmfStopPlayCharSet, playDetails.dtmfEnable, playDetails.repeatPlay, 
         (0 == playDetails.fileFormat) ? "WAV" : "VOX", playDetails.nonBlocking);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DX13, lLogText);

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

   if( 1 == playDetails.dtmfEnable)
   {
      pTpt = lTpt;
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
         lIdx++;
      }
      else
      {
         //Terminate play for all the Dtmf, so configure Termination condn at dtmf len = 1
         lTpt[lIdx].tp_type   = IO_EOT;
         lTpt[lIdx].tp_termno = DX_MAXDTMF;
         lTpt[lIdx].tp_length = 1;
         lTpt[lIdx].tp_flags  = TF_MAXDTMF;
         lIdx++;
      }

      if (dx_clrdigbuf(mIsdnDx[lDxDevIdx].dxHandle) != 0) 
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Clear Digit Buffer Failed ECo:%ld ESr:%s",
               mIsdnDx[lDxDevIdx].dxHandle, 
               ATDV_LASTERR (mIsdnDx[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[lDxDevIdx].dxHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(ISDN_DX11, lLogText);

         return false;
      }
   }

   for(int lFile = 0; lFile < playDetails.vFileCount; lFile++)
   {
      if(0 == mIsdnDx[lDxDevIdx].iott[lFile].io_fhandle)
      {
         if((mIsdnDx[lDxDevIdx].iott[lFile].io_fhandle = 
                  open(playDetails.voiceFile[lFile], O_RDONLY, 0666)) == -1)
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "DxH:%d open Failed File:%s FileCount:%d Errno:%d ErrStr:%s", 
                  mIsdnDx[lDxDevIdx].dxHandle, playDetails.voiceFile[lFile], 
                  lFile, errno, strerror(errno));
            TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DX14, lLogText);

            for(int lFlOpn = 0; lFlOpn < lFile; lFlOpn++)
            {
               //close all previous opened files
               if(0 != mIsdnDx[lDxDevIdx].iott[lFlOpn].io_fhandle)
               {
                  close(mIsdnDx[lDxDevIdx].iott[lFlOpn].io_fhandle);
                  mIsdnDx[lDxDevIdx].iott[lFlOpn].io_fhandle = 0;
               }
            }

            return false;
         }
         else
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN, 
                  "DxH:%d open Success File:%s FileCount:%d", 
                  mIsdnDx[lDxDevIdx].dxHandle, playDetails.voiceFile[lFile], 
                  lFile + 1);
            T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
            gLog.GenerateLog(ISDN_DX21, lLogText);

            mIsdnDx[lDxDevIdx].iott[lFile].io_type    = 
               (lFile == playDetails.vFileCount-1) ? IO_DEV | IO_EOT : IO_DEV ;
            mIsdnDx[lDxDevIdx].iott[lFile].io_offset  = 0;
            mIsdnDx[lDxDevIdx].iott[lFile].io_length  = -1;
            mIsdnDx[lDxDevIdx].iott[lFile].io_nextp   = NULL;
            mIsdnDx[lDxDevIdx].iott[lFile].io_prevp   = NULL;   
         }
      }
   }

   //if(dx_playiottdata (mIsdnDx[lDxDevIdx].dxHandle, &mIsdnDx[lDxDevIdx].iott[0], lTpt, &lXpb, EV_ASYNC) != 0)
   if(dx_playiottdata (mIsdnDx[lDxDevIdx].dxHandle, &mIsdnDx[lDxDevIdx].iott[0], pTpt, &lXpb, EV_ASYNC) != 0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d play File:%s FileCount:%d Failed ECo:%ld ESr:%s", 
            mIsdnDx[lDxDevIdx].dxHandle, playDetails.voiceFile[0], playDetails.vFileCount,
            ATDV_LASTERR (mIsdnDx[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[lDxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX14, lLogText);

      CloseDxIOFiles(lDxDevIdx, playDetails.vFileCount);

      return false;
   }
   else
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d play File:%s FileCount:%d Success",
            mIsdnDx[lDxDevIdx].dxHandle, playDetails.voiceFile[0], playDetails.vFileCount);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX15, lLogText);
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : GetDigits
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::GetDigits(MpGetDigit &getDigit, INT32 lDxDevIdx)
{
   DV_TPT            lTpt[20];
   INT16             lStopGetDtmfCharLen        = 0;
   int               lIdx                       = 0;
   int               lTpLength                  = 0;
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   memset(lTpt, 0, sizeof(lTpt));

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d GetDigit NOD:%d TC:%s ITC:%d MaxTime:%ds IDTimeOut:%dms",
         mIsdnDx[lDxDevIdx].dxHandle, getDigit.numOfDig,getDigit.teminateChars,
         getDigit.includeTerminateChar, getDigit.maxTime, getDigit.interDigitTimeOut);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DX16, lLogText);

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
            mIsdnDx[lDxDevIdx].dxHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX17, lLogText);
      return false;       
   }
   lTpt[lIdx].tp_type   = IO_EOT;
   lTpt[lIdx].tp_termno = DX_IDDTIME; //DX_MAXSIL;
   lTpt[lIdx].tp_length = (int)(getDigit.interDigitTimeOut/100);//2000 ms in dialogic is 2000 100ms so / 100
   lTpt[lIdx].tp_flags  = TF_IDDTIME; //TF_MAXSIL;

   if(dx_getdig(mIsdnDx[lDxDevIdx].dxHandle, lTpt, &mIsdnDx[lDxDevIdx].digits, EV_ASYNC) != ISDN_DX_SUCCESS)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Get Digit Failed ECo:%ld ESr:%s",
            mIsdnDx[lDxDevIdx].dxHandle,
            ATDV_LASTERR (mIsdnDx[lDxDevIdx].dxHandle), ATDV_ERRMSGP(mIsdnDx[lDxDevIdx].dxHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX17, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Get Digit Issue Success",
         mIsdnDx[lDxDevIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DX18, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : PlayStop
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN IsdnDxDevice::StopChnlAct(INT32 lDxDevIdx)
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";

   if(ISDN_DX_FAILURE == dx_stopch(mIsdnDx[lDxDevIdx].dxHandle, EV_ASYNC))
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Stop Voice Channel Action Failed", 
            mIsdnDx[lDxDevIdx].dxHandle);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(ISDN_DX19, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, "DxH:%d Stop Voice Channel Action Success", 
         mIsdnDx[lDxDevIdx].dxHandle);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(ISDN_DX20, lLogText);

   return true;
}


//----------------------------------------------------------------------
// METHOD NAME :CloseDxIOFiles
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN  IsdnDxDevice::CloseDxIOFiles(int dxIdx, int fileCnt)
{
   for(int lFlIdx = 0; lFlIdx < fileCnt; lFlIdx++)
   {
      if(0 != mIsdnDx[dxIdx].iott[lFlIdx].io_fhandle)
      {
         close(mIsdnDx[dxIdx].iott[lFlIdx].io_fhandle);
         mIsdnDx[dxIdx].iott[lFlIdx].io_fhandle = 0;
      }
   }

   return true;
}
