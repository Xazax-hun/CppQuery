#ifndef __UTIL_H__
#define __UTIL_H__

template <typename T> class OwningPtr {
public:
  OwningPtr() : ptr(nullptr) {}
  OwningPtr(T *ptr) : ptr(ptr) {}
  ~OwningPtr() { delete ptr; }

  T *operator->() { return ptr; }

  operator bool() { return ptr; }

  void reset(T *newPtr) {
    delete ptr;
    ptr = newPtr;
  }

  T *get() { return ptr; }

private:
  T *ptr;
};

template <typename T> OwningPtr<T> makeOwningPtr(T *ptr) {
  return OwningPtr<T>(ptr);
}

#endif