static const char* id="@(#) $Id: IvrPegHandler.cc,v 1.1.1.1.2.2 2012/02/21 08:28:16 shijuc Exp $";
//----------------------------------------------------------------------
// NAME
// IvrPegHandler.cc
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright (C) 2005
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated: Divya K                        Date: FEB-2011
//----------------------------------------------------------------------
#include "IvrPegHandler.h"

//-------------------------------------------------------------
// METHOD      : Constructor 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//-------------------------------------------------------------
IvrPegHandler::IvrPegHandler()
{
	mPegData                = (EventInfo*) malloc(sizeof(EventInfo));
	mPreviousOneSecPegData  = (EventInfo*) malloc(sizeof(EventInfo));
	mPreviousFiveSecPegData = (EventInfo*) malloc(sizeof(EventInfo));
	mPreviousTenSecPegData     = (EventInfo*) malloc(sizeof(EventInfo));

	memset(mPreviousOneSecData, 0, sizeof(mPreviousOneSecData));
	memset(mPreviosFiveSecData, 0, sizeof(mPreviosFiveSecData));
	memset(mPreviousTenSecData, 0, sizeof(mPreviousTenSecData));
	
	memset(mTotalOneSecData , 0, sizeof(mTotalOneSecData));
	memset(mTotalFiveSecData, 0, sizeof(mTotalFiveSecData));
	memset(mTotalTenSecData , 0, sizeof(mTotalTenSecData));
}

//-------------------------------------------------------------
// METHOD      : Destructor
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//-------------------------------------------------------------
IvrPegHandler::~IvrPegHandler()
{
}

//-------------------------------------------------------------
// METHOD      : Init()
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - success / failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::Init()
{

   if ( false == ReadConfig())
	{
		return false;
	}

	if ( false == CreateIpc())
	{
		return false;
	}

	return true;
}

//-------------------------------------------------------------
// METHOD      : DeInit()
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - success / failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::DeInit()
{
	free(mPegData);
	free(mPreviousOneSecPegData);
	free(mPreviousFiveSecPegData);
	free(mPreviousTenSecPegData);

	return true;
}

//-------------------------------------------------------------
// METHOD      : ReadConfig
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - Success / Failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::ReadConfig ()
{
	TEXT            lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	CfgRead         lCfgRead(gProcessName);

	if (false == lCfgRead.CfgInit(IVR_IPC_CFG))
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,  "%s File not found" , IVR_IPC_CFG);
		TERR(gTrace, printf("\n%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(GSYS09 , lLogText);
		return false;
	}

	if (lCfgRead.GetConfigNum( "SHM_MIS_PEG_KEY", mPegShmQKey , 
				                  IVR_MIN_SHM_KEY, IVR_MAX_SHM_KEY)!= CFG_OK)
	{
		snprintf( lLogText,MAX_LOG_TEXT_LEN, 
				    "ConfigRead failed for SHM_MIS_PEG_KEY in %s", IVR_IPC_CFG);
		TERR(gTrace, printf("\n%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(GSYS11, lLogText);
		lCfgRead.CfgDeInit();
		return false;
	}
	else
	{
		T(gTrace, printf("%s: SHM_MIS_PEG_KEY = %ld\n", gProcessName,mPegShmQKey););
	}

	if (lCfgRead.GetConfigNum( "SHM_TRAFFIC_PEG_KEY", mPegTrafficShmQKey , 
				                  IVR_MIN_SHM_KEY, IVR_MAX_SHM_KEY)!= CFG_OK)
	{
		snprintf( lLogText,MAX_LOG_TEXT_LEN, 
				    "ConfigRead failed for SHM_TRAFFIC_PEG_KEY in %s", IVR_IPC_CFG);
		TERR(gTrace, printf("\n%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(GSYS11, lLogText);
		lCfgRead.CfgDeInit();
		return false;
	}
	else
	{
		T(gTrace, printf("%s: SHM_TRAFFIC_PEG_KEY = %ld\n", 
					gProcessName,mPegTrafficShmQKey););
	}

	lCfgRead.CfgDeInit();
	return true;
}

//-------------------------------------------------------------
// METHOD      : CreateIpc 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - Success / Failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::CreateIpc ()
{
	BOOLEAN  	lShmCreatedForFirstTime = true;
	TEXT        lLogText[MAX_LOG_TEXT_LEN + 1] = "";

	mMaxShmSize     =  sizeof(EventInfo);

	INT32 lShmKey = mPegShmQKey;
	mShmId = shmget(lShmKey, mMaxShmSize, IPC_PERM);

	if (mShmId < 0)
	{
		//Shared memory should be created for the first time
		mShmId = shmget(lShmKey, mMaxShmSize, IPC_CREAT|IPC_PERM);

		if (mShmId < 0)
		{
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"Failed to create shared memory. ShmId: %ld ShmKey: %ld Size:%ld",
					mShmId, lShmKey, mMaxShmSize);
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(GSYS04, lLogText);
			return false;
		}
		else
		{
			lShmCreatedForFirstTime = true;
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"Shared memory created. ShmId: %ld ShmKey: %ld",
					mShmId, lShmKey);
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(PEG01, lLogText);
		}
	}
	else
	{
		//Shared memory already exists
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"Shared memory already exists. ShmId:%ld ShmKey:%ld",
				mShmId, lShmKey);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(PEG02, lLogText);
	}

	mOnlinePegData =  (EventInfo*) shmat(mShmId,(void*)0,0);

	if (mOnlinePegData == NULL)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"Failed to attach SHM ShmId:%ld ShmKey:%ld",
				mShmId, lShmKey);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(GSYS04, lLogText);

		return false;
	}

	/*if (lShmCreatedForFirstTime == true)
	{
		memset(mOnlinePegData, 0, mMaxShmSize);
	}*/

	//Shared Memory 
	lShmKey = mPegTrafficShmQKey;
	mMaxDispShmSize = MAX_ONLINE_TRAFFIC_DATA;

	memset(&mShmId, 0, sizeof(mShmId));
	mShmId = shmget(lShmKey, mMaxDispShmSize, IPC_PERM);

	if (mShmId < 0)
	{
		//Shared memory should be created for the first time
		mShmId = shmget(lShmKey, mMaxDispShmSize, IPC_CREAT|IPC_PERM);

		if (mShmId < 0)
		{
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"Failed to create shared memory. ShmId: %ld ShmKey: %ld Size:%ld",
					mShmId, lShmKey, mMaxDispShmSize);
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(GSYS04, lLogText);
			return false;
		}
		else
		{
			lShmCreatedForFirstTime = true;
			snprintf(lLogText, MAX_LOG_TEXT_LEN,
					"Shared memory created. ShmId: %ld ShmKey: %ld",
					mShmId, lShmKey);
			TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
			gLog.GenerateLog(PEG01, lLogText);
		}
	}
	else
	{
		//Shared memory already exists
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"Shared memory already exists. ShmId:%ld ShmKey:%ld",
				mShmId, lShmKey);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(PEG02, lLogText);
	}

	mOnlineData =  (TEXT*) shmat(mShmId,(void*)0,0);

	if (mOnlineData == NULL)
	{
		snprintf(lLogText, MAX_LOG_TEXT_LEN,
				"Failed to attach SHM ShmId:%ld ShmKey:%ld",
				mShmId, lShmKey);
		TERR(gTrace, printf("%s: %s\n", gProcessName, lLogText););
		gLog.GenerateLog(GSYS04, lLogText);

		return false;
	}

	if (lShmCreatedForFirstTime == true)
	{
		memset(mOnlineData, 0, mMaxDispShmSize);
	}

	return true;
}

//-------------------------------------------------------------
// METHOD      : PopulateShm ()
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - Success / Failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::PopulateShm ()
{
	memcpy(mPegData, mOnlinePegData, mMaxShmSize);
	return true;
}

//-------------------------------------------------------------
// METHOD      : InitTrafficData 
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - Success / Failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::InitTrafficData()
{
   PopulateShm ();
	memcpy(mPreviousOneSecPegData, mOnlinePegData, mMaxShmSize);
	memcpy(mPreviousFiveSecPegData, mOnlinePegData, mMaxShmSize);
	memcpy(mPreviousTenSecPegData, mOnlinePegData, mMaxShmSize);

	return true;
}

//-------------------------------------------------------------
// METHOD      : GetCurrentSecTraffic
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - Success / Failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::GetCurrentSecTraffic(BOOLEAN&  lOneSecTrafficFlag, 
		                                      BOOLEAN& lFiveSecTrafficFlag, BOOLEAN& lTenSecTrafficFlag)
{

	PopulateShm ();

	if(lOneSecTrafficFlag == true)
	{
		ConvertToTrafficShmData(mPegData, mPreviousOneSecPegData, mOneSecData);
		TotalTrafficShmData(mPegData, mPreviousOneSecPegData, mTotalOneSecData);
		memcpy(mPreviousOneSecPegData, mPegData, mMaxShmSize);
		strcpy(mPreviousOneSecData, mOneSecData);
      //T(gTrace, printf("01Sec%s\n", mOneSecData););
	}

	if(lFiveSecTrafficFlag == true)
	{
		ConvertToTrafficShmData(mPegData, mPreviousFiveSecPegData, mFiveSecData);
		TotalTrafficShmData(mPegData, mPreviousOneSecPegData, mTotalFiveSecData);
		memcpy(mPreviousFiveSecPegData, mPegData, mMaxShmSize);
		strcpy(mPreviosFiveSecData, mFiveSecData);
      //T(gTrace, printf("05Sec%s\n", mFiveSecData););
	}

	if(lTenSecTrafficFlag == true)
	{
		ConvertToTrafficShmData(mPegData, mPreviousTenSecPegData, mTenSecData);
		TotalTrafficShmData(mPegData, mPreviousOneSecPegData, mTotalTenSecData);
		memcpy(mPreviousTenSecPegData, mPegData, mMaxShmSize);
		strcpy(mPreviousTenSecData, mTenSecData);
      T(gTrace, printf("10Sec%s\n", mTenSecData););
	}

	return true;
}

//-------------------------------------------------------------
// METHOD      : LoadShm
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN - Success / Failure
//-------------------------------------------------------------
BOOLEAN IvrPegHandler::LoadShm ()
{
	TEXT    lTempData[50000] = "";

	sprintf(mOnlineData, "{[%s%s][%s%s][%s%s]}", mOneSecData, mTotalOneSecData, 
			     mFiveSecData, mTotalFiveSecData, mTenSecData, mTotalTenSecData);

	return true;
}

//-------------------------------------------------------------
// METHOD      : ConvertToTrafficShmData
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//-------------------------------------------------------------
void IvrPegHandler::ConvertToTrafficShmData(EventInfo* lData, EventInfo* lPreviouData, TEXT* lTrafficData)
{
	TEXT      lFormattedData[1000] = "";
   UINT32    lValue;

	strcpy(lTrafficData, "|");
	for(int Indx = 1; Indx <= MAX_PEG_EVENTS; Indx++)
	{
		lValue = lData->OnlinePegCount[Indx][0] - lPreviouData->OnlinePegCount[Indx][0];

		sprintf(lFormattedData, "%ld:", lValue);
		strcat(lTrafficData, lFormattedData);
	}
	strcat(lTrafficData, "|");
}

//-------------------------------------------------------------
// METHOD      : TotalTrafficShmData
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : None
//-------------------------------------------------------------
void IvrPegHandler::TotalTrafficShmData(EventInfo* lData, EventInfo* lPreviouData, TEXT* lTrafficData)
{
	TEXT      lFormattedData[1000] = "";
	
	strcpy(lTrafficData, "|");

	for(int Indx = 1; Indx <= MAX_PEG_EVENTS; Indx++)
	{
		sprintf(lFormattedData, "%ld:", lData->OnlinePegCount[Indx][0]);
		strcat(lTrafficData, lFormattedData);
	}
	strcat(lTrafficData, "|");
}


