/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "onevn/components/onevn_sync/tools.h"

#include <string>
#include <sstream>

#include "crypto/random.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#include "build/build_config.h"

namespace onevn_sync {

namespace tools {

std::string GenerateObjectId() {
  //16 random 8-bit unsigned numbers
  const size_t length = 16;
  uint8_t bytes[length];
  crypto::RandBytes(bytes, sizeof(bytes));
  std::stringstream ss;
  for (size_t i = 0; i < length; ++i) {
    const uint8_t &byte = bytes[i];
    ss << std::dec << (int)byte;
    if (i != length - 1) {
      ss << ", ";
    }
  }
  return ss.str();
}

std::string GetPlatformName() {
  #if defined(OS_ANDROID)
      const std::string platform = "android";
  #elif defined(OS_WIN)
      const std::string platform = "windows";
  #elif defined(OS_LINUX)
      const std::string platform = "linux";
  #elif defined(OS_MACOSX)
      const std::string platform = "macosx";
  #elif defined(OS_IOS)
      const std::string platform = "ios";
  #endif
  return platform;
}

bool IsTimeEmpty(const base::Time &time) {
  return time.is_null() || base::checked_cast<int64_t>(time.ToJsTime()) == 0;
}

} // namespace tools

} // namespace onevn_sync
