#include <iostream>
#include <cstdio>
#include <memory>
#include <ostream>
#include <unistd.h>
#include "../src/log.h" 
#include "../src/util.h"

int main() {
    // test1
    sylar::Logger::ptr logger{new sylar::Logger{"root"}};
    logger->addAppender(sylar::StdoutLoggerAppender::ptr{new sylar::StdoutLoggerAppender});
    // SYLAR_LOG_DEBUG(logger) << "test macro";

    // test2
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("root")) << "test macro";
}
