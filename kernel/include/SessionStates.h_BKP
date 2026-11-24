// @(#) $Id
//---------------------------------------------------------------
// NAME      SessionStates.h
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
// Originated   : Ramprasad M                Date:11/11/2010
//
// Modified     : Ramprasad M                Date:04/09/2013
// Modification : Release handled in all the states
//---------------------------------------------------------------
#ifndef INCL_SES_STATE_H
#define INCL_SES_STATE_H
#include "SessionManager.h"
State NO_STATE[] = {
							{EVT_IAM, 1, {ACT_SND_CCXML_ALT}},
							{EVT_EXT_PLY_RCV, 1, {ACT_SND_VXML_REQ}},
							{EVT_EXT_PTH_RCV, 1, {ACT_SND_EXT_PTH_CKT}},
                     {EVT_EXT_TEAR_RCV, 2, {ACT_SND_EXT_TEAR_CKT, ACT_EXIT}},  
                     {EVT_DIAL_OUT, 1, {ACT_SND_IAM}},
                     {EVT_TERMINATE_CHILD, 1, {ACT_NULL}},
                     {EVT_REL, 1, {ACT_SND_RLC}},
                     {EVT_VXML_RSP_RCV, 1, {ACT_EXIT}},
							{EVT_NULL, 1, {ACT_NULL}},
						 };

State CCXML_ALT_SND[] = {
								  {EVT_CCXML_ALT_ACK_RCV, 2, {ACT_SND_ACM,ACT_SND_ANM}},
								  {EVT_CCXML_EXT_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
								  {EVT_CCXML_ALT_NACK_RCV, 1, {ACT_SND_REL}},
								  {EVT_CCXML_DISCONN_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
								  {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
                          {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  {EVT_NULL, 1, {ACT_NULL}},
							   };
								  
//State ANM_SND[] = {
//                     {EVT_CPG, 1, {ACT_SND_CCXML_CONN}},
//                     {EVT_CCXML_EXT_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
//                     {EVT_CCXML_ALT_NACK_RCV, 1, {ACT_SND_REL}},
//                     {EVT_CCXML_DISCONN_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
//                     {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
//                     {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
//                     {EVT_NULL, 1, {ACT_NULL}},
//                  };

State EXT_VXML_PLY_SND[] = {
								  		{EVT_VXML_RSP_RCV, 2, {ACT_SND_EXT_DLG_EXT, ACT_EXIT}},
								  		{EVT_EXT_STP_RCV, 1, {ACT_SND_EXT_VXML_TERM}},
								  		{EVT_EXT_PTH_RCV, 1, {ACT_SND_EXT_PTH_CKT}},
								  		{EVT_EXT_TEAR_RCV, 2, {ACT_SND_EXT_TEAR_CKT, ACT_EXIT}},  
										{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  		{EVT_NULL, 1, {ACT_EXIT}},
									};

State CCXML_ACM_SND[] = {
								  {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
                          {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  {EVT_NULL, 1, {ACT_NULL}},
								};

State CCXML_ANM_SND[] = {
                          {EVT_CPG, 1, {ACT_SND_CCXML_CONN}},
                          {EVT_CCXML_EXT_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
                          {EVT_CCXML_ALT_NACK_RCV, 1, {ACT_SND_REL}},
                          {EVT_CCXML_DISCONN_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
								  {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
                          {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  {EVT_NULL, 1, {ACT_NULL}},
								};

State CCXML_CONN_SND[] = {
								  {EVT_CCXML_DLG_STR_RCV, 1, {ACT_SND_VXML_REQ}},
                          {EVT_CCXML_DISCONN_RCV, 1, {ACT_SND_REL}},
                          {EVT_CCXML_EXT_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
                          {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
                          {EVT_CCXML_UNJOIN, 2, {ACT_CCXML_UNJOIN_CALLS, ACT_CCXML_UNJOIN_RSP}},
                          {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  {EVT_NULL, 1, {ACT_NULL}},
							   };

State VXML_REQ_SND[] = {
								  {EVT_EXT_PLY_RCV, 1, {ACT_SND_VXML_REQ}},
								  {EVT_VXML_RSP_RCV, 1, {ACT_SND_DLG_EXT}},
								  {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  {EVT_NULL, 1, {ACT_NULL}},
							  };

State CCXML_DLG_EXT_SND[]	= {
								  			{EVT_CCXML_EXT_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
                                 {EVT_CCXML_DLG_STR_RCV, 1, {ACT_SND_VXML_REQ}},
								  			{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
                                 {EVT_REL, 1, {ACT_SND_RLC}},
								  			{EVT_NULL, 1, {ACT_NULL}},
									  };
                                // {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},  required for multple vxml dialogs in ccxml

State	CCXML_REL_SND[]	= {
								  		{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
                              {EVT_REL, 1, {ACT_SND_RLC}},
								  		{EVT_NULL, 1, {ACT_NULL}},
								  };

State CCXML_DISCO_SND[]	= {
                              {EVT_CCXML_UNJOIN, 2, {ACT_CCXML_UNJOIN_CALLS, ACT_CCXML_UNJOIN_RSP}},
								  		{EVT_CCXML_EXT_RCV, 1, {ACT_SND_VXML_TERM}},
								  		{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
                              {EVT_REL, 1, {ACT_SND_RLC}},
                              {EVT_IAM, 1, {ACT_SND_CCXML_ALT}},
								  		{EVT_NULL, 1, {ACT_NULL}},
								  };

State	EXT_VXML_TERM_SND[] = {
								  		{EVT_VXML_RSP_RCV,1, { ACT_EXIT}},
								  		{EVT_EXT_PLY_RCV, 1, {ACT_SND_VXML_REQ}},
                              {EVT_EXT_PTH_RCV, 1, {ACT_SND_EXT_PTH_CKT}},
								  		{EVT_EXT_TEAR_RCV, 2, {ACT_SND_EXT_TEAR_CKT, ACT_EXIT}},  
								  		{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  		{EVT_NULL, 1, {ACT_NULL}},
									 };

State	EXT_VXML_PATCH_SND[] = {
								  		{EVT_EXT_TEAR_RCV, 2, {ACT_SND_EXT_TEAR_CKT, ACT_EXIT}},
								  		{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  		{EVT_NULL, 1, {ACT_NULL}},
									 };

State	EXT_VXML_TEAR_SND[]	= {
								  		{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  		{EVT_NULL, 1, {ACT_NULL}},
								     };

State	DIAL_OUT_IAM_SND[]	= {
                              {EVT_ACM, 1, {ACT_STATE_ACM}},
                              {EVT_ANM, 1, {ACT_SND_CALL_CREATED}},
								      {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  		{EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  		{EVT_NULL, 1, {ACT_NULL}},
								     };

State ACM_RCVD[]           =  {
                               {EVT_ANM, 1, {ACT_SND_CALL_CREATED}},
								       {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  		 {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  		 {EVT_NULL, 1, {ACT_NULL}},
                              };

State CCXML_CALL_CREATED[] =  {
                               {EVT_CCXML_JOIN, 2, {ACT_CCXML_JOIN_CALLS, ACT_CCXML_JOIN_RSP}},
								  		 {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
                               {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  		 {EVT_NULL, 1, {ACT_NULL}},
                              };

State CCXML_CALL_JOIN[] =     {
								       {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								       {EVT_CCXML_EXT_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
                               {EVT_CCXML_UNJOIN, 2, {ACT_CCXML_UNJOIN_CALLS, ACT_CCXML_UNJOIN_RSP}},
								  		 {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								  		 {EVT_NULL, 1, {ACT_NULL}},
                              };

State CCXML_CALL_UNJOIN[] =   {
								  		 {EVT_CCXML_EXT_RCV, 2, {ACT_SND_REL, ACT_EXIT}},
								  		 {EVT_TERMINATE_CHILD, 1, {ACT_TERMINATE_CHILD}},
								       {EVT_REL, 1, {ACT_SND_CCXML_DISCO}},
								  		 {EVT_NULL, 1, {ACT_NULL}},
                              };


State* gStateTable[] = { NO_STATE, CCXML_ALT_SND, CCXML_ACM_SND, CCXML_ANM_SND, CCXML_CONN_SND, 
								 VXML_REQ_SND, CCXML_DLG_EXT_SND, CCXML_REL_SND, CCXML_DISCO_SND, EXT_VXML_PLY_SND, 
								 EXT_VXML_TERM_SND, EXT_VXML_PATCH_SND, EXT_VXML_TEAR_SND, DIAL_OUT_IAM_SND,ACM_RCVD,
                         CCXML_CALL_CREATED, CCXML_CALL_JOIN, CCXML_CALL_UNJOIN };

typedef bool (*Action)();

Action gActionTable[] = { SendDoNothing, SendCcxmlCallAlert, SendAcm, SendAnm, SendCcxmlCallConnected, SendVxmlRequest,
							SendCcxmlDlgExit, SendRel, SendCcxmlDisconnect, SendVxmlTerminate, TerminateChild, 
							ExitChild, SendExternalDlgExit, SendExtVxmlTerminate, SendExtPatchCircuit, SendExtTearCircuit,
							SendExtVxmlTerminateRsp, SendIam, ChangeStateToAcm, SendCallCreated, JoinCcxmlCalls,
                     SendCcxmlJoinRsp, UnjoinCcxmlCalls, SendCcxmlUnjoinRsp, SendRlc};

#endif
