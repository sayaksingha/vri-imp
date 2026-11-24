// @(#) $Id: IsdnDxDevice.h,v 1.1.4.3 2016/04/12 21:59:36 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       IsdnDxDevice.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2012
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Chethan V M                        27/07/2013
//----------------------------------------------------------------------
#ifndef ISDN_DX_DEVICE_H
#define ISDN_DX_DEVICE_H

#include "IsdnUtil.h"

static const int gDMValues[255] =  {0,/*0th Index*/  
0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    0,    0,    0,    DM_P, 0,    0,    0,    0,    0,
0,    DM_S, 0,    0,    0,    0,    0,    DM_0, DM_1, DM_2,
DM_3, DM_4, DM_5, DM_6, DM_7, DM_8, DM_9, 0,    0,    0,
0,    0,    0,    0,    DM_A, DM_B, DM_C, DM_D, 0,    0,
0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
0,    0,    0,    0,    0,    0,    DM_A, DM_B, DM_C, DM_D,
0,    0,    0,    0,    0,    0,    0,    0,    0};

/*Check Dialogic Header File For This Numeric Values (dxxxlib.h)*/

class IsdnDxDevice
{
   public:

      IsdnDxDevice();

      ~IsdnDxDevice();

      BOOLEAN  OpenDxDevice(INT32);

      BOOLEAN  GetDxXmitslot(INT32);
      
      BOOLEAN  CloseDxDevice(INT32);

      INT32 GetFreeDxDevice();

      BOOLEAN DxListen(int, long&);

      BOOLEAN DxUnlisten(int);

      BOOLEAN Play(MpPlay &, INT32 );
      BOOLEAN StopChnlAct(INT32);
      BOOLEAN Record(MpRecord &, INT32 );
      BOOLEAN GetDigits(MpGetDigit &, INT32 );

      BOOLEAN FreeDxDevice(INT32);

      BOOLEAN CloseDxIOFiles(int, int);

   protected:
      IsdnDx  mIsdnDx[ISDN_MAX_DX_DEVICE];
      UINT32  mNumOfDxDevice;

};

#endif
