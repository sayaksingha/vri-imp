static const char* id="@(#) $Id: IvrUtil.cc,v 1.1.1.1.2.6.2.2 2017/11/14 02:55:49 mram Exp $" ;
//----------------------------------------------------------------------
// NAME
// IvrUtil.cc
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
// Originated :                                              12-AUG-2009
//----------------------------------------------------------------------
#include "IvrUtil.h"

INT32 IvrUtil::msSignal;
BOOLEAN IvrUtil::msKeepRunning = true;
BOOLEAN IvrUtil::msSigCfg = false;
BOOLEAN IvrUtil::msSigTrc = false;
INT16   IvrUtil::mNumOfCkts=0;
INT16   IvrUtil::mMappedCkt[NUM_OF_CKTS];
extern Log gLog;
extern CTrace gTrace;
extern TEXT        gLogText[MAX_LOG_TEXT_LEN];

void FREE(void* memblock)
{
   if(memblock != NULL)
   {
      free(memblock);
      memblock = NULL;
   }
}
//----------------------------------------------------------------------
// METHOD      : LoadSignals
// DESCRIPTION : Load required signal for the processes
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void IvrUtil::LoadSignals ()
{
   signal (SIGHUP, SIG_IGN);
   signal (SIGCHLD, SIG_IGN);
   signal (SIGINT, IvrUtil::HandleSignal);
   signal (SIGTERM, IvrUtil::HandleSignal);
   signal (SIGQUIT, IvrUtil::HandleSignal);
   signal (SIGTSTP, IvrUtil::HandleSignal);
   signal (SIGPIPE, IvrUtil::HandleSignal);
   signal (SIGTRACE, IvrUtil::HandleSignal);
   signal (SIGCFG, IvrUtil::HandleSignal);
}

//----------------------------------------------------------------------
// METHOD      : GetSignalNo
// DESCRIPTION : 
// PARAMETER   : no arguments
// RETURN      : INT32
//----------------------------------------------------------------------
INT32 IvrUtil::GetSignalNo ()
{
   return msSignal;
}

//----------------------------------------------------------------------
// METHOD      : HandleSignal
// DESCRIPTION : Handle required signal for the process
// PARAMETER   : 
// RETURN      : None
//----------------------------------------------------------------------
void IvrUtil::HandleSignal (int sig)
{
   TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
  // INT16 lLogSeverity = MAJOR;

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
BOOLEAN IvrUtil::KeepRunning ()
{
   if (IvrUtil::CheckTrace ())
   {
      gTrace.SetTrace ();
      IvrUtil::ResetTraceFlag ();
   }

   return msKeepRunning;
}

//----------------------------------------------------------------------
// METHOD      : ResetKeepRunning
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void IvrUtil::ResetKeepRunning ()
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
void IvrUtil::ProcessToExit ()
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
BOOLEAN IvrUtil::ReloadConfig ()
{
   return msSigCfg;
}

//----------------------------------------------------------------------
// METHOD      : ResetConfigFlag
// DESCRIPTION : Reset config reload status
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void IvrUtil::ResetConfigFlag ()
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
TEXT * IvrUtil::GetConfigError (const ConfigReturn lCfgRet)
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
BOOLEAN IvrUtil::CheckTrace ()
{
   return msSigTrc;
}

//----------------------------------------------------------------------
// METHOD      : ResetTraceFlag
// DESCRIPTION : Reset config trace variable
// PARAMETER   : None
// RETURN      : None
//----------------------------------------------------------------------
void IvrUtil::ResetTraceFlag ()
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
BOOLEAN IvrUtil::ReCreateMsgQueue (MsgQueue & queue, const key_t key)
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
BOOLEAN IvrUtil::HandleQueueCreateError (msgQueueError err, MsgQueue & queue)
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
BOOLEAN IvrUtil::HandleQueueReadError (msgQueueError err, MsgQueue & queue)
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
BOOLEAN IvrUtil::HandleQueueWriteError (msgQueueError err,
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
         snprintf (lLogText, MAX_LOG_TEXT_LEN,
               "Q_BYTES_LIMIT_ERR occured while writing to queue:%d", key);
         err = queue.WriteMsg (ptr, size, mtype, mode);
         if (Q_SUCCESS == err)
            return true;
         //else
         //   return false;

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
TEXT * IvrUtil::ConvertQueErrToStr (const msgQueueError queErr)
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
// METHOD      : GetCommandType
// DESCRIPTION :
// PARAMETER   :
// RETURN      : int
//----------------------------------------------------------------------
EnumCcxmlCommands IvrUtil::GetCommandType (TEXT* cmd)
{
   if(strcmp(cmd, "command.accept") == 0)
      return CCXML_ACCEPT;
   else if(strcmp(cmd, "command.cancel") == 0)
      return CCXML_CANCEL;
   else if(strcmp(cmd, "command.createcall") == 0)
      return CCXML_CREATECALL;
   else if(strcmp(cmd, "command.createconference") == 0)
      return CCXML_CREATECONFERENCE;
   else if(strcmp(cmd, "command.createccxml") == 0)
      return CCXML_CREATECCXML;
   else if(strcmp(cmd, "command.destroyconference") == 0)
      return CCXML_DESTROYCONFERENCE;
   else if(strcmp(cmd, "command.dialogprepare") == 0)
      return CCXML_DIALOGPREPARE;
   else if(strcmp(cmd, "command.dialogstart") == 0)
      return CCXML_DIALOGSTART;
   else if(strcmp(cmd, "command.dialogterminate") == 0)
      return CCXML_DIALOGTERMINATE;
   else if(strcmp(cmd, "command.disconnect") == 0)
      return CCXML_DISCONNECT;
   else if(strcmp(cmd, "command.exit") == 0)
      return CCXML_EXIT;
   else if(strcmp(cmd, "command.fetch") == 0)
      return CCXML_FETCH;
   else if(strcmp(cmd, "command.hold") == 0)
      return CCXML_HOLD;
   else if(strcmp(cmd, "command.join") == 0)
      return CCXML_JOIN;
   else if(strcmp(cmd, "command.move") == 0)
      return CCXML_MOVE;
   else if(strcmp(cmd, "command.redirect") == 0)
      return CCXML_REDIRECT;
   else if(strcmp(cmd, "command.reject") == 0)
      return CCXML_REJECT;
   else if(strcmp(cmd, "command.send") == 0)
      return CCXML_SEND;
   else if(strcmp(cmd, "command.unjoin") == 0)
      return CCXML_UNJOIN;
   else
      return CCXML_UNKNOWN;
}

//----------------------------------------------------------------------
// METHOD      : GetCommandType
// DESCRIPTION :
// PARAMETER   :
// RETURN      : int
//----------------------------------------------------------------------
EnumDirection IvrUtil::GetJoinDirectionValue (TEXT* joindir)
{
   char* lptr;

   if((lptr = strstr(joindir, "transmit")) != NULL)
      return TRANSMIT;

   else if((lptr = strstr(joindir, "receive")) != NULL)
      return RECEIVE;

   else 
      return TRANS_RECEIVE;
}
//----------------------------------------------------------------------
// METHOD      : ConvertCcxmlEventToString
// DESCRIPTION :
// PARAMETER   :
// RETURN      : int
//----------------------------------------------------------------------
TEXT * IvrUtil::ConvertCcxmlEventToString (const int event)
{
   switch(event)
   {
      case EVT_CC_NONE:
         return (TEXT*)"none";

      case EVT_CC_ALERTING:
         return (TEXT*)"connection.alerting";

      case EVT_CC_CONNECTED:
         return (TEXT*)"connection.connected";

      case EVT_CC_DISCONNECTED:
         return (TEXT*)"connection.disconnected";

      case EVT_CC_ERROR:
         return (TEXT*)"connection.error";

      case EVT_CC_FAILED:
         return (TEXT*)"connection.failed";

      case EVT_CC_PROGRESSING:
         return (TEXT*)"connection.progressing";

      case EVT_CC_REDIRECTED:
         return (TEXT*)"connection.redirected";

      case EVT_CC_SIGNAL:
         return (TEXT*)"connection.signal";

      case EVT_CC_DLG_EXIT:
         return (TEXT*)"dialog.exit";

      case EVT_CC_CONF_JOINED:
         return (TEXT*)"conference.joined";

      case EVT_CC_CONF_UNJOINED:
         return (TEXT*)"conference.unjoined";

      default:
         return (TEXT*)"none";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertVxmlEventToString
// DESCRIPTION :
// PARAMETER   :
// RETURN      : int
//----------------------------------------------------------------------
TEXT * IvrUtil::ConvertVxmlEventToString (const int event)
{
   switch(event)
   {
      case EVT_VX_URL:
         return (TEXT*)"VXML_URL";

      case EVT_VX_TERM:
         return (TEXT*)"VXML_STOP";

      default:
         return (TEXT*)"VXML_NONE";
   }
}

//------------------------------------------------------------------------
// METHOD      : ReadMappedCktNum()
// DESCRIPTION : This function gets the Mapped Out Circuit Id
// PARAMETER   : Reference to Incoming Circuit Id
// RETURN      : success/failure status
// THROWS      : none
//------------------------------------------------------------------------
BOOLEAN IvrUtil::ReadMappedCktNum(BOOLEAN kernelFlg)
{
   /*	INT32        lVal;
      CfgRead      lCfgReadObj(gProcessName);
      TEXT         lLogText[MAX_LOG_TEXT_LEN];
      TEXT         lCktId[CKT_LEN];
      TEXT         lCktIdNums[CKT_LEN];
      TEXT         lInCktStr[CKT_LEN];
      TEXT         lOutCktStr[CKT_LEN];
      UINT32       lInCkt;
      UINT32       lOutCkt;

      if (false == lCfgReadObj.CfgInit(IVR_KER_CFG))
      {
      sprintf (lLogText,"File -> kernel.cfg not found");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
      }

      if (lCfgReadObj.GetConfigNum("NUM_OF_CIRCUITS",lVal,CKT_MIN,CKT_MAX) != CFG_OK)
      {
      sprintf (lLogText, "Config failed for NUM_OF_CIRCUITS ");
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
      }
      else
      {
      mNumOfCkts = lVal;
      }

      for (UINT16 lIteration = 0; lIteration <= CKT_MAX; lIteration++)
      {
      mMappedCkt[lIteration] = lIteration;
      }

      for (UINT8 lCount = 1 ; lCount <= mNumOfCkts ; lCount++ )
      {
      sprintf(lCktId,"CIRCUIT_%d",lCount);
      if (lCfgReadObj.GetConfigStr(lCktId, lCktIdNums, CKT_MIN, CKT_MAX) != CFG_OK)
      {
      sprintf (lLogText, "Config falied for CIRCUIT_%d ", lCount);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
      }

      StrTokenizer lStrTok (lCktIdNums);
      lStrTok.GetNextToken(lInCktStr, ',');
      lStrTok.GetNextToken(lOutCktStr, ',');

      lInCkt = atoi(lInCktStr);
      lOutCkt = atoi(lOutCktStr);

      if(0 == lInCkt || 0 == lOutCkt)
      {
      sprintf (lLogText, "Config falied for CIRCUIT_%d ", lCount);
      TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
      return false;
      }

      if (true == kernelFlg)
      {
      mMappedCkt[lInCkt] = lOutCkt;
      }
      else
      {
      mMappedCkt[lOutCkt] = lInCkt;
      }
      }
    */
   return true;
}
//------------------------------------------------------------------------
// METHOD      : GetMappedCkt(TEXT *incomingId)
// DESCRIPTION : This function gets the Mapped Out Circuit Id
// PARAMETER   : Reference to Incoming Circuit Id
// RETURN      : success/failure status
// THROWS      : none
//------------------------------------------------------------------------
UINT32 IvrUtil::GetMappedCktNum(UINT32 incomingId)
{
   return mMappedCkt[incomingId];;
}

EnumFetchMethod IvrUtil::GetMethodType (TEXT* method)
{
   if(strcasecmp(method, "GET") == 0)
      return GET;
   else if(strcasecmp(method, "POST") == 0)
      return POST;

   return FETCHNONE;
}

EnumQueryType IvrUtil::GetQueryType(TEXT* lType)
{
   if(strcasecmp(lType, "SELECT") == 0)
      return SELECT;
   else if(strcasecmp(lType, "DELETE") == 0)
      return DELETE;
   else if(strcasecmp(lType, "INSERT") == 0)
      return INSERT;
   else if(strcasecmp(lType, "UPDATE") == 0)
      return UPDATE;

   return QUERYNONE;
}

EnumCcxmlEvent IvrUtil::GetEventNumber(TEXT* lEvent)
{
   if(strncmp(lEvent, "connection.", 11) == 0)
   {
      if(strcmp(lEvent, "connection.alerting") == 0)
         return EVT_CC_ALERTING;
      else if(strcmp(lEvent, "connection.connected") == 0)
         return EVT_CC_CONNECTED;
      else if(strcmp(lEvent, "connection.disconnected") == 0)
         return EVT_CC_DISCONNECTED;
      else if(strcmp(lEvent, "connection.error") == 0)
         return EVT_CC_ERROR;
      else if(strcmp(lEvent, "connection.failed") == 0)
         return EVT_CC_FAILED;
      else if(strcmp(lEvent, "connection.progressing") == 0)
         return EVT_CC_PROGRESSING;
      else if(strcmp(lEvent, "connection.redirected") == 0)
         return EVT_CC_REDIRECTED;
      else if(strcmp(lEvent, "connection.signal") == 0)
         return EVT_CC_SIGNAL;
   }
   else if(strncmp (lEvent, "dialog.", 7) == 0)
   {
      if(strcmp(lEvent, "dialog.exit") == 0)
         return EVT_CC_DLG_EXIT;
      else if(strcmp(lEvent, "dialog.disconnect") == 0)
         return EVT_CC_DLG_DISCONNECTED;
      else if(strcmp(lEvent, "dialog.prepared") == 0)
         return EVT_CC_DLG_PREPARED;
      else if(strcmp(lEvent, "dialog.started") == 0)
         return EVT_CC_DLG_STARTED;
      else if(strcmp(lEvent, "dialog.transfer") == 0)
         return EVT_CC_DLG_TRANSFER;
   }
   else if(strncmp(lEvent, "conference.", 11) == 0)
   {
      if(strcmp(lEvent, "conference.created") == 0)
         return EVT_CC_CONF_CREATED;
      else if(strcmp(lEvent, "conference.destroyed") == 0)
         return EVT_CC_CONF_DESTROYED;
      else if(strcmp(lEvent, "conference.joined") == 0)
         return EVT_CC_CONF_JOINED;
      else if(strcmp(lEvent, "conference.unjoined") == 0)
         return EVT_CC_CONF_UNJOINED;
   }
   else if(strcmp(lEvent, "ccxml.created") == 0)
      return EVT_CC_CCXML_CREATED;
   else
      return EVT_CC_NONE;
   /*else if(strcmp(lEvent, "") == 0)
     {
     return ;
     }*/
     return EVT_CC_NONE;
}


TEXT* IvrUtil::ConvertChannelStatusToString (const EnumIVRChannelStatus  status)
{
   /*switch(status)
     {
     case IVR_CHANNEL_UNINITIALIZED:
     return "\33[35mNo-Call\33[0m";
     case IVR_CHANNEL_FREE:
     return "\33[34mWaiting\33[0m";
     case IVR_CHANNEL_IN_CALL:
     return "\33[32mIn-Call\33[0m";
     case IVR_CHANNEL_REL_IN_PROGRESS:
     return "\33[33mCleanup\33[0m";
     case IVR_CHANNEL_PLAYING:
     return "\33[32mPlaying\33[0m";
     case IVR_CHANNEL_RECORDING:
     return "\33[36mRecording\33[0m";
     default:
     return "\33[31mUnknown\33[0m";
     }
    */
   switch(status)
   {
      case IVR_CHANNEL_UNINITIALIZED:
      case IVR_CHANNEL_UN_BLOCKED:
         return (TEXT*)"\33[35mN/A \33[0m";
      case IVR_CHANNEL_FREE:
      case IVR_CHANNEL_RELEASED:
         return (TEXT*)"\33[34mWait\33[0m";
      case IVR_CHANNEL_IN_CALL:
         return (TEXT*)"\33[32mCall\33[0m";
      case IVR_CHANNEL_REL_IN_PROGRESS:
         return (TEXT*)"\33[34mStop\33[0m";
      case IVR_CHANNEL_PLAYING:
         return (TEXT*)"\33[32mPlay\33[0m";
      case IVR_CHANNEL_RECORDING:
         return (TEXT*)"\33[36mRec.\33[0m";
      case IVR_CHANNEL_DTMF:
         return (TEXT*)"\33[33mDtmf\33[0m";
      case IVR_CHANNEL_CMD_IN_PROGRESS:
         return (TEXT*)"\33[34mCmd.\33[0m";
      case IVR_CHANNEL_BLOCKED:
         return (TEXT*)"\33[35mBlk.\33[0m";
      default:
         return (TEXT*)"\33[31mFail\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertEvrEventToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* IvrUtil::ConvertEvrEventToStr(const int evrEvent)
{
   switch (evrEvent)
   {
      case EVT_IVR_NONE:
         return (TEXT*)"EVT_NONE";

      case EVT_IVR_IN_CALL:
         return (TEXT*)"EVT_IN_CALL";

      case EVT_IVR_OUT_CALL:
         return (TEXT*)"EVT_OUT_CALL";

      case EVT_IVR_EXT_PLAY:
         return (TEXT*)"EVT_EXT_PLAY";

      case EVT_IVR_EXT_STOP:
         return (TEXT*)"EVT_EXT_STOP";

      case EVT_IVR_EXT_PATCH:
         return (TEXT*)"EVT_EXT_PATCH";

      case EVT_IVR_EXT_TEAR:
         return (TEXT*)"EVT_EXT_TEAR";

      case EVT_IVR_SCHEDULE_CALL:
         return (TEXT*)"EVT_SCHEDULE_CALL";

      case EVT_IVR_CCXML:
         return (TEXT*)"EVT_CCXML";

      case EVT_IVR_VXML:
         return (TEXT*)"EVT_VXML";

      case EVT_IVR_MEDIA_PLAY:
         return (TEXT*)"EVT_MEDIA_PLAY";

      case EVT_IVR_MEDIA_STOP:
         return (TEXT*)"EVT_MEDIA_STOP";

      case EVT_IVR_MEDIA_PATCH:
         return (TEXT*)"EVT_MEDIA_PATCH";

      case EVT_IVR_MEDIA_TEAR:
         return (TEXT*)"EVT_MEDIA_TEAR";

      case EVT_IVR_MEDIA_DTMF:
         return (TEXT*)"EVT_MEDIA_DTMF";

      case EVT_IVR_NOTIFICATION:
         return (TEXT*)"EVT_NOTIFICATION";

      case EVT_IVR_DEDICATION:
         return (TEXT*)"EVT_DEDICATION";

      case EVT_IVR_GIFT:
         return (TEXT*)"EVT_GIFT";

      case EVT_IVR_PACKAGE_SUB:
         return (TEXT*)"EVT_PACKAGE_SUB";

      case EVT_IVR_PACKAGE_UNSUB:
         return (TEXT*)"EVT_PACKAGE_UNSUB";

      case EVT_IVR_DAILY_SUB:
         return (TEXT*)"EVT_DAILY_SUB";

      case EVT_IVR_DAILY_RENEW:
         return (TEXT*)"EVT_DAILY_RENEW";

      case EVT_IVR_RENEW:
         return (TEXT*)"EVT_RENEW";

      case EVT_IVR_OBD_SCHEDULE:
         return (TEXT*)"EVT_IVR_OBD_SCHEDULE";

      case EVT_IVR_OBD_DISPATCH:
         return (TEXT*)"EVT_IVR_OBD_DISPATCH";

      case EVT_IVR_OBD_RSP:
         return (TEXT*)"EVT_IVR_OBD_RSP";

      case EVT_IVR_OBD_RETRY_DISPATCH:
         return (TEXT*)"EVT_IVR_OBD_RETRY_DISPATCH";

      case EVT_IVR_OBD_RETRY_RSP:
         return (TEXT*)"EVT_IVR_OBD_RETRY_RSP";
      
      case EVT_IVR_OBD_ALERT:
         return (TEXT*)"EVT_IVR_OBD_ALERT";
      
      case EVT_IVR_SAS_SCHEDULE:
         return (TEXT*)"IVR_SAS_SCHEDULE";

      case EVT_IVR_SAS_RSP:
         return (TEXT*)"IVR_SAS_RSP";

      case EVT_IVR_SAS_RETRY_SCHEDULE:
         return (TEXT*)"IVR_SAS_RETRY_SCHEDULE";

      case EVT_IVR_SAS_RETRY_RSP:
         return (TEXT*)"IVR_SAS_RETRY_RSP";

      case EVT_IVR_SAS_ALERT:
         return (TEXT*)"IVR_SAS_ALERT";

      case EVT_IVR_SAS_BILL:
         return (TEXT*)"IVR_SAS_BILL";

      case EVT_IVR_VMS_SUBS_REG:
         return (TEXT*)"EVT_IVR_VMS_SUBS_REG";

      case EVT_IVR_VMS_SUBS_UNREG:
         return (TEXT*)"EVT_IVR_VMS_SUBS_UNREG";

      case EVT_IVR_VMS_SUBS_PIN_RESET:
         return (TEXT*)"EVT_IVR_VMS_SUBS_PIN_RESET";

      case EVT_IVR_VMS_SUBS_PIN_CHANGE:
         return (TEXT*)"EVT_IVR_VMS_SUBS_PIN_CHANGE";

      case EVT_IVR_VMS_SUBS_ACT_FTR:
         return (TEXT*)"EVT_IVR_VMS_SUBS_ACT_FTR";

      case EVT_IVR_VMS_SUBS_DEACT_FTR:
         return (TEXT*)"EVT_IVR_VMS_SUBS_DEACT_FTR";

      case EVT_IVR_VMS_SUBS_RENEW:
         return (TEXT*)"EVT_IVR_VMS_SUBS_RENEW";

      case EVT_IVR_VMS_SUBS_ADD:
         return (TEXT*)"EVT_IVR_VMS_SUBS_ADD";

      case EVT_IVR_VMS_SUBS_DEL:
         return (TEXT*)"EVT_IVR_VMS_SUBS_DEL";
         
      case EVT_IVR_VMS_SUBS_HELP:
         return (TEXT*)"EVT_IVR_VMS_SUBS_HELP";
         
      case EVT_IVR_VMS_RCVD_CALL:
         return (TEXT*)"EVT_IVR_VMS_RCVD_CALL";
         
      case EVT_IVR_VMS_ACCESS_ACCOUNT:
         return (TEXT*)"EVT_IVR_VMS_ACCESS_ACCOUNT";
         
      case EVT_IVR_VMS_DEPOSIT:
         return (TEXT*)"EVT_IVR_VMS_DEPOSIT";
         
      case EVT_IVR_VMS_PIN:
         return (TEXT*)"EVT_IVR_VMS_PIN";
         
      case EVT_IVR_VMS_REPLY:
         return (TEXT*)"EVT_IVR_VMS_REPLY";
         
      case EVT_IVR_VMS_FWD:
         return (TEXT*)"EVT_IVR_VMS_FWD";
         
      case EVT_IVR_VMS_READ:
         return (TEXT*)"EVT_IVR_VMS_READ";
         
      case EVT_IVR_VMS_SAVED:
         return (TEXT*)"EVT_IVR_VMS_SAVED";
         
      case EVT_IVR_VMS_DELETE:
         return (TEXT*)"EVT_IVR_VMS_DELETE";
         
      case EVT_IVR_VMS_GREET_DEPOSIT:
         return (TEXT*)"EVT_IVR_VMS_GREET_DEPOSIT";
         
      case EVT_IVR_VMS_SIGN_DEPOSIT:
         return (TEXT*)"EVT_IVR_VMS_SIGN_DEPOSIT";
         
      case EVT_IVR_VMS_LANG_CHANGE:
         return (TEXT*)"EVT_IVR_VMS_LANG_CHANGE";
         
      case EVT_IVR_VMS_GREET_CHANGE:
         return (TEXT*)"EVT_IVR_VMS_GREET_CHANGE";
         
      case EVT_IVR_VMS_VOICEMAIL:
         return (TEXT*)"EVT_IVR_VMS_VOICEMAIL";
         
      case EVT_IVR_VMS_NOTIFICATION:
         return (TEXT*)"EVT_IVR_VMS_NOTIFICATION";
         
      case EVT_IVR_VMS_LP_CHANGE:
         return (TEXT*)"EVT_IVR_VMS_LP_CHANGE";
         
      case RSN_VMS_RETRY_ATTEMPT_EXCEEDED:
         return (TEXT*)"RSN_VMS_RETRY_ATTEMPT_EXCEEDED";

      default:
         return (TEXT*)"\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertEvrCauseToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* IvrUtil::ConvertEvrCauseToStr(const int evrCause)
{
   /*   CAS_IVR_NONE             = 0,
   // Cause for EVT_IVR_IN_CALL/EVT_IVR_OUT_CALL Events
   CAS_IVR_CALL_RECEIVED    = 1,
   CAS_IVR_CALL_DIALLED     = 2,
   CAS_IVR_CALL_ANSWERED    = 3,
   CAS_IVR_CALL_DISCONNECTED= 4,
   CAS_IVR_CALL_REJECTED    = 5,
   CAS_IVR_CALL_COMPLETE    =  13,

   // Cause for EVT_IVR_EXT_PLAY/EVT_IVR_EXT_STOP/EVT_IVR_EXT_PATCH/EVT_IVR_EXT_TEAR Event
   CAS_IVR_EXT_REQ_RECEIVED = 6,
   CAS_IVR_EXT_REQ_COMPLETE = 7,
   CAS_IVR_EXT_REQ_FAILED   = 8,

   // Cause for EVT_IVR_SCHEDULE_CALL/EVT_IVR_CCXML/EVT_IVR_VXML/EVT_IVR_MEDIA
   //  EVT_IVR_NOTIFICATION/EVT_IVR_DEDICATION/EVT_IVR_GIFT/EVT_IVR_PACKAGE Events
   CAS_IVR_REQ_SUBMITTED = 9,
   CAS_IVR_REQ_RECEIVED  = 10,
   CAS_IVR_REQ_COMPLETE  = 11,
   CAS_IVR_REQ_FAILED    = 12,

   //OBD
   CAS_IVR_OBD_SUCCESS  =  51,
   CAS_IVR_OBD_FAILURE  =  52
*/
   switch (evrCause)
   {
      case CAS_IVR_NONE: 
         return (TEXT*)"CAS_NONE";

      case CAS_IVR_CALL_RECEIVED: 
         return (TEXT*)"CAS_CALL_RECEIVED";

      case CAS_IVR_CALL_DIALLED: 
         return (TEXT*)"CAS_CALL_DIALLED";

      case CAS_IVR_CALL_ANSWERED:
         return (TEXT*)"CAS_CALL_ANSWERED";

      case CAS_IVR_CALL_DISCONNECTED: 
         return (TEXT*)"CAS_CALL_DISCONNECTED";

      case CAS_IVR_CALL_REJECTED: 
         return (TEXT*)"CAS_CALL_REJECTED";

      case CAS_IVR_CALL_COMPLETE:
         return (TEXT*)"CAS_CALL_COMPLETE";

      case CAS_IVR_EXT_REQ_RECEIVED: 
         return (TEXT*)"CAS_EXT_REQ_RECEIVED";

      case CAS_IVR_EXT_REQ_COMPLETE: 
         return (TEXT*)"CAS_EXT_REQ_COMPLETE";

      case CAS_IVR_EXT_REQ_FAILED: 
         return (TEXT*)"CAS_EXT_REQ_FAILED";

      case CAS_IVR_REQ_SUBMITTED: 
         return (TEXT*)"CAS_REQ_SUBMITTED";

      case CAS_IVR_REQ_RECEIVED: 
         return (TEXT*)"CAS_REQ_RECEIVED";

      case CAS_IVR_REQ_COMPLETE: 
         return (TEXT*)"CAS_REQ_COMPLETE";

      case CAS_IVR_REQ_FAILED: 
         return (TEXT*)"CAS_REQ_FAILED";

      case CAS_IVR_OBD_SUCCESS:
         return (TEXT*)"CAS_OBD_SUCCESS";

      case CAS_IVR_OBD_FAILURE:
         return (TEXT*)"CAS_OBD_FAILURE";
      
      case CAS_IVR_OBD_SUBMITTED:
         return (TEXT*)"CAS_IVR_OBD_SUBMITTED";

      case CAS_IVR_SAS_SUCCESS:
        return (TEXT*)"IVR_SAS_SUCCESS";

      case CAS_IVR_SAS_FAILURE:
        return (TEXT*)"IVR_SAS_FAILURE";

      case CAS_IVR_SAS_SUBMITTED:
        return (TEXT*)"IVR_SAS_SUBMITTED";
     
      case CAS_IVR_VMS_SUCCESS:
        return (TEXT*)"CAS_IVR_VMS_SUCCESS";
     
      case CAS_IVR_VMS_FAILURE:
        return (TEXT*)"CAS_IVR_VMS_FAILURE";
     
      case CAS_IVR_VMS_SUBMITTED:
        return (TEXT*)"CAS_IVR_VMS_SUBMITTED";
     
      case CAS_IVR_VMS_SUBS_REQ_DROPPED:
        return (TEXT*)"CAS_IVR_VMS_SUBS_REQ_DROPPED";
     
      case CAS_IVR_VMS_SUBS_REQ_PROCESSED:
        return (TEXT*)"CAS_IVR_VMS_SUBS_REQ_PROCESSED";
     
      case CAS_IVR_VMS_SUBS_REQ_FAILED:
        return (TEXT*)"CAS_IVR_VMS_SUBS_REQ_FAILED";

      default:
         return (TEXT*)"\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertEvrReasonToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* IvrUtil::ConvertEvrReasonToStr(const int evrReason)
{
   switch (evrReason)
   {
      case RSN_IVR_NONE:
         return (TEXT*)"RSN_NONE";

      case RSN_IVR_CALL_BUSY:
         return (TEXT*)"CALL_BUSY";

      case RSN_IVR_CALL_NOT_REACHABLE:
         return (TEXT*)"CALL_NOT_REACHABLE";

      case RSN_IVR_CALL_NO_REPLY:
         return (TEXT*)"CALL_NO_REPLY";

      case RSN_IVR_CALL_BARRED:
         return (TEXT*)"CALL_BARRED";

      case RSN_IVR_CALL_INVALID_DEST_NUMBER:
         return (TEXT*)"CALL_INVALID_DEST_NUMBER";

      case RSN_IVR_CALL_APP_NOT_ACTIVE:
         return (TEXT*)"CALL_APP_NOT_ACTIVE";

      case RSN_IVR_CALL_MULTPLE_ROUTE:
         return (TEXT*)"CALL_MULTIPLE_ROUTE";

      case RSN_IVR_CALL_SC_NOT_ACTIVE:
         return (TEXT*)"CALL_SC_NOT_ACTIVE";

      case RSN_IVR_EXT_REQ_INVALID_CHANNEL:
         return (TEXT*)"EXT_REQ_INVALID_CHANNEL";

      case RSN_IVR_EXT_REQ_INVAILD_APP:
         return (TEXT*)"EXT_REQ_INVAILD_APP";

      case RSN_IVR_EXT_REQ_RESOURCE_UNAVAILABLE:
         return (TEXT*)"EXT_REQ_RESOURCE_UNAVAILABLE";

      case RSN_IVR_SCH_REQ_CHANNEL_UNAVAILABLE:
         return (TEXT*)"SCH_REQ_CHANNEL_UNAVAILABLE";

      case RSN_IVR_SCH_REQ_INVALID_APP:
         return (TEXT*)"SCH_REQ_INVALID_APP";

      case RSN_IVR_SCH_REQ_INACTIVE_APP:
         return (TEXT*)"SCH_REQ_INACTIVE_APP";

      case RSN_IVR_SCH_REQ_RESOURCE_UNAVAILABLE:
         return (TEXT*)"SCH_REQ_RESOURCE_UNAVAILABLE";

      case RSN_IVR_CCXML_INVAILD_URL:
         return (TEXT*)"CCXML_INVAILD_URL";

      case RSN_IVR_CCXML_INVALID_DOC:
         return (TEXT*)"CCXML_INVALID_DOC";

      case RSN_IVR_CCXML_RESOURCE_UNAVAILABLE:
         return (TEXT*)"CCXML_RESOURCE_UNAVAILABLE";

      case RSN_IVR_VXML_INVAILD_URL:
         return (TEXT*)"VXML_INVAILD_URL";

      case RSN_IVR_VXML_INVALID_DOC:
         return (TEXT*)"VXML_INVALID_DOC";

      case RSN_IVR_VXML_RESOURCE_UNAVAILABLE:
         return (TEXT*)"VXML_RESOURCE_UNAVAILABLE";

      case RSN_IVR_MEDIA_REQ_INVALID_FILE:
         return (TEXT*)"MEDIA_REQ_INVALID_FILE";

      case RSN_IVR_MEDIA_REQ_INVALID_STREAM:
         return (TEXT*) "MEDIA_REQ_INVALID_STREAM";

      case RSN_IVR_MEDIA_REQ_INVALID_FORMAT:
         return (TEXT*)"MEDIA_REQ_INVALID_FORMAT";

      case RSN_IVR_MEDIA_REQ_INVALID_STATE:
         return (TEXT*)"MEDIA_REQ_INVALID_STATE";

      case RSN_IVR_MEDIA_REQ_RESOURCE_UNAVAILABLE:
         return (TEXT*)"MEDIA_REQ_RESOURCE_UNAVAILABLE";

      case RSN_IVR_NOTIFICATION_SMS_FAILED:
         return (TEXT*)"NOTI_SMS_FAILED";

      case RSN_IVR_NOTIFICATION_SNS_INVALID_APP:
         return (TEXT*)"NOTI_SNS_INVALID_APP";

      case RSN_IVR_NOTIFICATION_SNS_NO_CREDENTIALS:
         return (TEXT*)"NOTI_SNS_NO_CREDENTIALS";

      case RSN_IVR_NOTIFICATION_SNS_INVALID_CREDENTIALS:
         return (TEXT*)"NOTI_SNS_INVALID_CREDENTIALS";

      case RSN_MMP_DEDICATION_CHARGING_FAILED:
         return (TEXT*)"DEDICATION_CHARGING_FAILED";

      case RSN_MMP_DEDICATION_APP_NOT_ACTIVE:
         return (TEXT*)"DEDICATION_APP_NOT_ACTIVE";

      case RSN_MMP_DEDICATION_ATTEMPTS_EXCEEDS:
         return (TEXT*)"DEDICATION_ATTEMPTS_EXCEEDS";

      case RSN_MMP_DEDICATION_EXPIRED:
         return (TEXT*)"DEDICATION_EXPIRED";

      case RSN_MMP_DEDICATION_CALL_FAILED:
         return (TEXT*)"DEDICATION_CALL_FAILED";

      case RSN_MMP_GIFT_REQ_INVALID_RECEIPENT:
         return (TEXT*)"GIFT_REQ_INVALID_RECEIPENT";

      case RSN_MMP_GIFT_REQ_INVALID_SENDER:
         return (TEXT*)"GIFT_REQ_INVALID_SENDER";

      case RSN_MMP_PACKAGE_DELETED:
         return (TEXT*)"PACKAGE_DELETED";

      case RSN_MMP_PACKAGE_CHARGE_FAILED:
         return (TEXT*)"PACKAGE_CHARGE_FAILED";

      case RSN_MMP_PACKAGE_QUOTA_EXCEED:
         return (TEXT*)"PACKAGE_QUOTA_EXCEED";

      case RSN_MMP_PACKAGE_ALREADY_SUBSCRIBED:
         return (TEXT*)"PACKAGE_ALREADY_SUBSCRIBED";

      case RSN_MMP_SYSTEM_ERR:
         return (TEXT*)"SYSTEM_ERR";

      case RSN_MMP_DB_ERR:
         return (TEXT*)"DB_ERR";

      case RSN_MMP_PACKAGE_DISABLED:
         return (TEXT*)"PACKAGE_DISABLED";

      case RSN_MMP_INVALID_PKG:
         return (TEXT*)"INVALID_PKG";

      case RSN_MMP_NUMBER_OF_DATA_ERR:
         return (TEXT*)"NUMBER_OF_DATA_ERR";

      case RSN_MMP_MANDATORY_DATA_MISSING:
         return (TEXT*)"MANDATORY_DATA_MISSING";

      case RSN_MMP_PACKAGE_EXPIRED:
         return (TEXT*)"PACKAGE_EXPIRED";

      case RSN_MMP_SUBS_TYPE_CHANGE:
         return (TEXT*)"SUBS_TYPE_CHANGE";

      case RSN_MMP_PROV_VAL_SUCCESS:
         return (TEXT*)"PROV_VAL_SUCCESS";

      case RSN_MMP_PACKAGE_REG_SUCCESS:
         return (TEXT*)"PACKAGE_REG_SUCCESS";

      case RSN_MMP_RENEW_SUCCESS:
         return (TEXT*)"RENEW_SUCCESS";

      case RSN_MMP_PACKAGE_UNREG_SUCCESS:
         return (TEXT*)"PACKAGE_UNREG_SUCCESS";

      case RSN_MMP_PACKAGE_CHANGE_SUCCESS:
         return (TEXT*)"PACKAGE_CHANGE_SUCCESS";

      case RSN_MMP_ALREADY_RENEWED:
         return (TEXT*)"ALREADY_RENEWED";

      case RSN_MMP_PACKAGE_NOT_AVAILABLE:
         return (TEXT*)"PACKAGE_NOT_AVAILABLE";

      case RSN_MMP_PACKAGE_NOT_REGISTERED:
         return (TEXT*)"PACKAGE_NOT_REGISTERED";

      case RSN_MMP_AUTO_RENEW_DISABLED:
         return (TEXT*)"AUTO_RENEW_DISABLED";

      case RSN_MMP_MSISDN_IN_DEDICTD_BL:
         return (TEXT*)"MSISDN_IN_DEDICTD_BL";

      case RSN_MMP_GET_COS_FAILED:
         return (TEXT*)"GET_COS_FAILED";

      case RSN_MMP_INSUFFICIENT_BALANCE:
         return (TEXT*)"INSUFFICIENT_BALANCE";

      case RSN_MMP_NILL_BALANCE:
         return (TEXT*)"NILL_BALANCE";

      case RSN_MMP_DAILY_BILL_SUCCESS:
         return (TEXT*)"DAILY_BILL_SUCCESS";

      case RSN_MMP_MUTIPLE_GIFTS_DISABLED:
         return (TEXT*)"MUTIPLE_GIFTS_DISABLED";

      case RSN_MMP_DEDICATE_SUCCESS:
         return (TEXT*)"DEDICATE_SUCCESS";

      case RSN_MMP_GIFT_SUCCESS:
         return (TEXT*)"GIFT_SUCCESS";

      case RSN_MMP_LOYALTY_SUCCESS:
         return (TEXT*)"LOYALTY_SUCCESS";

      case RSN_MMP_DEDICATE_INVALID_CONTENT_ID:
         return (TEXT*)"INVALID_CONTENT_ID";

      case RSN_MMP_REQUEST_REJECTED:
         return (TEXT*)"REQUEST_REJECTED";

      case RSN_MMP_INVALID_DATA:
         return (TEXT*)"INVALID_DATA";

      case RSN_MMP_MULTIPLE_TONEID_IN_REQ:
         return (TEXT*)"MULTIPLE_TONEID_IN_REQ";

         //IN Errors
      case RSN_MMP_BILLING_CFG_ERR:
         return (TEXT*)"BILLING_CFG_ERR";

      case RSN_MMP_BILLING_FAILURE:
         return (TEXT*)"BILLING_FAILURE";

      case RSN_MMP_BLOCKED_SUBSCRIBER:
         return (TEXT*)"BLOCKED_SUBSCRIBER";

      case RSN_MMP_STILL_IN_VALIDITY_LIMIT: 
         return (TEXT*)"STILL_IN_VALIDITY_LIMIT";

      case RSN_MMP_SUFF_BAL_DAILY_FAIL:       
         return (TEXT*)"SUFF_BAL_DAILY_FAIL";

      case RSN_MMP_REG_DAILY_BILL_SUC:              
         return (TEXT*)"REG_DAILY_BILL_SUC";

      case RSN_MMP_GET_SUBS_TYPE_FAIL:             
         return (TEXT*)"GET_SUBS_TYPE_FAIL";

      case RSN_MMP_SUBSCRIBER_NOT_REGISTERED: 
         return (TEXT*)"SUBSCRIBER_NOT_REGISTERED";

      case RSN_MMP_NUMBER_NORM_FAILED:         
         return (TEXT*)"NUMBER_NORM_FAILED";

      case RSN_MMP_SPECAIL_CHARS_PRESENT:             
         return (TEXT*)"SPECAIL_CHARS_PRESENT";

      case RSN_MMP_GIFT_NUMBER_REGISTERED:            
         return (TEXT*)"GIFT_NUMBER_REGISTERED";

      case RSN_MMP_UNAUTHORISED_INTERFACE:
         return (TEXT*)"UNAUTHORISED_INTERFACE";

      case RSN_MMP_DAILY_BILL_DISABLED:              
         return (TEXT*)"DAILY_BILL_DISABLED";

         // ----------------- OBD ---------------------
         // SCHEDULED
      case RSN_OBD_SCHLD_HOLIDAY:             
         return (TEXT*)"SCHLD_HOLIDAY";

      case RSN_OBD_SCHLD_EXPIRY:                    
         return (TEXT*)"SCHLD_EXPIRY";

      case RSN_OBD_SCHLD_DL_ACTIVE:                      
         return (TEXT*)"SCHLD_DL_ACTIVE";

      case RSN_OBD_SCHLD_INVALID_TIMEZONE:
         return (TEXT*)"SCHLD_INVALID_TIMEZONE";

      case RSN_OBD_SCHLD_INVALID_TIMEFRAME:              
         return (TEXT*)"SCHLD_INVALID_TIMEFRAME";

      case RSN_OBD_SCHLD_SYSTEM_BL:             
         return (TEXT*)"SYSTEM_BL";

      case RSN_OBD_SCHLD_COMPLETE_BL:
         return (TEXT*)"COMPLETE_BL";

      case RSN_OBD_SCHLD_CLIENT_BL:
         return (TEXT*)"CLIENT_BL";

      case RSN_OBD_SCHLD_CAMP_EXCEEDED:
         return (TEXT*)"SCHLD_CAMP_EXCEEDED";

      case RSN_OBD_SCHLD_SUCCESS:                 
         return (TEXT*)"SCHLD_SUCCESS";

      case RSN_OBD_RESCHLD_SUCCESS:                 
         return (TEXT*)"RESCHLD_SUCCESS";

         // DISPATCH
      case RSN_OBD_DSPTCH_EXPIRY:                       
         return (TEXT*)"DSPTCH_EXPIRY";

      case RSN_OBD_DSPTCH_NO_QUOTA:
         return (TEXT*)"DSPTCH_NO_QUOTA";

      case RSN_OBD_DSPTCH_THROTTLE:                     
         return (TEXT*)"DSPTCH_THROTTLE";

      case RSN_OBD_SYS_ERR:                     
         return (TEXT*)"OBD_SYS_ERR";

      case RSN_OBD_DB_ERR:                             
         return (TEXT*)"DB_ERR";

         // RESPONSE
      case RSN_OBD_RSP_NO_REACH:
         return (TEXT*)"RSP_NO_REACH";

      case RSN_OBD_RSP_SWITCH_OFF:
         return (TEXT*)"RSP_SWITCH_OFF";

      case RSN_OBD_RSP_NO_ANSWER:                      
         return (TEXT*)"RSP_NO_ANSWER";

      case RSN_OBD_RSP_BUSY:                       
         return (TEXT*)"RSP_BUSY";

      case RSN_OBD_RSP_REJECT:
         return (TEXT*)"RSP_REJECT";

      case RSN_OBD_RSP_FAX:                          
         return (TEXT*)"RSP_FAX";

      case RSN_OBD_RSP_ANS_MACHINE:
         return (TEXT*)"RSP_ANS_MACHINE";

      case RSN_OBD_RSP_INVALID_NUMBER:
         return (TEXT*)"RSP_INVALID_NUMBER";

      case RSN_OBD_DSPTCH_SUCCESS:
         return (TEXT*)"DSPTCH_SUCCESS";
      
      case RSN_OBD_RSP_FREE_CHANNEL:
          return (TEXT*)"FREE_CHANNEL";

      case RSN_OBD_RSP_SMS_SENT:
          return (TEXT*)"SMS_SENT";

      case RSN_OBD_RETRY_RESCHLD_SUCCESS:
          return (TEXT*)"RETRY_RESCHLD_SUCCESS";

      case RSN_OBD_RSP_OTHERS:
          return (TEXT*)"RSP_OTHERS";

      case RSN_OBD_RSP_CHANNEL_NOT_AVAILABLE:
          return (TEXT*)"CHANNEL_NOT_AVAILABLE";

      case RSN_OBD_ALT_SMS:
          return (TEXT*)"RSN_OBD_ALT_SMS";
     
      case RSN_SAS_BILL_SUCCESS:
          return (TEXT*)"SAS_BILL_SUCCESS";
      
      case RSN_SAS_BILL_FAILED:
         return (TEXT*)"SAS_BILL_FAILED";

      case RSN_SAS_BILL_INSUFFICIENT:
         return (TEXT*)"SAS_BILL_INSUFFICIENT";

      case RSN_SAS_RSP_FAILED:
         return (TEXT*)"SAS_RSP_FAILED";

      case RSN_SAS_SYSTEM_ERR:
         return (TEXT*)"SAS_SYSTEM_ERR";
     
      case RSN_SAS_DB_FAILURE:
         return (TEXT*)"SAS_DB_FAILURE";
    
      case RSN_SAS_RSP_SUCCESS:
         return (TEXT*)"SAS_RSP_SUCCESS";

      case RSN_SAS_RSP_RETRY:
         return (TEXT*)"SAS_RETRY";

      case RSN_VMS_SYSTEM_ERR: 
         return (TEXT*)"RSN_VMS_SYSTEM_ERR";

      case RSN_VMS_DB_ERR:
         return (TEXT*)"RSN_VMS_DB_ERR";

      case RSN_VMS_CGW_ERR:
         return (TEXT*)"RSN_VMS_CGW_ERR";

      case RSN_VMS_CGW_NACK:
         return (TEXT*)"RSN_VMS_CGW_NACK";

      case RSN_VMS_CGW_TIMEOUT:
         return (TEXT*)"RSN_VMS_CGW_TIMEOUT";

      case RSN_VMS_INSUFFICIENT_BALANCE:
         return (TEXT*)"RSN_VMS_INSUFFICIENT_BALANCE";

      case RSN_VMS_BALANCE_QUERY_ERR:
         return (TEXT*)"RSN_VMS_BALANCE_QUERY_ERR";

      case RSN_VMS_INVALID_KWD:
         return (TEXT*)"RSN_VMS_INVALID_KWD";

      case RSN_VMS_INVALID_PKG:
         return (TEXT*)"RSN_VMS_INVALID_PKG";

      case RSN_VMS_INVALID_REQUEST:
         return (TEXT*)"RSN_VMS_INVALID_REQUEST";

      case RSN_VMS_INVALID_FORMAT:
         return (TEXT*)"RSN_VMS_INVALID_FORMAT";

      case RSN_VMS_INVALID_MSISDN:
         return (TEXT*)"RSN_VMS_INVALID_MSISDN";

      case RSN_VMS_INVALID_DURATION:
         return (TEXT*)"RSN_VMS_INVALID_DURATION";

      case RSN_VMS_INVALID_PASSWORD:
         return (TEXT*)"RSN_VMS_INVALID_PASSWORD";

      case RSN_VMS_INCORRECT_PASSWORD:
         return (TEXT*)"RSN_VMS_INCORRECT_PASSWORD";

      case RSN_VMS_PASSWD_LNTH_MISMATCH:
         return (TEXT*)"RSN_VMS_PASSWD_LNTH_MISMATCH";

      case RSN_VMS_LOGIN_ATTEMPTS_EXCEEDED:
         return (TEXT*)"RSN_VMS_LOGIN_ATTEMPTS_EXCEEDED";

      case RSN_VMS_SAME_AS_PRESENT_PASSWD:
         return (TEXT*)"RSN_VMS_SAME_AS_PRESENT_PASSWD";

      case RSN_VMS_PKG_ALREADY_REGISTERED:
         return (TEXT*)"RSN_VMS_PKG_ALREADY_REGISTERED";

      case RSN_VMS_PKG_ALREADY_RENEWED:
         return (TEXT*)"RSN_VMS_PKG_ALREADY_RENEWED";

      case RSN_VMS_FTR_ALREADY_ACTIVE:
         return (TEXT*)"RSN_VMS_FTR_ALREADY_ACTIVE";

      case RSN_VMS_FTR_ALREADY_INACTIVE:
         return (TEXT*)"RSN_VMS_FTR_ALREADY_INACTIVE";

      case RSN_VMS_FTR_NOT_AVAILABLE:
         return (TEXT*)"RSN_VMS_FTR_NOT_AVAILABLE";

      case RSN_VMS_SUBS_IN_SRV_BL:
         return (TEXT*)"RSN_VMS_SUBS_IN_SRV_BL";

      case RSN_VMS_NOT_AUTHORISED:
         return (TEXT*)"RSN_VMS_NOT_AUTHORISED";

      case RSN_VMS_PKG_NOT_SUBSCRIBED:
         return (TEXT*)"RSN_VMS_PKG_NOT_SUBSCRIBED";

      case RSN_VMS_PKG_RENEW_NOT_ALLOWED:
         return (TEXT*)"RSN_VMS_PKG_RENEW_NOT_ALLOWED";

      case RSN_VMS_SUBS_TYPE_CHANGE:
         return (TEXT*)"RSN_VMS_SUBS_TYPE_CHANGE";

      case RSN_VMS_GET_SUBS_TYPE_FAILED:
         return (TEXT*)"RSN_VMS_GET_SUBS_TYPE_FAILED";

      case RSN_VMS_ALREADY_IN_LIST:
         return (TEXT*)"RSN_VMS_ALREADY_IN_LIST";

      case RSN_VMS_LIST_UNKNOWN:
         return (TEXT*)"RSN_VMS_LIST_UNKNOWN";

      case RSN_VMS_LIST_LIMIT_EXCEEDED:
         return (TEXT*)"RSN_VMS_LIST_LIMIT_EXCEEDED";

      case RSN_VMS_CALLER_NOT_IN_BL:
         return (TEXT*)"RSN_VMS_CALLER_NOT_IN_BL";

      case RSN_VMS_CALLER_NOT_IN_ML:
         return (TEXT*)"RSN_VMS_CALLER_NOT_IN_ML";

      case RSN_VMS_ALERT_WITHOUT_AUTO_RENEW:
         return (TEXT*)"RSN_VMS_ALERT_WITHOUT_AUTO_RENEW";

      case RSN_VMS_ALERT_WITH_AUTO_RENEW:
         return (TEXT*)"RSN_VMS_ALERT_WITH_AUTO_RENEW";

      case RSN_VMS_VALIDATION_SUC:
         return (TEXT*)"RSN_VMS_VALIDATION_SUC";

      case RSN_VMS_PKG_REG_SUC:
         return (TEXT*)"RSN_VMS_PKG_REG_SUC";

      case RSN_VMS_PKG_RENEW_SUC:
         return (TEXT*)"RSN_VMS_PKG_RENEW_SUC";

      case RSN_VMS_PKG_DEREG_SUC:
         return (TEXT*)"RSN_VMS_PKG_DEREG_SUC";

      case RSN_VMS_ACTIVATE_FTR_SUC:
         return (TEXT*)"RSN_VMS_ACTIVATE_FTR_SUC";

      case RSN_VMS_DEACTIVATE_FTR_SUC:
         return (TEXT*)"RSN_VMS_DEACTIVATE_FTR_SUC";

      case RSN_VMS_RESET_PIN_SUC:
         return (TEXT*)"RSN_VMS_RESET_PIN_SUC";

      case RSN_VMS_CHANGE_PASSWD_SUC:
         return (TEXT*)"RSN_VMS_CHANGE_PASSWD_SUC";

      case RSN_VMS_ADD_BL_SUC:
         return (TEXT*)"RSN_VMS_ADD_BL_SUC";

      case RSN_VMS_ADD_ML_SUC:
         return (TEXT*)"RSN_VMS_ADD_ML_SUC";

      case RSN_VMS_DELETE_ML_SUC:
         return (TEXT*)"RSN_VMS_DELETE_ML_SUC";

      case RSN_VMS_BLACKLISTED:
         return(TEXT*)"RSN_VMS_BLACKLISTED";

      case RSN_VMS_ACC_BLOCKED:
         return (TEXT*)"RSN_VMS_ACC_BLOCKED";

      case RSN_VMS_ACC_INACTIVE:
         return (TEXT*)"RSN_VMS_ACC_INACTIVE";

      case RSN_VMS_ACC_EXPIRED:
         return (TEXT*)"RSN_VMS_ACC_EXPIRED";

      case RSN_VMS_MAIL_BOX_FULL:
         return (TEXT*)"RSN_VMS_MAIL_BOX_FULL";

      case RSN_VMS_CALL_DECLINED:
         return (TEXT*)"RSN_VMS_CALL_DECLINED";

      case RSN_VMS_DEPOSIT_DELETED:
         return (TEXT*)"RSN_VMS_DEPOSIT_DELETED";

      case RSN_VMS_AUTHENTICATION_FAILED:
         return (TEXT*)"RSN_VMS_AUTHENTICATION_FAILED";

      case RSN_VMS_SAVED_BOX_FULL:
         return (TEXT*)"RSN_VMS_SAVED_BOX_FULL";

      case RSN_VMS_LESS_THAN_MIN_LENGTH:
         return (TEXT*)"RSN_VMS_LESS_THAN_MIN_LENGTH";

      case RSN_VMS_BL_LIST_EXCEEDED:
         return (TEXT*)"RSN_VMS_BL_LIST_EXCEEDED";

      case RSN_VMS_MAIL_LIST_EXCEEDED:
         return (TEXT*)"RSN_VMS_MAIL_LIST_EXCEEDED";

      case RSN_VMS_GET_NEW_MAIL_ID_FAILED:
         return (TEXT*)"RSN_VMS_GET_NEW_MAIL_ID_FAILED";

      case RSN_VMS_FWD_NO_ACCOUNT:
         return (TEXT*)"RSN_VMS_FWD_NO_ACCOUNT";

      case RSN_VMS_RPLY_NO_ACCOUNT:
         return (TEXT*)"RSN_VMS_RPLY_NO_ACCOUNT";

      case RSN_VMS_USER_ABORT :
         return (TEXT*)"RSN_VMS_USER_ABORT";
    
      case RSN_VMS_RETRY_ATTEMPT_EXCEEDED:
         return (TEXT*)"RSN_VMS_RETRY_ATTEMPT_EXCEEDED";
    
      case RSN_VMS_BILLING_FAIL:
         return (TEXT*)"RSN_VMS_BILLING_FAIL";
    
      case RSN_VMS_AUTO_INSUFFICIENT_BALANCE:
         return (TEXT*)"RSN_VMS_AUTO_INSUFFICIENT_BALANCE";
      
      case RSN_VMS_AUTO_BILLING_FAIL:
         return (TEXT*)"RSN_VMS_AUTO_BILLING_FAIL";

      default:
          return (TEXT*)"\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertReqModeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT*
//----------------------------------------------------------------------
TEXT* IvrUtil::ConvertReqModeToStr (const int reqMode)
{
   switch (reqMode)
   {
      case IVR:
         return (TEXT*)"IVR";

      case SMS:
         return (TEXT*)"SMS";

      case USSD:
         return (TEXT*)"USSD";

      case GUI:
         return (TEXT*)"GUI";

      case VALIDITY:
         return (TEXT*)"VALIDITY";

      default:
         return (TEXT*)"\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD NAME : DisplayEvr
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : void
//----------------------------------------------------------------------
void IvrUtil::DisplayEvr (IvrEvrMsg & rEvrMsg)
{
   printf ("\n=======================IvrEvrMsg=====================\n");
   IVR_PRINT_SPACE;
   printf ("EvrEvent--------------:%d[%s]\n", rEvrMsg.evrEvent,
         IvrUtil::ConvertEvrEventToStr (rEvrMsg.evrEvent));
   IVR_PRINT_SPACE;
   printf ("EvrCause--------------:%d[%s]\n", rEvrMsg.evrCause,
         IvrUtil::ConvertEvrCauseToStr (rEvrMsg.evrCause));
   IVR_PRINT_SPACE;
   printf ("EvrReason-------------:%d[\33[32m%s\33[0m]\n", rEvrMsg.evrReason,
         IvrUtil::ConvertEvrReasonToStr (rEvrMsg.evrReason));
   IVR_PRINT_SPACE;
   printf ("ReqMode---------------:%d[%s]\n", rEvrMsg.reqMode,
         IvrUtil::ConvertReqModeToStr (rEvrMsg.reqMode));
   IVR_PRINT_SPACE;
   printf ("CalRefNum-------------:%s\n", rEvrMsg.callRefNum);
   IVR_PRINT_SPACE;
   printf ("RefId-----------------:%d\n", rEvrMsg.refId);
   IVR_PRINT_SPACE;
   printf ("ShortCode-------------:%s\n", rEvrMsg.shortCode);
   IVR_PRINT_SPACE;
   printf ("CldNum----------------:%s\n", rEvrMsg.cldNum);
   IVR_PRINT_SPACE;
   printf ("ClgNum----------------:%s\n", rEvrMsg.clgNum);
   IVR_PRINT_SPACE;
   printf ("Content---------------:%s[%d]\n", rEvrMsg.content, 
         (int) strlen(rEvrMsg.content));
   IVR_PRINT_SPACE;
   printf ("Duration--------------:%d\n", rEvrMsg.duration);
   IVR_PRINT_SPACE;
   printf ("Amount----------------:%d\n", rEvrMsg.charge);
   IVR_PRINT_SPACE;
   printf ("GenTime---------------:%ld\n", rEvrMsg.genTime);
   printf ("\n======================================================\n");
}

//----------------------------------------------------------------------
// METHOD NAME : DisplayCdr
// DESCRIPTION :
// PARAMETERS  : None
// RETURN      : void
//----------------------------------------------------------------------
void IvrUtil::DisplayCdr (IvrCdrMsg & rCdrMsg)
{
   printf ("\n=======================IvrCdrMsg=====================\n");
   IVR_PRINT_SPACE;
   printf ("SubsNum---------------:%s\n", rCdrMsg.subsNum);
   IVR_PRINT_SPACE;
   printf ("SubsType--------------:%d\n", rCdrMsg.subsType);
   IVR_PRINT_SPACE;
   printf ("CalRefNum-------------:%s\n", rCdrMsg.callRefNum);
   IVR_PRINT_SPACE;
   printf ("ServiceType-----------:%d\n", rCdrMsg.serviceType);
   IVR_PRINT_SPACE;
   printf ("Interface-------------:%d\n", rCdrMsg.interface);
   IVR_PRINT_SPACE;
   printf ("RefId-----------------:%d\n", rCdrMsg.refId);
   IVR_PRINT_SPACE;
   printf ("Content---------------:%s[%d]\n", rCdrMsg.content, 
         (int)strlen(rCdrMsg.content));
   IVR_PRINT_SPACE;
   printf ("Duration--------------:%d\n", rCdrMsg.duration);
   IVR_PRINT_SPACE;
   printf ("Amount----------------:%d\n", rCdrMsg.charge);
   IVR_PRINT_SPACE;
   printf ("Units-----------------:%d\n", rCdrMsg.units);
   printf ("\n======================================================\n");
}

//----------------------------------------------------------------------
// METHOD      : ConvertSchTypeToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT
//----------------------------------------------------------------------
TEXT* IvrUtil::ConvertSchTypeToStr (const int schType)
{
   switch(schType)
   {
      case IVR_DEDICATION:
         return (TEXT*)"DEDICATION";

      case IVR_REMINDER:
         return (TEXT*)"REMINDER";

      case IVR_OTHERS:
         return (TEXT*)"OTHERS";

      default:
         return (TEXT*)"\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : ConvertStatusToStr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : TEXT
//----------------------------------------------------------------------
TEXT* IvrUtil::ConvertStatusToStr (const int ivrStatus)
{
   switch(ivrStatus)
   {
      case IVR_SCHEDULED:
         return (TEXT*)"SCHEDULED";

      case IVR_IN_PROCESS:
         return (TEXT*)"IN_PROCESS";

      case IVR_DELIVERED:
         return (TEXT*)"DELIVERED";

         //case IVR_TEMP_FAILURE:
         // return "TEMP_FAILURE";

      case IVR_FAILED:
         return (TEXT*)"FAILED";

      default:
         return (TEXT*)"\33[33mUNKNOWN\33[0m";
   }
}

//----------------------------------------------------------------------
// METHOD      : Replace
// DESCRIPTION : Repalces string
// PARAMETER   :
// RETURN      : None
//----------------------------------------------------------------------
void IvrUtil::Replace (TEXT * str, const TEXT * find, const TEXT * replace)
{
   TEXT *lPtrBeg;
   TEXT *lPtrEnd;
   TEXT *lPtrTemp;
   INT16 lLenDiff;
   UINT8 lIndex = 0;

   lPtrBeg = strstr (str, find);

   if (NULLPTR != lPtrBeg)
   {
      lPtrEnd = lPtrBeg + strlen (find);
      lLenDiff = strlen (find) - strlen (replace);

      if (0 == lLenDiff)   // simply replace the contents
      {
         lPtrTemp = lPtrEnd;
      }
      else
      {
         lPtrTemp = lPtrBeg + strlen (replace);
         memmove (lPtrTemp, lPtrEnd, strlen (lPtrEnd) + 1);
      }

      // finally replace the str
      while (lPtrBeg != lPtrTemp)
      {
         *lPtrBeg = replace[lIndex++];
         lPtrBeg++;
      }
   }

   return;
}

//----------------------------------------------------------------------
// METHOD      : ReplaceAll
// DESCRIPTION : Repalces All occurences of substring in main string
// PARAMETER   : source String, pattern, substitue string, dest string
// RETURN      : length of new string after substitution
//----------------------------------------------------------------------
int IvrUtil::ReplaceAll(char* str, char* pattern, char* subs, char *dest)
{
   int lLength, lTempLength, lPatternLength;
   int lTempIdx = 0, lIdx = 0;

   lLength = strlen(str);
   lTempLength = strlen(subs);
   lPatternLength = strlen(pattern);

   //printf("String: %d %s, Substitute: %c, %d, %s\n", lLength, str, pattern, lTempLength, subs);
   for(lIdx = 0; lIdx<lLength;)
   {
      if(strncmp(pattern, &str[lIdx], lPatternLength) == 0)
      {
         for(int i=0; i<lTempLength; i++)
            dest[lTempIdx++] = subs[i];

         lIdx += lPatternLength ;
      }
      else
         dest[lTempIdx++] = str[lIdx++];
   }

   dest[lTempIdx] = '\0';

   //printf("String: %d %s\n", lTempLength, dest);
   return lTempIdx;
}

