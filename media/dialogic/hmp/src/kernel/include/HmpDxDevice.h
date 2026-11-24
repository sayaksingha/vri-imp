// @(#) $Id: HmpDxDevice.h,v 1.1.2.1 2012/02/21 08:31:36 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpDxDevice.h
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
// AUTHOR         : Ramprasad M                              12-JAN-2012
//
// 1.MODIFIED BY  : <Name>       <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef HmpDxDevice_H
#define HmpDxDevice_H

#include "HmpUtil.h"

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

class HmpDxDevice
{
   public:

      HmpDxDevice();

      ~HmpDxDevice();

      BOOLEAN  OpenDxDevice();

      BOOLEAN  CloseDxDevice();

      INT32 GetFreeDxDevice();

      BOOLEAN DxListen(int, long&);

      BOOLEAN DxUnlisten(int);

      BOOLEAN Play(MpPlay &, INT32 );
      BOOLEAN StopChnlAct(INT32);
      BOOLEAN Record(MpRecord &, INT32 );
      BOOLEAN GetDigits(MpGetDigit &, INT32 );

      BOOLEAN FreeDxDevice(INT32);

      BOOLEAN CloseDxIOFiles(int, int);

      HmpDxStruct mHmpDxStruct[MAX_DX_DEVICE];

      UINT32 mNumOfDxDevice;

};

#endif
