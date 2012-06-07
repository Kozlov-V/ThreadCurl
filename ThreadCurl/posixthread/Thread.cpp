//
//  Thread.cpp
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
#include "ThreadCurl/Thread.h"
#include<stdio.h>

#if HAVE_PTHREAD
extern "C"
{
    typedef void * (* Start) (void *);
}
#endif

#if defined WIN32
unsigned __stdcall
#elif HAVE_PTHREAD
void
#endif
ThreadCallBack (void * t)
{
    (reinterpret_cast<TCThread*>(t))->run();
#if defined WIN32
    _endthreadex (0);
    return 0;
#endif
}

TCThread::TCThread ()
{
#if (defined WIN32) || HAVE_PTHREAD
#else
    printf("%s\n", "Threads not supported on this platform");
#endif
}


TCThread::~TCThread ()
{
#if defined WIN32
    DWORD status = ::WaitForSingleObject (_thread, INFINITE);
    assert (status ==  WAIT_OBJECT_0);
    bool ok = ::CloseHandle (_thread) != FALSE;
    assert (ok);
#elif HAVE_PTHREAD
    if(int error = ::pthread_join (_thread, 0))
        //if(int error = ::pthread_cancel(_thread))
        printf("%s (%d)\n", "pthread_join() was not successful",error);
#else
    printf("%s\n", "Threads not supported on this platform");
#endif
}


void
TCThread::start ()
{
#if defined WIN32
    unsigned id;
    _thread = (HANDLE)::_beginthreadex (0, 0, &threadLoop, this, 0, &id);
    
    if (_thread == 0)
        printf("%s\n", "Cannot create new thread");
#elif HAVE_PTHREAD
    if (int error = ::pthread_create (&_thread, 0, Start (ThreadCallBack), this))
        printf("%s (%d)\n", "Cannot create new thread", error);
#else
    printf("%s\n", "Threads not supported on this platform");
#endif
}