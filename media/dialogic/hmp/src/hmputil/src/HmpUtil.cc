static const char *id = "@(#) $Id: HmpUtil.cc,v 1.1.2.4 2016/04/12 21:59:33 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpUtil.cc
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
#include "HmpUtil.h"
INT32 HmpUtil::msSignal;
BOOLEAN HmpUtil::msKeepRunning = true;
BOOLEAN HmpUtil::msSigCfg = false;
BOOLEAN HmpUtil::msSigTrc = false;

extern Log gLog;
extern CTrace gTrace;

//----------------------------------------------------------------------
// METHOD      : LoadSignals
// DESCRIPTION : Load required signal for the processes
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void HmpUtil::LoadSignals ()
{
   signal (SIGHUP, SIG_IGN);
   signal (SIGCHLD, SIG_IGN);
   signal (SIGINT, HmpUtil::HandleSignal);
   signal (SIGTERM, HmpUtil::HandleSignal);
   signal (SIGQUIT, HmpUtil::HandleSignal);
   signal (SIGTSTP, HmpUtil::HandleSignal);
   signal (SIGPIPE, HmpUtil::HandleSignal);
   signal (SIGTRACE, HmpUtil::HandleSignal);
   signal (SIGCFG, HmpUtil::HandleSignal);
}

//----------------------------------------------------------------------
// METHOD      : GetSignalNo
// DESCRIPTION :
// PARAMETER   : no arguments
// RETURN      : INT32
//----------------------------------------------------------------------
INT32 HmpUtil::GetSignalNo ()
{
   return msSignal;
}

//----------------------------------------------------------------------
// METHOD      : HandleSignal
// DESCRIPTION : Handle required signal for the process
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
void HmpUtil::HandleSignal (int sig)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   INT16 lLogSeverity = MAJOR;

   if (false == msKeepRunning)
      _exit (sig);

   msSignal = sig;
   switch (sig)
   {
      case SIGTRACE:
         {
            lLogSeverity = INFO;
            msSigTrc = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTRC <%d> - Changing Trace", msSignal);
            break;
         }

      case SIGCFG:
         {
            lLogSeverity = INFO;
            msSigCfg = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGCFG <%d> - Reloading Configuration",
                  msSignal);
            break;
         }

      case SIGTERM:
         {
            msKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTERM <%d> - Exiting...", msSignal);
            break;
         }

      case SIGINT:
         {
            msKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGINT <%d> - Exiting...", msSignal);
            break;
         }

      case SIGQUIT:
         {
            msKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGQUIT <%d> - Exiting...", msSignal);
            break;
         }

      case SIGPIPE:
         {
            msKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGPIPE <%d> - Exiting...", msSignal);
            break;
         }

      case SIGSEGV:
         {
            msKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGSEGV <%d> - Exiting...", msSignal);
            break;
         }

      case SIGTSTP:
         {
            msKeepRunning = false;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTSTP <%d> - Exiting...", msSignal);
            break;
         }

      default:
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIG_UNKNOWN <%d> - Signal Ignored...",
                  msSignal);
         }
   }

   TERR (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS13, lLogText, lLogSeverity);
   signal (sig, HandleSignal);

   return;
}

//----------------------------------------------------------------------
// METHOD      : KeepRunning
// DESCRIPTION : Returns status whether process should continue or not
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpUtil::KeepRunning ()
{
   if (HmpUtil::CheckTrace ())
   {
      gTrace.SetTrace ();
      HmpUtil::ResetTraceFlag ();
   }

   return msKeepRunning;
}

//----------------------------------------------------------------------
// METHOD      : ResetKeepRunning
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void HmpUtil::ResetKeepRunning ()
{
   msKeepRunning = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : ProcessToExit
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void HmpUtil::ProcessToExit ()
{
   msKeepRunning = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION : Returns status whether process should  reload config
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpUtil::ReloadConfig ()
{
   return msSigCfg;
}

//----------------------------------------------------------------------
// METHOD      : ResetConfigFlag
// DESCRIPTION : Reset config reload status
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void HmpUtil::ResetConfigFlag ()
{
   msSigCfg = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : GetConfigError
// DESCRIPTION :
// PARAMETER   : ConfigReturn  Enum
// RETURN      : Config Error String
//----------------------------------------------------------------------
TEXT * HmpUtil::GetConfigError (const ConfigReturn lCfgRet)
{
   switch (lCfgRet)
   {
      case CFG_FILE_OPEN_ERR:
         return "CFG_FILE_OPEN_ERR";

      case CFG_KEYWORD_NOT_FOUND:
         return "CFG_KEYWORD_NOT_FOUND";

      case CFG_VALUE_NOT_CONFIGURED:
         return "CFG_VALUE_NOT_CONFIGURED";

      case CFG_BUFFER_OVERFLOW:
         return "CFG_BUFFER_OVERFLOW";

      case CFG_RANGE_EXCEEDED:
         return "CFG_RANGE_EXCEEDED";

      case CFG_UNKNOWN_ERR:
      default:
         return "CFG_UNKNOWN_ERR";
   }
}

//----------------------------------------------------------------------
// METHOD      : CheckTrace
// DESCRIPTION : Returns status whether process should check trace
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpUtil::CheckTrace ()
{
   return msSigTrc;
}

//----------------------------------------------------------------------
// METHOD      : ResetTraceFlag
// DESCRIPTION : Reset config trace variable
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void HmpUtil::ResetTraceFlag ()
{
   msSigTrc = false;
   return;
}

//----------------------------------------------------------------------
// METHOD      : ReCreateMsgQueue
// DESCRIPTION : Recreate  the Msg Queues on error
// PARAMETER   : MsgQueue& msgQueue
//             : key_t     key
// RETURN      : success/failure
//----------------------------------------------------------------------
BOOLEAN HmpUtil::ReCreateMsgQueue (MsgQueue & queue, const key_t key)
{
   if (Q_SUCCESS != queue.Create (key, 0666 | IPC_CREAT))
   {
      TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";

      snprintf (lLogText, MAX_LOG_TEXT_LEN,
            "Could not recreate queue Key:%d. ", key);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
      gLog.GenerateLog (GSYS06, lLogText);
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD      : HandleQueueCreateError
// DESCRIPTION : Handle Queue Create Error
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpUtil::HandleQueueCreateError (msgQueueError err, MsgQueue & queue)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   key_t key;

   key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_CREATE_PERMISSION_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_CREATE_PERMISSION_ERR occured while creating queue %d",
               key);
         break;

      case Q_FLAG_ERROR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_FLAG_ERROR occured while creating queue %d", key);
         break;

      case Q_REMOVAL_MARKED:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_REMOVAL_MARKED occured while creating queue %d", key);
         break;

      case Q_NO_CREAT_FLAG:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_NO_CREAT_FLAG occured while creating queue %d", key);
         break;

      case Q_NOT_ENOUGH_MEM:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_NOT_ENOUGH_MEM occured while creating queue %d", key);
         break;

      case Q_LIMIT_EXCEED:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_LIMIT_EXCEED occured while creating queue %d", key);
         break;

      case Q_MSG_TYPE_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_MSG_TYPE_ERR occured while creating queue %d", key);
         break;

      default:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Error:%d occured while creating queue %d", err, key);
   }

   TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS07, lLogText);
   return false;
}

//----------------------------------------------------------------------
// METHOD      : HandleQueueReadError
// DESCRIPTION : Handle Queue Read Error
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpUtil::HandleQueueReadError (msgQueueError err, MsgQueue & queue)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   key_t key;

   key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_MSG_LEN_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_MSG_LEN_ERR occured while reading from queue:%d", key);
         break;

      case Q_READ_PERM_ERR:
         msKeepRunning = false;
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_READ_PERM_ERR occured while reading from queue:%d", key);
         break;

      case Q_READ_SIGNAL_ERR:
         return false;

      case Q_READ_BUF_ADDR_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_READ_BUF_ADDR_ERR occured while reading from queue:%d",
               key);
         break;

      case Q_REMOVED_ERR:
      case Q_READ_INVALID_ID:
         {
            if (false == ReCreateMsgQueue (queue, key))
               msKeepRunning = false;

            return false;
         }

      case Q_NO_MSG_WITH_TYPE:
         return false;

      case Q_READ_UNKNOWN_ERR:
      default:
         {
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Error:%d (%s) occured while reading from queue:%d",
                  err, ConvertQueErrToStr (err), key);
            break;
         }
   }

   TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS07, lLogText);

   return false;
}

//----------------------------------------------------------------------
// METHOD      : HandleQueueWriteError
// DESCRIPTION : Handle Queue Write Error
// PARAMETER   :
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN HmpUtil::HandleQueueWriteError (msgQueueError err,
      MsgQueue & queue, void *ptr,
      INT16 & size, int mtype, modeOption mode)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   key_t key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_WRITE_INVALID_ID:
      case Q_WRITE_PERM_ERR:
      case Q_NOT_EXIST:
         {
            if (ReCreateMsgQueue (queue, key))
            {
               while (msKeepRunning)
               {
                  err = queue.WriteMsg (ptr, size, mtype, mode);
                  if (Q_SUCCESS == err)
                     return true;
                  else
                     HandleQueueWriteError (err, queue, ptr, size, mode);
               }
            }

            msKeepRunning = false;
            return false;
         }

      case Q_WRITE_BUF_ADDR_ERR:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_WRITE_BUF_ADDR_ERR occured while writing to queue:%d",
               key);
         break;

      case Q_WRITE_SIGNAL_ERR:
         {
            while (msKeepRunning)
            {
               err = queue.WriteMsg (ptr, size, mtype, mode);
               if (Q_SUCCESS == err)
                  return true;
               else
                  HandleQueueWriteError (err, queue, ptr, size, mode);
            }

            return false;
         }

      case Q_WRITE_NO_MEM:
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_WRITE_NO_MEM occured while writing to queue:%d", key);
         break;

      case Q_BYTES_LIMIT_ERR:
         err = queue.WriteMsg (ptr, size, mtype, mode);
         if (Q_SUCCESS == err)
            return true;
         else
            return false;

      case Q_WRITE_UNKNOWN_ERR:
      default:
         if (BLOCKING == mode)
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Error:%d (%s) occured while writing to queue:%d",
                  err, ConvertQueErrToStr (err), key);
         break;
   }

   TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
   gLog.GenerateLog (GSYS08, lLogText);
   return false;
}


//----------------------------------------------------------------------
// METHOD      : ConvertNextReqStatToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertNextReqStatToStr (EnumChannelReqStatus channelReqStatus)
{
   switch(channelReqStatus)
   {
      case NEXT_IDLE:
         return "NEXT_IDLE";
      case NEXT_PLAY_REQUIRED:
         return "NEXT_PLAY_REQUIRED";
      case NEXT_RECORD_REQUIRED:
         return "NEXT_RECORD_REQUIRED";
      case NEXT_GET_DTMF_REQUIRED:
         return "NEXT_GET_DTMF_REQUIRED";
      default:
         return "UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertIpmStreamToStr
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT*  HmpUtil::ConvertIpmStreamToStr(EnumStreamType lStrmType)
{
   switch(lStrmType)
   {
      case IPM_NULL:
         return "IPM_NULL";
      case IPM_LIVE_STREAM:
         return "IPM_LIVE_STREAM";
      case IPM_ON_DEMAND_STREAM:
         return "IPM_ON_DEMAND_STREAM";
      default:
         return "UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertQueErrToStr
// DESCRIPTION : Converts Queue Error to str
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertQueErrToStr (const msgQueueError queErr)
{
   switch (queErr)
   {
      case Q_SUCCESS:
         return "Q_SUCCESS";

      case Q_INVALID_COMMAND:
         return "Q_INVALID_COMMAND";

      case Q_MSG_TYPE_ERR:
         return "Q_MSG_TYPE_ERR";

      case Q_NOT_EXIST:
         return "Q_NOT_EXIST";

      case Q_CREATE_PERMISSION_ERR:
         return "Q_CREATE_PERMISSION_ERR";

      case Q_FLAG_ERROR:
         return "Q_FLAG_ERROR";

      case Q_REMOVAL_MARKED:
         return "Q_REMOVAL_MARKED";

      case Q_NO_CREAT_FLAG:
         return "Q_NO_CREAT_FLAG";

      case Q_NOT_ENOUGH_MEM:
         return "Q_NOT_ENOUGH_MEM";

      case Q_LIMIT_EXCEED:
         return "Q_LIMIT_EXCEED";

      case Q_DELETE_PERMISSION_ERR:
         return "Q_DELETE_PERMISSION_ERR";

      case Q_DELETE_INVALID_SECOND_ARG:
         return "Q_DELETE_INVALID_SECOND_ARG";

      case Q_MSG_LEN_ERR:
         return "Q_MSG_LEN_ERR";

      case Q_READ_PERM_ERR:
         return "Q_READ_PERM_ERR";

      case Q_READ_BUF_ADDR_ERR:
         return "Q_READ_BUF_ADDR_ERR";

      case Q_REMOVED_ERR:
         return "Q_REMOVED_ERR";

      case Q_READ_SIGNAL_ERR:
         return "Q_READ_SIGNAL_ERR";

      case Q_READ_INVALID_ID:
         return "Q_READ_INVALID_ID";

      case Q_READ_UNKNOWN_ERR:
         return "Q_READ_UNKNOWN_ERR";

      case Q_NO_MSG_WITH_TYPE:
         return "Q_NO_MSG_WITH_TYPE";

      case Q_BYTES_LIMIT_ERR:
         return "Q_BYTES_LIMIT_ERR";

      case Q_WRITE_PERM_ERR:
         return "Q_WRITE_PERM_ERR";

      case Q_WRITE_BUF_ADDR_ERR:
         return "Q_WRITE_BUF_ADDR_ERR";

      case Q_WRITE_SIGNAL_ERR:
         return "Q_WRITE_SIGNAL_ERR";

      case Q_WRITE_INVALID_ID:
         return "Q_WRITE_INVALID_ID";

      case Q_WRITE_NO_MEM:
         return "Q_WRITE_NO_MEM";

      case Q_WRITE_UNKNOWN_ERR:
         return "Q_WRITE_UNKNOWN_ERR";

      case Q_STATUS_PERMISSION_ERR:
         return "Q_STATUS_PERMISSION_ERR";

      case Q_STATUS_INVALID_ADDRESS_ERR:
         return "Q_STATUS_INVALID_ADDRESS_ERR";

      case Q_UNKNOWN_ERR:
      default:
         return "Q_UNKNOWN_ERR";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertIpmChnStatToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertIpmChnStatToStr(EnumIpmChannelState lIpmChnlSt)
{  
   switch(lIpmChnlSt)
   {
      case IPM_CHANNEL_IDLE:
         return "IPM_CHANNEL_IDLE";
      case IPM_CHANNEL_STREAM:
         return "IPM_CHANNEL_STREAM";
      default:
         return "UNKNOWN";
   }
}


//----------------------------------------------------------------------
// METHOD      : ConvertCmdToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertCmdToStr(EnumMediaCmdTypes lCmd)
{
   switch(lCmd)
   {
      case MC_INVAL_CMD_TYPE:
         return "MC_INVAL_CMD_TYPE";
      case MC_CNTTN:
         return "MC_CNTTN";
      case MC_DISTN:
         return "MC_DISTN";
      case MC_CNTSP:
         return "MC_CNTSP";
      case MC_DISSP:
         return "MC_DISSP";
      case MC_GETDIG:
         return "MC_GETDIG";
      case MC_GETDIGSTP:
         return "MC_GETDIGSTP";
      case MC_RECSTR:
         return "MC_RECSTR";
      case MC_RECSTP:
         return "MC_RECSTP";
      case MC_SETVOL:
         return "MC_SETVOL";
      case MC_SETSPD:
         return "MC_SETSPD";
      case MC_PAUSE:
         return "MC_PAUSE";
      case MC_RESUME:
         return "MC_RESUME";
      case MC_FORWARD:
         return "MC_FORWARD";
      case MC_REWIND:
         return "MC_REWIND";
      case MC_STREAM:
         return "MC_STREAM";
      case MC_STREAM_STOP:
         return "MC_STREAM_STOP";
      default:
         return "UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertEvrEventToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertEvrEventToStr (const int evrEvent)
{
   switch (evrEvent)
   {
      case HMP_EVR_EVT_NO_EVT:
         return "EVT_NONE";

      case HMP_EVR_EVT_RCV:
         return "RCV";

      case HMP_EVR_EVT_DISCARDED:
         return "DISCARDED";

      case HMP_EVR_EVT_DROPPED:
         return "DROPPED";

      case HMP_EVR_EVT_PROCESSED:
         return "PROCESSED";

      default:
         return "UNKNOWN";
   }
}


//----------------------------------------------------------------------
// METHOD      : ConvertHmpEventToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertHmpEventToStr (const int hmpEvent)
{
   switch (hmpEvent)
   {
      case TDX_PLAY:
         return "TDX_PLAY";

      case TDX_RECORD:
         return "TDX_RECORD";

      case TDX_GETDIG:
         return "TDX_GETDIG";

      case TDX_ERROR:
         return "TDX_ERROR";

      case TDX_BARGEIN:
         return "TDX_BARGEIN";

      default:
         return "UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertVoiceChnlStatToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertVoiceChnlStatToStr (EnumVoiceChannelState chnlStatus)
{
   switch(chnlStatus)
   {
      case VOICE_CHANNEL_IDLE:
         return "VOICE_CHANNEL_IDLE";
      case VOICE_CHANNEL_PLAYING:
         return "VOICE_CHANNEL_PLAYING";
      case VOICE_CHANNEL_PLAY_STOPING:
         return "VOICE_CHANNEL_PLAY_STOPING";
      case VOICE_CHANNEL_RECORDING:
         return "VOICE_CHANNEL_RECORDING";
      case VOICE_CHANNEL_RECORD_STOPING:
         return "VOICE_CHANNEL_RECORD_STOPING";
      case VOICE_CHANNEL_GET_DTMF:
         return "VOICE_CHANNEL_GET_DTMF";
      case VOICE_CHANNEL_GET_DTMF_STOPING:
         return "VOICE_CHANNEL_GET_DTMF_STOPING";
      default:
         return "UNKNOWN CHANNEL STATUS";
   }
}

//----------------------------------------------------------------------
// METHOD      : VoiceTermReason
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::VoiceTermReason(const long termRes)
{
   switch(termRes)
   {
      case TM_NORMTERM:            /* Normal Termination */
         return "NormalTerm";
      case TM_MAXDTMF:             /* Max Number of Digits Recd */
         return "MaxDtmf";
      case TM_MAXSIL:              /* Max Silence */
         return "MaxSil";
      case TM_MAXNOSIL:            /* Max Non-Silence */
         return "MaxNoSil";
      case TM_LCOFF:               /* Loop Current Off */
         return "LCOff";
      case TM_IDDTIME:             /* Inter Digit Delay */
         return "IDDTime";
      case TM_MAXTIME:             /* Max Function Time Exceeded */
         return "MaxTime";
      case TM_DIGIT:               /* Digit Mask or Digit Type Term. */
         return "DigitRcvd";
      case TM_PATTERN:             /* Pattern Match Silence Off */
         return "Pattern";
      case TM_USRSTOP:             /* Function Stopped by User */
         return "UsrStop";
      case TM_EOD:                 /* End of Data Reached on Playback */
         return "EndOfData";
      case TM_BADFILE:             /* Play terminated due to Bad input file */
         return "BadFile";
      case TM_BARGEIN:             /* Play terminated due to Barge-in */
         return "BargeIn";
      case TM_ERROR:               /* I/O Device Error */
         return "Error";
      case TM_MAXDATA:             /* Max Data reached for FSK */
         return "MaxData";
      default:
         return "Term_Unknown";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertEvrCauseToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertEvrCauseToStr (const int evrCause)
{
   return "";
}
//----------------------------------------------------------------------
// METHOD      : ConvertEvrTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * HmpUtil::ConvertEvrTypeToStr (const int evrType)
{
   switch (evrType)
   {
      case EVR_TYPE_HMP_PROV:
         return "PROV";

      default:
         return "UNKNOWN";
   }
}
//----------------------------------------------------------------------
// METHOD NAME : DisplayEvr
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : void
//----------------------------------------------------------------------
void HmpUtil::DisplayEvr (HmpEvrMsg & rEvrMsg)
{
   printf ("\n=======================HmpEvrMsg=====================\n");
   HMP_PRINT_SPACE;
   printf ("EvrType---------------:%d\n", rEvrMsg.evrType);
   HMP_PRINT_SPACE;
   printf ("EvrEvent--------------:%d\n", rEvrMsg.evrEvent);
   HMP_PRINT_SPACE;
   printf ("EvrCause--------------:%d\n", rEvrMsg.evrCause);
   HMP_PRINT_SPACE;

   switch (rEvrMsg.evrType)
   {
      case EVR_TYPE_HMP_PROV:
         printf ("Print Other info------:%d\n", 1);
         HMP_PRINT_SPACE;
         break;

      default:
         break;
   }

   printf ("\n======================================================\n");
}
