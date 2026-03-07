#ifndef CEDITOR_GAPBUFFER_H_
#define CEDITOR_GAPBUFFER_H_

#include "editable-string.h"
#include <cassert>
#include <cstdlib>
#include <exception>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

using std::string, std::stringstream, std::ostream;

template <typename T> class GapBuffer : public EditableString<T> {
private:
  T *buffer;
  int capacity, gap_start, gap_end;
  void Init(int initial_capacity);

public:
  struct Iterator {
    using iterator_category = random_access_iterator_tag;
    using difference_type = int;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    Iterator(GapBuffer<value_type> *gap_buffer) {
      this->pos = 0;
      this->gap_buffer = gap_buffer;
    }

    Iterator(GapBuffer<value_type> *gap_buffer, unsigned int pos) {
      this->pos = pos;
      this->gap_buffer = gap_buffer;

      if (pos > gap_buffer->Size()) {
        throw out_of_range("can't make ptr at pos");
      }
    }

    reference operator*() {
      if (pos >= gap_buffer->gap_start) {
        return *(gap_buffer->buffer + pos - gap_buffer->gap_start +
                 gap_buffer->gap_end);
      }

      return *(gap_buffer->buffer + pos);
    }

    pointer operator->() {
      if (pos >= gap_buffer->gap_start) {
        return gap_buffer->buffer + pos - gap_buffer->gap_start +
               gap_buffer->gap_end;
      }

      return gap_buffer->buffer + pos;
    }

    Iterator &operator++() {
      if (pos >= gap_buffer->Size()) {
        // TODO: Determine desired behavior (out_of_range) or other
        return *this;
      }

      this->pos++;
      return *this;
    }
    Iterator &operator++(int) {
      if (pos >= gap_buffer->Size()) {
        // TODO: Determine desired behavior (out_of_range) or other
        return *this;
      }

      this->pos++;
      return *this;
    }

    Iterator &operator--() {
      if (pos <= 0) {
        return *this;
      }

      this->pos--;
      return *this;
    }

    bool operator==(Iterator &other) {
      return pos = other.pos && gap_buffer == other.gap_buffer;
    }
    bool operator!=(Iterator &other) {
      return pos != other.pos || gap_buffer != other.gap_buffer;
    }

    difference_type operator-(Iterator &other) { return this->pos - other.pos; }

    Iterator &operator-(difference_type diff) {
      return GapBuffer<T>::Iterator(gap_buffer, max(0, (int)(pos - diff)));
    }

    Iterator &operator+(difference_type diff) {
      return GapBuffer<T>::Iterator(gap_buffer,
                                    min(gap_buffer->Size(), pos + diff));
    }

  protected:
    unsigned int pos;   // position in string
    unsigned int index; // index in buffer
    GapBuffer<T> *gap_buffer;
  };

  GapBuffer();
  GapBuffer(int initial_capacity);
  void EnsureCapacity(int new_capacity);
  void MoveGap(int position);
  ~GapBuffer();

  // EditableString methods
  int Size() override;
  T CharAt(int position) override;

  void Insert(int position, T c) override;
  void Del(int position) override;
  void Clear() override;

  T *ToArr() override;
  string ToString() override;
  string ToDebug() override;

  Iterator &begin();
  Iterator &end();
};

template <typename T> GapBuffer<T>::GapBuffer() { Init(256); }

template <typename T> GapBuffer<T>::GapBuffer(int initial_capacity) {
  Init(initial_capacity);
}

template <typename T> void GapBuffer<T>::Init(int initial_capacity) {
  assert(initial_capacity >= 1);

  capacity = initial_capacity;
  buffer = new T[capacity]{};
  gap_start = 0;
  gap_end = capacity;
}

template <typename T> GapBuffer<T>::~GapBuffer() { delete[] buffer; }

template <typename T> int GapBuffer<T>::Size() {
  return capacity - gap_end + gap_start;
}

template <typename T> void GapBuffer<T>::MoveGap(int position) {
  assert(position >= 0);

  while (gap_start > position) {
    gap_start--;
    gap_end--;

    buffer[gap_end] = buffer[gap_start];
    buffer[gap_start] = '\0';
  }

  while (gap_start < position) {
    buffer[gap_start] = buffer[gap_end];
    buffer[gap_end] = '\0';

    gap_start++;
    gap_end++;
  }
}

template <typename T> void GapBuffer<T>::Del(int position) {
  if (position < 0 || position > Size())
    throw out_of_range("Cannot delete index " + to_string(position));

  if (position != gap_start - 1)
    MoveGap(position + 1);

  gap_start--;
  buffer[gap_start] = '\0';
}

template <typename T> void GapBuffer<T>::Insert(int position, T c) {
  if (gap_end - gap_start <= 1)
    EnsureCapacity(capacity * 2);

  if (position != gap_start)
    MoveGap(position);

  buffer[gap_start] = c;
  gap_start++;
}

template <typename T> T GapBuffer<T>::CharAt(int position) {
  if (position < gap_start)
    return buffer[position];

  return buffer[position - gap_start + gap_end];
}

template <typename T> void GapBuffer<T>::Clear() {
  int i;
  for (i = 0; i <= gap_start; i++) {
    buffer[i] = '\0';
  }

  for (i = gap_end; i < capacity; i++) {
    buffer[i] = '\0';
  }

  gap_start = 0;
  gap_end = capacity;
}

template <typename T> string GapBuffer<T>::ToString() {
  stringstream stream{};
  int i;

  for (i = 0; i < gap_start; i++) {
    stream << buffer[i];
  }

  for (i = gap_end; i < capacity; i++) {
    stream << buffer[i];
  }

  return stream.str();
}

template <typename T> T *GapBuffer<T>::ToArr() {
  T *arr = new T[Size()];
  int i;

  for (i = 0; i < gap_start; i++) {
    arr[i] = buffer[i];
  }

  for (i = gap_end; i < capacity; i++) {
    arr[i] = buffer[i];
  }

  return arr;
}

template <typename T> string GapBuffer<T>::ToDebug() {
  using namespace std;
  stringstream stream{};
  int i;

  stream << "[ ";
  if (capacity > 0) {
    stream << buffer[0];
  }

  for (i = 1; i < capacity; i++) {
    stream << ", " << buffer[i];
  }

  stream << "]" << endl;

  return stream.str();
}

template <typename T> void GapBuffer<T>::EnsureCapacity(int new_capacity) {
  T *new_buffer = new T[new_capacity]{};
  int i = 0;

  for (; i < gap_start; i++) {
    new_buffer[i] = buffer[i];
  }
  int end_size = capacity - gap_end;
  for (i = 1; i <= end_size; i++) {
    new_buffer[new_capacity - i] = buffer[capacity - i];
  }

  delete[] buffer;
  buffer = new_buffer;
  gap_end = new_capacity - end_size;
  capacity = new_capacity;
}

template <typename T> typename GapBuffer<T>::Iterator &GapBuffer<T>::begin() {
  return *(new Iterator(this));
}

template <typename T> typename GapBuffer<T>::Iterator &GapBuffer<T>::end() {
  return *(new Iterator(this, this->Size()));
}

#endif // CEDITOR_GAPBUFFER_H_
