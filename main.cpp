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
    // insert code here...
    /*
    std::cout << "Hello, World!\n";
    TranscodingWorker transcodingWorker("camera1", "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov", "rtmp://127.0.0.1:1935/live/camera1");
    cout<<"befor"<<endl;
    transcodingWorker.start();
    transcodingWorker.join();
    cout<<"after"<<endl;
     */
    
    HttpServer* httpServer = new HttpServer("0.0.0.0", 4253);
    httpServer->dispatch();
    return 0;
}
