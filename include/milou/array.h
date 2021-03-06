/** @file

    Various array containers and algorithms.

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

#pragma once

#include <vector>
#include <milou/string.h>

namespace milou {
  namespace array {

    typedef std::vector<milou::string::String> Strings;

    // Chomp all strings in an array.
    Strings&
    chomp(Strings& arr)
    {
      for (auto s : arr)
        milou::string::chomp(s);
      return arr;
    }

    // Size-of...
    template <typename T>
    inline size_t
    size(std::vector<T>& v)
    {
      return v.size();
    }

    template <typename T>
    inline size_t
    size(std::vector<T> *v)
    {
      return v->size();
    }

    // Sort an array, optionally with a sort lambda
    template <typename T>
    inline void
    sort(std::vector<T>& v)
    {
      sort(v.begin(), v.end());
    }

    template <typename T>
    inline void
    sort(std::vector<T> *v)
    {
      sort(v->begin(), v->end());
    }

    // Unique an array, this requires the array to be sorted already.
    template <typename T>
    inline void
    unique(std::vector<T>& v)
    {
      v.erase(unique(v.begin(), v.end()), v.end());
    }

    template <typename T>
    inline void
    unique(std::vector<T> *v)
    {
      v->erase(unique(v->begin(), v->end()), v->end());
    }

  } // namespace array
} // namespace milou


/*
 local variables:
 mode: C++
 indent-tabs-mode: nil
 c-basic-offset: 2
 c-comment-only-line-offset: 0
 c-file-offsets: ((statement-block-intro . +)
                  (label . 0)
                  (statement-cont . +)
                  (innamespace . 0))
 end:
*/
