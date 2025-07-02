#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <vector>     // For std::vector
#include <stdexcept>  // For std::invalid_argument, std::out_of_range
#include <utility>    // For std::forward, std::move

// A simple fixed-size ring buffer (circular queue) implementation.
// This class provides a basic ring buffer that allows elements to be added
// and removed in a FIFO (First-In, First-Out) manner. When the buffer is full,
// adding new elements will overwrite the oldest elements.
template <typename T>
class RingBuffer {
public:
    // Constructs a new RingBuffer object with a specified capacity.
    // The capacity must be greater than 0.
    explicit RingBuffer(size_t capacity);

    // Adds an element to the back of the buffer (copy version).
    // If the buffer is full, the oldest element is overwritten.
    void push(const T& item);

    // Adds an element to the back of the buffer (move version).
    // If the buffer is full, the oldest element is overwritten.
    void push(T&& item);

    // Constructs an element in-place at the back of the buffer.
    // If the buffer is full, the oldest element is overwritten.
    template <typename... Args>
    void emplace(Args&&... args);

    // Attempts to add an element to the back of the buffer.
    // Returns true if successful, false if the buffer is full (and no overwrite occurs).
    // This version does NOT overwrite existing elements when full.
    bool try_push(const T& item);

    // Attempts to add an element to the back of the buffer (move version).
    // Returns true if successful, false if the buffer is full (and no overwrite occurs).
    bool try_push(T&& item);

    // Removes and returns the oldest element from the front of the buffer.
    // Throws std::out_of_range if the buffer is empty.
    T pop();

    // Attempts to remove and return the oldest element from the front of the buffer.
    // Returns true if successful, false if the buffer is empty.
    bool try_pop(T& out_item);

    // Returns a const reference to the oldest element without removing it.
    // Throws std::out_of_range if the buffer is empty.
    const T& front() const;

    // Returns a reference to the element at the specified index, with bounds checking.
    // The index is relative to the current front of the buffer (0 is the front).
    // Throws std::out_of_range if the index is out of bounds.
    T& at(size_t index);

    // Returns a const reference to the element at the specified index, with bounds checking.
    // The index is relative to the current front of the buffer (0 is the front).
    // Throws std::out_of_range if the index is out of bounds.
    const T& at(size_t index) const;

    // Checks if the buffer is empty.
    bool empty() const;

    // Checks if the buffer is full.
    bool full() const;

    // Returns the current number of elements in the buffer.
    size_t size() const;

    // Returns the maximum capacity of the buffer.
    size_t capacity() const;

    // Clears the buffer, making it empty.
    void clear();

    // Iterator support:
    // This nested class allows RingBuffer to be used with range-based for loops.
    class RingBufferIterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = T*;
        using reference         = T&;

        // Constructor: Takes a pointer to the RingBuffer's internal buffer,
        // the starting logical index (relative to head_), the buffer's head,
        // and its capacity.
        RingBufferIterator(T* data_ptr, size_t current_logical_index, size_t buffer_head, size_t buffer_capacity);

        // Dereference operator
        reference operator*() const;
        pointer operator->() const;

        // Pre-increment
        RingBufferIterator& operator++();

        // Post-increment
        RingBufferIterator operator++(int);

        // Equality comparison
        friend bool operator==(const RingBufferIterator& a, const RingBufferIterator& b);
        // Inequality comparison
        friend bool operator!=(const RingBufferIterator& a, const RingBufferIterator& b);

    private:
        T* data_ptr_;                 // Pointer to the start of the underlying std::vector's data
        size_t current_logical_index_; // Current index relative to the logical start (0 to size-1)
        size_t buffer_head_;          // The head index of the RingBuffer
        size_t buffer_capacity_;      // The capacity of the RingBuffer
    };

    // Returns an iterator to the beginning of the buffer.
    RingBufferIterator begin();

    // Returns a const iterator to the beginning of the buffer.
    RingBufferIterator begin() const;

    // Returns an iterator to the end of the buffer (one past the last element).
    RingBufferIterator end();

    // Returns a const iterator to the end of the buffer (one past the last element).
    RingBufferIterator end() const;

    // Returns a const iterator to the beginning of the buffer.
    RingBufferIterator cbegin() const;

    // Returns a const iterator to the end of the buffer.
    RingBufferIterator cend() const;

private:
    std::vector<T> buffer_;   // The underlying storage for elements
    size_t capacity_;         // The maximum number of elements the buffer can hold
    size_t head_;             // Index of the oldest element (next to be read)
    size_t tail_;             // Index of the next available slot (next to be written)
    size_t size_;             // Current number of elements in the buffer
};

#endif // RING_BUFFER_HPP
