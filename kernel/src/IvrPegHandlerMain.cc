static const char* id="@(#) $Id: IvrPegHandlerMain.cc,v 1.1.1.1.2.3 2016/04/12 21:59:19 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// IvrPegHandlerMain.cc
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

Log              gLog;
CTrace           gTrace (TRACE_IVR_PEG_HDLR_ENV);
TEXT             gProcessName[MAX_PROCESS_NAME + 1] = "";
//HealthCheckApi   gHealthCheckObj;

//-------------------------------------------------------------
// METHOD      : main()
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
int main(int argc, char **argv)
{
	ProcessLock          lProcessLock;
	IvrPegHandler        lIvrPegHandlerObj;

	strcpy(gProcessName, basename(argv[0]));

	if (argc > 1)
	{
		printf("\33[31mInstance number to this process not allowed.\33[0m\n");
	   printf("%s: TRACE: export %s=1\n", gProcessName, TRACE_IVR_PEG_HDLR_ENV);
		exit(1);
	}

	IvrUtil::LoadSignals();
	gLog.SetProcessName(gProcessName);

	printf("%s: Starting...\n", gProcessName);

	if(gLog.Init() == false)
	{
		printf("%s: \33[31mCould not initialize log object. Exiting...\33[0m\n", gProcessName);
		return 0;
	}

	// Startup log
	gLog.GenerateLog(GSYS01);

	lProcessLock.SetProdProcName((TEXT*)IVR_NAME, gProcessName);
	if(lProcessLock.Lock() != LOCK_SUCCESS)
	{
		printf("%s: \33[31mProcess already running. Exiting...\33[0m\n", gProcessName);
		gLog.GenerateLog(GSYS16, gProcessName);
		return 0;
	}

	gTrace.SetTraceKey(TRACE_IVR_PEG_HDLR_ENV);

	//Init
	if( false == lIvrPegHandlerObj.Init())
		return false;

	printf("%s: \33[32mInitialization Okay...\33[0m\n", gProcessName);
	gLog.GenerateLog(GSYS03);

	timeval           ltimeval;
	struct   tm*      ltm;
	UINT32            lSleepTime;

	gettimeofday(&ltimeval, NULL);
	ltm = localtime(&(ltimeval.tv_sec));
	lSleepTime = 1000000 - ltimeval.tv_usec;
	usleep(lSleepTime);

	BOOLEAN  lFiveSecTrafficFlag = true;
	BOOLEAN  lTenSecTrafficFlag  = true;
	BOOLEAN  lOneSecTrafficFlag  = true;

	lIvrPegHandlerObj.InitTrafficData();

	lFiveSecTrafficFlag = false;
	lTenSecTrafficFlag  = false;
	lOneSecTrafficFlag  = false;

	while(IvrUtil::KeepRunning())
	{
		lOneSecTrafficFlag = true;
		lIvrPegHandlerObj.GetCurrentSecTraffic(lOneSecTrafficFlag, lFiveSecTrafficFlag, lTenSecTrafficFlag);
		lOneSecTrafficFlag = false;

		if(ltm->tm_sec % 5 == 0)
		{
			lFiveSecTrafficFlag = true;
			lIvrPegHandlerObj.GetCurrentSecTraffic(lOneSecTrafficFlag, lFiveSecTrafficFlag, lTenSecTrafficFlag);
			lFiveSecTrafficFlag = false;
		}

		if(ltm->tm_sec % 10 == 0)
		{
			lTenSecTrafficFlag  = true;
			lIvrPegHandlerObj.GetCurrentSecTraffic(lOneSecTrafficFlag, lFiveSecTrafficFlag, lTenSecTrafficFlag);
			lTenSecTrafficFlag  = false;
		}

		lIvrPegHandlerObj.LoadShm ();

		gettimeofday(&ltimeval, NULL);
		ltm = localtime(&(ltimeval.tv_sec));
		lSleepTime = 1000000 - ltimeval.tv_usec;
		usleep(lSleepTime);
	}

	lIvrPegHandlerObj.DeInit();
	printf("%s: \33[31mShutting down... \33[0m\n", gProcessName);
	gLog.GenerateLog(GSYS02);

	return 0;
}

