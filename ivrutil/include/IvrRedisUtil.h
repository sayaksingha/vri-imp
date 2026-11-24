// @(#) $Id: IvrRedisUtil.h,v 1.1.2.1 2018/01/18 00:15:56 shijuc Exp $
//----------------------------------------------------------------------
// NAME
// IvrRedisUtil.h
//
// COPYRIGHT
// Tayana Software Solutions Ltd -- Copyright(C) 2006
// All rights reserved. No part of this computer program
// may be used or reproduced in any form by any
// means without prior written permission of
// Tayana Software Solutions Ltd
//
// DESCRIPTION
//
// Originated :                                              22-OCT-2010
//----------------------------------------------------------------------
#ifndef INCL_IVR_REDIS_UTIL_DEF_H
#define INCL_IVR_REDIS_UTIL_DEF_H

#include "IvrUtil.h"

#include <hiredis/hiredis.h>
#define CALL_DETAILS_EXPIRY_TIME 60

class IvrRedisUtil
{
   public:
      static redisContext * ConnectToIvrRedis(const char *hostname, int port);

      static redisContext * ConnectToIvrRedis();

      static void DisconnectRedis();

      static int StoreCallDetailsToIvrRedis(char *msisdn, char *to, char *from, char *origcalled);

      static int GetStringValueFromIvrRedis(const char *key, char *field, char *val);

      static  inline int GetToURIFromIvrRedis(const char *msisdn, char *to)
      {
         return GetStringValueFromIvrRedis(msisdn, "TO", to);
      }

      static inline int GetFromURIFromIvrRedis(const char *msisdn, char *from)
      {
         return GetStringValueFromIvrRedis(msisdn, "FROM", from);
      }

      static inline int GetOrigCalledURIFromIvrRedis(const char *msisdn, char *origcalled)
      {
         return GetStringValueFromIvrRedis(msisdn, "ORIG_CALLED", origcalled);
      }

      static inline int GetCallDetailsFromIvrRedis(char *msisdn, char *to, char *from, char *origcalled)
      {
         int lRetVal=0;

         if(GetStringValueFromIvrRedis(msisdn, "TO", to))
            if(GetStringValueFromIvrRedis(msisdn, "FROM", from))
               lRetVal = GetStringValueFromIvrRedis(msisdn, "ORIG_CALLED", origcalled);
         return lRetVal;
      }

   protected:
      static redisContext *redisContextIVR;
};

#endif
