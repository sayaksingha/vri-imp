// @(#) $Id: OktoClient.h,v 1.1.1.1.2.3 2016/04/12 21:59:02 shijuc Exp $
/****************License************************************************
 *
 * Copyright 2003-2004.  Phonologies (India) Pvt Ltd.
 *
 * Use of this software is subject to notices and obligations set forth
 * in the Phonologies License 1.0  which is included with this software.
 *
 * Phonologies, InterpreXer, Oktopous Media Server are registered trademarks or
 * trademarks of Phonologies (India) Pvt Ltd . in India and other countries.
 *
 ***********************************************************************/

#ifndef OKTO_CLIENT
#define OKTO_CLIENT

#include <string>
#include <OktoPlatform.hpp>
#include "IvrStructs.h"
//#include "Fetcher.hpp"
//#include "DocumentParser.hpp"
#include "OktoEvent.hpp"

class OktopousEventThread;
class OutputQueue;
class OktoInterpreter;
class PLog;

class OktoClient : public OktoPlatform {
	
	public:
		OktoClient();
		~OktoClient(){};
		int buildClient(char * ccxmlurl, CcxmlRequest& lCcxmlRequest);
		int PostEvent(CcxmlRequest& lCcxmlRequest);

		void  SendExit(const VXIchar *sessionid, const VXIchar *pid );
		bool  CreateIpc();

TEXT* GetProperty(VXIchar* attribute, VXIMap* event );
	// will be called after executing the <exit> or Interpreter exits for anyother reason.
	virtual OktoPlatform::Result CCXMLComplete(
          const VXIchar           *sessionid,
          OktoInterpreter::Result result,
          VXIValue                *docResult,
          struct OktoInterpreter::Resources *resources,
          VXIMap * exitEventMap,
          const VXIchar * parentid){
			//fprintf(stderr,"THE END Res:%d PId:%ls\n", result, parentid);
			SendExit(sessionid, parentid);
			//_exit(0);
		};

     virtual OktoPlatform::Result GetID(
          IdType    idType,
          VXIString **id ){};

		key_t mCcxmlRspQueKey;
		MsgQueue mCcxmlRspQue;
	private:
	  	OktopousEventThread * eventThread;
		OutputQueue		     * outQ;
		OktoInterpreter	  * myInterpreter;
		PLog				     * gblLog;
};

#endif
