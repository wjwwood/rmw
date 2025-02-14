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

#ifndef RMW_RMW_IMPL_CPP_MACROS_HPP_
#define RMW_RMW_IMPL_CPP_MACROS_HPP_

#include <iostream>
#include <sstream>
#include <string>

#include <rmw/error_handling.h>
#include <rmw/impl/config.h>  // For RMW_AVOID_MEMORY_ALLOCATION
#include <rmw/impl/cpp/demangle.hpp>  // For demangle.

// *INDENT-OFF* (prevent uncrustify from using four space indention here)
#define RMW_TRY_PLACEMENT_NEW(Destination, BufferForNew, FailureAction, Type, ...) try { \
  Destination = new(BufferForNew) Type(__VA_ARGS__); \
} catch(const std::exception & exception) { \
  rmw_set_error_string(( \
    std::string("caught C++ exception ") + rmw::impl::cpp::demangle(exception) + \
    " constructing " #Type ": " + exception.what() \
  ).c_str()); \
  FailureAction; \
} catch(...) { \
  rmw_set_error_string("caught unknown C++ exception constructing " #Type); \
  FailureAction; \
}

#define RMW_TRY_DESTRUCTOR(Statement, Type, FailureAction) try { \
  Statement; \
} catch(const std::exception & exception) { \
  rmw_set_error_string(( \
    std::string("caught C++ exception in destructor of " #Type ": ") + \
    rmw::impl::cpp::demangle(exception) + ": " + exception.what() \
  ).c_str()); \
  FailureAction; \
} catch(...) { \
  rmw_set_error_string("caught unknown C++ exception in destructor of " #Type); \
  FailureAction; \
}

#define RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(Statement, Type) try { \
  Statement; \
} catch(const std::exception & exception) { \
  std::stringstream ss; \
  ss << "caught C++ exception in destructor of " #Type " while handling a failure: " \
     << rmw::impl::cpp::demangle(exception) << ": " << exception.what() \
     << ", at: " << __FILE__ << ":" << __LINE__ << '\n'; \
  (std::cerr << ss.str()).flush(); \
} catch(...) { \
  std::stringstream ss; \
  ss << "caught unknown C++ exception in destructor of " #Type \
     << " while handling a failure at: " << __FILE__ << ":" << __LINE__ << '\n'; \
  (std::cerr << ss.str()).flush(); \
}

#if RMW_AVOID_MEMORY_ALLOCATION
#define RMW_CHECK_TYPE_IDENTIFIERS_MATCH(ElementName, ElementTypeID, ExpectedTypeID, OnFailure) { \
  if (ElementTypeID != ExpectedTypeID) { \
    char __msg[1024]; \
    snprintf( \
      __msg, 1024, \
      #ElementName " implementation '%s'(%p) does not match rmw implementation '%s'(%p)", \
      ElementTypeID, ElementTypeID, ExpectedTypeID, ExpectedTypeID); \
    rmw_set_error_string(__msg); \
    OnFailure; \
  } \
}
#else  // RMW_AVOID_MEMORY_ALLOCATION
#define RMW_CHECK_TYPE_IDENTIFIERS_MATCH(ElementName, ElementTypeID, ExpectedTypeID, OnFailure) { \
  if (ElementTypeID != ExpectedTypeID) { \
    size_t __bytes_that_would_have_been_written = snprintf( \
      NULL, 0, \
      #ElementName " implementation '%s'(%p) does not match rmw implementation '%s'(%p)", \
      ElementTypeID, ElementTypeID, ExpectedTypeID, ExpectedTypeID); \
    char * __msg = (char *)rmw_allocate(__bytes_that_would_have_been_written + 1); \
    snprintf( \
      __msg, __bytes_that_would_have_been_written + 1, \
      #ElementName " implementation '%s'(%p) does not match rmw implementation '%s'(%p)", \
      ElementTypeID, ElementTypeID, ExpectedTypeID, ExpectedTypeID); \
    rmw_set_error_string(__msg); \
    rmw_free(__msg); \
    OnFailure; \
  } \
}
#endif  // RMW_AVOID_MEMORY_ALLOCATION
// *INDENT-ON*

#endif  // RMW_RMW_IMPL_CPP_MACROS_HPP_
