// @(#) $Id: SspStates.h,v 1.1.4.3 2016/04/12 21:59:01 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// SspStates.h
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
// Originated : KOTHARI KOMAL                           Date:23-FEB-2012
//----------------------------------------------------------------------
#ifndef INCL_SSP_STATES_H
#define INCL_SSP_STATES_H

#include "SspProc.h"

SspStates NO_STATE[]       ={
                                 {SSP_EVT_DP2_RCVD,   1, {SSP_ACT_SND_DP2}},
                                 {SSP_EVT_DP12_RCVD,  1, {SSP_ACT_SND_DP12}},
                                 {SSP_EVT_NULL,       1, {SSP_ACT_DO_NOTHING}}
                                };


SspStates DP12_SENT[]={
                           {SSP_EVT_ABORT_FRM_IN,   2, {SSP_ACT_ABORT_TO_MSC,
                                                           SSP_ACT_EXIT
                                                          }                     },
                                 
                           {SSP_EVT_ABORT_FRM_MSC,  2, {SSP_ACT_ABORT_TO_IN,
                                                           SSP_ACT_EXIT
                                                          }                     },
                               
                           {SSP_EVT_BCSM_REQ,       1, {SSP_ACT_SND_EVT_BCSM_REQ}},
                                 
                           {SSP_EVT_APPLY_CHRG_REQ, 1, {SSP_ACT_SND_APPL_CHRG_REQ}},
                                 
                           {SSP_EVT_CALL_INFO_REQ,  1, {SSP_ACT_SND_CALL_INFO_REQ}},
                                 
                           {SSP_EVT_IN_ALL_REQ_RCVD,3, {SSP_ACT_SND_EVT_BCSM_REQ,
                                                           SSP_ACT_SND_APPL_CHRG_REQ,
                                                           SSP_ACT_SND_CALL_INFO_REQ
                                                          }                            },
                                 
                           {SSP_EVT_ACTV_TEST_REQ,  1, {SSP_ACT_SND_ACTV_TEST_REQ}},
                                 
                           {SSP_EVT_ACTV_TEST_RSP,  1, {SSP_ACT_SND_ACTV_TEST_RSP}},
                                 
                           //Change State after Continue
                           {SSP_EVT_CONTINUE_RCVD,  1, {SSP_ACT_SND_CONTINUE_TO_MSC}},
                           
                           //Change State after Connect
                           {SSP_EVT_CONN,           1, {SSP_ACT_SND_CONN}},
                                 
                           {SSP_EVT_IN_RELEASE,     2, {SSP_ACT_SND_RELEASE_TO_MSC,
                                                           SSP_ACT_EXIT
                                                          }                     },

                           {SSP_EVT_NULL,           1, {SSP_ACT_DO_NOTHING}}
                          };

SspStates DP2_SENT[]={
                          {SSP_EVT_ABORT_FRM_IN,   2, {SSP_ACT_ABORT_TO_MSC,
                                                          SSP_ACT_EXIT
                                                         }                     },
                                 
                          {SSP_EVT_ABORT_FRM_MSC,  2, {SSP_ACT_ABORT_TO_IN,
                                                          SSP_ACT_EXIT
                                                         }                     },
                                 
                          {SSP_EVT_BCSM_REQ,       1, {SSP_ACT_SND_EVT_BCSM_REQ}},
                                 
                          {SSP_EVT_APPLY_CHRG_REQ, 1, {SSP_ACT_SND_APPL_CHRG_REQ}},
                                 
                          {SSP_EVT_CALL_INFO_REQ,  1, {SSP_ACT_SND_CALL_INFO_REQ}},
                                 
                          {SSP_EVT_IN_ALL_REQ_RCVD,3, {SSP_ACT_SND_EVT_BCSM_REQ,
                                                          SSP_ACT_SND_APPL_CHRG_REQ,
                                                          SSP_ACT_SND_CALL_INFO_REQ
                                                         }                            },
                                 
                          {SSP_EVT_ACTV_TEST_REQ,  1, {SSP_ACT_SND_ACTV_TEST_REQ}},
                                 
                          {SSP_EVT_ACTV_TEST_RSP,  1, {SSP_ACT_SND_ACTV_TEST_RSP}},
                                 
                           //Change State after Continue
                          {SSP_EVT_CONTINUE_RCVD,  1, {SSP_ACT_SND_CONTINUE_TO_MSC}},
                          
                           //Change State after Connect
                          {SSP_EVT_CONN,           1, {SSP_ACT_SND_CONN}},
                                 
                          {SSP_EVT_IN_RELEASE,     2, {SSP_ACT_SND_RELEASE_TO_MSC,
                                                          SSP_ACT_EXIT
                                                         }                     },

                          {SSP_EVT_NULL,           1, {SSP_ACT_DO_NOTHING}}
                         };

SspStates EVENT_T_BCSM[]={
                              {SSP_EVT_ABORT_FRM_IN,   2, {SSP_ACT_ABORT_TO_MSC,
                                                              SSP_ACT_EXIT
                                                             }                     },
                                 
                              {SSP_EVT_ABORT_FRM_MSC,  2, {SSP_ACT_ABORT_TO_IN,
                                                                 SSP_ACT_EXIT
                                                                }                     },
                                 
                              {SSP_EVT_IN_RELEASE,     2, {SSP_ACT_DO_NOTHING,
                                                              SSP_ACT_EXIT
                                                             }                     },

                              {SSP_EVT_T_BUSY,         1, {SSP_ACT_SND_T_BUSY}},

                              {SSP_EVT_T_NO_ANS,       1, {SSP_ACT_SND_T_NO_ANS}},

                              {SSP_EVT_T_ANS,          1, {SSP_ACT_SND_T_ANS}},

                              {SSP_EVT_T_DISCONN,      1, {SSP_ACT_SND_T_DISCONN}},
                                 
                              {SSP_EVT_APPLY_CHRG_RSP, 1, {SSP_ACT_SND_APPL_CHRG_RSP}},
                                 
                              {SSP_EVT_CALL_INFO_RSP,  1, {SSP_ACT_SND_CALL_INFO_RSP}},
                                 
                              {SSP_EVT_ACTV_TEST_RSP,  1, {SSP_ACT_SND_ACTV_TEST_RSP}},
                                 
                              {SSP_EVT_CONTINUE_RCVD,  2, {SSP_ACT_DO_NOTHING,
                                                              SSP_ACT_EXIT
                                                             }                        },
                                 
                              {SSP_EVT_CONN,           1, {SSP_ACT_SND_CONN}},
                                 
                              {SSP_EVT_NULL,           1, {SSP_ACT_DO_NOTHING}}
                             };

SspStates EVENT_O_BCSM[]={
                              {SSP_EVT_ABORT_FRM_IN,   2, {SSP_ACT_ABORT_TO_MSC,
                                                              SSP_ACT_EXIT
                                                             }                     },
                                 
                              {SSP_EVT_ABORT_FRM_MSC,  2, {SSP_ACT_ABORT_TO_IN,
                                                              SSP_ACT_EXIT
                                                             }                     },
                                 
                              {SSP_EVT_IN_RELEASE,     2, {SSP_ACT_DO_NOTHING,
                                                              SSP_ACT_EXIT
                                                             }                     },

                              {SSP_EVT_O_BUSY,         1, {SSP_ACT_SND_O_BUSY}},

                              {SSP_EVT_O_NO_ANS,       1, {SSP_ACT_SND_O_NO_ANS}},

                              {SSP_EVT_O_ANS,          1, {SSP_ACT_SND_O_ANS}},

                              {SSP_EVT_O_DISCONN,      1, {SSP_ACT_SND_O_DISCONN}},
                                 
                              {SSP_EVT_APPLY_CHRG_RSP, 1, {SSP_ACT_SND_APPL_CHRG_RSP}},
                                 
                              {SSP_EVT_CALL_INFO_RSP,  1, {SSP_ACT_SND_CALL_INFO_RSP}},
                                 
                              {SSP_EVT_ACTV_TEST_RSP,  1, {SSP_ACT_SND_ACTV_TEST_RSP}},
                                 
                              {SSP_EVT_CONTINUE_RCVD,  2, {SSP_ACT_DO_NOTHING,
                                                              SSP_ACT_EXIT
                                                             }                        },
                                 
                              {SSP_EVT_CONN,           1, {SSP_ACT_SND_CONN}},
                                 
                              {SSP_EVT_NULL,           1, {SSP_ACT_DO_NOTHING}}
                             };

SspStates* gInRealyStateTable[] = {
                                         NO_STATE,
                                         DP12_SENT,
                                         DP2_SENT,
                                         EVENT_T_BCSM,
                                         EVENT_O_BCSM
                                      };

typedef bool (SspProc::*InRealyActions)(INT32, UINT32);

InRealyActions gInRealyActionTable[] = {
                                          &SspProc::DoNothing,
                                          
                                          &SspProc::SendAbortToIn,
                                          
                                          &SspProc::SendAbortToMsc,
                                          
                                          &SspProc::SendDp2,
                                          
                                          &SspProc::SendDp12,
                                          
                                          &SspProc::SendApplChrgReq,
                                          
                                          &SspProc::SendApplChrgRsp,
                                          
                                          &SspProc::SendCallInfoReq,
                                          
                                          &SspProc::SendCallInfoRsp,
                                          
                                          &SspProc::SendEvtBCSMReq,
                                          
                                          &SspProc::SendTBusy,
                                          
                                          &SspProc::SendTNoAns,
                                          
                                          &SspProc::SendTAns,
                                          
                                          &SspProc::SendTDiscon,
                                          
                                          &SspProc::SendOBusy,
                                          
                                          &SspProc::SendONoAns,
                                          
                                          &SspProc::SendOAns,
                                          
                                          &SspProc::SendODiscon,
                                          
                                          &SspProc::SendActivityReq,
                                          
                                          &SspProc::SendActivityRsp,
                                          
                                          &SspProc::SendContinueToMsc,
                                          
                                          &SspProc::SendConnectToMsc,
                                          
                                          &SspProc::SendReleaseToMsc,
                                          
                                          &SspProc::ClearDlgInfo
                                       };

#endif
