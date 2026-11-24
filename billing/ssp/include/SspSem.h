// @(#) $Id: SspSem.h,v 1.1.4.3 2016/04/12 21:59:01 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// SspSem.h
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
#ifndef INCL_SSP_SEM_H
#define INCL_SSP_SEM_H

#include "SspUtil.h"
#include "SspConstDef.h"

class SspSem
{
   public:
      SspSem ();

      ~SspSem ();

      BOOLEAN Init (INT32, INT32);

      BOOLEAN Lock();

      BOOLEAN UnLock();

   protected:

      INT32 mSemKey;

      INT32 mSemId;
      
      INT32 mSemNo;

      BOOLEAN CreateSem();
};

#endif
