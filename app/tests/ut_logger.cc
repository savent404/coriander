/**
 * @file ut_logger.cc
 * @author Savent Gate (savent_gate@outlook.com)
 * @brief
 * @date 2023-07-31
 *
 * Copyright 2023 savent_gate
 *
 */
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>

#include "coriander/base/ilogger.h"

using coriander::base::ILogger;

struct MockLogger : public ILogger {
  MOCK_METHOD(void, log, (const char* msg), (noexcept));
};
struct MockLogger2 : public ILogger {
  virtual void log(const char* msg) noexcept {
    prev_msg = msg;
    std::cout << msg << std::endl;
  }

 public:
  const char* prev_msg = nullptr;
};
TEST(Logger, basic) {
  MockLogger logger;

  EXPECT_CALL(logger, log("Hello,World")).Times(1);
  logger.log("Hello,World");
}

TEST(Logger, fmt) {
  auto logger = std::make_shared<MockLogger2>();

  CORIANDER_LOG_INFO(logger, "Hello,World!");
  ASSERT_STREQ(logger->prev_msg, "[INFO] Hello,World!");
}
