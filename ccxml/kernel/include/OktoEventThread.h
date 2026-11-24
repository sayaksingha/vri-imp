// @(#) $Id: OktoEventThread.h,v 1.1 2011/03/08 08:24:31 root Exp $
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

#ifndef OktopousEventThread_H
#define OktopousEventThread_H

#include <Thread.hpp>
#include <OutputQueue.hpp>
#include "assert.h"
class OktopousEventThread : public Thread
{
	protected:
		///
		void thread();

		void cancel(){};
	public:
		///
		OktopousEventThread(OutputQueue* output ) {

			myEventQueue = output;
		}

		virtual ~OktopousEventThread() {};

		///
		OutputQueue *myEventQueue;
};

#endif
