/** @file

    Various string related algorithms and extensions.

    @section license License

    Licensed to the Apache Software Foundation (ASF) under one or more
    contributor license agreements.  See the NOTICE file distributed with
    this work for additional information regarding copyright ownership.  The
    ASF licenses this file to you under the Apache License, Version 2.0 (the
    "License"); you may not use this file except in compliance with the
    License.  You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _milou_strings_H_
#define _milou_strings_H_

#include <string>
#include <boost/algorithm/string.hpp>

namespace milou;
namespace string;

typedef std::string String;

// Trim any trailing \r\n.
template<typename T>
inline void
chomp(T& input, const std::locale& loc=std::locale())
{
  boost::algorithm::trim_right_if(input, boost::is_any_of("\r\n"));
}

inline size_t
size(String& s)
{
  return s.size();
}

inline size_t
size(String *s)
{
  return s->size();
}

#endif // _milou_strings_H_
