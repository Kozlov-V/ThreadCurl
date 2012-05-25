//
//  AsyncHttpRequest.cpp
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

#include "AsyncHttpRequest.h"
#include "curl/curl.h"

AsyncHttpRequest *AsyncHttpRequest::m_pInstance = new AsyncHttpRequest();
size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam);
void    AsyncHttpRequest::SendData(const char *buffer,AsyncHttpRequestResponder  responder,const std::string & url,AsyncHttpRequest_RequestType requestType)
{
    Lock loc(mMutex);
    AsyncHttpRequest_RequestData *rd = new AsyncHttpRequest_RequestData;
    rd->responder = responder;
    rd->data = const_cast<char*>(buffer);
   
    rd->requestType = requestType;
    rd->url = url;
    
    requestCache.push_back(rd);
}
size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
{
	char *temp = (char*)recvBuffer;
    
	memcpy((char*)userParam,temp,size*nmemb);
    
    return size*nmemb; 
}
void AsyncHttpRequest::SetOption(bool enableDebug)
{
    m_pInstance->enableDebug = enableDebug;
}
int AsyncHttpRequest::getMethodSend(const char *data,char *recv,const char *url)
{

    std::string request(url);
	
	CURL* easy_handle=curl_easy_init();
	
	char *encodedURL = curl_easy_escape(easy_handle,data, strlen(data));
	
	request.append(encodedURL);
	//struct curl_slist *header=NULL;
	//header= curl_slist_append(header,request.c_str());
	if(m_pInstance->enableDebug)
        curl_easy_setopt(easy_handle, CURLOPT_VERBOSE,1L);/*open comment when debug mode.*/
	
	curl_easy_setopt(easy_handle,CURLOPT_URL,request.c_str());
	//curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,header);
	curl_easy_setopt(easy_handle,CURLOPT_HTTP_TRANSFER_DECODING,1);
	curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
	//char *revcData = new char[100000];
	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,recv);
	
	CURLcode code=curl_easy_perform(easy_handle);//
	
	//curl_slist_free_all(header);
	
	curl_easy_cleanup(easy_handle);
	
	curl_free(encodedURL);
	return code;
}
int    AsyncHttpRequest::postSendData(const char *data,char *recv,const char *url)
{
	CURL* easy_handle = curl_easy_init();
	
	char *encodedURL = curl_easy_escape(easy_handle,data, strlen(data));
	
	curl_easy_setopt(easy_handle,CURLOPT_URL,url);
	
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
            AsyncHttpRequest_RequestData* content=NULL;
            content = requestCache.front();
            
			char *revcData = new char[100000];
			memset(revcData,0,100000);
            int code = -1;
            if(content->requestType ==AsyncHttpRequest_GET)
            {
                 code = getMethodSend(content->data,revcData,content->url.c_str());
            }
            else 
            {
                 code = postSendData(content->data,revcData,content->url.c_str());
            }
            
            requestCache.pop_front();//delete first element
            
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