// #!/bin/env milou -lcares
// Implies -std=c++0x with gcc

// /opt/gcc/bin/g++  -g -O3 -pedantic -Wall -L/opt/gcc/lib64 -Wl,-rpath=/opt/gcc/lib64 -I ../include -std=c++11 -lcares genremap.cc

/** @file

    Read domain names (one per line) from either stdin, or a provided file,
    do a DNS lookup, and generate an ATS remap.config file. Kinda useless for
    most, but something I needed.

    @section license License

    Licensed to the Apache Software Foundation (ASF) under one
    or more contributor license agreements.  See the NOTICE file
    distributed with this work for additional information
    regarding copyright ownership.  The ASF licenses this file
    to you under the Apache License, Version 2.0 (the
    "License"); you may not use this file except in compliance
    with the License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

// This is the easiest "kitchen sink" include
#include <milou/milou.h>


// This gets called everytime we get a response from the DNS processor.
// It could also be a lambda, or functor.
void
callback(const DNSResponse &response)
{
#if 0
  String ip = response.ip();

  if (size(ip) > 0)
    cout << "map http://" << response.mDomain << " http://" << ip << endl;
  else
    cerr << "Failed lookup: " << response.mDomain << endl;
#endif
}

int
main(int argc, char* argv[])
{
  DNSResolver res(100, callback);
  EventLoop loop;

  // TODO: Collect / move this to some standard startup?
  ios_base::sync_with_stdio(false);
  cout << nounitbuf;

  // Read all the STDIN lines, and add them to be resolved.
  while (cin) {
    String line;

    getline(cin, line);
    chomp(line);
    trim(line);
    res.queue(line);
  }

  // Sort and unique all the resolver requests.
  res.sort();
  res.unique();

  //loop.loop(res);
  res.event_loop();
}


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
