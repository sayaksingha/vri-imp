// @(#) $Id: IsdnHandler.h,v 1.1.4.3 2016/04/12 21:59:36 shijuc Exp $
//----------------------------------------------------------------------
// NAME IsdnHandler.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2011
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR : Chethan V M                          Date : 14/08/2013
//----------------------------------------------------------------------
#ifndef ISDN_HANDLER_H
#define ISDN_HANDLER_H

#include "IsdnQueApi.h"
#include "IsdnConfigApi.h"
#include "IsdnDevice.h"

class IsdnHandler
{
   public:

      IsdnHandler();

      ~IsdnHandler();

      BOOLEAN  Init();

      BOOLEAN  DeInit();
      
      BOOLEAN  ReloadConfig();

      BOOLEAN ReadRequest();

      BOOLEAN ProcessRequest();

      BOOLEAN ProcessNonGcEvents(METAEVENT &);

      BOOLEAN ProcessGcEvents(METAEVENT &);

      BOOLEAN ReadIsupMsg(GenIsupMsg &);

      BOOLEAN ProcessIsupMsg(GenIsupMsg &);

      BOOLEAN ProcessChnlMgmtReq();

   public:

      IsdnQueApi         mIsdnQueApi;

      INT16    mMinCktId;    
      
      INT16    mMaxCktId;    

      IsdnDevice     mIsdnDevice;

      MpResponseDetails mMpResponse;

   private:
      
      BOOLEAN     StartSrlThread();

      BOOLEAN     StartIsdnCallThread();

      BOOLEAN     StartChnlMgmtThread();

      BOOLEAN     IssuePlayStop();

      BOOLEAN     IssueGetDigitStop();

      BOOLEAN     IssueRecordStop();

   private:
      
      IsdnConfigApi     mIsdnConfigApi;

      MpRequestDetails  mMpRequest;


};

#endif
