//
//  WorkContainerProxy.cpp
//  DDTranscoding
//
//  Created by 欧江平 on 2019/3/10.
//  Copyright © 2019 欧江平. All rights reserved.
//

#include "WorkContainerProxy.hpp"

WorkContainerProxy* WorkContainerProxy::proxy = NULL;
std::thread* WorkContainerProxy::th = NULL;
WorkersContainer* WorkContainerProxy::workersContainer;
MessageQueue<ControllCmdContext>* WorkContainerProxy::msgQueue;
