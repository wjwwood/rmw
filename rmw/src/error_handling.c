// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <rmw/allocators.h>
#include <rmw/error_handling.h>
#include <rmw/impl/config.h>
#include <rmw/macros.h>

// It's not recommended to set RMW_ERROR_STRING_SIZE lower than 256 + RMW_LINE_NUMBER_STRING_SIZE.
// These are only used when RMW_AVOID_MEMORY_ALLOCATION is 1.
#define RMW_ERROR_STRING_SIZE 4096
#define RMW_LINE_NUMBER_STRING_SIZE 255
#define RMW_MIN_ERROR_MSG_LEN 64

#if RMW_AVOID_MEMORY_ALLOCATION
RMW_THREAD_LOCAL char __rmw_error_string[RMW_ERROR_STRING_SIZE] = "\0";
#else
RMW_THREAD_LOCAL char * __rmw_error_string = 0;
#endif
static const char __fragment_1[] = ", at ";
static const char __fragment_2[] = ":";
static const char __truncate_str[] = "...";

void
rmw_set_error_state(const char * error_string, const char * file, int line_number)
{
#if RMW_AVOID_MEMORY_ALLOCATION
  auto error_string_ = (error_string ? error_string : "");
  auto error_string_len = strlen(error_string_);
  auto file_ = (file ? file : "");
  auto file_len = strlen(file_);
  char line_number_[RMW_LINE_NUMBER_STRING_SIZE] = "\0";
  auto line_number_length = snprintf(line_number_, sizeof(line_number_), "%d", line_number);
  if (line_number_length < 0 || line_number_length > RMW_LINE_NUMBER_STRING_SIZE) {
    // Could not encode the line number into a string.
#if RMW_REPORT_ERROR_HANDLING_ERRORS
    fprintf(
      stderr, "[rmw|error_handling.c:" RMW_STR(__LINE__)
      "] failed to encode line number in string of length "
      RMW_STR(RMW_LINE_NUMBER_STRING_SIZE));
#endif
    line_number_[0] = '\0';
    line_number_length = 0;
  }
  // Calculate the length of the resulting error message.
  auto length_of_other_stuff = file_len;
  if (file_len != 0) {
    length_of_other_stuff += strlen(__fragment_1);
    if (line_number_length != 0) {
      length_of_other_stuff += strlen(__fragment_2);
      length_of_other_stuff += line_number_length;
    }
  }
  // First clear the storage.
  rmw_reset_error_string();
  // Then start to fill in the error string, truncating where needed.
  // As long as RMW_ERROR_STRING_SIZE is at least 512, it should leave enough room for all parts.
  // E.g.: 512 - 255 for the line nummber - 64 for the min error msg - 64 for the min file path,
  //       which leaves 128 for the fragments (', at ' and ':') and more file path or error msg.
  auto total_length = error_string_len + length_of_other_stuff;
  auto min_threshold = RMW_ERROR_STRING_SIZE - (RMW_MIN_ERROR_MSG_LEN + sizeof(__truncate_str));
  auto amount_written = 0;
  if (total_length >= RMW_ERROR_STRING_SIZE) {
    // The message will be truncated.
#if RMW_REPORT_ERROR_HANDLING_ERRORS
    fprintf(
      stderr,
      "[rmw|error_handling.c:%d] failed to encode complete error string into a string of length "
      "%d so the error messsage will be truncated\n",
      __LINE__, RMW_STR(RMW_ERROR_STRING_SIZE));
#endif
    if (length_of_other_stuff >= min_threshold) {
      // Copy the minimum message into the error string storage, and then add the truncation str.
      strncpy(__rmw_error_string, error_string_, RMW_MIN_ERROR_MSG_LEN);
      strncpy(__rmw_error_string + RMW_MIN_ERROR_MSG_LEN, __truncate_str, sizeof(__truncate_str));
      amount_written += RMW_MIN_ERROR_MSG_LEN + sizeof(__truncate_str);
    } else {
      // Copy as much of the message into the strin as there is space.
      strncpy(__rmw_error_string, error_string_, RMW_ERROR_STRING_SIZE - length_of_other_stuff);
      amount_written += RMW_ERROR_STRING_SIZE - length_of_other_stuff;
    }
  } else {
    // There is enough space for the whole message.
    strncpy(__rmw_error_string, error_string_, error_string_len);
  }
  // Write the first message fragment
  strncpy(__rmw_error_string + amount_written, __fragment_1, sizeof(__fragment_1));
  amount_written += sizeof(__fragment_1);
  if (length_of_other_stuff >= min_threshold) {
    // The file name will need to be shortened.
    auto suffix_len = 0;
    if (line_number_length != 0) {
      suffix_len += line_number_length + strlen(__fragment_2);
    }
    auto amount_to_write = RMW_ERROR_STRING_SIZE - min_threshold - suffix_len;
    strncpy(__rmw_error_string + amount_written, __truncate_str, sizeof(__truncate_str));
    amount_written += sizeof(__truncate_str);
    strncpy(__rmw_error_string + amount_written, file_, amount_to_write);
    amount_written += amount_to_write;
  } else {
    // Write all of the file name.
    strncpy(__rmw_error_string + amount_written, file_, file_len);
    amount_written += file_len;
  }
  // Attach the line number suffix, if needed.
  if (line_number_length > 0) {
    strncpy(__rmw_error_string + amount_written, __fragment_2, sizeof(__fragment_2));
    amount_written += sizeof(__fragment_2);
    strncpy(__rmw_error_string + amount_written, line_number_, line_number_length);
    amount_written += line_number_length;
  }
  // Terminate the string.
  __rmw_error_string[amount_written] = '\0';
#else  // RMW_AVOID_MEMORY_ALLOCATION
#if RMW_REPORT_ERROR_HANDLING_ERRORS
  if (rmw_error_is_set()) {
    fprintf(
      stderr, "[rmw|error_handling.c:%d] error string being overwritten: %s\n",
      __LINE__, rmw_get_error_string_safe());
  }
#endif
  const char * error_string_ = (error_string ? error_string : "");
  const char * file_ = (file ? file : "");
  size_t bytes_it_would_have_written = snprintf(
    NULL, 0, "%s%s%s%s%d", error_string_, __fragment_1, file_, __fragment_2, line_number);
  __rmw_error_string = (char *)rmw_allocate(bytes_it_would_have_written + 1);
#if RMW_REPORT_ERROR_HANDLING_ERRORS
  if (!__rmw_error_string) {
    // rmw_allocate failed, but fprintf might work?
    fprintf(
      stderr,
      "[rmw|error_handling.c:%d] failed to allocate memory for the error string\n",
      __LINE__);
  }
#endif
  if (!__rmw_error_string) {
    return;
  }
  snprintf(
    __rmw_error_string, bytes_it_would_have_written + 1,
    "%s%s%s%s%d", error_string_, __fragment_1, file_, __fragment_2, line_number);
#endif  // RMW_AVOID_MEMORY_ALLOCATION
}

const char *
rmw_get_error_string()
{
  return __rmw_error_string;
}

bool
rmw_error_is_set()
{
#if RMW_AVOID_MEMORY_ALLOCATION
  return __rmw_error_string[0] != '\0';
#else
  return __rmw_error_string != NULL;
#endif
}

const char *
rmw_get_error_string_safe()
{
  if (!rmw_error_is_set()) {
    return "error string not set";
  }
  return __rmw_error_string;
}

void
rmw_reset_error_string()
{
#if RMW_AVOID_MEMORY_ALLOCATION
  memset(__rmw_error_string, '\0', RMW_ERROR_STRING_SIZE);
#else
  if (__rmw_error_string) {
    rmw_free(__rmw_error_string);
  }
  __rmw_error_string = NULL;
#endif
}
