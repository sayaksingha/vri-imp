static const char *id = "@(#) $Id: SipUtil.cc,v 1.1.4.1 2016/04/12 21:59:25 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       SipUtil.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#include "SipUtil.h"
INT32 SipUtil::msSignal;
BOOLEAN SipUtil::msKeepRunning = true;
BOOLEAN SipUtil::msSigCfg = false;
BOOLEAN SipUtil::msSigTrc = false;

extern Log gLog;
extern CTrace gTrace;

//----------------------------------------------------------------------
// METHOD      : LoadSignals
// DESCRIPTION : Load required signal for the processes
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void SipUtil::LoadSignals ()
{
   signal (SIGHUP, SIG_IGN);
   signal (SIGCHLD, SIG_IGN);
   signal (SIGINT, SipUtil::HandleSignal);
   signal (SIGTERM, SipUtil::HandleSignal);
   signal (SIGQUIT, SipUtil::HandleSignal);
   signal (SIGTSTP, SipUtil::HandleSignal);
   signal (SIGPIPE, SipUtil::HandleSignal);
   signal (SIGTRACE, SipUtil::HandleSignal);
   signal (SIGCFG, SipUtil::HandleSignal);
}

//----------------------------------------------------------------------
// METHOD      : GetSignalNo
// DESCRIPTION :
// PARAMETER   : no arguments
// RETURN      : INT32
//----------------------------------------------------------------------
INT32 SipUtil::GetSignalNo ()
{
   return msSignal;
}

//----------------------------------------------------------------------
// METHOD      : HandleSignal
// DESCRIPTION : Handle required signal for the process
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
void SipUtil::HandleSignal (int sig)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   //INT16 lLogSeverity = MAJOR;

   if (false == msKeepRunning)
      _exit (sig);

   msSignal = sig;
   switch (sig)
   {
      case SIGTRACE:
         {
            //lLogSeverity = INFO;
            msSigTrc = true;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGTRC <%d> - Changing Trace", msSignal);
            break;
         }

      case SIGCFG:
         {
            //lLogSeverity = INFO;
            msSigCfg = true;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGCFG <%d> - Reloading Configuration",
                  msSignal);
            break;
         }

      case SIGTERM:
         {
            msKeepRunning = false;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGTERM <%d> - Exiting...", msSignal);
            break;
         }

      case SIGINT:
         {
            msKeepRunning = false;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGINT <%d> - Exiting...", msSignal);
            break;
         }

      case SIGQUIT:
         {
            msKeepRunning = false;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGQUIT <%d> - Exiting...", msSignal);
            break;
         }

      case SIGPIPE:
         {
            msKeepRunning = false;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGPIPE <%d> - Exiting...", msSignal);
            break;
         }

      case SIGSEGV:
         {
            msKeepRunning = false;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGSEGV <%d> - Exiting...", msSignal);
            break;
         }

      case SIGTSTP:
         {
            msKeepRunning = false;
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIGTSTP <%d> - Exiting...", msSignal);
            break;
         }

      default:
         {
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
                  "Received signal SIG_UNKNOWN <%d> - Signal Ignored...",
                  msSignal);
         }
   }

   TERR (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   //gLog.GenerateLog (GSYS13, lLogText, lLogSeverity);
   signal (sig, HandleSignal);

   return;
}

//----------------------------------------------------------------------
// METHOD      : KeepRunning
// DESCRIPTION : Returns status whether process should continue or not
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN SipUtil::KeepRunning ()
{
   if (SipUtil::CheckTrace ())
   {
      gTrace.SetTrace ();
      SipUtil::ResetTraceFlag ();
   }

   return msKeepRunning;
}

//----------------------------------------------------------------------
// METHOD      : ResetKeepRunning
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void SipUtil::ResetKeepRunning ()
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
void SipUtil::ProcessToExit ()
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
BOOLEAN SipUtil::ReloadConfig ()
{
   return msSigCfg;
}

//----------------------------------------------------------------------
// METHOD      : ResetConfigFlag
// DESCRIPTION : Reset config reload status
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void SipUtil::ResetConfigFlag ()
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
TEXT * SipUtil::GetConfigError (const ConfigReturn lCfgRet)
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
BOOLEAN SipUtil::CheckTrace ()
{
   return msSigTrc;
}

//----------------------------------------------------------------------
// METHOD      : ResetTraceFlag
// DESCRIPTION : Reset config trace variable
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void SipUtil::ResetTraceFlag ()
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
BOOLEAN SipUtil::ReCreateMsgQueue (MsgQueue & queue, const key_t key)
{
   if (Q_SUCCESS != queue.Create (key, 0666 | IPC_CREAT))
   {
      TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";

      snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
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
BOOLEAN SipUtil::HandleQueueCreateError (msgQueueError err, MsgQueue & queue)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   key_t key;

   key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_CREATE_PERMISSION_ERR:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_CREATE_PERMISSION_ERR occured while creating queue %d",
               key);
         break;

      case Q_FLAG_ERROR:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_FLAG_ERROR occured while creating queue %d", key);
         break;

      case Q_REMOVAL_MARKED:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_REMOVAL_MARKED occured while creating queue %d", key);
         break;

      case Q_NO_CREAT_FLAG:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_NO_CREAT_FLAG occured while creating queue %d", key);
         break;

      case Q_NOT_ENOUGH_MEM:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_NOT_ENOUGH_MEM occured while creating queue %d", key);
         break;

      case Q_LIMIT_EXCEED:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_LIMIT_EXCEED occured while creating queue %d", key);
         break;

      case Q_MSG_TYPE_ERR:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_MSG_TYPE_ERR occured while creating queue %d", key);
         break;

      default:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
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
BOOLEAN SipUtil::HandleQueueReadError (msgQueueError err, MsgQueue & queue)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
   key_t key;

   key = queue.GetQueueKey ();

   switch (err)
   {
      case Q_MSG_LEN_ERR:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_MSG_LEN_ERR occured while reading from queue:%d", key);
         break;

      case Q_READ_PERM_ERR:
         msKeepRunning = false;
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
               "Q_READ_PERM_ERR occured while reading from queue:%d", key);
         break;

      case Q_READ_SIGNAL_ERR:
         return false;

      case Q_READ_BUF_ADDR_ERR:
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
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
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
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
BOOLEAN SipUtil::HandleQueueWriteError (msgQueueError err,
      MsgQueue & queue, void *ptr,
      INT16 & size, int mtype, modeOption mode)
{
   TEXT lLogText[TSS_MAX_LOG_TEXT_LEN + 1] = "";
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
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
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
         snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
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
            snprintf (lLogText, TSS_MAX_LOG_TEXT_LEN,
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
TEXT * SipUtil::ConvertQueErrToStr (const msgQueueError queErr)
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
// METHOD      : ConvertEvrEventToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * SipUtil::ConvertEvrEventToStr (const int evrEvent)
{
   switch (evrEvent)
   {
      case SIP_EVR_EVT_NO_EVT:
         return "EVT_NONE";

      case SIP_EVR_EVT_RCV:
         return "RCV";

      case SIP_EVR_EVT_DISCARDED:
         return "DISCARDED";

      case SIP_EVR_EVT_DROPPED:
         return "DROPPED";

      case SIP_EVR_EVT_PROCESSED:
         return "PROCESSED";

      default:
         return "UNKNOWN";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertEvrCauseToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * SipUtil::ConvertEvrCauseToStr (const int evrCause)
{
   return NULL;
}
//----------------------------------------------------------------------
// METHOD      : ConvertEvrTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT * SipUtil::ConvertEvrTypeToStr (const int evrType)
{
switch (evrType)
   {
      case EVR_TYPE_SIP_PROV:
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
void SipUtil::DisplayEvr (SipEvrMsg & rEvrMsg)
{
   printf ("\n=======================SipEvrMsg=====================\n");
   SIP_PRINT_SPACE;
   printf ("EvrType---------------:%d\n", rEvrMsg.evrType);
   SIP_PRINT_SPACE;
   printf ("EvrEvent--------------:%d\n", rEvrMsg.evrEvent);
   SIP_PRINT_SPACE;
   printf ("EvrCause--------------:%d\n", rEvrMsg.evrCause);
   SIP_PRINT_SPACE;

   switch (rEvrMsg.evrType)
      {
            case EVR_TYPE_SIP_PROV:
               printf ("Print Other info------:%d\n", 1);
               SIP_PRINT_SPACE;
               break;

            default:
               break;
      }

      printf ("\n======================================================\n");
}
