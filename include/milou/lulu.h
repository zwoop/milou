/** @file

    This is a convenience include file, intended for the typical user to
    include. This does pull in the kitchen sink, but for a "script" it is
    fine.

    This focusing on the fact that the "scripts" should be as short and
    quick to type as possible.

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

#if defined(__has_feature)
# define HAS_FEATURE(x) __has_feature(x)
#else
# define HAS_FEATURE(x) 0
#endif

#define HAS_DELEGATING_CONSTRUCTOR HAS_FEATURE(cxx_delegating_constructors) ||\
                                   defined(__GNUC__) && (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 7)



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
