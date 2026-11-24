// @(#) $Id: SipEvrApi.h,v 1.1.4.1 2016/04/12 21:59:24 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       SipEvrApi.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_SIP_EVR_API_DEF_H
#define INCL_SIP_EVR_API_DEF_H

#include "SipConstDef.h"
#include "SipStructs.h"

#define SIP_GEN_PROV_EVR_DB      1
//#define SIP_GEN_XXXX_EVR_DB    2

#define SIP_GEN_PROV_EVR_FILE    16
//#define SIP_GEN_XXXX_EVR_FILE  32

#define SIP_MAX_EVR_EVENT        255

enum EnumSipEvrType
{
   EVR_TYPE_UNKNOWN      = 0,
   EVR_TYPE_SIP_PROV    = 1,
   //EVR_TYPE_SIP_BILL  = 2
   //EVR_TYPE_SIP_XXXXX  = 3
};

enum EnumSipEvrEvt
{
   SIP_EVR_EVT_NO_EVT     = 0,

   SIP_EVR_EVT_RCV        = 1,

   SIP_EVR_EVT_DISCARDED  = 2,
   SIP_EVR_EVT_DROPPED    = 3,

   SIP_EVR_EVT_PROCESSED = 4,

   SIP_EVR_EVT_SNT       = 5

};

enum EnumSipEvrCause
{
};

struct SipProvEvrMsg
{
};

//struct SipxxxxEvrMsg
//{
//};


struct SipEvrMsg
{
   EnumSipEvrType    evrType;     // EVR_TYPE
   EnumSipEvrEvt     evrEvent;    // EVR_EVENT
   EnumSipEvrCause   evrCause;    // EVR_CAUSE
   //TEXT                 msisdn[SS7_MAX_ADDRESS_LEN + 1];
   time_t               gentime;
   
   union
   {
      SipProvEvrMsg    provEvr;
      //SipxxxxEvrMsg    xxxxEvr;
   };
};

class SipEvrApi
{
   public:

      SipEvrApi ();

      BOOLEAN Init ();

      BOOLEAN ReadEvrMessage (SipEvrMsg & evrMsg);

      BOOLEAN GenerateProvEvr (EnumSipEvrEvt evrEvent,
            EnumSipEvrCause   evrCause,
            TEXT *             msisdn,
            SipProvEvrMsg &   provEvr);

      BOOLEAN GenerateEvr (SipEvrMsg & evrMsg, TEXT * logText);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateMsgQueue ();

      key_t mEvrHdlrQueKey;
      MsgQueue mEvrHdlrQue;
};

#endif
