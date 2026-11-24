static const char* id="@(#) $Id: MmpDbManager.cc,v 1.1.4.3 2016/04/12 21:59:41 shijuc Exp $" ;
//---------------------------------------------------------------------------------
//  NAME       :  MmpDbManager.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2011 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated  :  Divya K                                      Dated: 24-JAN-2012
//---------------------------------------------------------------------------------

#include "MmpDbManager.h"

extern  int      errno;

//-------------------------------------------------------------
// METHOD      : MmpDbManager
// DESCRIPTION : constructor
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
MmpDbManager::MmpDbManager()
{
   memset(mDbName, 0, IVR_MAX_DB_NAME_LEN);
   memset(mDbUser, 0, MAX_DB_USER);
   memset(mDbPass, 0, MAX_DB_PASS);
}

//-------------------------------------------------------------
// METHOD      : ~MmpDbManager
// DESCRIPTION : Destructor
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
MmpDbManager::~MmpDbManager()
{
}

//-------------------------------------------------------------------------------
// METHOD      : commit
// DESCRIPTION : Default Destructor
// PARAMETER   : no arguments
// RETURN      : no return value
//-------------------------------------------------------------------------------
void MmpDbManager::CommitNow()
{
   mConnection.Commit();
}

//-------------------------------------------------------------
// METHOD        : connect()
// DESCRIPTION   :
// PARAMETER     : None
// RETURN        : success/failure status
//-------------------------------------------------------------
DbErrorCodes MmpDbManager::Connect()
{
   TEXT*   lpTempPtr = NULL;
   TEXT    lLogText[MAX_LOG_TEXT_LEN + 1]= "";

   try
   {
      lpTempPtr = getenv("MMP_DB_NAME");

      if (NULLPTR == lpTempPtr)
         strncpy(mDbName, IVR_DB_NAME, IVR_MAX_DB_NAME_LEN - 1);
      else
         strncpy(mDbName, lpTempPtr, IVR_MAX_DB_NAME_LEN - 1);

      lpTempPtr = getenv("MMP_DB_USER");

      if (NULLPTR == lpTempPtr)
         strncpy(mDbUser, IVR_DB_USER, MAX_DB_USER - 1);
      else
         strncpy(mDbUser, lpTempPtr, MAX_DB_USER - 1);

      lpTempPtr = getenv("MMP_DB_PASS");

      if (NULLPTR == lpTempPtr)
         strncpy(mDbPass, IVR_DB_PASS, MAX_DB_PASS - 1);
      else
         strncpy(mDbPass, lpTempPtr, MAX_DB_PASS - 1);

      mConnection.Connect(mDbName ,  mDbUser , mDbPass , SA_Oracle_Client);

      snprintf(lLogText, MAX_LOG_TEXT_LEN, "Connected to Database");

      gLog.GenerateLog(DBP08 , lLogText);

      return this->CheckDBStatus ();
   }

   catch(SAException& e)
   {
      return this->ErrorHandler(e,(TEXT*)"ConnectDb()",(TEXT*)"");
   }
}

//-------------------------------------------------------------
// METHOD        : Init()
// DESCRIPTION   :
// PARAMETER     : None
// RETURN        : success/failure status
//-------------------------------------------------------------
DbErrorCodes MmpDbManager::Init(int lDbClient)
{
   TEXT*   lpTempPtr = NULL;
   TEXT    lLogText[MAX_LOG_TEXT_LEN + 1]= "";

   lpTempPtr = getenv("MMP_DB_NAME");

   if (NULLPTR == lpTempPtr)
      strncpy(mDbName, IVR_DB_NAME, IVR_MAX_DB_NAME_LEN - 1);
   else
      strncpy(mDbName, lpTempPtr, IVR_MAX_DB_NAME_LEN - 1);

   lpTempPtr = getenv("MMP_DB_USER");

   if (NULLPTR == lpTempPtr)
      strncpy(mDbUser, IVR_DB_USER, MAX_DB_USER - 1);
   else
      strncpy(mDbUser, lpTempPtr, MAX_DB_USER - 1);

   lpTempPtr = getenv("MMP_DB_PASS");

   if (NULLPTR == lpTempPtr)
      strncpy(mDbPass, IVR_DB_PASS, MAX_DB_PASS - 1);
   else
      strncpy(mDbPass, lpTempPtr, MAX_DB_PASS - 1);

   if(lDbClient == 1)
      mDbClient = SA_MySQL_Client;
   else
      mDbClient = SA_Oracle_Client;

   try
   {
      mConnection.Connect(mDbName,  mDbUser, mDbPass, mDbClient);
      mConnection.setAutoCommit (SA_AutoCommitOn);
      mConnection.setOption("store");
      mCommand.setConnection (&mConnection);

      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Connected to Database lDbName:%s DbUser:%s DbClient:%s", mDbName, mDbUser, 
            ((mDbClient == SA_MySQL_Client)?"MySQL":"Oracle"));
      gLog.GenerateLog(DBP08 , lLogText);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););

      return DB_NO_ERR;
   }

   catch(SAException& e)
   {
      this->ErrorHandler(e,(TEXT*)"Init()",(TEXT*)"");
      return DB_INIT_ERR;
   }
}

//-------------------------------------------------------------
// METHOD        : connect()
// DESCRIPTION   :
// PARAMETER     : None
// RETURN        : success/failure status
//-------------------------------------------------------------
DbErrorCodes MmpDbManager::Init(TEXT* lDbName, TEXT* lUser, TEXT* lPasswd, int lDbClient)
{
   TEXT*   lpTempPtr = NULL;
   TEXT    lLogText[MAX_LOG_TEXT_LEN + 1]= "";

   if(lDbName[0] != '\0')
   {
      strncpy(mDbName, lDbName, IVR_MAX_DB_NAME_LEN - 1);
   }
   else
   {
      lpTempPtr = getenv("MMP_DB_NAME");
      if (NULLPTR != lpTempPtr)
         strncpy(mDbName, lpTempPtr, IVR_MAX_DB_NAME_LEN - 1);
   }

   if (lUser[0] != '\0')
   {
      strncpy(mDbUser, lUser, MAX_DB_USER - 1);
   }
   else
   {
      lpTempPtr = getenv("MMP_DB_USER");
      if (NULLPTR != lpTempPtr)
         strncpy(mDbUser, lpTempPtr, MAX_DB_USER - 1);
   }

   if (lPasswd[0] != '\0')
   {
      strncpy(mDbPass, lPasswd, MAX_DB_PASS - 1);
   }
   else
   {
      lpTempPtr = getenv("MMP_DB_PASS");
      if (NULLPTR != lpTempPtr)
         strncpy(mDbPass, lpTempPtr, MAX_DB_PASS - 1);
   }

   if(lDbClient == 1)
      mDbClient = SA_MySQL_Client;
   else
      mDbClient = SA_Oracle_Client;

   try
   {
      mConnection.Connect(mDbName,  mDbUser, mDbPass, mDbClient);
      mConnection.setAutoCommit (SA_AutoCommitOn);
      mCommand.setConnection (&mConnection);

      snprintf(lLogText, MAX_LOG_TEXT_LEN, 
            "Connected to Database lDbName:%s DbUser:%s DbClient:%s", mDbName, mDbUser, 
            ((mDbClient == SA_MySQL_Client)?"MySQL":"Oracle"));
      gLog.GenerateLog(DBP08 , lLogText);
      T (gTrace, printf ("%s: %s\n", gProcessName, lLogText););

      return DB_NO_ERR;
   }

   catch(SAException& e)
   {
      this->ErrorHandler(e,(TEXT*)"Init()",(TEXT*)"");
      return DB_INIT_ERR;
   }
}

//-------------------------------------------------------------
// METHOD      : Disconnect
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : success/failure status
//-------------------------------------------------------------
DbErrorCodes MmpDbManager::Disconnect()
{
   TEXT   lLogText[MAX_LOG_TEXT_LEN + 1]= "";

   try
   {
      if(mConnection.isConnected())
      {
         mConnection.Disconnect();
         snprintf(lLogText, MAX_LOG_TEXT_LEN, "Disconnected from Database");
         gLog.GenerateLog(DBP09, lLogText);
      }
      return DB_NO_ERR;
   }
   catch(SAException& e)
   {
      this->ErrorHandler(e,(TEXT*)"disconnect()",(TEXT*)"");
      return DB_INIT_ERR;
   }
}

//-------------------------------------------------------------
// METHOD      : CheckDBStatus
// DESCRIPTION : Chacking Status of Database
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
DbErrorCodes MmpDbManager::CheckDBStatus()
{
   try
   {
      if (mConnection.isAlive ())
         return DB_NO_ERR;
      else if (mConnection.isConnected ())
         mConnection.Disconnect ();

      mConnection.Connect (mDbName, mDbUser, mDbPass, mDbClient);
      mConnection.setAutoCommit (SA_AutoCommitOn);
      mCommand.setConnection (&mConnection);
      gLog.GenerateLog (DBP08);
   }

   catch (SAException & e)
   {
      T (gTrace, printf ("%s: Exception while checking Db Status\n",
               gProcessName););

      return this->ErrorHandler (e,(TEXT*) "CheckDBStatus()",(TEXT*)"");
   }

   return DB_NO_ERR;
}


//------------------------------------------------------------------------------
// METHOD      : ErrorHandler
// DESCRIPTION : This function  sets global errno depending upon error retrived
//               from database and generates appropriate log
// PARAMETER   : Reference to SAException , Function Name , Debug Information
// RETURN      : success/failure status
//-------------------------------------------------------------------------------
DbErrorCodes MmpDbManager::ErrorHandler(SAException& err, TEXT* funcName, TEXT* debugInfo)
{
   TEXT            lErrMsg[MAX_LOG_TEXT_LEN];
   DbErrorCodes    lDbError;

   snprintf(lErrMsg, MAX_LOG_TEXT_LEN, "DBP: Func:%s Arg:%s Err:%s,Code %d",
         funcName,debugInfo,(const char *)err.ErrText(),err.ErrNativeCode());

   TERR(gTrace, printf("%s: %s\n", gProcessName, lErrMsg););

   switch(err.ErrNativeCode())
   {
      case 1:
         {
            lDbError = DB_UNIQUE_KEY_VIOLATION_ERR;
            gLog.GenerateLog(DBP04, lErrMsg);
         }
         break;

      case 1403:
         {
            lDbError = DB_NO_RECORD_FOUND_ERR;
            gLog.GenerateLog(DBP03, lErrMsg);
         }
         break;

      case 1012:
      case 1034:
      case 1092:
      case 3113:
      case 3114:
      case 12514:
      case 12519:
      case 12541:
      case 12571:
      case 2006:  //MySQL server has gone away,Code 2006 (DB_CLIENT = 1)
      case 0:
         {
            gLog.GenerateLog (DBP00, lErrMsg);
            try
            {
               if (mConnection.isConnected ())
                  mConnection.Disconnect ();

               mConnection.Connect (mDbName, mDbUser, mDbPass, SA_Oracle_Client);
               mCommand.setConnection (&mConnection);

               T (gTrace, printf ("%s: DbName:%s DbUser:%s Connected to Db\n",
                        gProcessName, mDbName, mDbUser););
               gLog.GenerateLog (DBP08);
               lDbError = DB_NO_ERR;
            }
            catch (SAException & e)
            {
               T (gTrace, printf ("%s: DbName:%s DbUser:%s Failed to Connect\n",
                        gProcessName, mDbName, mDbUser););
               usleep (100);
            }
            lDbError = DB_CONNECT_FAILURE_ERR;
         }
         break;
      default:
         {
            lDbError = DB_UNKNOWN_ERR;
            //gLog.GenerateLog(DBP07, lErrMsg, UNKNOWN);        
	    gLog.GenerateLog(DBP07, lErrMsg); 	         
	 }
         break;
   }
   return lDbError;
}

//----------------------------------------------------------------------
// METHOD      : ReadCfgNum
// DESCRIPTION : Reads the configuration details from the
//               CONFIG_DETAILS table.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::ReadCfgNum (const TEXT * key, INT32 & cfgVal,
      INT32 minVal, INT32 maxVal)
{
   TEXT lCfgStr[MAX_LOG_TEXT_LEN + 1] = "";
   INT32 lCfgVal = 0;
   DbErrorCodes lDbErr;

   // Number can't go beyond 10 digits in a 32 bit machine
   lDbErr = this->ReadCfgStr (key, lCfgStr, 1, 14);
   if (DB_NO_ERR != lDbErr)
   {
      return lDbErr;
   }

   lCfgVal = atol (lCfgStr);

   if (lCfgVal < minVal || lCfgVal > maxVal)
   {
      TERR (gTrace,
            printf ("%s: %s Parameter Value exceeds boundary values.\n",
               gProcessName, key););
      return DB_UNKNOWN_ERR;
   }

   cfgVal = lCfgVal;

   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : ReadCfgStr
// DESCRIPTION : Reads the string configuration details from the
//               CONFIG_DETAILS table.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::ReadCfgStr (const TEXT * key, TEXT * Value,
      INT32 minStrLen, INT32 maxStrLen)
{
   static BOOLEAN lsCmdSet = false;
   static SACommand lsCmd;
   int lRecordCnt = 0;

   try
   {
      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "SELECT VALUE "
               "  FROM IVR_CONFIG_MAIN "
               " WHERE CONFIG_KEY = :key "
               "   AND (LENGTH(VALUE) >= :minval "
               "   AND LENGTH(VALUE) <= :maxval)");

         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
      }

      lsCmd.Param ("key").setAsString () = key;
      lsCmd.Param ("minval").setAsLong () = minStrLen;
      lsCmd.Param ("maxval").setAsLong () = maxStrLen;
      lsCmd.Execute ();

      while(true == lsCmd.FetchNext ())
      {
         lRecordCnt++;
         strcpy (Value, lsCmd.Field ("VALUE").asString ());
      }

      if(lRecordCnt > 0)
         return DB_NO_ERR;
      else
         return DB_NO_RECORD_FOUND_ERR;
   }

   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "ReadCfgStr()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : ReadCfgStr
// DESCRIPTION : Reads the string configuration details from the
//               CONFIG_DETAILS table.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::UpdateDedicationRsp(DedicationRsp& lDedicationRsp)
{
   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : PollScheduleResponse
// DESCRIPTION : Reads the string configuration details from the
//               CONFIG_DETAILS table.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::PollScheduleResponse(DedicationRsp& lDedicationRsp)
{
   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : ReadCfgStr
// DESCRIPTION : Reads the string configuration details from the
//               CONFIG_DETAILS table.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
//DbErrorCodes MmpDbManager::ScheduleDedications(UINT32& lDId)
DbErrorCodes MmpDbManager::ScheduleDedications(DedicationReq& lDedicationReq, TEXT *dedicationAppl)
{
   static BOOLEAN   lsCmdSet = false;
   static SACommand lsCmd;
   TEXT   lExtRefNum[21] = "";

   try
   {
      if(!lsCmdSet)
      {
         TEXT        lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "INSERT INTO IVR_SCHEDULE (SCHEDULE_TYPE,CALL_REF_NUM,REQUEST_MODE,RECIPIENT_NO,"
               "SENDER_NO,SHORT_CODE,ENTRY_TIME,SCHEDULE_TIME,DEL_ATTEMPTS,STATUS,END_TIME,EXT_REF_NUM)"
               " VALUES(0,:ref,:mode,:dest,:orig,:appl,SYSDATE(),SYSDATE(),0,:stat,:endtime,:extref)");

         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
      }
      sprintf(lExtRefNum, "%d", lDedicationReq.dedicateId);
      //lsCmd.Param ("type").setAsShort()       = DEDICATION;             
      lsCmd.Param ("ref").setAsString()       = lDedicationReq.callRefNum;
      lsCmd.Param ("extref").setAsString()    = lExtRefNum;
      lsCmd.Param ("orig").setAsString()      = lDedicationReq.origAddr;
      lsCmd.Param ("dest").setAsString()      = lDedicationReq.destAddr; 
      lsCmd.Param ("appl").setAsString()      = dedicationAppl;       
      lsCmd.Param ("mode").setAsShort()       = lDedicationReq.reqMode;             
      lsCmd.Param ("stat").setAsShort()       = IVR_SCHEDULED;           
      lsCmd.Param ("endtime").setAsDateTime() = lDedicationReq.validity_time;    

      lsCmd.Execute ();

      INT16 lRowsEffected = lsCmd.RowsAffected();
      if(lRowsEffected > 0)
         return DB_NO_ERR;
      else
         return DB_UPDATE_FAILURE_ERR;
   }
   catch(SAException& e)
   {
      lsCmdSet = false;
      return ErrorHandler (e, (TEXT*)"ScheduleDedications()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : FetchDedications 
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::FetchDedications(DedicationReq& lDedicationReq, INT32& lMaxDelAttmpts,
                                            TEXT* lSysStartTime, TEXT* lSysEndTime)
{
   static BOOLEAN   lsCmdSet = false;
   static BOOLEAN   lsUpCmdSet = false;
   static SACommand lsCmd;
   static SACommand lsUpCmd;
   INT16  lFetchCount=0;
   struct tm lPET;
   struct tm lPST;
   struct tm lSysET;

   DbErrorCodes lDbErr = this->CheckDBStatus ();

   if (DB_NO_ERR != lDbErr)
      return lDbErr;

   try
   {
      if(!lsCmdSet)
      {
         TEXT        lSqlStr[MMP_MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);
         /*snprintf (lSqlStr, MAX_SQL_STR_LEN,
           "SELECT a.DEDICATION_ID, a.CALL_REF_NUM, a.REQUEST_MODE,a.RECIPIENT_NO, a.SENDER_NO,"
           "a.RECORD_ID, a.CONTENT_ID, a.STATUS, a.CHARGED,a.DEL_ATTEMPTS,b.PREF_END_TIME FROM "
           " MMP_DEDICATION_DETAILS a, MMP_SUBSCRIBER b WHERE a.DEL_TIME <= SYSDATE() AND a.DEL_ATTEMPTS < :attmpt "
           " AND a.STATUS != :stat AND a.DEL_TIME BETWEEN b.PREF_START_TIME AND b.PREF_END_TIME "
           " AND a.RECIPIENT_NO = b.SUBS_NO"); 
          */

         snprintf (lSqlStr, MMP_MAX_SQL_STR_LEN,
                     "SELECT DEDICATION_ID, CALL_REF_NUM, REQUEST_MODE, RECIPIENT_NO,"
                     "SENDER_NO, RECORD_ID, CONTENT_ID, a.STATUS, CHARGED, DEL_ATTEMPTS,"
                     "DEL_TIME, CAST(CONCAT(CAST(current_date() AS CHAR),' ',"
                     "cast(b.PREF_START_TIME AS CHAR)) AS DATETIME) AS PST,"
                     "CAST(CONCAT(CAST(current_date() AS CHAR),' ',cast(b.PREF_END_TIME AS CHAR))"
                     " AS DATETIME) AS PET,"
                     "CAST(CONCAT(CAST(current_date() AS CHAR),' ',:spet) AS DATETIME) AS SYS_ET"
                     " FROM MMP_DEDICATION_DETAILS AS a LEFT JOIN MMP_SUBSCRIBER AS b ON "
                     "a.RECIPIENT_NO = b.SUBS_NO WHERE NEXT_DEL_TIME <= SYSDATE() AND DEL_ATTEMPTS <= :attmpt"
                     " AND a.STATUS IN (:stat,:stat2) "
                     "AND ((CURTIME() BETWEEN b.PREF_START_TIME AND b.PREF_END_TIME) OR"
                     " ((CURTIME() BETWEEN (cast(:spst as TIME)) AND (cast(:spet as TIME))) "
                     " AND((b.PREF_START_TIME IS NULL) OR (b.PREF_END_TIME IS NULL)))) LIMIT 1");

         lsCmd.setCommandText (lSqlStr);

         //T(gTrace, printf("%s: FetchDedications SQL: %s\n", gProcessName, lSqlStr););
         lsCmdSet = true;
      }
      lsCmd.Param ("attmpt").setAsShort () = lMaxDelAttmpts;
      lsCmd.Param ("stat").setAsShort ()   = MMP_ACCEPTED;
      lsCmd.Param ("stat2").setAsShort ()  = MMP_PENDING;
      lsCmd.Param ("spet").setAsString ()  = lSysEndTime;
      lsCmd.Param ("spst").setAsString ()  = lSysStartTime;
      lsCmd.Execute ();

      if(true == lsCmd.FetchNext ())
      {
         lFetchCount++;
         strcpy(lDedicationReq.callRefNum, lsCmd.Field ("CALL_REF_NUM").asString());
         strcpy(lDedicationReq.origAddr  , lsCmd.Field ("SENDER_NO").asString());
         strcpy(lDedicationReq.destAddr  , lsCmd.Field ("RECIPIENT_NO").asString());
         lDedicationReq.dedicateId       = lsCmd.Field ("DEDICATION_ID").asLong();
         lDedicationReq.reqMode          = (EnumReqMode)lsCmd.Field ("REQUEST_MODE").asShort();
         lDedicationReq.recordId         = lsCmd.Field ("RECORD_ID").asLong();
         lDedicationReq.contentId        = lsCmd.Field ("CONTENT_ID").asLong();
         lDedicationReq.attempts         = lsCmd.Field ("DEL_ATTEMPTS").asLong();
         lDedicationReq.charged          = lsCmd.Field ("CHARGED").asShort();
         lDedicationReq.status           = (EnumDedicationStatus)lsCmd.Field ("STATUS").asShort();
         //lDedicationReq.validity_time    = lsCmd.Field("PREF_END_TIME").asDateTime();
         lPET    = lsCmd.Field("PET").asDateTime();
         lPST    = lsCmd.Field("PST").asDateTime();
         lSysET  = lsCmd.Field("SYS_ET").asDateTime();

         if(lPET.tm_year == 0)
             lDedicationReq.validity_time = lSysET;
         else
             lDedicationReq.validity_time = lPET;

         while (true == lsCmd.FetchNext ());
         if(!lsUpCmdSet)
         {
            TEXT        lSqlStr[MAX_SQL_STR_LEN + 1] = "";

            lsUpCmd.setConnection (&mConnection);
            snprintf (lSqlStr, MAX_SQL_STR_LEN,"UPDATE MMP_DEDICATION_DETAILS SET STATUS=:status, "
                  "DEL_ATTEMPTS=DEL_ATTEMPTS+1 WHERE DEDICATION_ID =:DedicateId");

            lsUpCmd.setCommandText (lSqlStr);

            lsUpCmdSet = true;
         }
         lsUpCmd.Param ("DedicateId").setAsShort () = lDedicationReq.dedicateId;
         lsUpCmd.Param ("status").setAsShort ()       = MMP_IN_TRANSIT;
         lsUpCmd.Execute ();

         INT16 lRowsEffected = lsUpCmd.RowsAffected();
         if(lRowsEffected > 0) 
            return DB_NO_ERR;
         //else
         //continue;

      }

      if(lFetchCount > 0)
         return DB_NO_ERR;
      else
      {
         return DB_NO_RECORD_FOUND_ERR;
      }

   }
   catch(SAException& e)
   {
      lsCmdSet = false;
      lsUpCmdSet = false;
      return ErrorHandler (e,(TEXT*) "FetchDedications()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : UpdateBillingRsp 
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::UpdateBillingRsp(UINT32& lId)
{
   static BOOLEAN   lsUpdateCmdSet = false;
   static SACommand lsUpdateCmd;

   try
   {
      if(!lsUpdateCmdSet)
      {
         TEXT        lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsUpdateCmd.setConnection (&mConnection);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "UPDATE MMP_DEDICATION_DETAILS SET CHARGED=1 WHERE DEDICATION_ID=:DId");

         lsUpdateCmd.setCommandText (lSqlStr);
         lsUpdateCmdSet = true;
      }
      lsUpdateCmd.Param ("DId").setAsShort()       = lId;             
      lsUpdateCmd.Execute ();

      return DB_NO_ERR;
      /*INT16 lRowsEffected = lsUpdateCmd.RowsAffected();
      if(lRowsEffected > 0)
         return DB_NO_ERR;
      else
         return DB_UPDATE_FAILURE_ERR;*/
   }
   catch(SAException& e)
   {
      lsUpdateCmdSet = false;
      return ErrorHandler (e, (TEXT*)"ScheduleDedications()",(TEXT*)"");
   }
   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : FetchNotifications 
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::FetchNotifications(NotiReq& lNotiReq)
{
   static BOOLEAN   lsCmdSet = false;
   static BOOLEAN   lsUpCmdSet = false;
   static BOOLEAN   lsCmdExeQuerySet = false;
   static SACommand lsCmd;
   static SACommand lsUpCmd;
   INT16  lFetchCount=0;
   INT16  lFetchSize = 20;

   try
   {
      if(!lsCmdExeQuerySet)
      {
         if(!lsCmdSet)
         {
            TEXT        lSqlStr[MAX_SQL_STR_LEN + 1] = "";
            SAString    sBulkSize;

            lsCmd.setConnection (&mConnection);
            snprintf (lSqlStr, MAX_SQL_STR_LEN,"SELECT a.NOTI_ID, a.NOTI_EVENT, a.CALL_REF_NUM, a.SUB_AGENT, "
                  "a.USER_ID, a.TEXT, a.RECIPIENT_NO, a.SENDER_NO, b.TEXT AS TEMPLATE FROM MMP_NOTIFICATION a, "
                  " MMP_NOTIFY_EVENTS b WHERE DEL_AGENT = :agent AND DEL_TIME <= SYSDATE() AND STATUS = :stat");

            lsCmd.setCommandText (lSqlStr);
            sBulkSize.Format("%d", lFetchSize);
            lsCmd.setOption("PreFetchRows") = sBulkSize;

            lsCmdSet = true;
         }
         lsCmd.Param ("agent").setAsShort () = MMP_NOTI_SMS;
         lsCmd.Param ("stat").setAsShort ()  = MMP_NOTI_NEW;
         lsCmd.Execute ();
         lsCmdExeQuerySet = true;
      }

      if (true == lsCmd.FetchNext ())
      {
         lFetchCount++;
         strcpy(lNotiReq.callRefNum  , lsCmd.Field ("CALL_REF_NUM").asString());
         strcpy(lNotiReq.userId      , lsCmd.Field ("USER_ID").asString());
         strcpy(lNotiReq.msgData     , lsCmd.Field ("TEXT").asString());
         strcpy(lNotiReq.msg_Template, lsCmd.Field ("TEMPLATE").asString());
         strcpy(lNotiReq.origAddr    , lsCmd.Field ("SENDER_NO").asString());
         strcpy(lNotiReq.destAddr    , lsCmd.Field ("RECIPIENT_NO").asString());
         lNotiReq.notiId               = lsCmd.Field ("NOTI_ID").asLong();
         lNotiReq.notiEvent            = (EnumNotiEvent)lsCmd.Field ("NOTI_EVENT").asShort();
         lNotiReq.subAgent             = (EnumModuleId)lsCmd.Field ("SUB_AGENT").asLong();

         if(!lsUpCmdSet)
         {
            TEXT        lSqlStr[MAX_SQL_STR_LEN + 1] = "";

            lsUpCmd.setConnection (&mConnection);
            snprintf (lSqlStr, MAX_SQL_STR_LEN,"UPDATE NOTIFICATION SET STATUS=:status "
                  "WHERE NOTI_ID =:Id");

            lsUpCmd.setCommandText (lSqlStr);

            lsUpCmdSet = true;
         }
         lsUpCmd.Param ("Id").setAsShort ()      = lNotiReq.notiId;
         lsUpCmd.Param ("status").setAsShort ()  = MMP_NOTI_IN_TRANSIT;
         lsUpCmd.Execute ();

         INT16 lRowsEffected = lsUpCmd.RowsAffected();
         if(lRowsEffected > 0) 
            return DB_NO_ERR;
         //else
         //continue;

      }
      else
         lsCmdExeQuerySet = false;

      if(lFetchCount > 0)
         return DB_NO_ERR;
      else
      {
         lsCmdExeQuerySet = false;
         return DB_NO_RECORD_FOUND_ERR;
      }

   }
   catch(SAException& e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "FetchNotifications()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : UpdateNotiStatus 
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes  MmpDbManager::UpdateNotiStatus(EnumNotiStatus lStatus, UINT32& lNotiId)
{
   static BOOLEAN   lsUpdateCmdSet = false;
   static SACommand lsUpdateCmd;

   try
   {
      if(!lsUpdateCmdSet)
      {
         TEXT        lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsUpdateCmd.setConnection (&mConnection);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "UPDATE NOTIFICATION SET STATUS=:status WHERE NOTI_ID=:Id");

         lsUpdateCmd.setCommandText (lSqlStr);
         lsUpdateCmdSet = true;
      }
      lsUpdateCmd.Param ("Id").setAsLong()       = lNotiId;             
      lsUpdateCmd.Param ("status").setAsShort()   = (EnumNotiStatus)lStatus;             
      lsUpdateCmd.Execute ();

      INT16 lRowsEffected = lsUpdateCmd.RowsAffected();
      if(lRowsEffected > 0)
         return DB_NO_ERR;
      else
         return DB_UPDATE_FAILURE_ERR;
   }
   catch(SAException& e)
   {
      lsUpdateCmdSet = false;
      return ErrorHandler (e, (TEXT*)"UpdateNotiStatus()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : GetAllInterfaceDetails
// DESCRIPTION : Get All Interface Details
// PARAMETER   : GgiInterfaceDetails &
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
#if 0
DbErrorCodes MmpDbManager::GetAllInterfaceDetails (GgiInterfaceDetails & interfaceDetails)
{
   static BOOLEAN lsCmdSet = false;
   static SACommand lsCmd;

   DbErrorCodes lDbErr = this->CheckDBStatus ();

   if (DB_NO_ERR != lDbErr)
      return lDbErr;

   try
   {
      if (true != lsCmdSet)
      {
         TEXT lSqlStr[MMP_MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);

         snprintf (lSqlStr, MMP_MAX_SQL_STR_LEN,
               "SELECT a.interface_id, a.status, "
               "       b.response, b.conf_sms "
               "  FROM interface_mast a, wsm_interface_details b "
               " WHERE a.interface_id = b.interface_id");
         lsCmd.setCommandText (lSqlStr);

         lsCmd.Execute ();

         lsCmdSet = true;
      }

      if (true != lsCmd.FetchNext ())
      {
         lsCmdSet = false;
         return DB_NO_RECORD_FOUND_ERR;
      }
      interfaceDetails.interfaceId =
         (EnumGgiInterface) lsCmd.Field ("interface_id").asUShort ();
      interfaceDetails.status = lsCmd.Field ("status").asShort ();
      interfaceDetails.rspReq = lsCmd.Field ("response").asShort ();
      interfaceDetails.smsReq = lsCmd.Field ("conf_sms").asShort ();

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      lsCmdSet = false;
      return this->ErrorHandler (e, "GetAllInterfaceDetails");
   }
}
#endif

//----------------------------------------------------------------------
// METHOD      : InsertInterfaceResponse
// DESCRIPTION : Insert Interface Response
// PARAMETER   : GgiInterfaceRsp
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::InsertInterfaceResponse (GgiInterfaceRsp interfaceRspMsg)
{
   static BOOLEAN lsCmdFlag = false;
   static SACommand lsCmd;

   DbErrorCodes lDbErr = this->CheckDBStatus ();
   if (DB_NO_ERR != lDbErr)
      return lDbErr;

   try
   {
      if (true != lsCmdFlag)
      {
         TEXT lSqlStr[MMP_MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);

         snprintf (lSqlStr, MMP_MAX_SQL_STR_LEN,
               "INSERT INTO GGI.GGI_RESPONSE_MESSAGE "
               "       (RECORD_ID, KEYWORD, ORIG_MSISDN, STATUS) "
               "VALUES (:recordid, :key, :msisdn, :state) ");

         lsCmd.setCommandText (lSqlStr);
         lsCmdFlag = true;
      }

      lsCmd.Param ("recordid").setAsLong () = interfaceRspMsg.recordId;
      lsCmd.Param ("key").setAsString () = interfaceRspMsg.keyword;
      lsCmd.Param ("msisdn").setAsString () = interfaceRspMsg.msisdn;
      lsCmd.Param ("state").setAsShort () = interfaceRspMsg.reqStatus;

      lsCmd.Execute ();
      mConnection.Commit ();

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      lsCmdFlag = false;
      return this->ErrorHandler (e,(TEXT*) "InsertInterfaceResponse()", (TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpGetSubsImsi()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpGetSubsImsi (TEXT * msisdn, TEXT * imsi)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;
   int lRecordCnt = 0;

   this->CheckDBStatus ();

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("SELECT IMSI "
               "FROM MMP_SUBSCRIBER "
               "WHERE SUBS_NO = :in_msisdn and ROWNUM = 1");

         lsCmdSet = true;
      }

      //-------------- Set Input Parameters -------------------------
      lsCmd.Param ("in_msisdn").setAsString () = msisdn;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      while( true == lsCmd.FetchNext())
      {
         lRecordCnt++;   
         strcpy(imsi, lsCmd.Field("IMSI").asString ());
      }

      if(lRecordCnt > 0 )
         return DB_NO_ERR;
      else
         return DB_NO_RECORD_FOUND_ERR;

   }
   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpGetSubsImsi()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : GetMmpSubsDetails
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::GetMmpSubsDetails (TEXT * msisdn, MmpSubsInfo & subsInfo)
{
   DbErrorCodes lDbErr;
   static BOOLEAN lsCmdSet = false;
   static SACommand lsCmd;
   int lRecordCnt = 0;

   try
   {
      lDbErr = this->CheckDBStatus ();

      if (DB_NO_ERR != lDbErr)
      {
         return lDbErr;
      }

      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MMP_MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);

         snprintf (lSqlStr, MMP_MAX_SQL_STR_LEN,
               " SELECT a.SUBS_NO, a.SUBS_TYPE, a.STATUS"
               " FROM MMP_SUBSCRIBER a, MMP_PACKAGE_SUBSCRIPTION b"
               " WHERE a.SUBS_NO = b.SUBS_NO AND a.SUBS_NO = :msisdn"
               " AND a.STATUS = 0 ");
         lsCmd.setCommandText (lSqlStr);

         lsCmdSet = true;
      }

      lsCmd.Param ("msisdn").setAsString () = msisdn;

      lsCmd.Execute ();

      while(true == lsCmd.FetchNext ())
      {
         lRecordCnt++;
         strcpy(subsInfo.msisdn, lsCmd.Field ("subs_no").asString ());
         subsInfo.subsType = (EnumBillingSubsType)lsCmd.Field ("subs_type").asShort ();
         subsInfo.status = (EnumMmpSubsStatus) lsCmd.Field ("status").asShort ();

         T (gTrace,
               printf ("%s: Msisdn:%s SubsType:%d Status:%d\n",
                  gProcessName, subsInfo.msisdn, subsInfo.subsType, 
                  subsInfo.status););
      }

      if(lRecordCnt > 0)
         return DB_NO_ERR;
      else
         return DB_NO_RECORD_FOUND_ERR;
   }

   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "GetMmpSubsDetails ()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : GetProvKeyDetails
// DESCRIPTION : Get Key Details from DB
// PARAMETER   : TEXT *, TEXT *
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::GetProvKeyDetails (BINARY & provKey, 
      MmpProvKey & rProvKeyDetails)
{
   static BOOLEAN lsCmdFlag = false;
   static SACommand lsCmd;
   UINT32 lRecordCOunt=0;

   this->CheckDBStatus ();

   try
   {
      if (lsCmdFlag == false)
      {
         TEXT lSqlStr[MMP_MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);

         sprintf (lSqlStr,
               "SELECT PACKAGE_ID "
               "  FROM MMP_PKG_KEY_DETAILS WHERE PKG_KEY = :provkey");
         lsCmd.setCommandText (lSqlStr);

         lsCmdFlag = true;
      }

      lsCmd.Param("provkey").setParamType(SA_dtBytes) ;

      memcpy(lsCmd.Param ("provkey").setAsBytes ().GetBinaryBuffer(provKey.length),
            &provKey.data, provKey.length);

      lsCmd.Param("provkey").setAsBytes().ReleaseBinaryBuffer(provKey.length);

      MmpUtil::PrintUnicode(provKey.data, provKey.length, sizeof(provKey.data));

      lsCmd.Execute ();

      while (true == lsCmd.FetchNext ())
      {
         lRecordCOunt++;
         rProvKeyDetails.argType = MMP_ARG_PKG;
         rProvKeyDetails.argId = lsCmd.Field ("PACKAGE_ID").asLong ();

         T (gTrace, printf("%s: KeyType:%d KeyId:%d\n", gProcessName,
                  rProvKeyDetails.argType, rProvKeyDetails.argId);
               MmpUtil::PrintHex (provKey.data, provKey.length););
      }

      if(lRecordCOunt > 0)      
      {
         return DB_NO_ERR;
      }
      else
      {
         lsCmdFlag = false;

         T (gTrace, printf("%s: No Record Found For Provision Key\n", gProcessName););

         return DB_NO_RECORD_FOUND_ERR;
      }
   }

   catch (SAException & e)
   {
      lsCmdFlag = false;
      return ErrorHandler (e,(TEXT*) "GetProvKeyDetails ()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : GetContentInfo
// DESCRIPTION : Get Content Details from DB
// PARAMETER   : TEXT *, TEXT *
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::GetContentInfo (TEXT * lData, UINT32 & contentId)
{
   static BOOLEAN lsCmdFlag = false;
   static SACommand lsCmd;
   UINT32 lRecordCount = 0, lCnt = 0;

   try
   {
      if (lsCmdFlag == false)
      {
         TEXT lSqlStr[MMP_MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);

         sprintf (lSqlStr,
               "SELECT count(*) as lCnt "
               "  FROM MMP_CONTENT_MAST WHERE CONTENT_ID = :lData");
         lsCmd.setCommandText (lSqlStr);

         lsCmdFlag = true;
      }

      contentId = atoi(lData);
      lsCmd.Param ("lData").setAsLong () = contentId;

      lsCmd.Execute ();

      while (true == lsCmd.FetchNext ())
      {
         lRecordCount++;
         lCnt = lsCmd.Field ("lCnt").asLong ();
      }

      if(lCnt > 0)      
         return DB_NO_ERR;
      else
      {
         lsCmdFlag = false;
         T (gTrace, printf("%s: No Record Found For Requested Content\n", 
                  gProcessName););

         return DB_NO_RECORD_FOUND_ERR;
      }
   }

   catch (SAException & e)
   {
      lsCmdFlag = false;
      return ErrorHandler (e,(TEXT*) "GetContentInfo ()",(TEXT*)"");
   }
}

//------------------------------------------------------------------------
// METHOD      : DeleteHlrProvCmdDetails
// DESCRIPTION :
// PARAMETER   : Msisdn
// RETURN      : DbError ( Success/Failure )
//------------------------------------------------------------------------
DbErrorCodes MmpDbManager::DeletePreviousProfile (TEXT * msisdn)
{
   static BOOLEAN lsCmdFlag = false;
   static SACommand lsCmd;

   this->CheckDBStatus ();

   try
   {
      if (false == lsCmdFlag)
      {
         TEXT lSqlStr[MMP_MAX_SQL_STR_LEN + 1] = "";
         lsCmd.setConnection (&mConnection);

         memset (lSqlStr, 0, sizeof (lSqlStr));
         strcpy (lSqlStr, "DELETE FROM MMP_SUBSCRIBER WHERE subs_no= :msisdn and status = 1");

         lsCmd.setCommandText (lSqlStr);
         lsCmdFlag = true;
      }

      lsCmd.Param ("msisdn").setAsString () = msisdn;

      lsCmd.Execute ();
      mConnection.Commit ();
   }

   catch (SAException & e)
   {
      mConnection.Rollback ();
      lsCmdFlag = false;
      return ErrorHandler (e,(TEXT*) "DeletePreviousProfile ()",(TEXT*)"");
   }

   return DB_NO_ERR;
}

//--------------------------------------------------------------------//
//---------------- Validation Specific -------------------------------//
//--------------------------------------------------------------------//
//----------------------------------------------------------------------
// METHOD      : MmpValidateRegPkg()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpValidateRegPkg (MmpSubsProvDetails & provDetails) 
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   T (gTrace, printf ("==MmpValidateRegPkg==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Msisdn-------------:%s\n", gProcessName, provDetails.subsInfo.msisdn);
         MMP_PRINT_SPACE;
         printf ("%s: LanguageId---------:%d\n", gProcessName, provDetails.subsInfo.languageId);
         MMP_PRINT_SPACE;
         printf ("%s: SubscriptionType---:%d[%s]\n", gProcessName, provDetails.subsInfo.subsType,
            MmpUtil::ConvertSubsTypeToStr(provDetails.subsInfo.subsType));
         MMP_PRINT_SPACE;
         printf ("%s: Package ID --------:%d\n", gProcessName, provDetails.pkgId);
         MMP_PRINT_SPACE;
         printf ("%s: RequestMode--------:%d\n", gProcessName, provDetails.subsInfo.reqMode););

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_val_reg");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_result").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_amount").setParamType (SA_dtNumeric); 
         lsCmd.Param ("o_daily_rent").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_subs_status").setParamType (SA_dtNumeric); 
         lsCmd.Param ("o_validitydate").setParamType (SA_dtDateTime);
         lsCmd.Param ("o_expirydate").setParamType (SA_dtDateTime);
         lsCmd.Param ("o_daily_start_time").setParamType (SA_dtDateTime);
         lsCmd.Param ("o_daily_end_time").setParamType (SA_dtDateTime);
         lsCmd.Param ("o_autorenew_flg").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_multiple_gifts").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_min_usg").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_cont_usg").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_ded_usg").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_subs_usg").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_pkg_type").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_pkg_name").setParamType (SA_dtBLob);
         lsCmd.Param ("o_pkg_length").setParamType (SA_dtNumeric);

         lsCmdSet = true;
      }

      //-------------- Set Input Parameters -------------------------
      lsCmd.Param ("i_msisdn").setAsString () = provDetails.subsInfo.msisdn;
      lsCmd.Param ("i_languageid").setAsLong () = provDetails.subsInfo.languageId;
      lsCmd.Param ("i_substype").setAsLong () = provDetails.subsInfo.subsType;
      lsCmd.Param ("io_pkgid").setAsLong () = provDetails.pkgId;
      lsCmd.Param ("i_intrfid").setAsLong () = provDetails.subsInfo.reqMode;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      MMP_PROV_VAL_REG_GET_DFT_OUT_PARAMS;
      MMP_GET_DATE_PARAMS;

      provDetails.subsInfo.pkgInfo.cost         = lsCmd.Param ("o_amount").asShort ();
      provDetails.subsInfo.pkgInfo.dailySubCost = lsCmd.Param ("o_daily_rent").asShort ();
      provDetails.subsInfo.pkgInfo.autoRenew = lsCmd.Param ("o_autorenew_flg").asShort ();
      provDetails.subsInfo.pkgInfo.multipleGifts = lsCmd.Param ("o_multiple_gifts").asShort ();
      provDetails.subsInfo.pkgInfo.minUsg = lsCmd.Param ("o_min_usg").asShort ();
      provDetails.subsInfo.pkgInfo.contentUsg = lsCmd.Param ("o_cont_usg").asShort ();
      provDetails.subsInfo.pkgInfo.dedicationUsg = lsCmd.Param ("o_ded_usg").asShort ();
      provDetails.subsInfo.pkgInfo.subscriptionUsg = lsCmd.Param ("o_subs_usg").asShort ();
      provDetails.subsInfo.pkgInfo.pkgType = (EnumMmpPkgType) lsCmd.Param ("o_pkg_type").asShort ();
      provDetails.subsInfo.status = (EnumMmpSubsStatus) lsCmd.Param ("o_subs_status").asShort ();

      //------------------------------------------------------------
      T (gTrace, printf ("%s: RegCost------------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.cost);
            MMP_PRINT_SPACE;
            printf ("%s: DailySubCost-------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.dailySubCost);
            MMP_PRINT_SPACE;
            printf ("%s: AutoRenewFlg-------:%d\n", gProcessName, 
               provDetails.subsInfo.pkgInfo.autoRenew);
            MMP_PRINT_SPACE;
            printf ("%s: MultipleGifts------:%d\n", gProcessName, 
               provDetails.subsInfo.pkgInfo.multipleGifts);
            MMP_PRINT_SPACE;
            printf ("%s: MinUsg-------------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.minUsg);
            MMP_PRINT_SPACE;
            printf ("%s: ContentUsg---------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.contentUsg);
            MMP_PRINT_SPACE;
            printf ("%s: DedicationUsg------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.dedicationUsg);
            MMP_PRINT_SPACE;
            printf ("%s: SubscriptionUsg----:%d\n", gProcessName,
                  provDetails.subsInfo.pkgInfo.subscriptionUsg);
            MMP_PRINT_SPACE;
            printf ("%s: PkgType------------:%d[%s]\n", gProcessName,
                  provDetails.subsInfo.pkgInfo.pkgType, 
                  MmpUtil::ConvertPkgTypeToStr(provDetails.subsInfo.pkgInfo.pkgType)););

            while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpValidateRegPkg()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpValidateDeReg()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpValidateDereg (MmpSubsProvDetails & provDetails)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   T (gTrace, printf ("==MmpValidateDereg==\n");
         MMP_PRINT_SPACE;
         printf ("%s: PackageId----------:%d\n", gProcessName, provDetails.pkgId);
         MMP_PRINT_SPACE;
         printf ("%s: Msisdn-------------:%s\n", gProcessName, provDetails.subsInfo.msisdn););

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_val_dereg");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_result").setParamType (SA_dtNumeric);
         lsCmd.Param ("io_pkgid").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_languageid").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_amount").setParamType (SA_dtString); 
         lsCmd.Param ("o_subs_status").setParamType (SA_dtNumeric); 
         lsCmd.Param ("o_subs_type").setParamType (SA_dtNumeric); 
         lsCmd.Param ("o_validitydate").setParamType (SA_dtDateTime);
         lsCmd.Param ("o_expirydate").setParamType (SA_dtDateTime);

         lsCmdSet = true;
      }

      //-------------- Set Input Parameters -------------------------
         lsCmd.Param ("io_pkgid").setAsLong () = provDetails.pkgId;
         lsCmd.Param ("i_msisdn").setAsString () = provDetails.subsInfo.msisdn;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asShort ();
      provDetails.pkgId = lsCmd.Param ("io_pkgid").asShort ();
      provDetails.subsInfo.languageId = lsCmd.Param ("o_languageid").asShort ();
      provDetails.subsInfo.pkgInfo.cost = lsCmd.Param ("o_amount").asShort ();
      provDetails.subsInfo.status = (EnumMmpSubsStatus) lsCmd.Param ("o_subs_status").asShort ();
      provDetails.subsInfo.subsType = (EnumBillingSubsType) 
         lsCmd.Param ("o_subs_type").asShort ();
      provDetails.subsInfo.validityDate  = lsCmd.Param ("o_validitydate").asDateTime ();
      provDetails.subsInfo.expiryDate = lsCmd.Param ("o_expirydate").asDateTime ();

      T(gTrace, printf ("=========================\n");
            MMP_PRINT_SPACE;
            printf ("%s: Result-------------:%d[\33[32m%s\33[0m]\n",
               gProcessName, provDetails.evrReason,
               IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));
            MMP_PRINT_SPACE;
            printf ("%s: MmpPkgId-----------:%d\n",
               gProcessName, provDetails.pkgId);
            MMP_PRINT_SPACE;
            printf ("%s: SubsStatus---------:%d[%s]\n", 
               gProcessName, provDetails.subsInfo.status, 
               MmpUtil::ConvertSubsStatusToStr(provDetails.subsInfo.status));
            MMP_PRINT_SPACE;
            printf ("%s: SubsType----------:%d[%s]\n", 
               gProcessName, provDetails.subsInfo.subsType,
               MmpUtil::ConvertSubsTypeToStr (provDetails.subsInfo.subsType));
            MMP_PRINT_SPACE;
            printf ("%s: ValidityDate-------:%02d/%02d/%04d %02d:%02d:%02d\n",
               gProcessName, provDetails.subsInfo.validityDate.tm_mday,
               provDetails.subsInfo.validityDate.tm_mon + 1,
               provDetails.subsInfo.validityDate.tm_year + 1900,
               provDetails.subsInfo.validityDate.tm_hour,
               provDetails.subsInfo.validityDate.tm_min,
               provDetails.subsInfo.validityDate.tm_sec);
            MMP_PRINT_SPACE;
            printf ("%s: ExpiryDate---------:%02d/%02d/%04d %02d:%02d:%02d\n",
                  gProcessName, provDetails.subsInfo.expiryDate.tm_mday,
                  provDetails.subsInfo.expiryDate.tm_mon + 1,
                  provDetails.subsInfo.expiryDate.tm_year + 1900,
                  provDetails.subsInfo.expiryDate.tm_hour,
                  provDetails.subsInfo.expiryDate.tm_min,
                  provDetails.subsInfo.expiryDate.tm_sec);
            MMP_PRINT_SPACE;
            printf ("%s: DeregFlg-----------:%d\n", gProcessName, 
                  provDetails.subsInfo.deregflag););

            //------------------------------------------------------------

            while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e, (TEXT*)"MmpValidateDereg()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpValidateRenew()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpValidateRenew (MmpSubsProvDetails & provDetails)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   T (gTrace, printf ("==MmpValidateRenew==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Msisdn-------------:%s\n", gProcessName, provDetails.subsInfo.msisdn);
     );

   try
   {
      lsCmd.setConnection (&mConnection);
      lsCmd.setCommandText ("mmp_val_renew");

      lsCmd.Param ("o_validitydate").setParamType (SA_dtDateTime);
      lsCmd.Param ("o_expirydate").setParamType (SA_dtDateTime);
      lsCmd.Param ("o_nextbilldate").setParamType (SA_dtDateTime);

      //-------------- Set Input Parameters -------------------------
      lsCmd.Param ("i_msisdn").setAsString () = provDetails.subsInfo.msisdn;
      lsCmd.Param ("io_duration").setAsShort () = provDetails.duration;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      MMP_PROV_VAL_REG_GET_DFT_OUT_PARAMS;

      provDetails.subsInfo.startDate =
         lsCmd.Param ("o_validitydate").asDateTime ();
      provDetails.subsInfo.expiryDate =
         lsCmd.Param ("o_expirydate").asDateTime ();

      while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) " MmpValidateRenew ()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpValidateDedicate()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpValidateDedicate (MmpSubsProvDetails & provDetails, UINT32 & lContentId)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   T (gTrace, printf ("==MmpValidateDedicate==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Msisdn-------------:%s\n", gProcessName, provDetails.subsInfo.msisdn);
         MMP_PRINT_SPACE;
         printf ("%s: DedicateMsisdn-----:%s\n", gProcessName, provDetails.subsInfo.giftNum);
         MMP_PRINT_SPACE;
         printf ("%s: ContentId----------:%d\n", gProcessName, lContentId););

   try
   {
      lsCmd.setConnection (&mConnection);
      lsCmd.setCommandText ("mmp_val_ded");

      //-------------- Set Output Parameters ------------------------
      lsCmd.Param ("o_result").setParamType (SA_dtLong);

      //-------------- Set Input Parameters -------------------------
      lsCmd.Param ("i_msisdn").setAsString () = provDetails.subsInfo.msisdn;
      lsCmd.Param ("i_dedicate_msisdn").setAsString () = provDetails.subsInfo.giftNum;
      lsCmd.Param ("i_content_id").setAsLong () = lContentId;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asShort ();
      provDetails.amount = lsCmd.Param ("o_amount").asShort ();
      provDetails.subsInfo.pkgInfo.pkgId = lContentId;

      T(gTrace, printf ("=========================\n");
            MMP_PRINT_SPACE;
            printf ("%s: Result-------------:%d[\33[32m%s\33[0m]\n",
               gProcessName, provDetails.evrReason,
               IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));
            MMP_PRINT_SPACE;
            printf ("%s: Amount------------:%d\n",
               gProcessName, provDetails.amount););

      while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) " MmpValidateDedicate ()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpValidateGift()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpValidateGift (MmpSubsProvDetails & provDetails)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   T (gTrace, printf ("==MmpValidateGift==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Msisdn-------------:%s\n", gProcessName, provDetails.subsInfo.msisdn);
         MMP_PRINT_SPACE;
         printf ("%s: GiftMsisdn---------:%s\n", gProcessName, provDetails.subsInfo.giftNum);
         MMP_PRINT_SPACE
         printf ("%s: SubsType-----------:%d\n",gProcessName, provDetails.subsInfo.subsType);
         MMP_PRINT_SPACE
         printf ("%s: PackageId----------:%d\n",gProcessName, provDetails.pkgId);
     );

   try
   {
      lsCmd.setConnection (&mConnection);
      lsCmd.setCommandText ("mmp_val_gift");

      //-------------- Set Input Parameters -------------------------
      lsCmd.Param ("i_pkgid").setAsLong () = provDetails.pkgId;
      lsCmd.Param ("i_orig_msisdn").setAsString () = provDetails.subsInfo.msisdn;
      lsCmd.Param ("i_gift_msisdn").setAsString () = provDetails.subsInfo.giftNum;
      lsCmd.Param ("i_subs_type").setAsShort () = provDetails.subsInfo.subsType;

      //-------------- Set Output Parameters -------------------------
      lsCmd.Param ("o_amount").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_result").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_daily_start_time").setParamType (SA_dtDateTime);
      lsCmd.Param ("o_daily_end_time").setParamType (SA_dtDateTime);
      lsCmd.Param ("o_validitydate").setParamType (SA_dtDateTime);
      lsCmd.Param ("o_expirydate").setParamType (SA_dtDateTime);
      lsCmd.Param ("o_autorenew_flg").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_min_usg").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_cont_usg").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_ded_usg").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_subs_usg").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_pkg_type").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_subs_status").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_pkg_name").setParamType (SA_dtBLob);
      lsCmd.Param ("o_pkg_length").setParamType (SA_dtNumeric);
      lsCmd.Param ("o_validity").setParamType (SA_dtNumeric);

      lsCmd.Execute ();
      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asShort ();
      provDetails.amount    = lsCmd.Param ("o_amount").asShort ();
      provDetails.subsInfo.pkgInfo.autoRenew = lsCmd.Param ("o_autorenew_flg").asShort ();
      provDetails.subsInfo.status = (EnumMmpSubsStatus) lsCmd.Param ("o_subs_status").asShort ();
      provDetails.subsInfo.pkgInfo.minUsg = lsCmd.Param ("o_min_usg").asShort ();
      provDetails.subsInfo.pkgInfo.contentUsg = lsCmd.Param ("o_cont_usg").asShort ();
      provDetails.subsInfo.pkgInfo.dedicationUsg = lsCmd.Param ("o_ded_usg").asShort ();
      provDetails.subsInfo.pkgInfo.subscriptionUsg = lsCmd.Param ("o_subs_usg").asShort ();
      provDetails.subsInfo.pkgInfo.pkgType = (EnumMmpPkgType) lsCmd.Param ("o_pkg_type").asShort ();
      provDetails.subsInfo.pkgInfo.pkgName.length = (UINT8)lsCmd.Param ("o_pkg_length").asShort ();
      memcpy(&provDetails.subsInfo.pkgInfo.pkgName.data, lsCmd.Param ("o_pkg_name").asBLob (), 
            provDetails.subsInfo.pkgInfo.pkgName.length);
      provDetails.subsInfo.pkgInfo.validity = lsCmd.Param ("o_validity").asShort ();      

      MMP_GET_DATE_PARAMS;
      T(gTrace, printf ("=========================\n");
            MMP_PRINT_SPACE;
            printf ("%s: Result-------------:%d[\33[32m%s\33[0m]\n",
               gProcessName, provDetails.evrReason,
               IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));
            MMP_PRINT_SPACE;
            printf ("%s: Amount-------------:%d\n",
               gProcessName, provDetails.amount);
            MMP_PRINT_SPACE;
            printf ("%s: AutoRenewFlg-------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.autoRenew);
            MMP_PRINT_SPACE;
            printf ("%s: MultipleGifts------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.multipleGifts);
            MMP_PRINT_SPACE;
            printf ("%s: MinUsg-------------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.minUsg);
            MMP_PRINT_SPACE;
            printf ("%s: ContentUsg---------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.contentUsg);
            MMP_PRINT_SPACE;
            printf ("%s: DedicationUsg------:%d\n", gProcessName,
               provDetails.subsInfo.pkgInfo.dedicationUsg);
            MMP_PRINT_SPACE;
            printf ("%s: SubscriptionUsg----:%d\n", gProcessName,
                  provDetails.subsInfo.pkgInfo.subscriptionUsg);
            MMP_PRINT_SPACE;
            printf ("%s: PkgType------------:%d[%s]\n", gProcessName,
                  provDetails.subsInfo.pkgInfo.pkgType,
                  MmpUtil::ConvertPkgTypeToStr(provDetails.subsInfo.pkgInfo.pkgType));
            MMP_PRINT_SPACE;
            printf ("%s: PkgName-------------:%s[%d]\n",
                  gProcessName, provDetails.subsInfo.pkgInfo.pkgName.data,
                  provDetails.subsInfo.pkgInfo.pkgName.length););

            while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e, (TEXT*)" MmpValidateGift ()",(TEXT*)"");
   }
}

//--------------------------------------------------------------------//
//---------------- PROVISIONING SPECIFIC -----------------------------//
//--------------------------------------------------------------------//
//----------------------------------------------------------------------
// METHOD      : MmpReg()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpReg (MmpSubsProvDetails & provDetails)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   try 
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_reg_pkg");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_result").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_subsid").setParamType (SA_dtNumeric);

         lsCmdSet = true;
      }

      //-------------- Set Input Parameters ----------------------------
      lsCmd.Param ("i_msisdn").setAsString () = provDetails.subsInfo.msisdn;
      lsCmd.Param ("i_imsi").setAsString () = provDetails.subsInfo.imsi;
      lsCmd.Param ("i_substype").setAsShort () = provDetails.subsInfo.subsType;
      lsCmd.Param ("i_pkgid").setAsShort () = provDetails.pkgId;
      lsCmd.Param ("i_interfaceid").setAsShort () = provDetails.subsInfo.reqMode;
      lsCmd.Param ("i_validitydate").setAsDateTime () = provDetails.subsInfo.validityDate;
      lsCmd.Param ("i_expirydate").setAsDateTime () = provDetails.subsInfo.expiryDate;
      lsCmd.Param ("i_amount").setAsLong () = provDetails.amount;
      lsCmd.Param ("i_languageid").setAsShort () = provDetails.subsInfo.languageId;
      lsCmd.Param ("i_subs_status").setAsLong () = provDetails.subsInfo.status;
      lsCmd.Param ("i_pkg_type").setAsShort () = provDetails.subsInfo.pkgInfo.pkgType;
      lsCmd.Param ("i_dly_start_time").setAsDateTime () = provDetails.subsInfo.dailyStartTime;
      lsCmd.Param ("i_dly_end_time").setAsDateTime () = provDetails.subsInfo.dailyEndTime;
      lsCmd.Param ("i_min_usg").setAsShort () = provDetails.subsInfo.pkgInfo.minUsg;
      lsCmd.Param ("i_cont_usg").setAsShort () = provDetails.subsInfo.pkgInfo.contentUsg;
      lsCmd.Param ("i_ded_usg").setAsShort () = provDetails.subsInfo.pkgInfo.dedicationUsg;
      lsCmd.Param ("i_subs_usg").setAsShort () = provDetails.subsInfo.pkgInfo.subscriptionUsg;
      lsCmd.Param ("i_auto_rnw_flg").setAsShort () = provDetails.subsInfo.pkgInfo.autoRenew;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asShort ();
      provDetails.subsInfo.subsId = lsCmd.Param ("o_subsid").asLong ();

      T (gTrace, printf ("=========================\n");
            MMP_PRINT_SPACE;
            printf ("%s: EvrReason -------------:%d[\33[32m%s\33[0m]\n", 
               gProcessName, provDetails.evrReason, 
               IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));
            MMP_PRINT_SPACE;  
            printf ("%s: SubscriptionId --------:%d\n", 
               gProcessName, provDetails.subsInfo.subsId);); 

      while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpRegPkg()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpDeReg()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpDereg (MmpSubsProvDetails & provDetails)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_dereg_pkg");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_result").setParamType (SA_dtNumeric);
         lsCmdSet = true;
      }

      //-------------- Set Input Parameters ----------------------------
      lsCmd.Param ("i_msisdn").setAsString ()  = provDetails.subsInfo.msisdn;
      lsCmd.Param ("i_dereg_flg").setAsShort() = provDetails.subsInfo.deregflag;
      lsCmd.Param ("i_pkgid").setAsLong() = provDetails.pkgId;
      lsCmd.Param ("i_expirydate").setAsDateTime() = provDetails.subsInfo.expiryDate;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asShort ();

      T (gTrace, printf ("=========================\n");
            MMP_PRINT_SPACE;
            printf ("%s: EvrReason -------------:%d[\33[32m%s\33[0m]\n",
               gProcessName, provDetails.evrReason, 
               IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));); 

      while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e, (TEXT*)"MmpDereg()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpDed()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpDedicate (MmpSubsProvDetails & provDetails) 
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   T (gTrace, printf ("==MmpDedicate==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Msisdn-------------:%s\n", gProcessName, provDetails.subsInfo.msisdn);
         MMP_PRINT_SPACE;
         printf ("%s: DedicateMsisdn-----:%s\n", gProcessName, provDetails.subsInfo.giftNum);
         MMP_PRINT_SPACE;
         printf ("%s: ContentId----------:%d\n", gProcessName, provDetails.subsInfo.pkgInfo.pkgId);
         MMP_PRINT_SPACE;
         printf ("%s: InterfaceId--------:%d\n", gProcessName, provDetails.subsInfo.reqMode);
         MMP_PRINT_SPACE;
         printf ("%s: Amount-------------:%d\n", gProcessName, provDetails.amount););

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_ded_pkg");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_result").setParamType (SA_dtLong);
         lsCmdSet = true;
      }

      //-------------- Set Input Parameters ----------------------------
      lsCmd.Param ("i_msisdn").setAsString () = provDetails.subsInfo.msisdn;
      lsCmd.Param ("i_dedicate_msisdn").setAsString () = provDetails.subsInfo.giftNum;
      lsCmd.Param ("i_content_id").setAsLong () = provDetails.subsInfo.pkgInfo.pkgId;
      lsCmd.Param ("i_intrfid").setAsLong () = provDetails.subsInfo.reqMode;
      lsCmd.Param ("i_amount").setAsLong () = provDetails.amount;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asLong ();

      T (gTrace, printf ("=========================\n");
            MMP_PRINT_SPACE;
            printf ("%s: EvrReason--------------:%d[\33[32m%s\33[0m]\n",
               gProcessName, provDetails.evrReason,
               IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));); 

      while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e, (TEXT*)"MmpDedicate()", (TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpGift()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpGift (MmpSubsProvDetails & provDetails)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_gift_pkg");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_result").setParamType (SA_dtNumeric);
         lsCmdSet = true;
      }

      //-------------- Set Input Parameters ----------------------------
      lsCmd.Param ("i_msisdn").setAsString () = provDetails.subsInfo.giftNum;
      lsCmd.Param ("i_pkgid").setAsLong () = provDetails.pkgId;
      lsCmd.Param ("i_intrfid").setAsLong () = provDetails.subsInfo.reqMode;
      lsCmd.Param ("i_gift_num").setAsString () = provDetails.subsInfo.msisdn;
      lsCmd.Param ("i_subs_status").setAsShort () = provDetails.subsInfo.status;
      lsCmd.Param ("i_validitydate").setAsDateTime () = provDetails.subsInfo.validityDate;
      lsCmd.Param ("i_expirydate").setAsDateTime () = provDetails.subsInfo.expiryDate;
      lsCmd.Param ("i_dly_start_time").setAsDateTime () = provDetails.subsInfo.dailyStartTime;
      lsCmd.Param ("i_dly_end_time").setAsDateTime () = provDetails.subsInfo.dailyEndTime;
      lsCmd.Param ("i_min_usg").setAsShort () = provDetails.subsInfo.pkgInfo.minUsg;
      lsCmd.Param ("i_cont_usg").setAsShort () = provDetails.subsInfo.pkgInfo.contentUsg;
      lsCmd.Param ("i_ded_usg").setAsShort () = provDetails.subsInfo.pkgInfo.dedicationUsg;
      lsCmd.Param ("i_subs_usg").setAsShort () = provDetails.subsInfo.pkgInfo.subscriptionUsg;
      lsCmd.Param ("i_pkg_type").setAsShort () = provDetails.subsInfo.pkgInfo.pkgType;
      lsCmd.Param ("i_auto_rnw_flg").setAsShort () = provDetails.subsInfo.pkgInfo.autoRenew;
      lsCmd.Param ("i_substype").setAsShort () = provDetails.subsInfo.giftSubsType;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asShort ();

      T (gTrace, printf ("=========================\n");
            MMP_PRINT_SPACE;
            printf ("%s: GiftMsisdn-------------:%s\n",
               gProcessName, provDetails.subsInfo.giftNum);
            MMP_PRINT_SPACE;
            printf ("%s: EvrReason -------------:%d[%s]\n",
               gProcessName, provDetails.evrReason,
               IvrUtil::ConvertEvrReasonToStr(provDetails.evrReason));); 

      while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpGift()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpRenew()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpRenew (MmpSubsProvDetails & provDetails)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   this->CheckDBStatus ();

   try
   {

      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_renew");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_result").setParamType (SA_dtLong);

         lsCmdSet = true;
      }

      lsCmd.Param ("i_expirydate").setAsDateTime () =
         provDetails.subsInfo.expiryDate;
      lsCmd.Param ("i_validitydate").setAsDateTime () =
         provDetails.subsInfo.startDate;
      lsCmd.Param ("i_amount").setAsLong () =
         provDetails.amount;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      provDetails.evrReason = lsCmd.Param ("o_result").asLong ();

      while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      if (1403 == e.ErrNativeCode ())
         return DB_NO_RECORD_FOUND_ERR;

      lsCmdSet = false;
      return ErrorHandler (e, (TEXT*) "MmpRenew ()", (TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : GetSubsPackage()
// DESCRIPTION :
// PARAMETER   : TEXT* subsNo, EnumMmpPkgType , SubsPackageInfo &
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::GetSubsPackage(TEXT* subsNo, EnumMmpPkgType packageType, 
      MmpSubsInfo &subsInfo, UINT32 usage)
{
   static SACommand lsCmd;
   TEXT   lSqlStr[MAX_SQL_STR_LEN + 1] = "";
   UINT32 lCount = 0;

   try
   {
      lsCmd.setConnection (&mConnection);

      switch(packageType)
      {
         case MMP_PACKAGE_MINUTES:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 1"
                  "       AND AVAILABLE_MIN_USAGE > %d"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ", TABLE_MMP_PACKAGE_SUBSCRIPTION, usage);

         case MMP_PACKAGE_CONTENTS:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 2"
                  "       AND AVAILABLE_CONTENT_USAGE > %d"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ", TABLE_MMP_PACKAGE_SUBSCRIPTION, usage);

         case MMP_PACKAGE_DEDICATION:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 4"
                  "       AND AVAILABLE_DEDICATION_USAGE > %d"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ", TABLE_MMP_PACKAGE_SUBSCRIPTION, usage);

         case MMP_PACKAGE_SUBSCRIPTION:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 8"
                  "       AND AVAILABLE_SUBSCRIPTION = %d"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ", TABLE_MMP_PACKAGE_SUBSCRIPTION, usage);

         default:
            return DB_NO_RECORD_FOUND_ERR;
      }
      lsCmd.setCommandText (lSqlStr);

      lsCmd.Execute ();

      while (true == lsCmd.FetchNext ())
      {
         if(0==lCount)
         {
            subsInfo.pkgInfo.pkgId   = lsCmd.Field ("PACKAGE_ID").asLong();
            subsInfo.pkgInfo.pkgType = (EnumMmpPkgType) lsCmd.Field ("PACKAGE_TYPE").asShort();
            subsInfo.availMin        = lsCmd.Field ("AVAILABLE_MIN_USAGE").asLong();
            subsInfo.availContent    = lsCmd.Field ("AVAILABLE_CONTENT_USAGE").asLong();
            subsInfo.availDedication = lsCmd.Field ("AVAILABLE_DEDICATION_USAGE").asLong();
            subsInfo.availSubscription = lsCmd.Field ("AVAILABLE_SUBSCRIPTION").asLong();
            subsInfo.status = (EnumMmpSubsStatus) lsCmd.Field ("STATUS").asShort();
            strcpy(subsInfo.msisdn, lsCmd.Field ("SUBS_NO").asString());
            subsInfo.startDate     = lsCmd.Field ("START_DATE").asDateTime();            
            subsInfo.validityDate  = lsCmd.Field ("VALIDITY_DATE").asDateTime();            
            subsInfo.expiryDate    = lsCmd.Field ("EXPIRY_DATE").asDateTime();
            subsInfo.dailyStartTime  = lsCmd.Field ("DAILY_START_TIME").asDateTime();
            subsInfo.dailyEndTime    = lsCmd.Field ("DAILY_END_TIME").asDateTime();
         }
         lCount++;
      }

      if(lCount > 0)
         return DB_NO_ERR;
      else
         return DB_NO_RECORD_FOUND_ERR;

   }
   catch (SAException & e)
   {
      return ErrorHandler (e,(TEXT*) "GetSubsPackage()",(TEXT*)"");
   }

}

//----------------------------------------------------------------------
// METHOD      : GetSubsPackage()
// DESCRIPTION :
// PARAMETER   : TEXT* subsNo, EnumMmpPkgType , SubsPackageInfo &
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::GetSubsPackage(TEXT* subsNo, EnumMmpPkgType packageType, 
      SubsPackageInfo &packageInfo, UINT32 usage)
{
   static SACommand lsCmd;
   TEXT   lSqlStr[MAX_SQL_STR_LEN + 1] = "";
   UINT32 lCount = 0;

   try
   {
      lsCmd.setConnection (&mConnection);

      //T(gTrace, printf("%s: GetSubsPackage PKGT:%d, SUBNO:%s\n", gProcessName, packageType, subsNo););
      switch(packageType)
      {
         case MMP_PACKAGE_MINUTES:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 1"
                  "       AND AVAILABLE_MIN_USAGE > %d "
                  "       AND SUBS_NO = '%s'"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ", TABLE_MMP_PACKAGE_SUBSCRIPTION, usage, subsNo);
            break;

         case MMP_PACKAGE_CONTENTS:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 2"
                  "       AND AVAILABLE_CONTENT_USAGE > %d"
                  "       AND SUBS_NO = '%s'"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ", TABLE_MMP_PACKAGE_SUBSCRIPTION, usage, subsNo);
            break;

         case MMP_PACKAGE_DEDICATION:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 4"
                  "       AND AVAILABLE_DEDICATION_USAGE > %d"
                  "       AND SUBS_NO = '%s'"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ", TABLE_MMP_PACKAGE_SUBSCRIPTION, usage, subsNo);
            break;

         case MMP_PACKAGE_SUBSCRIPTION:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SUBSCRIPTION_ID,"
                  "       SUBS_NO,"
                  "       PACKAGE_ID,"
                  "       PACKAGE_TYPE,"
                  "       AVAILABLE_MIN_USAGE,"
                  "       AVAILABLE_CONTENT_USAGE,"
                  "       AVAILABLE_DEDICATION_USAGE,"
                  "       AVAILABLE_SUBSCRIPTION,"
                  "       START_DATE,"
                  "       VALIDITY_DATE,"
                  "       EXPIRY_DATE,"
                  "       DAILY_START_TIME,"
                  "       DAILY_END_TIME,"
                  "       STATUS"
                  "  FROM %s"
                  " WHERE     START_DATE <= sysdate()"
                  "       AND EXPIRY_DATE >= sysdate()"
                  "       AND DAILY_START_TIME <= time(sysdate())"
                  "       AND DAILY_END_TIME >= time(sysdate()) "
                  "       AND PACKAGE_TYPE & 8"
                  "       AND AVAILABLE_SUBSCRIPTION = %d"
                  "       AND SUBS_NO = '%s'"
                  " ORDER BY EXPIRY_DATE, DAILY_END_TIME ",(TEXT*) TABLE_MMP_PACKAGE_SUBSCRIPTION, usage,(TEXT*) subsNo);
            break;

         default:
            return DB_NO_RECORD_FOUND_ERR;
      }
      lsCmd.setCommandText (lSqlStr);

      //T(gTrace, printf("%s: GetSubsPackage SQL: %s\n", gProcessName, lSqlStr););
      lsCmd.Execute ();

      if(true == lsCmd.FetchNext ())
      {
         if(0==lCount)
         {
            packageInfo.subscriptionId  = lsCmd.Field ("SUBSCRIPTION_ID").asLong();
            packageInfo.packageId       = lsCmd.Field ("PACKAGE_ID").asLong();
            packageInfo.packageType     = (EnumMmpPkgType) lsCmd.Field ("PACKAGE_TYPE").asShort();
            packageInfo.min             = lsCmd.Field ("AVAILABLE_MIN_USAGE").asLong();
            packageInfo.content         = lsCmd.Field ("AVAILABLE_CONTENT_USAGE").asLong();
            packageInfo.dedication      = lsCmd.Field ("AVAILABLE_DEDICATION_USAGE").asLong();
            packageInfo.subscription    = lsCmd.Field ("AVAILABLE_SUBSCRIPTION").asLong();
            packageInfo.status = (EnumMmpSubsStatus) lsCmd.Field ("STATUS").asShort();
            packageInfo.startDate       = lsCmd.Field ("START_DATE").asDateTime();            
            packageInfo.endDate         = lsCmd.Field ("EXPIRY_DATE").asDateTime();
            packageInfo.startTime       = lsCmd.Field ("DAILY_START_TIME").asDateTime();
            packageInfo.endTime         = lsCmd.Field ("DAILY_END_TIME").asDateTime();
         }
         lCount++;
      }
      else
         return DB_NO_RECORD_FOUND_ERR;

      while (true == lsCmd.FetchNext ());

      return DB_NO_ERR;

   }
   catch (SAException & e)
   {
      return ErrorHandler (e,(TEXT*) "GetSubsPackage()",(TEXT*)"");
   }

}

//----------------------------------------------------------------------
// METHOD      : MmpUpdateValidityDate()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpUpdateValidityDate (UINT32 lSubsId, INT16 lNumOfDays, INT16 lKeyword)

{
   static SACommand lsCmd;
   tm     lValidityDate, lExpiryDate;
   INT16  lGracePeriod = 0;
   TEXT   lSqlStr[MAX_SQL_STR_LEN + 1] = "";
   static BOOLEAN lsCmdSet = false;
   int lRecordCnt = 0;

   T (gTrace, printf ("==MmpUpdateValidityDate==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Subs Id------------:%d\n", gProcessName, lSubsId);
         MMP_PRINT_SPACE;
         printf ("%s: Num of Days--------:%d\n", gProcessName, lNumOfDays);
         MMP_PRINT_SPACE;
         printf ("%s: Keyword------------:%d[%s]\n", gProcessName, lKeyword,
            MmpUtil::ConvertKeywordToStr(lKeyword)););

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         if(MMP_KEYWORD_REG == lKeyword)
         {
            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "SELECT a.GRACE_PERIOD "
                  "FROM MMP_PACKAGE_MAST A, MMP_PACKAGE_SUBSCRIPTION B "
                  "WHERE B.SUBSCRIPTION_ID = :i_subsid AND A.PACKAGE_ID = B.PACKAGE_ID ");

            lsCmd.setCommandText (lSqlStr);
            lsCmdSet = true;
            lsCmd.Param ("i_subsid").setAsLong () = lSubsId;

            lsCmd.Execute ();

            while (true == lsCmd.FetchNext ())
            {
               lRecordCnt++;
               lGracePeriod = lsCmd.Field ("grace_period").asShort ();

            }

            memset(&lSqlStr, '\0', sizeof(MAX_SQL_STR_LEN + 1));

            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "UPDATE MMP_PACKAGE_SUBSCRIPTION "
                  " SET VALIDITY_DATE = DATE_ADD(NOW(), INTERVAL :i_days DAY), "
                  " EXPIRY_DATE = DATE_ADD(DATE_ADD(NOW(), INTERVAL :i_days DAY), INTERVAL "
                  " :i_graceperiod DAY) "
                  " WHERE SUBSCRIPTION_ID = :i_subsid");

            lsCmd.setCommandText (lSqlStr);

            lsCmd.Param ("i_graceperiod").setAsShort () = lGracePeriod;

            T (gTrace, printf ("======================\n");
                  MMP_PRINT_SPACE;
                  printf ("%s: GracePeriod--------:%d\n", gProcessName, lGracePeriod););

            //}
      }
      else if(MMP_KEYWORD_DAILYBILL == lKeyword || MMP_KEYWORD_RENEW == lKeyword)
      {
         memset(&lSqlStr, '\0', sizeof(MAX_SQL_STR_LEN + 1));

         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "SELECT VALIDITY_DATE, EXPIRY_DATE "
               " FROM MMP_PACKAGE_SUBSCRIPTION "
               " WHERE SUBSCRIPTION_ID = :i_subsid");

         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
         lsCmd.Param ("i_subsid").setAsLong () = lSubsId;

         lsCmd.Execute ();

         while (true == lsCmd.FetchNext ())
         {
            lRecordCnt++;
            lValidityDate = lsCmd.Field ("validity_date").asDateTime ();
            lExpiryDate = lsCmd.Field ("expiry_date").asDateTime ();
         }
         memset(&lSqlStr, '\0', sizeof(MAX_SQL_STR_LEN + 1));

         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "UPDATE MMP_PACKAGE_SUBSCRIPTION"
               " SET VALIDITY_DATE = DATE_ADD(:i_validitydate, INTERVAL :i_days DAY),"
               " EXPIRY_DATE = DATE_ADD(:i_expirydate, INTERVAL :i_days DAY)"
               " WHERE SUBSCRIPTION_ID = :i_subsid");
         lsCmd.setCommandText (lSqlStr);

         lsCmd.Param ("i_validitydate").setAsDateTime () = lValidityDate;
         lsCmd.Param ("i_expirydate").setAsDateTime ()   = lExpiryDate;

         T (gTrace, printf ("======================\n");
               MMP_PRINT_SPACE;
               printf ("%s: ValidityDate-------:%d-%d-%d\n", gProcessName,
                  lValidityDate.tm_mday, lValidityDate.tm_mon + 1, lValidityDate.tm_year+1900);
               MMP_PRINT_SPACE;
               printf ("%s: ExpiryDate---------:%d-%d-%d\n", gProcessName,
                  lExpiryDate.tm_mday, lExpiryDate.tm_mon + 1, lExpiryDate.tm_year+1900););

         // }
   }
} 
lsCmd.Param ("i_subsid").setAsLong () = lSubsId;
lsCmd.Param ("i_days").setAsLong ()   = lNumOfDays;

lsCmd.Execute ();
mConnection.Commit ();

INT16 lRowsEffected = lsCmd.RowsAffected();

if(lRowsEffected > 0)
   return DB_NO_ERR;
   else 
   return DB_UPDATE_FAILURE_ERR;
   }

catch (SAException & e)
{
   lsCmdSet = false;
   return ErrorHandler (e,(TEXT*) "MmpUpdateValidityDate()",(TEXT*)"");
}
}

//----------------------------------------------------------------------
// METHOD      : MmpRevertValidityDate()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpRevertValidityDate (UINT32 lSubsId, INT16 lNumOfDays, INT16 lKeyword)
{
   static SACommand lsCmd;
   tm     lValidityDate, lExpiryDate;
   TEXT   lSqlStr[MAX_SQL_STR_LEN + 1] = "";
   static BOOLEAN lsCmdSet = false;
   int lRecordCnt = 0;

   T (gTrace, printf ("==IcsRevertValidity==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Subs Id------------:%d\n", gProcessName, lSubsId);
         MMP_PRINT_SPACE;
         printf ("%s: Num of Days--------:%d\n", gProcessName, lNumOfDays);
         MMP_PRINT_SPACE;
         printf ("%s: Keyword------------:%d\n", gProcessName, lKeyword););

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         if(MMP_KEYWORD_RENEW == lKeyword)
         {
            memset(&lSqlStr, '\0', sizeof(MAX_SQL_STR_LEN + 1));

            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "SELECT VALIDITY_DATE, EXPIRY_DATE "
                  "FROM MMP_PACKAGE_SUBSCRIPTION "
                  "WHERE SUBSCRIPTION_ID = :i_subsid");

            lsCmd.setCommandText (lSqlStr);
            lsCmdSet = true;
            lsCmd.Param ("i_subsid").setAsLong () = lSubsId;

            lsCmd.Execute ();

            while (true == lsCmd.FetchNext ())
            {
               lRecordCnt++;
               lValidityDate = lsCmd.Field ("validity_date").asDateTime ();
               lExpiryDate = lsCmd.Field ("expiry_date").asDateTime ();

               T (gTrace, printf ("======================\n");
                     MMP_PRINT_SPACE;
                     printf ("%s: ValidityDate-------:%d-%d-%d\n", gProcessName,
                        lValidityDate.tm_mday, lValidityDate.tm_mon + 1, lValidityDate.tm_year+1900);
                     MMP_PRINT_SPACE;
                     printf ("%s: ExpiryDate---------:%d-%d-%d\n", gProcessName,
                        lExpiryDate.tm_mday, lExpiryDate.tm_mon + 1, lExpiryDate.tm_year+1900););
            }

            memset(&lSqlStr, '\0', sizeof(MAX_SQL_STR_LEN + 1));

            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "UPDATE MMP_PACKAGE_SUBSCRIPTION "
                  " SET VALIDITY_DATE = DATE_SUB(:i_validitydate, INTERVAL :i_days DAY), "
                  " EXPIRY_DATE = DATE_SUB(:i_expirydate, INTERVAL :i_days DAY)"
                  " WHERE SUBSCRIPTION_ID = :i_subsid");
            lsCmd.setCommandText (lSqlStr);
            lsCmdSet = true;

            lsCmd.Param ("i_validitydate").setAsDateTime () = lValidityDate;
            lsCmd.Param ("i_expirydate").setAsDateTime ()   = lExpiryDate;
         }
         lsCmdSet = false;
      }

      lsCmd.Param ("i_subsid").setAsLong () = lSubsId;
      lsCmd.Param ("i_days").setAsLong ()   = lNumOfDays;

      lsCmd.Execute ();
      mConnection.Commit ();

      if(lRecordCnt > 0)
         return DB_NO_ERR;
      else
         return DB_UPDATE_FAILURE_ERR;
   }

   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpRevertValidityDate()", (TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpGetDailyDetails
// DESCRIPTION :
// PARAMETER   : lHmOpCd
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpGetDailyDetails(TEXT * lMsisdn, MmpValidityReq & lValidityReq)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   try
   {
      T (gTrace, printf ("=============== MmpGetDailyDetails ================\n");
            MMP_PRINT_SPACE;
            printf ("%s: Msisdn-------------:%s\n", gProcessName, lMsisdn););

      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         lsCmd.setCommandText ("mmp_get_daily_details");

         //-------------- Set Output Parameter type --------------------
         lsCmd.Param ("o_subs_type").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_pkgid").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_pkg_name").setParamType (SA_dtBLob);
         lsCmd.Param ("o_pkg_length").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_subsid").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_validity_date").setParamType (SA_dtDateTime);
         lsCmd.Param ("o_expiry_date").setParamType (SA_dtDateTime);
         lsCmd.Param ("o_backlog").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_rent").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_daily_renew_rent").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_renew_amt").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_renew_interval").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_grace_period").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_sms_flg").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_expired").setParamType (SA_dtNumeric);
         lsCmd.Param ("o_languageid").setParamType (SA_dtNumeric);

         lsCmdSet = true;
      }

      //-------------- Set Input Parameters -------------------------
      lsCmd.Param ("i_msisdn").setAsString () = lMsisdn;
      lsCmd.Execute ();

      //-------------- Get Output Parameters -------------------------
      while(lsCmd.FetchNext());

      MMP_GET_VALIDITY_MGR_PARAMS;
      
      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpGetDailyDetails ()",(TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpUpdateDailyBillFlg()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpUpdateDailyBillFlg(UINT32 lSubsId, INT16 & lDailyBillFlg)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;

   T (gTrace, printf ("==MmpUpdateDailyBillFlg==\n");
         MMP_PRINT_SPACE;
         printf ("%s: SubsId-------------:%d\n", gProcessName, lSubsId);
         MMP_PRINT_SPACE;
         printf ("%s: DailyBillFlg-------:%d\n", gProcessName, lDailyBillFlg););

   try
   {
      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "UPDATE MMP_PACKAGE_SUBSCRIPTION "
               "SET DAILY_SMS_FLAG = :i_flg "
               "WHERE SUBSCRIPTION_ID = :i_subsid");
         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
      }

      lsCmd.Param ("i_subsid").setAsLong () = lSubsId;
      lsCmd.Param ("i_flg").setAsShort ()    = lDailyBillFlg;
      lsCmd.Execute ();
      mConnection.Commit ();

      INT16 lRowsEffected = lsCmd.RowsAffected();

      if(lRowsEffected > 0)
         return DB_NO_ERR;
      else
         return DB_UPDATE_FAILURE_ERR;
   }

   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpUpdateDailyBillFlg()", (TEXT*)"");
   }
}

//----------------------------------------------------------------------
// METHOD      : MmpChargeFromPackage()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::ChargeFromPackage(UINT32 subscriptionId, EnumMmpPkgType packageType, UINT32 usage, SubsPackageInfo &packageInfo)
{
   static SACommand lsCmd;
   TEXT   lSqlStr[MAX_SQL_STR_LEN + 1] = "";

   try
   {
      lsCmd.setConnection (&mConnection);

      //T(gTrace, printf("%s: GetSubsPackage PKGT:%d, SUBNO:%s\n", gProcessName, packageType, subsNo););
      switch(packageType)
      {
         case MMP_PACKAGE_MINUTES:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "UPDATE %s SET "
                  "       AVAILABLE_MIN_USAGE = (AVAILABLE_MIN_USAGE - :usage)"
                  " WHERE SUBSCRIPTION_ID = :subscriptionId", TABLE_MMP_PACKAGE_SUBSCRIPTION);
            break;

         case MMP_PACKAGE_CONTENTS:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "UPDATE %s SET "
                  "       AVAILABLE_CONTENT_USAGE = (AVAILABLE_CONTENT_USAGE - :usage)"
                  " WHERE SUBSCRIPTION_ID = :subscriptionId", TABLE_MMP_PACKAGE_SUBSCRIPTION);
            break;

         case MMP_PACKAGE_DEDICATION:
            snprintf (lSqlStr, MAX_SQL_STR_LEN, "UPDATE %s SET "
                  "       AVAILABLE_DEDICATION_USAGE = (AVAILABLE_DEDICATION_USAGE - :usage)"
                  " WHERE SUBSCRIPTION_ID = :subscriptionId", TABLE_MMP_PACKAGE_SUBSCRIPTION);
            break;

         case MMP_PACKAGE_SUBSCRIPTION:
            //snprintf (lSqlStr, MAX_SQL_STR_LEN, "UPDATE %s SET "
            //      "       AVAILABLE_DEDICATION_USAGE = (AVAILABLE_DEDICATION_USAGE - :usage)"
            //      " WHERE SUBSCRIPTION_ID = :subscriptionId", TABLE_MMP_PACKAGE_SUBSCRIPTION);
            return DB_NO_ERR;
            break;

         default:
            return DB_NO_RECORD_FOUND_ERR;
      }
      lsCmd.setCommandText (lSqlStr);

      lsCmd.Param ("usage").setAsLong () = usage;
      lsCmd.Param ("subscriptionId").setAsLong () = subscriptionId;

      lsCmd.Execute ();

      while (true == lsCmd.FetchNext ());

      return DB_NO_ERR;

   }
   catch (SAException & e)
   {
      return ErrorHandler (e, (TEXT*) "ChargeFromPackage()",(TEXT*)"");
   }

}


//----------------------------------------------------------------------
// METHOD      : MmpStatusUpdate()
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::MmpSubsStatusUpdate(TEXT * lMsisdn)
{
   static SACommand lsCmd;
   static BOOLEAN lsCmdSet = false;
   TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

   T (gTrace, printf ("==MmpSubsStatusUpdate==\n");
         MMP_PRINT_SPACE;
         printf ("%s: Msisdn-------------:%s\n", gProcessName, lMsisdn););

   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConnection);
         snprintf (lSqlStr,
                  MAX_SQL_STR_LEN,
                  "UPDATE MMP_SUBSCRIBER SET STATUS = 2 WHERE SUBS_NO = :msisdn");
         lsCmdSet = true;
      }
      
      lsCmd.setCommandText (lSqlStr);
      lsCmd.Param ("msisdn").setAsString () = lMsisdn;

      lsCmd.Execute ();

      INT16 lRowsEffected = lsCmd.RowsAffected();
      if(lRowsEffected > 0)
         return DB_NO_ERR;
      else
         return DB_UPDATE_FAILURE_ERR;

   }
   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "MmpSubsStatusUpdate()",(TEXT*)"");
   }

}

//----------------------------------------------------------------------
// METHOD      : UpdateDedicationStatus 
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes MmpDbManager::UpdateDedicationStatus(EnumDedicationStatus lStatus, UINT32& lId)
{
   static BOOLEAN   lsUpdateCmdSet = false;
   static SACommand lsUpdateCmd;

   try
   {
      if(!lsUpdateCmdSet)
      {
         TEXT        lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsUpdateCmd.setConnection (&mConnection);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "UPDATE MMP_DEDICATION_DETAILS SET STATUS=:st WHERE DEDICATION_ID=:DId");

         lsUpdateCmd.setCommandText (lSqlStr);
         lsUpdateCmdSet = true;
      }
      lsUpdateCmd.Param ("DId").setAsShort() = lId;             
      lsUpdateCmd.Param ("st").setAsShort()  = lStatus;             
      lsUpdateCmd.Execute ();

      INT16 lRowsEffected = lsUpdateCmd.RowsAffected();
      if(lRowsEffected > 0)
         return DB_NO_ERR;
      else
         return DB_UPDATE_FAILURE_ERR;
   }
   catch(SAException& e)
   {
      lsUpdateCmdSet = false;
      return ErrorHandler (e, (TEXT*) "UpdateDedicationStatus()", (TEXT*) "");
   }
   return DB_NO_ERR;
}


