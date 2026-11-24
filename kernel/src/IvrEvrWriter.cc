static const char *id ="@(#) $Id: IvrEvrWriter.cc,v 1.1.4.3.2.2 2016/05/13 02:14:34 mram Exp $";
//----------------------------------------------------------------------
// NAME
// IvrEvrWriter.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated : Roopa A M                                    22-AUG-2011
//----------------------------------------------------------------------
#include "IvrEvrApi.h"
#include "IvrConstDef.h"
#include "IvrDbManager.h"
#include "IvrLimits.h"
//#include "CdrGenerator.h"
#include "CdrApi.h"

//----------------------------------------------------------------------
Log gLog;
CTrace gTrace (TRACE_IVR_EVR_WRITER_ENV);
TEXT gProcessName[IVR_MAX_PROCESS_NAME + 1] = "";
//HealthCheckApi gHealthCheckObj;

key_t gCdrQueKey;
key_t gCdrInterQueKey;
MsgQueue gCdrInterQue;

INT16 gIvrCdrModuleId;
INT16 gIvrEvrModuleId;

INT16 gDbClient;

TEXT gLogText[MAX_LOG_TEXT_LEN + 1] = "";

IvrEvrMsg    gEvrMsg;
IvrCdrMsg    gCdrMsg;

IvrDbManager gDbObj;
IvrEvrApi    gIvrEvrApi;

UINT8 gGenerateEvr;
UINT8 gGenerateCdr;

TEXT gEvrDateFormat[IVR_MAX_DATE_FORMAT_LEN + 1];
TEXT gCdrDateFormat[IVR_MAX_DATE_FORMAT_LEN + 1];

UINT16 gIvrEvrEvents[IVR_MAX_EVR_EVENT + 1];
UINT16 gIvrCdrEvents[IVR_MAX_EVR_EVENT + 1];

// To Generate EVR in File, which will be uploaded to Database later using sqlloader
BOOLEAN           gTimeout = false;
UINT32            gEvrMisFileWriteTime=10;
FILE*             gFilePtr       = NULL;

TEXT              gEvrMisProcessingPath[IVR_MAX_FILE_NAME_LEN];
TEXT              gEvrMisActivePath[IVR_MAX_FILE_NAME_LEN];

TEXT              gFileName[IVR_MAX_FILE_NAME_LEN]   = "";

TEXT              gTimeStamp[IVR_MAX_TIME_LEN] = "";
TEXT              gTempStr[IVR_MAX_FILE_NAME_LEN]  = "";

void        EvrMisTimer(int sig);

BOOLEAN Init ();

BOOLEAN ReadConfig ();

void ReloadConfig ();

BOOLEAN CreateQueue ();

BOOLEAN ReadEvrMsg ();

BOOLEAN GenerateEvr ();

BOOLEAN GenerateCdr ();

BOOLEAN WriteIvrEvrToLoderFile ();

void EvrMisTimer(int sig)
{
      gTimeout = true;
         signal(SIGALRM   , EvrMisTimer);
}


//-------------------------------------------------------------
// METHOD      : ReadCdrData
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN ReadCdrData ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lTempStr[IVR_MAX_EVR_EVENTS_LEN + 1] = "";

   //Read cdr data related to prepaid
   if (lCfgRead.CfgInit (IVR_CDR_CFG) != true)
   {
      gLog.GenerateLog (GSYS09, IVR_CDR_CFG);
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, IVR_CDR_CFG););
      return false;
   }

   if (lCfgRead.GetConfigNum ("MODULEID", gIvrCdrModuleId,
            IVR_MIN_CDR_MODULEID,
            IVR_MAX_CDR_MODULEID) != CFG_OK)
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MODULEID(IvrCdr) = %d\n",
               gProcessName, gIvrCdrModuleId););
   }

   lCfgRead.CfgDeInit ();

   if (lCfgRead.CfgInit (IVR_SYS_CFG) != true)
   {
      gLog.GenerateLog (GSYS09, IVR_SYS_CFG);
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, IVR_SYS_CFG););

      return false;
   }

   if (lCfgRead.GetConfigNum ("GENERATE_CDR", gGenerateCdr,
            IVR_MIN_GENERATE_CDR,
            IVR_MAX_GENERATE_CDR) != CFG_OK)
   {
      gGenerateCdr = false;
   }
   T (gTrace, printf ("%s: GENERATE_CDR = %d\n", gProcessName, gGenerateCdr););

   if (lCfgRead.GetConfigStr ("CDR_DATE_FORMAT", gCdrDateFormat,
            IVR_MIN_DATE_FORMAT_LEN,
            IVR_MAX_DATE_FORMAT_LEN) != CFG_OK)
   {
      strcpy (gCdrDateFormat, CDR_DATE_FORMAT);
   }
   T (gTrace, printf ("%s: CDR_DATE_FORMAT = %s\n",
            gProcessName, gCdrDateFormat););

   memset (lTempStr, 0, sizeof (lTempStr));
   if (lCfgRead.GetConfigStr ("IVR_CDR_EVENTS", lTempStr,
            IVR_MIN_CDR_EVENTS_LEN,
            IVR_MAX_CDR_EVENTS_LEN) == CFG_OK)
   {
      TEXT lToken[IVR_MAX_CDR_EVENTS_LEN + 1] = "";
      int lEvent;

      T (gTrace,
            printf ("%s: IVR_CDR_EVENTS = %s\n", gProcessName, lTempStr););

      memset (gIvrCdrEvents, 0, sizeof (gIvrCdrEvents));

      StrTokenizer strtok (lTempStr);

      strtok.GetNextToken (lToken, ',');

      while ('\0' != lToken[0])
      {
         lEvent = atoi (lToken);

         //lEvent = atoi (lToken);
         if (lEvent < 0 || lEvent > IVR_MAX_CDR_EVENT)
         {
            TERR (gTrace,
                  printf ("%s: CDR_EVENT:%d. Discarding invalid Cdr event\n",
                     gProcessName, lEvent););
         }
         else
         {
            T (gTrace, printf ("%s: IVR_CDR_EVENT = %d\n",
                     gProcessName, lEvent););
            gIvrCdrEvents[lEvent] = true;
         }
         strtok.GetNextToken (lToken, ',');
      }
   }

   lCfgRead.CfgDeInit ();

   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadEvrData
// DESCRIPTION :
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN ReadEvrData ()
{
   CfgRead lCfgRead (gProcessName);
   TEXT lTempStr[IVR_MAX_EVR_EVENTS_LEN + 1] = "";

   if (lCfgRead.CfgInit (IVR_SYS_CFG) != true)
   {
      gLog.GenerateLog (GSYS09, IVR_SYS_CFG);
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, IVR_SYS_CFG););

      return false;
   }

   if (lCfgRead.GetConfigNum ("GENERATE_EVR", gGenerateEvr,
            IVR_MIN_GENERATE_EVR,
            IVR_MAX_GENERATE_EVR) != CFG_OK)
   {
      gGenerateEvr = false;
   }
   T (gTrace, printf ("%s: GENERATE_EVR = %d\n", gProcessName, gGenerateEvr););

   if (lCfgRead.GetConfigStr ("EVR_DATE_FORMAT", gEvrDateFormat,
            IVR_MIN_DATE_FORMAT_LEN,
            IVR_MAX_DATE_FORMAT_LEN) != CFG_OK)
   {
      strcpy (gEvrDateFormat, EVR_DATE_FORMAT);
   }
   T (gTrace, printf ("%s: EVR_DATE_FORMAT = %s\n",
            gProcessName, gEvrDateFormat););

   TEXT   lEvrMisPath[IVR_MAX_FILE_NAME_LEN] = "";
   if (CFG_OK != lCfgRead.GetConfigStr ("IVR_EVR_MIS_PATH", lEvrMisPath,
            IVR_MIN_FILE_PATH, IVR_MAX_FILE_PATH))
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN, "Config read failed for %s, configuring default",
            "IVR_EVR_MIS_PATH");
      strcpy(lEvrMisPath, "/opt/ivr/var/EVRRecord");
      TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
   }
   snprintf(gEvrMisActivePath, IVR_MAX_FILE_NAME_LEN, "%s/active", lEvrMisPath);
   T (gTrace, printf ("%s: %s_ACTIVE = %s\n",
            gProcessName, "IVR_EVR_MIS_PATH", gEvrMisActivePath););

   snprintf(gEvrMisProcessingPath, IVR_MAX_FILE_NAME_LEN, "%s/processing", lEvrMisPath);
   T (gTrace, printf ("%s: %s_PROCESSING = %s\n",
            gProcessName, "IVR_EVR_MIS_PATH", gEvrMisProcessingPath););

   if (lCfgRead.GetConfigNum ("EVR_FILE_WRITE_TIME", gEvrMisFileWriteTime,
            1,
            60) != CFG_OK)
   {
      gEvrMisFileWriteTime = 10;
   }
   T (gTrace, printf ("%s: EVR_FILE_WRITE_TIME = %d\n", gProcessName, gEvrMisFileWriteTime););

   memset (lTempStr, 0, sizeof (lTempStr));
   if (lCfgRead.GetConfigStr ("IVR_EVR_EVENTS", lTempStr,
            IVR_MIN_EVR_EVENTS_LEN,
            IVR_MAX_EVR_EVENTS_LEN) == CFG_OK)
   {
      TEXT lToken[IVR_MAX_EVR_EVENTS_LEN + 1] = "";
      int lEvent;

      T (gTrace,
            printf ("%s: IVR_EVR_EVENTS = %s\n", gProcessName, lTempStr););

      memset (gIvrEvrEvents, 0, sizeof (gIvrEvrEvents));

      StrTokenizer strtok (lTempStr);

      strtok.GetNextToken (lToken, ',');

      while ('\0' != lToken[0])
      {
         lEvent = atoi (lToken);

         if (lEvent < 0 || lEvent > IVR_MAX_EVR_EVENT)
         {
            TERR (gTrace,
                  printf ("%s: IVR_EVR_EVENT:%d. Discarding invalid Evr event\n",
                     gProcessName, lEvent););
         }
         else
         {
            T (gTrace, printf ("%s: IVR_EVR_EVENT = %d\n",
                     gProcessName, lEvent););

            gIvrEvrEvents[lEvent] = true;
         }
         strtok.GetNextToken (lToken, ',');
      }
   }
   else
   {
      gLog.GenerateLog (GSYS09, IVR_SYS_CFG);
      TERR (gTrace, printf ("%s: Evr EventsNot Configured %s\n",
               gProcessName, IVR_SYS_CFG););

      return false;
   }

   lCfgRead.CfgDeInit ();

   //Read evr data related to EVR to be generated in file
   if (lCfgRead.CfgInit (IVR_EVR_CFG) != true)
   {
      gLog.GenerateLog (GSYS09, IVR_EVR_CFG);
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, IVR_EVR_CFG););

      return false;
   }

   if (lCfgRead.GetConfigNum ("MODULEID", gIvrEvrModuleId,
            IVR_MIN_CDR_MODULEID,
            IVR_MAX_CDR_MODULEID) != CFG_OK)
   {
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace, printf ("%s: MODULEID(IvrProvEvr) = %d\n",
               gProcessName, gIvrEvrModuleId););
   }

   lCfgRead.CfgDeInit ();

   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN ReadConfig ()
{
   CfgRead lCfgRead (gProcessName);

   if (lCfgRead.CfgInit (IVR_IPC_CFG) != true)
   {
      TERR (gTrace, printf ("%s: Could not open file %s\n",
               gProcessName, IVR_IPC_CFG););
      gLog.GenerateLog (GSYS09, IVR_IPC_CFG);
      return false;
   }

   if (lCfgRead.GetConfigNum ("MSG_CDR_RCV", gCdrQueKey,
            IVR_MIN_MSG_Q_KEY, IVR_MAX_MSG_Q_KEY) != CFG_OK)
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s.",
            "MSG_CDR_RCV", IVR_IPC_CFG);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
      lCfgRead.CfgDeInit ();
      return false;
   }
   else
   {
      T (gTrace,
            printf ("%s: MSG_CDR_RCV = %d\n", gProcessName, gCdrQueKey););
   }

   // If configuration key : MSG_CDR_INTERMEDIATE_QUEUE not found in the
   // configuration file, the ussd kernel process directly writes the CDR
   // into MSG_CDR_RCV queue, assuming no intermediate queue is needed.
   if (lCfgRead.GetConfigNum ("MSG_CDR_INTERMEDIATE_QUEUE",
            gCdrInterQueKey, IVR_MIN_MSG_Q_KEY,
            IVR_MAX_MSG_Q_KEY) != CFG_OK)
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN,
            "Config read failed for %s. File:%s. Using MSG_CDR_RCV ",
            "MSG_CDR_INTERMEDIATE_QUEUE", IVR_IPC_CFG);

      T (gTrace, printf ("%s: %s\n", gProcessName, gLogText););

      gCdrInterQueKey = gCdrQueKey;
   }

   T (gTrace, printf ("%s: MSG_CDR_INTERMEDIATE_QUEUE = %d\n",
            gProcessName, gCdrInterQueKey););
   
   lCfgRead.CfgDeInit ();
      
   if (lCfgRead.CfgInit (IVR_KER_CFG) != true)
      {
         TERR (gTrace, printf ("%s: Could not open file %s\n",
                  gProcessName, IVR_KER_CFG););
         gLog.GenerateLog (GSYS09,(TEXT*) IVR_KER_CFG);
         return false;
      }

      if (CFG_OK !=
            lCfgRead.GetConfigNum ("IVR_DB_CLIENT", gDbClient,
               0, 10))
      {
         snprintf (gLogText, MAX_LOG_TEXT_LEN,
               "Config read failed for %s. File:%s.",
               "IVR_DB_CLIENT", IVR_KER_CFG);
         TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
         lCfgRead.CfgDeInit ();
         return false;
      }
      else
      {
         T (gTrace, printf ("%s: IVR_DB_CLIENT = %d\n",
                  gProcessName, gDbClient););
      }

   lCfgRead.CfgDeInit ();

   if (true != ReadCdrData ())
      return false;

   if (true != ReadEvrData ())
      return false;

   return true;
}

//-------------------------------------------------------------
// METHOD      : ReloadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//-------------------------------------------------------------
void ReloadConfig ()
{
   ReadEvrData ();

   ReadCdrData ();

   return;
}

//-------------------------------------------------------------
// METHOD      : CreateQueue
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN CreateQueue ()
{
   msgQueueError lQueErr;

   lQueErr = gCdrInterQue.Create (gCdrInterQueKey, IPC_CREAT | IPC_PERM);
   if (lQueErr != Q_SUCCESS)
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN,
            "Error creating MSG_CDR_INTERMEDIATE_QUEUE with %d",
            gCdrInterQueKey);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
      return false;
   }

   return true;
}

//-------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN Init ()
{
   memset (gLogText, 0, sizeof (gLogText));

   if (true != ReadConfig ())
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN, "ReadConfig failed. Exiting...");
      gLog.GenerateLog (GSYS04, gLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
      return false;
   }

   if (true != CreateQueue ())
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN, "CreateQueue failed. Exiting...");
      gLog.GenerateLog (GSYS04, gLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
      return false;
   }

   if (true != gIvrEvrApi.Init ())
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN,
            "Evr Api Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, gLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
      return false;
   }

   if (DB_NO_ERR != gDbObj.Init (gDbClient))
   {
      snprintf (gLogText, MAX_LOG_TEXT_LEN,
            "Db Object Init failed. Exiting...");
      gLog.GenerateLog (GSYS04, gLogText);
      TERR (gTrace, printf ("%s: %s\n", gProcessName, gLogText););
      return false;
   }

   return true;
}

//----------------------------------------------------------------------
// METHOD NAME : WriteToCdrQueue
// DESCRIPTION : 
// PARAMETERS  : None
// RETURN      : BOOLEAN
//----------------------------------------------------------------------
BOOLEAN WriteToCdrQueue (CdrRecord & rCdrRecord, int msgType)
{
   msgQueueError lQueErr;
   INT16 lMsgLen = sizeof (rCdrRecord);

   lQueErr =
      gCdrInterQue.WriteMsg ((void *) &rCdrRecord, lMsgLen, msgType, BLOCKING);
   if (lQueErr != Q_SUCCESS)
   {
      lMsgLen = sizeof (rCdrRecord);

      if (true != IvrUtil::HandleQueueWriteError (lQueErr, gCdrInterQue,
               &rCdrRecord, lMsgLen, msgType, BLOCKING))
      {
         return false;
      }
   }

   return true;
}

//-------------------------------------------------------------
// METHOD      : WriteIvrEvrToLoderFile
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN 
//-------------------------------------------------------------
BOOLEAN WriteIvrEvrToLoderFile ()
{
   timeval     lTimeval;
   struct tm*  lTime;
   TEXT        lTimeString[IVR_MAX_TIME_LEN] = "";
   static UINT8 lsLastDay = 0;

   if (!(gGenerateEvr & IVR_GEN_EVR_DB))
      return true;

   lTime = localtime (&gEvrMsg.genTime);

   // Added to close the current file at the end of the day, and open a new file for new day
   if((lTime->tm_mday != lsLastDay) && (gFilePtr != NULL))
   {
      fclose(gFilePtr);
      sprintf(gTempStr, "mv %s/IVR_EVR_*.txt  %s",
		      gEvrMisActivePath,
		      gEvrMisProcessingPath);
      system(gTempStr);
      gFilePtr = NULL;
   }	

   if( gFilePtr == NULL)
   {
      memset(&lTimeString , 0 , IVR_MAX_TIME_LEN);
      gettimeofday(&lTimeval, NULL);
      lTime = localtime(&(lTimeval.tv_sec));

      sprintf(lTimeString, "%02d%02d%02d_%02d%02d", lTime->tm_mday, lTime->tm_mon+1, lTime->tm_year%100 ,
            lTime->tm_hour, lTime->tm_min );    
      lsLastDay = lTime->tm_mday;

      sprintf(gFileName , "%s/IVR_EVR_%s.txt",
            gEvrMisActivePath,
            lTimeString);

      gFilePtr = fopen(gFileName ,"a");

      if(gFilePtr == NULL)
      {
         snprintf(gLogText, MAX_LOG_TEXT_LEN, "IVR_EVR File Open Failed :%s",
               gFileName);
         T(gTrace , printf("%s: %s\n", gProcessName, gLogText););
         gLog.GenerateLog(IVR_EVR10, gLogText);
         return false;
      }
      lTime = localtime (&gEvrMsg.genTime);
   }

   sprintf(lTimeString, "%04d-%02d-%02d %02d:%02d:%02d", (1900+lTime->tm_year), lTime->tm_mon+1, lTime->tm_mday,
         lTime->tm_hour, lTime->tm_min, lTime->tm_sec);
   //Fields List
   //+--------------+--------------+------+-----+---------+-------+
   //| Field        | Type         | Null | Key | Default | Extra |
   //+--------------+--------------+------+-----+---------+-------+
   //| EVR_EVENT    | smallint(6)  | YES  | MUL | NULL    |       |
   //| EVR_CAUSE    | smallint(6)  | YES  | MUL | NULL    |       |
   //| EVR_REASON   | smallint(6)  | YES  | MUL | NULL    |       |
   //| REF_ID       | int(11)      | YES  |     | NULL    |       |
   //| SHORT_CODE   | varchar(20)  | NO   |     | NULL    |       |
   //| REQUEST_MODE | int(11)      | YES  |     | NULL    |       |
   //| CALL_REF_NUM | varchar(20)  | YES  |     | NULL    |       |
   //| CLD_NUM      | varchar(20)  | YES  |     | NULL    |       |
   //| CLG_NUM      | varchar(20)  | YES  |     | NULL    |       |
   //| CONTENT_ID   | int(11)      | YES  |     | NULL    |       |
   //| CONTENT      | varchar(100) | YES  |     | NULL    |       |
   //| DURATION     | int(11)      | YES  |     | NULL    |       |
   //| CHARGE_UNIT  | smallint(6)  | YES  |     | 0       |       |
   //| CHARGE       | int(11)      | YES  |     | NULL    |       |
   //| GEN_TIME     | datetime     | YES  |     | NULL    |       |
   //| SCHEDULE_ID  | int(11)      | YES  |     | NULL    |       |
   //| CAMP_ID      | int(11)      | YES  |     | NULL    |       |
   //+--------------+--------------+------+-----+---------+-------+

   fprintf(gFilePtr, "%d,%d,%d,%ld,%s,%ld,%s,%s,%s,,%s,%ld,%d,%ld,%s,%ld,%ld\n",
         gEvrMsg.evrEvent,
         gEvrMsg.evrCause,
         gEvrMsg.evrReason,
         (LONG)gEvrMsg.refId,
         gEvrMsg.shortCode,
         (LONG)gEvrMsg.reqMode,
         gEvrMsg.callRefNum,
         gEvrMsg.cldNum,
         gEvrMsg.clgNum,
         gEvrMsg.content,
         (LONG)gEvrMsg.duration,
         0,                      // CHARGE_UNIT
         (LONG)gEvrMsg.charge,
         lTimeString,
         (LONG)gEvrMsg.scheduleId,
         (LONG)gEvrMsg.campId);
   
   T(gTrace,printf("%s: %s Wrote to Evr Master File\n", gProcessName, gLogText););
   gLog.GenerateLog(IVR_EVR09, 2, gLogText);

   return true;
}

//-------------------------------------------------------------
// METHOD      : WriteIvrEvrToDb
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN 
//-------------------------------------------------------------
BOOLEAN WriteIvrEvrToDb ()
{
   if (!(gGenerateEvr & IVR_GEN_EVR_DB))
      return true;

   DbErrorCodes lRetVal = DB_NO_ERR;

   for (UINT16 lCount = 0; lCount <= IVR_DB_ERR_RETRY_ATTEMPTS; lCount++)
   {
      lRetVal = gDbObj.InsertEvr (gEvrMsg);

      if (DB_NO_ERR == lRetVal)
      {
         gLog.GenerateLog (IVR_EVR09, 1, gLogText);
         T (gTrace, printf ("%s: %s ProvEvrMsg DB-Write success\n",
                  gProcessName, gLogText););

         return true;
      }
      else
      {
         usleep (IVR_DB_ERR_SLEEP_TIME);
      }
   }

   gLog.GenerateLog (IVR_EVR10, gLogText);
   TERR (gTrace, printf ("%s: %s Failed to insert ProvEvrMsg in DB\n",
            gProcessName, gLogText););

   return false;
}

//-------------------------------------------------------------
// METHOD      : WriteIvrEvrToFile
// DESCRIPTION : 
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN WriteIvrEvrToFile ()
{
   if (!(gGenerateEvr & IVR_GEN_EVR_FILE))
      return true;

   CdrRecord lEvrMsg;
   struct tm *tm_ptr;

   memset (&lEvrMsg, 0, sizeof (lEvrMsg));

   //Mapping evr fields
   lEvrMsg.num1 = (UINT32) gEvrMsg.reqMode;
   lEvrMsg.num2 = (UINT32) gEvrMsg.refId;
   lEvrMsg.num3 = (UINT32) gEvrMsg.duration;
   lEvrMsg.num4 = (UINT32) gEvrMsg.charge;
   lEvrMsg.num5 = (LONG) gEvrMsg.callRefNum;

   strncpy (lEvrMsg.text1, IvrUtil::ConvertEvrEventToStr (gEvrMsg.evrEvent),
         sizeof (lEvrMsg.text1) - 1);
   strncpy (lEvrMsg.text2, IvrUtil::ConvertEvrCauseToStr (gEvrMsg.evrCause),
         sizeof (lEvrMsg.text2) - 1);
   strncpy (lEvrMsg.text3, IvrUtil::ConvertEvrCauseToStr (gEvrMsg.evrReason),
         sizeof (lEvrMsg.text3) - 1);
   strncpy (lEvrMsg.text4, IvrUtil::ConvertReqModeToStr (gEvrMsg.reqMode),
         sizeof (lEvrMsg.text4) -1);
   strncpy (lEvrMsg.text5, gEvrMsg.clgNum, sizeof (lEvrMsg.text5) - 1);
   strncpy (lEvrMsg.text6, gEvrMsg.cldNum, sizeof (lEvrMsg.text6) - 1);
   strncpy (lEvrMsg.text7, gEvrMsg.content, sizeof (lEvrMsg.text7) - 1);

   tm_ptr = localtime (&gEvrMsg.genTime);
   strftime (lEvrMsg.text8, sizeof (lEvrMsg.text8) - 1, gEvrDateFormat, tm_ptr);

   //Write cdr message
   if (true != WriteToCdrQueue (lEvrMsg, gIvrEvrModuleId))
   {
      gLog.GenerateLog (IVR_EVR07, gLogText);
      TERR (gTrace, printf ("%s: %s Err:%d[%s] WriteToCdrQueue(Evr) Failed\n",
               gProcessName, gLogText, errno, strerror (errno)););

      return false;
   }

   gLog.GenerateLog (IVR_EVR08, 1, gLogText);
   T (gTrace, printf ("%s: %s WriteToCdrQueue(Evr) Success\n",
            gProcessName, gLogText););

   return true;
}

//-------------------------------------------------------------
// METHOD      : GenerateEvr
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN 
//-------------------------------------------------------------
BOOLEAN GenerateEvr ()
{
   WriteIvrEvrToLoderFile();
/*   WriteIvrEvrToDb ();
   WriteIvrEvrToFile ();
*/
   return true;
}

//-------------------------------------------------------------
// METHOD      : WriteIvrCdrToDb
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN 
//-------------------------------------------------------------
BOOLEAN WriteIvrCdrToDb ()
{
   if (!(gGenerateCdr & IVR_GEN_CDR_DB))
      return true;

   DbErrorCodes lRetVal = DB_NO_ERR;

   for (UINT16 lCount = 0; lCount <= IVR_DB_ERR_RETRY_ATTEMPTS; lCount++)
   {
      lRetVal = gDbObj.InsertCdr (gCdrMsg);

      if (DB_NO_ERR == lRetVal)
      {
         gLog.GenerateLog (IVR_EVR09, 1, gLogText);
         T (gTrace, printf ("%s: %s ProvCdrMsg DB-Write success\n",
                  gProcessName, gLogText););

         return true;
      }
      else
      {
         usleep (IVR_DB_ERR_SLEEP_TIME);
      }
   }

   gLog.GenerateLog (IVR_EVR10, gLogText);
   TERR (gTrace, printf ("%s: %s Failed to insert ProvEvrMsg in DB\n",
            gProcessName, gLogText););

   return false;
}
//-------------------------------------------------------------
// METHOD      : WriteIvrCdrToFile
// DESCRIPTION :
// PARAMETER   : BOOLEAN lIsPrepaid
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN WriteIvrCdrToFile ()
{
   CdrRecord lCdrMsg;
   struct tm *tm_ptr;

   memset (&lCdrMsg, 0, sizeof (lCdrMsg));

   //Mapping evr fields
   lCdrMsg.num1 = (UINT32) gCdrMsg.subsType;
   lCdrMsg.num2 = (LONG) gCdrMsg.callRefNum;
   lCdrMsg.num3 = (UINT32) gCdrMsg.serviceType;
   lCdrMsg.num4 = (UINT32) gCdrMsg.duration;
   lCdrMsg.num5 = (UINT32) gCdrMsg.charge;
   lCdrMsg.num6 = (UINT32) gCdrMsg.units;

   strncpy (lCdrMsg.text1, gCdrMsg.subsNum, sizeof (lCdrMsg.text1) - 1);
   strncpy (lCdrMsg.text2, gCdrMsg.content, sizeof (lCdrMsg.text7) - 1);

   tm_ptr = localtime (&gCdrMsg.genTime);
   strftime (lCdrMsg.text3, sizeof (lCdrMsg.text3) - 1, gCdrDateFormat, tm_ptr);

   if (true != WriteToCdrQueue (lCdrMsg, gIvrCdrModuleId))
   {
      gLog.GenerateLog (IVR_EVR03, gLogText);
      TERR (gTrace, printf ("%s: %s Err:%d[%s] WriteToCdrQueue(Cdr) Failed\n",
               gProcessName, gLogText, errno, strerror (errno)););

      return false;
   }

   gLog.GenerateLog (IVR_EVR04, 1, gLogText);
   T (gTrace, printf ("%s: %s WriteToCdrQueue(Cdr) Success\n",
            gProcessName, gLogText););

   return true;
}

//-------------------------------------------------------------
// METHOD      : GenerateCdr
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN GenerateCdr ()
{
   WriteIvrCdrToDb ();
   WriteIvrCdrToFile ();

   return true;
}

//-------------------------------------------------------------
// METHOD      : ReadEvrMessage
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN ReadEvrMessage ()
{
   gLogText[0] = '\0';
   memset (&gEvrMsg, 0, sizeof (gEvrMsg));

   return gIvrEvrApi.ReadEvrMessage (gEvrMsg);
}

//-------------------------------------------------------------
// METHOD      : ReadCdrMessage
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN ReadCdrMessage ()
{
   gLogText[0] = '\0';
   memset (&gCdrMsg, 0, sizeof (gCdrMsg));

   return gIvrEvrApi.ReadCdrMessage (gCdrMsg);
}

//-------------------------------------------------------------
// METHOD      : main
// DESCRIPTION :
// PARAMETER   : 
// RETURN      : int
//-------------------------------------------------------------
int main (int argc, char *argv[])
{
   ProcessLock lProcessLock;

   strcpy (gProcessName, "IVR_EVR");

   if (argc != 2)
   {
      printf ("Usage %s <instance number> <0-EVR | 1-CDR>\n", gProcessName);
      printf ("Trace: export %s=1\n", TRACE_IVR_EVR_WRITER_ENV);
      
      printf ("--------- Health Configuration ------------------------\n");
      printf ("File: health.cfg\n");
      printf ("HC_MODULE_%d_HEARTBEAT_FAILURE_ALERT_THRESHOLD      = 3\n",
            IVR_EVR_WRITER);
      printf ("HC_MODULE_%d_HEARTBEAT_FAILURE_ERR_THRESHOLD        = 5\n",
            IVR_EVR_WRITER);
      printf ("HC_MODULE_%d_HEARTBEAT_FAILURE_PROCESS_MON_ERR_CODE = 1\n",
            IVR_EVR_WRITER);

      printf ("File: ipc.cfg\n");
      printf ("HC_MODULE_%d_MSG_LEN = %lu\n", IVR_EVR_WRITER, sizeof (IvrEvrMsg));
      printf
         ("MSG_HC_MODULE_%d_Q_RCV should be same as MSG_IVR_EVR_HDLR_Q_RCV\n",
          IVR_EVR_WRITER);
      printf ("----------------- End ---------------------------------\n");
      return 1;
   }
   else
   {
      //strcat (gProcessName, argv[1]);
   }

   IvrUtil::LoadSignals ();

   gLog.SetProcessName (gProcessName);
   printf ("%s: Starting...\n", gProcessName);

   if (true != gLog.Init ())
   {
      printf ("%s: Could not initialize log object. Exiting...\n",
            gProcessName);
      return 1;
   }

   lProcessLock.SetProdProcName (IVR_NAME, gProcessName);
   if (LOCK_SUCCESS != lProcessLock.Lock ())
   {
      gLog.GenerateLog (GSYS16);
      printf ("%s: Process already running. Exiting...\n", gProcessName);
      return 1;
   }

   gLog.GenerateLog (GSYS01);

   gTrace.SetTraceKey (TRACE_IVR_EVR_WRITER_ENV);

   /*if (true != gHealthCheckObj.Init (IVR_EVR_WRITER))
   {
      printf ("%s: Health Check Object Init Failed\n", gProcessName);
      return 1;
   }*/

   if (true != Init ())
   {
      //gHealthCheckObj.WriteMsgToHealthCheck (HC_INIT_FAILURE, HC_CRITICAL);

      printf ("%s: Evr Writer Init Failed\n", gProcessName);
      return 1;
   }

   gLog.GenerateLog (GSYS03);
   printf ("%s:\33[32m Initialization Okay...\33[0m\n", gProcessName);

   BOOLEAN lIsEvrFound = false;
   BOOLEAN lIsCdrFound = false;

   signal(SIGALRM, EvrMisTimer);
   alarm(gEvrMisFileWriteTime * 60);

   while (IvrUtil::KeepRunning ())
   {
      lIsEvrFound = false;
      lIsCdrFound = false;

      if (IvrUtil::ReloadConfig ())
      {
         ReloadConfig ();
         IvrUtil::ResetConfigFlag ();
      }

      if(gTimeout == true)
      {
         gTimeout = false;
         alarm(gEvrMisFileWriteTime * 60);

         if(gFilePtr != NULL )
         {
            fclose(gFilePtr);
            sprintf(gTempStr, "mv %s/IVR_EVR_*.txt  %s",
                  gEvrMisActivePath,
                  gEvrMisProcessingPath);
            system(gTempStr);
            gFilePtr = NULL;
         }
      }
      lIsEvrFound = ReadEvrMessage ();
      //lIsCdrFound = ReadCdrMessage ();

      if (lIsEvrFound || lIsCdrFound)
      {
         if(lIsEvrFound)
         {
            snprintf (gLogText, MAX_LOG_TEXT_LEN,
                  "RefNum:%s EvrEvent:%d[%s] EvrCause:%d[%s] EvrReason:%d[%s] "
                  "ReqMode:%d[%s] Amt:%d ",
                  gEvrMsg.callRefNum,
                  gEvrMsg.evrEvent,
                  IvrUtil::ConvertEvrEventToStr (gEvrMsg.evrEvent),
                  gEvrMsg.evrCause,
                  IvrUtil::ConvertEvrCauseToStr (gEvrMsg.evrCause),
                  gEvrMsg.evrReason,
                  IvrUtil::ConvertEvrReasonToStr (gEvrMsg.evrReason),
                  gEvrMsg.reqMode,
                  IvrUtil::ConvertReqModeToStr (gEvrMsg.reqMode),
                  gEvrMsg.charge);

            if (gEvrMsg.evrEvent > IVR_MAX_EVR_EVENT || gEvrMsg.evrEvent < 0)
            {
               gLog.GenerateLog (IVR_EVR02, gLogText);
               TERR (gTrace, printf ("%s: %s Unknown EvrEvent. Discarding...\n",
                        gProcessName, gLogText););

               continue;
            }

            // Generate Evr if enabled
            if (gGenerateEvr && gIvrEvrEvents[gEvrMsg.evrEvent])
               GenerateEvr ();
            else
               T (gTrace, printf ("%s: %s Evr Generation disabled\n",
                        gProcessName, gLogText););
         }
         // Generate Cdr if enabled
         if(lIsCdrFound)
         {
            if (gGenerateCdr && gIvrCdrEvents[gEvrMsg.evrReason])
               GenerateCdr ();
            else
               T (gTrace, printf ("%s: %s Cdr Generation disabled\n",
                        gProcessName, gLogText););
         }
      }
      else
      {
         sleep(1);
         continue;
      }

   }

   if(gFilePtr != NULL )
   {
      fclose(gFilePtr);
      sprintf(gTempStr, "mv %s/IVR_EVR_*.txt  %s",
            gEvrMisActivePath,
            gEvrMisProcessingPath);
      system(gTempStr);
      gFilePtr = NULL;
   }

   //gHealthCheckObj.WriteMsgToHealthCheck (HC_PROCESS_SHUTDOWN, HC_INFO);

   gLog.GenerateLog (GSYS02);
   printf ("%s:\33[31m Shutting down ...\33[0m\n", gProcessName);

   return 0;
}
