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

#include <ares.h>
#include <netdb.h>

#include <functional>

#include <milou/array.h>
#include <milou/string.h>
#include <milou/pool.h>

namespace milou {
  namespace dns {

    // Class holding one response object (tightly integrated with the Request)
    struct DNSResponse {
      DNSResponse(milou::string::String& s, struct hostent * h)
        : mDomain(s), mHostent(h)
      { }

      milou::string::String& mDomain;
      struct hostent *mHostent;
    };

    typedef std::function<void (const DNSResponse& resp)> DNSCallback;

    // Main resolver object.
    class DNSResolver {
    public:
      // CTORs
      DNSResolver()
        : DNSResolver(10, NULL)
      { }

      DNSResolver(DNSCallback func)
        : DNSResolver(10, func)
      { }

      DNSResolver(int p, DNSCallback func)
        : _parallel(p), _callback(func), _reqs(0)
      {
        // ToDo: We should have an option class awrapper too
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

      // Some getter / setters.
      ares_channel channel() const { return _channel; }
      
      int parallel() const { return _parallel; }
      int parallel(int p) { return (_parallel = p); }

      milou::array::Strings& domains() { return _domains; }

      bool
      resolve(milou::string::String& s)
      {
        if (s.size() > 0) {
          _domains.push_back(s);
          return true;
        }
        return false;
      }

      void
      cancel(milou::string::String& s)
      {
        auto it = find(_domains.begin(), _domains.end(), s);

        if (it != _domains.end())
          _domains.erase(it);
      }
      
      void sort() { milou::array::sort(_domains); }
      void unique() { milou::array::unique(_domains); }

      bool process()
      {
        int nfds;
        fd_set readers, writers;
        struct timeval tv, *tvp;

        while (_domains.size() > 0 && _reqs < _parallel) {
          DNSRequest *req = _allocator.construct(this, _callback);

          ++_reqs;
          if (!req->lookupNext()) {  // Generally shouldn't happen...
            _allocator.destroy(req);
            break;
          }
        }

        // Now process whatever we have "pending".
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

    private:
      class DNSRequest {
      public:

        DNSRequest(DNSResolver *resolver, DNSCallback func)
          : _domain(""), _resolver(resolver), _function(func)
        { }

        ~DNSRequest() { --_resolver->_reqs; }

        bool
        lookupNext()
        {
          if (_resolver->_domains.size() > 0) {
            _domain = _resolver->_domains.back();
            _resolver->_domains.pop_back();
            ares_gethostbyname(_resolver->channel(), _domain.c_str(), AF_INET, (ares_host_callback)&_callback, this);
            return true;
          }

          _domain = milou::string::NULL_STRING;
          return false;
        }

        // DNSResponse mResponse;

      private:
        static void
        _callback(void *arg, int status, int timeouts, struct hostent *hostent)
        {
          DNSRequest *req = static_cast<DNSRequest*>(arg);
          DNSResponse resp(req->_domain, hostent);

          req->_function(resp);

          // Kick off more requests, if possible.
          if (!req->lookupNext())
            req->_resolver->_allocator.destroy(req);
        }

        milou::string::String _domain;
        DNSResolver *_resolver;
        DNSCallback _function;
      };

      ares_channel _channel;
      int _parallel;
      DNSCallback _callback;
      int _reqs;
      milou::array::Strings _domains;
      boost::object_pool<DNSRequest> _allocator;
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
