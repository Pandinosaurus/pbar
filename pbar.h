#ifndef __PBAR_H
#define __PBAR_H

#include <iostream>
#include <iterator>

namespace pbar {

template<class It>
class ProgressBar {
public:
  ProgressBar(It it, It it_end, size_t width, const char symbol='=')
      : count_(0),
        width_(width), 
        size_(std::distance(it, it_end)),
        symbol_(symbol), 
        it_begin_(*this, std::move(it)),
        it_end_(*this, std::move(it_end))
  {}

  struct iterator;

  iterator begin() const {
    return it_begin_;
  }

  iterator end() const {
    return it_end_;
  }

private:
  void notify();

  size_t count_;
  size_t width_;
  size_t size_;

  char symbol_;
  char left_delim_{'['};
  char right_delim_{']'};
  char pointer_{'>'};

  iterator it_begin_;
  iterator it_end_;

}; // class ProgressBar

template<class It>
struct ProgressBar<It>::iterator {

  using iterator_category = std::forward_iterator_tag;
  using value_type = typename It::value_type;
  using difference_type = typename It::difference_type;
  using pointer = typename It::pointer;
  using reference = typename It::reference;

public:
  iterator(ProgressBar<It>& parent, It iter) 
    : parent_(parent), iter_(std::move(iter)) {}

  iterator& operator++();
  iterator operator++(int);

  bool operator==(const iterator& other) const;
  bool operator!=(const iterator& other) const;

  reference operator*() const;

private:
  ProgressBar<It>& parent_;
  It iter_;

};

template<class It>
inline bool ProgressBar<It>::iterator::operator==(
    const ProgressBar<It>::iterator& other) const {
  return iter_ == other.iter_;
}

template<class It>
inline bool ProgressBar<It>::iterator::operator!=(
    const ProgressBar<It>::iterator& other) const {
  return !(*this == other);
}

template<class It>
inline typename It::reference ProgressBar<It>::iterator::operator*() const {
  return *iter_;
}

template<class It>
inline typename ProgressBar<It>::iterator& ProgressBar<It>::iterator::operator++() {
  ++(iter_); 
  ++parent_.count_;
  parent_.notify();
  return *this; 
}

template<class It>
inline typename ProgressBar<It>::iterator ProgressBar<It>::iterator::operator++(int) {
  auto retval = *this;
  ++(*this);
  return retval;
}

template<class It>
inline void ProgressBar<It>::notify() {
  size_t pos = width_*count_/size_;
  std::clog << left_delim_;
  for (int i=0; i<width_; i++) {
    if (i < pos)
      std::clog << symbol_;
    else if (i == pos)
      std::clog << pointer_;
    else
      std::clog << " ";
  }
  std::clog << right_delim_ << count_*100/size_ << "%\r";
  std::clog.flush();
}

}; // namespace pbar

#endif // __PBAR_H

