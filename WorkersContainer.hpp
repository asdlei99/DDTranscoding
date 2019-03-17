//
//  WorkersContainer.hpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/2.
//  Copyright © 2019 欧江平. All rights reserved.
//

#ifndef WorkersContainer_hpp
#define WorkersContainer_hpp

#include <stdio.h>
#include <map>
#include "TranscodingWorker.hpp"

extern "C" {
#include <cjson/cJSON.h>
}

using namespace std;

class WorkersContainer {
private:
    map<string, TranscodingWorker*> transMap;
public:
    bool addWork(TranscodingWorker* transcodingWorker);
    bool stopWork(string dst);
    bool clearWork(string dst);
    cJSON* getWorkList();
};

#endif /* WorkersContainer_hpp */
