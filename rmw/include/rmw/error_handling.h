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

RMW_PUBLIC
void
rmw_set_error_string(const char * error_string);

RMW_PUBLIC
const char *
rmw_get_error_string();

RMW_PUBLIC
const char *
rmw_get_error_string_safe();

#if __cplusplus
}
#endif

#endif  /* ROS_MIDDLEWARE_INTERFACE_ROS_MIDDLEWARE_INTERFACE_ERROR_HANDLING_H_ */
