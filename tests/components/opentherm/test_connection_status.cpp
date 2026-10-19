#include "esphome/components/opentherm/connection_status.h"
#include "gtest/gtest.h"

namespace esphome::opentherm::testing {

TEST(OpenthermConnectionStatus, DisconnectedUntilFirstResponse) {
  ConnectionStatus status;
  EXPECT_FALSE(status.is_connected(0, 10000));
  EXPECT_FALSE(status.is_connected(60000, 10000));
  status.record_response(0);
  EXPECT_TRUE(status.is_connected(0, 10000));
}

TEST(OpenthermConnectionStatus, TimeoutAndRecovery) {
  ConnectionStatus status;
  status.record_response(123);
  EXPECT_TRUE(status.is_connected(10122, 10000));
  EXPECT_FALSE(status.is_connected(10123, 10000));
  EXPECT_FALSE(status.is_connected(60000, 10000));
  status.record_response(60001);
  EXPECT_TRUE(status.is_connected(60001, 10000));
}

TEST(OpenthermConnectionStatus, SuccessfulRepliesRefreshDeadline) {
  ConnectionStatus status;
  status.record_response(1000);
  status.record_response(10000);
  EXPECT_TRUE(status.is_connected(15000, 10000));
  EXPECT_FALSE(status.is_connected(20000, 10000));
}

TEST(OpenthermConnectionStatus, MillisWraparound) {
  ConnectionStatus status;
  status.record_response(0xFFFFFF00);
  EXPECT_TRUE(status.is_connected(9743, 10000));
  EXPECT_FALSE(status.is_connected(9744, 10000));
  status.record_response(10000);
  EXPECT_TRUE(status.is_connected(10000, 10000));
}

TEST(OpenthermConnectionStatus, CriticalErrorResetsConnection) {
  ConnectionStatus status;
  status.record_response(1000);
  status.reset();
  EXPECT_FALSE(status.is_connected(1001, 10000));
}

TEST(OpenthermConnectionStatus, ExpiredConnectionCannotReturnAfterClockWraps) {
  ConnectionStatus status;
  status.record_response(123);
  EXPECT_FALSE(status.is_connected(10123, 10000));
  EXPECT_FALSE(status.is_connected(124, 10000));
}

}  // namespace esphome::opentherm::testing
