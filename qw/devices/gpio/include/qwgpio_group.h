/*
 * Copyright 2024,2025, Chris Kottaridis. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the copyright holders nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * NTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef QW_DEVICES_GPIO_INCLUDE_QWGPIO_GROUP_H_
#define QW_DEVICES_GPIO_INCLUDE_QWGPIO_GROUP_H_

#include <linux/gpio.h>

#include <expected>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "qw/devices/gpio/include/qwgpio.h"
#include "qw/devices/gpio/include/qwgpio_chip.h"

namespace qw::devices {

class QwGpioChip;  // predeclare

constexpr void set_gpio_line(struct gpio_v2_line_values* data, __u32 offset) {
  data->bits |= 1 << offset;
  data->mask |= 1 << offset;
}

constexpr void set_gpio_lines(struct gpio_v2_line_values* data,
                              const std::vector<__u32>& offsets) {
  for (const uint32_t offset : offsets) {
    data->bits |= 1 << offset;
    data->mask |= 1 << offset;
  }
}

constexpr void unset_gpio_line(struct gpio_v2_line_values* data, __u32 offset) {
  data->bits |= ~(1 << offset);
  data->mask |= 1 << offset;
}

constexpr void unset_gpio_lines(struct gpio_v2_line_values* data,
                                const std::vector<__u32>& offsets) {
  for (const uint32_t offset : offsets) {
    data->bits |= ~(1 << offset);
    data->mask |= 1 << offset;
  }
}

constexpr void clear_gpio_lines(struct gpio_v2_line_values* data) {
  data->bits = 0;
  data->mask = 0;
}

constexpr bool gpio_line_is_set(uint64_t* data, __u32 offset) {
  if ((*data & (1 << offset)) == 0)
    return false;
  return true;
}

constexpr bool gpio_line_is_unset(uint64_t* data, __u32 offset) {
  if ((*data & (1 << offset)) == 0)
    return true;
  return false;
}

class QwGpioGroup {
  friend class QwGpioChip;

 public:
  // The constructors are private and can only be called by friends

  std::expected<void, int> setValue(__u32 offset);

  std::expected<void, int> unsetValue(__u32 offset);

  std::expected<void, int> setValues(struct gpio_v2_line_values* data);

  std::expected<bool, int> getValue(__u32 offset);

  std::expected<uint64_t, int> getValues(struct gpio_v2_line_values* data);

  int getFd();

  std::expected<struct gpio_v2_line_event, int> readEvent();

  std::expected<struct gpio_v2_line_event, int> isEventAvailable();

  std::expected<void, int> configure(struct gpio_v2_line_config);

  std::expected<void, int> setDebouncePeriod(__u32 offset,
                                             std::chrono::microseconds period);

  std::expected<void, int> setDebouncePeriods(__u64 offsets,
                                              std::chrono::microseconds period);

  std::expected<void, int> setFlag(__u32 offset, __u64 flags);

  std::expected<void, int> setFlags(__u64 offsets, __u64 flags);

  void close();

  ~QwGpioGroup();

 private:
  __u32 offsets_[GPIO_V2_LINES_MAX] = {};
  char consumer_[GPIO_MAX_NAME_SIZE] = {};
  struct gpio_v2_line_config config_ = {};
  __u32 num_lines_ = 0;
  __u32 event_buffer_size_ = 0;
  /* Pad to fill implicit padding and reserve space for future use. */
  // __u32 padding[5]; we don't need to save padding here
  __s32 fd_ = QW_GPIO_FD_NOT_OPENED;
  __u32 limit_mask_ = 0;
  std::string key_ = {};
  std::shared_ptr<QwGpioChip> chip_parent_ = {};

  /*
   * Constructors are private and only made by QwGpioChip
   */
  explicit QwGpioGroup(std::string key, std::shared_ptr<QwGpioChip> parent_chip,
                       struct gpio_v2_line_request);
};

}  // namespace qw::devices

#endif  // QW_DEVICES_GPIO_INCLUDE_QWGPIO_GROUP_H_
