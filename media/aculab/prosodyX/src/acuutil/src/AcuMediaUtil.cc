static const char *id = "@(#) $Id: AcuMediaUtil.cc,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       AcuMediaUtil.cc
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
// AUTHOR         : Roopa A M                            23-JUL-2012
//----------------------------------------------------------------------
#include "AcuMediaUtil.h"

//----------------------------------------------------------------------
// METHOD      : ConvertPlayStatusToStr
// DESCRIPTION : Converts Play Status to Str
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * AcuMediaUtil::ConvertPlayStatusToStr(EnumAcuPlaystatus playStatus)
{
   switch(playStatus)
   {
      case ACU_PLAY_NEXT_NOT_REQD:
         return "PLAY_NEXT_NOT_REQD";

      case ACU_PLAY_NEXT_REQD:
         return "PLAY_NEXT_REQD";

      case ACU_PLAY_NEXT_IMMEDIATE:
         return "PLAY_NEXT_IMMEDIATE";

      case ACU_PLAY_MULTIPLE_FILES:
         return "PLAY_MULTIPLE_FILES";

      case ACU_PLAY_MULTIPLE_FILES_COMPLETED:
         return "PLAY_MULTIPLE_FILES_COMPLETED";

      default:
         return "\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertFaxRcToStr
// DESCRIPTION : Converts Fax Error codes to String
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * AcuMediaUtil::ConvertFaxRcToStr(int lRc)
{
   switch(lRc)
   {
      case kSMFaxNullPointer:
         return "FAX_GEN_NULL_PTR";

      case kSMFaxNoSessionCreated:
         return "FAX_GEN_NO_SESSION_CREATED";

      case kSMFaxNoTIFFPageCreated:
         return "FAX_GEN_NO_TIFF_PAGE_CRTD";

      case kSMFaxResourceAllocFailed:
         return "FAX_GEN_RESOURCE_ALLOC_FAILED";

      case kSMFaxResourceError:
         return "FAX_GEN_RESOURCE_ERR";

      case kSMFaxResourceReleaseFailed:
         return "FAX_GEN_RESOURCE_RELEASE_FAILED";

      case kSMFaxWrongAPICall:
         return "FAX_GEN_WRONG_API_CALL";

      case kSMFaxDCNNormal:
         return "FAX_DCN_NORMAL";

      case kSMFaxDCNCNGFailed:   
         return "FAX_DCN_CNG_FAILED";

      case kSMFaxDCNCTCMaxTries:
         return "FAX_DCN_CTC_MAX_TRIES";

      case kSMFaxDCNCTCNoAnswer:             
         return "FAX_DCN_CTC_NO_ANS";

      case kSMFaxDCNCTCNotEnabled:
         return "FAX_DCN_CTC_DISABLED";

      case kSMFaxDCNCTRNotReceived: 
         return "FAX_DCN_CTC_NOT_RCVD";

      case kSMFaxDCNInvalidPageHandle: 
         return "FAX_DCN_INVALID_PG_HANDL";

      case kSMFaxDCNModemAbort:            
         return "FAX_DCN_MODEM_ABORT";

      case kSMFaxDCNModemError:              
         return "FAX_DCN_MODEM_ERR";

      case kSMFaxDCNNoDCNFromRemote:         
         return "FAX_DCN_NO_DCN_FRM_REMOTE";

      case kSMFaxDCNPageAccessDenied:
         return "FAX_DCN_PG_ACCESS_DENY";

      case kSMFaxDCNPageQualityPoor:        
         return "FAX_DCN_PG_QLTY_POOR";

      case kSMFaxDCNPageReadError:        
         return "FAX_DCN_PG_READ_ERR";

      case kSMFaxDCNPageRetryExceeded:
         return "FAX_DCN_PG_RETRY_EXCEEDED";

      case kSMFaxDCNQuitOnERR:      
         return "FAX_DCN_QUIT_ON_ERR";

      case kSMFaxDCNRRNoAnswer:              
         return "FAX_DCN_RR_NO_ANS";

      case kSMFaxDCNRemoteAbrupt:
         return "FAX_DCN_REMOTE_ABRUPT";

      case kSMFaxDCNRemoteIncompatible:
         return "FAX_DCN_REMOTE_INCOMPATI";

      case kSMFaxDCNRemoteNotReady:     
         return "FAX_DCN_REMOTE_NOT_READY";

      case kSMFaxDCNRemoteSentDCN:         
         return "FAX_DCN_REMOTE_SENT_DCN";

      case kSMFaxDCNTimerExpired:          
         return "FAX_DCN_TIMER_EXPIRY";

      case kSMFaxDCNTimerT1Expired:
         return "FAX_DCN_TIMER_T1_EXPIRY";

      case kSMFaxDCNTimerT2Expired:         
         return "FAX_DCN_TIMER_T2_EXPIRY";

      case kSMFaxDCNTimerT4Expired:         
         return "FAX_DCN_TIMER_T4_EXPIRY";

      case kSMFaxDCNTimerT5Expired:         
         return "FAX_DCN_TIMER_T5_EXPIRY";

      case kSMFaxDCNTrainCapsExhausted:
         return "FAX_DCN_TRAIN_CAPS_EXHAUSTD";

      case kSMFaxDCNTrainRetryExceeded:     
         return "FAX_DCN_TRAIN_RETRY_EXCEED";

      case kSMFaxDCNTrainWaitExpired:     
         return "FAX_DCN_TRAIN_WAIT_EXPITY";

      case kSMFaxDCNTranscodeFailed:       
         return "FAX_DCN_TRANS_CODE_FAILED";

      case kSMFaxDCNUserInterrupt:        
         return "FAX_DCN_USER_INTERRUPT";

      case kSMFaxDCNUserOption:          
         return "FAX_DCN_USER_OPTION";

      case kSMFaxDCNV21PacketCRCBad:
         return "FAX_DCN_V21_PACKET_CRC_BAD";

      case kSMFaxDCNV21PacketUnexpected:
         return "FAX_DCN_V21_PACKET_UNEXPECTD";

      case kSMFaxDCNV21ResendLimitHit:    
         return "FAX_DCN_V21_RESND_LMT_HIT";

      case kSMFaxERRORApiInCorrect:      
         return "FAX_ERR_API_INCORRECT";

      case kSMFaxDCNMinDataRateReached:
         return "FAX_DCN_MIN_DATA_RATE_REACHD";

      case kSMFaxDCNT38PacketUnexpected:
         return "FAX_DCN_38_PACKET_UNEXPECTD";

      case kSMFaxStateMachineTerminated:
            return "FAX_SM_TERMINATED";

      case kSMFaxStateMachineRunning:
            return "FAX_SM_RUNNING";

      case kSMFaxStateMachineStalled:
            return "FAX_SM_STALLED";

      case kSMFaxTranscodeNeeded:
            return "FAX_TRANS_CODE_NEEDED";

      case kSMFaxPageOK:
            return "FAX_PG_OK";

      case kSMFaxPageWriteError:
            return "FAX_PG_WRITE_ERR";

      case kSMFaxPageReadError:
            return "FAX_PG_READ_ERR";

      case kSMFaxPageNotStored:
            return "FAX_PG_NOT_STORED";

      case kSMFaxPageResourceError:
            return "FAX_PG_RESOURCE_ERR";

      case kSMFaxPageWrongWidth:
            return "FAX_PG_WRONG_WIDTH";

      case kSMFaxModeTransmitter:
            return "FAX_MODE_TX";

      case kSMFaxModeReceiver:
            return "FAX_MODE_RX";

      case kSMFaxModeCalling:
            return "FAX_MODE_CALLING";

      case kSMFaxModeCalled:
            return "FAX_MODE_CALLED";

      case kSMFaxModePolling:
            return "FAX_MODE_POLLING";

      case kSMFaxModePolled:
            return "FAX_MODE_POLLED";

      case kSMFaxPolling:
            return "FAX_POLLING";

      case kSMFaxNonPolling:
            return "FAX_NON_POLLING";

      case kSMFaxLastPage:
            return "FAX_LAST_PG";

      case kSMFaxNotLastPage:
            return "FAX_NOT_LAST_PG";

      case kSMFaxInterruptOk:
            return "FAX_INTERUPT_OK";

      case kSMFaxInterruptFailed:
            return "FAX_INTERUPT_FAILED";

      case kSMFaxInterrupted:
            return "FAX_INTERUPTD";

      case kSMFaxInterruptPolite:
            return "FAX_INTERUPT_POLITE";

      case kSMFaxInterruptRude:
            return "FAX_INTERUPT_RUDE";

      case kSMFaxInterruptTimedOut:
            return "FAX_INTERUPT_TIMED_OUT";

      case kSMFaxTimerIdT1:
            return "FAX_TIMER_ID_T1";

      case kSMFaxTimerIdT2:
            return "FAX_TIMER_ID_T2";

      case kSMFaxTimerIdT3:
            return "FAX_TIMER_ID_T3";

      case kSMFaxTimerIdT4:
            return "FAX_TIMER_ID_T4";

      case kSMFaxTimerIdT5:
            return "FAX_TIMER_ID_T5";

      case kSMFaxTimerIdCI:
            return "FAX_TIMER_ID_CI";

      case kSMFaxDSMWarmUp:
            return "FAX_DSM_WARM_UP";

      case kSMFaxDSMTxNegotiate:
            return "FAX_DSM_TX_NEGOTIATE";

      case kSMFaxDSMTxPage:
            return "FAX_DSM_TX_PG";

      case kSMFaxDSMRxNegotiate:
            return "FAX_DSM_RX_NEGOTIATE";

      case kSMFaxDSMRxPage:
            return "FAX_DSM_RX_PG";

      case  kSMFaxSMVPaceOK:
            return "FAX_SMV_PACE_OK";

      case kSMFaxSMVPaceWarmup:
            return "FAX_SMV_PACE_WARMUP";

      case kSMFaxSMVOutOfPace:
            return "FAX_SMV_OUT_OF_PACE";

      case kSMFaxSMVInvalidStateFn:
            return "FAX_SMV_INVALID_STATE_FUN";

      case kSMFaxSMVStateUnrecognised:
            return "FAX_SMV_STATE_UNRECOGNSD";

      case kSMFaxSMVStateMachineTerminated:
            return "FAX_SMV_STATE_MC_TERMINATED";

      default:
            return "UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertFaxRcToStr
// DESCRIPTION : Converts Fax Error codes to String
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* AcuMediaUtil::ConvertPlayCtrlsToStr(EnumAcuPlayCtrl playCtrl)
{
   switch(playCtrl)
   {
      case ACU_PLAY_NONE:
         return "NONE";

      case ACU_PLAY_PAUSE:
         return "PAUSE";

      case ACU_PLAY_RESUME:
         return "RESUME";

      case ACU_PLAY_FORWARD:
         return "FORWARD";

      case ACU_PLAY_REWIND:
         return "REWIND";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertTingTypeToStr
// DESCRIPTION : Converts Ting Trace to string
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* AcuMediaUtil::ConvertTingTypeToStr(EnumAcuTingTrace tingTrace)
{
   switch(tingTrace)
   {
      case ACU_TING_TRACE_NONE:
         return "TING_TRACE_NONE";

      case ACU_TING_TRACE_ENABLE:
         return "TING_TRACE_ENABLE";

      default:
         return "\33[31mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertMediaTypeToStr
// DESCRIPTION : Converts Media Trace to string
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* AcuMediaUtil::ConvertMediaTypeToStr(EnumAcuMediaTrace mediaTrace)
{
   switch (mediaTrace)
   {
      case ACU_MEDIA_TRACE_NONE:
         return "MEDIA_TRACE_NONE";

      case ACU_MEDIA_TRACE_INIT:
         return "MEDIA_TRACE_INIT";

      case ACU_MEDIA_TRACE_PLAY:
         return "MEDIA_TRACE_PLAY";

      case ACU_MEDIA_TRACE_RECORD:
         return "MEDIA_TRACE_RECORD";

      case ACU_MEDIA_TRACE_DTMF:
         return "MEDIA_TRACE_DTMF";

      case ACU_MEDIA_TRACE_ALL:
         return "MEDIA_TRACE_ALL";

      default:
         return "\33[31mUNKNOWN\33[0m";
   }     
}

//----------------------------------------------------------------------
// METHOD      : ConvertMediaTypeToStr
// DESCRIPTION : Converts Media Trace to string
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* AcuMediaUtil::ConvertDataTypeToStr(kSMDataFormat dataFormat)
{
   switch(dataFormat)
   {
      case kSMDataFormatNone:
         return "kSMDataFormatNone";

      case kSMDataFormatALawPCM:
         return "kSMDataFormatALawPCM";

      case kSMDataFormatULawPCM:
         return "kSMDataFormatULawPCM";

      case kSMDataFormatOKIADPCM:
         return "kSMDataFormatOKIADPCM";

      case kSMDataFormatACUBLKPCM:
         return "kSMDataFormatACUBLKPCM";

      case kSMDataFormat16bit:
         return "kSMDataFormat16bit";

      case kSMDataFormat8bit:
         return "kSMDataFormat8bit";

      case kSMDataFormatSigned8bit:
         return "kSMDataFormatSigned8bit";

      case kSMDataFormatIMAADPCM:
         return "kSMDataFormatIMAADPCM";

      case kSMDataFormatSpeex:
         return "kSMDataFormatSpeex";

      case kSMDataFormat8KHzPCM:
         return "kSMDataFormat8KHzPCM";

      case kSMDataFormat8KHzALawPCM:
         return "kSMDataFormat8KHzALawPCM";

      case kSMDataFormat8KHzULawPCM:
         return "kSMDataFormat8KHzULawPCM";

      case kSMDataFormat8KHzOKIADPCM:
         return "kSMDataFormat8KHzOKIADPCM";

      case kSMDataFormat8KHzACUBLKPCM:
         return "kSMDataFormat8KHzACUBLKPCM";

      case kSMDataFormat6KHzPCM:
         return "kSMDataFormat6KHzPCM";

      case kSMDataFormat6KHzALawPCM:
         return "kSMDataFormat6KHzALawPCM";

      case kSMDataFormat6KHzULawPCM:
         return "kSMDataFormat6KHzULawPCM";

      case kSMDataFormat6KHzOKIADPCM:
         return "kSMDataFormat6KHzOKIADPCM";

      case kSMDataFormat6KHzACUBLKPCM:
         return "kSMDataFormat6KHzACUBLKPCM";

      case kSMDataFormatCELP8KBPS:
         return "kSMDataFormatCELP8KBPS";

      case kSMDataFormat8KHz16bitMono:
         return "kSMDataFormat8KHz16bitMono";

      case kSMDataFormat8KHz8bitMono:
         return "kSMDataFormat8KHz8bitMono";

      case kSMDataFormat8KHzSigned8bitMono:
         return "kSMDataFormat8KHzSigned8bitMono";

      default:
         return "\33[31mUNKNOWN\33[0m";
   }
}
