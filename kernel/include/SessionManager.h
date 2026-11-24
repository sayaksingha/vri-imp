// @(#) $Id: SessionManager.h,v 1.1.1.1.2.6.2.2 2018/01/17 23:03:43 mram Exp $
//---------------------------------------------------------------
// NAME      SessionManager.h
//
// COPYRIGHT
// BITL -- Copyright (C) 2001 BPL Innovision Technologies Ltd.,
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// BPL Innovision Technologies Ltd.
//
//
// DESCRIPTION
// This implementation file is for the CallProcessing Module.
//
// Originated : Ramprasad M                       Date:11/11/2010
//---------------------------------------------------------------
#ifndef INCL_SES_MAN_H
#define INCL_SES_MAN_H

#include "IvrUtil.h"
#include "MediaInterface.h"
#include "BillingApi.h"
#include "IvrConfigApi.h"
#include "IvrRedisUtil.h"

enum EnumCallState
{
	ST_NO_STATE,
	ST_CCXML_ALT_SND,
	ST_ACM_SND,
	ST_ANM_SND,
	ST_CCXML_CONN_SND,
	ST_VXML_REQ_SND,
	ST_CCXML_DLG_EXT_SND,
	ST_REL_SND,
	ST_CCXML_DISCO_SND,
	ST_EXT_VXML_PLY_SND,
	ST_EXT_VXML_TERM_SND,
	ST_EXT_VXML_PTH_SND,
	ST_EXT_VXML_TEAR_SND,
   ST_DIAL_OUT_IAM_SND,
   ST_ACM_RCVD,
   ST_CCXML_CALL_CREATED,
   ST_CCXML_CALL_JOIN,
   ST_CCXML_CALL_UNJOIN
};

enum EnumCallEvent
{
	EVT_NULL,
	EVT_IAM,
	EVT_CCXML_ALT_ACK_RCV,
	EVT_CCXML_ALT_NACK_RCV,
	EVT_CCXML_DISCONN_RCV,
	EVT_CCXML_DLG_STR_RCV,
	EVT_EXT_PLY_RCV,
	EVT_EXT_STP_RCV,
	EVT_EXT_PTH_RCV,
	EVT_EXT_TEAR_RCV,
	EVT_VXML_RSP_RCV,
	EVT_CCXML_EXT_RCV,
	EVT_REL,
   EVT_DIAL_OUT,
   EVT_ACM,
   EVT_ANM,
   EVT_CCXML_JOIN,
   EVT_CCXML_UNJOIN,
	EVT_TERMINATE_CHILD,
   EVT_CPG
};

enum EnumCallAction
{
	ACT_NULL,
	ACT_SND_CCXML_ALT,
	ACT_SND_ACM,
	ACT_SND_ANM,
	ACT_SND_CCXML_CONN,
	ACT_SND_VXML_REQ,
	ACT_SND_DLG_EXT,
	ACT_SND_REL,
	ACT_SND_CCXML_DISCO,
	ACT_SND_VXML_TERM,
	ACT_TERMINATE_CHILD,
	ACT_EXIT,
	ACT_SND_EXT_DLG_EXT,
	ACT_SND_EXT_VXML_TERM,
	ACT_SND_EXT_PTH_CKT,
	ACT_SND_EXT_TEAR_CKT,
	ACT_SND_EXT_TERM_RSP,
   ACT_SND_IAM,
   ACT_STATE_ACM,
   ACT_SND_CALL_CREATED,
   ACT_CCXML_JOIN_CALLS,
   ACT_CCXML_JOIN_RSP,
   ACT_CCXML_UNJOIN_CALLS,
   ACT_CCXML_UNJOIN_RSP,
   ACT_SND_RLC
};

struct State
{
	EnumCallEvent     evt;
	UINT8             noOfActions;
	EnumCallAction    fid[MAX_ACTION_WITHIN_EVENT];
};

bool SendDoNothing(); 

bool SendAcm();

bool SendAnm();

bool SendRel();

bool SendRlc();

bool SendIam();

bool ChangeStateToAcm();

bool SendCallCreated(); 

bool JoinCcxmlCalls();

bool SendCcxmlJoinRsp();

bool UnjoinCcxmlCalls();

bool SendCcxmlUnjoinRsp();

bool TerminateChild();

bool ExitChild();

bool Init();

void ReloadConfig();

BOOLEAN ReadCallAlertFromQueue();

void ClearChildQueue();

BOOLEAN SendCallAlertToChild();

BOOLEAN ReadMsgFromChildQ();

EnumCallEvent GetEvent();

EnumCallEvent GetIsupEvent();

EnumCallEvent GetCcxmlEvent();

EnumCallEvent GetVxmlEvent();

EnumCallEvent GetExtEvent();

BOOLEAN SendCcxmlCallAlert();

BOOLEAN SendCcxmlCallConnected();

BOOLEAN SendVxmlRequest();

BOOLEAN SendCcxmlDlgExit();

BOOLEAN SendExternalDlgExit();

BOOLEAN SendAcm();

BOOLEAN SendAnm();

BOOLEAN SendRel();

BOOLEAN SendCcxmlDisconnect();

BOOLEAN SendVxmlTerminate();

BOOLEAN SendExtVxmlTerminate();

BOOLEAN SendExtVxmlTerminateRsp();

BOOLEAN SendExtPatchCircuit();

BOOLEAN SendExtTearCircuit();

void SendTermChild();

void SendReleaseCall();

BOOLEAN ExitChild();

BOOLEAN TerminateChild();

BOOLEAN SendDoNothing();

TEXT* ConvertStateIdentifier(UINT8 Identifier);

TEXT* ConvertEventIdentifier(UINT8 Identifier);

void GetSystemTime(TEXT *Buf);

BOOLEAN  ReadKernelCfg();

BOOLEAN ReadConfigData();

BOOLEAN  ReadSs7Cfg();

void MonitorChannelInstance(int sigNum);

BOOLEAN GenerateEvr();

BOOLEAN ProcessCallCharge(EnumBillingEvents billingEvents);

BOOLEAN ValidateCall(CcxmlRequest &ccxmlReq, INT32 rRoutedCall=0);

BOOLEAN  NumberNormalize(TEXT  *,TEXT *);
#endif
