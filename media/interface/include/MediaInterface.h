// @(#) $Id: MediaInterface.h,v 1.1.2.4.2.4 2018/02/26 23:17:59 mram Exp $
//----------------------------------------------------------------------------
// NAME :
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2010
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// ORIGINATED
//----------------------------------------------------------------------------
#include <cstring>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
using namespace std;

#include <signal.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/poll.h>
// Included to support md5 encoding of Password
#include <openssl/md5.h>

#include "Types.h"
//#include "Log.h"
#include "LogApi.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ProcessLock.h"
#include "CfgRead.h"
//#include "OamLogCodes.h"
//#include "TssDbMgr.h"
//#include "DbConstDef.h"
//#include "HashTable.h"
//#include "TCPSocket.h"
//#include "listenerTypes.h"
//#include "GenericAseStruct.h"
#include "StrTokenizer.h"

#include "MediaUtil.h"
#include "IvrConstDef.h"
#define MEDIA_IPC_CFG                           "ipc.cfg"
#define MEDIA_KER_CFG                           "kernel.cfg"
#define MEDIA_MIN_MSG_Q_KEY                     1000
#define MEDIA_MAX_MSG_Q_KEY                     9999
#define MEDIA_MIN_CHAN_PER_CARD                 1
#define MEDIA_MAX_CHAN_PER_CARD                 2048

#ifndef IPC_PERM
#define IPC_PERM  0666
#endif

#ifndef INCL__IVR_MEDIA_LIB_H
#define INCL__IVR_MEDIA_LIB_H

#define MAX_DTMF_STOP_PLAY_CHAR_SET             25
#define MAX_DTMF_STOP_RECORD_CHAR_SET           25
#define MAX_SYNWAY_CHANNEL                      720
#define MAX_SYNWAY_ERR_MSG                      500


#define SYNWAY_MIN_FILE_FORMAT_LEN              1
#define SYNWAY_MAX_FILE_FORMAT_LEN              10
#define SYNWAY_MIN_TIMEOUT_VAL                  1
#define SYNWAY_MAX_TIMEOUT_VAL                  4000000
#define MIN_SYNWAY_GET_DTMF_DIGIT_ASCII         0
#define MAX_SYNWAY_GET_DTMF_DIGIT_ASCII         127


#define MAX_VOICE_FILES                         25
#define MAX_VOICEFILE_LEN                       100
#define MAX_DTMF_RSP_DIGITS                     50
#define MAX_MSISDN_LEN                          21

#define MIN_DTMF_LEN                            1
//#define MAX_DTMF_LEN                            2
//
#include <iostream>
using namespace std;

//# Reset
#define C_COLOR_OFF          "\33[0m"

//# Regular Colors
#define C_BLACK              "\33[0;30m"
#define C_RED                "\33[0;31m"
#define C_GREEN              "\33[0;32m"
#define C_YELLOW             "\33[0;33m"
#define C_BLUE               "\33[0;34m"
#define C_PURPLE             "\33[0;35m"
#define C_CYAN               "\33[0;36m"
#define C_WHITE              "\33[0;37m"

//# Bold
#define C_BBLACK             "\33[1;30m"
#define C_BRED               "\33[1;31m"
#define C_BGREEN             "\33[1;32m"
#define C_BYELLOW            "\33[1;33m"
#define C_BBLUE              "\33[1;34m"
#define C_BPURPLE            "\33[1;35m"
#define C_BCYAN              "\33[1;36m"
#define C_BWHITE             "\33[1;37m"

enum EnumStatus
{
   MP_RSP_SUCCESS            = 0,
   MP_RSP_FAILURE,
   MP_RSP_BUSY,
   MP_RSP_INVALID_CHANNEL,
   MP_RSP_UNKNOWN_STATUS
};

enum EnumMediaCmdTypes
{
   MC_INVAL_CMD_TYPE = 0,                              // Invalid Commamd
   MC_CNTTN,                                          // Play
   MC_DISTN,                                          // Stop
   MC_CNTSP,                                          // Connect speech
   MC_DISSP,                                          // Disconnect speech
   MC_GETDIG,
   MC_GETDIGSTP,
   MC_RECSTR,                                         //Record Start
   MC_RECSTP,                                         //Record Stop
   MC_SETVOL,
   MC_SETSPD,
   MC_PAUSE,
   MC_RESUME,
   MC_FORWARD,
   MC_REWIND,
   MC_STREAM,
   MC_STREAM_STOP
};

enum EnumVoiceProcessErr
{
   MP_SUCCESS                    = 0,
   MP_CFG_READ_ERR               = 1,
   MP_BOARD_INIT_ERR             = 2,
   MP_CKT_UNAVAILABLE_ERR        = 3,
   MP_PLAY_STOP_CMD_ERR          = 4,
   MP_RECORD_STOP_CMD_ERR        = 5,
   MP_PLAY_CMD_ERR               = 6,
   MP_BADPARM_ERR                = 7,
   MP_CKT_BUSY_ERR               = 8,
   MP_RECORD_CMD_ERR             = 9,
   MP_GET_DIG_CMD_ERR            = 10,
   MP_CANCEL_GET_DIG_CMD_ERR     = 11,
   MP_CONNECT_CMD_ERR            = 12,
   MP_DEINIT_ERR                 = 13,
   MP_ADJ_VOL_SPD_ERR            = 14,
   MP_INVALID_CMD_ERR            = 15,
   MP_INVALID_CHANNEL            = 16,
   MP_GET_DIG_CMD_ALREADY_DONE   = 17,
   MP_PAUSE_CMD_ERR              = 18,
   MP_RSUME_CMD_ERR              = 19,
   MP_FORWARD_CMD_ERR            = 20,
   MP_REWIND_CMD_ERR             = 21,
   MP_INIT_ERR                   = 22
};

enum EnumChannelStatus
{
   CKT_UNAVAILABLE               = 0,
   CKT_IDLE                      = 1,
   CKT_PLAYING                   = 2,
   CKT_PLAY_STOPING              = 3,
   CKT_RECORD_STOPING            = 4,
   CKT_GETDIG                    = 5,
   CKT_RECORDING                 = 6,
   CKT_PATCHED                   = 7,
   CKT_PLAYING_AND_GETDIG        = 8,
   CKT_RECORDING_AND_GETDIG      = 9,
   CKT_PLAY_STOPING_AND_GETDIG   = 10,
   CKT_RECORD_STOPING_AND_GETDIG,
    //If U Add Any More States Add that in Timer Check Thread also
   CKT_INITIALIZED,   //newly added for sip media handling
   CKT_ALERTING,   //newly added for sip  handling
   CKT_RINGING   //newly added for sip  handling
};

enum EnumMediaFileFormat
{
   MEDIA_WAV   = 0,
   MEDIA_VOX
};

enum EnumCompType
{
   COMPANDING_ALAW = 1,
   COMPANDING_MULAW = 2
};

enum EnumCktCmd
{
   CKT_CMND_BLOCK,
   CKT_CMND_UNBLOCK,
   CKT_CMND_REL
};

enum EnumCktCmdStatus
{
   CKT_CMND_BLOCKED,
   CKT_CMND_UNBLOCKED,
   CKT_CMND_RELEASED
};

struct playControls
{
   TEXT    pauseDtmf;
   TEXT    resumeDtmf;
   TEXT    forwardDtmf;
   TEXT    rewindDtmf;
};

struct MpPlay
{
   unsigned int      seqNum;
   UINT16      circuitId;
   BOOLEAN     dtmfEnable;//enable dtmf Detection While Playing before Calling MpGetdigit()
   BOOLEAN     repeatPlay;
   UINT8       vFileCount;
   TEXT        dtmfStopPlayCharSet[MAX_DTMF_STOP_PLAY_CHAR_SET];
   TEXT        voiceFile[MAX_VOICE_FILES][MAX_VOICEFILE_LEN];

   EnumMediaFileFormat fileFormat;

   BOOLEAN      playCtrlEnable; //To work play controls dtmfEnable flag should be true
   playControls playCtrl;

   BOOLEAN      nonBlocking;
};

struct MpStream
{
   unsigned int seqNum;
   UINT16      circuitId;
   TEXT        rtspUrl[240];
};

struct MpStop
{
   unsigned int seqNum;
   UINT16     circuitId;
};

struct MpGetDigit
{
   unsigned int seqNum;
   UINT16     circuitId;
   UINT16     numOfDig;
   UINT32     interDigitTimeOut; //in milli seconds  or Silence between Digits
   TEXT       teminateChars[20];
   BOOLEAN    includeTerminateChar; //true -> include, false -> not include
   UINT32     maxTime; //Maximum time to collect all Digits //not supported in Synway

   TEXT       rspDigits[MAX_DTMF_RSP_DIGITS];
};

struct MpSwitch
{
   unsigned int seqNum;
   UINT16    circuitId_1;
   UINT16    circuitId_2;
   TEXT      recordFile[MAX_VOICEFILE_LEN];
};

struct MpRecord
{
   unsigned int seqNum;
   UINT16    circuitId;
   UINT32    recTime; //in milli seconds
   TEXT      voiceFile[MAX_VOICEFILE_LEN];
   TEXT      dtmfStopRecCharSet[MAX_DTMF_STOP_RECORD_CHAR_SET];
   EnumMediaFileFormat fileFormat;
};

struct MpSetVolSpd
{
   unsigned int seqNum;
   UINT16    circuitId;
   UINT16    adjSize;
};

struct MpPause
{
   unsigned int seqNum;
   UINT16    circuitId;
};

struct MpResume
{
   unsigned int seqNum;
   UINT16    circuitId;
};

struct MpForward
{
   unsigned int seqNum;
   UINT16    circuitId;
};

struct MpRewind
{
   unsigned int seqNum;
   UINT16    circuitId;
};

struct MpDummy
{
   unsigned int seqNum;
   UINT16   circuitId;
};

union MpData
{
   MpPlay         playMsg;
   MpStream       stream;
   MpStop         stopMsg;
   MpGetDigit     getDigit;
   MpRecord       recordMsg;
   MpSwitch       switchChnl;
   MpSetVolSpd    setVolSpd;
   MpPause        pauseMsg;
   MpResume       resumeMsg;
   MpRewind       rewindMsg;
   MpForward      forwardMsg;
   MpDummy        dummy;
};

struct MpRequestDetails
{
   EnumMediaCmdTypes       cmdType;
   MpData                  mpReqData;
   TEXT                    clgNumber[MAX_MSISDN_LEN];
   TEXT                    cldNumber[MAX_MSISDN_LEN];
};

struct MpResponseDetails
{
   EnumStatus              rspMsg;
   EnumMediaCmdTypes       cmdType;
   MpData                  mpRspData;
   EnumVoiceProcessErr     voiceProcessErr;
};

struct CktCmdReq
{
   UINT16       channelId;
   EnumCktCmd   cktCmd;
};

struct CktCmdRsp
{
   UINT16             channelId;
   EnumCktCmdStatus   status;
};

extern Log gLog;
extern CTrace gTrace;
extern TEXT gProcessName[];
//extern HealthCheckApi gHealthCheckObj;

class MediaInterface
{

   //Request structure
   //Response Structure

   key_t    RdMsgQKey;  
   key_t    WrMsgQKey;  
   MsgQueue RdResponseQ;        
   MsgQueue WrRequestQ;        
   INT16    mMaxNoOfChanPerCard;
   BOOLEAN  mStopRequested;
   BOOLEAN  ValidateDtmfAndSetOperation(MpRequestDetails, MpResponseDetails, TEXT*, TEXT*, TEXT*, TEXT*);
   BOOLEAN  SendReqAndRcvResp(MpRequestDetails);
   BOOLEAN  SendPauseReqAndRcvResp(MpRequestDetails);
   BOOLEAN  SendResumeReqAndRcvResp(MpRequestDetails);
   BOOLEAN  SendRewindReqAndRcvResp(MpRequestDetails);
   BOOLEAN  SendForwardReqAndRcvResp(MpRequestDetails);

   public:

   MediaInterface();
   ~MediaInterface();
   BOOLEAN Init();
   BOOLEAN ReadConfig();
   BOOLEAN CreateMsgQ();
   BOOLEAN GetMsgType(UINT16 & channelId, long &lMsgType); 
   BOOLEAN PlayBlockingMode(MpRequestDetails &lReq,MpResponseDetails &lResp);
   BOOLEAN PlayNonBlockingMode(MpRequestDetails &lReq);
   BOOLEAN GetDtmfDigits(MpRequestDetails &lReq,MpResponseDetails &lResp);
   BOOLEAN GetDtmfDigits(MpRequestDetails &lReq,MpResponseDetails &lResp, TEXT *pauseDtmf, TEXT *resumeDtmf, TEXT *fwdDtmf, TEXT *rwndDtmf);
   BOOLEAN RecordVoice(MpRequestDetails &lReq,MpResponseDetails &lResp);
   BOOLEAN SwitchCalls(MpRequestDetails &lReq,MpResponseDetails &lResp);
   BOOLEAN StopProcessing(MpRequestDetails &lReq,MpResponseDetails &lResp);
   BOOLEAN RxResponse(const UINT16 lMsgType, MpResponseDetails &lResp,modeOption &lMode);
   BOOLEAN RxResponseNonBlocking(const UINT16 lMsgType, MpResponseDetails &lResp,int noOfAttempts=5);
   BOOLEAN TxRequest(MpRequestDetails &lReq,long &lMsgType);
   BOOLEAN PrepareForNewCall();
};
#endif
