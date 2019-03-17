//
//  TranscodingWorker.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/2/27.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef TranscodingWorker_hpp
#define TranscodingWorker_hpp

#include <stdio.h>

#include <iostream>
#include <thread>

#include "Transcoding.hpp"

using namespace std;

class TranscodingWorker {
private:
    thread th;
    string name;
    string src;
    string dst;
    ControlContex* controlContext;
public:
public:
    TranscodingWorker();
    TranscodingWorker(string src, string dst);
    
    string getSrc() {
        return this->src;
    }
    
    string getDst() {
        return this->dst;
    }
    
    void setSrc(string src) {
        this->src = src;
    }
    
    void setDst(string dst) {
        this->dst = dst;
    }
    
    string getName() {
        return this->name;
    }
    
    void setName(string name) {
        this->name = name;
    }
    
    void start();
    void stop();
    void join();
    
    int getStatus() {
        return get_running_flag(controlContext);
    }
    
    ~TranscodingWorker();
};

#endif /* TranscodingWorker_hpp */
