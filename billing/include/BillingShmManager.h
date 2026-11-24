//----------------------------------------------------------------------
// NAME
// BillingShmManager.h
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
// Originated : KOTHARI KOMAL                           Date:05-MAR-2012
//----------------------------------------------------------------------
#ifndef INCL_SSP_SHM_MANAGER_H
#define INCL_SSP_SHM_MANAGER_H

#include "Types.h"

template <typename Data>
class BillingShmManager
{
   public:
   
   BillingShmManager();
   
   ~BillingShmManager();
   
   BOOLEAN Init(INT32, INT32, INT32);

   inline BOOLEAN InsertData(Data*, INT32);

   inline BOOLEAN GetData(Data*, INT32);

   inline BOOLEAN ResetSlot(INT32);

   // This method can be used only with INTIGER data types.
   INT32  GetFreeSlot(INT32);
   
   inline Data* GetShmPointer();

   protected:
   
   BOOLEAN CreateShm();
   
   INT32  mShmKey;

   INT32  mMax;
   
   INT32  mMin;

   INT32  mFreeDlgId;
   
   Data*  mpShmPtr;
};

#include "BillingShmManager.tcc"
#endif
