//---------------------------------------------------------------
// NAME      IvrScheduler.h
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
// Originated : Divya K                       Date: 17-JAN-2012
//---------------------------------------------------------------
#ifndef INCL_IVR_FEEDBACK_H
#define INCL_IVR_FEEDBACK_H
#include "IvrQueApi.h"
#include "IvrUtil.h"
#include "IvrStructs.h"
#include "IvrConfigApi.h"
#include "IvrDbManager.h"

class IvrFeedback
{
   public:
      //Constructor
      IvrFeedback();
      
      //Destructor
      ~IvrFeedback();

      //Initialises IvrScheduler Module 
      BOOLEAN Init() ;

		BOOLEAN ReadRequest();

		BOOLEAN ProcessRequest();
		
		BOOLEAN ReadResponse();

		BOOLEAN ProcessResponse();
		

   private:

      BOOLEAN        CreateQueue();
      
      BOOLEAN        ReadConfig() ;

      BOOLEAN        GetReq();

      BOOLEAN        UpdateResponse();

      int            mReadQKey;
      MsgQueue       mReadQObj;
		
      int            mWriteQKey;
      MsgQueue       mWriteQObj;

      ScheduleReq    mScheduleReq[IVR_NO_OF_SCHEDULE_RECORDS];

      ScheduleRsp    mScheduleRsp;

      IvrQueApi      mIvrQueApi;

      IvrConfigApi   mIvrConfigApi;

      IvrDbManager   mIvrDbManager;

      UINT32         mTotalCount;
};

#endif
