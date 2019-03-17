//
//  HttpResult.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/14.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef HttpResult_hpp
#define HttpResult_hpp

#include <stdio.h>
extern "C" {
#include <cjson/cJSON.h>
}

#include "ErrorCode.hpp"

class HttpResult {
private:
public:
    static cJSON* makeJson(ErrorCode errorCode, cJSON* data) {
        cJSON* json = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "data", data);
        cJSON_AddNumberToObject(json, "code", errorCode.getCode());
        cJSON_AddStringToObject(json, "msg", errorCode.getMsg().c_str());
        return json;
    }
    
    static cJSON* success(cJSON* data, std::string msg = "success") {
        ErrorCode errorCode(MEDIA_ERROR_CODE_SUCCESS, msg);
        cJSON* json = makeJson(errorCode, data);
        return json;
    }
};

#endif /* HttpResult_hpp */
