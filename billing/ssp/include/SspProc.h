// @(#) $Id: SspProc.h,v 1.1.4.3 2016/04/12 21:59:01 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// SspProc.h
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated : KOTHARI KOMAL                           Date:23-FEB-2012
//----------------------------------------------------------------------
#ifndef INCL_SSP_PROC_H
#define INCL_SSP_PROC_H

#include "SspConstDef.h"

class SspProc
{
   //--------- Public Methods ---------
public:
   SspProc();

   ~SspProc();
   
   BOOLEAN Init();

   BOOLEAN ReloadConfig();

   BOOLEAN ReadReqFromChargingQue(UINT32);

   BOOLEAN ProcessChargingRequest(UINT32);

   BOOLEAN ReadRspFromInQue(UINT32);

   BOOLEAN ProcessInRsp(UINT32);

  //*****************************************************
  //Please Note These function is shared by both threads*
  //*****************************************************
   BOOLEAN DoNothing(INT32, UINT32);

   BOOLEAN SendAbortToIn(INT32, UINT32);
    
   BOOLEAN SendDp2(INT32, UINT32);

   BOOLEAN SendDp12(INT32, UINT32);

   BOOLEAN SendApplChrgReport(INT32, UINT32);

   BOOLEAN SendCallInfoReport(INT32, UINT32);

   BOOLEAN SendEvtReportBCSM(INT32, UINT32);

   BOOLEAN SendTBusy(INT32, UINT32);
    
   BOOLEAN SendTNoAnswer(INT32, UINT32);
    
   BOOLEAN SendTAnswer(INT32, UINT32);
    
   BOOLEAN SendTDisconnect(INT32, UINT32);
    
   BOOLEAN SendOBusy(INT32, UINT32);
    
   BOOLEAN SendONoAnswer(INT32, UINT32);
    
   BOOLEAN SendOAnswer(INT32, UINT32);
    
   BOOLEAN SendODisconnect(INT32, UINT32);
    
   BOOLEAN SendActivityReq(INT32, UINT32);
    
   BOOLEAN SendActivityRsp(INT32, UINT32);
    
   BOOLEAN ClearDlgInfo(INT32, UINT32);
   //--------------------End------------------------

   //------- Public Data members -------------
   EnumSspStates      mSspThrdState;
   EnumSspEvents      mSspThrdEvt;
   int                mSspReqChannelId;

   EnumSspStates      mInThreadState;
   EnumSspEvents      mInThreadEvt;
   int                mInDlgId;

private:
   //------- Private Member Functions ---------
   //*****************************************************
   //Please Note These function is shared by both threads*
   //*****************************************************
   BOOLEAN DecodeTcapMsg(TcapMsg&, CapMsg&, UINT32);

   BOOLEAN DecodeTcapInvoke(TcapMsg&, CapMsg&, UINT32);

   BOOLEAN DecodeTcapAbort(TcapMsg&, CapMsg&, UINT32);

   BOOLEAN GetEvent(CapMsg&, EnumSspEvents&, UINT32);

   BOOLEAN GetCapInvokeEvent(CapMsg&, EnumSspEvents&, UINT32);

   BOOLEAN PutGsmScfAddr(TCAPAddress*, TEXT*, UINT32);

   BOOLEAN PutMscAddr(TCAPAddress*, TEXT*, UINT32);
   //-------------------End-------------------------------

   //------- Private Data members -------------
   SspCfg          mCfgObj;
   SspQueApi       mQueObj;
   SspSem          mSemObj;

   SspShmManager<SspDlgDetails> mDlgDet;

   //SspShmManager<INT32>           mFreePool;
   SspShmManager<int>           mFreePool;

   // This is used by Main thread....
   BillingRequest    mBillingRequest;  // Request received from Charging Interface Queueng 172.20.0.218
 
   SspDlgDetails     mSspDlgDet;
   SspDlgDetails*    mpSspDlgDet;
   CapMsg            mSspCapMsg;

   // This is used by In Rsp thread...
   TcapMsg           mInTcapMsg;
   CapMsg            mInCapMsg;
   SspDlgDetails*    mpInRspDlgDet;
   
   // ss7 Cap util objects.. shared by both threads
   Cap_ActivityTest              mActivityTestObj;

   Cap_ApplyCharging             mApplyChargingObj;
   Cap_ApplyChargingReport       mApplyChargingReportObj;

   Cap_CallInfoRequest           mCallInfoRequestObj;
   Cap_CallInfoReport            mCallInfoReportObj;

   Cap_Connect                   mConnectObj;

   Cap_Continue                  mContinueObj;

   Cap_RequestReportBCSM         mRequestReportBCSMObj;
   Cap_EventReportBCSM           mEventReportBCSMObj;

   Cap_InitialDP                 mInitialDPObj;

   Cap_ReleaseCall               mReleaseCallObj;
   
};
#endif
