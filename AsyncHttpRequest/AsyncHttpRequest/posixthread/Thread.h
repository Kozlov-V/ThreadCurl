#ifndef POSIX_THREAD_H
#define POSIX_THREAD_H
#include "config.h"
bool supportsThreads();

class Thread
{
public:
    
    Thread ();
    virtual ~Thread ();
    
    void		start ();
    virtual void	run () = 0;
    
private:
    
#if defined WIN32
	HANDLE _thread;
#elif HAVE_PTHREAD
	pthread_t _thread;
#endif
    
    void operator = (const Thread& t);	// not implemented
    Thread (const Thread& t);		// not implemented
};
#endif