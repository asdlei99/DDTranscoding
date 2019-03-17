//
//  BaseException.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/6.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef BaseException_hpp
#define BaseException_hpp

#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

enum SERVER_ERROR_CODE {
    UNKOWN_ERROR = 10001,
    HTTP_SERVER_CREATE_ERROR = 10002
};

class BaseException {
private:
    SERVER_ERROR_CODE serverErrorCode;
    string msg;
public:
    BaseException() {
        this->serverErrorCode = UNKOWN_ERROR;
        this->msg = "unkown error";
    }
    
    BaseException(SERVER_ERROR_CODE serverErrorCode, string msg) {
        this->serverErrorCode = serverErrorCode;
        this->msg = msg;
    }
    
    virtual const char * what(void) {
        return msg.c_str();
    }
};

#endif /* BaseException_hpp */
