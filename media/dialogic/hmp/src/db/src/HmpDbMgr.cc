static const char *id = "@(#) $Id: HmpDbMgr.cc,v 1.1.2.1 2012/02/21 08:31:32 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
//       HmpDbMgr.cc
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
#include "HmpDbMgr.h"

//----------------------------------------------------------------------
// METHOD      : HmpDbMgr
// DESCRIPTION : Constructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
HmpDbMgr::HmpDbMgr ()
{

}

//----------------------------------------------------------------------
// METHOD      : HmpDbMgr
// DESCRIPTION : Destructor
// PARAMETER   : NONE
// RETURN      : NONE
//----------------------------------------------------------------------
HmpDbMgr::~HmpDbMgr ()
{
   this->DeInit ();
}

//----------------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION : Initializes the HmpDbMgr object.
//               Establishes Connection to database.
// PARAMETER   : None
// RETURN      : DbErrorCodes
//----------------------------------------------------------------------
DbErrorCodes HmpDbMgr::Init (TEXT * DbUserName, TEXT * DbUserPass, TEXT * DbName)
{
   TEXT *lpTempPtr;

   memset (mDbName, 0, sizeof (mDbName));
   memset (mDbUser, 0, sizeof (mDbUser));
   memset (mDbPass, 0, sizeof (mDbPass));

   lpTempPtr = getenv ("DB_NAME");

   if ('\0' != DbName[0])
   {
      strncpy (mDbName, DbName, HMP_MAX_DB_NAME_LEN);
   }
   else
   {
      if (NULLPTR == lpTempPtr)
         strncpy (mDbName, HMP_DB_NAME, HMP_MAX_DB_NAME_LEN);
      else
         strncpy (mDbName, lpTempPtr, HMP_MAX_DB_NAME_LEN);
   }

   if ('\0' != DbUserName[0])
   {
      strncpy (mDbUser, DbUserName, HMP_MAX_DB_USER_LEN);
   }
   else
   {
      lpTempPtr = getenv ("DB_USER");
      if (NULLPTR == lpTempPtr)
         strncpy (mDbUser, HMP_DB_USER, HMP_MAX_DB_USER_LEN);
      else
         strncpy (mDbUser, lpTempPtr, HMP_MAX_DB_USER_LEN);
   }

   if ('\0' != DbUserPass[0])
   {
      strncpy (mDbPass, DbUserPass, HMP_MAX_DB_PASS_LEN);
   }
   else
   {
      lpTempPtr = getenv ("DB_PASS");
      if (NULLPTR == lpTempPtr)
         strncpy (mDbPass, HMP_DB_PASS, HMP_MAX_DB_PASS_LEN);
      else
         strncpy (mDbPass, lpTempPtr, HMP_MAX_DB_PASS_LEN);
   }

   T (gTrace, printf ("%s: DbName:%s DbUser:%s\n",
            gProcessName, mDbName, mDbUser););

   try
   {
      mConnection.Connect (mDbName, mDbUser, mDbPass, DB_CLIENT_HMP);
      mConnection.setAutoCommit (SA_AutoCommitOn);
      mCommand.setConnection (&mConnection);

      gLog.GenerateLog (DBP08);
      T (gTrace, printf ("%s: DbName:%s DbUser:%s Connected to Db\n",
               gProcessName, mDbName, mDbUser););

      return this->CheckDbStatus ();
   }
   catch (SAException & e)
   {
      return this->ErrorHandler (e, "Init()");
   }
}

//----------------------------------------------------------------------
// METHOD      : DeInit
// DESCRIPTION : DeInitializes the HmpDbMgr object.
//               Disconnect database connection
// PARAMETER   : None
// RETURN      : DbErrorCodes
//----------------------------------------------------------------------
DbErrorCodes HmpDbMgr::DeInit ()
{
   if (mConnection.isConnected ())
   {
      mConnection.Disconnect ();
      gLog.GenerateLog (DBP09);
   }

   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : CheckDbStatus
// DESCRIPTION : Check Db Status
// PARAMETER   : no arguments
// RETURN      : DbErrorCodes
//----------------------------------------------------------------------
DbErrorCodes HmpDbMgr::CheckDbStatus ()
{
   try
   {
      if (mConnection.isAlive ())
         return DB_NO_ERR;
      else if (mConnection.isConnected ())
         mConnection.Disconnect ();

      mConnection.Connect (mDbName, mDbUser, mDbPass, DB_CLIENT_HMP);
      mConnection.setAutoCommit (SA_AutoCommitOn);
      mCommand.setConnection (&mConnection);
      gLog.GenerateLog (DBP08);
   }
   catch (SAException & e)
   {
      T (gTrace, printf ("%s: Exception while checking Db Status\n",
               gProcessName););

      return this->ErrorHandler (e, "CheckDbStatus()");
   }

   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : ErrorHandler
// DESCRIPTION : This function  sets global errno depending upon error
//                retrived from database and generates appropriate log
// PARAMETER   : Reference to SAException ,
//               Function Name ,
//               Debug Information
// RETURN      : DbErrorCodes
//----------------------------------------------------------------------
DbErrorCodes HmpDbMgr::ErrorHandler (SAException & err, TEXT * funcName,
      TEXT * debugInfo)
{
   TEXT lErrMsg[MAX_LOG_TEXT_LEN + 1] = "";
   DbErrorCodes lDbError = DB_NO_ERR;

   snprintf (lErrMsg, MAX_LOG_TEXT_LEN, "DBM: Func:%s Arg:%s Err(%d):%s ",
         funcName, debugInfo, err.ErrNativeCode (),
         (const char *) err.ErrText ());

   TERR (gTrace, printf ("%s: DBM: Func:%s Arg:%s Err(%d):%s\n",
            gProcessName, funcName, debugInfo, err.ErrNativeCode (),
            (const char *) err.ErrText ()););

   switch (err.ErrNativeCode ())
   {
      case 1403:
         gLog.GenerateLog (DBP03, lErrMsg);
         lDbError = DB_NO_RECORD_FOUND_ERR;
         break;

      case 1:
         gLog.GenerateLog (DBP02, lErrMsg);
         lDbError = DB_UNIQUE_KEY_VIOLATION_ERR;
         break;

      case 2291:
         gLog.GenerateLog (DBP02, lErrMsg);
         lDbError = DB_FORIGN_KEY_VIOLATION_ERR;
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
      case 0:
         gLog.GenerateLog (DBP00, lErrMsg);
         try
         {
            if (mConnection.isConnected ())
               mConnection.Disconnect ();

            mConnection.Connect (mDbName, mDbUser, mDbPass, DB_CLIENT_HMP);
            mConnection.setAutoCommit (SA_AutoCommitOn);
            mCommand.setConnection (&mConnection);
            T (gTrace, printf ("%s: DbName:%s DbUser:%s Connected to Db\n",
                     gProcessName, mDbName, mDbUser););
            gLog.GenerateLog (DBP08);
         }
         catch (SAException & e)
         {
            T (gTrace, printf ("%s: DbName:%s DbUser:%s Failed to Connect\n",
                     gProcessName, mDbName, mDbUser););
            usleep (100);
         }

         lDbError = DB_CONNECT_FAILURE_ERR;

         break;

      default:
         {
            gLog.GenerateLog (DBP00, lErrMsg);
            TERR (gTrace, printf ("%s: %s ErrNativeCode:%d\n",
                     gProcessName, lErrMsg, err.ErrNativeCode ()););
            lDbError = DB_UNKNOWN_ERR;

            if (false == mConnection.isConnected ())
            {
               try
               {
                  mConnection.Connect (mDbName, mDbUser, mDbPass, DB_CLIENT_HMP);
                  mConnection.setAutoCommit (SA_AutoCommitOn);
                  mCommand.setConnection (&mConnection);
                  T (gTrace, printf ("%s: DbName:%s DbUser:%s Connected to Db\n",
                           gProcessName, mDbName, mDbUser););
                  gLog.GenerateLog (DBP08);
               }
               catch (SAException & e)
               {
                  T (gTrace, printf ("%s: DbName:%s DbUser:%s Failed to Connect\n",
                           gProcessName, mDbName, mDbUser););
                  lDbError = DB_CONNECT_FAILURE_ERR;
                  usleep (100);
               }
            }
            break;
         }
   }

   return lDbError;
}
//----------------------------------------------------------------------
// METHOD      : InsertHmpProvEvr
// DESCRIPTION :
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes HmpDbMgr::InsertHmpProvEvr (const HmpEvrMsg & evrMsg)
{
   static BOOLEAN lsCmdFlag = false;
   static SACommand lsCmd;
   static int lPrevDay = 0;
   struct tm * lpTmStruct;
   TEXT lSqlStr[HMP_MAX_SQL_STR_LEN + 1] = "";

   lpTmStruct = localtime (&evrMsg.gentime);

   if (lPrevDay != lpTmStruct->tm_mday)
   {
      lPrevDay = lpTmStruct->tm_mday;
      lsCmdFlag = false;
   }

   try
   {
      if (false == lsCmdFlag)
      {
         lsCmd.setConnection (&mConnection);

         snprintf (lSqlStr, HMP_MAX_SQL_STR_LEN,
               "INSERT INTO %s_%02d%02d (evr_event, evr_cause, evr_reason, msisdn) VALUES (:event, :cause, :reason, :intrfce, :msisdn, ) ",
               "HMP_PROV_EVR", lpTmStruct->tm_mday, lpTmStruct->tm_mon + 1);
         lsCmd.setCommandText (lSqlStr);
         lsCmdFlag = true;
      }
      lsCmd.Param ("event").setAsShort () = evrMsg.evrEvent;
      lsCmd.Param ("cause").setAsShort () = evrMsg.evrCause;
      lsCmd.Param ("reason").setAsULong () = evrMsg.evrType;
      lsCmd.Param ("msisdn").setAsString () = evrMsg.msisdn;

      lsCmd.Execute ();

      T (gTrace, printf ("%s: ProvEvr Record Inserted For Msisdn:%s to %s_%02d%02d table\n",
               gProcessName, evrMsg.msisdn,
               "HMP_PROV_EVR", lpTmStruct->tm_mday, lpTmStruct->tm_mon + 1););

      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdFlag = false;
      return ErrorHandler (e, "InsertHmp1xEvr()");
   }
   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : ReadCfgNum
// DESCRIPTION : Reads the configuration details from the
//               CONFIG_DETAILS table.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes HmpDbMgr::ReadCfgNum (const TEXT * key, INT32 & cfgVal,
      INT32 minVal, INT32 maxVal)
{
   TEXT lCfgStr[MAX_LOG_TEXT_LEN + 1] = "";
   INT32 lCfgVal = 0;
   DbErrorCodes lDbErr;

   // Number can't go beyond 10 digits in a 32 bit machine
   lDbErr = this->ReadCfgStr (key, lCfgStr, 1, 10);
   if (DB_NO_ERR != lDbErr)
   {
      return lDbErr;
   }

   printf("TBD Db 1 -------- %d %d %d \n", lCfgVal, minVal, maxVal);
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
DbErrorCodes HmpDbMgr::ReadCfgStr (const TEXT * key, TEXT * Value,
      INT32 minStrLen, INT32 maxStrLen)
{
   static BOOLEAN lsCmdSet = false;
   static SACommand lsCmd;

   try
   {
      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConnection);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "SELECT VALUE "
               "  FROM CONFIG_MAIN "
               " WHERE CONFIG_KEY = :Key "
               "   AND (LENGTH(VALUE) >= :minval "
               "   AND LENGTH(VALUE) <= :maxval )");

         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;

      printf("TBD ---------- %s  %d %d %s\n", key, minStrLen, maxStrLen,
            lSqlStr);
      }


      lsCmd.Param ("key").setAsString () = key;
      lsCmd.Param ("minval").setAsLong () = minStrLen;
      lsCmd.Param ("maxval").setAsLong () = maxStrLen;
      lsCmd.Execute ();

      if (true == lsCmd.FetchNext ())
      {
         strcpy (Value, lsCmd.Field ("VALUE").asString ());
      }
      else
      {
         return DB_NO_RECORD_FOUND_ERR;
      }

      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdSet = false;

      return ErrorHandler (e, "ReadCfgStr()");
   }
}
