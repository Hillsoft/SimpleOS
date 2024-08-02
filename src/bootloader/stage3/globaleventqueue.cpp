#include "globaleventqueue.hpp"

#include "mysty/circularBuffer.hpp"
#include "mysty/storage.hpp"

namespace simpleos {

namespace {

mysty::StorageFor<
    mysty::FixedCircularBuffer<mysty::unique_ptr<EventDispatcherImplBase>, 256>>
    eventQueue;

} // namespace

void initializeEventQueue() {
  eventQueue.emplace();
}

void scheduleEventImpl(mysty::unique_ptr<EventDispatcherImplBase> dispatcher) {
  eventQueue->emplace_back(mysty::move(dispatcher));
}

bool areEventsWaiting() {
  return eventQueue->size() > 0;
}

void dispatchNextEvent() {
  eventQueue->pop_front()->dispatch();
}

} // namespace simpleos
