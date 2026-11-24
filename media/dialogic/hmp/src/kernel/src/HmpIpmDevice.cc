static const char *id = "@(#) $Id: HmpIpmDevice.cc,v 1.1.2.4 2016/04/12 21:59:34 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpIpmDevice.cc
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
#include "HmpIpmDevice.h"

//----------------------------------------------------------------------
// METHOD NAME : HmpIpmDevice
// DESCRIPTION : Constructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpIpmDevice::HmpIpmDevice()
{
   memset(mHmpIpmStruct, 0, sizeof(HmpIpmStruct) * MAX_IPM_DEVICE);
}

//----------------------------------------------------------------------
// METHOD NAME : ~HmpIpmDevice
// DESCRIPTION : Destructor
// PARAMETERS  : None
// RETURN      : None
//----------------------------------------------------------------------
HmpIpmDevice::~HmpIpmDevice()
{
}

//----------------------------------------------------------------------
// METHOD NAME : OpenIpmDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::OpenIpmDevice()
{
   TEXT              lLogText[MAX_LOG_TEXT_LEN] = "";
   SC_TSINFO         lSc_tsinfo;

   RtspClient::RtspLogInit();

   for(UINT32 lIdx = 1; lIdx <= mNumOfIpmDevice; lIdx++)
   {
      mHmpIpmStruct[lIdx].ipmHandle = ipm_Open(mHmpIpmStruct[lIdx].ipmDeviceName, NULL, EV_SYNC);

      if(0 > mHmpIpmStruct[lIdx].ipmHandle)
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "IpmDevice:%s IpmH:%d ECo:%ld ESr:%s ipm_Open Failed",
               mHmpIpmStruct[lIdx].ipmDeviceName, mHmpIpmStruct[lIdx].ipmHandle,
               ATDV_LASTERR(mHmpIpmStruct[lIdx].ipmHandle),
               ATDV_ERRMSGP(mHmpIpmStruct[lIdx].ipmHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_IPM01, lLogText);
         return false;
      }
      
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "IpmDevice:%s IpmH:%d ipm_Open Success", 
            mHmpIpmStruct[lIdx].ipmDeviceName, 
            mHmpIpmStruct[lIdx].ipmHandle);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););

      lSc_tsinfo.sc_numts = 1;
      lSc_tsinfo.sc_tsarrayp = &mHmpIpmStruct[lIdx].scts;

      if (HMP_FAILURE == ipm_GetXmitSlot(mHmpIpmStruct[lIdx].ipmHandle, &lSc_tsinfo, EV_SYNC))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "IpmDevice:%s IpmH:%d ECo:%ld ESr:%s ipm_GetXmitSlot Failed",
               mHmpIpmStruct[lIdx].ipmDeviceName, mHmpIpmStruct[lIdx].ipmHandle, 
               ATDV_LASTERR(mHmpIpmStruct[lIdx].ipmHandle), 
               ATDV_ERRMSGP(mHmpIpmStruct[lIdx].ipmHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_IPM02, lLogText);
         return false;
      }
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "IpmDevice:%s IpmH:%d TimeSlot:%ld ipm_GetXmitSlot Success", 
               mHmpIpmStruct[lIdx].ipmDeviceName, mHmpIpmStruct[lIdx].ipmHandle,
               mHmpIpmStruct[lIdx].scts);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););

      mHmpIpmStruct[lIdx].mediaInfo.unCount = 1;
      mHmpIpmStruct[lIdx].mediaInfo.MediaData[0].eMediaType = MEDIATYPE_AUDIO_LOCAL_RTP_INFO;

      if(HMP_FAILURE == ipm_GetLocalMediaInfo(mHmpIpmStruct[lIdx].ipmHandle, &mHmpIpmStruct[lIdx].mediaInfo, EV_SYNC))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "IpmDevice:%s IpmH:%d ECo:%ld ESr:%s ipm_GetLocalMediaInfo Failed",
               mHmpIpmStruct[lIdx].ipmDeviceName, mHmpIpmStruct[lIdx].ipmHandle,
               ATDV_LASTERR(mHmpIpmStruct[lIdx].ipmHandle), 
               ATDV_ERRMSGP(mHmpIpmStruct[lIdx].ipmHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_IPM04, lLogText);
         return false;
      }
      mHmpIpmStruct[lIdx].audioPort = mHmpIpmStruct[lIdx].mediaInfo.MediaData[0].mediaInfo.PortInfo.unPortId;
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmDevice:%s IpmH:%d AudioPort:%d Local MediaInfo Success",
            mHmpIpmStruct[lIdx].ipmDeviceName, mHmpIpmStruct[lIdx].ipmHandle, mHmpIpmStruct[lIdx].audioPort); 
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : CloseIpmDevice
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::CloseIpmDevice()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = 1; lIdx <= mNumOfIpmDevice; lIdx++)
   {
      if(HMP_FAILURE == ipm_Close(mHmpIpmStruct[lIdx].ipmHandle, 0))
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN, 
               "IpmDevice:%s IpmH:%d ipm_Close Failed", 
               mHmpIpmStruct[lIdx].ipmDeviceName, mHmpIpmStruct[lIdx].ipmHandle);
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_IPM03, lLogText);
      }
      snprintf(lLogText, MAX_LOG_TEXT_LEN,
            "IpmDevice:%s IpmH:%d ipm_Close Success", 
            mHmpIpmStruct[lIdx].ipmDeviceName, mHmpIpmStruct[lIdx].ipmHandle);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   }

   RtspClient::RtspLogDeInit();

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : CreateRtspSession
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::CreateRtspSession(MpRequestDetails &mpReqDetails)
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";
   TEXT     lUrl[240 + 1] = "";

   strcpy(lUrl, mpReqDetails.mpReqData.stream.rtspUrl);
   mHmpIpmStruct[mFreeIpmIdx].rtspClient = new RtspClient(mHmpIpmStruct[mFreeIpmIdx].audioPort, 0);

   if(NULL == mHmpIpmStruct[mFreeIpmIdx].rtspClient)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d Device:%s Creating RTSP object Failed", 
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle, 
            mFreeIpmIdx, mHmpIpmStruct[mFreeIpmIdx].ipmDeviceName);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM05, lLogText);

      return false;
   }
   mHmpIpmStruct[mFreeIpmIdx].rtspObjCreated = true;

   if (mHmpIpmStruct[mFreeIpmIdx].rtspClient->CreateRTSPSession(lUrl, false) == HMP_FAILURE)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d Device:%s Creating RTSP Session Failed", 
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
            mFreeIpmIdx, mHmpIpmStruct[mFreeIpmIdx].ipmDeviceName);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM06, lLogText);
      
      return false;
   }
   mHmpIpmStruct[mFreeIpmIdx].rtspSesCreated = true;
   
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d Device:%s Creating RTSP Session Success", 
         mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
         mFreeIpmIdx, mHmpIpmStruct[mFreeIpmIdx].ipmDeviceName);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM07, lLogText);

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SetupRtspMediainfo
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
void HmpIpmDevice::SetupRtspMediainfo()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";
   int      lMediaCtr = 0;

   // clear the RemoteMediaInfo structures
   memset(&mHmpIpmStruct[mFreeIpmIdx].mediaInfo, 0, sizeof(IPM_MEDIA_INFO));

   // local audio
   if ( strncmp(mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetAudioCodecName(), "AMR", 3)==0)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d Setting Local Audio Media Info, AMR Codec", 
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
            mFreeIpmIdx);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM08, lLogText);

      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].eMediaType = MEDIATYPE_AUDIO_LOCAL_CODER_INFO;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eCoderType = CODER_TYPE_AMRNB_12_2k;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eFrameSize = CODER_FRAMESIZE_20;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unFramesPerPkt = 10;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eVadEnable = CODER_VAD_ENABLE;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unCoderPayloadType = mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetAudioPayloadType();
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unRedPayloadType = 97;
      ++lMediaCtr;
      
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d Setting Local Audio Media Info, AMR Codec Options", 
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
            mFreeIpmIdx);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM09, lLogText);

      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].eMediaType = MEDIATYPE_AUDIO_LOCAL_CODER_OPTIONS_INFO;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.AudioCoderOptionsInfo.unVersion = IPM_AUDIO_CODER_OPTIONS_INFO_VERSION;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.AudioCoderOptionsInfo.unCoderOptions=CODER_OPT_AMR_CMR_TRACK | CODER_OPT_AMR_OCTET; 
      //CODER_OPT_AMR_EFFICIENT;
      ++lMediaCtr;
   }
   else if ( strncmp(mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetAudioCodecName(), "PCMU", 4)==0)
   {
      // G.711
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d Setting Local Audio Media Info, g711 ULAW Codec", 
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle, mFreeIpmIdx);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM08, lLogText);

      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].eMediaType = MEDIATYPE_AUDIO_LOCAL_CODER_INFO;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eCoderType = CODER_TYPE_G711ULAW64K;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eFrameSize = CODER_FRAMESIZE_30;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unFramesPerPkt = 1;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eVadEnable = CODER_VAD_DISABLE;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unCoderPayloadType  = 0;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unCoderPayloadType = mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetAudioPayloadType();
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unRedPayloadType = 0;
      ++lMediaCtr;
   }
   else if ( strncmp(mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetAudioCodecName(), "PCMA", 4)==0)
   {
      // G.711
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d Setting Local Audio Media Info, g711 ALAW Codec", 
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle, mFreeIpmIdx);
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM09, lLogText);

      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].eMediaType = MEDIATYPE_AUDIO_LOCAL_CODER_INFO;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eCoderType = CODER_TYPE_G711ULAW64K;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eFrameSize = CODER_FRAMESIZE_30;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unFramesPerPkt = 1;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.eVadEnable = CODER_VAD_DISABLE;
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unCoderPayloadType = mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetAudioPayloadType();
      mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.CoderInfo.unRedPayloadType = 0;
      ++lMediaCtr;
   }

   // remote audio rtp port
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d Setting Remote Audio RTP Info", 
         mHmpIpmStruct[mFreeIpmIdx].ipmHandle, mFreeIpmIdx);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM10, lLogText);
   
   mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].eMediaType = MEDIATYPE_AUDIO_REMOTE_RTP_INFO;
   strcpy(mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.PortInfo.cIPAddress, mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetRTSPAudioIP());
   mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.PortInfo.unPortId = mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetRTSPAudioPort();
   ++lMediaCtr;

   // remote audio rtcp port
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d Setting Remote Audio RTCP Info", 
         mHmpIpmStruct[mFreeIpmIdx].ipmHandle, mFreeIpmIdx);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM11, lLogText);

   mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].eMediaType = MEDIATYPE_AUDIO_REMOTE_RTCP_INFO;
   strcpy(mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.PortInfo.cIPAddress, mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetRTSPAudioIP());
   mHmpIpmStruct[mFreeIpmIdx].mediaInfo.MediaData[lMediaCtr].mediaInfo.PortInfo.unPortId = mHmpIpmStruct[mFreeIpmIdx].rtspClient->GetRTSPAudioPort() + 1;
   ++lMediaCtr;

   mHmpIpmStruct[mFreeIpmIdx].mediaInfo.unCount = lMediaCtr;
}

//----------------------------------------------------------------------
// METHOD NAME : StartStream
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::StartStream()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";
   int      lRetIpm = 0;
   
   this->SetupRtspMediainfo();
   
   lRetIpm = ipm_StartMedia(mHmpIpmStruct[mFreeIpmIdx].ipmHandle, 
         &mHmpIpmStruct[mFreeIpmIdx].mediaInfo, DATA_IP_RECEIVEONLY, EV_ASYNC);
   
   if(HMP_FAILURE == lRetIpm)
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d Device:%s ipm_StartMedia Failed Error:%ld ErrStr:%s",
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
            mFreeIpmIdx,
            ATDV_NAMEP(mHmpIpmStruct[mFreeIpmIdx].ipmHandle),
            ATDV_LASTERR(mHmpIpmStruct[mFreeIpmIdx].ipmHandle),
            ATDV_ERRMSGP(mHmpIpmStruct[mFreeIpmIdx].ipmHandle));
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM12, lLogText);
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d  device name:%s ipm_StartMedia Success",
         mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
         mFreeIpmIdx,
         ATDV_NAMEP(mHmpIpmStruct[mFreeIpmIdx].ipmHandle));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM13, lLogText);
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : TearSessions
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::TearSessions(int idx)
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   if ( ipm_Stop(mHmpIpmStruct[idx].ipmHandle, STOP_MEDIA, EV_SYNC) < 0 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d TearSessions:ipm_Stop Failed for Device Name = %s",
            mHmpIpmStruct[idx].ipmHandle,
            idx,
            ATDV_NAMEP(mHmpIpmStruct[idx].ipmHandle));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM14, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d TearSessions:ipm_Stop Success for Device Name = %s",
         mHmpIpmStruct[idx].ipmHandle,
         idx,
         ATDV_NAMEP(mHmpIpmStruct[idx].ipmHandle));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM15, lLogText);

   if(true == mHmpIpmStruct[idx].rtspSesCreated)
   {
   //   mHmpIpmStruct[idx].rtspClient->Teardown();
      mHmpIpmStruct[idx].rtspSesCreated = false;
   }
   
   if(true == mHmpIpmStruct[idx].rtspObjCreated)
   {
      delete mHmpIpmStruct[idx].rtspClient;
      mHmpIpmStruct[idx].rtspClient = NULL;
      mHmpIpmStruct[idx].rtspObjCreated = false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : StopIpmMedia
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::StopStream()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   if ( ipm_Stop(mHmpIpmStruct[mFreeIpmIdx].ipmHandle, STOP_MEDIA, EV_SYNC) < 0 )
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d ipm_Stop Failed for Device Name = %s",
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
            mFreeIpmIdx,
            ATDV_NAMEP(mHmpIpmStruct[mFreeIpmIdx].ipmHandle));
      T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM14, lLogText);
      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d ipm_Stop Success for Device Name = %s",
         mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
         mFreeIpmIdx,
         ATDV_NAMEP(mHmpIpmStruct[mFreeIpmIdx].ipmHandle));
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM15, lLogText);
   
   if(true == mHmpIpmStruct[mFreeIpmIdx].rtspSesCreated)
   {
      if(true == mHmpIpmStruct[mFreeIpmIdx].rtspClient->Teardown())
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "IpmH:%d Index:%d Tear Session Success for Device Name = %s",
               mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
               mFreeIpmIdx,
               ATDV_NAMEP(mHmpIpmStruct[mFreeIpmIdx].ipmHandle));
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      }
      else
      {
         snprintf(lLogText, MAX_LOG_TEXT_LEN,
               "IpmH:%d Index:%d Tear Session Failed for Device Name = %s",
               mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
               mFreeIpmIdx,
               ATDV_NAMEP(mHmpIpmStruct[mFreeIpmIdx].ipmHandle));
         TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      }
      mHmpIpmStruct[mFreeIpmIdx].rtspSesCreated = false;
   }

   if(true == mHmpIpmStruct[mFreeIpmIdx].rtspObjCreated)
   {
      delete mHmpIpmStruct[mFreeIpmIdx].rtspClient;
      mHmpIpmStruct[mFreeIpmIdx].rtspClient = NULL;
      mHmpIpmStruct[mFreeIpmIdx].rtspObjCreated = false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : RtspPlay
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::RtspPlay()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   if(true != mHmpIpmStruct[mFreeIpmIdx].rtspClient->Play())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d RTSP Play Error",
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
            mFreeIpmIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM20, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d RTSP Play Started",
         mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
         mFreeIpmIdx);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM16, lLogText);
 
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : RtspStop
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::RtspStop()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   if(true != mHmpIpmStruct[mFreeIpmIdx].rtspClient->Pause())
   {
      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "IpmH:%d Index:%d RTSP Play Stop Error",
            mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
            mFreeIpmIdx);
      TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog(HMP_IPM21, lLogText);

      return false;
   }

   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "IpmH:%d Index:%d RTSP Play Stoped",
         mHmpIpmStruct[mFreeIpmIdx].ipmHandle,
         mFreeIpmIdx);
   T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM17, lLogText);

   return true;
}


//----------------------------------------------------------------------
// METHOD NAME : SetIdleIpmDevice
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : INT32
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::SetIdleIpmDevice(INT32 ipmIdx)
{
   mHmpIpmStruct[ipmIdx].deviceState = DEVICE_IDLE;
   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : SetFreeIpmDevice
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : INT32
//----------------------------------------------------------------------
BOOLEAN HmpIpmDevice::SetFreeIpmDevice()
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = 1; lIdx <= mNumOfIpmDevice; lIdx++)
   {
      if(DEVICE_IDLE == mHmpIpmStruct[lIdx].deviceState)
      {
         mHmpIpmStruct[lIdx].deviceState = DEVICE_BUSY;
         mFreeIpmIdx = lIdx;
         mHmpIpmStruct[lIdx].index = lIdx;
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Index:%d Device:%s Free Ipm Device", 
               mFreeIpmIdx, mHmpIpmStruct[lIdx].ipmDeviceName);
         T (gTrace, printf("%s: %s\n", gProcessName, lLogText););
         gLog.GenerateLog(HMP_IPM18, lLogText);
         return true;
      }
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, "No Free Ipm Device");
   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog(HMP_IPM19, lLogText);

   return false;
}

//----------------------------------------------------------------------
// METHOD NAME : CheckLiveStream
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : INT32
//----------------------------------------------------------------------
INT16 HmpIpmDevice::CheckLiveStream(TEXT * rtspUrl)
{
   TEXT     lLogText[MAX_LOG_TEXT_LEN] = "";

   for(UINT32 lIdx = 1; lIdx <= mIpmLiveStreamCount; lIdx++)
   {
      if( 0 == strcmp(rtspUrl, mHmpIpmStruct[lIdx].rtspUrl))
      {
         return lIdx;
      }
   }
   snprintf(lLogText, MAX_LOG_TEXT_LEN, 
         "No Matching Live Streams Url:%s", rtspUrl);
   TERR (gTrace, printf("%s: %s\n", gProcessName, lLogText););

   return -1;
}
