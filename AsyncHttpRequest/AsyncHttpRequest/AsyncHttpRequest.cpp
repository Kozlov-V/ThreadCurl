//
//  AsyncHttpRequest.cpp
//
//  Created by mini xingcloud on 12-4-28.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#include "AsyncHttpRequest.h"
#include "curl/curl.h"

AsyncHttpRequest *AsyncHttpRequest::m_pInstance = new AsyncHttpRequest();
size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam);
void    AsyncHttpRequest::SendData(const char *buffer,Responder  responder)
{
    Lock loc(mMutex);
    RequestData *rd = new RequestData;
    rd->responder = responder;
    rd->data = const_cast<char*>(buffer);
    
    requestCache.push_back(rd);
}
size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
{
	char *temp = (char*)recvBuffer;
	int length = strlen(temp);
	memcpy((char*)userParam,temp,length);
    
    return size*nmemb; 
}
void    AsyncHttpRequest::SetOption(const std::string & url, bool enableDebug,RequestType requestType )
{
    m_pInstance->url = url;
    
    m_pInstance->requestType = requestType;
    m_pInstance->enableDebug = enableDebug;
}
int     AsyncHttpRequest::getMethodSend(const char *data,char *recv)
{

    std::string request(m_pInstance->url);
	
	CURL* easy_handle=curl_easy_init();
	
	char *encodedURL = curl_easy_escape(easy_handle,data, strlen(data));
	
	request.append(encodedURL);
	//struct curl_slist *header=NULL;
	//header= curl_slist_append(header,request.c_str());
	if(m_pInstance->enableDebug)
        curl_easy_setopt(easy_handle, CURLOPT_VERBOSE,1L);/*open comment when debug mode.*/
	
	curl_easy_setopt(easy_handle,CURLOPT_URL,request.c_str());
	//curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,header);
	
	curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
	//char *revcData = new char[100000];
	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,recv);
	
	CURLcode code=curl_easy_perform(easy_handle);//
	
	//curl_slist_free_all(header);
	
	curl_easy_cleanup(easy_handle);
	
	curl_free(encodedURL);
	return code;
}
int    AsyncHttpRequest::postSendData(const char *data,char *recv)
{
	CURL* easy_handle = curl_easy_init();
	
	char *encodedURL = curl_easy_escape(easy_handle,data, strlen(data));
	
	curl_easy_setopt(easy_handle,CURLOPT_URL,m_pInstance->url.c_str());
	
	char *temp= new char[strlen(encodedURL)+6];
	sprintf(temp,"param=%s",encodedURL);
	
	curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS,temp);
	
	curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,strlen(temp));
	
	curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
	
	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,recv);
	
    if(m_pInstance->enableDebug)
        curl_easy_setopt(easy_handle,CURLOPT_POST,1);
    
	curl_easy_setopt(easy_handle,CURLOPT_VERBOSE,1); /* open comment when debug mode.*/
	
	CURLcode code=curl_easy_perform(easy_handle);

	curl_free(encodedURL);
	curl_easy_cleanup(easy_handle);
	return code;
}
void AsyncHttpRequest::run()
{
    while(1)
    {
        usleep(10000);
        
        if(requestCache.size()>0)
        {
            RequestData* content=NULL;
            content = requestCache.front();
            
            requestCache.pop_front();//delete first element
			char *revcData = new char[100000];
			memset(revcData,0,100000);
            int code = -1;
            if(m_pInstance->requestType ==GET)
            {
                 code = getMethodSend(content->data,revcData);
            }
            else 
            {
                 code = postSendData(content->data,revcData);
            }
			if(content->responder !=NULL)
			{
				if(code == 0)
				{
					content->responder(revcData);//TODO
				}
				else 
				{
					content->responder(NULL);
				}
			}
            
        }
    }
}