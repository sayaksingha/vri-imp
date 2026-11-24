// @(#) $Id: HmpConstDef.h,v 1.1.2.1 2012/02/21 08:31:34 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpConstDef.h
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
// AUTHOR         : Hmp                                  23-Nov-2011
//
// 1.MODIFIED BY  : <Name>                                <Date>
//   MODIFICATION : <Details>
//----------------------------------------------------------------------
#ifndef INCL_HMP_CONST_DEF_H
#define INCL_HMP_CONST_DEF_H

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
#include <sys/stat.h>
#include <fcntl.h>
#include "errno.h"

#include "RtspClient.h"
#include "Types.h"
#include "Log.h"
#include "LogApi.h"
#include "ctrace.h"
#include "MsgQueue.h"
#include "ProcessLock.h"
#include "CfgRead.h"
#include "OamLogCodes.h"
#include "DbConstDef.h"
#include "HashTable.h"

#include "HmpLimits.h"
#include "HmpLogCodes.h"
#include "OamHealthCheckApi.h"

/* Dialogic Header Files */
#include <stdio.h>
#include <srllib.h>
#include <ipmlib.h>
#include "dxxxlib.h"
#include <devmgmt.h>
#include <dtilib.h>



//----------------------------------------------------------------------
#ifndef HMP_IPC_PERM
#define HMP_IPC_PERM                   0666
#endif

#ifndef SIGCFG
#define SIGCFG                         10
#endif

#ifndef SIGTRC
#define SIGTRC                         12
#endif

#define HMP_NAME                       "Hmp"

#define HMP_FAILURE                    -1
#define HMP_SUCCESS                    0

//----------------------------------------------------------------------
// Traces
#define TRACE_HMP_EVR_WRITER_ENV    "TRACE_HMP_EVR_WRITER"
#define TRACE_HMP_ENV               "TRACE_HMP"

//----------------------------------------------------------------------
// Config files
#define HMP_IPC_CFG                   "ipc.cfg"
#define HMP_KER_CFG                   "kernel.cfg"
#define HMP_SYS_CFG                   "system.cfg"
#define HMP_PRODUCT_CFG               "Product.cfg"
#define HMP_CDR_CFG                   "cdr.cfg"
#define HMP_PROV_EVR_CFG              "provEvr.cfg"

#define HMP_DB_ERR_SLEEP_TIME         100
#define HMP_DB_ERR_RETRY_ATTEMPTS     2
#define CDR_DATE_FORMAT               "%Y%m%d%H%M%S"
#define EVR_DATE_FORMAT               "%Y%m%d%H%M%S"


#define DX_DEVICE_NAME_LENGTH         100
#define IPM_DEVICE_NAME_LENGTH        100
#define DTI_DEVICE_NAME_LENGTH        100
#define MAX_DX_DEVICE                 600
#define MAX_DTI_DEVICE                600
#define MAX_IPM_DEVICE                600
#define MAX_BOARD_NAME_LEN            100
#define HMP_MAX_REQ_KEY_LEN           20 
#define HMP_MAX_HASH_TABLE_ENTRY      5000
#define HMP_MAX_CONFIG_PARAMS         100
#define HMP_MAX_CONFIG_PARAM_LEN      30 
#define HMP_MAX_CONFIG_VALUE_LEN      100
#define DX_MAXIMUM_FILES              25             
#define MAX_DTI_LSTN_IPM              50

#define MAX_CONNCURRENT_ACTION        25

#define HMP_PRINT_SPACE \
   printf ("                ");

extern Log gLog;
//extern CTrace gTrace;
extern TEXT gProcessName[];
extern HealthCheckApi gHealthCheckObj;
#endif
