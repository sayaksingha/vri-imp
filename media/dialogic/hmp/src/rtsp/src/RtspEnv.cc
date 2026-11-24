static const char *id = "@(#) $Id: RtspEnv.cc,v 1.1.2.1 2012/02/21 08:31:40 shijuc Exp $";
#include "RtspEnv.h"
#include "RtspClient.h"
#include <boost/bind.hpp>
#include <queue>
#include <iostream>

RtspEnv* RtspEnv::pInst = NULL;
char RtspEnv::watchVariable = '\0';

RtspEnv* RtspEnv::Instance()
{
	if (pInst == NULL)
	{
		pInst = new RtspEnv();
	}
	return pInst;
}

RtspEnv::RtspEnv() {
   scheduler = BasicTaskScheduler::createNew();
   env = BasicUsageEnvironment::createNew(*scheduler);
   progName.assign("Tayana Media Player");
   workerThread = new boost::thread(boost::bind(RtspEnv::WorkerThreadFcn, this));
}

void RtspEnv::WorkerThreadFcn(RtspEnv* pEnv) {
   pEnv->watchVariable = '\0';
   while (1) {
      if (pEnv->watchVariable == '\0')
      {

         pEnv->scheduler->doEventLoop(&pEnv->watchVariable);
      }
      else
         usleep(20000);
   }
}

void RtspEnv::armDroppedConnection(double timer, RtspClient* pClient)
{
     watchVariable = '1';
     scheduler->turnOnBackgroundReadHandling(pClient->socketNum(),
        (TaskScheduler::BackgroundHandlerProc*)&incomingRequestHandler, pClient);
     if (timer > 0.0) {
        int uSecsToDelay = (int)(timer*1000000.0);
	scheduler->scheduleDelayedTask(uSecsToDelay, timeoutHandler, this);
     }
     watchVariable = '\0';
}

void RtspEnv::incomingRequestHandler(void* clientData, int mask) {
   RtspClient* pClient = (RtspClient*)clientData;
   RtspEnv* pEnv = RtspEnv::Instance();
   watchVariable = '1';
   pClient->SessionTerminated();
   pEnv->scheduler->turnOffBackgroundReadHandling(pClient->socketNum());
   watchVariable = '\0';
}

void RtspEnv::timeoutHandler(void* clientData) {
   RtspClient* pClient = (RtspClient*)clientData;
   pClient->SessionTerminated();
}
