//
//  MessageQueue.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/10.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef MessageQueue_hpp
#define MessageQueue_hpp

#include <stdio.h>
#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class MessageQueue {
private:
    std::queue<T> msgQueue;
    std::condition_variable cv;
    std::mutex mtx;
public:
    //inline
    void write(T msg) {
        std::unique_lock<std::mutex> lck(mtx);
        msgQueue.push(msg);
        cv.notify_one();
    }
    
    //inline
    T read() {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck);
        T msg = msgQueue.front();
        msgQueue.pop();
        return msg;
    }
    
};

#endif /* MessageQueue_hpp */
