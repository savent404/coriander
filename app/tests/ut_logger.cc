/**
 * @file ut_logger.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gtest/gtest.h>

#include <memory>

#include "coriander/base/ilogger.h"
#include "coriander/base/loggerstream.h"

using namespace coriander::base;

struct Logger : public ILogger {
  void log(const char* msg) noexcept override { mMsg = msg; }
  std::string mMsg;
};

TEST(Logger, basic) {
  Logger logger;
  logger.log("Hello,World");
  EXPECT_EQ(logger.mMsg, "Hello,World");
}

TEST(Logger, stream) {
  auto logger = std::make_shared<Logger>();
  auto os = LoggerStream(logger);

  os << "Hello,World";
  EXPECT_EQ(logger->mMsg, "");
  os << std::endl;
  EXPECT_EQ(logger->mMsg, "Hello,World\n");
}