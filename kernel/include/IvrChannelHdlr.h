//---------------------------------------------------------------
// NAME      IvrChannelHdlr.h
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
// Originated : Divya K                       Date: 03-FEB-2012
//---------------------------------------------------------------
#ifndef INCL_IVR_SCHEDULER_H
#define INCL_IVR_SCHEDULER_H

#include "IvrQueApi.h"
#include "IvrUtil.h"
#include "IvrStructs.h"
#include "IvrConfigApi.h"
#include "IvrDbManager.h"
//#include "IsupStructs.h"
#include "MediaInterface.h"
#include "IvrChannelApi.h"

class IvrChannelHdlr
{
   public:
      //Constructor
      IvrChannelHdlr();
      
      //Destructor
      ~IvrChannelHdlr();

      BOOLEAN    Init() ;

		BOOLEAN    ReadRequest();

		BOOLEAN    ProcessRequest(UINT32&);
		
      BOOLEAN    FetchReq(UINT32&, UINT32&, UINT32&);

   private:

      BOOLEAN        SendRel();

      BOOLEAN        UpdateChStatus();

      BOOLEAN        SendCmd(EnumChannelCmd& ,UINT16&);

      IvrDbManager   mIvrDbManager;

      IvrQueApi      mIvrQueApi;

      IvrConfigApi   mIvrConfigApi;

      IvrChannelApi  mIvrChannelApi;

      GenIsupMsg     mGenIsupMsg;

      ChannelMgmt    mChannelMgmt[MAX_NUM_OF_CKT_CMDS];
     
      //CktCmdMsg      mCktCmdMsg;
      CktCmdReq      mCktCmdMsg;

      //CktCmdRspMsg   mCktCmdRspMsg;
      CktCmdRsp      mCktCmdRspMsg;
};

#endif
