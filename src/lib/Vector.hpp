#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "../kernel/MemoryManager/Kmalloc.hpp"
#include "string.hpp"
#include <stdint.h>

template <typename T> class Vector {
public:
  T *data;
  uint64_t _size;
  uint64_t _capacity;

  Vector();
  ~Vector();

  void push_back(T value);
  void Insert(uint64_t index, T value);
  void Remove(uint64_t index);
  T get(uint64_t index);
  uint64_t size();

  // Helper to allow vector[i] syntax
  T &operator[](uint64_t index) { return data[index]; }
};

// --- IMPLEMENTATIONS ---

template <typename T> Vector<T>::Vector() {
  _size = 0;
  _capacity = 4;
  data = (T *)kmalloc(_capacity * sizeof(T));
  // Construct each element with the default constructor
  for (uint64_t i = 0; i < _capacity; i++) {
    new (&data[i]) T();
  }
}

template <typename T> Vector<T>::~Vector() {
  for (uint64_t i = 0; i < _size; i++) {
    data[i].~T();
  }
  if (data)
    kfree(data);
}

// Mark this inline so the linker doesn't complain about multiple copies
template <typename T> inline void Grow(Vector<T> *vector) {
  uint64_t new_cap = vector->_capacity * 2;
  T *new_data = (T *)kmalloc(new_cap * sizeof(T));
  // Zero-initialize so String::operator= doesn't try to delete[] garbage
  for (uint64_t i = 0; i < new_cap * sizeof(T); i++) {
    ((uint8_t *)new_data)[i] = 0;
  }
  for (uint64_t i = 0; i < vector->_size; i++) {
    new_data[i] = vector->data[i];
  }
  // Destroy the old elements
  for (uint64_t i = 0; i < vector->_size; i++) {
    vector->data[i].~T();
  }
  if (vector->data)
    kfree(vector->data);
  vector->_capacity = new_cap;
  vector->data = new_data;
}

template <typename T> void Vector<T>::push_back(T value) {
  if (_size >= _capacity) {
    Grow(this);
  }
  this->data[_size] = value;
  _size++;
}

template <typename T> uint64_t Vector<T>::size() { return _size; }

template <typename T> T Vector<T>::get(uint64_t index) {
  if (index >= _size)
    return T(); // Return "empty" object if out of bounds
  return data[index];
}

// Even if you don't use these yet, the linker needs them if they are declared
template <typename T> void Vector<T>::Insert(uint64_t index, T value) {
  if (index > _size)
    return;
  if (_size >= _capacity)
    Grow(this);
  for (uint64_t i = _size; i > index; i--) {
    data[i] = data[i - 1];
  }
  data[index] = value;
  _size++;
}

template <typename T> void Vector<T>::Remove(uint64_t index) {
  if (index >= _size)
    return;
  for (uint64_t i = index; i < _size - 1; i++) {
    data[i] = data[i + 1];
  }
  _size--;
}

#endif