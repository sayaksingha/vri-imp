// @(#) $Id: HmpDevice.h,v 1.1.2.1 2012/02/21 08:31:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpDevice.h
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
#ifndef HmpDevice_H
#define HmpDevice_H

#include "HmpReqMapApi.h"
#include "HmpDtiDevice.h"
#include "HmpIpmDevice.h"
#include "HmpDxDevice.h"


class HmpDevice : public HmpDxDevice, public HmpDtiDevice, public HmpIpmDevice
{
   public:
      
      HmpDevice();
      
      ~HmpDevice();
      
      BOOLEAN OpenDevices();

      BOOLEAN CloseDevices();

      BOOLEAN ProcessEvent();

      BOOLEAN StartSrEvent(MpResponseDetails &);

      BOOLEAN Init();

      BOOLEAN DeInit();

      BOOLEAN PlayFile(MpRequestDetails &);
      BOOLEAN RecordFile(MpRequestDetails &);
      BOOLEAN GetDigit(MpRequestDetails &);

      BOOLEAN Stop(INT32 seq, INT32 cic);

      void DeviceUnlisten(int lSeq, int dxHdlIdx = -1, int ipmHdlIdx = -1, int dtiHdlIdx = -1);

      BOOLEAN ProcessEvents(long , int, MpResponseDetails &);

      ChannelStatus mChannelStatus[MAX_DTI_DEVICE];

      BOOLEAN HandlePlayStopped(HmpReqMap &, INT32, MpResponseDetails &);
      BOOLEAN HandleRecordStopped(HmpReqMap &, INT32, MpResponseDetails &);
      BOOLEAN HandleGetDigitStopped(HmpReqMap &, INT32, MpResponseDetails &);

      BOOLEAN HandleNextRequest(HmpReqMap &, INT32, INT32, MpResponseDetails &);

      BOOLEAN PlayStream(MpRequestDetails &, BOOLEAN play = true);

      BOOLEAN FillHmpReqAndInstToHash(HmpReqMap &, MpRequestDetails &, INT32, INT32);

      BOOLEAN StopAndScheduleRequest(INT32, INT32, MpRequestDetails &, HmpReqMap &, EnumChannelReqStatus);

      BOOLEAN FillIpmHmpReqAndInstToHash(INT32, MpRequestDetails &, INT32);

      BOOLEAN StreamReq(MpRequestDetails &);

      BOOLEAN StopStreamReq(INT32, INT32);

   protected:
      
      BOOLEAN GetDeviceInfo();
      BOOLEAN VirtualBoardCount(AUID PhyBd);
      BOOLEAN Subdevices(TEXT* VBName);

      HmpReqMapApi   mHmpReqMapApi;

      TEXT     mLogText[MAX_LOG_TEXT_LEN];

};

#endif

