// @(#) $Id: BillingStructs.h,v 1.1.4.3 2016/04/12 21:59:00 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// BillingStructs.h
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
// Originated : KOTHARI KOMAL                                12-JAN-2012
//----------------------------------------------------------------------
#ifndef INCL_MMP_BILLING_STRUCTS_DEF_H
#define INCL_MMP_BILLING_STRUCTS_DEF_H

#include "Types.h"
#include "Ss7Util.h"
#include "MmpStructs.h"
#include "BillingApi.h"

enum EnumChargeModuleState
{
   CM_IDLE           = 0x00,
   CM_CALL_RECEIVED  = 0x01,
   CM_CALL_CONTINUE  = 0x02,
   CM_CALL_ANSWERED  = 0x03,
   CM_CALL_COMPLETE  = 0x04,
   CM_PLAY_CONTENT   = 0x05,
   CM_PLAY_CONTINUE  = 0x06,
   CM_PLAY_START     = 0x07,
   CM_PALY_COMPLETE  = 0x08
};

enum EnumChargeFunction
{
   CHARGE_FUNCTION_NONE     = 0x00,
   CHARGE_FUNCTION_DURATION = 0x01,
   CHARGE_FUNCTION_CONTENT  = 0x02,
   CHARGE_FUNCTION_MAX      = CHARGE_FUNCTION_CONTENT
};

enum EnumChargeUnit
{
   CHARGE_UNIT_AMOUNT       = 1,
   CHARGE_UNIT_NOM          = 2,
   CHARGE_UNIT_CONTENT      = 3,
   CHARGE_UNIT_SUBSCRIPTION = 4,
   CHARGE_UNIT_DEDICATION   = 5,
   CHARGE_UNIT_OTHER_FLAT   = 6
};
/*
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
*/
struct MmpCdrMsg
{
   //Information Specific to CDRs
   TEXT            cldParty[SS7_MAX_ADDRESS_LEN + 1];
   TEXT            clgParty[SS7_MAX_ADDRESS_LEN + 1];
   TEXT            imsi[SS7_MAX_ADDRESS_LEN + 1];
   TEXT            scAddress[SS7_MAX_ADDRESS_LEN + 1];
   TEXT            referenceNumber[SS7_MAX_ADDRESS_LEN + 1];
   //TimeAndTimeZone timeAndTimeZone;   //CapStructs.h
   // Total Call Duration / No of Sms.
   EnumChargeUnit  chargeUnit;
   UINT32          unitCharged;
   UINT32          duration;
   //Call or SMS Connect/Continue Time
   time_t          connectTime;
   time_t          answerTime;
   time_t          disconnectTime;
   time_t          stopTime;
   UINT16          attemptElapsedTime;
   UINT16          connectedElapsedTime;

   UINT8           releaseCause;
   BOOLEAN         isInternational;
};

/***********************************************************/
/*          Package Billing Retalted Structures            */
/***********************************************************/

struct BillingLimits
{
   UINT32            subscriptionId;
   UINT32            packageId;
   EnumMmpPkgType    packageType;
   UINT32            usage;
};

struct SubsPackageInfo
{
   EnumMmpPkgType    packageType;
   TEXT              msisdn[MAX_MSISDN_LEN+1];
   UINT32            subscriptionId;
   UINT32            packageId;
   UINT32            min;
   UINT32            content;
   UINT32            dedication;
   UINT32            subscription;
   UINT8             status;
   tm                startDate;
   tm                endDate;
   tm                startTime;
   tm                endTime;
};

/***********************************************************/
/*    Shared Memory details for Billing Records            */
/***********************************************************/

// Billing Dlg Details
struct BillingRecordDetails
{
   UINT32             appId;
   UINT32             channelId;
   UINT32             contentId;
   EnumBillingEvents  initialEvent;
   EnumBillingEvents  lastEvent;
   EnumBillingEvents  currentEvent;
   unsigned int       referenceNum;

   TEXT               msisdn[MAX_MSISDN_LEN+1];
   TEXT               shortCode[MAX_MSISDN_LEN+1];
   TEXT               imsi[MAX_MSISDN_LEN+1];
   EnumBillingSubsType       subsType;

   EnumChargeFunction chargeFunction;
   EnumChargeMode     chargeMode;
   UINT32             chargeId;

   TEXT               purpose[MAX_PURPOSE_LEN+1];
   UINT32             expiryTime;

   UINT32             timerTag;
   UINT32             dlgId;
   
   int                acrTimer;
   int                nomTimer;
   UINT16             pendingReoprts;
   time_t             startTime;
   time_t             endTime;
   UINT32             currstate;
   UINT32             currEvt;
   BOOLEAN            startFlag;

   SubsPackageInfo    packageInfo;
   BillingLimits      limits;
   MmpCdrMsg          cdr;

   BillingRecordDetails()
   {
      channelId        = 0;
      dlgId            = 0;
      appId            = 0;
      initialEvent     = BILLING_UNKNOWN_EVENT;
      subsType         = (EnumBillingSubsType)SUBS_TYPE_UNKNOWN;
      chargeMode       = CHARGE_MODE_UNKNOWN;
      chargeId         = 0;
      referenceNum     = 0;
      expiryTime       = 0;
      acrTimer         = 0;
      nomTimer         = 0;
      pendingReoprts   = 0;

      startTime        = 0;
      endTime          = 0;
      currstate        = 0;
      currEvt          = BILLING_UNKNOWN_EVENT;
      startFlag        = false;

      memset(&msisdn , '\0', sizeof(msisdn));
      memset(&imsi   , '\0', sizeof(imsi));
      memset(&cdr    , '\0', sizeof(MmpCdrMsg));
      memset(&purpose, '\0', sizeof(purpose));
      memset(&packageInfo, 0, sizeof(SubsPackageInfo));
   }
};



#endif
