// @(#) $Id: IvrPegHandler.h,v 1.1.1.1.2.1 2016/04/12 21:59:17 shijuc Exp $
//---------------------------------------------------------------
// NAME      IvrPegHandler.h
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
//
// DESCRIPTION
//
// Originated : DIVYA K                         Date: 21-JAN-2011
//---------------------------------------------------------------
#include "Types.h"
#include <stdio.h>
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
#include "PegApi.h"
//nclude "Peg.h"

class IvrPegHandler
{
	public:
		IvrPegHandler();
		~IvrPegHandler();

		BOOLEAN   Init();

		BOOLEAN   DeInit();

		BOOLEAN   ReadConfig();

		BOOLEAN   CreateIpc();

		BOOLEAN   PopulateShm();

		BOOLEAN   LoadShm();

		void      ConvertToTrafficShmData(EventInfo* lData, EventInfo* lPreviouData, TEXT* lTrafficData);

		BOOLEAN   GetCurrentSecTraffic(BOOLEAN&, BOOLEAN&, BOOLEAN&);
		//BOOLEAN   GetCurrentSecTraffic();

		BOOLEAN   InitTrafficData();

		BOOLEAN   GetOneSecData();

		BOOLEAN   GetFiveSecData();

		BOOLEAN   GetTenSecData();

		void        TotalTrafficShmData(EventInfo* lData, EventInfo* lPreviouData, TEXT* lTrafficData);

   private:

		INT32         mPegShmQKey;
		INT32         mPegTrafficShmQKey;

		struct        EventInfo   *mOnlinePegData;

		EventInfo     *mPegData;
		EventInfo     *mPreviousOneSecPegData;
		EventInfo     *mPreviousFiveSecPegData;
		EventInfo     *mPreviousTenSecPegData;
		//EventInfo   *mPreviousPegData;

		INT32         mShmId;

		UINT32        mMaxShmSize;
		UINT32        mMaxDispShmSize;

		TEXT          mCurrSecData[1000];
		TEXT          mOneSecData[1000];
		TEXT          mFiveSecData[1000];
		TEXT          mTenSecData[1000];
		
		TEXT          mOnlineTraffic[1000];
		TEXT*         mOnlineData;

		TEXT          mPreviousOneSecData[1000];
		TEXT          mPreviosFiveSecData[1000];
		TEXT          mPreviousTenSecData[1000];
		
		TEXT          mTotalOneSecData[1000];
		TEXT          mTotalFiveSecData[1000];
		TEXT          mTotalTenSecData[1000];
};
