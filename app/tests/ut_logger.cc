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

#include <memory>

#include "coriander/base/ilogger.h"
#include "coriander/base/loggerstream.h"

using coriander::base::ILogger;
using coriander::base::LoggerStream;

struct MockLogger : public ILogger {
  MOCK_METHOD(void, log, (const char* msg), (noexcept));
};

TEST(Logger, basic) {
  MockLogger logger;

  EXPECT_CALL(logger, log("Hello,World")).Times(1);
  logger.log("Hello,World");
}

TEST(Logger, stream) {
  auto logger = std::make_shared<MockLogger>();
  auto os = LoggerStream(logger);

  // only print once for the whole line
  EXPECT_CALL(*logger, log(testing::_)).Times(1);
  os << "Hello,World";
  os << std::endl;
  os << "FOO"
     << "FFFF";
}
