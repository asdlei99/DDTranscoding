//
//  WorkContainerProxy.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/10.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef WorkContainerProxy_hpp
#define WorkContainerProxy_hpp

#include <stdio.h>
#include <thread>
#include <iostream>

#include "MessageQueue.hpp"
#include "WorkersContainer.hpp"

enum ControllCmd {
    CONTROLL_CMD_EXIT = 0,
    CONTROLL_CMD_ADD,
    CONTROLL_CMD_DEL
};

typedef struct ControllCmdContext {
    ControllCmd cmd;
    std::string src;
    std::string dst;
    void* arg;
} ControllCmdContext;

class WorkContainerProxy {
private:
    WorkContainerProxy(){
    }

    ~WorkContainerProxy() {
    }
    static std::thread* th;
    static WorkersContainer* workersContainer;
    static WorkContainerProxy* proxy;
    static MessageQueue<ControllCmdContext>* msgQueue;
public:
    static WorkContainerProxy* getWorkContainerProxy(WorkersContainer* container) {
        if(proxy == NULL) {
            workersContainer = container;
            msgQueue = new MessageQueue<ControllCmdContext>();
            proxy = new WorkContainerProxy();
            th = new std::thread(loop);

        }
        return NULL;
    }

    static WorkContainerProxy* getWorkContainerProxy() {
        return proxy;
    }

    static void loop() {
        while(true) {
            ControllCmdContext controllCmdContext = msgQueue->read();
            switch (controllCmdContext.cmd) {
                case CONTROLL_CMD_EXIT:
                {
                    workersContainer->clearWork(controllCmdContext.dst);
                }
                    break;
                case CONTROLL_CMD_ADD:
                {
                    TranscodingWorker *transcodingWorker = new TranscodingWorker(controllCmdContext.src, controllCmdContext.dst);
                    transcodingWorker->start();
                    workersContainer->addWork(transcodingWorker);
                }
                    break;
                case CONTROLL_CMD_DEL:
                {
                    workersContainer->stopWork(controllCmdContext.dst);
                }
                    break;
                default:
                    break;
            }
        }
    }

    static void clearWork(std::string dst) {
        ControllCmdContext context = {CONTROLL_CMD_EXIT, "", dst};
        msgQueue->write(context);
    }

    static void add(string src, string dst) {
        ControllCmdContext context = {CONTROLL_CMD_ADD, src, dst};
        msgQueue->write(context);
    }

    static void del(string dst) {
        ControllCmdContext context = {CONTROLL_CMD_DEL, "", dst};
        msgQueue->write(context);
    }
};

#endif /* WorkContainerProxy_hpp */
