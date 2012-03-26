// #!/bin/env milou -lcares
// Implies -std=c++0x with gcc

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

struct MyState {
  ares_channel mChannel;
  String mDomain;
  Strings *mDomains;
};

// Setup the state structure (for C-ARES etc.)
void
initialize(MyState &state)
{
  struct ares_options options;

  options.lookups = const_cast<char*>("b");
  ios_base::sync_with_stdio(false);
#if CARES_HAVE_ARES_LIBRARY_INIT
  ares_library_init(ARES_LIB_INIT_ALL);
#endif

  ares_init_options(&state.mChannel, &options, ARES_OPT_LOOKUPS);
}

// Cleanup C-ARES primarily.
void
cleanup(MyState &state)
{
  ares_destroy(state.mChannel);
#if CARES_HAVE_ARES_LIBRARY_CLEANUP
  ares_library_cleanup();
#endif
}

void
caresCallback(void *arg, int status, int timeouts, struct hostent *hostent)
{
  void doRequest(MyState *state); // Forward declaration.
  MyState *state = static_cast<MyState*>(arg);

  if (hostent) {
    char ip[INET6_ADDRSTRLEN];

    inet_ntop(hostent->h_addrtype, hostent->h_addr_list[0], ip, sizeof(ip));
    cout << "map http://" << state->mDomain << " http://" << ip << endl;
  } else {
    cerr << "Failed lookup: " << state->mDomain << endl;
  }
  doRequest(state);
  delete state; // These can only be cloned from the MasterState
}


void
doRequest(MyState *aState)
{
  if (aState->mDomains->size()) {
    MyState *state = new MyState(*aState);

    state->mDomain = state->mDomains->back();
    ares_gethostbyname(state->mChannel, state->mDomain.c_str(), AF_INET, (ares_host_callback)&caresCallback, state);
    state->mDomains->pop_back();
  }
}


int main(int argc, char* argv[])
{
  MyState masterState;
  Strings domains;    // TODO: Would be nice to e.g. do String mDomains[];  and make that generate the vector.
  auto reqs = MAX_DNS_REQUESTS;

  initialize(masterState);
  masterState.mDomains = &domains;

  while (cin) {
    String line;

    getline(cin, line);
    chomp(line);
    trim(line);
    if (line.size() > 0)
      domains.push_back(line);
  }

  // TODO: These needs easier versions, e.g. sort(domains);
  sort(domains.begin(), domains.end());
  domains.erase(unique(domains.begin(), domains.end()), domains.end());

  // Kick off MAX_DNS_REQUESTS initially, and then start the event loop.
  while (reqs-- != 0 && domains.size())
    doRequest(&masterState);

  while (1) {
    int nfds, count;
    fd_set readers, writers;
    struct timeval tv, *tvp;

    FD_ZERO(&readers);
    FD_ZERO(&writers);
    nfds = ares_fds(masterState.mChannel, &readers, &writers);
    if (nfds == 0)
      break;
    tvp = ares_timeout(masterState.mChannel, NULL, &tv);
    count = select(nfds, &readers, &writers, NULL, tvp);
    ares_process(masterState.mChannel, &readers, &writers);
  }

  cleanup(masterState);
}
