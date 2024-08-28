#pragma once

#include "mysty/smart_pointer.hpp"
#include "mysty/utility.hpp"

namespace simpleos {

void initializeEventQueue();

template <typename TEvent>
class EventHandler {
 public:
  virtual ~EventHandler() = default;

  virtual void handleEvent(TEvent event) {}
};

template <typename TEvent>
EventHandler<TEvent>*& getHandler() {
  static EventHandler<TEvent>* handler = nullptr;
  return handler;
}

template <typename TEvent>
void registerHandler(EventHandler<TEvent>* handler) {
  getHandler<TEvent>() = handler;
}

class EventDispatcherImplBase {
 public:
  virtual ~EventDispatcherImplBase() = default;

  virtual void dispatch() && {}
};

template <typename TEvent>
class EventDispatcherImpl : public EventDispatcherImplBase {
 public:
  EventDispatcherImpl(TEvent event) : event_(mysty::move(event)) {}

  virtual ~EventDispatcherImpl() = default;

  void dispatch() && override {
    EventHandler<TEvent>* handler = getHandler<TEvent>();
    if (handler != nullptr) {
      handler->handleEvent(mysty::move(event_));
    }
  }

 private:
  TEvent event_;
};

void scheduleEventImpl(mysty::unique_ptr<EventDispatcherImplBase> dispatcher);

template <typename TEvent>
void scheduleEvent(TEvent event) {
  scheduleEventImpl(
      mysty::make_unique<EventDispatcherImpl<TEvent>>(mysty::move(event)));
}

bool areEventsWaiting();

void dispatchNextEvent();

} // namespace simpleos
