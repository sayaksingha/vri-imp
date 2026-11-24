// @(#) $Id: RtspClient.h,v 1.1.2.1 2012/02/21 08:31:39 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//        RtspClient.h  
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2012
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
#if !defined(RTSP_H)
#define RTSP_H

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"

#ifdef SUPPORT_REAL_RTSP
#include "../RealRTSP/include/RealRTSP.hh"
#endif
#include "RtspEnv.h"
#include <string>

#define RTSP_INIT                0
#define RTSP_OPTIONS_SENT_RCVD   (1 << 0)
#define RTSP_DESCRIBE_SENT       (1 << 1)
#define RTSP_DESCRIBE_SENT_RCVD  (1 << 2)
#define RTSP_CREATE_MED_SESSION  (1 << 3)
#define RTSP_SETUP_AUD_SESSION   (1 << 4)
#define RTSP_SETUP_VID_SESSION   (1 << 5)


class RtspClient {
public:
   RtspClient(unsigned short ipmRxAudioPort, unsigned short ipmRxVideoPort);

   ~RtspClient() {; }
   int CreateRTSPSession(const char* url, bool startPlayAlso);
   unsigned GetRTSPVideoProfile(); 

   int Play(); 
   void armDroppedConnection(double timer);
   int Pause();
   int Teardown();

   int Reset(); 
   int PrintSDP(); 

   int PrintAudioSessionInfo() ;

   int PrintVideoSessionInfo(); 

   
   int GetAudioPayloadType() ;
   int GetVideoPayloadType() ;
   const char* GetAudioCodecName() ;
   const char* GetVideoCodecName() ;
   unsigned short GetRTSPAudioPort() ;
   unsigned short GetRTSPVideoPort() ;
   const char* GetRTSPAudioIP();
   const char* GetRTSPVideoIP();
   const char* decoder_config_info();
 
   int socketNum(); 
   static void timeoutHandler(void* clientData);
   void armKeepAlive();
   static void keepAliveHandler(void* clientData);
   void SendKeepAlive(); 
   void SessionTerminated();
   static void RtspLogInit();
   static void PrintRtspLog(char *);
   static void RtspLogDeInit();

private:

   RtspClient() {; }

   char* SendOptionsCmdAndGetResponse(const char* url); 

   char* getSDPDescriptionFromURL(const char* url, char const* username = NULL, char const* password = NULL,
                               char const* proxyServerName = NULL,
                               unsigned short proxyServerPortNum = 0,
                               unsigned short clientStartPort = 0) ;
   
   unsigned short m_ipmRxAudioPort;
   unsigned short m_ipmRxVideoPort;
   unsigned short m_rtspServerAudioPort;
   unsigned short m_rtspServerVideoPort;
   unsigned long  m_state;
   std::string m_RTSPVideoIP;
   std::string m_RTSPAudioIP;
   int m_audioPayloadType;
   int m_videoPayloadType;
   std::string m_videoCodecName;
   std::string m_audioCodecName;
   std::string m_url;
   NetAddress m_unRtspAddress;
   std::string m_RTSPAddr;
   unsigned short m_rtspPortNum;
   char const* m_urlSuffix;
   Medium* m_ourClient ;
   UsageEnvironment* m_pEnv;
   MediaSession* session ;
   MediaSubsession* m_pVidSubSession;
   MediaSubsession* m_pAudSubSession;

   double m_duration ;
   double m_durationSlop ;
   double m_initialSeekTime ;
   double m_scale ;
   RtspEnv* m_pRTSPEnv;

protected:
   static FILE *mRtspLogFile;
};
#endif

