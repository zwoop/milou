/** @file

    Easy to use, asynchronous DNS resolver.

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

#include <milou/array.h>

namespace milou {
  namespace dns {

    class DNSResolver {
    public:
      // CTOR
      DNSResolver()
      {
        struct ares_options options;

        options.lookups = const_cast<char*>("b");
#if CARES_HAVE_ARES_LIBRARY_INIT
        ares_library_init(ARES_LIB_INIT_ALL);
#endif
        ares_init_options(&_channel, &options, ARES_OPT_LOOKUPS);
      }

      // DTOR
      ~DNSResolver()
      {
        ares_destroy(_channel);
#if CARES_HAVE_ARES_LIBRARY_CLEANUP
        ares_library_cleanup();
#endif
      }

      // Main processing point, call this one you have populated a domain or
      // two in the resolver.
      bool
      process()
      {
        int nfds;
        fd_set readers, writers;
        struct timeval tv, *tvp;

        FD_ZERO(&readers);
        FD_ZERO(&writers);
        nfds = ares_fds(_channel, &readers, &writers);
        if (nfds == 0)
          return false;

        tvp = ares_timeout(_channel, NULL, &tv);
        select(nfds, &readers, &writers, NULL, tvp);
        ares_process(_channel, &readers, &writers);

        return true;
      }

      ares_channel channel() const { return _channel; }

      // Members
      milou::array::Strings mDomains;

    private:
      ares_channel _channel;
    };

  } // namespace dns
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
