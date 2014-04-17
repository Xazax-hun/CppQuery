#ifndef __UTIL_H__
#define __UTIL_H__

/// \brief Smart pointer that owns it's resource.
///
/// This is a small RAII smart pointer to tie the lifetime of an object to
/// another one. When a unique pointer is not the right choice, because some
/// objects may copy this pointer but will not get the ownership, the owning
/// pointer might be the right choice.
template <typename T> class OwningPtr {
public:
  OwningPtr() : ptr(nullptr) {}
  OwningPtr(T *ptr) : ptr(ptr) {}
  ~OwningPtr() { delete ptr; }

  /// Make the interface of the pointer accessible.
  T *operator->() { return ptr; }
  const T *operator->() const { return ptr; }

  /// Make it possible to use the pointer as boolean expression
  operator bool() { return ptr; }

  /// Delete the old pointer and set this object to hold a new one.
  void reset(T *newPtr) {
    delete ptr;
    ptr = newPtr;
  }

  /// Get back the raw pointer
  T *get() { return ptr; }

private:
  T *ptr;
};


/// Convenient function to create owning pointers.
template <typename T> OwningPtr<T> makeOwningPtr(T *ptr) {
  return OwningPtr<T>(ptr);
}

#endif