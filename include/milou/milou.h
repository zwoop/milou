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

#include <iostream>
#include <algorithm>
#include <vector>

#include <milou/lulu.h>
#include <milou/string.h>
#include <milou/array.h>
#include <milou/hash.h>
#include <milou/perl.h>
#include <milou/dns.h>

// Also make sure to drag in the kitchen sink into the name space.
using namespace std;
using namespace boost::algorithm;

using namespace milou::string;
using namespace milou::array;
using namespace milou::hash;
using namespace milou::events;
using namespace milou::perl;
using namespace milou::dns;


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
