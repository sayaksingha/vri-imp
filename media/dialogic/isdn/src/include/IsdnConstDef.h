// @(#) $Id: IsdnConstDef.h,v 1.1.4.3 2016/04/12 21:59:34 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       IsdnConstDef.h
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
// AUTHOR         : Chethan V M                      25/07/2013
//----------------------------------------------------------------------
#ifndef INCL_ISDN_CONST_DEF_H
#define INCL_ISDN_CONST_DEF_H

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

#include "IsdnLimits.h"
#include "IsdnLogCodes.h"
#include "OamHealthCheckApi.h"

/* Dialogic Header Files */
#include <stdio.h>
#include <srllib.h>
#include "dxxxlib.h"
#include <devmgmt.h>
#include <dtilib.h>
#include <gcip.h>
#include <gclib.h>
#include <gcerr.h>
#include <gcisdn.h>

#include "sctools.h"

#include "Ss7ConstDef.h"
#include "IsupStructs.h"

//----------------------------------------------------------------------
#ifndef ISDN_IPC_PERM
#define ISDN_IPC_PERM                   0666
#endif

#ifndef SIGCFG
#define SIGCFG                         10
#endif

#ifndef SIGTRC
#define SIGTRC                         12
#endif

#define ISDN_NAME                       "Isdn"

#define ISDN_FAILURE                    -1
#define ISDN_SUCCESS                    0

//----------------------------------------------------------------------
// Config files
#define ISDN_IPC_CFG                   "ipc.cfg"
#define ISDN_KER_CFG                   "kernel.cfg"
#define ISDN_SYS_CFG                   "system.cfg"
#define ISDN_PRODUCT_CFG               "Product.cfg"
#define ISDN_CDR_CFG                   "cdr.cfg"
#define ISDN_PROV_EVR_CFG              "provEvr.cfg"
#define ISDN_MEDIA_CFG                 "media.cfg"

#define ISDN_DB_ERR_SLEEP_TIME         100
#define ISDN_DB_ERR_RETRY_ATTEMPTS     2
#define CDR_DATE_FORMAT               "%Y%m%d%H%M%S"
#define EVR_DATE_FORMAT               "%Y%m%d%H%M%S"


#define ISDN_DX_DEVICE_NAME_LENGTH         50
#define ISDN_DTI_DEVICE_NAME_LENGTH        50
#define ISDN_GC_DEVICE_NAME_LENGTH         50
#define ISDN_DEVICE_NAME_LENGTH            50

#define ISDN_DEVICE_NAME_LENGTH            50
#define ISDN_MAX_PHY_BOARD                 50
#define ISDN_MAX_VIRTUAL_BOARD             100
#define ISDN_MAX_SUB_DEVICES               500

#define ISDN_MAX_DX_DEVICE                 1200
#define ISDN_MAX_DTI_DEVICE                1200
#define ISDN_MAX_GC_DEVICE                 1200
#define ISDN_MAX_BOARD_NAME_LEN            100

#define ISDN_DX_MAXIMUM_FILES              25             

#define ISDN_MAX_CONNCURRENT_ACTION        25


#define ISDN_DX_FAILURE                    -1
#define ISDN_DTI_FAILURE                   -1

#define ISDN_DX_SUCCESS                    0

#define TRACE_ISDN_ENV                     "TRACE_ISDN_MEDIA_HDLR"

#define ISDN_INV_CHANNEL_NUMBER                -1

#define ISDN_PRINT_SPACE \
   printf ("                ");

extern Log gLog;
extern CTrace gTrace;
extern TEXT gProcessName[];
#endif
