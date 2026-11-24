// @(#) $Id
//----------------------------------------------------------------------
// NAME
// CcxmlProcessor.h
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

#include "Types.h"
#include <stdio.h>
#include <OktoClient.h>
//#include "Log.h"
#include "LogApi.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ConfigInfo.h"
#include "ProcessLock.h"
#include "CfgRead.h"
#include "IvrConstDef.h"
#include "IvrStructs.h"
#include "IvrUtil.h"
#include "Timer.h"
#include "IvrQueApi.h"
//#include <mcheck.h>

/*Log    gLog;
CTrace gTrace (TRACE_IVR_CCXML_ENV);
TEXT   gProcessName[MAX_PROCESS_NAME + 1] = "";
TEXT   gLogText[MAX_LOG_TEXT_LEN + 1] = "";*/

class CcxmlProcessor
{
	public:
      CcxmlProcessor();
		~CcxmlProcessor();

		BOOLEAN Init ();

		BOOLEAN ReadConfig ();

		BOOLEAN CreateQueue ();

		BOOLEAN ReadRequest (CcxmlRequest&);

		BOOLEAN ReadEvent (CcxmlRequest&, INT16&);

		BOOLEAN ProcessReq();

		BOOLEAN CreateChild(TEXT* uri);

		BOOLEAN BuildRequest(TEXT* uri);

		BOOLEAN SendNackRspToSmgr(CcxmlRequest&, EnumExitReason);
		
		IvrQueApi      mIvrQueApi;
	private:

		key_t          mCxReqQueKey;
		key_t          mCxResQueKey;
		key_t          mCxInterQueKey;

		MsgQueue       mCxReqQue;
		MsgQueue       mCxResQue;
		MsgQueue       mCxInterQue;

		OktoClient     *mclient;
		CcxmlRequest   mCcxmlRequest;

		pid_t          mparentid;

		TEXT           mLogText[MAX_LOG_TEXT_LEN];

      TEXT           mXmlPath[IVR_MAX_XML_FILE_LEN];

};
