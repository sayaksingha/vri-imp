// @(#) $Id: IsdnDevice.h,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       IsdnDevice.h
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
// AUTHOR         : Chethan V M                     25/07/2013
//----------------------------------------------------------------------
#ifndef ISDN_DEVICE_H
#define ISDN_DEVICE_H

#include "IsdnDtiDevice.h"
#include "IsdnDxDevice.h"
#include "IsdnConfigApi.h"
#include "IsdnUtil.h"

struct ScheduledConnect
{
   BOOLEAN    isScheduled;
   GenIsupAnm schConnect;
};

class IsdnDevice : public IsdnDxDevice, public IsdnDtiDevice
{
   public:
      
      IsdnDevice();
      
      ~IsdnDevice();
      
      BOOLEAN OpenDevices();

      BOOLEAN CloseDevices();

      BOOLEAN ProcessEvent();

      BOOLEAN GetSrEvent(METAEVENT &);

      BOOLEAN Init(UINT16, UINT16);

      BOOLEAN InitSharedMem();

      BOOLEAN DeInit();

      INT32   MapVoiceDevHdl(int);

      EnumIsdnProcRetType PlayFile(MpRequestDetails &);
      BOOLEAN             RecordFile(MpRequestDetails &);
      BOOLEAN             GetDigit(MpRequestDetails &);

      BOOLEAN Stop(INT32 seq, INT32 cic);

      BOOLEAN ProcessVoiceEvents(long , int, MpResponseDetails &);

      ChannelStatus mChannelStatus[ISDN_MAX_GC_DEVICE];

      BOOLEAN HandlePlayStopped(INT32, MpResponseDetails &);
      BOOLEAN HandleRecordStopped(INT32, MpResponseDetails &);
      BOOLEAN HandleGetDigitStopped(INT32, MpResponseDetails &);
      BOOLEAN HandleTdxError(INT32, MpResponseDetails &);

      BOOLEAN HandleNextRequest(INT32, MpResponseDetails &);

      BOOLEAN FillIsdnVoiceReq(IsdnVoiceDetails &, MpRequestDetails &);

      BOOLEAN StopAndScheduleRequest(INT32, INT32, MpRequestDetails &, EnumIsdnVoiceChannelReqStatus);

      void    ChannelReset(INT32 cicIdx);

      void    CallInfoReset(INT32 cicIdx);

      void    GetGcErrInfo(GC_INFO &);

      void    GetGcResultInfo(INT32, METAEVENT *, GC_INFO &);

      INT16   SendIsdnCallMsgToNw(GenIsupMsg &);

      BOOLEAN ProcessGlobalCallEvents(METAEVENT &, GenIsupMsg &);

      BOOLEAN SendIsdnChnlMgmtMsgToNw(CktCmdReq &, CktCmdRsp &);

      int     GetCallState(INT32 cic, long crn);

      EnumIsdnVoiceChannelState GetVoiceChannelStatus(INT32 cic);

      BOOLEAN ConnectSpeech(MpRequestDetails);

      BOOLEAN DisconnectSpeech(MpRequestDetails);

   private:
      BOOLEAN InitGc();

      BOOLEAN DeInitGc();

      BOOLEAN ResetLineDevice(INT32, unsigned long);
      
      BOOLEAN ReceiveCall(IsdnChannel* const, unsigned long);

      BOOLEAN HandleOfferedCall(IsdnChannel* const, GenIsupMsg &);

      ScheduledConnect mScheduledConnect[ISDN_MAX_GC_DEVICE];

   protected:
      
      BOOLEAN GetDeviceInfo();
      BOOLEAN VirtualBoardCount(AUID, int);
      BOOLEAN Subdevices(TEXT*, int, int);
      void    PrintDevice();

      TEXT    mLogText[MAX_LOG_TEXT_LEN];
   
   private:

      UINT16  mMinCkt;
      UINT16  mMaxCkt;

      int     mShmChnlMgmtId;
         
      IsdnConfigApi mIsdnConfigApi;
      DeviceInfo    mDeviceInfo;
      
      IsdnChannel   mIsdnChannel[ISDN_MAX_GC_DEVICE];
      
      long          mDevHdlsForSrWait[ISDN_MAX_GC_DEVICE];
      int           mCountDevHdlsForSrWait;

      ChannelStatus *mpIsdnChnlStatus;
};

#endif

