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


// Standard stuff, we should include this with milou/milou.h
static const int MAX_DNS_REQUESTS = 100; // Max outstanding DNS requestsa

// One of these per resolver / state.
struct MyState {
  MyState()
  {
    struct ares_options options;

    options.lookups = const_cast<char*>("b");
#if CARES_HAVE_ARES_LIBRARY_INIT
    ares_library_init(ARES_LIB_INIT_ALL);
#endif
    ares_init_options(&mChannel, &options, ARES_OPT_LOOKUPS);
  }

  ~MyState()
  {
    ares_destroy(mChannel);
#if CARES_HAVE_ARES_LIBRARY_CLEANUP
    ares_library_cleanup();
#endif
  }

  // Members
  ares_channel mChannel;
  Strings mDomains;
};

struct Request {
  Request(MyState *state)
    : mDomain(""), mState(state)
  { }

  String mDomain;
  MyState *mState;

  bool
  lookupNext()
  {
    // Forward decl
    void caresCallback(void *arg, int status, int timeouts, struct hostent *hostent);

    if (size(mState->mDomains) > 0) {
      mDomain = mState->mDomains.back();
      mState->mDomains.pop_back();
      if (size(mDomain) > 0) {
        ares_gethostbyname(mState->mChannel, mDomain.c_str(), AF_INET, (ares_host_callback)&caresCallback, this);
        return true;
      }
    }
    return false;
  }

};


void
caresCallback(void *arg, int status, int timeouts, struct hostent *hostent)
{
  Request *req = static_cast<Request*>(arg);

  if (hostent) {
    char ip[INET6_ADDRSTRLEN];

    inet_ntop(hostent->h_addrtype, hostent->h_addr_list[0], ip, sizeof(ip));
    cout << "map http://" << req->mDomain << " http://" << ip << endl;
  } else {
    cerr << "Failed lookup: #" << req->mDomain << "#" << endl;
  }

  if (!req->lookupNext())
    delete req;
}


// Main entry point for this script. The fluff above is only to deal with a
// wonky asynchronous resolver (we probably should abstract that away ...)
int
main(int argc, char* argv[])
{
  MyState state;
  auto reqs = MAX_DNS_REQUESTS;

  // TODO: Collect / move this to some standard startup
  ios_base::sync_with_stdio(false);

  while (cin) {
    String line;

    getline(cin, line);
    chomp(line);
    trim(line);
    if (size(line) > 0)
      state.mDomains.push_back(line);
  }

  sort(state.mDomains);
  unique(state.mDomains); // Explicit sort is still required ... TODO ?

  // Kick off MAX_DNS_REQUESTS initially, and then start the event loop.
  while (reqs-- != 0) {
    Request *req = new Request(&state);

    if (!req->lookupNext())
      break;
  }

  while (1) {
    int nfds;
    fd_set readers, writers;
    struct timeval tv, *tvp;

    FD_ZERO(&readers);
    FD_ZERO(&writers);
    nfds = ares_fds(state.mChannel, &readers, &writers);
    if (nfds == 0)
      break;
    tvp = ares_timeout(state.mChannel, NULL, &tv);
    select(nfds, &readers, &writers, NULL, tvp);
    ares_process(state.mChannel, &readers, &writers);
  }
}
