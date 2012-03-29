// #!/bin/env milou -lcares
// Implies -std=c++0x with gcc

// /opt/gcc/bin/g++  -g -O3 -Wall -L/opt/gcc/lib64 -Wl,-rpath=/opt/gcc/lib64 -I ../include -std=c++11 -lcares genremap.cc

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

#include <ares.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <milou/milou.h> // This is the easiest "kitchen sink" include

// TODO: We should have a milou_main()
int
main(int argc, char* argv[])
{
  DNSResolver res(100);

  // TODO: Collect / move this to some standard startup?
  ios_base::sync_with_stdio(false);

  while (cin) {
    String line;

    getline(cin, line);
    chomp(line);
    trim(line);
    if (size(line) > 0)
      res.push(line);
  }

  res.sort();
  res.unique();

  auto func = [&] (const DNSResponse &response) {
    if (response.mHostent) {
      char ip[INET6_ADDRSTRLEN];

      inet_ntop(response.mHostent->h_addrtype, response.mHostent->h_addr_list[0], ip, sizeof(ip));
      std::cout << "map http://" << response.mDomain << " http://" << ip << std::endl;
    } else {
      std::cerr << "Failed lookup: #" << response.mDomain << "#" << std::endl;
    }
  };

  // Spin baby, spin!
  while (res.process(func))
    ;
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
