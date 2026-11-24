// @(#) $Id: HmpIpmDevice.h,v 1.1.2.1 2012/02/21 08:31:36 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpIpmDevice.h
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
#ifndef HmpIpmDevice_H
#define HmpIpmDevice_H

#include "HmpUtil.h"


class HmpIpmDevice 
{
   public:
      
      HmpIpmDevice();
      
      ~HmpIpmDevice();
      
      BOOLEAN OpenIpmDevice();

      BOOLEAN CloseIpmDevice();
      
      BOOLEAN CreateRtspSession(MpRequestDetails &);
      
      BOOLEAN StartStream( );
      
      BOOLEAN StopStream();

      BOOLEAN RtspPlay();

      BOOLEAN RtspStop();

      BOOLEAN SetFreeIpmDevice();

      BOOLEAN SetIdleIpmDevice(INT32);

      INT16 CheckLiveStream(TEXT * rtspUrl);

      BOOLEAN TearSessions(int);
      
      HmpIpmStruct mHmpIpmStruct[MAX_IPM_DEVICE];

      UINT32   mIpmLiveStreamCount;

   protected: 
      
      UINT32   mNumOfIpmDevice;

      
      INT16    mFreeIpmIdx;


   private: 
      
      void SetupRtspMediainfo ();
};

#endif
