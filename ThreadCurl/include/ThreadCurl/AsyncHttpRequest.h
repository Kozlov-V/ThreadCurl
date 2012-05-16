//
//  AsyncHttpRequest.h
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

#ifndef AsyncHttpRequest_H
#define AsyncHttpRequest_H

#include "ThreadCurl/Thread.h"
#include "ThreadCurl/Mutex.h"
#include<string>
#include <list>
//request callback function type 
typedef void (*AsyncHttpRequestResponder)(void *param);

struct RequestData
{
    AsyncHttpRequestResponder responder;
    char * data;	
};

typedef enum
{
    AsyncHttpRequest_GET,
    AsyncHttpRequest_POST
}AsyncHttpRequest_RequestType;

class AsyncHttpRequest :public Thread
{
public:
    static  AsyncHttpRequest* get_instance(){return m_pInstance;}
	/**
     @param url specific request server url
     @param enableDebug  print debug message
     @param requestType request method type
     */
    void    SetOption(const std::string & url,AsyncHttpRequest_RequestType requestType =AsyncHttpRequest_GET, bool enableDebug =false);
    
    /**
     @param buffer send data buffer
     @param responder  request callback 
     */
    void    SendData(const char *buffer,AsyncHttpRequestResponder responder);
    
protected:
    void    run();
private:
    std::string url;
    AsyncHttpRequest_RequestType requestType;
    bool enableDebug;
	int    getMethodSend(const char *data,char *recv);
	int    postSendData(const char *data,char *recv);
    static AsyncHttpRequest *m_pInstance;
    AsyncHttpRequest(){ start();}
    Mutex mMutex;
    std::list<RequestData*> requestCache;
};

#endif
