#include "gap-buffer.h"
#include <cassert>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <string>

using std::string, std::stringstream, std::ostream;

template <typename T> GapBuffer<T>::GapBuffer() { Init(256); }

template <typename T> GapBuffer<T>::GapBuffer(int initial_capacity) {
  Init(initial_capacity);
}

template <typename T> void GapBuffer<T>::Init(int initial_capacity) {
  assert(initial_capacity >= 1);

  capacity = initial_capacity;
  buffer = new T[capacity];
  gap_start = 0;
  gap_end = capacity;
}

template <typename T> GapBuffer<T>::~GapBuffer() { delete[] buffer; }

template <typename T> int GapBuffer<T>::Size() {
  return capacity - gap_end + gap_start;
}

template <typename T> void GapBuffer<T>::MoveGap(int position) {
  assert(position >= 0);
  assert(gap_end != capacity);

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
  if (position < 0)
    return;

  if (position != gap_start - 1)
    MoveGap(position + 1);

  buffer[gap_start] = '\0';
  gap_start--;
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

template <typename T> void GapBuffer<T>::EnsureCapacity(int new_capacity) {
  T *new_buffer = new T[new_capacity];
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

template class GapBuffer<char>;
