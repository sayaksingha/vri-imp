static const char *id = "@(#) $Id: RtspClient.cc,v 1.1.2.1 2012/02/21 08:31:39 shijuc Exp $";
#include "RtspClient.h"
#include "RtspEnv.h"
//#include "mmstream.h"
#define DEBUG

using namespace std;

FILE* RtspClient::mRtspLogFile = NULL;
static char gTimeString[100] = "";
static char* GetCurTime();

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
RtspClient::RtspClient(unsigned short ipmRxAudioPort, unsigned short ipmRxVideoPort) 
{
   Reset();
   m_ipmRxVideoPort = ipmRxVideoPort;
   m_ipmRxAudioPort = ipmRxAudioPort;
   m_pRTSPEnv = RtspEnv::Instance();  
   m_pEnv = m_pRTSPEnv->GetEnv();
   m_ourClient = RTSPClient::createNew(*m_pEnv, 0, m_pRTSPEnv->GetAppName(), 0);
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::Pause() 
{
   PrintRtspLog("Inside Pause");

   RTSPClient* rtspClient = (RTSPClient*)m_ourClient;
   if (rtspClient) {
      if (session) {
         if(1 ==  rtspClient->pauseMediaSession (*session))
            return 1;
      }
   }
   return 0;
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::Teardown() 
{
   PrintRtspLog("Inside Teardown");

   RTSPClient* rtspClient = (RTSPClient*)m_ourClient;
   if (rtspClient) {
      if (session) {
         if(1 == rtspClient->teardownMediaSession(*session))
            return 1;
      }
   }
   return 0;  
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::CreateRTSPSession(const char* url, bool startPlayAlso) 
{
   char lRtspLogText[100] = "";

   m_state = RTSP_INIT;
   char str[200];

   m_url.assign(url);
   if ( RTSPClient::parseRTSPURL (*m_pEnv, m_url.c_str(), m_unRtspAddress, m_rtspPortNum, &m_urlSuffix) == False) 
   {
      sprintf(str,"m_url.c_str(%s)\n",m_url.c_str());
      *m_pEnv << str;
      return -1;
   }
   else
   {
      sprintf(lRtspLogText, "Parsing url success. m_rtspPortNum:%d", m_rtspPortNum);
      PrintRtspLog(lRtspLogText);
   }

   unsigned x = *((unsigned*)m_unRtspAddress.data());
   char temp[16];
   sprintf(temp, "%d.%d.%d.%d", x & 0xff, (x>>8)& 0xff, (x>>16) & 0xff, (x>>24) & 0xff);
   m_RTSPAddr.assign(temp);

   char* optionsResponse = SendOptionsCmdAndGetResponse(m_url.c_str());
   if (optionsResponse == NULL) 
   {
//      *m_pEnv << "NULL options response \n";
      PrintRtspLog("NULL options response");
      return -1;
   }
   else
   {
      PrintRtspLog("Options response received");
      delete [] optionsResponse;
   }

   char* sdpDescription = getSDPDescriptionFromURL(m_url.c_str());
   if (sdpDescription == NULL) 
   {
//      *m_pEnv << "NULL sdp description response \n";
      PrintRtspLog("NULL sdp description response");
      return -1;
   }
   m_state = RTSP_OPTIONS_SENT_RCVD;

   session = MediaSession::createNew(*m_pEnv, sdpDescription);
   delete[] sdpDescription;

   if (session == NULL) {
  //    *m_pEnv << "Failed to create a MediaSession object from the SDP description: " << m_pEnv->getResultMsg() << "\n";

      PrintRtspLog("Failed to create a MediaSession object from the SDP description");
      return -1;
   } else if (!session->hasSubsessions()) {
    //  *m_pEnv << "This session has no media subsessions (i.e., \"m=\" lines)\n";
      PrintRtspLog("This session has no media subsessions");
      return -1;
   }

   // Then, setup the "RTPSource"s for the session:
   MediaSubsessionIterator iter(*session);
   MediaSubsession *subsession;
   while ((subsession = iter.next()) != NULL) {
      if (strstr(subsession->mediumName(), "audio") || strstr(subsession->mediumName(), "AUDIO"))
         subsession->setClientPortNum(m_ipmRxAudioPort);
      if (strstr(subsession->mediumName(), "video") || strstr(subsession->mediumName(), "VIDEO"))
         subsession->setClientPortNum(m_ipmRxVideoPort);
   }

   // Now, send the SETUPs
   iter.reset(); // go to head 
   while ((subsession = iter.next()) != NULL) {
#if PRINT_RTSP_OUTPUT
      *m_pEnv << "Subsession name: " << subsession->mediumName() << "\n";
#endif
      if (subsession->clientPortNum() == 0) continue; // port # was not set

      if (!((RTSPClient*)m_ourClient)->setupMediaSubsession(*subsession,
               False, False)) {
         *m_pEnv << "Failed to setup \"" << subsession->mediumName()
            << "/" << subsession->codecName()
            << "\" subsession: " << m_pEnv->getResultMsg() << "\n";
      } else {
#if PRINT_RTSP_OUTPUT
         *m_pEnv << "Setup \"" << subsession->mediumName()
            << "/" << subsession->codecName()
            << "\" subsession (client ports " << subsession->clientPortNum()
            << "-" << subsession->clientPortNum()+1 << ")\n";
#endif
         if (strstr(subsession->mediumName(), "audio") || strstr(subsession->mediumName(), "AUDIO"))
         {
            m_rtspServerAudioPort = subsession->serverPortNum; 
            m_audioPayloadType = subsession->rtpPayloadFormat();
            m_audioCodecName.assign( subsession->codecName());
            m_pAudSubSession = subsession;

            struct sockaddr_in serverAddr;
            serverAddr.sin_addr.s_addr =m_pAudSubSession->connectionEndpointAddress ();
            if (serverAddr.sin_addr.s_addr)
               m_RTSPAudioIP.assign(inet_ntoa(serverAddr.sin_addr));
            else {
               m_RTSPAudioIP.assign(m_RTSPAddr);
            }
         }
         if (strstr(subsession->mediumName(), "video") || strstr(subsession->mediumName(), "VIDEO"))
         {
            m_rtspServerVideoPort = subsession->serverPortNum; 
            m_videoPayloadType = subsession->rtpPayloadFormat(); 
            m_videoCodecName.assign(subsession->codecName());
            m_pVidSubSession = subsession;
            struct sockaddr_in serverAddr;
            serverAddr.sin_addr.s_addr =m_pVidSubSession->connectionEndpointAddress ();
            if (serverAddr.sin_addr.s_addr)
               m_RTSPVideoIP.assign(inet_ntoa(serverAddr.sin_addr));
            else {
               m_RTSPVideoIP.assign(m_RTSPAddr);
            }

         }
      }
   }

   //if (m_pVidSubSession == NULL || m_pAudSubSession == NULL) 
   if (m_pAudSubSession == NULL) 
   {
      PrintRtspLog("Audio Session is NULL");
      return -1;
   } 

   if (startPlayAlso) 
   {
      Play();
   }
   return 0;
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::Play() 
{
   double startTime = 0.0;
   double endTime = m_pAudSubSession->playEndTime();
   //double endTime = m_pVidSubSession->playEndTime();
   //double endTime = 70.0; 
   double scale = 1.0;


   if (m_ourClient == NULL || session == NULL) 
   {
      PrintRtspLog("m_ourClient or session is NULL");
      return False;
   }
   RTSPClient* rtspClient = (RTSPClient*)m_ourClient;
   if(0 == rtspClient->playMediaSession(*session, (float)startTime, (float)endTime, (float)scale))
      return 0;
   else
      return 1;
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
const char* RtspClient::decoder_config_info() 
{
   return m_pVidSubSession->fmtp_config();
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::SessionTerminated() 
{
   //m_pCMMStream->StopMedia();
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::armDroppedConnection(double timer) 
{
   m_pRTSPEnv->armDroppedConnection(timer, this);
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::armKeepAlive() 
{
   int secondsToDelay = 19;
   int uSecsToDelay = (int)(secondsToDelay*1000000.0);
   m_pEnv->taskScheduler().scheduleDelayedTask(uSecsToDelay, keepAliveHandler, this);
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::SendKeepAlive() 
{
   int secondsToDelay = 19;
   int uSecsToDelay = (int)(secondsToDelay*1000000.0);
   m_pEnv->taskScheduler().scheduleDelayedTask(uSecsToDelay, keepAliveHandler, this);

}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::timeoutHandler(void * clientData) 
{
   RtspClient* pRTSPClient = (RtspClient*)clientData;
   pRTSPClient->SendKeepAlive();
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::keepAliveHandler(void * clientData) 
{
   char lRtspLogText[100] = "";
   RtspClient* pRTSPClient = (RtspClient*)clientData;

   sprintf(lRtspLogText, "keep alive timer fired for %p", pRTSPClient);
   PrintRtspLog(lRtspLogText);
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::Reset() 
{
   m_audioPayloadType = -1;
   m_videoPayloadType = -1;
   m_rtspServerAudioPort = 65535;
   m_rtspServerVideoPort = 65535;
   m_videoCodecName.assign("") ;
   m_audioCodecName.assign("") ;
   m_duration = 0;
   m_durationSlop = -1.0; // extra seconds to play at the end
   m_initialSeekTime = 0.0f;
   m_scale = 1.0f;
   m_pAudSubSession = NULL;
   m_pVidSubSession = NULL;
   return 0;
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::PrintSDP() 
{
   *m_pEnv << m_pAudSubSession->savedSDPLines() << "\n";
   *m_pEnv << m_pVidSubSession->savedSDPLines() << "\n";
   return 0;
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::PrintAudioSessionInfo() 
{
   struct timeval t = {0};

   *m_pEnv << "------\n";
   *m_pEnv << " clientPortNum: " << m_pAudSubSession->clientPortNum() << "\n";     
   *m_pEnv << " rtpPayloadFormat: " << m_pAudSubSession->rtpPayloadFormat () << "\n";     
   *m_pEnv << " mediumName: " << m_pAudSubSession->mediumName () << "\n";     
   *m_pEnv << " codecName : " << m_pAudSubSession->codecName  () << "\n";     
   *m_pEnv << " protocolName  : " << m_pAudSubSession->protocolName () << "\n";     
   *m_pEnv << " controlPath: " << m_pAudSubSession->controlPath () << "\n";     
   *m_pEnv << " fmtp_objecttype : " << m_pAudSubSession->fmtp_objecttype  () << "\n";     
   *m_pEnv << " fmtp_octetalign : " << m_pAudSubSession->fmtp_octetalign  () << "\n";     
   *m_pEnv << " fmtp_robustsorting : " << m_pAudSubSession->fmtp_robustsorting () << "\n";     
   *m_pEnv << " playStartTime : " << m_pAudSubSession->playStartTime () << "\n";
   *m_pEnv << " playEndTime : " << m_pAudSubSession->playEndTime () << "\n";
   *m_pEnv << " getNormalPlayTime : " << m_pAudSubSession->getNormalPlayTime (t) << "\n";
   *m_pEnv << "------\n";
   return 0;
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::PrintVideoSessionInfo() 
{
   struct timeval t = {0};
   *m_pEnv << "------\n";
   *m_pEnv << " clientPortNum: " << m_pVidSubSession->clientPortNum() << "\n";     
   *m_pEnv << " rtpPayloadFormat: " << m_pVidSubSession->rtpPayloadFormat () << "\n";     
   *m_pEnv << " mediumName: " << m_pVidSubSession->mediumName () << "\n";     
   *m_pEnv << " codecName : " << m_pVidSubSession->codecName  () << "\n";     
   *m_pEnv << " fmtp_objecttype : " << m_pVidSubSession->fmtp_objecttype  () << "\n";     
   *m_pEnv << " protocolName  : " << m_pVidSubSession->protocolName () << "\n";     
   *m_pEnv << " controlPath: " << m_pVidSubSession->controlPath () << "\n";     
   *m_pEnv << " videoWidth: " << m_pVidSubSession->videoWidth () << "\n";     
   *m_pEnv << " videoHeight: " << m_pVidSubSession->videoHeight () << "\n";     
   *m_pEnv << " videoFPS: " << m_pVidSubSession->videoFPS () << "\n";     
   *m_pEnv << " fmtp_profile_level_id: " << m_pVidSubSession->fmtp_profile_level_id() << "\n";
   *m_pEnv << " fmtp_sizelength: " << m_pVidSubSession->fmtp_sizelength() << "\n";
   *m_pEnv << " fmtp_streamtype: " << m_pVidSubSession->fmtp_streamtype() << "\n";
   *m_pEnv << " fmtp_config: " << m_pVidSubSession->fmtp_config() << "\n";
   *m_pEnv << " fmtp_mode: " << m_pVidSubSession->fmtp_mode() << "\n";
   *m_pEnv << " fmtp_spropparametersets: " << m_pVidSubSession->fmtp_spropparametersets() << "\n";
   *m_pEnv << " playStartTime : " << m_pVidSubSession->playStartTime () << "\n";
   *m_pEnv << " playEndTime : " << m_pVidSubSession->playEndTime () << "\n";
   *m_pEnv << " getNormalPlayTime : " << m_pVidSubSession->getNormalPlayTime (t) << "\n";
   *m_pEnv << "------\n";

   return 0;
}


//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::GetAudioPayloadType() 
{ 
   return m_audioPayloadType; 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::GetVideoPayloadType() 
{ 
   return m_videoPayloadType; 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
const char* RtspClient::GetAudioCodecName() 
{ 
   return m_audioCodecName.c_str(); 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
const char* RtspClient::GetVideoCodecName() 
{ 
   return m_videoCodecName.c_str(); 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
unsigned short RtspClient::GetRTSPAudioPort() 
{ 
   return m_rtspServerAudioPort; 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
unsigned short RtspClient::GetRTSPVideoPort() 
{ 
   return m_rtspServerVideoPort; 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
const char* RtspClient::GetRTSPAudioIP() 
{ 
   return m_RTSPAudioIP.c_str(); 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
const char* RtspClient::GetRTSPVideoIP() 
{ 
   return m_RTSPVideoIP.c_str(); 
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
unsigned RtspClient::GetRTSPVideoProfile() 
{
   return m_pVidSubSession->fmtp_profile_level_id();
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
int RtspClient::socketNum() 
{
   return ((RTSPClient*)m_ourClient)->socketNum();
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
char* RtspClient::SendOptionsCmdAndGetResponse(const char* url) 
{
   RTSPClient* rtspClient = (RTSPClient*)m_ourClient;
   return rtspClient->sendOptionsCmd(url, NULL, NULL);
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
char* RtspClient::getSDPDescriptionFromURL(const char* url, char const* username , 
      char const* password ,
      char const* proxyServerName ,
      unsigned short proxyServerPortNum ,
      unsigned short clientStartPort ) 
{
   RTSPClient* rtspClient = (RTSPClient*)m_ourClient;
   char* result;
   if (username != NULL && password != NULL) {
      result = rtspClient->describeWithPassword(url, username, password);
   } else {
      result = rtspClient->describeURL(url);
   }

   unsigned statusCode;
   statusCode = rtspClient->describeStatus();
   return result;
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::RtspLogInit()
{
   char *lProjPath;
   char lProj[200] = "";

   if(NULL == mRtspLogFile)
   {
      lProjPath = getenv("SYS_HOME");
      if(NULL == lProjPath)
      {
         printf("SYS_HOME env Not Set exiting .............\n");
         exit(0);
      }
      snprintf(lProj, 200, "%s/var/log/RtspLog_%s", lProjPath, GetCurTime());
      mRtspLogFile = fopen(lProj, "a");
      if(NULL == mRtspLogFile)
      {
         printf("Rtsp Log File failed File:%s  exiting .............\n",
               lProj);
      }
   }
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::RtspLogDeInit()
{
   if(NULL != mRtspLogFile)
   {
      fclose(mRtspLogFile);
      mRtspLogFile = NULL;
   }
}

//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
void RtspClient::PrintRtspLog(char *strLog)
{
   fprintf(mRtspLogFile, "%s %s\n\n", GetCurTime(), strLog);
}


//----------------------------------------------------------------------
// METHOD NAME : 
// DESCRIPTION :
// PARAMETERS  : 
// RETURN      : 
//----------------------------------------------------------------------
char* GetCurTime()
{
   char lMiliSecStr[15] = "";
   struct timeval lTimeValue;
   struct tm * lLocalTime;
   int ltimelen;

   gettimeofday(&lTimeValue, NULL);
   lLocalTime = localtime(&(lTimeValue.tv_sec));


   ltimelen = strftime(gTimeString, 100, "%d%m%Y:%H%M%S", lLocalTime);
   sprintf(lMiliSecStr, ":%06ld", lTimeValue.tv_usec);
   strcat(gTimeString, lMiliSecStr);

   return gTimeString;
}
