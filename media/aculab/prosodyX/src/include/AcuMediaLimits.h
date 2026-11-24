// @(#) $Id: AcuMediaLimits.h,v 1.1.4.3 2016/04/12 21:59:27 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// AcuMediaLimits.h
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
// Originated :                                              21-JUL-2012
//----------------------------------------------------------------------
#ifndef INCL_ACU_MEDIA_LIMITS_DEF_H
#define INCL_ACU_MEDIA_LIMITS_DEF_H

#define ACULAB_MAX_PROCESS_NAME               50

#define ACULAB_MIN_SEM_KEY                    1000
#define ACULAB_MAX_SEM_KEY                    9999

#define ACULAB_MIN_SHM_KEY                    1000
#define ACULAB_MAX_SHM_KEY                    9999

#define ACULAB_MIN_MSG_Q_KEY                  1000
#define ACULAB_MAX_MSG_Q_KEY                  9999

#define ACULAB_MIN_BOOL                       0
#define ACULAB_MAX_BOOL                       1

#define MAX_KEYS                              5
#define KEY_LENGTH                            15 
#define VAL_LENGTH                            15

#define MIN_NUM_OF_ACU_CARDS                  1
#define MAX_NUM_OF_ACU_CARDS                  20

#define CKT_MIN                               0
#define CKT_MAX                               1000

#define ACULAB_MIN_CHANNEL_ID                 0
#define ACULAB_MAX_CHANNEL_ID                 2048

#define ACULAB_MIN_CHAN_PER_CARD              1
#define ACULAB_MAX_CHAN_PER_CARD              2048

#define IVR_MAX_DB_NAME_LEN                       25
#define MAX_DB_USER_NAME                      25
#define MAX_DB_PASSWD_LEN                     25
#define MAX_QTYPE_LEN                         25
#define MAX_QUERY_LEN                         500
#define MAX_VARIABLE_LEN                      200
#define MAX_NO_OF_VARIABLES                   100

#define MAX_PEG_EVENTS                        100

#define MAX_NUM_OF_CKT_CMDS                   20

#define MEDIA_TRACE_MIN                       0
#define MEDIA_TRACE_MAX                       5

#define TING_TRACE_MIN                        0
#define TING_TRACE_MAX                        5

#define MIN_INSTANCES                         1
#define MAX_INSTANCES                         4

#define MIN_CHANNELS                          60
#define MAX_CHANNELS                          240

#endif
