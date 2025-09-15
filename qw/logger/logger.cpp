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

#include "include/logger.h"

#include <filesystem>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::filesystem::path;
using std::ios;
using std::ofstream;
using std::string;

namespace qw::logging {

  Logger logger;

Logger::Logger() {}

void Logger::log(int priority, string message) {
  if (priority > max_priority_reporting_) {
    return;
  }
  switch (mode_) {
    case LOGGER_MODE_NOLOGGING:
      break;
    case LOGGER_MODE_FILE:
      cout << message << endl;
      cout.flush();
      break;
    case LOGGER_MODE_JOURNAL:
      sd_journal_print(priority, message.c_str());
      break;
  }

  return;
}

void Logger::setMode(LoggerMode mode) {
  mode_ = mode;

  if (mode == LOGGER_MODE_FILE) {
    if ((log_path_ != "") && (log_stream_.is_open() == true)) {
      if (cout.rdbuf() == log_stream_.rdbuf()) {
        /*
         * Set the cout.rdbuf() back to cout's buffer
         */
        cout.rdbuf(cout_buffer_);
      }
      log_stream_.close();
    }
    log_path_ = "";
  }

  return;
}

void Logger::setMode(LoggerMode mode, path log_path) {
  mode_ = mode;

  /*
   * If the current log file is the same as the new one just exit
   */
  if (log_path == log_path_) {
    return;
  }

  log_path_ = log_path;

  /*
   * Check if the current cout.rdbuf() is set to log_stream_
   */
  if (cout.rdbuf() == log_stream_.rdbuf()) {
    /*
     * Set the cout.rdbuf() back to cout's buffer
     */
    cout.rdbuf(cout_buffer_);
  }

  /*
   * If the current file is open close it.
   */
  if (log_stream_.is_open() == true) {
    log_stream_.close();
  }

  /*
   * Assign log_stream_ to the new file
   */
  log_stream_ = ofstream(log_path.string(), ios::out | ios::app);
  /*
   * Assign the cout.rdbuf() to the new file's rdbuf()
   */
  cout_buffer_ = cout.rdbuf();
  cout.rdbuf(log_stream_.rdbuf());

  return;
}

LoggerMode Logger::getMode() {
  return mode_;
}

void Logger::setMaxPriorityReporting(LoggerPriority priority) {
  max_priority_reporting_ = priority;

  return;
}

LoggerPriority Logger::getMaxPriorityReporting() {
  return max_priority_reporting_;
}

Logger::~Logger() {
  /*
   * If the log_strem_.rdbuf() is the same as cout.rdbuf()
   * Put the old cout_buffer_ back to cout.
   */
  if (log_stream_.rdbuf() == cout.rdbuf()) {
    cout.rdbuf(cout_buffer_);
    cout_buffer_ = nullptr;
  }

  return;
}

}  // namespace qw::logging
