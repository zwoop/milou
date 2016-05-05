/** @file

    This a simple event processing handler, and also the base class for
    all asynchronous event processing interfaces.

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

#include <libev/ev.h>
#include <vector>

namespace milou {
  namespace events {

    // Base class for all event handling classes.
    class EventLoop;
    class EventHandler {
    public:
      EventHandler()
        : _started(false)
      { }

      virtual void start(EventLoop *loop) { _started = true;}

    private:
      bool _started;
    };

    // TODO: Should this store a smart ptr instead?
    typedef std::vector<EventHandler> EventHandlers;

    class EventLoop {
    public:
      EventLoop()
        : _loop(EV_DEFAULT)
      { }

      void add(EventHandler &h, bool start=true) {
        if (start) {
          h.start(this);
          _started.push_back(h);
        } else {
          _pending.push_back(h);
        }
      }

      void loop() {
        // Make sure all handlers have started before we go into the event loop.
        for (auto h : _pending) {
          h.start(this);
          _started.push_back(h); // TODO: Figure out how to move between containers without copy semantics.
        }
      }

      // Convenient, for the case when there is only one EventHandler, we can
      // add it, and start the event loop all in one call.
      void
      loop(EventHandler &h) {
        h.start(this);
        _started.push_back(h);
        loop();
      }

    private:
      EventHandlers _pending;
      EventHandlers _started;
      struct ev_loop *_loop;
    };

  } // namespace events
} // namespace milou
