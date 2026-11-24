//----------------------------------------------------------------------
// NAME
// SspStruct.h
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
#ifndef INCL_SSP_STRUCT_H
#define INCL_SSP_STRUCT_H
//---------------------------------------------------------------------//
//------------------ Charging Module Related States--------------------//
//---------------------------------------------------------------------//
enum EnumSspStates
{
   SSP_ST_NO_STATE            = 0,
   SSP_ST_IDLE                = 0,

   SSP_ST_CALL_RECEIVED       = 1,
   SSP_ST_CALL_SETUP          = 2,
   SSP_ST_CONTINUE_CALL       = 2,
   SSP_ST_CALL_IN_PROGRESS    = 3,
   SSP_ST_CALL_ANSWERED       = 3,
   SSP_ST_CALL_DISCONNECTED   = 4,

   SSP_ST_CONTENT_PLAY        = 5,
   SSP_ST_CONTENT_SETUP       = 6,
   SSP_ST_CONTINUE_PLAY       = 6,
   SSP_ST_CONTENT_IN_PROGRESS = 7,
   SSP_ST_CONTENT_STARTED     = 7,    
   SSP_ST_CONTENT_STOPPED     = 8   
};

//---------------------------------------------------------------------//
//--------------------- Charging Module Related Events-----------------//
//---------------------------------------------------------------------//
enum EnumSspEvents
{
   SSP_EVT_NULL              = 0,

   SSP_EVT_CALL_RECEIVED     = 1,
   SSP_EVT_CALL_ANSWERED     = 2,
   SSP_EVT_CALL_DISCONNECTED = 3,
   SSP_CONTENT_PLAY          = 4,
   SSP_CONTENT_START         = 5,
   SSP_CONTENT_STOP          = 6,
   SSP_EVT_TIMER_ACR         = 7,  // Apply Charging Report Timer Expires; ACR has to be sent to SCP
   SSP_EVT_TIMER_NOM         = 8,  // Number Of Minutes over, Call has to be disconnected.

   SSP_EVT_IN_START_EVENTS   = 10,
   SSP_EVT_IN_ABORT          = 11,
   SSP_EVT_IN_APPLY_CHRG_REQ = 12,
   SSP_EVT_IN_CALL_INFO_REQ  = 13,
   SSP_EVT_IN_RR_BCSM_REQ    = 14,
   SSP_EVT_IN_ACTV_TEST_REQ  = 15,
   SSP_EVT_IN_ALL_REQ        = 16,
   SSP_EVT_IN_CONTINUE       = 17,
   SSP_EVT_IN_CONNECT        = 18,
   SSP_EVT_IN_RELEASE        = 19,
};

//---------------------------------------------------------------------//
//----------------- Charging Module Related Actions--------------------//
//---------------------------------------------------------------------//
enum EnumSspActions
{
   SSP_ACT_DO_NOTHING = 0,

   SSP_ACT_HDL_CALL_RECEIVED,
   SSP_ACT_HDL_CALL_ANSWER,
   SSP_ACT_HDL_CALL_DISCONNECT,
   SSP_ACT_HDL_CONTENT_PLAY,
   SSP_ACT_HDL_CONTENT_START,
   SSP_ACT_HDL_CONTENT_STOP,
   
   SSP_ACT_IN_ABORT,

   SSP_ACT_SND_DP2,
   SSP_ACT_SND_DP12,
   SSP_ACT_SND_APPL_CHRG_REPORT,
   SSP_ACT_SND_CALL_INFO_REPORT,
   SSP_ACT_SND_T_BUSY,
   SSP_ACT_SND_T_NO_ANS,
   SSP_ACT_SND_T_ANS,
   SSP_ACT_SND_T_DISCONN,
   SSP_ACT_SND_O_BUSY,
   SSP_ACT_SND_O_NO_ANS,
   SSP_ACT_SND_O_ANS,
   SSP_ACT_SND_O_DISCONN,
   SSP_ACT_SND_ACTV_TEST_REQ,
   SSP_ACT_SND_ACTV_TEST_RSP,

   SSP_ACT_START_AC_TIMER,
   SSP_ACT_START_NOM_TIMER,
   
   SSP_ACT_EXIT
};

struct SspStates
{
   EnumSspEvents    event;
   UINT8            noOfActions;
   EnumSspActions   act[SSP_MAX_NO_ACT_WITHIN_EVT +1];
};

// Dlg Details
struct SspDlgDetails
{
   UINT32            channelId;
   int               dlgId;
   UINT32            appId;
   EnumBillingEvents initialEvent;
   EnumBillingEvents currentEvent;
   EnumSubsType      subsType;
   EnumChargeMode    chargeMode;
   TEXT              msisdn[MAX_MSISDN_LEN+1];
   TEXT              imsi[MAX_MSISDN_LEN+1];
   UINT32            chargeId;
   UINT32            referenceNum;
   TEXT              purpose[MAX_PURPOSE_LEN+1];
   UINT32            expiryTime;

   int               acrTimer;
   int               nomTimer;
   UINT16            pendingReoprts;

   time_t             startTime;
   time_t             endTime;
   EnumSspStates      currstate;
   EnumSspEvents      currEvt;
   BOOLEAN            startFlag;
   BOOLEAN            tcapFlag;

   MmpCdrMsg          cdr;

   union
   {
      CapMsg          capMsg;
      TcapMsg         tcapMsg;
   };


   SspDlgDetails()
   {
      channelId        = 0;
      dlgId            = 0;
      appId            = 0;
      initialEvent     = SSP_EVT_NULL;
      subsType         = SUBS_TYPE_UNKNOWN;
      chargeMode       = CHARGE_MODE_UNKNOWN;
      chargeId         = 0;
      referenceNum     = 0;
      expiryTime       = 0;
      acrTimer         = 0;
      nomTimer         = 0;
      pendingReoprts   = 0;

      startTime        = 0;
      endTime          = 0;
      currstate        = SSP_ST_NO_STATE;
      currEvt          = SSP_EVT_NULL;
      startFlag        = false;
      tcapFlag         = false;      // By default it will indicate the Cap Msg.

      memset(&msisdn , '\0', sizeof(msisdn));
      memset(&imsi   , '\0', sizeof(imsi));
      memset(&cdr    , '\0', sizeof(MmpCdrMsg));
      memset(&purpose, '\0', sizeof(purpose)); 
      memset(&capMsg , '\0', sizeof(capMsg));
      memset(&tcapMsg, '\0', sizeof(tcapMsg));
   }
};

// evr Structure...
struct SspEvr
{
   TEXT               origcalledNo[SSP_MAX_MSISDN_LEN +1];
   TEXT               origcalledImsi[SSP_MAX_IMSI_LEN +1];
   TEXT               calledNo[SSP_MAX_MSISDN_LEN +1];
   TEXT               calledImsi[SSP_MAX_IMSI_LEN +1];
   TEXT               callingNo[SSP_MAX_MSISDN_LEN +1];
   time_t             evrTime;
   UINT16             dlgId;
   UINT16             channelId;
   
   EnumSspStates      currstate;
   EnumSspEvents      currEvt;
};

struct MsisdnRange
{
   TEXT    minVal [SSP_MAX_MSISDN_LEN +1];
   UINT8   minLen;
   TEXT    maxVal [SSP_MAX_MSISDN_LEN +1];
   UINT8   maxLen;
};

struct GsmscfInfo
{
   TEXT          scfAddr[SSP_MAX_MSISDN_LEN +1];
   UINT8         noOfmsisdnRanges;
   MsisdnRange   msisdnRange[SSP_MAX_NO_MSISDN +1];
};

/*
   TIME | EVENT | I/C DLG | O/G DLG | ORIG MSISDN |ORIG IMSI | CALLED MSISDN |CALLED IMSI |
   CALLING MSISDN| FROM_MSC | TO_MSC | FROM_IN | TO_IN
 */
#endif

