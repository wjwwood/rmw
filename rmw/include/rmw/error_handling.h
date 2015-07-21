// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#ifndef ROS_MIDDLEWARE_INTERFACE_ROS_MIDDLEWARE_INTERFACE_ERROR_HANDLING_H_
#define ROS_MIDDLEWARE_INTERFACE_ROS_MIDDLEWARE_INTERFACE_ERROR_HANDLING_H_

#if __cplusplus
extern "C"
{
#endif

#include "macros.h"
#include "visibility_control.h"

typedef struct rmw_error_state_t {
  const char * message;
  const char * file;
  int line_number;
} rmw_error_state_t;

/// Sets the error message, the file it occurred in, and the line on which it occurred.
/**
 * This is not meant to be used directly, but instead via the RMW_SET_ERROR_MSG(msg) macro.
 */
RMW_PUBLIC
void
rmw_set_error_state(const char * error_msg, const char * file, int line_number);

/// Sets the error message, automatically appending the current file and line number.
/**
 * If an error message was previously set, and rmw_reset_error was not called
 * since, and this library was built with RMW_REPORT_ERROR_HANDLING_ERRORS set
 * on, then previously set error message will be printed to stderr.
 */
#define RMW_SET_ERROR_MSG(msg) rmw_set_error_state(msg, __FILE__, __LINE__);

/// Returns true if the error has been set and has not been reset since, otherwise false.
RMW_PUBLIC
bool
rmw_error_is_set();

/// Returns a struct with the error message and the file and line number on which is was set.
RMW_PUBLIC
const rmw_error_state_t *
rmw_get_error_state();

/// Returns a string with the error message followed by ', at <file>:<line>', or NULL if not set.
RMW_PUBLIC
const char *
rmw_get_error_string();

/// Returns a string with the error message followed by ', at <file>:<line>', or "" if not set.
/**
 * This function is guaranteed to return a valid c-string.
 */
RMW_PUBLIC
const char *
rmw_get_error_string_safe();

/// Resets the error state by clearing any previously set error state.
RMW_PUBLIC
void
rmw_reset_error();

#if __cplusplus
}
#endif

#endif  /* ROS_MIDDLEWARE_INTERFACE_ROS_MIDDLEWARE_INTERFACE_ERROR_HANDLING_H_ */
