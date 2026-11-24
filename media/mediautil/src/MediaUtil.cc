static const char *id = "@(#) $Id: MediaUtil.cc,v 1.1.4.3 2016/04/12 21:59:38 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       MediaUtil.cc
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
#include "MediaUtil.h"
INT32 MediaUtil::msSignal;
BOOLEAN MediaUtil::msKeepRunning = true;
BOOLEAN MediaUtil::msSigCfg = false;
BOOLEAN MediaUtil::msSigTrc = false;

extern Log gLog;
extern CTrace gTrace;
extern TEXT gProcessName[];

//----------------------------------------------------------------------
// METHOD      : LoadSignals
// DESCRIPTION : Load required signal for the processes
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MediaUtil::LoadSignals ()
{
   signal (SIGHUP, SIG_IGN);
   signal (SIGCHLD, SIG_IGN);
   signal (SIGINT, MediaUtil::HandleSignal);
   signal (SIGTERM, MediaUtil::HandleSignal);
   signal (SIGQUIT, MediaUtil::HandleSignal);
   signal (SIGTSTP, MediaUtil::HandleSignal);
   signal (SIGPIPE, MediaUtil::HandleSignal);
   signal (SIGTRACE, MediaUtil::HandleSignal);
   signal (SIGCFG, MediaUtil::HandleSignal);
}

//----------------------------------------------------------------------
// METHOD      : GetSignalNo
// DESCRIPTION :
// PARAMETER   : no arguments
// RETURN      : INT32
//----------------------------------------------------------------------
INT32 MediaUtil::GetSignalNo ()
{
   return msSignal;
}

//----------------------------------------------------------------------
// METHOD      : HandleSignal
// DESCRIPTION : Handle required signal for the process
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
void MediaUtil::HandleSignal (int sig)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   //INT16 lLogSeverity = MAJOR;

   if (false == msKeepRunning)
      _exit (sig);

   msSignal = sig;
   switch (sig)
   {
      case SIGTRACE:
         {
     //       lLogSeverity = INFO;
            msSigTrc = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTRC <%d> - Changing Trace", msSignal);
            break;
         }

      case SIGCFG:
         {
     //       lLogSeverity = INFO;
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
  // gLog.GenerateLog (GSYS13, lLogText, lLogSeverity);
   gLog.GenerateLog (GSYS13, lLogText); 
   signal (sig, HandleSignal);

   return;
}

//----------------------------------------------------------------------
// METHOD      : KeepRunning
// DESCRIPTION : Returns status whether process should continue or not
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MediaUtil::KeepRunning ()
{
   if (MediaUtil::CheckTrace ())
   {
      gTrace.SetTrace ();
      MediaUtil::ResetTraceFlag ();
   }

   return msKeepRunning;
}

//----------------------------------------------------------------------
// METHOD      : ResetKeepRunning
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MediaUtil::ResetKeepRunning ()
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
void MediaUtil::ProcessToExit ()
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
BOOLEAN MediaUtil::ReloadConfig ()
{
   return msSigCfg;
}

//----------------------------------------------------------------------
// METHOD      : ResetConfigFlag
// DESCRIPTION : Reset config reload status
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MediaUtil::ResetConfigFlag ()
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
TEXT * MediaUtil::GetConfigError (const ConfigReturn lCfgRet)
{
   switch (lCfgRet)
   {
      case CFG_FILE_OPEN_ERR:
         return (TEXT*)"CFG_FILE_OPEN_ERR";

      case CFG_KEYWORD_NOT_FOUND:
         return (TEXT*)"CFG_KEYWORD_NOT_FOUND";

      case CFG_VALUE_NOT_CONFIGURED:
         return (TEXT*)"CFG_VALUE_NOT_CONFIGURED";

      case CFG_BUFFER_OVERFLOW:
         return (TEXT*)"CFG_BUFFER_OVERFLOW";

      case CFG_RANGE_EXCEEDED:
         return (TEXT*)"CFG_RANGE_EXCEEDED";

      case CFG_UNKNOWN_ERR:
      default:
         return (TEXT*)"CFG_UNKNOWN_ERR";
   }
}

//----------------------------------------------------------------------
// METHOD      : CheckTrace
// DESCRIPTION : Returns status whether process should check trace
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MediaUtil::CheckTrace ()
{
   return msSigTrc;
}

//----------------------------------------------------------------------
// METHOD      : ResetTraceFlag
// DESCRIPTION : Reset config trace variable
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MediaUtil::ResetTraceFlag ()
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
BOOLEAN MediaUtil::ReCreateMsgQueue (MsgQueue & queue, const key_t key)
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
BOOLEAN MediaUtil::HandleQueueCreateError (msgQueueError err, MsgQueue & queue)
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
BOOLEAN MediaUtil::HandleQueueReadError (msgQueueError err, MsgQueue & queue)
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
BOOLEAN MediaUtil::HandleQueueWriteError (msgQueueError err,
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
// METHOD      : ConvertQueErrToStr
// DESCRIPTION : Converts Queue Error to str
// PARAMETER   : 
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * MediaUtil::ConvertQueErrToStr (const msgQueueError queErr)
{
   switch (queErr)
   {
      case Q_SUCCESS:
         return (TEXT*)"Q_SUCCESS";

      case Q_INVALID_COMMAND:
         return (TEXT*)"Q_INVALID_COMMAND";

      case Q_MSG_TYPE_ERR:
         return (TEXT*)"Q_MSG_TYPE_ERR";

      case Q_NOT_EXIST:
         return (TEXT*)"Q_NOT_EXIST";

      case Q_CREATE_PERMISSION_ERR:
         return (TEXT*)"Q_CREATE_PERMISSION_ERR";

      case Q_FLAG_ERROR:
         return (TEXT*)"Q_FLAG_ERROR";

      case Q_REMOVAL_MARKED:
         return (TEXT*)"Q_REMOVAL_MARKED";

      case Q_NO_CREAT_FLAG:
         return (TEXT*)"Q_NO_CREAT_FLAG";

      case Q_NOT_ENOUGH_MEM:
         return (TEXT*)"Q_NOT_ENOUGH_MEM";

      case Q_LIMIT_EXCEED:
         return (TEXT*)"Q_LIMIT_EXCEED";

      case Q_DELETE_PERMISSION_ERR:
         return (TEXT*)"Q_DELETE_PERMISSION_ERR";

      case Q_DELETE_INVALID_SECOND_ARG:
         return (TEXT*)"Q_DELETE_INVALID_SECOND_ARG";

      case Q_MSG_LEN_ERR:
         return (TEXT*)"Q_MSG_LEN_ERR";

      case Q_READ_PERM_ERR:
         return (TEXT*)"Q_READ_PERM_ERR";

      case Q_READ_BUF_ADDR_ERR:
         return (TEXT*)"Q_READ_BUF_ADDR_ERR";

      case Q_REMOVED_ERR:
         return (TEXT*)"Q_REMOVED_ERR";

      case Q_READ_SIGNAL_ERR:
         return (TEXT*)"Q_READ_SIGNAL_ERR";

      case Q_READ_INVALID_ID:
         return (TEXT*)"Q_READ_INVALID_ID";

      case Q_READ_UNKNOWN_ERR:
         return (TEXT*)"Q_READ_UNKNOWN_ERR";

      case Q_NO_MSG_WITH_TYPE:
         return (TEXT*)"Q_NO_MSG_WITH_TYPE";

      case Q_BYTES_LIMIT_ERR:
         return (TEXT*)"Q_BYTES_LIMIT_ERR";

      case Q_WRITE_PERM_ERR:
         return (TEXT*)"Q_WRITE_PERM_ERR";

      case Q_WRITE_BUF_ADDR_ERR:
         return (TEXT*)"Q_WRITE_BUF_ADDR_ERR";

      case Q_WRITE_SIGNAL_ERR:
         return (TEXT*)"Q_WRITE_SIGNAL_ERR";

      case Q_WRITE_INVALID_ID:
         return (TEXT*)"Q_WRITE_INVALID_ID";

      case Q_WRITE_NO_MEM:
         return (TEXT*)"Q_WRITE_NO_MEM";

      case Q_WRITE_UNKNOWN_ERR:
         return (TEXT*)"Q_WRITE_UNKNOWN_ERR";

      case Q_STATUS_PERMISSION_ERR:
         return (TEXT*)"Q_STATUS_PERMISSION_ERR";

      case Q_STATUS_INVALID_ADDRESS_ERR:
         return (TEXT*)"Q_STATUS_INVALID_ADDRESS_ERR";

      case Q_UNKNOWN_ERR:
      default:
         return (TEXT*)"\33[33mQ_UNKNOWN_ERR\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertCmdToStr
// DESCRIPTION : Converts Cmd Type to str
// PARAMETER   : 
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * MediaUtil::ConvertCmdToStr(EnumMediaCmdTypes cmdType, unsigned int &lseq, 
            UINT16 &lCkt, MpData &lMpData)
{
   switch(cmdType)
   {
      case MC_INVAL_CMD_TYPE:
         lseq = 0;
         lCkt = 0;
         return (TEXT*)"INVALID";

      case MC_CNTTN:
         lseq = lMpData.playMsg.seqNum;
         lCkt = lMpData.playMsg.circuitId;
         return (TEXT*)"MC_CNTTN";

      case MC_DISTN:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_DISTN";

      case MC_CNTSP:
         lseq = lMpData.switchChnl.seqNum;
         lCkt = lMpData.switchChnl.circuitId_1;
         return (TEXT*)"MC_CNTSP";

      case MC_DISSP:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_DISSP";

      case MC_GETDIG:
         lseq = lMpData.getDigit.seqNum;
         lCkt = lMpData.getDigit.circuitId;
         return (TEXT*)"MC_GETDIG";

      case MC_GETDIGSTP:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_GETDIGSTP";

      case MC_RECSTR:
         lseq = lMpData.recordMsg.seqNum;
         lCkt = lMpData.recordMsg.circuitId;
         return (TEXT*)"MC_RECSTR";

      case MC_RECSTP:
         lseq = lMpData.stopMsg.seqNum;
         lCkt = lMpData.stopMsg.circuitId;
         return (TEXT*)"MC_RECSTP";

      case MC_SETVOL:
         lseq = lMpData.setVolSpd.seqNum;
         lCkt = lMpData.setVolSpd.circuitId;
         return (TEXT*)"MC_SETVOL";

      case MC_SETSPD:
         lseq = lMpData.setVolSpd.seqNum;
         lCkt = lMpData.setVolSpd.circuitId;
         return (TEXT*)"MC_SETSPD";

      case MC_PAUSE:
         lseq = lMpData.pauseMsg.seqNum;
         lCkt = lMpData.pauseMsg.circuitId;
         return (TEXT*)"MC_PAUSE";

      case MC_RESUME:
         lseq = lMpData.resumeMsg.seqNum;
         lCkt = lMpData.resumeMsg.circuitId;
         return (TEXT*)"MC_RESUME";

      case MC_FORWARD:
         lseq = lMpData.forwardMsg.seqNum;
         lCkt = lMpData.forwardMsg.circuitId;
         return (TEXT*)"MC_FORWARD";

      case MC_REWIND:
         lseq = lMpData.rewindMsg.seqNum;
         lCkt = lMpData.rewindMsg.circuitId;
         return (TEXT*)"MC_REWIND";

      case MC_STREAM:
         lseq = lMpData.stream.seqNum;
         lCkt = lMpData.stream.circuitId;
         return (TEXT*)"MC_STREAM";

      case MC_STREAM_STOP:
         return (TEXT*)"MC_STREAM_STOP";

      default:
         lseq = 0;
         lCkt = 0;
         return (TEXT*)"CMD_CORRUPT";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertStatusToStr
// DESCRIPTION : Converts Channel Status to Str
// PARAMETER   : 
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * MediaUtil::ConvertStatusToStr(EnumChannelStatus chanStatus)
{
   switch(chanStatus)
   {
      case CKT_UNAVAILABLE:
         return (TEXT*)"CKT_UNAVAILABLE";

      case CKT_IDLE:
         return (TEXT*)"CKT_IDLE";

      case CKT_PLAYING:
         return (TEXT*)"CKT_PLAYING";

      case CKT_PLAY_STOPING:
         return (TEXT*)"CKT_PLAY_STOPING";

      case CKT_RECORD_STOPING:
         return (TEXT*)"CKT_RECORD_STOPING";

      case CKT_GETDIG:
         return (TEXT*)"CKT_GETDIG";

      case CKT_RECORDING:
         return (TEXT*)"CKT_RECORDING";

      case CKT_PATCHED:
         return (TEXT*)"CKT_PATCHED";

      case CKT_PLAYING_AND_GETDIG:
         return (TEXT*)"CKT_PLAYING_AND_GETDIG";

      case CKT_RECORDING_AND_GETDIG:
         return (TEXT*)"CKT_RECORDING_AND_GETDIG";

      case CKT_PLAY_STOPING_AND_GETDIG:
         return (TEXT*)"CKT_PLAY_STOPING_AND_GETDIG";

      case CKT_RECORD_STOPING_AND_GETDIG:
         return (TEXT*)"CKT_RECORD_STOPING_AND_GETDIG";

      default:
         return (TEXT*)"\33[33mUNKNOWN\33[0m";
   }
}
