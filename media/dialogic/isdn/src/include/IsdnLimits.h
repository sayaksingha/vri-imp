// @(#) $Id: IsdnLimits.h,v 1.1.4.3 2016/04/12 21:59:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       IsdnLimits.h
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
// AUTHOR         : Chethan V M               25/07/2013
//----------------------------------------------------------------------
#ifndef INCL_ISDN_LIMITS_DEF_H
#define INCL_ISDN_LIMITS_DEF_H

#define ISDN_MAX_PROCESS_NAME               50

#define ISDN_MIN_SEM_KEY                    1000
#define ISDN_MAX_SEM_KEY                    9999

#define ISDN_MIN_SHM_KEY                    1000
#define ISDN_MAX_SHM_KEY                    9999

#define ISDN_MIN_MSG_Q_KEY                  1000
#define ISDN_MAX_MSG_Q_KEY                  9999

#define ISDN_MIN_BOOL                       0
#define ISDN_MAX_BOOL                       1

#define ISDN_MIN_EVR_EVENTS_LEN             2
#define ISDN_MAX_EVR_EVENTS_LEN             100

#define ISDN_MIN_CDR_EVENTS_LEN             1
#define ISDN_MAX_CDR_EVENTS_LEN             100

#define ISDN_MIN_MODULEID                   1
#define ISDN_MAX_MODULEID                   99

#define ISDN_MIN_GENERATE_CDR               0
#define ISDN_MAX_GENERATE_CDR               3

#define ISDN_MIN_GENERATE_EVR               0
#define ISDN_MAX_GENERATE_EVR               255

#define ISDN_MIN_DATE_FORMAT_LEN            2
#define ISDN_MAX_DATE_FORMAT_LEN            20

#define ISDN_MIN                            1
#define ISDN_MAX                            20

#define MIN_DIALOGIC_OUTGOING_CALL_WAIT_TIME 10
#define MAX_DIALOGIC_OUTGOING_CALL_WAIT_TIME 600

#endif
