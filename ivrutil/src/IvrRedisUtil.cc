static const char* id="@(#) $Id: IvrRedisUtil.cc,v 1.1.2.1 2018/01/18 00:16:27 shijuc Exp $" ;
//----------------------------------------------------------------------
// NAME
// IvrRedisUtil.cc
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
// Originated :                                              12-AUG-2009
//----------------------------------------------------------------------
#include "IvrRedisUtil.h"

extern Log gLog;
extern CTrace gTrace;
extern TEXT        gLogText[MAX_LOG_TEXT_LEN];
redisContext * IvrRedisUtil::redisContextIVR;
redisContext * IvrRedisUtil::ConnectToIvrRedis(const char *hostname, int port)
{
        DisconnectRedis();
        struct timeval timeout = { 1, 500000 }; // 1.5 seconds
        redisContextIVR = redisConnectWithTimeout(hostname, port, timeout);
        if (redisContextIVR == NULL || redisContextIVR->err) {
                if (redisContextIVR) {
                        printf("Connection error: %s\n", redisContextIVR->errstr);
                        redisFree(redisContextIVR);
                } else {
                        printf("Connection error: can't allocate redis context\n");
                }
                return NULL;
        }
        return redisContextIVR;
}

redisContext * IvrRedisUtil::ConnectToIvrRedis()
{
        DisconnectRedis();
        struct timeval timeout = { 1, 500000 }; // 1.5 seconds
        redisContextIVR = redisConnectWithTimeout("127.0.0.1", 6379, timeout);
        if (redisContextIVR == NULL || redisContextIVR->err) {
                if (redisContextIVR) {
                        printf("Connection error: %s\n", redisContextIVR->errstr);
                        redisFree(redisContextIVR);
                } else {
                        printf("Connection error: can't allocate redis context\n");
                }
                return NULL;
        }
        return redisContextIVR;
}

void IvrRedisUtil::DisconnectRedis()
{
   /* Disconnects and frees the context */
   redisFree(redisContextIVR);
}

int IvrRedisUtil::StoreCallDetailsToIvrRedis(char *msisdn, char *to, char *from, char *origcalled)
{
    redisReply *reply;
    /* Get a Value from Hash Key */
    reply = (redisReply * )redisCommand(redisContextIVR,"HSET %s TO %s FROM %s ORIG_CALLED %s", msisdn, to, from, origcalled);
    if(reply->type != REDIS_REPLY_INTEGER)
    {
            printf("HSET Error: %s\n", reply->str);
            freeReplyObject(reply);
            return 0;
    }
    reply = (redisReply * )redisCommand(redisContextIVR,"EXPIRE %s %d", msisdn, CALL_DETAILS_EXPIRY_TIME);
    return 1;
}

int IvrRedisUtil::GetStringValueFromIvrRedis(const char *key, char *field, char *val)
{
    redisReply *reply;
    /* Get a Value from Hash Key */
    reply = (redisReply * )redisCommand(redisContextIVR,"HGET %s %s", key, field);
    if(reply->type != REDIS_REPLY_STRING)
    {
            printf("HGET Error: %s\n", reply->str);
            freeReplyObject(reply);
            return 0;
    }
    sprintf(val,"%s", reply->str);
    return 1;
}
