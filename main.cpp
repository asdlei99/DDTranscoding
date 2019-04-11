//
//  main.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/2/27.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include <iostream>
#include "TranscodingWorker.hpp"
#include "HttpServer.hpp"
#include "MessageQueue.hpp"
#include <unistd.h>

using namespace std;

extern "C" {
    #include <unistd.h>
}

using namespace std;

static void msg_read(MessageQueue<int>* msg) {
    while(true) {
        cout<<msg->read()<<endl;
    }
}

static void msg_write(MessageQueue<int>* msg) {
    int count = 0;
    while (true) {
        msg->write(count++);
        sleep(1);
    }
}

int main(int argc, const char * argv[]) {
    int pid = fork();
    if(pid < 0) {
        perror("DDTranscoding start error");
    } else if(pid == 0) {
        cout<<"start DDTranscoding success!"<<endl;
        setsid();
        HttpServer* httpServer = new HttpServer("0.0.0.0", 4253);
        httpServer->dispatch();
    }
    return 0;
}
