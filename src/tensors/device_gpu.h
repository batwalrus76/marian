#pragma once

#include <cmath>
#include <cstdint>

#include "common/definitions.h"

namespace marian {

class DeviceGPU {
private:
  uint8_t* data_;
  size_t size_;
  DeviceId deviceId_;
  size_t alignment_;

  size_t align(size_t size) {
    return ceil(size / (float)alignment_) * alignment_;
  }

public:
  DeviceGPU(DeviceId deviceId, size_t alignment = 256)
      : data_(0), size_(0), deviceId_(deviceId), alignment_(alignment) {}

  ~DeviceGPU();

  void reserve(size_t size);

  uint8_t* data() { return data_; }

  size_t size() { return size_; }

  DeviceId getDevice() { return deviceId_; }
};
}