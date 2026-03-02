#ifndef CEDITOR_GAPBUFFER_H_
#define CEDITOR_GAPBUFFER_H_

#include "editable-string.h"
#include <string>

template <typename T> class GapBuffer : public EditableString<T> {
private:
  T *buffer;
  int capacity, gap_start, gap_end;
  void Init(int initial_capacity);
  void MoveGap(int position);

public:
  GapBuffer();
  GapBuffer(int initial_capacity);
  void EnsureCapacity(int new_capacity);
  ~GapBuffer();

  // EditableString methods
  int Size() override;
  T CharAt(int position) override;

  void Insert(int position, T c) override;
  void Del(int position) override;
  void Clear() override;

  T *ToArr() override;
  string ToString() override;
};

#endif // CEDITOR_GAPBUFFER_H_
