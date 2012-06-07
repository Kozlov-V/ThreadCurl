//
//  Mutex.cpp
//  ThreadCurl
//
//  A lightweight crossplatform middleware for thread and http request coding. Support iOS/Android/Windows
//
//  Source code is hosted at:
//      https://github.com/chuckzhang/ThreadCurl
//

/*
 
 http://en.wikipedia.org/wiki/MIT_License
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 */
#include "ThreadCurl/Mutex.h"
#include<stdio.h>
TCMutex::TCMutex ()
{
#if defined WIN32
    ::InitializeCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if (int error = ::pthread_mutex_init (&_mutex, 0))
        printf("%s (%d)\n","Cannot initialize mutex", error);
#endif
}

TCMutex::~TCMutex ()
{
#if defined WIN32
    ::DeleteCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if(int error = ::pthread_mutex_destroy (&_mutex))
        printf("%s (%d)\n","Cannot de-initialize mutex", error);
#endif
}

void
TCMutex::lock () const
{
#if defined WIN32
    ::EnterCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if (int error = ::pthread_mutex_lock (&_mutex))
        printf("%s (%d)\n", "Cannot lock mutex", error);
#endif
}


void
TCMutex::unlock () const
{
#if defined WIN32
    ::LeaveCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if (int error = ::pthread_mutex_unlock (&_mutex))
        printf("%s (%d)\n", "Cannot unlock mutex", error);
#endif
}