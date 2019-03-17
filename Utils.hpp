//
//  Utils.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/6.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <stdio.h>
#include <openssl/md5.h>
#include <string>

#define MD5_SECRET_LEN_16     (16)
#define MD5_BYTE_STRING_LEN   (4)

extern "C" {
#include <cjson/cJSON.h>
}

class Utils {
public:
    static const char* getJsonStr(cJSON* json, const char* key);
    static std::string commonMd5Secret32(const std::string& src);
};

#endif /* Utils_hpp */
