// @(#) $Id: HmpDtiDevice.h,v 1.1.2.1 2012/02/21 08:31:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpDtiDevice.h
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
// AUTHOR         : Ramprasad M                              12-JAN-2012
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef HmpDtiDevice_H
#define HmpDtiDevice_H

#include "HmpUtil.h"

class HmpDtiDevice
{
   public:

      HmpDtiDevice();

      ~HmpDtiDevice();

      BOOLEAN OpenDtiDevice();

      BOOLEAN CloseDtiDevice();
      
      BOOLEAN DtListen(int, long &);
      
      BOOLEAN DtUnListen(int);


      HmpDtiStruct mHmpDtiStruct[MAX_DTI_DEVICE];

      UINT32 mNumOfDtiDevice;
};

#endif
