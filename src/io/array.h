#include <algorithm>
#include <cstdint>
#include <memory>

#ifndef ARRAY__H
#define ARRAY__H

namespace tomocam {

    struct dims_t {
        uint32_t n0;
        uint32_t n1;
        uint32_t n2;
    };

    template <typename T>
    struct Slice {
        uint32_t nrows;
        uint32_t ncols;
        T *ptr;
        T &operator[](uint32_t i) { return ptr[i]; }
        const T &operator[](uint32_t i) const { return ptr[i]; }
    };

    template <typename T>
    class Array {
      private:
        dims_t dims_;
        uint32_t size_;
        std::unique_ptr<T[]> ptr_;

      public:
        Array() : dims_(0, 0, 0), size_(0), ptr_(nullptr) {}

        Array(uint32_t x, uint32_t y, uint32_t z) :
            dims_{x, y, z},
            size_(x * y * z),
            ptr_(std::make_unique<T[]>(size_)) {}

        Array(dims_t d) :
            dims_(d),
            size_(d.n1 * d.n2 * d.n0),
            ptr_(std::make_unique<T[]>(size_)) {}

        Array(const Array &rhs) {
            dims_ = rhs.dims_;
            size_ = rhs.size_;
            ptr_ = std::make_unique<T[]>(size_);
            std::copy(rhs.begin(), rhs.end(), ptr_.get());
        }

        Array &operator=(const Array &rhs) {
            if (this != &rhs) {
                dims_ = rhs.dims_;
                size_ = rhs.size_;
                ptr_.reset();
                ptr_ = std::make_unique<T[]>(size_);
                std::copy(rhs.begin(), rhs.end(), ptr_.get());
            }
            return *this;
        }

        uint32_t flatIdx(uint32_t i, uint32_t j, uint32_t k) const {
            return (i * dims_.n1 * dims_.n2 + j * dims_.n2 + k);
        }

        T *begin() { return ptr_.get(); }
        const T *begin() const { return ptr_.get(); }
        T *end() { return ptr_.get() + size_; }
        const T *end() const { return ptr_.get() + size_; }

        [[nodiscard]] dims_t dims() const { return dims_; }
        [[nodiscard]] uint32_t size() const { return size_; }
        [[nodiscard]] uint32_t nslices() const { return dims_.n0; }
        [[nodiscard]] uint32_t nrows() const { return dims_.n1; }
        [[nodiscard]] uint32_t ncols() const { return dims_.n2; }

        // indexing
        T &operator[](uint32_t i) { return ptr_[i]; }
        T operator[](uint32_t i) const { return ptr_[i]; }

#if (__cplusplus == 202302L)
        T &operator[](uint32_t i, uint32_t j, uint32_t k) {
            return ptr_[flatIdx(i, j, k)];
        }
        T operator[](uint32_t i, uint32_t j, uint32_t k) const {
            return ptr_[flatIdx(i, j, k)];
        }
#endif

        T &operator[](dims_t i) { return ptr_[flatIdx(i.n0, i.n1, i.n2)]; }
        const T &operator[](dims_t i) const {
            return ptr_[flatIdx(i.n0, i.n1, i.n2)];
        }

        // get slices
        auto slice(uint32_t i) {
            return Slice<T>{dims_.n1, dims_.n2,
                ptr_.get() + (i * dims_.n1 * dims_.n2)};
        }
        auto slice(uint32_t i) const {
            return Slice<T>{dims_.n1, dims_.n2,
                ptr_.get() + (i * dims_.n1 * dims_.n2)};
        }

        T min() const {
            auto min_it = std::min_element(this->begin(), this->end());
            return *min_it;
        }

        T max() const {
            auto max_it = std::max_element(this->begin(), this->end());
            return *max_it;
        }
    };
} // namespace tomocam
#endif // ARRAY__H
