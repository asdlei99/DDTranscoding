//
//  TranscodingWorker.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/2/27.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include "TranscodingWorker.hpp"
#include "Utils.hpp"

TranscodingWorker::TranscodingWorker() {
    this->name = name;
}

TranscodingWorker::TranscodingWorker(string src, string dst) {
    controlContext = (ControlContex*) malloc(sizeof(ControlContex));
    this->name = Utils::commonMd5Secret32(dst);
    this->src = src;
    this->dst = dst;
}

TranscodingWorker::~TranscodingWorker() {
    free(controlContext);
}

void TranscodingWorker::start() {
    set_running_flag(controlContext,  RUNNING_FLAG_PREPARE);
    this->th = thread(transcoding, src.c_str(), dst.c_str(), controlContext);
}

void TranscodingWorker::stop() {
    set_running_flag(controlContext,  RUNNING_FLAG_STOPPING);
}

void TranscodingWorker::join() {
    this->th.join();
}
