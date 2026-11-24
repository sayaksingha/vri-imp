static const char* id="@(#) $Id: IvrPegWriter.cc,v 1.1.2.1 2012/01/03 10:35:08 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// VxmlProcessor.cc
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// CLASS TYPE
//
// DESCRIPTION
//
// Originated : KOTHARI KOAML                           DATE:28-MAY-2011
//----------------------------------------------------------------------

#include "IvrUtil.h"
#include "IvrQueApi.h"

#include "limits.h"

TEXT 				gProcessName[IVR_MAX_PROCESS_NAME + 1] = "";
Log 				gLog;
CTrace 			gTrace (TRACE_IVR_VXML_ENV);
TEXT 				gLogText[MAX_LOG_TEXT_LEN + 1] = "";
INT32          gReferenceNum;
HealthCheckApi gHealthCheckObj;

IvrQueApi      gIvrQueApi;

//-------------------------------------------------------------
// METHOD      : Init
// DESCRIPTION :
// PARAMETER   : None
// RETURN      : BOOLEAN
//-------------------------------------------------------------
BOOLEAN Init ()
{
	TEXT lLogText[MAX_LOG_TEXT_LEN + 1] = "";
	memset (lLogText, 0, sizeof (lLogText));

	if (false ==  gIvrQueApi.Init())
	{
		snprintf (lLogText, MAX_LOG_TEXT_LEN, "Queue API Init failed. Exiting...");
		gLog.GenerateLog (GSYS04, lLogText);
		TERR (gTrace, printf ("%s: %s\n", gProcessName, lLogText););
		return false;
	}

	return true;
}

//-------------------------------------------------------------
// METHOD      :
// DESCRIPTION :
// PARAMETER   :
// RETURN      :
//-------------------------------------------------------------
int main(int argc, char **argv)
{
	ProcessLock lProcessLock;
   IvrPeg      ivrPeg;
	strcpy (gProcessName, basename (argv[0]));

	if (argc != 2)
	{
		printf ("Usage %s <instance number>\n", gProcessName);
		printf ("Trace: export %s=1\n", TRACE_IVR_PEG_WRITER_ENV);
		return 1;
	}

	IvrUtil::LoadSignals ();

	// setting name of executable for log
	gLog.SetProcessName (gProcessName);

	printf ("%s: Starting...\n", gProcessName);

	// Initialize log object
	if (true != gLog.Init ())
	{
		printf("%s:\33[32m Failed to initialize Log Obj. Exiting...\33[0m\n", gProcessName);
		return 1;
	}

	lProcessLock.SetProdProcName (IVR_NAME, gProcessName);
	if (LOCK_SUCCESS != lProcessLock.Lock ())
	{
		gLog.GenerateLog (GSYS16);
		printf ("%s: Process already running. Exiting...\n", gProcessName);
		return 1;
	}

	// Startup log
	gLog.GenerateLog (GSYS01);

	// Set Trace Key
	gTrace.SetTraceKey (TRACE_IVR_PEG_WRITER_ENV);

	if (false == Init ())
		return 1;

   while (IvrUtil::KeepRunning ())
   {
      if (true == gIvrQueApi.ReadFromIvrPegQueue(ivrPeg))
      {
         gLog.PegEvent (ivrPeg.eventCode); //, ivrPeg.action, ivrPeg.count);
      }
      else
      {
         sleep(1);
      }
   }// end of while loop (main process)

   printf ("%s: Shutting down...\n", gProcessName);
   gLog.GenerateLog (GSYS02);

   return 0;
}
