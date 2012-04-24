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

#include <arpa/inet.h>

#include <ares.h>
#include <netdb.h>

#include <functional>

#include <milou/array.h>
#include <milou/string.h>
#include <milou/pool.h>
#include <milou/events.h>

static void
sock_callback(void *data, ares_socket_t socket_fd, int readable, int writable)
{
}


namespace milou {
  namespace dns {

    // Class holding one response object (tightly integrated with the Request)
    class DNSResponse {
    public:
      DNSResponse(milou::string::String& s, struct hostent * h)
        : mDomain(s), mHostent(h)
      { }

      // Return a given IP in the response (first by default)
      milou::string::String
      ip(int ix = 0) const
      {
        if (mHostent && mHostent->h_addr_list) {
          char **list = mHostent->h_addr_list;
          char ip[INET6_ADDRSTRLEN];

          while (*list && ix-- > 0)
            ++list;

          if (*list) {
            inet_ntop(mHostent->h_addrtype, *list, ip, sizeof(ip));
            return ip;
          }
        }

        return milou::string::NULL_STRING;
      }

      // Return a vector of all IPs, as strings.
      milou::array::Strings
      ips() const
      {
        milou::array::Strings str;

        if (mHostent && mHostent->h_addr_list) {
          char **list = mHostent->h_addr_list;
          char ip[INET6_ADDRSTRLEN];

          while (*list) {
            inet_ntop(mHostent->h_addrtype, *list, ip, sizeof(ip));
            ++list;
          }
          str.push_back(ip);
        }

        return str;
      }

      milou::string::String& mDomain;
      struct hostent *mHostent;
    };

    typedef std::function<void (const DNSResponse& resp)> DNSCallback;

    // Main resolver object.
    class DNSResolver {
    public:
      // CTOR
#if HAS_DELEGATING_CONSTRUCTOR
      DNSResolver()
        : DNSResolver(10, NULL)
      { }

      DNSResolver(DNSCallback func)
        : DNSResolver(10, func)
      { }

      DNSResolver(int p, DNSCallback func)
#else
      DNSResolver(int p=10, DNSCallback func=NULL)
#endif
        : _parallel(p), _callback(func), _reqs(0)

      {
        // ToDo: We should have an option class awrapper too
        struct ares_options options;

#if CARES_HAVE_ARES_LIBRARY_INIT
        ares_library_init(ARES_LIB_INIT_ALL);
#endif
        options.sock_state_cb = sock_callback;
        options.sock_state_cb_data = NULL;
        options.lookups = const_cast<char*>("b");

        ares_init_options(&_channel, &options, ARES_OPT_LOOKUPS|ARES_OPT_SOCK_STATE_CB);
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

      // Process all requests... This is a "main" event loop.
      // TODO: Once we have more asynchronous libraries, we should
      // unify this all into one event loop.
      void event_loop()
      {
        while (1) {
          if (!process())
            break;
        }
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
