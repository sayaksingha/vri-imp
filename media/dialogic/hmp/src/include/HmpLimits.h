// @(#) $Id: HmpLimits.h,v 1.1.2.1 2012/02/21 08:31:35 shijuc Exp $
//----------------------------------------------------------------------
// NAME
//       HmpLimits.h
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
#ifndef INCL_HMP_LIMITS_DEF_H
#define INCL_HMP_LIMITS_DEF_H

#define HMP_MAX_PROCESS_NAME               50

#define HMP_MIN_SEM_KEY                    1000
#define HMP_MAX_SEM_KEY                    9999

#define HMP_MIN_SHM_KEY                    1000
#define HMP_MAX_SHM_KEY                    9999

#define HMP_MIN_MSG_Q_KEY                  1000
#define HMP_MAX_MSG_Q_KEY                  9999

#define HMP_MIN_BOOL                       0
#define HMP_MAX_BOOL                       1

#define HMP_MIN_EVR_EVENTS_LEN             2
#define HMP_MAX_EVR_EVENTS_LEN             100

#define HMP_MIN_CDR_EVENTS_LEN             1
#define HMP_MAX_CDR_EVENTS_LEN             100

#define HMP_MIN_MODULEID                   1
#define HMP_MAX_MODULEID                   99

#define HMP_MIN_GENERATE_CDR               0
#define HMP_MAX_GENERATE_CDR               3

#define HMP_MIN_GENERATE_EVR               0
#define HMP_MAX_GENERATE_EVR               255

#define HMP_MIN_DATE_FORMAT_LEN            2
#define HMP_MAX_DATE_FORMAT_LEN            20

#define HMP_MIN                            1
#define HMP_MAX                            20

#define HMP_MIN_IPM_RESOURSE               0  
#define HMP_MAX_IPM_RESOURSE               240
#endif
