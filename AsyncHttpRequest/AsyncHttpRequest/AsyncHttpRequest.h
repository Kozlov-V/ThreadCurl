//
//  AsyncHttpRequest.h
//
//  Created by mini xingcloud on 12-4-28.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#ifndef AsyncHttpRequest_H
#define AsyncHttpRequest_H

#include "posixthread/Thread.h"
#include "posixthread/Mutex.h"
#include<string>
#include <list>
//request callback function type 
typedef void (*Responder)(void *param);

struct RequestData
{
    Responder responder;
    char * data;	
};
typedef enum
{
    GET,
    POST
}RequestType;

class AsyncHttpRequest :public Thread
{
public:
	
    static  AsyncHttpRequest* get_instance(){return m_pInstance;}
	/**
     @param url specific request server url
     @param enableDebug  print debug message
     @param requestType request method type
     */
    void    SetOption(const std::string & url, bool enableDebug =false,RequestType requestType =GET);
    
    /**
     @param buffer send data buffer
     @param responder  request callback 
     */
    void    SendData(const char *buffer,Responder responder);
    
protected:
    void    run();
private:
    std::string url;
    RequestType requestType;
    bool enableDebug;
	int    getMethodSend(const char *data,char *recv);
	int    postSendData(const char *data,char *recv);
    static AsyncHttpRequest *m_pInstance;
    AsyncHttpRequest(){ start();}
    Mutex mMutex;
    std::list<RequestData*> requestCache;
};

#endif
