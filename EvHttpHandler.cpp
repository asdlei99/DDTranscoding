//
//  EvHttpHandler.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/6.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include "EvHttpHandler.hpp"
#include <iostream>
#include <stdlib.h>
#include "WorkersContainer.hpp"

extern "C" {
#include <cjson/cJSON.h>
}

#include "Utils.hpp"
#include "TranscodingWorker.hpp"
#include "WorkContainerProxy.hpp"
#include "HttpResult.hpp"

using namespace std;

void EvHttpHandler::handler404(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf = evbuffer_new();
    if(!buf)
    {
        cout<<"failed to create response buffer \n";
        return;
    }
    
    evbuffer_add_printf(buf, "Oh, Page Not Found\n");
    evhttp_send_reply(req, HTTP_NOTFOUND, "NOT FOUND", buf);
    evbuffer_free(buf);
}

void EvHttpHandler::list(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf = evbuffer_new();
    if(!buf)
    {
        cout<<"failed to create response buffer \n";
        return;
    }
    
    WorkersContainer* workerContainer = (WorkersContainer*) arg;
    cJSON* json = workerContainer->getWorkList();
    cJSON* retJson = HttpResult::success(json);
    
    char* jsonStr = cJSON_Print(retJson);
    evbuffer_add_printf(buf, jsonStr);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
    
    free(jsonStr);
    cJSON_Delete(retJson);
}

void EvHttpHandler::add(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf = evbuffer_new();
    if(!buf)
    {
        cout<<"failed to create response buffer \n";
        return;
    }
    
    struct evbuffer *readBuf = evhttp_request_get_input_buffer(req);
    
    string strBuf = "";
    while (evbuffer_get_length(readBuf)) {
        char cbuf[128];
        memset(cbuf, 0, sizeof(cbuf));
        evbuffer_remove(readBuf, cbuf, sizeof(cbuf) - 1);
        strBuf += cbuf;
    }
    
    cJSON *reqJSON = cJSON_Parse(strBuf.c_str());
    const char* src = Utils::getJsonStr(reqJSON, "src");
    const char* dst = Utils::getJsonStr(reqJSON, "dst");
    if(src && dst) {
        printf("start transcoding [src=%s,dst=%s]\n", src, dst);
        WorkContainerProxy::add(src, dst);
    } else {
        perror("must set src and dst");
    }
    
    cJSON* retJson = HttpResult::success(NULL);
    char* retStr = cJSON_Print(retJson);
    cJSON_Delete(reqJSON);
    evbuffer_add_printf(buf, retStr);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
    cJSON_Delete(retJson);
    free(retStr);
}

void EvHttpHandler::del(struct evhttp_request *req, void *arg)
{
    struct evbuffer *buf = evbuffer_new();
    if(!buf)
    {
        cout<<"failed to create response buffer \n";
        return;
    }
    
    struct evbuffer *readBuf = evhttp_request_get_input_buffer(req);
    
    string strBuf = "";
    while (evbuffer_get_length(readBuf)) {
        char cbuf[128];
        memset(cbuf, 0, sizeof(cbuf));
        evbuffer_remove(readBuf, cbuf, sizeof(cbuf) - 1);
        strBuf += cbuf;
    }
    
    cJSON *reqJSON = cJSON_Parse(strBuf.c_str());
    const char* dst = Utils::getJsonStr(reqJSON, "dst");
    if(dst) {
        printf("delete transcoding [dst=%s]\n", dst);
        WorkContainerProxy::del(dst);
    } else {
        perror("must set src and dst");
    }
    
    cJSON* retJson = HttpResult::success(NULL);
    char* retStr = cJSON_Print(retJson);
    cJSON_Delete(reqJSON);
    evbuffer_add_printf(buf, retStr);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
    cJSON_Delete(retJson);
    free(retStr);
}
