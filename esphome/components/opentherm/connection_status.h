#pragma once

#include <cstdint>

namespace esphome::opentherm {

// Tracks verified replies, independently of the configured boiler sensors.
class ConnectionStatus {
 public:
  void record_response(uint32_t now) {
    this->last_response_ = now;
    this->received_response_ = true;
  }

  bool is_connected(uint32_t now, uint32_t timeout) {
    if (this->received_response_ && static_cast<uint32_t>(now - this->last_response_) >= timeout)
      this->received_response_ = false;
    return this->received_response_;
  }

  void reset() { this->received_response_ = false; }

 protected:
  uint32_t last_response_{0};
  bool received_response_{false};
};

}  // namespace esphome::opentherm
