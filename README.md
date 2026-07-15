# Concurrent Binary Search Tree

A thread-safe concurrent binary search tree (BST) implemented in modern C++20. This project demonstrates fine-grained synchronization techniques using per-node locking to enable concurrent access while maintaining binary search tree properties

## Features

- Thread-safe insertions
- Thread-safe deletions
- Concurrent read operations
- Fine-grained locking using `std::shared_mutex`
- Atomic node counting using `std::atomic`
- Smart pointer memory management with `std::shared_ptr`
- Iterative implementations (no recursion)

## Supported Operations

### Tree Modification
- Insert
- Delete

### Search
- Contains
- Get minimum value
- Get maximum value
- Tree depth
- Node count

### Traversals
- Inorder
- Reverse Inorder
- Preorder
- Reverse Preorder
- Postorder
- Breadth-First Search (Level Order)

## Design

The implementation uses **lock coupling (hand-over-hand locking)** during tree traversal. Each node owns its own `std::shared_mutex`, allowing multiple threads to operate on different parts of the tree simultaneously instead of serializing all operations behind a single global mutex.

The tree stores:

- Per-node shared/exclusive mutexes
- Atomic node count
- Smart pointers for automatic memory management
- Cached tree depth

Traversal algorithms and destruction are implemented iteratively using explicit stacks and queues to avoid recursive function calls.
