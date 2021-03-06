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
#include "cJSON.h"
#include "md5.h"
#include<map>

AsyncHttpRequest *AsyncHttpRequest::m_pInstance = new AsyncHttpRequest();
size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam);
void    AsyncHttpRequest::SendData(const char *buffer,AsyncHttpRequestResponder  responder,const std::string & url,AsyncHttpRequest_RequestType requestType)
{
    TCLock loc(mMutex);
    AsyncHttpRequest_RequestData *rd = new AsyncHttpRequest_RequestData;
    rd->responder = responder;
    rd->data = const_cast<char*>(buffer);
   
    rd->requestType = requestType;
    rd->url = url;
    
    requestCache.push_back(rd);
}

size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
{
	//char *temp = (char*)recvBuffer;
    
	//memcpy((char*)userParam,temp,size*nmemb);
  
    CallbackData *cba = (CallbackData*)userParam;

    memcpy((char*)cba->buffer+cba->position,recvBuffer,size*nmemb);
     
    cba->position +=size*nmemb;

    return size*nmemb; 
}
size_t postRecvWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
{
	//char *temp = (char*)recvBuffer;
    
	//memcpy((char*)userParam,temp,size*nmemb);
    
    CallbackData *cba = (CallbackData*)userParam;
    
    memcpy((char*)cba->buffer+cba->position,recvBuffer,size*nmemb);
    
    cba->position +=size*nmemb;
    
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
	
    CallbackData *cba = (CallbackData*)malloc(sizeof(CallbackData));
    cba->buffer = (char*)malloc(100000);
   
    cba->position = 0;
	curl_easy_setopt(easy_handle,CURLOPT_URL,request.c_str());
	//curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,header);
	curl_easy_setopt(easy_handle,CURLOPT_HTTP_TRANSFER_DECODING,1);
	curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
	
	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,cba);
	CURLcode code=curl_easy_perform(easy_handle);//
	
    memcpy(recv,cba->buffer,cba->position);
    
    free(cba->buffer);
    free(cba);
  
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
    
	int encodeLength = strlen(encodedURL);
	char *temp= new char[encodeLength+8];
    memset(temp,0,encodeLength+8);
	sprintf(temp,"params=%s",encodedURL);
//	int tl = strlen(temp);
    
	curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS,temp);
	
	curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,strlen(temp));
	
	curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postRecvWriteData);//receive callback function
	
    CallbackData *cba = (CallbackData*)malloc(sizeof(CallbackData));
    cba->buffer = (char*)malloc(100000);
    
    cba->position = 0;

	curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,cba);
	
//    if(m_pInstance->enableDebug)
    curl_easy_setopt(easy_handle,CURLOPT_POST,1);
    
//	curl_easy_setopt(easy_handle,CURLOPT_VERBOSE,1); /* open comment when debug mode.*/
	
	CURLcode code=curl_easy_perform(easy_handle);

    memcpy(recv,cba->buffer,cba->position);
    
    free(cba->buffer);
    free(cba);
    
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
            if(strlen(content->data)>0)
            {
                free(content->data);
            }
            requestCache.pop_front();//delete first element
            
			if(content->responder !=NULL)
			{
				if(code == 0)
				{
					content->responder(revcData);//TODO
                    revcData = NULL;
				}
				else 
				{
					content->responder(NULL);
				}
			}
            delete content;          
            if(revcData)
                delete revcData;
        }
    }
}


void AsyncHttpRequest::CreateMD5Key(char *result,int argNumber,...)
{
    std::map<std::string,std::string> ms;
    va_list arg_ptr;
    va_start(arg_ptr, argNumber);
    
    for (int i = 0; i < argNumber; i++)
    {
        const char * key=va_arg(arg_ptr,const char*);
        const char * value=va_arg(arg_ptr,const char*);
        ms[key] = value;
    }
    
    std::string sigSeed;
    
    for(std::map<std::string,std::string>::iterator iter =ms.begin(); iter!= ms.end();iter++)
    {
        sigSeed.append(iter->second);
    }
    sigSeed.append(MD5PRIVATEKEY);//
    
    unsigned char md5Result[16] = {0};
    CMD5 md5;
    md5.Process((unsigned char*)sigSeed.c_str(), strlen(sigSeed.c_str()), md5Result);
    
    char strSig[128] = {0};
    for (int i = 0; i < 16; i++)
    {
        sprintf(strSig + i*2, "%02x", *(md5Result + i));
    }
    strcpy(result,strSig);
    
    va_end(arg_ptr);
}	

void AsyncHttpRequest::CreateMD5KeyFromCJSON(char *result,...)
{
    va_list arg_ptr;
    va_start(arg_ptr, result);
    std::map<std::string,std::string> ms;
    cJSON * value=va_arg(arg_ptr,cJSON*);
    cJSON * firstChild = value->child;
    while (firstChild) {
        switch (firstChild->type) {
            case cJSON_String:
                ms[firstChild->string] = firstChild->valuestring;
                break;
            case cJSON_Number:
                char buff[10];
                memset(&buff, 0, 10);
                sprintf(buff, "%d",firstChild->valueint);
                ms[firstChild->string] = buff;
                break;
            default:
                break;
        }
        
        firstChild=firstChild->next;
    }
    va_end(arg_ptr);
    
    std::string sigSeed;
    
    for(std::map<std::string,std::string>::iterator iter =ms.begin(); iter!= ms.end();iter++)
    {
        sigSeed.append(iter->second);
    }
    sigSeed.append(MD5PRIVATEKEY);//
    
    unsigned char md5Result[16] = {0};
    CMD5 md5;
    md5.Process((unsigned char*)sigSeed.c_str(), strlen(sigSeed.c_str()), md5Result);
    
    char strSig[128] = {0};
    for (int i = 0; i < 16; i++)
    {
        sprintf(strSig + i*2, "%02x", *(md5Result + i));
    }
    strcpy(result,strSig);
}



