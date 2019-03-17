//
//  Utils.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/6.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include "Utils.hpp"
#include <string.h>

const char* Utils::getJsonStr(cJSON* json, const char* key) {
    cJSON* item = cJSON_GetObjectItem(json, key);
    if(item) {
        return item->valuestring;
    }
    return NULL;
}

std::string Utils::commonMd5Secret32(const std::string& src)
{
    MD5_CTX ctx;
    
    std::string md5String;
    unsigned char md[MD5_SECRET_LEN_16] = { 0 };
    char tmp[MD5_BYTE_STRING_LEN] = { 0 };
    
    MD5_Init( &ctx );
    MD5_Update( &ctx, src.c_str(), src.size() );
    MD5_Final( md, &ctx );
    
    for( int i = 0; i < 16; ++i )
    {
        memset( tmp, 0x00, sizeof( tmp ) );
        snprintf( tmp,sizeof(tmp) , "%02X", md[i] );
        md5String += tmp;
    }
    return md5String;
}
