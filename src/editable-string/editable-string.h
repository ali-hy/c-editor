#ifndef CEDITOR_EDITABLESTRING_H_
#define CEDITOR_EDITABLESTRING_H_
#include <iterator>
#include <ostream>
#include <string>

using namespace std;

template <typename T> class EditableString {
public:
  // Getters
  virtual int Size() = 0;
  virtual T CharAt(int position) = 0;

  // Modifiers
  virtual void Insert(int index, T character) = 0;
  virtual void Del(int index) = 0;
  virtual void Clear() = 0;

  // Conversions
  virtual T *ToArr() = 0;
  virtual string ToString() = 0;
  virtual string ToDebug() = 0;

  virtual ~EditableString() = default;
};

#endif // CEDITOR_EDITABLESTRING_H_
