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
#include "include/qwgpio_group.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <expected>
#include <cstring>

using std::expected;
using std::string;
using std::unexpected;

namespace qw::devices {

QwGpioGroup::QwGpioGroup(string key, std::shared_ptr<QwGpioChip> chip_parent,
                         struct gpio_v2_line_request request)
    : key_(key), chip_parent_(chip_parent) {

  num_lines_ = request.num_lines;
  memcpy(offsets_, request.offsets, num_lines_ * sizeof(__u32));
  strncpy(consumer_, request.consumer, GPIO_MAX_NAME_SIZE);
  config_ = request.config;
  event_buffer_size_ = request.event_buffer_size;
  fd_ = request.fd;

  /*
   * This limit_mask_ limits the mask for get and set vlaues.
   * You don't want to allow masks more than the number of lines
   * in a group.
   */
  limit_mask_ = (1 << num_lines_) - 1;

  return;
}

std::expected<void, int> QwGpioGroup::setValue(__u32 offset) {
  struct gpio_v2_line_values data = {};  // Zero out the data structure
  set_gpio_line(&data, offset);  // Set the one bit
  return setValues(&data);  // Go call setValues()
}

std::expected<void, int> QwGpioGroup::unsetValue(__u32 offset) {
  struct gpio_v2_line_values data = {};  // Zero out the data structure
  unset_gpio_line(&data, offset);  // Set the mask to 1 and bits to 0
  return setValues(&data);  // Go call setValues()
}

std::expected<void, int> QwGpioGroup::setValues(struct gpio_v2_line_values *data) {
  struct gpio_v2_line_values values;

  if (fd_ < 0) {
    return unexpected(ENODEV);
  }
  // values.bits = data->bits;
  // You can't ask for items beyond the num_lines so apply the limit mask
  // values.mask = data->mask & limit_mask_;

  int result = ::ioctl(fd_, GPIO_V2_LINE_SET_VALUES_IOCTL, data);
  if (result != 0) {
    return unexpected(errno);
  }

  return {};
}

expected<bool, int> QwGpioGroup::getValue(__u32 offset) {
  struct gpio_v2_line_values data = {};
  data.mask |= 1 << offset;
  expected<uint64_t, int> ex_values = getValues(&data);
  if (ex_values.has_value() != true) {
    return unexpected(ex_values.error());
  }
  if ((ex_values.value() & data.mask) != data.mask) {
    return false;
  }

  return true;
}

std::expected<uint64_t, int> QwGpioGroup::getValues(struct gpio_v2_line_values *data) {
  struct gpio_v2_line_values values;

  if (fd_ < 0) {
    return unexpected(ENODEV);
  }

  // values.bits = 0;  // It doesn't matter what bits is. They will be set by ioctl
  // You can't ask for items beyond the num_lines so apply the limit mask
  // values.mask = data.mask & limit_mask_;

  int result = ::ioctl(fd_, GPIO_V2_LINE_GET_VALUES_IOCTL, data);
  if (result != 0) {
    return unexpected(errno);
  }

  // We just need to return the bits. The calling routine should know what mask
  // They used and so which bits are important to them.
  return values.bits;
}

int QwGpioGroup::getFd() {
  return fd_;
}

void QwGpioGroup::close() {
  if (fd_ >= 0) {
    ::close(fd_);
    fd_ = QW_GPIO_FD_NOT_OPENED;
  }

  return;
}

expected<struct gpio_v2_line_event, int> QwGpioGroup::readEvent() {
  struct gpio_v2_line_event event;

  if (fd_ < 0) {
    return unexpected(ENODEV);
  }

  int result = ::read(fd_, &event, sizeof(struct gpio_v2_line_event));
  if (result == 0) {
    return unexpected(ENODATA);
  }
  if (result != sizeof(struct gpio_v2_line_event)) {
    return unexpected(errno);
  }

  return event;
}

expected<void, int> QwGpioGroup::configure(struct gpio_v2_line_config config) {
  if (fd_ < 0) {
    return unexpected(ENODEV);
  }

  int result = ::ioctl(fd_, GPIO_V2_LINE_SET_CONFIG_IOCTL, &config);
  if (result != 0) {
    return unexpected(errno);
  }

  return {};
}


std::expected<void, int> QwGpioGroup::setDebouncePeriod(__u32 offset,
                                             std::chrono::microseconds period) {
  struct gpio_v2_line_config config = {};

  // There is only 1 attribute for debouncing
  config.num_attrs = 1;
  // Set the id for debouncing and period values
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
  config.attrs[0].attr.debounce_period_us = period.count();
  // Identify which line this applies to.
  config.attrs[0].mask = (1 << offset);

  return configure(config);
}

std::expected<void, int> QwGpioGroup::setDebouncePeriods(__u64 offsets,
                                              std::chrono::microseconds period) {
  struct gpio_v2_line_config config = {};

  // There is only 1 attribute for debouncing
  config.num_attrs = 1;
  // Set the id for debouncing and period values
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_DEBOUNCE;
  config.attrs[0].attr.debounce_period_us = period.count();
  // Identify which line this applies to.
  config.attrs[0].mask = offsets;

  return configure(config);
}

std::expected<void, int> QwGpioGroup::setFlag(__u32 offset, __u64 flags) {
  struct gpio_v2_line_config config = {};  // zero out all values

  // There is only 1 attribute to set a flag
  config.num_attrs = 1;
  // Set the flag values
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_FLAGS;
  config.attrs[0].attr.flags = flags;
  // Identify which line this applies to.
  config.attrs[0].mask = (1 << offset);

  return configure(config);
}

std::expected<void, int> QwGpioGroup::setFlags(__u64 offsets, __u64 flags) {
  struct gpio_v2_line_config config = {};  // zero out all values

  config.flags = 0;
  // There is only 1 attribute to set a flag
  config.num_attrs = 1;
  // Set the flag values
  config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_FLAGS;
  config.attrs[0].attr.flags = flags;
  // Identify which line this applies to.
  config.attrs[0].mask = offsets;

  return configure(config);
}

QwGpioGroup::~QwGpioGroup() {
  // delete the group from the chip group's list
  chip_parent_->deleteGroup(key_);

  if (fd_ >= 0) {
    ::close(fd_);
  }
}

}  // namespace qw::devices
