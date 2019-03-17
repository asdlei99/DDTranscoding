//
//  ErrorCode.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/14.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef ErrorCode_hpp
#define ErrorCode_hpp

#include <stdio.h>
#include <string>

enum MEDIA_ERROR_CODE {
    MEDIA_ERROR_CODE_SUCCESS = 10000,
    MEDIA_ERROR_CODE_DST_ALREADY_EXIST = 10001
};

class ErrorCode {
private:
    MEDIA_ERROR_CODE code;
    std::string msg;
    
public:
    MEDIA_ERROR_CODE getCode() {
        return this->code;
    }
    
    std::string getMsg() {
        return this->msg;
    }
    
    ErrorCode(MEDIA_ERROR_CODE code, std::string msg) {
        this->code = code;
        this->msg = msg;
    }
};

#endif /* ErrorCode_hpp */
