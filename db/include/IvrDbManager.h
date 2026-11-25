// @(#) $Id: IvrDbManager.h,v 1.1.1.1.2.5.2.1 2017/06/03 02:41:49 mram Exp $
//-----------------------------------------------------------------------------
// NAME           : IvrDbManager.h
//
// COPYRIGHT      : TSS -- Copyright (C) 2011 Tayana Software Solutions Pvt Ltd.,
//                  All rights reserved. No part of this computer program
//                  may be used or reproduced in any form by any
//                  means without prior written permission of
//                  Tayana Software Solutions Pvt Ltd.
//
// Originated     : Divya K                               Dated : 09-FEB-2011
//-----------------------------------------------------------------------------

#ifndef IVR_DBMGR_H
#define IVR_DBMGR_H

#include <vector>

#include "SQLAPI.h"
#include "IvrStructs.h"
#include "IvrLimits.h"
#include "IvrConstDef.h"
#include "IvrUtil.h"
#include "TssDbMgr.h"
#include <stdlib.h>

using namespace std;

#define MAX_DB_USER  		TSS_MAX_DB_USER_LEN         
#define MAX_DB_PASS             TSS_MAX_DB_PASS_LEN
#define MAX_SQL_STR_LEN		TSS_MAX_SQL_STR_LEN

//#define DB_USER               "ivr"
//#define DB_PASS               "ivr"
//#define DB_NAME               "BPLORA"

class  IvrDbManager
{
	public:

		IvrDbManager();
		~IvrDbManager();

		TEXT                    mDbName[IVR_MAX_DB_NAME_LEN];                  // Database Name
		TEXT                    mDbUser[MAX_DB_USER];                  // User Name
		TEXT                    mDbPass[MAX_DB_PASS];                  // Password
      SAClient_t              mDbClient;

		DbErrorCodes   Connect(int lDbClient);

      DbErrorCodes   Init(int lDbClient);

		DbErrorCodes    ReInit();

		DbErrorCodes   Init(TEXT* lDbName, TEXT* lUser, TEXT* lPasswd, int lDbClient);

		DbErrorCodes   Disconnect();

		DbErrorCodes    CheckDBStatus();

		void            CommitNow();
		
		DbErrorCodes    ErrorHandler(SAException&  err,TEXT*  funcName,TEXT* debugInfo = "", BOOLEAN genTrace=true);

		DbErrorCodes    ExecuteQuery(EnumQueryType& lQtype, TEXT* lQuery, 
				                       TEXT lVars[][MAX_VARIABLE_LEN], int& varCnt, TEXT*  ErrMsg,
											  int& lRowsFetched);

		DbErrorCodes    ExecuteQuery1(EnumQueryType& lQtype, TEXT* lQuery, 
				                       TEXT lVars[][MAX_VARIABLE_LEN], int& varCnt, TEXT*  ErrMsg,
											  int& lRowsFetched);

      DbErrorCodes ReadCfgNum (const TEXT * key, INT32 & Value, INT32 minVal, INT32 maxVal);

      DbErrorCodes ReadCfgStr (const TEXT * key, TEXT * Value, INT32 minVal, INT32 maxVal);

      DbErrorCodes    FetchScheduledReq(ScheduleReq lScheduleReq[100], INT16& lRetrySec, 
                                        INT16& lMaxRetry, UINT32& lTotalCount, UINT32 noc = 1 );
      
      DbErrorCodes    FetchScheduledReqLoad(ScheduleReq lScheduleReq[100], INT16& lRetrySec, 
                                        INT16& lMaxRetry, UINT32& lTotalCount, UINT32 noc = 1 , UINT32 lFetchCnt = 15);
      
      DbErrorCodes    UpdateScheduledRsp(ScheduleRsp& );
      
      DbErrorCodes    UpdateScheduledRspLoad(ScheduleRsp& );
      
      DbErrorCodes    PostScheduleStatusUpdate(UINT32 scheduleId);

      DbErrorCodes    FetchChannelCmd( ChannelMgmt lChannelMgmt[MAX_NUM_OF_CKT_CMDS], UINT32& lMinChannelId,
                              UINT32& lMaxChannelId);

      DbErrorCodes    FetchFeedbackRsp();

      DbErrorCodes    InsertEvr (const IvrEvrMsg &);

      DbErrorCodes    InsertCdr (const IvrCdrMsg &);

      DbErrorCodes    GetAppName (const TEXT * , INT16, Application&);

      DbErrorCodes    CheckChannelNo(Application&);

      DbErrorCodes    GetTotalIvrChannels(INT16 &);

      DbErrorCodes    ReadChannelConfig(IvrChannelConfig &channel, EnumIVRChannelDirection dir);
      
   private:
      SAConnection    mConn;
		SACommand       mCommand;

		TEXT            mLogText[MAX_LOG_TEXT_LEN];            // Log Text
		TEXT            mSqlStr[MAX_SQL_STR_LEN];              // Sql String
};

#endif

