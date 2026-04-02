# C2x Examples

A collection of C23 and modern C practice programs, data structures, and small experiments.

This repository is my hands-on study notebook for catching up with modern C after spending many years away from the language standard. The goal is to learn by writing code, testing ideas, and revisiting core concepts through small focused examples.

## Goals

- Practice modern C and C23 features
- Strengthen fundamentals through small, self-contained programs
- Build and refine core data structures and algorithms
- Explore low-level and systems-oriented C programming techniques
- Keep examples simple enough to study, modify, and reuse

## Repository Layout

The repository includes both topic-based folders and standalone source files.

### Topic folders

- `src/deque` - deque implementation experiments
- `src/getline` - input and line-reading utilities
- `src/graph` - Graph related
- `src/hashtable` - hash table work
- `src/heap` - heap and heap-based sorting experiments
- `src/llist` - linked list implementations
- `src/network` - socket and networking examples
- `src/set` - set data structure work
- `src/sort` - sortting related full generic quicksort implementation  etc.
- `src/stack` - stack implementations

### Standalone examples

The `src` directory also contains smaller focused examples covering topics such as:

- sorting
- hashing
- linked lists
- `restrict`
- thread-local storage
- inline assembly
- function attributes and related language features
- object-style struct patterns
- tracing and logging experiments
- graph/visualizatino of shortest path
 
## Build

### Build with Release settings

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Build wit Debug settings

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

## Notes

This repository is primarily for learning, experimentation, and practice. Some examples are intentionally small and focused on a single concept rather than being production-style libraries.

Over time, this repository serves as a reference of what I have been studying and implementing in modern C.

## License

GPL-3.0
