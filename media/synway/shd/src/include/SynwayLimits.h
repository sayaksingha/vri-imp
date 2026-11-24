// @(#) $Id: SynwayLimits.h,v 1.1.4.3 2016/04/12 21:59:38 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// IvrLimits.h
//
// COPYRIGHT
// Tayana Software Solutions Pvt Ltd -- Copyright(C) 2007
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Pvt Ltd
//
// DESCRIPTION
//
// Originated :                                              21-OCT-2010
//----------------------------------------------------------------------
#ifndef INCL_SYNWAY_LIMITS_DEF_H
#define INCL_SYNWAY_LIMITS_DEF_H

#define SYNWAY_MAX_PROCESS_NAME               50

#define SYNWAY_MIN_SEM_KEY                    1000
#define SYNWAY_MAX_SEM_KEY                    9999

#define SYNWAY_MIN_SHM_KEY                    1000
#define SYNWAY_MAX_SHM_KEY                    9999

#define SYNWAY_MIN_MSG_Q_KEY                  1000
#define SYNWAY_MAX_MSG_Q_KEY                  9999

#define SYNWAY_MIN_BOOL                       0
#define SYNWAY_MAX_BOOL                       1

#define MAX_KEYS                           5

#ifndef KEY_LENGTH
#define KEY_LENGTH                         15 
#endif

#ifndef VAL_LENGTH
#define VAL_LENGTH                         15
#endif

#ifndef CKT_MIN
#define CKT_MIN                            0
#endif

#define CKT_MAX                            1000

#define SYNWAY_MIN_CHANNEL_ID                 0
#define SYNWAY_MAX_CHANNEL_ID                 2048

#define SYNWAY_MIN_CHAN_PER_CARD              1
#define SYNWAY_MAX_CHAN_PER_CARD              2048

#ifndef IVR_MAX_DB_NAME_LEN
#define IVR_MAX_DB_NAME_LEN                    25
#endif

#define MAX_DB_USER_NAME                   25
#define MAX_DB_PASSWD_LEN                  25
#define MAX_QTYPE_LEN                      25
#define MAX_QUERY_LEN                      500

#ifndef MAX_VARIABLE_LEN
#define MAX_VARIABLE_LEN                   200
#endif

#define MAX_NO_OF_VARIABLES                100

#define MAX_PEG_EVENTS                     100

#define SYNWAY_MIN_FILE_PATH_LEN           1

#define SYNWAY_MAX_FILE_PATH_LEN           200

#define MAX_NUM_OF_CKT_CMDS                20

#define MIN_FILE_PROGRESS_CHECK            10
#define MAX_FILE_PROGRESS_CHECK            600


#define MIN_SYNWAY_MAX_CALL_DURATION       1
#define MAX_SYNWAY_MAX_CALL_DURATION       30
#endif
