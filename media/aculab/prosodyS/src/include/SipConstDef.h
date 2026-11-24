// @(#) $Id: SipConstDef.h,v 1.1.4.1.2.1 2016/07/05 02:58:14 mram Exp $
//----------------------------------------------------------------------
// NAME
//       SipConstDef.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2015
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION :
//
// AUTHOR         : Sip                                  23-Feb-2015
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_SIP_CONST_DEF_H
#define INCL_SIP_CONST_DEF_H

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
#include <openssl/md5.h>

#include "acu_type.h"
#include "res_lib.h"
#include "cl_lib.h"
#include "sw_lib.h"
#include "iptel_lib.h"
#include "smclib.h"
#include "smbesp.h"
#include "smdrvr.h"
#include "smrtp.h"
#include "smbesp.h"
#include "smhlib.h"
#include "smwavlib.h"

#include "Types.h"
#include "LogApi.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ProcessLock.h"
#include "CfgRead.h"
#include "HashTable.h"
#include "TCPSocket.h"

#include "SipLimits.h"
#include "SipLogCodes.h"
#include "Ss7ConstDef.h"
#include "IsupStructs.h"
//#include "SipMediaStructs.h"
//#include "MediaInterface.h"
//----------------------------------------------------------------------
#ifndef IPC_PERM
#define IPC_PERM  0666
#endif

#ifndef SIGCFG
#define SIGCFG    10
#endif

#ifndef SIGTRC
#define SIGTRC    12
#endif

#define  SIP_MAX_HASH_TABLE_ENTRY 2500

#define SIP_NAME                      "Sip"

//----------------------------------------------------------------------
// Traces
#define TRACE_SIP_EVR_WRITER_ENV    "TRACE_SIP_EVR_WRITER"
#define TRACE_SIP_HDLR_ENV        "TRACE_SIP_HDLR"

//----------------------------------------------------------------------
// Config files
#define SIP_IPC_CFG                   "ipc.cfg"
#define SIP_KER_CFG                   "kernel.cfg"
#define SIP_SYS_CFG                   "system.cfg"
#define SIP_PRODUCT_CFG               "Product.cfg"
#define SIP_CDR_CFG                   "cdr.cfg"
#define SIP_PROV_EVR_CFG              "provEvr.cfg"

#define SIP_DB_ERR_SLEEP_TIME         100
#define SIP_DB_ERR_RETRY_ATTEMPTS     2
#define CDR_DATE_FORMAT               "%Y%m%d%H%M%S"
#define EVR_DATE_FORMAT               "%Y%m%d%H%M%S"

#define SIP_PRINT_SPACE \
   printf ("                ");

extern Log gLog;
extern CTrace gTrace;
extern TEXT gProcessName[];
#endif
