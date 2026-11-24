//@(#) $Id: AcuCardDetails.h,v 1.1.4.3 2016/04/12 21:59:26 shijuc Exp $
//---------------------------------------------------------------
// NAME
// fullpath/AcuCardDetails.h
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
// CLASS TYPE
// abstract or concrete
//
// DESCRIPTION
//
// Originated : Roopa A M
//----------------------------------------------------------------
#ifndef ACU_CARD_DETAILS_H
#define ACU_CARD_DETAILS_H

#define MAX_SEQUENCE_STRING 10

#include "AcuMediaConstDef.h"
#include "AcuMediaUtil.h"

struct ModuleInfo
{
   tSMModuleId moduleId;
   INT16  moduleIdx;
};

struct CardDetails
{
   UINT32         cardId;
   TEXT           cardSerialNum[ACULAB_MAX_SERIAL + 1];
   ModuleInfo     moduleInfo[ACULAB_MAX_NO_MODULES];
};

typedef HashTable< TEXT, MAX_SEQUENCE_STRING, CardDetails > MediaCardDetails;

class AcuCardDetails 
{
   public:

      AcuCardDetails();

      ~AcuCardDetails();

      //------------------------------------------------------------
      BOOLEAN Init(UINT32 maxReferenceEntries);

      BOOLEAN ReadConfig();

      BOOLEAN DeInit();

      //------------------------------------------------------------
      BOOLEAN InsertCardDetails(CardDetails &);

      BOOLEAN DeleteCardDetails(UINT32 lCardId);

      BOOLEAN UpdateCardDetails(UINT32 lCardId, CardDetails &);

      BOOLEAN GetCardDetails(UINT32 lCardId, CardDetails &);

      BOOLEAN ClearCardDetails();

      int GetTotalCardInfo(UINT32 lCardId[ACULAB_MAX_NUM_CARDS]);

   protected:
      key_t mShmCardDetailsKey;
      CardDetails*     mpHashMem;
      MediaCardDetails  mCardDir;

      int       mMaxSize;
};
#endif
