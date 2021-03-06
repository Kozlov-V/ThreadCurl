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

//configure MD5 private key
#define MD5PRIVATEKEY "5cf202f067ik012ecae31edb0f3c16bhOM"

//request callback function type 
typedef enum
{
    AsyncHttpRequest_GET,
    AsyncHttpRequest_POST
}AsyncHttpRequest_RequestType;

typedef void (*AsyncHttpRequestResponder)(void *param);

struct AsyncHttpRequest_RequestData
{
    AsyncHttpRequestResponder responder;
    char * data;
	std::string url;
    AsyncHttpRequest_RequestType requestType;
};

typedef struct callbackData
{
    void *buffer;
    int position;
}CallbackData;
class AsyncHttpRequest :public TCThread
{
public:
    static  AsyncHttpRequest* get_instance(){return m_pInstance;}
	/**
     @param url specific request server url
     @param enableDebug  print debug message
     @param requestType request method type
     */
    void    SetOption(bool enableDebug =false);
    
    /**
     @param buffer send data buffer
     @param responder  request callback 
     */
    void    SendData(const char *buffer,AsyncHttpRequestResponder responder,const std::string & url,AsyncHttpRequest_RequestType requestType =AsyncHttpRequest_GET);
    
    /**
     @param result  out value of createMD5 key
     @param argNumber key number 
     @param ...       pass in key/value
     */
    void CreateMD5Key(char *result,int argNumber,...);
    /**
     @param result  out value of createMD5 key
     @param ...       pass in CJSON object, keys will be ordered alphabetically
     */
    void CreateMD5KeyFromCJSON(char *result,...);
    
protected:
    void    run();
private:

    bool   enableDebug;
	int    getMethodSend(const char *data,char *recv,const char *url);
	int    postSendData(const char *data,char *recv,const char *url);
    static AsyncHttpRequest *m_pInstance;
    AsyncHttpRequest(){ start();}
    TCMutex mMutex;
    std::list<AsyncHttpRequest_RequestData*> requestCache;
};

#endif
