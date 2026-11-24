static const char* id="@(#) $Id: MmpUtil.cc,v 1.1.4.3 2016/04/12 21:59:45 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// MmpUtil.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2006
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Divya K                                      24-JAN-2012
//----------------------------------------------------------------------
#include "MmpUtil.h"

INT32     MmpUtil::msSignal;
BOOLEAN   MmpUtil::msKeepRunning = true;
BOOLEAN   MmpUtil::msSigCfg = false;
BOOLEAN   MmpUtil::msSigTrc = false;
INT16     MmpUtil::mNumOfCkts=0;
INT16     MmpUtil::mMappedCkt[NUM_OF_CKTS];

extern    Log      gLog;
extern    CTrace   gTrace;
extern    TEXT     gLogText[MAX_LOG_TEXT_LEN];

//----------------------------------------------------------------------
// METHOD      : LoadSignals
// DESCRIPTION : Load required signal for the processes
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MmpUtil::LoadSignals ()
{
   signal (SIGHUP, SIG_IGN);
   signal (SIGCHLD, SIG_IGN);
   signal (SIGINT, MmpUtil::HandleSignal);
   signal (SIGTERM, MmpUtil::HandleSignal);
   signal (SIGQUIT, MmpUtil::HandleSignal);
   signal (SIGTSTP, MmpUtil::HandleSignal);
   signal (SIGPIPE, MmpUtil::HandleSignal);
   signal (SIGTRACE, MmpUtil::HandleSignal);
   signal (SIGCFG, MmpUtil::HandleSignal);
}

//----------------------------------------------------------------------
// METHOD      : GetSignalNo
// DESCRIPTION : 
// PARAMETER   : no arguments
// RETURN      : INT32
//----------------------------------------------------------------------
INT32 MmpUtil::GetSignalNo ()
{
   return msSignal;
}

//----------------------------------------------------------------------
// METHOD      : HandleSignal
// DESCRIPTION : Handle required signal for the process
// PARAMETER   : 
// RETURN      : None
//----------------------------------------------------------------------
void MmpUtil::HandleSignal (int sig)
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
    //        lLogSeverity = INFO;
            msSigTrc = true;
            snprintf (lLogText, MAX_LOG_TEXT_LEN,
                  "Received signal SIGTRC <%d> - Changing Trace", msSignal);
            break;
         }

      case SIGCFG:
         {
      //      lLogSeverity = INFO;
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

   //TERR (gTrace, printf ("%s: %s \n", gProcessName, lLogText););
   //gLog.GenerateLog (GSYS13, lLogText, lLogSeverity);
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
BOOLEAN MmpUtil::KeepRunning ()
{
   if (MmpUtil::CheckTrace ())
   {
      gTrace.SetTrace ();
      MmpUtil::ResetTraceFlag ();
   }

   return msKeepRunning;
}

//----------------------------------------------------------------------
// METHOD      : ResetKeepRunning
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MmpUtil::ResetKeepRunning ()
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
void MmpUtil::ProcessToExit ()
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
BOOLEAN MmpUtil::ReloadConfig ()
{
   return msSigCfg;
}

//----------------------------------------------------------------------
// METHOD      : ResetConfigFlag
// DESCRIPTION : Reset config reload status
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MmpUtil::ResetConfigFlag ()
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
TEXT * MmpUtil::GetConfigError (const ConfigReturn lCfgRet)
{
   switch (lCfgRet)
   {
      case CFG_FILE_OPEN_ERR:
         return (TEXT*) "CFG_FILE_OPEN_ERR";

      case CFG_KEYWORD_NOT_FOUND:
         return (TEXT*) "CFG_KEYWORD_NOT_FOUND";

      case CFG_VALUE_NOT_CONFIGURED:
         return (TEXT*) "CFG_VALUE_NOT_CONFIGURED";

      case CFG_BUFFER_OVERFLOW:
         return (TEXT*) "CFG_BUFFER_OVERFLOW";

      case CFG_RANGE_EXCEEDED:
         return (TEXT*) "CFG_RANGE_EXCEEDED";

      case CFG_UNKNOWN_ERR:
      default:
         return (TEXT*) "CFG_UNKNOWN_ERR";
   }
}

//----------------------------------------------------------------------
// METHOD      : CheckTrace
// DESCRIPTION : Returns status whether process should check trace
// PARAMETER   : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN MmpUtil::CheckTrace ()
{
   return msSigTrc;
}

//----------------------------------------------------------------------
// METHOD      : ResetTraceFlag
// DESCRIPTION : Reset config trace variable
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void MmpUtil::ResetTraceFlag ()
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
BOOLEAN MmpUtil::ReCreateMsgQueue (MsgQueue & queue, const key_t key)
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
BOOLEAN MmpUtil::HandleQueueCreateError (msgQueueError err, MsgQueue & queue)
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
BOOLEAN MmpUtil::HandleQueueReadError (msgQueueError err, MsgQueue & queue)
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
BOOLEAN MmpUtil::HandleQueueWriteError (msgQueueError err,
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
               //while (msKeepRunning)
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
            //while (msKeepRunning)
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
                  "Error:%d (Mtype:%d) (%s) occured while writing to queue:%d",
                  err, mtype, ConvertQueErrToStr (err), key);
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
TEXT * MmpUtil::ConvertQueErrToStr (const msgQueueError queErr)
{
   switch (queErr)
   {
      case Q_SUCCESS:
         return (TEXT*) "Q_SUCCESS";

      case Q_INVALID_COMMAND:
         return (TEXT*) "Q_INVALID_COMMAND";

      case Q_MSG_TYPE_ERR:
         return (TEXT*) "Q_MSG_TYPE_ERR";

      case Q_NOT_EXIST:
         return (TEXT*) "Q_NOT_EXIST";

      case Q_CREATE_PERMISSION_ERR:
         return (TEXT*) "Q_CREATE_PERMISSION_ERR";

      case Q_FLAG_ERROR:
         return (TEXT*) "Q_FLAG_ERROR";

      case Q_REMOVAL_MARKED:
         return (TEXT*) "Q_REMOVAL_MARKED";

      case Q_NO_CREAT_FLAG:
         return (TEXT*) "Q_NO_CREAT_FLAG";

      case Q_NOT_ENOUGH_MEM:
         return (TEXT*) "Q_NOT_ENOUGH_MEM";

      case Q_LIMIT_EXCEED:
         return (TEXT*) "Q_LIMIT_EXCEED";

      case Q_DELETE_PERMISSION_ERR:
         return (TEXT*) "Q_DELETE_PERMISSION_ERR";

      case Q_DELETE_INVALID_SECOND_ARG:
         return (TEXT*) "Q_DELETE_INVALID_SECOND_ARG";

      case Q_MSG_LEN_ERR:
         return (TEXT*) "Q_MSG_LEN_ERR";

      case Q_READ_PERM_ERR:
         return (TEXT*) "Q_READ_PERM_ERR";

      case Q_READ_BUF_ADDR_ERR:
         return (TEXT*) "Q_READ_BUF_ADDR_ERR";

      case Q_REMOVED_ERR:
         return (TEXT*) "Q_REMOVED_ERR";

      case Q_READ_SIGNAL_ERR:
         return (TEXT*) "Q_READ_SIGNAL_ERR";

      case Q_READ_INVALID_ID:
         return (TEXT*) "Q_READ_INVALID_ID";

      case Q_READ_UNKNOWN_ERR:
         return (TEXT*) "Q_READ_UNKNOWN_ERR";

      case Q_NO_MSG_WITH_TYPE:
         return (TEXT*) "Q_NO_MSG_WITH_TYPE";

      case Q_BYTES_LIMIT_ERR:
         return (TEXT*) "Q_BYTES_LIMIT_ERR";

      case Q_WRITE_PERM_ERR:
         return (TEXT*) "Q_WRITE_PERM_ERR";

      case Q_WRITE_BUF_ADDR_ERR:
         return (TEXT*) "Q_WRITE_BUF_ADDR_ERR";

      case Q_WRITE_SIGNAL_ERR:
         return (TEXT*) "Q_WRITE_SIGNAL_ERR";

      case Q_WRITE_INVALID_ID:
         return (TEXT*) "Q_WRITE_INVALID_ID";

      case Q_WRITE_NO_MEM:
         return (TEXT*) "Q_WRITE_NO_MEM";

      case Q_WRITE_UNKNOWN_ERR:
         return (TEXT*) "Q_WRITE_UNKNOWN_ERR";

      case Q_STATUS_PERMISSION_ERR:
         return (TEXT*) "Q_STATUS_PERMISSION_ERR";

      case Q_STATUS_INVALID_ADDRESS_ERR:
         return (TEXT*) "Q_STATUS_INVALID_ADDRESS_ERR";

      case Q_UNKNOWN_ERR:
      default:
         return (TEXT*) "\33[33mQ_UNKNOWN_ERR\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : PrintHex
// DESCRIPTION : Prints in Hexadecimal format
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
void MmpUtil::PrintHex (const UINT8 * data, int numberOfBytes,
      int arrayLen)
{
   // printf("      Number Of Bytes = %d\n", numberOfBytes);

   for (int i = 0; i < numberOfBytes && i < arrayLen;)
   {
      printf("      ");
      for (int j = 0; j < 16 && i < numberOfBytes && i < arrayLen; j++, i++)
         printf("%02X ", data[i]);
      printf("\n");
   }
}

//----------------------------------------------------------------------
// METHOD      : PrintUnicode
// DESCRIPTION :
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
void MmpUtil::PrintUnicode (const UINT8 * data,
      int numberOfBytes, int arrayLen)
{
   BOOLEAN lAscii = true;

   for (int i = 1; i < numberOfBytes && i < arrayLen; i += 2)
   {
      if (data[i - 1])
      {
         printf ("?");
         lAscii = false;
      }
      else
         printf ("%c", data[i]);
   }

   if (true != lAscii)
   {
      printf (" [");
      MmpUtil::PrintHex (data, numberOfBytes, arrayLen);
      printf ("]");
   }

   return;
}

//----------------------------------------------------------------------
// METHOD NAME : ConvertToUnicode
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : void
//----------------------------------------------------------------------
void MmpUtil::ConvertToUnicode (UINT8 * data, UINT8 &length)
{
   if (length > (0xFF /2))
      length = 0xFF /2;

   UINT8 * lpTempData  = new UINT8[length];

   memcpy (lpTempData, data, length);

   T (gTrace, printf ("%s: DataLength:%d Data before Conversion:%s\n",
            gProcessName, length, data);
         PrintHex (lpTempData, length););

   length *= 2;

   memset (data, 0, length);

   for(int i = 1, j = 0; i <= length; i += 2, j++)
   {
      data[i] = lpTempData[j];
   }

   T (gTrace, printf ("%s: DataLength:%d Data after Conversion\n",
            gProcessName, length);
         PrintHex (data, length););

   delete []lpTempData;

   return;
}

//----------------------------------------------------------------------
// METHOD NAME : ConvertToUnicode
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : void
//----------------------------------------------------------------------
void MmpUtil::ConvertToUnicode (BINARY &binary)
{
   BINARY lData = binary;

   if (lData.length > (sizeof (binary.data) / 2))
   {
      TERR (gTrace,
            printf ("%s: Length:%d Data Length Exceeded. Truncating...\n",
               gProcessName, lData.length););
      lData.length = sizeof (binary.data) / 2;
   }

   binary.length *= 2;

   memset (binary.data, 0, sizeof (binary.data));

   for(int i = 1, j = 0; i <= binary.length; i += 2, j++)
   {
      binary.data[i] = lData.data[j];
   }

   return;
}

//----------------------------------------------------------------------
// METHOD NAME : ConvertToUnicode
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : void
//----------------------------------------------------------------------
void MmpUtil::ConvertToUnicode (GgiReqMsg & reqMsg)
{
   UINT8 lReqData[sizeof (reqMsg.reqData)];
   int lTotalLength = 0;

   T (gTrace, printf ("%s: DataLength before Conversion: ", gProcessName);
         PrintHex (reqMsg.reqDataLength, reqMsg.totalData, GGI_MAX_NUM_OF_REQ_DATA););

   for(UINT8 i = 0; i < reqMsg.totalData && i < GGI_MAX_NUM_OF_REQ_DATA; i++)
   {
      lTotalLength += reqMsg.reqDataLength[i];
      reqMsg.reqDataLength[i] *= 2;
   }

   T (gTrace, printf ("%s: DataLength after Conversion:\n", gProcessName);
         PrintHex (reqMsg.reqDataLength, reqMsg.totalData, GGI_MAX_NUM_OF_REQ_DATA););

   memset (&lReqData, 0, sizeof (lReqData));

   T (gTrace, printf ("%s: DataLength:%d Data before Conversion: ",
            gProcessName, lTotalLength);
         PrintHex (reqMsg.reqData, lTotalLength););

   lTotalLength *= 2;

   for(int i = 1, j = 0;
         i <= lTotalLength && i < (int)sizeof (lReqData);
         i += 2, j++)
   {
      lReqData[i] = toupper (reqMsg.reqData[j]);
   }

   memcpy(reqMsg.reqData, lReqData, sizeof(lReqData));

   T (gTrace, printf ("%s: DataLength:%d Data after Conversion:\n",
            gProcessName, lTotalLength);
         PrintHex (reqMsg.reqData, lTotalLength););

   reqMsg.languageId = 1; // Unicode English (Enumeration to be defined)

   return;
}

//----------------------------------------------------------------------
// METHOD      : ConvertProvKeyTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* MmpUtil::ConvertProvKeyTypeToStr (const int provKeyType)
{
   switch(provKeyType)
   {
      case MMP_ARG_PKG:
         return (TEXT*) "PACKAGE";

      case MMP_ARG_LANGUAGE:
         return (TEXT*) "LANGUAGE";

      case MMP_ARG_NUMERIC:
         return (TEXT*) "NUMERIC";

      case MMP_ARG_ALPHANUMERIC:
         return (TEXT*) "ALPHANUMERIC";

      default:
         return (TEXT*) "\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertKeywordToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT
//----------------------------------------------------------------------
TEXT* MmpUtil::ConvertKeywordToStr (const int keyword)
{
   switch (keyword)
   {
      case MMP_KEYWORD_REG:
         return (TEXT*) "REG";

      case MMP_KEYWORD_DEREG:
         return (TEXT*) "DEREG";

      case MMP_KEYWORD_RENEW:
         return (TEXT*) "RENEW";

      case MMP_KEYWORD_DEDICATE:
         return (TEXT*) "DEDICATE";

      case MMP_KEYWORD_GIFT:
         return (TEXT*) "GIFT";

      case MMP_KEYWORD_LOYALTY:
         return (TEXT*) "LOYALTY";

      case MMP_KEYWORD_DAILYBILL:
         return (TEXT*) "DAILYBILL";

      default :
         return (TEXT*) "\33[33mUNKNOWN KEYWORD\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertInterfaceIdToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT
//----------------------------------------------------------------------
TEXT* MmpUtil::ConvertInterfaceIdToStr (const int interface)
{
   switch(interface)
   {
      case GGI_INTERFACE_GUI:
         return (TEXT*) "GUI";

      case GGI_INTERFACE_SMS:
         return (TEXT*) "SMS";

      case GGI_INTERFACE_USSD:
         return (TEXT*) "USSD";

      case GGI_INTERFACE_IVR:
         return (TEXT*) "IVR";

      case GGI_INTERFACE_PROV:
         return (TEXT*) "PROV";

      case GGI_INTERFACE_VAL_TKR:
         return (TEXT*) "VAL_TKR";

      case GGI_INTERFACE_WAP:
         return (TEXT*) "WAP";

      case GGI_INTERFACE_PORTAL:
         return (TEXT*) "PORTAL";

      default:
         return (TEXT*) "\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertSubsTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT
//----------------------------------------------------------------------
TEXT* MmpUtil::ConvertSubsTypeToStr (const int subsType)
{
   switch(subsType)
   {
      case BILLING_SUBS_TYPE_PREPAID:
         return (TEXT*) "PREPAID";

      case BILLING_SUBS_TYPE_POSTPAID:
         return (TEXT*) "POSTPAID";

      default:
         return (TEXT*) "\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertSubsStatusToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT
//----------------------------------------------------------------------
TEXT* MmpUtil::ConvertSubsStatusToStr (const int subsStatus)
{
   switch(subsStatus)
   {
      case MMP_SUBS_STATS_ACTIVE:
         return (TEXT*) "ACTIVE";

      case MMP_SUBS_STATS_INACTIVE:
         return (TEXT*) "INACTIVE";

      case MMP_SUBS_STATS_BLOCKED:
         return (TEXT*) "BLOCKED";

      default:
         return (TEXT*) "\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertPkgTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT
//----------------------------------------------------------------------
TEXT* MmpUtil::ConvertPkgTypeToStr (const int pkgType)
{
   switch(pkgType)
   {
      case MMP_PACKAGE_UNKNOWN:
         return (TEXT*) "UNKNOWN";

      case MMP_PACKAGE_MINUTES:
         return (TEXT*) "MINUTES";

      case MMP_PACKAGE_CONTENTS:
         return (TEXT*) "CONTENTS";

      case MMP_PACKAGE_DEDICATION:
         return (TEXT*) "DEDICATION";

      case MMP_PACKAGE_SUBSCRIPTION:
         return (TEXT*) "SUBSCRIPTION";

      case MMP_PACKAGE_MCS:
         return (TEXT*) "MCS";

      case MMP_PACKAGE_ALL:
         return (TEXT*) "ALL";

      default:
         return (TEXT*) "\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertUniodeToAscii
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//----------------------------------------------------------------------
void MmpUtil::ConvertUniodeToAscii (UINT8 * pData, UINT16 dataLen, TEXT * pAscii)
{
   int j = 0;

   for (int i = 1; i < dataLen; i += 2, j++)
      pAscii[j] = pData[i];

   pAscii[j] = '\0';

   return;
}
