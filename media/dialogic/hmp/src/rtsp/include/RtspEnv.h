// @(#) $Id: RtspEnv.h,v 1.1.2.1 2012/02/21 08:31:39 shijuc Exp $
#if !defined(RTSP_ENV_H)
#define RTSP_ENV_H

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include <string>
#include <boost/thread.hpp>

class RtspClient;

class RtspEnv {
public:
    ~RtspEnv();
    void armDroppedConnection(double timer, RtspClient* pClient);
    static void incomingRequestHandler(void* clientData, int mask);
    static void timeoutHandler(void* clientData); 
    static RtspEnv* Instance();
    static RtspEnv* pInst ;
    UsageEnvironment* GetEnv() { return env; }
    const char* GetAppName() { return progName.c_str(); }
    static void WorkerThreadFcn(RtspEnv*);
    static char watchVariable;
protected:
    RtspEnv();

private:
    UsageEnvironment* env;
    TaskScheduler* scheduler;
    std::string    progName;
    boost::thread  *workerThread;
};
#endif
