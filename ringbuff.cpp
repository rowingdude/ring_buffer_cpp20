#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>
#include <utility>

template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity)
        : buffer_(capacity), capacity_(capacity), head_(0), tail_(0), size_(0) {
        if (capacity == 0) {
            throw std::invalid_argument("RingBuffer capacity must be greater than 0.");
        }
    }

    void push(const T& item) {
        buffer_[tail_] = item;
        tail_ = (tail_ + 1) % capacity_;

        if (size_ < capacity_) {
            size_++;
        } else {
            head_ = (head_ + 1) % capacity_;
        }
    }

    void push(T&& item) {
        buffer_[tail_] = std::move(item);
        tail_ = (tail_ + 1) % capacity_;

        if (size_ < capacity_) {
            size_++;
        } else {
            head_ = (head_ + 1) % capacity_;
        }
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        buffer_[tail_] = T(std::forward<Args>(args)...);
        tail_ = (tail_ + 1) % capacity_;

        if (size_ < capacity_) {
            size_++;
        } else {
            head_ = (head_ + 1) % capacity_;
        }
    }

    bool try_push(const T& item) {
        if (full()) {
            return false;
        }
        push(item);
        return true;
    }

    bool try_push(T&& item) {
        if (full()) {
            return false;
        }
        push(std::move(item));
        return true;
    }

    T pop() {
        if (empty()) {
            throw std::out_of_range("Cannot pop from an empty RingBuffer.");
        }

        T item = std::move(buffer_[head_]);
        head_ = (head_ + 1) % capacity_;
        size_--;
        return item;
    }

    bool try_pop(T& out_item) {
        if (empty()) {
            return false;
        }
        out_item = std::move(buffer_[head_]);
        head_ = (head_ + 1) % capacity_;
        size_--;
        return true;
    }

    const T& front() const {
        if (empty()) {
            throw std::out_of_range("Cannot get front from an empty RingBuffer.");
        }
        return buffer_[head_];
    }

    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of bounds for RingBuffer::at()");
        }
        return buffer_[(head_ + index) % capacity_];
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of bounds for RingBuffer::at() const");
        }
        return buffer_[(head_ + index) % capacity_];
    }

    bool empty() const {
        return size_ == 0;
    }

    bool full() const {
        return size_ == capacity_;
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    void clear() {
        head_ = 0;
        tail_ = 0;
        size_ = 0;
    }

    class RingBufferIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        RingBufferIterator(T* data_ptr, size_t current_logical_index, size_t buffer_head, size_t buffer_capacity)
            : data_ptr_(data_ptr),
              current_logical_index_(current_logical_index),
              buffer_head_(buffer_head),
              buffer_capacity_(buffer_capacity) {}

        reference operator*() const {
            return data_ptr_[(buffer_head_ + current_logical_index_) % buffer_capacity_];
        }
        pointer operator->() const {
            return &data_ptr_[(buffer_head_ + current_logical_index_) % buffer_capacity_];
        }

        RingBufferIterator& operator++() {
            current_logical_index_++;
            return *this;
        }

        RingBufferIterator operator++(int) {
            RingBufferIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const RingBufferIterator& a, const RingBufferIterator& b) {
            return a.current_logical_index_ == b.current_logical_index_;
        }
        friend bool operator!=(const RingBufferIterator& a, const RingBufferIterator& b) {
            return !(a == b);
        }

    private:
        T* data_ptr_;
        size_t current_logical_index_;
        size_t buffer_head_;
        size_t buffer_capacity_;
    };

    RingBufferIterator begin() {
        return RingBufferIterator(buffer_.data(), 0, head_, capacity_);
    }

    RingBufferIterator begin() const {
        return RingBufferIterator(const_cast<T*>(buffer_.data()), 0, head_, capacity_);
    }

    RingBufferIterator end() {
        return RingBufferIterator(buffer_.data(), size_, head_, capacity_);
    }

    RingBufferIterator end() const {
        return RingBufferIterator(const_cast<T*>(buffer_.data()), size_, head_, capacity_);
    }

    RingBufferIterator cbegin() const {
        return RingBufferIterator(const_cast<T*>(buffer_.data()), 0, head_, capacity_);
    }

    RingBufferIterator cend() const {
        return RingBufferIterator(const_cast<T*>(buffer_.data()), size_, head_, capacity_);
    }


private:
    std::vector<T> buffer_;
    size_t capacity_;
    size_t head_;
    size_t tail_;
    size_t size_;
};
