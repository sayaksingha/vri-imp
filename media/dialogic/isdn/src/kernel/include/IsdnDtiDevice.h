// @(#) $Id: IsdnDtiDevice.h,v 1.1.4.3 2016/04/12 21:59:36 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       IsdnDtiDevice.h
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
#ifndef ISDN_DTI_DEVICE_H
#define ISDN_DTI_DEVICE_H

#include "IsdnUtil.h"

class IsdnDtiDevice
{
   public:

      IsdnDtiDevice();

      ~IsdnDtiDevice();

      BOOLEAN OpenDtiDevice(INT32);

      BOOLEAN GetDtiXmitslot(INT32);

      BOOLEAN CloseDtiDevice(INT32);
      
      BOOLEAN DtListen(int, long &);
      
      BOOLEAN DtUnListen(int);

   protected:
      IsdnDti mIsdnDti[ISDN_MAX_DTI_DEVICE];
      UINT32 mNumOfDtiDevice;

};

#endif
