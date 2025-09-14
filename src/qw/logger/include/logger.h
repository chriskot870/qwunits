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

#ifndef SRC_LIB_QW_LOGGER_INCLUDE_LOGGER_H_
#define SRC_LIB_QW_LOGGER_INCLUDE_LOGGER_H_

#include <syslog.h>
#include <systemd/sd-journal.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using std::string;

namespace qw::logging {

enum LoggerMode {
  LOGGER_MODE_NOLOGGING,
  LOGGER_MODE_FILE,
  LOGGER_MODE_JOURNAL,
};

/*
 * These must match syslog.h values
 */
enum LoggerPriority {
  LOGGER_EMERG,   /* 0 system is unusable */
  LOGGER_ALERT,   /* 1 action must be taken immediately */
  LOGGER_CRIT,    /* 2 critical conditions */
  LOGGER_ERR,     /* 3 error conditions */
  LOGGER_WARNING, /* 4 warning conditions */
  LOGGER_NOTICE,  /* 5 normal but significant condition */
  LOGGER_INFO,    /* 6 informational */
  LOGGER_DEBUG    /*7 debug-level messages */
};

class Logger {
 public:
  Logger();

  void log(int priority, string message);

  void setMode(LoggerMode mode);

  void setMode(LoggerMode mode, std::filesystem::path log_path);

  LoggerMode getMode();

  void setMaxPriorityReporting(LoggerPriority level);

  LoggerPriority getMaxPriorityReporting();

  ~Logger();

 private:
  LoggerMode mode_ = LOGGER_MODE_NOLOGGING;

  LoggerPriority max_priority_reporting_ = LOGGER_DEBUG;

  std::ofstream log_stream_;

  std::streambuf* cout_buffer_ = nullptr;

  std::filesystem::path log_path_ = "";
};

extern Logger logger;

}  // namespace qw::logging

#endif  // SRC_LIB_QW_LOGGER_INCLUDE_LOGGER_H_
