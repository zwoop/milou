/** @file

    Various (other) things to make milou look like Perl... Not sure why, but
    seemed like a good idea at the time.

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

#ifndef _milou_perl_H_
#define _milou_perl_H_

#include <boost/any.hpp>

typedef boost::any my;

using boost::any_cast;

// Also make sure to drag in the kitchen sink into the name space.
using namespace std;
using namespace boost::algorithm;

#endif // _milou_perl_H_
