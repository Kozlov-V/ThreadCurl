//
//  Mutex.h
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

#ifndef POSIX_MUTEX_H
#define POSIX_MUTEX_H

#include "Common.h"

class Lock;

class Mutex
{
public:
    
    Mutex ();
    virtual ~Mutex ();
    
private:
    
    void	lock () const;
    void	unlock () const;
    
#if defined WIN32
	mutable CRITICAL_SECTION _mutex;
#elif HAVE_PTHREAD
	mutable pthread_mutex_t _mutex;
#endif
    
    void operator = (const Mutex& M);	// not implemented
    Mutex (const Mutex& M);		// not implemented
    
    friend class Lock;
};


class Lock
{
public:
    
    Lock (const Mutex& m, bool autoLock = true):
	_mutex (m),
	_locked (false)
    {
        if (autoLock)
        {
            _mutex.lock();
            _locked = true;
        }
    }
    
    ~Lock ()
    {
        if (_locked)
            _mutex.unlock();
    }
    
    void acquire ()
    {
        _mutex.lock();
        _locked = true;
    }
    
    void release ()
    {
        _mutex.unlock();
        _locked = false;
    }
    
    bool locked ()
    {
        return _locked;
    }
    
private:
    
    const Mutex &	_mutex;
    bool		_locked;
};

#endif
