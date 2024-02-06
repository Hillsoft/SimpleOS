#pragma once

#include "mysty/typeattributes.hpp"

namespace mysty {

template <typename TIterator>
concept Iterator = requires(TIterator iterator, TIterator otherIterator) {
  typename TIterator::value_t;
  { *iterator } -> same_as<typename TIterator::value_t&>;
  { iterator == otherIterator } -> same_as<bool>;
  { iterator != otherIterator } -> same_as<bool>;
  ++iterator;
};

template <typename TIterable, typename TValue>
concept Iterable = requires(TIterable container) {
  typename TIterable::iterator_t;
  requires(Iterator<typename TIterable::iterator_t>);
  requires(same_as<TValue, typename TIterable::iterator_t::value_t>);
  { container.begin() } -> same_as<typename TIterable::iterator_t>;
  { container.end() } -> same_as<typename TIterable::iterator_t>;
};

template <typename TIterable, typename TValue>
concept ConstIterable = requires(TIterable const& container) {
  typename TIterable::const_iterator_t;
  requires(Iterator<typename TIterable::const_iterator_t>);
  requires(
      same_as<TValue const, typename TIterable::const_iterator_t::value_t>);
  { container.begin() } -> same_as<typename TIterable::const_iterator_t>;
  { container.end() } -> same_as<typename TIterable::const_iterator_t>;
};

} // namespace mysty
