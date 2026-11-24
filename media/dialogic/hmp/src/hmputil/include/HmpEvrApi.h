// @(#) $Id: HmpEvrApi.h,v 1.1.2.1 2012/02/21 08:31:32 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpEvrApi.h
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
// AUTHOR         : Hmp                                  23-Nov-2011
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_HMP_EVR_API_DEF_H
#define INCL_HMP_EVR_API_DEF_H

#include "HmpConstDef.h"
#include "HmpStructs.h"

#define HMP_GEN_PROV_EVR_DB      1
//#define HMP_GEN_XXXX_EVR_DB    2

#define HMP_GEN_PROV_EVR_FILE    16
//#define HMP_GEN_XXXX_EVR_FILE  32

#define HMP_MAX_EVR_EVENT        255

enum EnumHmpEvrType
{
   EVR_TYPE_UNKNOWN      = 0,
   EVR_TYPE_HMP_PROV    = 1
   //EVR_TYPE_HMP_BILL  = 2
   //EVR_TYPE_HMP_XXXXX  = 3
};

enum EnumHmpEvrEvt
{
   HMP_EVR_EVT_NO_EVT     = 0,

   HMP_EVR_EVT_RCV        = 1,

   HMP_EVR_EVT_DISCARDED  = 2,
   HMP_EVR_EVT_DROPPED    = 3,

   HMP_EVR_EVT_PROCESSED = 4,

   HMP_EVR_EVT_SNT       = 5

};

enum EnumHmpEvrCause
{
};

struct HmpProvEvrMsg
{
};

//struct HmpxxxxEvrMsg
//{
//};


struct HmpEvrMsg
{
   EnumHmpEvrType    evrType;     // EVR_TYPE
   EnumHmpEvrEvt     evrEvent;    // EVR_EVENT
   EnumHmpEvrCause   evrCause;    // EVR_CAUSE
   //TEXT                 msisdn[SS7_MAX_ADDRESS_LEN + 1];
   TEXT                 msisdn[20 + 1];
   time_t               gentime;
   
   union
   {
      HmpProvEvrMsg    provEvr;
      //HmpxxxxEvrMsg    xxxxEvr;
   };
};

class HmpEvrApi
{
   public:

      HmpEvrApi ();

      BOOLEAN Init ();

      BOOLEAN ReadEvrMessage (HmpEvrMsg & evrMsg);

      BOOLEAN GenerateProvEvr (EnumHmpEvrEvt evrEvent,
            EnumHmpEvrCause   evrCause,
            TEXT *             msisdn,
            HmpProvEvrMsg &   provEvr);

      BOOLEAN GenerateEvr (HmpEvrMsg & evrMsg, TEXT * logText);

   private:
      BOOLEAN ReadConfig ();

      BOOLEAN CreateMsgQueue ();

      key_t mEvrHdlrQueKey;
      MsgQueue mEvrHdlrQue;
};

#endif
