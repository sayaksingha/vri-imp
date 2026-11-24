// @(#) $Id: BillingApi.h,v 1.1.4.3 2016/04/12 21:58:59 shijuc Exp $
//----------------------------------------------------------------------------
// NAME 
// BillingApi.h 
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright (C) 2006, 
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd.
//
// CLASS TYPE
//
// DESCRIPTION
// This is a API(library class) for applications to invoke real-time billing
// (IN-Interactin) with subscribers account.
//
// Originated: KOTHARI KOMAL                                       16-JAN-2012
//----------------------------------------------------------------------------
#ifndef INCL_BILLING_API_H
#define INCL_BILLING_API_H

#include "Types.h"
#include "BillingUtil.h"

#ifdef MAX_MSISDN_LEN
#undef MAX_MSISDN_LEN
#endif

#define MAX_MSISDN_LEN              20
#define MAX_PURPOSE_LEN             50

#define BILLING_DEFAULT_EXPIRY_TIME 30 
#define BILLING_MIN_EXPIRY_TIME     5
#define BILLING_MAX_EXPIRY_TIME     300

#define BILLING_MIN_MSG_Q_KEY       IVR_MIN_MSG_Q_KEY
#define BILLING_MAX_MSG_Q_KEY       IVR_MAX_MSG_Q_KEY

enum EnumChargeMode
{
   CHARGE_MODE_UNKNOWN = 0x00,
   CHARGE_MODE_PACKAGE = 0x01,
   CHARGE_MODE_SCP     = 0x02
};

enum EnumBillingEvents
{
   BILLING_UNKNOWN_EVENT        = 0x00,
   BILLING_CALL_RECEIVED        = 0x01,
   BILLING_CALL_ANSEWERED       = 0x02,
   BILLING_CALL_DISCONNECTED    = 0x03,
   BILLING_CONTENT_PLAY         = 0x04,
   BILLING_CONTENT_START        = 0x05,
   BILLING_CONTENT_STOP         = 0x06,
   BILLING_OFFLINE_DEDICATION   = 0x07,
   BILLING_OFFLINE              = 0x08,
   BILLING_TIMER_EXPIRY         = 0x09
};
#define MAX_BILLING_EVENTS  9

enum EnumBillingSubsType
{
   BILLING_SUBS_TYPE_UNKNOWN  = -1,
   BILLING_SUBS_TYPE_POSTPAID = 0,
   BILLING_SUBS_TYPE_PREPAID  = 1
};

enum EnumBillingStatus
{
   BILLING_SUCCESS             = 0x00,
   BILLING_DISABLED            = 0x01,
   BILLING_FAILED              = 0x02,
   BILLING_UNKNOWN_SUBS        = 0x03,
   BILLING_INVALID_CHARGE_INFO = 0x04,
   BILLING_SCP_NOT_AVAILABLE   = 0x05,
   BILLING_TIMEOUT             = 0x06,
   BILLING_SYSTEM_FAILURE      = 0x07,
   BILLING_LOW_BALANCE         = 0x08,
   BILLING_NO_PACKAGE          = 0x09,
   BILLING_UNKNOWN
};


struct BillingRequest
{
   UINT32            appId;
   EnumBillingEvents event;
   EnumBillingSubsType      subsType;
   EnumChargeMode    mode;
   TEXT              msisdn[MAX_MSISDN_LEN+1];
   TEXT              shortCode[MAX_MSISDN_LEN+1];
   TEXT              imsi[MAX_MSISDN_LEN+1];
   UINT32            chargeId;
   UINT32            contentId;
   UINT32            channelId;
   unsigned int      referenceNum;
   TEXT              purpose[MAX_PURPOSE_LEN+1];
   UINT32            expiryTime;
};

struct BillingResponse
{
   UINT32            appId;
   EnumBillingEvents event;
   EnumBillingStatus status;
   EnumChargeMode    mode;
   TEXT              msisdn[MAX_MSISDN_LEN+1];
   UINT32            refId;
   unsigned int      referenceNum;
   UINT32            relCause;
};

//#include "BillingUtil.h"

class BillingApi
{
   public:
      BillingApi ();

      ~BillingApi ();

      BOOLEAN Init (UINT8 appId=0);

      // Process for Application to send request and receive response
      BOOLEAN ProcessBillingRequest (BillingRequest & billingRequest,
            BillingResponse & billingResponse);

      const char* GetVersion();

      // Read responce and give it to common process for application
      BOOLEAN ReceiveBillingResponse (BillingResponse & mAppRsp, int msgType);

      // Reads request from commonn process and inserts for charging gateway
      BOOLEAN SubmitBillingRequest (BillingRequest & mAppReq);

   private:
      msgQueueError CreateMsgQueue (MsgQueue & queue, const key_t key);

      BOOLEAN HandleQueueReadError (msgQueueError err, MsgQueue & queue);

   protected:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateIPC ();

      MsgQueue mBillingReqQue;
      key_t mBillingReqQKey;

      MsgQueue mBillingRspQue;
      key_t mBillingRspQKey;

      UINT8 mAppId;

};

#endif
