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

#ifndef QW_UNITS_INCLUDE_UNIT_MEASUREMENT_H_
#define QW_UNITS_INCLUDE_UNIT_MEASUREMENT_H_

#include <algorithm>
#include <chrono>
#include <deque>
#include <expected>  // cpplint counts this as a c system header // NOLINT

#include "fmt/chrono.h"
#include "fmt/format.h"

/*
 * I broke the rule of not defining using in a header.
 * The reason is I may want to switch from system_clock
 * to utc_clock. It is noce to have that in one place.
 * Changing it in MeasurementTimeClock will then 
 * change it in all files that need the clock used
 * nby measurments.
 * Of course they should all use MeasurementTimeClock
 */
using MeasurementTimeClock = std::chrono::time_point<std::chrono::system_clock>;

namespace qw::units {

constexpr std::chrono::seconds kMaxHistoryTimeSpan(60 *
                                                10);  // 10 minutes of samples
constexpr std::chrono::seconds kHistoryInterval10m(60 * 10);
constexpr std::chrono::seconds kHistoryInterval2m(60 * 2);

template<typename Tunit>
class UnitMeasurement {
 public:
  UnitMeasurement() {}

  UnitMeasurement(Tunit measurement, Tunit accuracy, MeasurementTimeClock time_stamp)
    : measurement_(measurement), accuracy_(accuracy), time_stamp_(time_stamp) {}

  Tunit measurement() {
    return measurement_;
  }

  Tunit accuracy() {
    return accuracy_;
  }

  MeasurementTimeClock timeStamp() {
    return time_stamp_;
  }
 private:
  Tunit measurement_;
  Tunit accuracy_;
  MeasurementTimeClock time_stamp_;
};

template<typename Tmunit>
concept IsUnitMeasurement = requires(qw::units::UnitMeasurement<Tmunit> obj) {
  // Make sure the 3 functions return the expected types
  { obj.timeStamp()}-> std::same_as<MeasurementTimeClock>;
  { obj.measurement()} -> std::same_as<Tmunit>;
  { obj.accuracy()} -> std::same_as<Tmunit>;
};

template<typename Tmunit>
requires IsUnitMeasurement<Tmunit>
class MeasurementHistory {
 public:
  MeasurementHistory() {}

  explicit MeasurementHistory(std::chrono::seconds maximum_time) : maximum_time_(maximum_time) {}

  void setMaximumTime(std::chrono::seconds time_span) {
    maximum_time_ = time_span;

    /*
     * Since we changed the time span cleanup the list with the new maximum_time
     */
    prune();

    return;
  }

  std::chrono::seconds getMaximumTime() { return maximum_time_; }

  size_t size() {
    /*
     * Get rid of any old entries before returning a count
     */
    prune();

    size_t count = history_.size();

    return count;
  }

  bool empty() {
    // Call size() it will do the pruning
    bool result = size() == 0 ? true : false;

    return result;
  }

  std::expected<qw::units::UnitMeasurement<Tmunit>, int> last() {
    if (history_.empty()) {
      return std::unexpected(ERANGE);
    }

    /*
     * Get rid of any old entries before returning a count
     */
    prune();

    return history_.back();
  }

  size_t countOverPeriod(std::chrono::seconds time_span) {
    size_t count = 0;
    auto current_time = std::chrono::system_clock::now();
    /*
     * Get rid of any old entries before returning a count
     */
    prune();

    /*
     * Iterate through the list backwards, which is from newest measurement.
     */
    for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
      if ((current_time - (*it).timeStamp()) <= time_span) {
        count++;
      }
    }
    return count;
  }

  void add(qw::units::UnitMeasurement<Tmunit> data) {
    /*
   * We take every opportunity to keep the list small
   * so clean up measurements that are out of range before
   * adding any new items.
   */
  prune();

    /*
   * Make sure the speed is within the maximum time
   */
    auto current_time = std::chrono::system_clock::now();
    if ((current_time - data.timeStamp()) <= maximum_time_) {
      history_.push_back(data);
    }

    /*
   * We need the deque to be ordered by time.
   * On the chance the measurements got added in different order
   * we want to sort by time whenever a new measurement is added.
   * This way we can be sure of the order of the measurements by time.
   * We use a lambda function to make the comparison.
   * We go in reverse order so the older (smaller) times get moved to the front.
   * THis way when we read from the back we get the newest times. As a time moves
   * to the front it gets removed by prune.
   */
    sort(history_.rbegin(), history_.rend(),
         [](qw::units::UnitMeasurement<Tmunit> a, qw::units::UnitMeasurement<Tmunit> b) {
           /*
     * We want the older times near the front when going in reverse order.
     * So, if a.timeStamp() > b.timeStamp() return true.
     */
           if (a.timeStamp() > b.timeStamp()) {
             return true;
           }
           return false;
         });

    return;
  }

  std::expected<Tmunit, int> average(std::chrono::seconds time_span) {
    uint count = 0;
    Tmunit total(0);

    prune();

    /*
     * If history is empty return an error so we don't divide by 0
     */
    if (history_.empty() == true) {
      return std::unexpected(ENODATA);
    }

    /*
     * Iterate through the list backwards, which is from newest measurement.
     */
    auto current_time = std::chrono::system_clock::now();
    for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
      /*
       * If it is within the time stamp add the value
       */
      if ((current_time - (*it).timeStamp()) <= time_span) {
        count++;
        /*
         * Now get the base_value of the measurment value.
         * add it to the total. 
         * Then create a new mph for it.
         * SpeedHistory has to be a friend of MilesPerHour for
         * this to work.
         */
        Tmunit tval = (*it).measurement();
        total += tval;
      } else {
        break;
      }
    }

    if (count == 0) {
      return std::unexpected(ENOTSUP);
    }

    Tmunit ave_mph = total / count;

    return ave_mph;
  }

  std::expected<qw::units::UnitMeasurement<Tmunit>, int> gust(std::chrono::seconds time_span) {
    prune();

    /*
     * Return error if there is no data
     */
    if (history_.empty() == true) {
      return std::unexpected(ENODATA);
    }

    auto current_time = std::chrono::system_clock::now();
    qw::units::UnitMeasurement<Tmunit> max_measurement = history_.back();
    for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
      /*
       * If we go past the time_span then exit loop
       */
      if ((current_time - (*it).timeStamp()) > time_span) {
        break;
      }
      /*
       * If this value is greater then the maximum, make it the maximum
       */
      if ((*it).measurement() > max_measurement.measurement()) {
        max_measurement = (*it);
      }
    }

    return max_measurement;
  }

 private:
  std::deque<qw::units::UnitMeasurement<Tmunit>> history_;

  std::chrono::seconds maximum_time_ = kMaxHistoryTimeSpan;

  void prune() {
    auto current_time = std::chrono::system_clock::now();

    /*
     * We look at the fron of the history which is the oldest one.
     * If the time stamp is greater than the maximum time we pop it
     * off the history deque. We keep doing this till we run into an
     * element that is less than or equal to the maximum_time or there
     * are no elements left in the history.
     */
    while ((history_.empty() != true) &&
           ((current_time - history_.front().timeStamp()) > maximum_time_)) {
      history_.pop_front();
    }

    return;
  }
};

}  // namespace qw::units

#endif  // QW_UNITS_INCLUDE_UNIT_MEASUREMENT_H_
