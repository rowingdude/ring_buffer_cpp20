## C++20 Ring Buffer: Documentation and Example Usage

This document provides explanations and examples for the RingBuffer class template.

## RingBuffer Class Overview

The RingBuffer is a simple fixed-size circular queue implementation. It allows elements to be added and removed in a FIFO (First-In, First-Out) manner. When the buffer is full, adding new elements will overwrite the oldest elements.

**Key Features:**

*    Fixed Size: The buffer is initialized with a fixed capacity.
*    Overwrite on Full: When push or emplace is used on a full buffer, the oldest element is automatically overwritten.
*    Move Semantics: Optimized push and pop operations leverage C++11 move semantics to avoid unnecessary copies for non-trivial types.
*    In-Place Construction: The emplace method allows elements to be constructed directly within the buffer, minimizing overhead.
*    Safe Access: at() provides bounds-checked access to elements, throwing std::out_of_range for invalid indices.
*    Non-Throwing Operations: try_push and try_pop offer alternatives to push and pop that return a boolean indicating success or failure, useful in contexts where exceptions are undesirable.
*    Clear Functionality: The clear() method resets the buffer to an empty state.
*    Iterator Support: The RingBuffer provides iterators (begin(), end(), cbegin(), cend()) allowing it to be used with range-based for loops and standard library algorithms.

**Member Variables:**

*    buffer_: The underlying std::vector used for storage.
*    capacity_: The maximum number of elements the buffer can hold.
*    head_: The index of the oldest element (the next to be read/popped).
*    tail_: The index of the next available slot (the next to be written/pushed).
*    size_: The current number of elements in the buffer.

**Member Functions:**

`explicit RingBuffer(size_t capacity):`

  Constructs a new RingBuffer object with a specified capacity.
  Throws std::invalid_argument if capacity is 0.

`void push(const T& item):`

  Adds an element to the back of the buffer (copy version).
  If the buffer is full, the oldest element is overwritten.

`void push(T&& item):`

  Adds an element to the back of the buffer (move version).
  If the buffer is full, the oldest element is overwritten.

`template <typename... Args> void emplace(Args&&... args):`

  Constructs an element in-place at the back of the buffer using Args....
  If the buffer is full, the oldest element is overwritten.

`bool try_push(const T& item):`

  Attempts to add an element to the back of the buffer (copy version).
  Returns true if successful, false if the buffer is full (and no overwrite occurs). This version does NOT overwrite existing elements when full.

`bool try_push(T&& item):`

  Attempts to add an element to the back of the buffer (move version).
  Returns true if successful, false if the buffer is full (and no overwrite occurs).

`T pop():`

  Removes and returns the oldest element from the front of the buffer.
  Throws std::out_of_range if the buffer is empty.

`bool try_pop(T& out_item):`

  Attempts to remove and return the oldest element from the front of the buffer.
  Returns true if successful, false if the buffer is empty. The popped item is moved into out_item.

`const T& front() const:`

  Returns a const reference to the oldest element without removing it.
  Throws std::out_of_range if the buffer is empty.

`T& at(size_t index):`

  Returns a reference to the element at the specified index, with bounds checking.
  The index is relative to the current front of the buffer (0 is the front).
  Throws std::out_of_range if the index is out of bounds.

`const T& at(size_t index) const:`

  Returns a const reference to the element at the specified index, with bounds checking.
  The index is relative to the current front of the buffer (0 is the front).
  Throws std::out_of_range if the index is out of bounds.

`bool empty() const:`

  Checks if the buffer is empty. Returns true if the buffer contains no elements, false otherwise.

`bool full() const:`

  Checks if the buffer is full. Returns true if the buffer has reached its maximum capacity, false otherwise.

`size_t size() const:`

  Returns the current number of elements in the buffer.

`size_t capacity() const:`

  Returns the maximum capacity of the buffer.

`void clear():`

  Clears the buffer, making it empty. Note that elements are not explicitly destructed here; they will be overwritten or destructed when the buffer itself is destructed.

`RingBufferIterator begin() / const RingBufferIterator begin() const:`

  Returns an iterator to the beginning of the buffer.

`RingBufferIterator end() / const RingBufferIterator end() const:`

  Returns an iterator to the end of the buffer (one past the last element).

`const RingBufferIterator cbegin() const:`

  Returns a const iterator to the beginning of the buffer.

`const RingBufferIterator cend() const:`

  Returns a const iterator to the end of the buffer.

For example:

```cpp

#include <iostream>
#include <string>
#include <stdexcept> // For catching exceptions
#include <utility>   // For std::move

// Assuming the RingBuffer class is available (e.g., in a header file or included directly)
// #include "RingBuffer.h" // If in a separate file

int main() {
    std::cout << "--- RingBuffer of integers (capacity 3) ---" << std::endl;
    RingBuffer<int> intBuffer(3);

    std::cout << "Is empty: " << (intBuffer.empty() ? "Yes" : "No") << ", Size: " << intBuffer.size() << std::endl;

    intBuffer.push(10);
    intBuffer.push(20);
    std::cout << "Pushed 10, 20. Size: " << intBuffer.size() << ", Front: " << intBuffer.front() << std::endl;

    intBuffer.push(30);
    std::cout << "Pushed 30. Size: " << intBuffer.size() << ", Is full: " << (intBuffer.full() ? "Yes" : "No") << ", Front: " << intBuffer.front() << std::endl;

    // Demonstrate at()
    std::cout << "Element at index 0: " << intBuffer.at(0) << std::endl; // Should be 10
    std::cout << "Element at index 1: " << intBuffer.at(1) << std::endl; // Should be 20
    try {
        std::cout << "Element at index 3: " << intBuffer.at(3) << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "Caught exception for at(3): " << e.what() << std::endl;
    }

    intBuffer.push(40); // Overwrites 10
    std::cout << "Pushed 40 (overwrites 10). Size: " << intBuffer.size() << ", Front: " << intBuffer.front() << std::endl; // Should be 20

    std::cout << "Popped: " << intBuffer.pop() << ", Size: " << intBuffer.size() << ", Front: " << intBuffer.front() << std::endl; // Popped 20, Front 30
    std::cout << "Popped: " << intBuffer.pop() << ", Size: " << intBuffer.size() << ", Front: " << intBuffer.front() << std::endl; // Popped 30, Front 40

    intBuffer.push(50);
    std::cout << "Pushed 50. Size: " << intBuffer.size() << ", Front: " << intBuffer.front() << std::endl; // Front 40

    std::cout << "Popped: " << intBuffer.pop() << ", Size: " << intBuffer.size() << std::endl; // Popped 40
    std::cout << "Popped: " << intBuffer.pop() << ", Size: " << intBuffer.size() << std::endl; // Popped 50

    std::cout << "Is empty: " << (intBuffer.empty() ? "Yes" : "No") << ", Size: " << intBuffer.size() << std::endl;

    try {
        intBuffer.pop(); // Attempt to pop from empty buffer
    } catch (const std::out_of_range& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    std::cout << "\n--- RingBuffer of strings (capacity 2) ---" << std::endl;
    RingBuffer<std::string> stringBuffer(2);
    stringBuffer.push("Apple");
    stringBuffer.push("Banana");
    std::cout << "Pushed Apple, Banana. Size: " << stringBuffer.size() << ", Front: " << stringBuffer.front() << std::endl;

    stringBuffer.push("Cherry"); // Overwrites Apple
    std::cout << "Pushed Cherry (overwrites Apple). Size: " << stringBuffer.size() << ", Front: " << stringBuffer.front() << std::endl; // Should be Banana

    std::cout << "Popped: " << stringBuffer.pop() << std::endl; // Popped Banana
    std::cout << "Popped: " << stringBuffer.pop() << std::endl; // Popped Cherry
    std::cout << "Is empty: " << (stringBuffer.empty() ? "Yes" : "No") << std::endl;

    std::cout << "\n--- Demonstrating try_push/try_pop and emplace ---" << std::endl;
    RingBuffer<std::string> demoBuffer(2);
    std::string s1 = "Hello";
    std::string s2 = "World";

    if (demoBuffer.try_push(s1)) {
        std::cout << "try_push 'Hello' successful. Size: " << demoBuffer.size() << std::endl;
    }
    if (demoBuffer.try_push(std::move(s2))) { // Using move semantics
        std::cout << "try_push 'World' (moved) successful. Size: " << demoBuffer.size() << std::endl;
    }

    // This push will overwrite 'Hello'
    demoBuffer.push("New Item");
    std::cout << "Pushed 'New Item' (overwrites 'Hello'). Front: " << demoBuffer.front() << std::endl;

    std::string popped_item;
    if (demoBuffer.try_pop(popped_item)) {
        std::cout << "try_pop successful: " << popped_item << ". Size: " << demoBuffer.size() << std::endl; // Should be World
    }
    if (demoBuffer.try_pop(popped_item)) {
        std::cout << "try_pop successful: " << popped_item << ". Size: " << demoBuffer.size() << std::endl; // Should be New Item
    }
    if (!demoBuffer.try_pop(popped_item)) {
        std::cout << "try_pop failed (buffer empty)." << std::endl;
    }

    std::cout << "\n--- Demonstrating emplace and iterators ---" << std::endl;
    RingBuffer<std::string> iterBuffer(4);
    iterBuffer.emplace("First");
    iterBuffer.emplace("Second");
    iterBuffer.emplace("Third");
    iterBuffer.emplace("Fourth"); // Buffer is full
    iterBuffer.emplace("Fifth");  // Overwrites "First"

    std::cout << "Buffer contents (using range-based for loop):" << std::endl;
    for (const auto& s : iterBuffer) {
        std::cout << "- " << s << std::endl;
    }
    // Expected output: Second, Third, Fourth, Fifth

    std::cout << "Clearing buffer..." << std::endl;
    iterBuffer.clear();
    std::cout << "Is empty after clear: " << (iterBuffer.empty() ? "Yes" : "No") << ", Size: " << iterBuffer.size() << std::endl;


    return 0;
}

```
