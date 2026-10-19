#include <cmath>
#include "esphome/components/opentherm/opentherm_macros.h"
#include "gtest/gtest.h"

namespace esphome::opentherm::testing {

namespace {
struct NumericReading {
  float state;
  void publish_state(float value) { this->state = value; }
};

struct BinaryReading {
  bool state;
  bool has_state{true};
  void invalidate_state() { this->has_state = false; }
};

struct Readings {
  NumericReading *temperature{nullptr};
  NumericReading *modulation{nullptr};
  BinaryReading *flame{nullptr};

  void invalidate() {
    OPENTHERM_INVALIDATE_SENSOR(temperature)
    OPENTHERM_INVALIDATE_SENSOR(modulation)
    OPENTHERM_INVALIDATE_BINARY_SENSOR(flame)
  }
};
}  // namespace

TEST(OpenthermTelemetry, LostConnectionInvalidatesReadingsWithoutInventingZeroes) {
  NumericReading temperature{63.5f}, modulation{42.0f};
  BinaryReading flame{true};
  Readings readings{&temperature, &modulation, &flame};
  readings.invalidate();
  EXPECT_TRUE(std::isnan(temperature.state));
  EXPECT_TRUE(std::isnan(modulation.state));
  EXPECT_FALSE(flame.has_state);
  EXPECT_TRUE(flame.state);  // Invalidation must not publish a synthetic OFF state.
}

TEST(OpenthermTelemetry, RecoveryNeedsFreshValueForEachReading) {
  NumericReading temperature{63.5f}, modulation{42.0f};
  Readings readings{&temperature, &modulation};
  readings.invalidate();
  temperature.publish_state(61.0f);
  EXPECT_FLOAT_EQ(temperature.state, 61.0f);
  EXPECT_TRUE(std::isnan(modulation.state));
}

TEST(OpenthermTelemetry, MissingOptionalReadingsAreSafe) {
  Readings readings;
  EXPECT_NO_FATAL_FAILURE(readings.invalidate());
}

}  // namespace esphome::opentherm::testing
