static const char* id="@(#) $Id: IvrDbManager.cc,v 1.1.1.1.2.5.2.3 2025/01/02 11:42:33 mram Exp $" ;
//---------------------------------------------------------------------------------
//  NAME       :  IvrDbManager.cc
//
// COPYRIGHT      : TSS -- Copyright (C) 2011 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated  :  Divya K                               Dated:  09 Feb 2011
//---------------------------------------------------------------------------------

#include "IvrDbManager.h"

extern  int      errno;

//-------------------------------------------------------------
// METHOD      : IvrDbManager
// DESCRIPTION : constructor
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrDbManager::IvrDbManager()
{
	memset(mDbName, 0, IVR_MAX_DB_NAME_LEN);
	memset(mDbUser, 0, MAX_DB_USER);
	memset(mDbPass, 0, MAX_DB_PASS);
}

//-------------------------------------------------------------
// METHOD      : ~IvrDbManager
// DESCRIPTION : Destructor
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
IvrDbManager::~IvrDbManager()
{
	this->Disconnect();
}

//-------------------------------------------------------------------------------
// METHOD      : commit
// DESCRIPTION : Default Destructor
// PARAMETER   : no arguments
// RETURN      : no return value
//-------------------------------------------------------------------------------
void IvrDbManager::CommitNow()
{
	mConn.Commit();
}

//-------------------------------------------------------------
// METHOD        : connect()
// DESCRIPTION   :
// PARAMETER     : None
// RETURN        : success/failure status
//-------------------------------------------------------------
DbErrorCodes IvrDbManager::Connect(int lDbClient)
{
	TEXT*   lpTempPtr = NULL;
	TEXT    lLogText[MAX_LOG_TEXT_LEN + 1]= "";

	try
	{
		lpTempPtr = getenv("IVR_DB_NAME");

		if (NULLPTR == lpTempPtr)
			strncpy(mDbName, IVR_DB_NAME, IVR_MAX_DB_NAME_LEN - 1);
		else
			strncpy(mDbName, lpTempPtr, IVR_MAX_DB_NAME_LEN - 1);

		lpTempPtr = getenv("IVR_DB_USER");

		if (NULLPTR == lpTempPtr)
			strncpy(mDbUser, IVR_DB_USER, MAX_DB_USER - 1);
		else
			strncpy(mDbUser, lpTempPtr, MAX_DB_USER - 1);

		lpTempPtr = getenv("IVR_DB_PASS");

		if (NULLPTR == lpTempPtr)
			strncpy(mDbPass, IVR_DB_PASS, MAX_DB_PASS - 1);
		else
			strncpy(mDbPass, lpTempPtr, MAX_DB_PASS - 1);

		if(lDbClient == 1)
			mDbClient = SA_MySQL_Client;
		else if(lDbClient == 10)
         mDbClient = SA_PostgreSQL_Client;
      else
			mDbClient = SA_Oracle_Client;

		mConn.Connect(mDbName ,  mDbUser , mDbPass , mDbClient);

		snprintf(lLogText, MAX_LOG_TEXT_LEN, "Connected to Database");

		gLog.GenerateLog(DBP08 , lLogText);

		return DB_NO_ERR;
	}

	catch(SAException& e)
	{
		ErrorHandler(e, (TEXT*)"ConnectDb()" , (TEXT*) "");
		return DB_INIT_ERR;
	}
	return DB_NO_ERR;
}

//-------------------------------------------------------------
// METHOD        : Init()
// DESCRIPTION   :
// PARAMETER     : None
// RETURN        : success/failure status
//-------------------------------------------------------------
DbErrorCodes IvrDbManager::Init(int lDbClient)
{
	TEXT*   lpTempPtr = NULL;
	TEXT    lLogText[MAX_LOG_TEXT_LEN + 1]= "";

   try
	{
		lpTempPtr = getenv("IVR_DB_NAME");

		if (NULLPTR == lpTempPtr)
			strncpy(mDbName, IVR_DB_NAME, IVR_MAX_DB_NAME_LEN - 1);
		else
			strncpy(mDbName, lpTempPtr, IVR_MAX_DB_NAME_LEN - 1);

		lpTempPtr = getenv("IVR_DB_USER");

		if (NULLPTR == lpTempPtr)
			strncpy(mDbUser, IVR_DB_USER, MAX_DB_USER - 1);
		else
			strncpy(mDbUser, lpTempPtr, MAX_DB_USER - 1);

		lpTempPtr = getenv("IVR_DB_PASS");

		if (NULLPTR == lpTempPtr)
			strncpy(mDbPass, IVR_DB_PASS, MAX_DB_PASS - 1);
		else
			strncpy(mDbPass, lpTempPtr, MAX_DB_PASS - 1);

		if(lDbClient == 1)
			mDbClient = SA_MySQL_Client;
      else if(lDbClient == 10)
         mDbClient = SA_PostgreSQL_Client;
		else
			mDbClient = SA_Oracle_Client;

		mConn.Connect(mDbName ,  mDbUser , mDbPass , mDbClient);
		mConn.setAutoCommit (SA_AutoCommitOn);
		mConn.setOption("store");


		snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"Connected to Database lDbName:%s DbUser:%s DbCLient:%d", mDbName, mDbUser, mDbClient);
		T(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(DBP08 , lLogText);

		T(gTrace, printf("%s: DB API client version : %ld\n", gProcessName, mConn.ClientVersion()););

		return DB_NO_ERR;
	}

	catch(SAException& e)
	{
		ErrorHandler(e, (TEXT*)"IvrDb-Init()", (TEXT*) "");
		snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"Failed to Connect to Database lDbName:%s DbUser:%s DbCLient:%d", mDbName, mDbUser, mDbClient);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return DB_INIT_ERR;
	}
	return DB_NO_ERR;
}

//-------------------------------------------------------------
// METHOD        : connect()
// DESCRIPTION   :
// PARAMETER     : None
// RETURN        : success/failure status
//-------------------------------------------------------------
DbErrorCodes IvrDbManager::Init(TEXT* lDbName, TEXT* lUser, TEXT* lPasswd, int lDbClient)
{
	TEXT*   lpTempPtr = NULL;
	TEXT    lLogText[MAX_LOG_TEXT_LEN + 1]= "";

	try
	{
		if(lDbName != nullptr)
		{
			strncpy(mDbName, lDbName, IVR_MAX_DB_NAME_LEN - 1);
		}
		else
		{
			lpTempPtr = getenv("IVR_DB_NAME");
			if (NULLPTR == lpTempPtr)
				strncpy(mDbName, lpTempPtr, IVR_MAX_DB_NAME_LEN - 1);
		}

		if (lUser != nullptr)
		{
			strncpy(mDbUser, lUser, MAX_DB_USER - 1);
		}
		else
		{
			lpTempPtr = getenv("IVR_DB_USER");
			if (NULLPTR == lpTempPtr)
				strncpy(mDbUser, lpTempPtr, MAX_DB_USER - 1);
		}

		if (lPasswd != nullptr)
		{
			strncpy(mDbPass, lPasswd, MAX_DB_PASS - 1);
		}
		else
		{
			lpTempPtr = getenv("IVR_DB_PASS");
			if (NULLPTR == lpTempPtr)
				strncpy(mDbPass, lpTempPtr, MAX_DB_PASS - 1);
		}

		if(lDbClient == 1)
			mDbClient = SA_MySQL_Client;
      else if(lDbClient == 10)
         mDbClient = SA_PostgreSQL_Client;
		else
			mDbClient = SA_Oracle_Client;

		mConn.Connect(mDbName,  mDbUser, mDbPass, mDbClient);
		mConn.setAutoCommit (SA_AutoCommitOn);
		mConn.setOption("store");

		snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"Connected to Database lDbName:%s DbUser:%s", mDbName, mDbUser);
		gLog.GenerateLog(DBP08 , lLogText);

		return DB_NO_ERR;
	}

	catch(SAException& e)
	{
		ErrorHandler(e, (TEXT*)"IvrDb-Init()", (TEXT*) "");
		snprintf(lLogText, MAX_LOG_TEXT_LEN, 
				"Failed to Connect to Database lDbName:%s DbUser:%s DbCLient:%d Err:%s", mDbName, mDbUser, mDbClient, (const char *)e.ErrText());
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		return DB_INIT_ERR;
	}
	return DB_NO_ERR;
}

//-------------------------------------------------------------
// METHOD      : DeInit 
// DESCRIPTION : DeInitializing DB Object
// PARAMETER   :
// RETURN      : DbErrorCodes
//-------------------------------------------------------------
DbErrorCodes IvrDbManager::ReInit()
{
	TEXT   lLogText[MAX_LOG_TEXT_LEN + 1]= "";
	try
	{
		if (mConn.isAlive () || mConn.isConnected())
		{
			mConn.Disconnect ();
		}

		int lDbClient = 1;
		if(mDbClient == SA_MySQL_Client)
         lDbClient = 1;
      if(mDbClient == SA_PostgreSQL_Client)
         lDbClient = 10;
      else
			lDbClient = 0;

		this->Init(lDbClient);
		snprintf(lLogText, MAX_LOG_TEXT_LEN, "Connected to Database");

		gLog.GenerateLog (DBP08, lLogText);

	}
	catch(SAException& e)
	{
		return this->ErrorHandler (e,(TEXT*) "IvrDb-ReInit()", (TEXT*) "");
	}

	return DB_NO_ERR ;
}


//-------------------------------------------------------------
// METHOD      : Disconnect
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : success/failure status
//-------------------------------------------------------------
DbErrorCodes IvrDbManager::Disconnect()
{
	TEXT   lLogText[MAX_LOG_TEXT_LEN + 1]= "";
	try
	{
		if(mConn.isConnected())
		{
			mConn.Disconnect();
			snprintf(lLogText, MAX_LOG_TEXT_LEN, "Disconnected from Database");
			gLog.GenerateLog(DBP09, lLogText);
			return DB_NO_ERR;
		}
		else
		{
			return DB_INIT_ERR;
		}

	}
	catch(SAException& e)
	{
		ErrorHandler(e, (TEXT*)"disconnect()", (TEXT*) "");
		return DB_INIT_ERR;
	}
}

//-------------------------------------------------------------
// METHOD      : CheckDBStatus
// DESCRIPTION : Chacking Status of Database
// PARAMETER   :
// RETURN      : BOOLEAN
//-------------------------------------------------------------
DbErrorCodes IvrDbManager::CheckDBStatus()
{
	TEXT   lLogText[MAX_LOG_TEXT_LEN + 1]= "";
	try
	{
		if (mConn.isAlive ())
		{
         if(mDbClient == SA_MySQL_Client)
         {
            long lServerVersion = mConn.ServerVersion();
            T(gTrace, printf("%s: MySql server Version - %ld\n", gProcessName,  lServerVersion););
         }
			return DB_NO_ERR;
		}
		else if (mConn.isConnected ())
			mConn.Disconnect ();

		int lDbClient = 1;
		if(mDbClient == SA_MySQL_Client)
			lDbClient = 1;
      if(mDbClient == SA_PostgreSQL_Client)
         lDbClient = 10;
      else
			lDbClient = 0;

		this->Init(mDbName, mDbUser, mDbPass, lDbClient);
		snprintf(lLogText, MAX_LOG_TEXT_LEN, "Connected to Database");

		gLog.GenerateLog (DBP08, lLogText);

	}
	catch(SAException& e)
	{
		return this->ErrorHandler (e,(TEXT*) "CheckDbStatus()", (TEXT*) "");
	}

	return DB_NO_ERR ;
}


//------------------------------------------------------------------------------
// METHOD      : ErrorHandler
// DESCRIPTION : This function  sets global errno depending upon error retrived
//               from database and generates appropriate log
// PARAMETER   : Reference to SAException , Function Name , Debug Information
// RETURN      : success/failure status
//-------------------------------------------------------------------------------
DbErrorCodes IvrDbManager::ErrorHandler(SAException& err, TEXT* funcName, TEXT* debugInfo, BOOLEAN genTrace)
{
	TEXT            lErrMsg[MAX_LOG_TEXT_LEN];
	DbErrorCodes    lDbError;

	snprintf(lErrMsg, MAX_LOG_TEXT_LEN, "DBP: Func:%s Arg:%s Err:%s,Code %d",
			funcName,debugInfo,(const char *)err.ErrText(),err.ErrNativeCode());
	if(genTrace)
	{
		TERR(gTrace, printf("%s: %s\n", gProcessName, lErrMsg););
	}

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

		case 1452: // MySQL : error 1452 - Cannot add or update a child row: a foreign key constraint fails
			{
				lDbError = DB_FORIGN_KEY_VIOLATION_ERR;
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
		case -1: //SQLAPI - API client not set
			{
				gLog.GenerateLog (DBP00, lErrMsg);
				try
				{
					if (mConn.isConnected ())
						mConn.Disconnect ();

					mConn.Connect (mDbName, mDbUser, mDbPass, mDbClient);
					mConn.setAutoCommit (SA_AutoCommitOn);
					mConn.setOption("store");
					mCommand.setConnection (&mConn);

					if(genTrace)
					{
						T (gTrace, printf ("%s: DbName:%s DbUser:%s Connected to Db\n",
									gProcessName, mDbName, mDbUser););
					}
					gLog.GenerateLog (DBP08);
					lDbError = DB_NO_ERR;
				}
				catch (SAException & e)
				{
					if(genTrace)
					{
						T (gTrace, printf ("%s: DbName:%s DbUser:%s Failed to Connect\n",
									gProcessName, mDbName, mDbUser););
					}
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

//------------------------------------------------------------------------------
// METHOD      : ExecuteQuery 
// DESCRIPTION : This function Executes the query and returns the result 
// PARAMETER   : 
// RETURN      : success/failure status
//-------------------------------------------------------------------------------
DbErrorCodes  IvrDbManager::ExecuteQuery(EnumQueryType& lQtype, TEXT* lQuery, TEXT lVars[][MAX_VARIABLE_LEN], int& varCnt, TEXT* ErrMsg, int& lRowsFetched)
{
	static SACommand     lsSelectCommand;
	int    lVaCnt        = 0;
	TEXT   fieldName[20] = "";
	TEXT   lReturn_vars[MAX_NO_OF_VARIABLES][MAX_VARIABLE_LEN];

	DbErrorCodes lDbErr = CheckDBStatus();
	if (DB_NO_ERR != lDbErr)
	{
		return lDbErr;
	}


	try
	{
		lsSelectCommand.setConnection (&mConn);
		memset (mSqlStr, 0, sizeof (mSqlStr));

		sprintf(mSqlStr, "%s", lQuery );
		lsSelectCommand.setCommandText (mSqlStr);
		lsSelectCommand.Execute ();

		TEXT*   lToken;
		TEXT    lDtype[10] = "";
		int     lStartIndx = 0;
		lRowsFetched = 0;

		switch(lQtype)
		{
			case SELECT:
				{
					while( true == lsSelectCommand.FetchNext())
					{
						for(int i=lStartIndx, j=0;  j<varCnt ; i+=2, j++)
						{
							lToken = strtok(lVars[j],":");
							strcpy(fieldName, lToken);
							strncpy(lDtype, lToken+strlen(lToken)+1, 1);

							sprintf(lReturn_vars[i], "%s%d", fieldName, lRowsFetched);
							if(strcasecmp(lDtype, "S") == 0)
							{
								lVaCnt++;
								strcpy(lReturn_vars[i+1], lsSelectCommand.Field(fieldName).asString());
							}
							else if(strcasecmp(lDtype, "L") == 0)
							{
								lVaCnt++;
								long ltmpVal = lsSelectCommand.Field(fieldName).asLong();
								sprintf(lReturn_vars[i+1], "%ld", ltmpVal);
							}
							else if(strcasecmp(lDtype, "I") == 0 || strcasecmp(lDtype, "SH") == 0)
							{
								lVaCnt++;
								int ltmpVal = lsSelectCommand.Field(fieldName).asShort();
								sprintf(lReturn_vars[i+1], "%d", ltmpVal);
							}
							else if(strcasecmp(lDtype, "D") == 0)
							{
								lVaCnt++;
								//SADateTime ltempDate;
								//ltempDate = lsSelectCommand.Field(fieldName).asDateTime();
								//struct tm lDate = tm( ltempDate );
								struct tm lDate = lsSelectCommand.Field(fieldName).asDateTime();                        
								sprintf( lReturn_vars[i+1], "%02d/%02d/%04d %02d:%02d:%02d", 
										lDate.tm_mday, lDate.tm_mon, (lDate.tm_year+1900), lDate.tm_hour, 
										lDate.tm_min, lDate.tm_sec);
							}
							else if(strcasecmp(lDtype, "R") == 0)
							{
								lVaCnt++;
								memcpy ((void *) &(lReturn_vars[i+1]), lsSelectCommand.Field(fieldName).asBytes().GetBuffer(sizeof(lReturn_vars[i+1])), sizeof(lReturn_vars[i+1]));
							}
							lStartIndx = i;
						}
						lStartIndx = lStartIndx + 2;
						lRowsFetched++;
					}
					break;
				}
			case UPDATE:
			case DELETE:
			case INSERT:
				{
					lVaCnt = lsSelectCommand.RowsAffected();
					CommitNow();

					break;
				}
			case CREATE:
				CommitNow();
				return DB_NO_ERR;
			default:break;
		}

		if(lVaCnt > 0)
		{
			memcpy(lVars, lReturn_vars, sizeof(lReturn_vars));
			if(lQtype == SELECT)
				varCnt = lVaCnt * 2;
			else
				varCnt = lVaCnt;
			return DB_NO_ERR;
		}
		else
			return DB_NO_RECORD_FOUND_ERR;

	}
	catch(SAException& e)
	{
		strcpy(ErrMsg, e.ErrText());
		return ErrorHandler(e,(TEXT*) "ExecuteQuery()", NULL, false);
	}
}

//------------------------------------------------------------------------------
// METHOD      : ExecuteQuery 
// DESCRIPTION : This function Executes the query and returns the result 
// PARAMETER   : 
// RETURN      : success/failure status
//-------------------------------------------------------------------------------
DbErrorCodes  IvrDbManager::ExecuteQuery1(EnumQueryType& lQtype, TEXT* lQuery, TEXT lVars[][MAX_VARIABLE_LEN], int& varCnt, TEXT* ErrMsg, int& lRowsFetched)
{
	static SACommand     lsSelectCommand;
	int    lVaCnt        = 0;
	TEXT   fieldName[20] = "";
	TEXT   lReturn_vars[MAX_NO_OF_VARIABLES][MAX_VARIABLE_LEN];
	TEXT   lTempVal[MAX_VARIABLE_LEN] ="";
	TEXT   lTempNum[MAX_VARIABLE_LEN] ="";
	int    lRowsToFetched = lRowsFetched;

	TEXT*   lToken;
	TEXT    lDtype[10] = "";

	DbErrorCodes lDbErr = CheckDBStatus();
	if (DB_NO_ERR != lDbErr)
	{
		return lDbErr;
	}

	try
	{
		lsSelectCommand.setConnection (&mConn);
		memset (mSqlStr, 0, sizeof (mSqlStr));

		sprintf(mSqlStr, "%s", lQuery );
		lsSelectCommand.setCommandText (mSqlStr);
		lsSelectCommand.Execute ();

		//int     lStartIndx = 0;
		lRowsFetched = 0;

		switch(lQtype)
		{
			case SELECT:
				{
					memset(&lReturn_vars[varCnt], 0, MAX_VARIABLE_LEN);
					for(int i=0;  i<varCnt ; i++)
					{
						lToken = strtok(lVars[i],":");
						strcpy(fieldName, lToken);
						strncpy(lDtype, lToken+strlen(lToken)+1, 1);
						strcat(lReturn_vars[varCnt], " ");
						strcat(lReturn_vars[varCnt], fieldName);
						if(lRowsToFetched == 1)
							sprintf(lReturn_vars[i], "<var name=\"%s\" expr=\"", fieldName);
						else
							sprintf(lReturn_vars[i], "<var name=\"%s\" expr=\"new Array(", fieldName);
						//printf("%s\n", lReturn_vars[i]);
					}
					while( true == lsSelectCommand.FetchNext())
					{
						if(lRowsToFetched > 0)
						{
							if(lRowsFetched == lRowsToFetched)
								break;
						}
						for(int j=0;  j<varCnt ; j++)
						{
							lToken = strtok(lVars[j],":");
							strcpy(fieldName, lToken);
							strncpy(lDtype, lToken+strlen(lToken)+1, 1);

							if(strcasecmp(lDtype, "S") == 0)
							{
								lVaCnt++;
								strcpy(lTempVal, "\'");
								strcat(lTempVal, lsSelectCommand.Field(fieldName).asString());
								strcat(lTempVal, "\',");
								strcat(lReturn_vars[j], lTempVal);
							}
							else if(strcasecmp(lDtype, "L") == 0)
							{
								lVaCnt++;
								long ltmpVal = lsSelectCommand.Field(fieldName).asLong();
								strcpy(lTempVal, "\'");
								sprintf(lTempNum, "%ld", ltmpVal);
								strcat(lTempVal, lTempNum);
								strcat(lTempVal, "\',");
								strcat(lReturn_vars[j], lTempVal);
							}
							else if(strcasecmp(lDtype, "I") == 0 || strcasecmp(lDtype, "SH") == 0)
							{
								lVaCnt++;
								int ltmpVal = lsSelectCommand.Field(fieldName).asShort();
								strcpy(lTempVal, "\'");
								sprintf(lTempNum, "%d", ltmpVal);
								strcat(lTempVal, lTempNum);
								strcat(lTempVal, "\',");
								strcat(lReturn_vars[j], lTempVal);
							}
							else if(strcasecmp(lDtype, "D") == 0)
							{
								lVaCnt++;
								//SADateTime ltempDate;
								//ltempDate = lsSelectCommand.Field(fieldName).asDateTime();
								//struct tm lDate = tm( ltempDate );
								struct tm lDate = lsSelectCommand.Field(fieldName).asDateTime();
								sprintf( lTempVal, "'%02d/%02d/%04d %02d:%02d:%02d',", 
										lDate.tm_mday, lDate.tm_mon+1, (lDate.tm_year+1900), 
										lDate.tm_hour, lDate.tm_min, lDate.tm_sec);
								strcat(lReturn_vars[j], lTempVal);
							}
							else if(strcasecmp(lDtype, "R") == 0)
							{
								lVaCnt++;
								memcpy ((void *) &lTempVal, lsSelectCommand.Field(fieldName).asBytes().GetBuffer(sizeof(lTempVal)), sizeof(lTempVal));
								strcat(lReturn_vars[j], lTempVal);
								strcat(lReturn_vars[j], ",");
							}
						}
						lRowsFetched++;
					}
					for(int j=0;  j<varCnt ; j++)
					{
						if(lRowsFetched!=0)
							lReturn_vars[j][strlen(lReturn_vars[j])-1] = '\0';
						if(lRowsToFetched == 1)
							strcat(lReturn_vars[j], "\"/>");
						else
							strcat(lReturn_vars[j], ")\"/>");
						//printf("%s\n", lReturn_vars[j]);
					}
					lVaCnt = varCnt;
					break;
				}
			case UPDATE:
			case DELETE:
			case INSERT:
				{
					lVaCnt = lsSelectCommand.RowsAffected();
					CommitNow();

					break;
				}
			case CREATE:
				CommitNow();
				return DB_NO_ERR;
			default:break;
		}


		if(lVaCnt > 0)
		{
			memcpy(lVars, lReturn_vars, sizeof(lReturn_vars));
			//varCnt = lVaCnt;
			return DB_NO_ERR;
		}
		else 
			return DB_NO_RECORD_FOUND_ERR;

	}
	catch(SAException& e)
	{
		strncpy(ErrMsg, e.ErrText(), (IVR_MAX_CGI_DB_ERR_MSG_LEN-1));
		//snprintf(ErrMsg, (IVR_MAX_CGI_DB_ERR_MSG_LEN - 1),"%s", e.ErrText());
		/*   for(int i=0;  i<varCnt ; i++)
		     {
		     lToken = strtok(lVars[i],":");
		     strcpy(fieldName, lToken);
		     strncpy(lDtype, lToken+strlen(lToken)+1, 1);
		     strcat(lReturn_vars[varCnt], " ");
		     strcat(lReturn_vars[varCnt], fieldName);
		     if(lRowsToFetched == 1)
		     sprintf(lReturn_vars[i], "<var name=\"%s\" expr=\"\">", fieldName);
		     else
		     sprintf(lReturn_vars[i], "<var name=\"%s\" expr=\"new Array()\">", fieldName);
		//printf("%s\n", lReturn_vars[i]);
		}
		memcpy(lVars, lReturn_vars, sizeof(lReturn_vars));
		*/
		return ErrorHandler(e, (TEXT*)"ExecuteQuery()", NULL, false);
	}
}

////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------
// METHOD      : ReadCfgNum
// DESCRIPTION : Reads the configuration details from the
//               CONFIG_DETAILS table.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::ReadCfgNum (const TEXT * key, INT32 & cfgVal,
		INT32 minVal, INT32 maxVal)
{
	TEXT lCfgStr[MAX_LOG_TEXT_LEN + 1] = "";
	INT32 lCfgVal = 0;

	DbErrorCodes lDbErr = CheckDBStatus();
	if (DB_NO_ERR != lDbErr)
	{
		return lDbErr;
	}


	// Number can't go beyond 10 digits in a 32 bit machine
	lDbErr = this->ReadCfgStr (key, lCfgStr, 1, 10);
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
DbErrorCodes IvrDbManager::ReadCfgStr (const TEXT * key, TEXT * Value,
		INT32 minStrLen, INT32 maxStrLen)
{
	static BOOLEAN lsCmdSet = false;
	static SACommand lsCmd;
	TEXT   lSqlStr[MAX_SQL_STR_LEN + 1] = "";

	DbErrorCodes lDbErr = CheckDBStatus();
	if (DB_NO_ERR != lDbErr)
	{
		lsCmdSet = false;
		return lDbErr;
	}

	try
	{
		if (lsCmdSet == false)
		{
			lsCmd.setConnection (&mConn);
			snprintf (lSqlStr, MAX_SQL_STR_LEN,
					"SELECT VALUE "
					"  FROM IVR_CONFIG_MAIN "
					" WHERE CONFIG_KEY = :key "
					"   AND (LENGTH(VALUE) >= :minval "
					"   AND LENGTH(VALUE) <= :maxval )");

			lsCmd.setCommandText (lSqlStr);
         lsCmd.setOption("HandleResult") = "store";
			lsCmdSet = true;
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

		while(lsCmd.FetchNext ());
		return DB_NO_ERR;
	}
	catch (SAException & e)
	{
		lsCmdSet = false;

		return ErrorHandler (e,(TEXT*) "ReadCfgStr()", (TEXT*) "");
	}
}

//----------------------------------------------------------------------
// METHOD      : FetchScheduledReq
// DESCRIPTION : Fetches the scheduled request 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::FetchScheduledReq(ScheduleReq lScheduleReq[IVR_NO_OF_SCHEDULE_RECORDS], 
		INT16& lRetrySec, INT16& lMaxRetry, UINT32& lTotalCount, UINT32 noc)
{
	static SACommand lsCmd;
	static SACommand lsCheckCmd;
	static SACommand lsUpCmd;
	static BOOLEAN lsCmdSet   = false;
	static BOOLEAN lsCheckCmdSet   = false;
	static BOOLEAN lsUpCmdSet = false;
	static INT16   lRtrySec   = lRetrySec;
	UINT32 lCount = 0;
	//TEXT   lParam[50] = "";
	DbErrorCodes lDbErr = CheckDBStatus();
	if (DB_NO_ERR != lDbErr)
	{
		lsCmdSet = false;
		lsCheckCmdSet = false;
		lsUpCmdSet = false;
		return lDbErr;
	}

	try
	{
		if (lsCheckCmdSet == false)
		{
			TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

			lsCheckCmd.setConnection (&mConn);
			snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT COUNT(*) AS REC_COUNT "
					"FROM IVR_SCHEDULE WHERE SCHEDULE_TIME <= SYSDATE() AND DEL_ATTEMPTS < :attempt "
					"AND STATUS IN (1) AND END_TIME > SYSDATE()");
			//snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SCHEDULE_ID, SCHEDULE_TYPE,"
			//          "CALL_REF_NUM, REQUEST_MODE, RECIPIENT_NO, SENDER_NO, APPL_NAME "
			//          "FROM IVR_SCHEDULE WHERE SCHEDULE_TIME <= SYSDATE() AND DEL_ATTEMPTS < :attempt "
			//          "AND STATUS IN (1) AND END_TIME > SYSDATE() LIMIT :lrow"
			//         );

			T(gTrace, printf("%s: CheckSchedule Query: %s\n", gProcessName, lSqlStr););
			lsCheckCmd.setCommandText (lSqlStr);
			lsCheckCmdSet = true;
		}

		lsCheckCmd.Param ("attempt").setAsLong () = lMaxRetry+1;
		//lsCheckCmd.Param ("lrow").setAsLong ()    = 1; //IVR_NO_OF_SCHEDULE_RECORDS;
		lsCheckCmd.Execute ();

		while (true == lsCheckCmd.FetchNext ())
		{
			lCount = lsCheckCmd.Field ("REC_COUNT").asLong ();
		}

		if(lCount >= noc)
		{
			return DB_NO_RECORD_FOUND_ERR;
		}

		lCount=0;
		if (lsCmdSet == false)
		{
			TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

			lsCmd.setConnection (&mConn);
			snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SCHEDULE_ID, SCHEDULE_TYPE,"
					"CALL_REF_NUM, REQUEST_MODE, RECIPIENT_NO, SENDER_NO, APPL_NAME, "
					"APP_NOP, APP_PARAM1, APP_PARAM2, APP_PARAM3, APP_PARAM4, APP_PARAM5 "
					"FROM IVR_SCHEDULE WHERE SCHEDULE_TIME <= SYSDATE() AND DEL_ATTEMPTS < :attempt "
					"AND STATUS IN (0,3) AND END_TIME > SYSDATE() "
					"AND RECIPIENT_NO NOT IN (SELECT RECIPIENT_NO FROM IVR_SCHEDULE WHERE STATUS =1) "
					"LIMIT :lrow"
				 );

			T(gTrace, printf("%s: FetchSchedule Query: %s\n", gProcessName, lSqlStr););
			lsCmd.setCommandText (lSqlStr);
			lsCmdSet = true;
		}

		lsCmd.Param ("attempt").setAsLong () = lMaxRetry;
		lsCmd.Param ("lrow").setAsLong ()    = 1; //IVR_NO_OF_SCHEDULE_RECORDS;
		lsCmd.Execute ();

		while (true == lsCmd.FetchNext ())
		{
			lTotalCount++;
			lScheduleReq[lCount].scheduleId = lsCmd.Field ("SCHEDULE_ID").asLong ();
			lScheduleReq[lCount].type       = (EnumScheduleType)lsCmd.Field ("SCHEDULE_TYPE").asShort ();
			lScheduleReq[lCount].mode       = (EnumReqMode)lsCmd.Field ("REQUEST_MODE").asShort ();
			strcpy(lScheduleReq[lCount].shortCode, lsCmd.Field ("APPL_NAME").asString());
			strcpy(lScheduleReq[lCount].origAddr , lsCmd.Field ("SENDER_NO").asString());
			strcpy(lScheduleReq[lCount].destAddr , lsCmd.Field ("RECIPIENT_NO").asString());
			lScheduleReq[lCount].varCount = lsCmd.Field ("APP_NOP").asShort();
			strcpy(lScheduleReq[lCount].var[0] , lsCmd.Field ("APP_PARAM1").asString());
			strcpy(lScheduleReq[lCount].var[1] , lsCmd.Field ("APP_PARAM2").asString());
			strcpy(lScheduleReq[lCount].var[2] , lsCmd.Field ("APP_PARAM3").asString());
			strcpy(lScheduleReq[lCount].var[3] , lsCmd.Field ("APP_PARAM4").asString());
			strcpy(lScheduleReq[lCount].var[4] , lsCmd.Field ("APP_PARAM5").asString());

			T(gTrace, printf("%s: DB Schedule Request :(%d): %s, %s, %s\n",gProcessName, lCount, lScheduleReq[lCount].shortCode, lScheduleReq[lCount].origAddr, lScheduleReq[lCount].destAddr););
			lCount++;
		}

		if(lTotalCount> 0)
		{
			T(gTrace, printf("%s: Total Schedule requests : %d\n", gProcessName, lTotalCount););
			if(lsUpCmdSet == false || lRtrySec != lRetrySec)
			{
				TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";
				lsUpCmd.setConnection (&mConn);
				snprintf (lSqlStr, MAX_SQL_STR_LEN,"UPDATE IVR_SCHEDULE SET DEL_ATTEMPTS=DEL_ATTEMPTS+1,"
						"SCHEDULE_TIME=(SYSDATE()+(%d/1440)), STATUS=1 WHERE SCHEDULE_ID=:shid", lRetrySec);
				lsUpCmd.setCommandText (lSqlStr);
				lsUpCmdSet = true;
				lRtrySec = lRetrySec;
			}
			lsUpCmd.Param ("shid").setAsLong () = lScheduleReq[lCount-1].scheduleId;
			lsUpCmd.Execute ();
			INT16 lRowsAffected = lsUpCmd.RowsAffected() ;

			if(lRowsAffected > 0)
			{
				T(gTrace, printf("%s: Status updated for SchId: %d\n", gProcessName, lScheduleReq[lCount-1].scheduleId););
				//while(lsUpCmd.FetchNext());
				return DB_NO_ERR;
			}
			else
				return DB_UPDATE_FAILURE_ERR;

			return DB_NO_ERR;
		}
		else
			return DB_NO_RECORD_FOUND_ERR;

	}
	catch (SAException & e)
	{
      lsCmdSet = false;

      return ErrorHandler (e,(TEXT*) "FetchScheduledReq()" , (TEXT*) "");
   }
}

//----------------------------------------------------------------------
// METHOD      : UpdateScheduledRsp 
// DESCRIPTION : Fetches the scheduled request 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::UpdateScheduledRsp(ScheduleRsp& lScheduleRsp)
{
   SACommand lsUpCmd;
   BOOLEAN   lsUpCmdSet   = false;
   struct tm * lpCallTime;
   UINT32 duration=0;
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsUpCmdSet = false;
      return lDbErr;
   }

   try
   {
      if (lsUpCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsUpCmd.setConnection (&mConn);
         snprintf (lSqlStr, MAX_SQL_STR_LEN, "UPDATE IVR_SCHEDULE SET STATUS=:stat,ERROR_ID=:err,"
                   " CALL_REF_NUM=:ref, "
                   " CALL_TIME=:callTime, "
                   " DURATION=:duration "
                   " WHERE SCHEDULE_ID = :shid AND APPL_NAME=:shcode");
                    
         lsUpCmd.setCommandText (lSqlStr);
         lsUpCmdSet = true;
      }

      lpCallTime = localtime(&lScheduleRsp.callTime);
      if(lScheduleRsp.answerTime > 0)
         duration = (lScheduleRsp.disconnectTime - lScheduleRsp.answerTime);

      lsUpCmd.Param ("stat").setAsShort ()    = lScheduleRsp.status;
      lsUpCmd.Param ("err").setAsLong ()      = lScheduleRsp.errorCode;
      lsUpCmd.Param ("ref").setAsString ()    = lScheduleRsp.callRefNum;
      lsUpCmd.Param ("shid").setAsLong ()     = lScheduleRsp.scheduleId;
      lsUpCmd.Param ("shcode").setAsString () = lScheduleRsp.shortCode;
      lsUpCmd.Param ("callTime").setAsDateTime() = *lpCallTime;
      lsUpCmd.Param ("duration").setAsLong ()      = duration;
      lsUpCmd.Execute ();

      INT16 lRowsAffected = lsUpCmd.RowsAffected() ;

      if(lRowsAffected > 0)
      {
         //while(lsUpCmd.FetchNext());
         this->PostScheduleStatusUpdate(lScheduleRsp.scheduleId);
         return DB_NO_ERR;
      }
      else
         return DB_UPDATE_FAILURE_ERR;
   }
   catch (SAException & e)
   {
      lsUpCmdSet = false;

      return ErrorHandler (e,(TEXT*) "UpdateScheduledRsp()", (TEXT*) "");
   }

}

//----------------------------------------------------------------------
// METHOD      : FetchChannelCmd 
// DESCRIPTION : 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::FetchChannelCmd( ChannelMgmt lChannelMgmt[MAX_NUM_OF_CKT_CMDS], 
                              UINT32& lMinChannelId, UINT32& lMaxChannelId)
{
   static SACommand lsCmd;
   static SACommand lsUpCmd;
   static BOOLEAN lsCmdSet   = false;
   static BOOLEAN lsUpCmdSet = false;
   UINT32 lCount = 0;

   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdSet = false;
      lsUpCmdSet = false;
      return lDbErr;
   }

   try
   {
      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConn);
         /*snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT COMMAND_ID, CHANNEL_ID, COMMAND,"
                   "STATUS, PATCH_CHANNEL_ID FROM CHANNEL_COMMANDS WHERE CHANNEL_ID =:cid");*/
         snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT COMMAND_ID, CHANNEL_ID, COMMAND,"
                   "STATUS, PATCH_CHANNEL_ID FROM CHANNEL_COMMANDS WHERE CHANNEL_ID "
                   "BETWEEN :minCid and :maxCid"
                  );
                    
         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
      }

      lsCmd.Execute ();

      while (true == lsCmd.FetchNext ())
      {
         lChannelMgmt[lCount].cmdId          = lsCmd.Field ("COMMAND_ID").asLong ();
         lChannelMgmt[lCount].status         = (EnumIVRChannelStatus)lsCmd.Field ("STATUS").asShort ();
         lChannelMgmt[lCount].patchChannelId = lsCmd.Field ("PATCH_CHANNEL_ID").asLong ();
         lChannelMgmt[lCount].cmd            = (EnumChannelCmd)lsCmd.Field ("COMMAND").asShort ();
         lChannelMgmt[lCount].channelId      = lsCmd.Field ("CHANNEL_ID").asLong();

         if(lsUpCmdSet == false)
         {
            TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";
            lsUpCmd.setConnection (&mConn);
            snprintf (lSqlStr, MAX_SQL_STR_LEN,"UPDATE CHANNEL_COMMANDS SET STATUS=:stat"
                      " WHERE CHANNEL_ID =:chid");
            lsUpCmd.setCommandText (lSqlStr);
            lsUpCmdSet = true;
         }
         lsUpCmd.Param ("chid").setAsLong () = lChannelMgmt[lCount].channelId;
         lsUpCmd.Param ("stat").setAsLong () = IVR_CHANNEL_CMD_IN_PROGRESS;

         lsUpCmd.Execute ();
         INT16 lRowsAffected = lsUpCmd.RowsAffected() ;

         if(lRowsAffected > 0)
         {
            //do nothing
         }
         else
            return DB_UPDATE_FAILURE_ERR;

         lCount++;
      }

      if(lCount > 0)
         return DB_NO_ERR;
      else
         return DB_NO_RECORD_FOUND_ERR;

   }
   catch (SAException & e)
   {
      lsCmdSet = false;

      return ErrorHandler (e, (TEXT*) "FetchChannelCmd()", (TEXT*) "");
   }
}



//----------------------------------------------------------------------
// METHOD      : FetchFeedbackRsp
// DESCRIPTION : Fetches the scheduled request 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::FetchFeedbackRsp()
{
   static SACommand lsCmd;
   static SACommand lsCheckCmd;
   static SACommand lsUpCmd;
   static BOOLEAN lsCmdSet   = false;
   static BOOLEAN lsCheckCmdSet   = false;
   static BOOLEAN lsUpCmdSet = false;
   UINT32 lCount = 0;
  UINT32  lRecId= 0;
  FILE        *fpReqRespFile; 
   TEXT        lFileName[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT        lLogText[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT        lTemp[MAX_LOG_TEXT_LEN + 1] = "";
   TEXT        lTime[15] = "";
   TEXT        lDate[15] = "";
         time_t      lCurrentTime;
      struct tm   *lpTimeStruct;

      memset(lTime, 0, 15);

      lCurrentTime = time(NULL);
      lpTimeStruct = localtime(&lCurrentTime);
      strftime(lDate, 14, "%Y%m%d", lpTimeStruct);
      strftime(lTime, 14, "%H:%M:%S", lpTimeStruct);
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdSet = false;
      lsUpCmdSet = false;
      lsCheckCmdSet = false;
      return lDbErr;
   }


   try
   {
      if (lsCheckCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCheckCmd.setConnection (&mConn);
         snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT RECORD_ID, OBD_FEEDBACK.OBD_APPL_ID, OBD_APP_NAME, DEST_NUM, OBD_FEEDBACK.FEEDBACK, ENTRY_TIME, READ_FLAG FROM OBD_FEEDBACK, OBD_APP WHERE OBD_APP.OBD_APPL_ID = OBD_FEEDBACK.OBD_APPL_ID  AND READ_FLAG=0 LIMIT 1;");
                    
         T(gTrace, printf("%s: Feedback Query: %s\n", gProcessName, lSqlStr););
         lsCheckCmd.setCommandText (lSqlStr);
         lsCheckCmdSet = true;
      }

      lsCheckCmd.Execute ();

      while (true == lsCheckCmd.FetchNext ())
      {
         lCount++;
         // Write Record  to File

         snprintf(lFileName, MAX_LOG_TEXT_LEN,    "/opt/ivr/var/log/feedback_%s.csv", lDate);

         T(gTrace ,cout<<gProcessName<<": Writing Feedback Response to the File : " << lFileName <<endl;);

         if( -1 == access(lFileName, F_OK)) //If file is not there Create a new file with header
         {
            fpReqRespFile = fopen(lFileName, "w");
            if(fpReqRespFile == NULL)
            {
               fpReqRespFile = fopen(lFileName, "w");
               if(fpReqRespFile == NULL)
               {
                  TERR(gTrace, printf("%s: Cannot open file %s\n", gProcessName, lFileName);)
                     fclose(fpReqRespFile);
                  gLog.GenerateLog(GSYS10, lLogText);
                  return DB_UPDATE_FAILURE_ERR;
               }
            }

            snprintf(lTemp ,MAX_LOG_TEXT_LEN,  "%-15s| %-10s| %-15s| %-10s| %-10s\n", "SERVICE NAME", "CLI", "CALLED NUM", "FEEDBACK", "TIME");
            fwrite( lTemp, sizeof(char) , strlen(lTemp), fpReqRespFile );
            snprintf(lTemp ,MAX_LOG_TEXT_LEN,  "---------------|-----------|----------------|-----------|-----------\n");
            fwrite( lTemp, sizeof(char) , strlen(lTemp), fpReqRespFile );
            fclose(fpReqRespFile);
         }

         fpReqRespFile = fopen(lFileName, "a+");
         if (NULL == fpReqRespFile)
         {
            snprintf(lLogText, MAX_LOG_TEXT_LEN - 1, "Failed to open file %s", lFileName);
            TERR(gTrace, printf("%s: Feedback file open failed %s", gProcessName, lLogText););
            gLog.GenerateLog(GSYS10, lLogText);
            return DB_UPDATE_FAILURE_ERR;
         }
         lRecId = lsCheckCmd.Field ("RECORD_ID").asLong();
         TEXT lText1[MAX_LOG_TEXT_LEN] = "";
         TEXT lText2[MAX_LOG_TEXT_LEN] = "";
         TEXT lText3[MAX_LOG_TEXT_LEN] = "";
         TEXT lText4[MAX_LOG_TEXT_LEN] = "";

         snprintf(lText4, MAX_LOG_TEXT_LEN, lsCheckCmd.Field ("OBD_APP_NAME").asString());
         snprintf(lText1, MAX_LOG_TEXT_LEN, lsCheckCmd.Field ("OBD_APPL_ID").asString());
         snprintf(lText2, MAX_LOG_TEXT_LEN, lsCheckCmd.Field ("DEST_NUM").asString());
         snprintf(lText3, MAX_LOG_TEXT_LEN, lsCheckCmd.Field ("FEEDBACK").asString());
         snprintf(lTemp ,MAX_LOG_TEXT_LEN,  "%-15s| %-10s| %-15s| %-10s| %-10s", lText4, lText1, lText2, lText3, lTime);
         fwrite( lTemp, sizeof(char) , strlen(lTemp), fpReqRespFile );
         fwrite( "\n", sizeof(char) ,1 , fpReqRespFile );
         fclose(fpReqRespFile);
      }

      if(lCount> 0)
      {
         if(lsUpCmdSet == false)
         {
            TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";
            lsUpCmd.setConnection (&mConn);
            snprintf (lSqlStr, MAX_SQL_STR_LEN,"UPDATE OBD_FEEDBACK SET READ_FLAG=1 WHERE RECORD_ID=:recId");
            lsUpCmd.setCommandText (lSqlStr);
            lsUpCmdSet = true;
         }
         lsUpCmd.Param ("recId").setAsLong () = lRecId;
         lsUpCmd.Execute ();
         INT16 lRowsAffected = lsUpCmd.RowsAffected() ;

         if(lRowsAffected > 0)
         {
            return DB_NO_ERR;
         }
         else
            return DB_UPDATE_FAILURE_ERR;

         return DB_NO_ERR;
      }
      else
         return DB_NO_RECORD_FOUND_ERR;

   }
   catch (SAException & e)
   {
      lsCmdSet = false;

      return ErrorHandler (e, (TEXT*) "FetchFeedbackRsp()", (TEXT*) "");
   }
}

//----------------------------------------------------------------------
// METHOD      : GetAppName
// DESCRIPTION : Reads Application name against short code
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::GetAppName (const TEXT * shortCode, INT16 callType, Application &app)
{
   static BOOLEAN lsCmdSet = false;
   static INT16   lsCallType = -1; 
   static SACommand lsCmd;
   
   T(gTrace, printf("%s: ShortCode/AppName:%s CallType:%d\n",
            gProcessName, shortCode, callType););
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdSet = false;
      return lDbErr;
   }

   if(lsCallType != callType)
   {
      lsCmdSet = false;
      lsCallType = callType;
   }

   try
   {
      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConn);
        // if(0 == callType)
         {
            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "SELECT a.APPL_NAME, a.APPL_ID, a.APPL_TYPE, a.TEST_CALL_CLI, a.STATUS, b.STATUS as ScSTATUS"
                  " FROM IVR_APPLICATION a, IVR_SHORT_CODE_MAST b "
                  " WHERE a.APPL_ID = b.APPL_ID and :shortCode like b.SHORT_CODE");
                  // modified fr Range based shortCodes for Voting Application
                  //" WHERE a.APPL_ID = b.APPL_ID and b.SHORT_CODE = :shortCode");
                  //" WHERE a.APPL_ID = b.APPL_ID and b.SHORT_CODE = :shortCode and b.STATUS = 0 ");

            //"SELECT APPL_NAME, APPL_TYPE, TEST_CALL_CLI, STATUS "
            //"  FROM IVR_APPLICATION "
            //" WHERE SHORT_CODE = :shortCode ");
         }
//         else
#if 0 
         {
            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "SELECT APPL_NAME, APPL_ID, APPL_TYPE, TEST_CALL_CLI, STATUS "
                  " FROM IVR_APPLICATION "
                  " WHERE APPL_NAME = :shortCode ");
         }
#endif
         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
      }

      lsCmd.Param ("shortCode").setAsString () = shortCode;
      lsCmd.Execute ();

      if (true == lsCmd.FetchNext ())
      {
         strcpy (app.appName, lsCmd.Field ("APPL_NAME").asString ());
         app.appType = lsCmd.Field ("APPL_TYPE").asShort ();
         app.appId = lsCmd.Field ("APPL_ID").asLong ();
         strcpy (app.testCli, lsCmd.Field ("TEST_CALL_CLI").asString ());
         app.status = lsCmd.Field ("STATUS").asShort ();
         app.scStatus = lsCmd.Field ("ScSTATUS").asShort ();
      }
      else
      {
         return DB_NO_RECORD_FOUND_ERR;
      }

      while(lsCmd.FetchNext ());
      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdSet = false;

      return ErrorHandler (e, (TEXT*) "GetAppName()", (TEXT*) "");
   }
}

//----------------------------------------------------------------------
// METHOD      : InsertEvr 
// DESCRIPTION : Inserts Evr into DB                        
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::InsertEvr (const IvrEvrMsg & lEvrMsg)
{
   static BOOLEAN lsCmdFlag = false;
   static SACommand lsCmd;
   static int lPrevDay = 0;
   struct tm * lpTmStruct;
   TEXT lSqlStr[MAX_SQL_STR_LEN + 1];

   lpTmStruct = localtime (&lEvrMsg.genTime);
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdFlag = false;
      return lDbErr;
   }

   if (lPrevDay != lpTmStruct->tm_mday)
   {
      lPrevDay = lpTmStruct->tm_mday;
      lsCmdFlag = false;
   }

   try
   {
      if (false == lsCmdFlag)
      {
         lsCmd.setConnection (&mConn);

         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "INSERT INTO %s_%02d%02d (evr_event, evr_cause, evr_reason, ref_id, short_code, request_mode, call_ref_num, cld_num, clg_num, content, duration, charge, gen_time, schedule_id,camp_id) VALUES (:event, :cause, :reason, :refid, :shortcode, :reqmode, :callrefnum, :cldnum, :clgnum, :content, :duration, :charge, :gentime, :shid, :cmpid) ",
               TABLE_IVR_EVR_DETAILS, lpTmStruct->tm_mday, lpTmStruct->tm_mon + 1);
         lsCmd.setCommandText (lSqlStr);
         lsCmdFlag = true;
      }

      lsCmd.Param ("event").setAsShort () = lEvrMsg.evrEvent;
      lsCmd.Param ("cause").setAsShort () = lEvrMsg.evrCause;
      lsCmd.Param ("reason").setAsULong () = lEvrMsg.evrReason;
      lsCmd.Param ("refid").setAsULong () = lEvrMsg.refId;
      lsCmd.Param ("gentime").setAsDateTime () = *lpTmStruct;
      lsCmd.Param ("reqmode").setAsShort () = lEvrMsg.reqMode;
      lsCmd.Param ("callrefnum").setAsString () = lEvrMsg.callRefNum;
      lsCmd.Param ("content").setAsString () = lEvrMsg.content;
      lsCmd.Param ("shortcode").setAsString () = lEvrMsg.shortCode;
      lsCmd.Param ("clgnum").setAsString () = lEvrMsg.clgNum;
      lsCmd.Param ("cldnum").setAsString () = lEvrMsg.cldNum;
      lsCmd.Param ("duration").setAsShort () = lEvrMsg.duration;
      lsCmd.Param ("charge").setAsShort () = lEvrMsg.charge;
      lsCmd.Param ("shid").setAsLong ()  = lEvrMsg.scheduleId;
      lsCmd.Param ("cmpid").setAsLong () = lEvrMsg.campId;

      lsCmd.Execute ();

      T (gTrace, printf ("%s: Evr Record Inserted For Msisdn:%s to %s_%02d%02d table\n",
               gProcessName, lEvrMsg.clgNum,
               TABLE_IVR_EVR_DETAILS, lpTmStruct->tm_mday, lpTmStruct->tm_mon + 1););

      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdFlag = false;
      return ErrorHandler (e,(TEXT*) "InsertEvr()", (TEXT*) "");
   }
   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : InsertCdr 
// DESCRIPTION : Inserts Cdr into DB                        
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::InsertCdr (const IvrCdrMsg & lCdrMsg)
{
   static BOOLEAN lsCmdFlag = false;
   static SACommand lsCmd;
   static int lPrevDay = 0;
   struct tm * lpTmStruct;
   TEXT lSqlStr[MAX_SQL_STR_LEN + 1];
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdFlag = false;
      return lDbErr;
   }

   lpTmStruct = localtime (&lCdrMsg.genTime);

   if (lPrevDay != lpTmStruct->tm_mday)
   {
      lPrevDay = lpTmStruct->tm_mday;
      lsCmdFlag = false;
   }

   try
   {
      if (false == lsCmdFlag)
      {
         lsCmd.setConnection (&mConn);

         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "INSERT INTO %s_%02d%02d (subs_no, subs_type, call_ref_num, service_type, content, duration, units, charge, gen_time) VALUES (:subsnum, :substype, :callrefnum, :servicetype, :content, :duration, :units, :charge, :gentime) ",
               TABLE_IVR_CDR_DETAILS, lpTmStruct->tm_mday, lpTmStruct->tm_mon + 1);
         lsCmd.setCommandText (lSqlStr);
         lsCmdFlag = true;
      }

      lsCmd.Param ("subsnum").setAsString () = lCdrMsg.subsNum;
      lsCmd.Param ("substype").setAsShort () = lCdrMsg.subsType;
      lsCmd.Param ("callrefnum").setAsString () = lCdrMsg.callRefNum;
      lsCmd.Param ("servicetype").setAsShort () = lCdrMsg.serviceType;
      lsCmd.Param ("content").setAsString () = lCdrMsg.content;
      lsCmd.Param ("duration").setAsShort () = lCdrMsg.duration;
      lsCmd.Param ("units").setAsShort () = lCdrMsg.units;
      lsCmd.Param ("charge").setAsShort () = lCdrMsg.charge;
      lsCmd.Param ("gentime").setAsDateTime () = *lpTmStruct;

      lsCmd.Execute ();

      T (gTrace, printf ("%s: Evr Record Inserted For Msisdn:%s to %s_%02d%02d table\n",
               gProcessName, lCdrMsg.subsNum,
               TABLE_IVR_EVR_DETAILS, lpTmStruct->tm_mday, lpTmStruct->tm_mon + 1););

      return DB_NO_ERR;
   }

   catch (SAException & e)
   {
      lsCmdFlag = false;
      return ErrorHandler (e,(TEXT*) "InsertCdr()", (TEXT*) "");
   }

   return DB_NO_ERR;
}

//----------------------------------------------------------------------
// METHOD      : CheckChannelNo 
// DESCRIPTION : Checks for the available channel                       
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::CheckChannelNo (Application &app)
{
   static BOOLEAN lsCmdSet = false;
   static SACommand lsCmd;
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdSet = false;
      return lDbErr;
   }

   T(gTrace, printf("%s: AppId:%d Channel:%d Type:%d\n",
            gProcessName, app.appId, app.channel, app.appType););
   
   try
   {
      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConn);
         if(SA_MySQL_Client == mDbClient)
         {
            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "SELECT STATUS FROM IVR_CHANNEL_CONFIG " 
                  "WHERE (APPL_ID = :appId OR APPL_ID IS NULL OR APPL_ID = \"-1\") "
                  "AND CHANNEL_NO = :channel AND CHANNEL_TYPE = :type and STATUS = 0");
         }
         else if(mDbClient == SA_PostgreSQL_Client)
         {
            snprintf(lSqlStr, MAX_SQL_STR_LEN,
                  "SELECT status FROM ivr_channel_config "
                  "WHERE (appl_id = :appId OR appl_id IS NULL OR appl_id = '-1') "
                  "AND channel_no = :channel AND channel_type = :type AND status = 0");
         }
         else
         {
            snprintf (lSqlStr, MAX_SQL_STR_LEN,
                  "SELECT STATUS FROM IVR_CHANNEL_CONFIG " 
                  "WHERE (APPL_ID = :appId OR APPL_ID IS NULL OR APPL_ID = '-1') "
                  "AND CHANNEL_NO = :channel AND CHANNEL_TYPE = :type and STATUS = 0");
         }

         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
      }

      lsCmd.Param ("appId").setAsLong () = app.appId;
      lsCmd.Param ("channel").setAsShort () = app.channel;
      lsCmd.Param ("type").setAsShort () = app.appType;
      lsCmd.Execute ();

      if (true == lsCmd.FetchNext ())
      {
         app.status = lsCmd.Field ("STATUS").asShort ();
      }
      else
      {
         return DB_NO_RECORD_FOUND_ERR;
      }

      while(lsCmd.FetchNext ());
      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdSet = false;

      return ErrorHandler (e, (TEXT*) "CheckChannelNo()" , (TEXT*) "");
   }
}

//----------------------------------------------------------------------
// METHOD      : ReadChannelConfig 
// DESCRIPTION : Reads the Channel Configuration                       
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::ReadChannelConfig(IvrChannelConfig &channel, EnumIVRChannelDirection dir)
{
   static SACommand lsCmd;
   INT16  lChannelNum = 0;
   TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      return lDbErr;
   }


   T(gTrace, printf("%s: Channel Direction:%d\n",
            gProcessName, dir););

   memset(&channel, 0, sizeof(IvrChannelConfig));
   try
   {
      lsCmd.setConnection (&mConn);

      if(IVR_CHANNEL_DIR_UNKNOWN == dir)
      {
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "SELECT a.CHANNEL_NO, a.CHANNEL_TYPE, a.APPL_ID, a.STATUS, b.APPL_NAME "
               "FROM IVR_CHANNEL_CONFIG a, IVR_APPLICATION b " 
               "WHERE a.STATUS = 0 AND a.APPL_ID = b.APPL_ID");
      }
      else
      {

         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "SELECT a.CHANNEL_NO, a.CHANNEL_TYPE, a.APPL_ID, a.STATUS, b.APPL_NAME "
               "FROM IVR_CHANNEL_CONFIG a, IVR_APPLICATION b " 
               "WHERE a.CHANNEL_TYPE = :dir AND a.STATUS = 0 AND a.APPL_ID = b.APPL_ID");
      }

      lsCmd.setCommandText (lSqlStr);

      //T(gTrace, printf("%s: ReadChannelConfig SQL: %s\n", gProcessName, lSqlStr););

      if(IVR_CHANNEL_DIR_UNKNOWN != dir)
         lsCmd.Param ("dir").setAsShort () = dir;

      lsCmd.Execute ();

      channel.channelCount = 0;

      while(lsCmd.FetchNext ())
      {
         lChannelNum = lsCmd.Field ("CHANNEL_NO").asLong();
         channel.details[lChannelNum].dir = (EnumIVRChannelDirection) lsCmd.Field ("CHANNEL_TYPE").asShort();
         //strcpy(channel.details[lChannelNum].shortCode, lsCmd.Field ("APPL_ID").asString());
         strcpy(channel.details[lChannelNum].appName, lsCmd.Field ("APPL_NAME").asString());
         channel.details[lChannelNum].appId = lsCmd.Field ("APPL_ID").asLong();
         channel.details[lChannelNum].status = (EnumIVRChannelStatus)lsCmd.Field ("STATUS").asShort ();
         channel.channelCount++;
      }

      if(channel.channelCount == 0)
      {
         return DB_NO_RECORD_FOUND_ERR;
      }

      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      return ErrorHandler (e, (TEXT*) "ReadChannelConfig()" , (TEXT*) "");
   }
}

//----------------------------------------------------------------------
// METHOD      : GetTotalIvrChannels 
// DESCRIPTION : Gets the total nuber of channels configured in IVR
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::GetTotalIvrChannels(INT16 &channelCount)
{
   static BOOLEAN lsCmdSet = false;
   static SACommand lsCmd;
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdSet = false;
      return lDbErr;
   }

   try
   {
      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConn);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,
               "SELECT MAX(CHANNEL_NO) AS CHANNEL_CNT FROM IVR_CHANNEL_CONFIG "); 

         lsCmd.setCommandText (lSqlStr);
         lsCmdSet = true;
      }

      lsCmd.Execute ();

      if(lsCmd.FetchNext ())
      {
         channelCount = lsCmd.Field ("CHANNEL_CNT").asLong ();
      }
      else
      {
         return DB_NO_RECORD_FOUND_ERR;
      }

      while(lsCmd.FetchNext ());
      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdSet = false;

      return ErrorHandler (e, (TEXT*)"GetTotalIvrChannels()", (TEXT*) "");
   }
}


//----------------------------------------------------------------------
// METHOD      : PostScheduleStatusUpdate 
// DESCRIPTION : Procedure to be invoked when schedule status gets 
//               updated.
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::PostScheduleStatusUpdate(UINT32 scheduleId)
{
   SACommand lsCmd;
   BOOLEAN   lsCmdSet=false;
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdSet = false;
      return lDbErr;
   }
   
   try
   {
      if (lsCmdSet == false)
      {
         lsCmd.setConnection (&mConn);
         lsCmd.setCommandText ("proc_schedule_status_update");

         //-------------- Set Output Parameters ------------------------
         lsCmd.Param ("o_result").setParamType (SA_dtLong);
         lsCmdSet = true;
      }
      //-------------- Set Input Parameters -------------------------
      lsCmd.Param ("i_schedule_id").setAsLong () = scheduleId;

      lsCmd.Execute ();

      //-------------- Get Output Parameters ------------------------
      UINT16 lResult  = lsCmd.Param ("o_result").asShort ();
      T(gTrace, printf("%s: proc_schedule_status_update : %d\n", gProcessName, lResult););
      //while(lsCmd.FetchNext());

      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e, (TEXT*) "PostScheduleStatusUpdate()", (TEXT*) "");
   }
}




//----------------------------------------------------------------------
// METHOD      : FetchScheduledReq
// DESCRIPTION : Fetches the scheduled request
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::FetchScheduledReqLoad(ScheduleReq lScheduleReq[IVR_NO_OF_SCHEDULE_RECORDS],
      INT16& lRetrySec, INT16& lMaxRetry, UINT32& lTotalCount, UINT32 noc, UINT32 lFetchCnt )
{
   static SACommand lsCmd;
   static SACommand lsCheckCmd;
   static SACommand lsUpCmd;
   static BOOLEAN lsCmdSet   = false;
   static BOOLEAN lsCheckCmdSet   = false;
   static BOOLEAN lsUpCmdSet = false;
   static INT16   lRtrySec   = lRetrySec;
   UINT32 lCount = 0;
   //TEXT   lParam[50] = "";
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsCmdSet = false;
      lsCheckCmdSet = false;
      lsUpCmdSet = false;
      return lDbErr;
   }

   try
   {
      //sleep(2);
      if (lsCheckCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCheckCmd.setConnection (&mConn);
         snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT COUNT(*) AS REC_COUNT "
               "FROM IVR_SCHEDULE WHERE SCHEDULE_TIME <= SYSDATE() AND DEL_ATTEMPTS <= 1 "
               "AND STATUS IN (1) AND END_TIME > SYSDATE()");
         //snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SCHEDULE_ID, SCHEDULE_TYPE,"
         //          "CALL_REF_NUM, REQUEST_MODE, RECIPIENT_NO, SENDER_NO, APPL_NAME "
         //          "FROM IVR_SCHEDULE WHERE SCHEDULE_TIME <= SYSDATE() AND DEL_ATTEMPTS < :attempt "
         //          "AND STATUS IN (1) AND END_TIME > SYSDATE() LIMIT :lrow"
         //         );

         T(gTrace, printf("%s: CheckSchedule Query: %s\n", gProcessName, lSqlStr););
         lsCheckCmd.setCommandText (lSqlStr);
         lsCheckCmd.setOption("HandleResult") = "store";
         lsCheckCmdSet = true;
      }
      lsCheckCmd.Execute ();

      BOOLEAN lFlg = false;
      while (true == lsCheckCmd.FetchNext ())
      {
         lFlg = true;
         lCount = lsCheckCmd.Field ("REC_COUNT").asLong ();
      }

      if (!lFlg)
         return DB_NO_RECORD_FOUND_ERR;

      if(lCount >= noc)
         return DB_NO_RECORD_FOUND_ERR;
      
      INT32  lRowFetchCnt = (noc-lCount);
      if ( lFetchCnt > IVR_NO_OF_SCHEDULE_RECORDS)
         lFetchCnt = 100;

      if ( lRowFetchCnt >  lFetchCnt)
         lRowFetchCnt = lFetchCnt;

      lCount=0;
      lTotalCount = 0;

      if (lsCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsCmd.setConnection (&mConn);
         snprintf (lSqlStr, MAX_SQL_STR_LEN, "SELECT SCHEDULE_ID, SCHEDULE_TYPE,"
               "CALL_REF_NUM, REQUEST_MODE, RECIPIENT_NO, SENDER_NO, APPL_NAME, "
               "APP_NOP, APP_PARAM1, APP_PARAM2, APP_PARAM3, APP_PARAM4, APP_PARAM5 "
               "FROM IVR_SCHEDULE WHERE SCHEDULE_TIME <= SYSDATE() AND DEL_ATTEMPTS <= 1 "
               "AND STATUS IN (0,3) AND END_TIME > SYSDATE() "
               "AND SCHEDULE_ID NOT IN (SELECT SCHEDULE_ID FROM IVR_SCHEDULE WHERE STATUS =1) "
               "LIMIT :lrow"
             );


         T(gTrace, printf("%s: FetchSchedule Query: %s\n", gProcessName, lSqlStr););
         lsCmd.setCommandText (lSqlStr);
         lsCmd.setOption("HandleResult") = "store";
         lsCmdSet = true;
      }

      lsCmd.Param ("lrow").setAsLong ()    = lRowFetchCnt; //IVR_NO_OF_SCHEDULE_RECORDS;
      lsCmd.Execute ();

      while (true == lsCmd.FetchNext ())
      {
         lTotalCount++;
         lScheduleReq[lCount].scheduleId = lsCmd.Field ("SCHEDULE_ID").asLong ();
         lScheduleReq[lCount].type       = (EnumScheduleType)lsCmd.Field ("SCHEDULE_TYPE").asShort ();
         lScheduleReq[lCount].mode       = (EnumReqMode)lsCmd.Field ("REQUEST_MODE").asShort ();
         strcpy(lScheduleReq[lCount].shortCode, lsCmd.Field ("APPL_NAME").asString());
         strcpy(lScheduleReq[lCount].origAddr , lsCmd.Field ("SENDER_NO").asString());
         strcpy(lScheduleReq[lCount].destAddr , lsCmd.Field ("RECIPIENT_NO").asString());
         lScheduleReq[lCount].varCount = lsCmd.Field ("APP_NOP").asShort();
         strcpy(lScheduleReq[lCount].var[0] , lsCmd.Field ("APP_PARAM1").asString());
         strcpy(lScheduleReq[lCount].var[1] , lsCmd.Field ("APP_PARAM2").asString());
         strcpy(lScheduleReq[lCount].var[2] , lsCmd.Field ("APP_PARAM3").asString());
         strcpy(lScheduleReq[lCount].var[3] , lsCmd.Field ("APP_PARAM4").asString());
         strcpy(lScheduleReq[lCount].var[4] , lsCmd.Field ("APP_PARAM5").asString());

         T(gTrace, printf("%s: DB Schedule Request :(%d): %s, %s, %s\n",gProcessName, lCount, lScheduleReq[lCount].shortCode, lScheduleReq[lCount].origAddr, lScheduleReq[lCount].destAddr););
         lCount++;
      }

      for ( int lIndex = 0; lIndex < lTotalCount; lIndex++)
      {
         T(gTrace, printf("%s: Total Schedule requests : %d\n", gProcessName, lTotalCount););
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";
         lsUpCmd.setConnection (&mConn);
         snprintf (lSqlStr, MAX_SQL_STR_LEN,"UPDATE IVR_SCHEDULE SET DEL_ATTEMPTS=1,"
               " STATUS=1 WHERE SCHEDULE_ID=:shid");
         lsUpCmd.setCommandText (lSqlStr);
         lsUpCmd.Param ("shid").setAsLong () = lScheduleReq[lIndex].scheduleId;
         lsUpCmd.Execute ();
         INT16 lRowsAffected = lsUpCmd.RowsAffected() ;

         if(lRowsAffected <= 0)
         {
            T(gTrace, printf("%s: Status update failed for SchId: %d\n", gProcessName, lScheduleReq[lIndex].scheduleId););
            lsUpCmdSet = false;
         }
      }
      
      return DB_NO_ERR;
   }
   catch (SAException & e)
   {
      lsCmdSet = false;
      return ErrorHandler (e,(TEXT*) "FetchScheduledReq()" , (TEXT*) "");
   }
}


//----------------------------------------------------------------------
// METHOD      : UpdateScheduledRspLoad 
// DESCRIPTION : Fetches the scheduled request 
// PARAMETER   :
// RETURN      : DbErrorCodes - Success/failure status
//----------------------------------------------------------------------
DbErrorCodes IvrDbManager::UpdateScheduledRspLoad(ScheduleRsp& lScheduleRsp)
{
   SACommand lsUpCmd;
   BOOLEAN   lsUpCmdSet   = false;
   struct tm * lpCallTime;
   UINT32 duration=0;
   
   DbErrorCodes lDbErr = CheckDBStatus();
   if (DB_NO_ERR != lDbErr)
   {
      lsUpCmdSet = false;
      return lDbErr;
   }

   try
   {
      if (lsUpCmdSet == false)
      {
         TEXT lSqlStr[MAX_SQL_STR_LEN + 1] = "";

         lsUpCmd.setConnection (&mConn);
         snprintf (lSqlStr, MAX_SQL_STR_LEN, "UPDATE IVR_SCHEDULE SET STATUS=0, DEL_ATTEMPTS =0, ERROR_ID=:err,"
                   " CALL_REF_NUM=:ref, SCHEDULE_TIME = DATE_ADD(SYSDATE(), INTERVAL 1 SECOND), "
                   " CALL_TIME=:callTime, "
                   " DURATION=:duration "
                   " WHERE SCHEDULE_ID = :shid AND APPL_NAME=:shcode");
                    
         lsUpCmd.setCommandText (lSqlStr);
         lsUpCmdSet = true;
      }

      lpCallTime = localtime(&lScheduleRsp.callTime);
      if(lScheduleRsp.answerTime > 0)
         duration = (lScheduleRsp.disconnectTime - lScheduleRsp.answerTime);

      //lsUpCmd.Param ("stat").setAsShort ()    = lScheduleRsp.status;
      lsUpCmd.Param ("err").setAsLong ()      = lScheduleRsp.errorCode;
      lsUpCmd.Param ("ref").setAsString ()    = lScheduleRsp.callRefNum;
      lsUpCmd.Param ("shid").setAsLong ()     = lScheduleRsp.scheduleId;
      lsUpCmd.Param ("shcode").setAsString () = lScheduleRsp.shortCode;
      lsUpCmd.Param ("callTime").setAsDateTime() = *lpCallTime;
      lsUpCmd.Param ("duration").setAsLong ()      = duration;
      lsUpCmd.Execute ();

      INT16 lRowsAffected = lsUpCmd.RowsAffected() ;

      if(lRowsAffected > 0)
      {
         //while(lsUpCmd.FetchNext());
         //this->PostScheduleStatusUpdate(lScheduleRsp.scheduleId);
         return DB_NO_ERR;
      }
      else
         return DB_UPDATE_FAILURE_ERR;
   }
   catch (SAException & e)
   {
      lsUpCmdSet = false;

      return ErrorHandler (e,(TEXT*) "UpdateScheduledRsp()", (TEXT*) "");
   }

}
