#pragma once

#include <detail/macros.hpp>

#include <cstdint>
#include <vector>

namespace flov {
namespace detail {

template <class _T, uint8_t _Size>
class StaticBuffer {
 public:
  using size_type = uint8_t;

  void PushBack(_T link) {
    FLOV_ASSERT(last < _Size);
    buffer[last++] = std::move(link);
  }

  _T& operator[](size_type index) {
    FLOV_ASSERT(index < _Size);
    return buffer[index];
  }

  const _T& operator[](size_type index) const {
    FLOV_ASSERT(index < _Size);
    return buffer[index];
  }

  size_type Size() const {
    return last;
  }

  const _T* Begin() const {
    return buffer;
  }
  const _T* End() const {
    return buffer + last;
  }

 private:
  size_type last = 0;
  _T buffer[_Size];
};

template <class _T, uint8_t _Size>
class VectorWithStaticBuffer {
 public:
  using size_type = typename std::vector<_T>::size_type;

  void PushBack(_T link) {
    buffer.Size() < _Size ? buffer.PushBack(std::move(link))
                          : vector.push_back(std::move(link));
  }

  _T& operator[](size_type index) {
    return index < _Size ? buffer[index] : vector[index - _Size];
  }

  const _T& operator[](size_type index) const {
    return index < _Size ? buffer[index] : vector[index - _Size];
  }

  size_type Size() const {
    return buffer.Size() + vector.size();
  }

  // TODO: impl correctly or remove
  const _T* Begin() const {
    return buffer.Begin();
  }
  const _T* End() const {
    return buffer.End();
  }

 private:
  StaticBuffer<_T, _Size> buffer;
  std::vector<_T> vector;
};

}  // namespace detail
}  // namespace flov