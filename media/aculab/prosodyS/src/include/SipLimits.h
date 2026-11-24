// @(#) $Id: SipLimits.h,v 1.1.4.1.2.3 2017/10/27 22:26:38 mram Exp $
//----------------------------------------------------------------------
// NAME
//       SipLimits.h
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
#ifndef INCL_SIP_LIMITS_DEF_H
#define INCL_SIP_LIMITS_DEF_H

#define SIP_MAX_PROCESS_NAME               50
#define SIP_MAX_REQ_KEY_LEN                50

#define SIP_MIN_SEM_KEY                    1000
#define SIP_MAX_SEM_KEY                    9999

#define SIP_MIN_SHM_KEY                    1000
#define SIP_MAX_SHM_KEY                    9999

#define SIP_MIN_MSG_Q_KEY                  1000
#define SIP_MAX_MSG_Q_KEY                  9999

#define SIP_MIN_BOOL                       0
#define SIP_MAX_BOOL                       1

#define SIP_MIN_EVR_EVENTS_LEN             2
#define SIP_MAX_EVR_EVENTS_LEN             100

#define SIP_MIN_CDR_EVENTS_LEN             1
#define SIP_MAX_CDR_EVENTS_LEN             100

#define SIP_MIN_MODULEID                   1
#define SIP_MAX_MODULEID                   99

#define SIP_MIN_GENERATE_CDR               0
#define SIP_MAX_GENERATE_CDR               3

#define SIP_MIN_GENERATE_EVR               0
#define SIP_MAX_GENERATE_EVR               255

#define SIP_MIN_DATE_FORMAT_LEN            2
#define SIP_MAX_DATE_FORMAT_LEN            20

#define SIP_MIN_LICENSE                    0
#define SIP_MAX_LICENSE                    2500

#define SIP_MIN_URI_LEN                    1
#define SIP_MAX_URI_LEN                    512 

#define SIP_MIN_TIMER_TABLE_SIZE           1
#define SIP_MAX_TIMER_TABLE_SIZE           20000

#define SIP_MIN_IP_LEN                     7
#define SIP_MAX_IP_LEN                     15

#define SIP_MIN_Q_FLUSH_CNT                1
#define SIP_MAX_Q_FLUSH_CNT                20000
#endif
