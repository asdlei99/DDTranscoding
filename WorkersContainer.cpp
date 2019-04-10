//
//  WorkersContainer.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/2.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include "WorkersContainer.hpp"
#include "Utils.hpp"

bool WorkersContainer::addWork(TranscodingWorker* transcodingWorker) {
    if(transMap.count(transcodingWorker->getName()) <= 0) {
        transMap[transcodingWorker->getName()] = transcodingWorker;
    }
    return false;
}

bool WorkersContainer::stopWork(string dst) {
    string name = Utils::commonMd5Secret32(dst);
    if(transMap.count(name) > 0) {
        TranscodingWorker* transcodingWork = transMap[name];
        if(transcodingWork != NULL) {
            transcodingWork->stop();
            return true;
        }
    }
    return false;
}

bool WorkersContainer::clearWork(string dst) {
    string name = Utils::commonMd5Secret32(dst);
    if(transMap.count(name) > 0) {
        TranscodingWorker* transcodingWork = transMap[name];
        if(transcodingWork != NULL) {
            //delete transcodingWork;
            transMap.erase(name);
            return true;
        }
    }
    return false;
}

cJSON* WorkersContainer::getWorkList() {
    map<string, TranscodingWorker*>::iterator it;
    it = transMap.begin();
    cJSON* json = cJSON_CreateArray();
    
    while(it != transMap.end())
    {
        cJSON* item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "name", it->second->getName().c_str());
        cJSON_AddStringToObject(item, "src", it->second->getSrc().c_str());
        cJSON_AddStringToObject(item, "dst", it->second->getDst().c_str());
        cJSON_AddNumberToObject(item, "status", it->second->getStatus());
        cJSON_AddItemToArray(json, item);
        it++;
    }
    return json;
}
