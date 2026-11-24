static const char* id="@(#) $Id: CcxmlProcessorMain.cc,v 1.1.1.1.2.3.2.1 2016/05/12 00:54:40 mram Exp $" ;
//----------------------------------------------------------------------
// NAME
// CcxmlProcessorMain.cc
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
// Originated: Divya K                        Date: DEC-2010
//----------------------------------------------------------------------

#include "CcxmlProcessor.h"

Log              gLog;
CTrace           gTrace (TRACE_IVR_CCXML_ENV);
TEXT             gProcessName[MAX_PROCESS_NAME + 1] = "";
//HealthCheckApi   gHealthCheckObj;
INT32            gReferenceNum;

//-------------------------------------------------------------
// METHOD      : 
// DESCRIPTION :
// PARAMETER   :
// RETURN      : 
//-------------------------------------------------------------
int main(int argc, char **argv)
{
	CcxmlRequest     lccxmlrequest;
	ProcessLock      lProcessLock;
	CcxmlProcessor   lCcxmlProcessorObj;

	if (argc != 2)
	{
		printf ("Usage %s <Instance> \n", gProcessName);
	   printf ("Trace: export %s=1\n", TRACE_IVR_CCXML_ENV);
		return 1;
	}
	else
	{
		//strcpy (gProcessName, basename(argv[0]));
		//strcat (gProcessName, argv[1]);
		sprintf(gProcessName, "IVR_CP%d", atoi(argv[1]));
	}

   //mtrace(); //TBC

   IvrUtil::LoadSignals ();

   gLog.SetProcessName (gProcessName);
   printf ("%s: Starting...\n", gProcessName);

	// Initialize log object
	if (true != gLog.Init ())
	{
		printf ("%s: \33[31mCould not initialize log object. Exiting...\33[0m\n",
				gProcessName);
		return 1;
	}

	lProcessLock.SetProdProcName ((TEXT*)IVR_NAME, gProcessName);
	if (LOCK_SUCCESS != lProcessLock.Lock ())
	{
		gLog.GenerateLog (GSYS16);
		printf ("%s: \33[31mProcess already running. Exiting...\33[0m\n", 
				gProcessName);
		return 1;
	}

	// Startup log
	gLog.GenerateLog (GSYS01);

	// Set Trace Key
	gTrace.SetTraceKey (TRACE_IVR_CCXML_ENV);

	//Initialise Evr Writer
	if (false == lCcxmlProcessorObj.Init ())
		return 1;

	gLog.GenerateLog (GSYS03);
	printf ("%s: \33[32mInitialization Okay...\33[0m\n", gProcessName);

	while (IvrUtil::KeepRunning ())
	{
		if (IvrUtil::ReloadConfig ())
		{
			//ReloadConfig ();
			IvrUtil::ResetConfigFlag ();
		}

		if (true == lCcxmlProcessorObj.ReadRequest (lccxmlrequest))
		{
			/*sprintf(gProcessName, "IVR_CCXML_%d", mCcxmlRequest.channelId);
			strcpy(argv[0], gProcessName);*/
			lCcxmlProcessorObj.BuildRequest(lccxmlrequest.ccxmlUrlRequest.url);
		}
	}

   //muntrace();  //TBC

	printf ("%s: \33[31mShutting down...\33[0m\n", gProcessName);
	gLog.GenerateLog (GSYS02);

	return 0;
}

