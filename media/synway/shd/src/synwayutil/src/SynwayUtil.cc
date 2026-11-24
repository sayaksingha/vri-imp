static const char *id = "@(#) $Id: SynwayUtil.cc,v 1.1.4.3 2016/04/12 21:59:40 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       SynwayUtil.cc
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
// AUTHOR         : Hmp                                  23-Nov-2011
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "SynwayUtil.h"


//-------------------------------------------------------------
// FUNCTION    : SynwayUtil
// DESCRIPTION : Constructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
SynwayUtil::SynwayUtil()
{
}

//-------------------------------------------------------------
// FUNCTION    : SynwayUtil
// DESCRIPTION : Destructor
// PARAMETER   : None
// RETURN      : None
// THROWS      : None
//-------------------------------------------------------------
SynwayUtil::~SynwayUtil()
{
}

//-------------------------------------------------------------
// FUNCTION    : ConvertChnlStToStr
// DESCRIPTION : 
// PARAMETER   : int
// RETURN      : TEXT*
// THROWS      : None
//-------------------------------------------------------------
TEXT* SynwayUtil::ConvertChnlStToStr(int chnlStatus)
{
   switch(chnlStatus)
   {
      case 0:
         return (TEXT*)"S_CALL_STANDBY";
      case 1:
         return (TEXT*)"S_CALL_PICKUPED";
      case 2:
         return (TEXT*)"S_CALL_RINGING";
      case 3:
         return (TEXT*)"S_CALL_TALKING";
      case 4:
         return (TEXT*)"S_CALL_ANALOG_WAITDIALTONE";
      case 5:
         return (TEXT*)"S_CALL_ANALOG_TXPHONUM";
      case 6:
         return (TEXT*)"S_CALL_ANALOG_WAITDIALRESULT";
      case 7:
         return (TEXT*)"S_CALL_PENDING";
      case 8:
         return (TEXT*)"S_CALL_OFFLINE";
      case 9:
         return (TEXT*)"S_CALL_WAIT_REMOTE_PICKUP";
      case 10:
         return (TEXT*)"S_CALL_ANALOG_CLEAR";
      case 11:
         return (TEXT*)"S_CALL_UNAVAILABLE";
      case 12:
         return (TEXT*)"S_CALL_LOCKED";
      case 19:
         return (TEXT*)"S_CALL_RemoteBlock";
      case 20:
         return (TEXT*)"S_CALL_LocalBlock";
      case 30:
         return (TEXT*)"S_CALL_Ss1InWaitPhoNum";
      case 31:
         return (TEXT*)"S_CALL_Ss1InWaitFwdStop";
      case 32:
         return (TEXT*)"S_CALL_Ss1InWaitCallerID";
      case 33:
         return (TEXT*)"S_CALL_Ss1InWaitKD";
      case 34:
         return (TEXT*)"S_CALL_Ss1InWaitKDStop";
      case 35:
         return (TEXT*)"S_CALL_SS1_SAYIDLE";
      case 36:
         return (TEXT*)"S_CALL_SS1WaitIdleCAS";
      case 37:
         return (TEXT*)"S_CALL_SS1PhoNumHoldup";
      case 38:
         return (TEXT*)"S_CALL_Ss1InWaitStopSendA3p";
      case 40:
         return (TEXT*)"S_CALL_Ss1OutWaitBwdAck";
      case 41:
         return (TEXT*)"S_CALL_Ss1OutTxPhoNum";
      case 42:
         return (TEXT*)"s1OutWaitAppendPhoNum";
      case 43:
         return (TEXT*)"S_CALL_Ss1OutTxCallerID";
      case 44:
         return (TEXT*)"s1OutWaitKB";
      case 45:
         return (TEXT*)"s1OutDetectA3p";
      case 50:
         return (TEXT*)"S_FAX_ROUND";
      case 51:
         return (TEXT*)"S_FAX_PhaseA";
      case 52:
         return (TEXT*)"S_FAX_PhaseB";
      case 53:
         return (TEXT*)"S_FAX_SendDCS";
      case 54:
         return (TEXT*)"S_FAX_Train";
      case 55:
         return (TEXT*)"S_FAX_PhaseC";
      case 56:
         return (TEXT*)"S_FAX_PhaseD";
      case 57:
         return (TEXT*)"S_FAX_NextPage";
      case 58:
         return (TEXT*)"S_FAX_AllSent";
      case 59:
         return (TEXT*)"S_FAX_PhaseE";
      case 60:
         return (TEXT*)"S_FAX_Reset";
      case 61:
         return (TEXT*)"S_FAX_Init";
      case 62:
         return (TEXT*)"S_FAX_RcvDCS";
      case 63:
         return (TEXT*)"S_FAX_SendFTT";
      case 64:
         return (TEXT*)"S_FAX_SendCFR";
      case 65:
         return (TEXT*)"S_FAX_SendPPS";
      case 66:
         return (TEXT*)"S_FAX_RcvPPR";
      case 67:
         return (TEXT*)"S_FAX_RepeatECMPage";
      case 68:
         return (TEXT*)"S_FAX_CTC_CTR";
      case 69:
         return (TEXT*)"S_FAX_SendPPR";
      case 70:
         return (TEXT*)"S_TUP_WaitPcmReset";
      case 71:
         return (TEXT*)"S_TUP_WaitSAM";
      case 72:
         return (TEXT*)"S_TUP_WaitGSM";
      case 73:
         return (TEXT*)"S_TUP_WaitCLF";
      case 74:
         return (TEXT*)"S_TUP_WaitPrefix";
      case 75:
         return (TEXT*)"S_TUP_WaitDialAnswer";
      case 76:
         return (TEXT*)"S_TUP_WaitRLG";
      case 77:
         return (TEXT*)"S_TUP_WaitSetCallerID";
      case 81:
         return (TEXT*)"S_ISDN_OUT_WAIT_NET_RESPONSE";
      case 82:
         return (TEXT*)"S_ISDN_OUT_PLS_APPEND_NO";
      case 83:
         return (TEXT*)"S_ISDN_IN_CHK_CALL_IN";
      case 84:
         return (TEXT*)"S_ISDN_IN_RCVING_NO";
      case 85:
         return (TEXT*)"S_ISDN_IN_WAIT_TALK";
      case 86:
         return (TEXT*)"S_ISDN_OUT_WAIT_ALERT";
      case 87:
         return (TEXT*)"S_ISDN_CALL_BEGIN";
      case 88:
         return (TEXT*)"S_ISDN_WAIT_HUANGUP";
      case 89:
         return (TEXT*)"S_ISDN_IN_CALL_PROCEEDING";
      case 100:
         return (TEXT*)"S_CALL_SENDRING";
      case 105:
         return (TEXT*)"S_SPY_RCVPHONUM";
      case 110:
         return (TEXT*)"S_SPY_SS1RESET";
      case 111:
         return (TEXT*)"S_SPY_SS1WAITBWDACK";
      case 112:
         return (TEXT*)"S_SPY_SS1WAITKB";
      case 120:
         return (TEXT*)"S_ISUP_WaitSAM";
      case 121:
         return (TEXT*)"S_ISUP_WaitRLC";
      case 122:
         return (TEXT*)"S_ISUP_WaitReset";
      case 123:
         return (TEXT*)"S_ISUP_LocallyBlocked";
      case 124:
         return (TEXT*)"S_ISUP_RemotelyBlocked";
      case 125:
         return (TEXT*)"S_ISUP_WaitDialAnswer";
      case 126:
         return (TEXT*)"S_ISUP_WaitINF";
      case 127:
         return (TEXT*)"S_ISUP_WaitSetCallerID";
      case 128:
         return (TEXT*)"S_DTRC_ACTIVE";
      case 129:
         return (TEXT*)"S_ISUP_Suspend";
      case 130:
         return (TEXT*)"S_CALL_EM_TXPHONUM";
      case 131:
         return (TEXT*)"S_CALL_EM_WaitIdleCAS";
      case 132:
         return (TEXT*)"S_CALL_VOIP_DIALING";
      case 133:
         return (TEXT*)"S_CALL_VOIP_WAIT_CONNECTED";
      case 134:
         return (TEXT*)"S_CALL_VOIP_CHANNEL_UNUSABLE";
      case 135:
         return (TEXT*)"S_CALL_DISCONECT";
      case 136:
         return (TEXT*)"S_CALL_SS1WaitFlashEnd";
      case 137:
         return (TEXT*)"S_CALL_FlashEnd";
      case 139:
         return (TEXT*)"S_CALL_SIGNAL_ERROR";
      case 140:
         return (TEXT*)"S_CALL_FRAME_ERROR";
      case 150:
         return (TEXT*)"S_CALL_VOIP_SESSION_PROCEEDING";
      case 151:
         return (TEXT*)"S_CALL_VOIP_REG_ING";
      case 152:
         return (TEXT*)"S_CALL_VOIP_REG_FAILED";
      case 153:
         return (TEXT*)"S_CALL_VOIP_CALL_ON_HOLD";
      case 160:
         return (TEXT*)"S_IP_MEDIA_LOCK";
      case 161:
         return (TEXT*)"S_IP_MEDIA_OPEN";
      case 162:
         return (TEXT*)"S_SPY_RBSWAITACK";
      case 163:
         return (TEXT*)"S_SPY_RBSSENDACK";
      case 170:
         return (TEXT*)"S_IPR_USING";
      case 171:
         return (TEXT*)"S_IPR_COMMUNICATING";
      case 300:
         return (TEXT*)"S_FAX_EOR_ERR";
      case 301:
         return (TEXT*)"S_FAX_RNR_RR";
      case 302:
         return (TEXT*)"S_FAX_RTN";
      case 303:
         return (TEXT*)"S_FAX_NextPage_EOM";
      case 400:
         return (TEXT*)"S_FAX_V34_PhaseV8";
      default:
         return (TEXT*)"CHNL_STATUS_UNKNOWN";
   }
}

//-------------------------------------------------------------
// FUNCTION    : ConvertSnwSigLinkStToStr
// DESCRIPTION : 
// PARAMETER   : DWORD
// RETURN      : TEXT*
// THROWS      : None
//-------------------------------------------------------------
TEXT* SynwayUtil::ConvertSnwSigLinkStToStr(DWORD dwParam)
{
   switch(dwParam)
   {
    case 1: 
       return (TEXT*)"OUT_OF_SERVICE";
      case 2: 
          return (TEXT*)"INITIAL_ALIGNMENT";
      case 3: 
          return (TEXT*)"ALIGNED_READY";
      case 4: 
          return (TEXT*)"ALIGNED_NOT_READY";
      case 5: 
          return (TEXT*)"IN_SERVICE";
      case 6: 
          return (TEXT*)"PROCESSOR_OUTAGE";
      default:
          return (TEXT*)"UNKNOWN";
   }
}

//-------------------------------------------------------------
// FUNCTION    : ConvertStatusToStr
// DESCRIPTION : 
// PARAMETER   : EnumChannelStatus
// RETURN      : TEXT*
// THROWS      : None
//-------------------------------------------------------------
TEXT* SynwayUtil::ConvertStatusToStr(EnumChannelStatus chnlStatus)
{
   switch(chnlStatus)
   {
      case CKT_UNAVAILABLE:
         return (TEXT*)"\33[31mN/A \33[0m";
      case CKT_IDLE:
         return (TEXT*)"\33[32mIDLE\33[0m";
      case CKT_PLAYING:
         return (TEXT*)"\33[35mPLAY\33[0m";
      case CKT_PLAY_STOPING:
         return (TEXT*)"\33[34mPLST\33[0m";
      case CKT_RECORD_STOPING:
         return (TEXT*)"\33[34mRCST\33[0m";
      case CKT_GETDIG:
         return (TEXT*)"\33[33mGDIG\33[0m";
      case CKT_RECORDING:
         return (TEXT*)"\33[36mRECD\33[0m";
      case CKT_PATCHED:
         return (TEXT*)"\33[34mPATC\33[0m";
      case CKT_PLAYING_AND_GETDIG:
         return (TEXT*)"\33[34mPDIG\33[0m";
      case CKT_RECORDING_AND_GETDIG:
         return (TEXT*)"\33[34mRDIG\33[0m";
      case CKT_PLAY_STOPING_AND_GETDIG:
         return (TEXT*)"\33[34mPSGD\33[0m";
      case CKT_RECORD_STOPING_AND_GETDIG:
         return (TEXT*)"\33[34mRSGD\33[0m";
      default:
         return (TEXT*)"\33[31mCURR\33[0m";
   }
}

//-------------------------------------------------------------
// FUNCTION    : ConvertSnwDialStToStr
// DESCRIPTION : 
// PARAMETER   : DWORD
// RETURN      : TEXT*
// THROWS      : None
//-------------------------------------------------------------
TEXT* SynwayUtil::ConvertSnwDialStToStr(const DWORD dwParam)
{
   switch(dwParam)
   {
      case 0: 
         return (TEXT*)"DIAL_STANDBY";
      case 1: 
         return (TEXT*)"DIAL_DIALING";
      case 2:
         return (TEXT*)"DIAL_ECHOTONE";
      case 3: 
         return (TEXT*)"DIAL_NO_DIALTONE";
      case 4: 
         return (TEXT*)"DIAL_BUSYTONE";
      case 5: 
         return (TEXT*)"DIAL_ECHO_NOVOICE";
      case 6: 
         return (TEXT*)"DIAL_NOVOICE";
      case 7:
         return (TEXT*)"DIAL_VOICE";
      case 8:
         return (TEXT*)"DIAL_VOICEF1";
      case 9:
         return (TEXT*)"DIAL_VOICEF2";
      case 10:
         return (TEXT*)"DIAL_NOANSWER";
      case 11:
         return (TEXT*)"DIAL_FAILURE";
      case 12:
         return (TEXT*)"DIAL_INVALID_PHONUM";
      case 13:
         return (TEXT*)"DIAL_SESSION_PROCEEDING";
      case 14:
         return (TEXT*)"DIAL_ISDN_PROGRESS";
      default:
         return (TEXT*)"DIAL_UNKNOWN";
   }
}

//-------------------------------------------------------------
// FUNCTION    : ConvertSigStatusToStr
// DESCRIPTION : 
// PARAMETER   : EnumSigChannelStatus
// RETURN      : TEXT*
// THROWS      : None
//-------------------------------------------------------------
TEXT* SynwayUtil::ConvertSigStatusToStr(EnumSigChannelStatus sigChnlStatus)
{
   switch(sigChnlStatus)
   {
      case SNW_CHNL_NOT_AVAILABLE:
         return (TEXT*)"\33[31mN/A \33[0m";
      case SNW_CHNL_IDLE:
         return (TEXT*)"\33[33mIDLE\33[0m";
      case SNW_IAM_SENT:
         return (TEXT*)"\33[35mIAM \33[0m";
      case SNW_WAIT_DAIL_ANS:
         return (TEXT*)"\33[35mIAMs\33[0m";
      case SNW_DIALING:
         return (TEXT*)"\33[33mDIAL\33[0m";
      case SNW_WAIT_REMOTE_PICKUP:
         return (TEXT*)"\33[34mACMr\33[0m";
      case SNW_ANM_RCVD:
         return (TEXT*)"\33[34mANMr\33[0m";
      case SNW_TALKING:
         return (TEXT*)"\33[32mCON \33[0m";
      case SNW_IAM_RCVD:
         return (TEXT*)"\33[33mIAMr\33[0m";
      case SNW_WAIT_ACM_SND:
         return (TEXT*)"\33[34mACMw\33[0m";
      case SNW_RINGING:
         return (TEXT*)"\33[34mRING\33[0m";
      case SNW_WAIT_RLC:
         return (TEXT*)"\33[36mRLCw\33[0m";
      case SNW_REMOTE_HANGUP:
         return (TEXT*)"\33[36mRELr\33[0m";
      case SNW_WAIT_REL_SND:
         return (TEXT*)"\33[34mRELw\33[0m";
      default:
         return (TEXT*)"\33[31mERR \33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertIsupMsgToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : IsupMsg String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertIsupMsgToStr(const GenIsupMsgType lGenIsupMsgType)
{
   switch (lGenIsupMsgType)
   {
      case GEN_ISUP_IAM:
         return (TEXT*)"IAM/SETUP";

      case GEN_ISUP_ACM:
         return (TEXT*)"ACM/ALERTING";

      case GEN_ISUP_ANM:
         return (TEXT*)"ANM/CONNECT";

      case GEN_ISUP_CPG:
         return (TEXT*)"GEN_ISUP_CPG";

      case GEN_ISUP_REL:
         return (TEXT*)"REL/DISCONNECT";

      case GEN_ISUP_RLC:
         return (TEXT*)"RLC/RELEASE";

      case GEN_ISUP_RSC:
         return (TEXT*)"GEN_ISUP_RSC";
      default:
         return (TEXT*)"GEN_ISUP_UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertCktCmdReqToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : IsupMsg String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertCktCmdReqToStr(const EnumCktCmd lCktCmd)
{
   switch (lCktCmd)
   {
      case CKT_CMND_BLOCK:
         return (TEXT*)"CKT_BLOCK";
      case CKT_CMND_UNBLOCK:
         return (TEXT*)"CKT_UNBLOCK";
      case CKT_CMND_REL:
         return (TEXT*)"CKT_REL";
      default:
         return (TEXT*)"CKT_CMD_UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertProcSigChnlStToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : IsupMsg String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertProcSigChnlStToStr(const EnumSigChannelStatus lSigChnlSt)
{
   switch (lSigChnlSt)
   {
      case SNW_CHNL_NOT_AVAILABLE:
         return (TEXT*)"N/A";
      case SNW_CHNL_IDLE:
         return (TEXT*)"IDLE";
      case SNW_IAM_SENT:
         return (TEXT*)"IAMsnd";
      case SNW_WAIT_DAIL_ANS:
         return (TEXT*)"IAMsndSucc";
      case SNW_DIALING:
         return (TEXT*)"DIALING";
      case SNW_WAIT_REMOTE_PICKUP:
         return (TEXT*)"ACMrcv";
      case SNW_ANM_RCVD:
         return (TEXT*)"ANMrcv";
      case SNW_TALKING:
         return (TEXT*)"CALL_CONN";
      case SNW_IAM_RCVD:
         return (TEXT*)"IAMrcv";
      case SNW_WAIT_ACM_SND:
         return (TEXT*)"ACMsndWaitFrmApp";
      case SNW_RINGING:
         return (TEXT*)"ACMsend";
      case SNW_WAIT_RLC:
         return (TEXT*)"WAIT_RLC";
      case SNW_REMOTE_HANGUP:
         return (TEXT*)"RemoteHangUp";
       case SNW_WAIT_REL_SND:
         return (TEXT*)"RELsndWaitFrmApp";
      default:
         return (TEXT*)"UnKnown";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertCktCmdStToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : IsupMsg String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertCktCmdStToStr(const EnumCktCmdStatus lCktCmdSt)
{
   switch (lCktCmdSt)
   {
      case CKT_CMND_BLOCKED:
         return (TEXT*)"CKT_BLOCKED";
      case CKT_CMND_UNBLOCKED:
         return (TEXT*)"CKT_UNBLOCKED";
      case CKT_CMND_RELEASED:
         return (TEXT*)"CKT_RELEASED";
      default:
         return (TEXT*)"CKT_CMD_STATUS_UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertPlayStopResToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertPlayStopResToStr(const DWORD dwParam)
{
   switch (dwParam)
   {
      case 0:
         return (TEXT*)"Playing";
      case 1:
         return (TEXT*)"DataComplete";
      case 2:
         return (TEXT*)"DtmfReceived";
      case 3:
         return (TEXT*)"VoiceActivity";
      case 4:
         return (TEXT*)"RemoteHangUp";
      case 5:
         return (TEXT*)"TermByProgram";
      case 6:
         return (TEXT*)"PlayingPaused";
      case 7:
         return (TEXT*)"TermByBusOpp";
      case 8:
         return (TEXT*)"NetworkFault";
      default:
         return (TEXT*)"UnKnown";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertRecordStopResToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertRecordStopResToStr(const DWORD dwParam)
{
   switch (dwParam)
   {
      case 1:
         return (TEXT*)"TermByProgram";
      case 2:
         return (TEXT*)"DtmfReceived";
      case 3:
         return (TEXT*)"RemoteHangUp";
      case 4:
         return (TEXT*)"Length/TimeComplete";
      case 5:
         return (TEXT*)"RecordingPaused";
      case 6:
         return (TEXT*)"WriteToFileFailed";
      case 7:
         return (TEXT*)"RTPTimeOut";
      case 8:
         return (TEXT*)"RTPFormatUnSupported";
      default:
         return (TEXT*)"UnKnown";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertGetDtmfStopResToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertGetDtmfStopResToStr(const DWORD dwParam)
{
   switch (dwParam)
   {
      case 1:
         return (TEXT*)"TimeOut";
      case 2:
         return (TEXT*)"DtmfTermChar";
      case 3:
         return (TEXT*)"DtmfStringLength";
      default:
         return (TEXT*)"UnKnown";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertCallPendingResToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertCallPendingResToStr(const int pedRes)
{
   switch(pedRes)
   {
      case 10:/*PEND_WaitBckStpMsg*/
         return (TEXT*)"WaitBckStpMsg";
      case 12:/*PEND_RemoteHangupOnTalking*/
         return (TEXT*)"RemoteHangupOnTalking";
      case 13:/*PEND_AutoDialFailed*/
         return (TEXT*)"AutoDialFailed";
      case 20:/*PEND_CalleeHangupOnTalking*/
         return (TEXT*)"CalleeHangupOnTalking";
      case 41:/*ISDN_CALLOVER*/
         return (TEXT*)"ISDN_CALLOVER";
      case 43:/*ISDN_HANGING*/
         return (TEXT*)"ISDN_HANGING";
      case 48:/*PEND_CalleeHangupOnWaitRemotePickUp*/
         return (TEXT*)"CalleeHangupOnWaitRemotePickUp";
      case 50:/*ISUP_RemoteSuspend*/
         return (TEXT*)"RemoteSuspend";
      case 54:/*PEND_RemoteHangupOnRinging*/
         return (TEXT*)"RemoteHangupOnRinging";
      case 63:/*PEND_RemoteHangupOnSuspend*/
         return (TEXT*)"RemoteHangupOnSuspend";
      case 64:/*PEND_CalleeHangupOnSuspend*/
         return (TEXT*)"CalleeHangupOnSuspend";
      default:
         return (TEXT*)"NotHandled";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertProcCallDirToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : String
//----------------------------------------------------------------------
TEXT* SynwayUtil::ConvertProcCallDirToStr(const EnumSnwCallDirection callDir)
{
   switch(callDir)
   {
      case SNW_IN_BOUND_CALL:
         return (TEXT*)"InBound";
      case SNW_OUT_BOUND_CALL:
         return (TEXT*)"OutBound";
      case SNW_NO_CALL:
      default:
         return (TEXT*)"NoCall";
   }
}
