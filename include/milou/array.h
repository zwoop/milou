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

#ifndef _milou_array_H_
#define _milou_array_H_

#include <vector>
#include <milou/strings.h>

typedef std::vector<String> Strings;

// Chomp all strings in an array.
Strings&
chomp(Strings& arr)
{
  for (auto s : arr)
    chomp(s);
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

#endif // _milou_array_H_
