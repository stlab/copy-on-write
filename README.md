# stlab::copy_on_write

Copy-on-write wrapper for any type.

## Overview

`stlab::copy_on_write<T>` is a smart pointer that implements copy-on-write semantics. It allows multiple instances to share the same underlying data until one of them needs to modify it, at which point a copy is made. This can provide significant performance benefits when dealing with expensive-to-copy objects that are frequently copied but rarely modified.

## Features

- **Thread-safe**: Uses atomic reference counting for safe concurrent access
- **Header-only**: No compilation required, just include the header
- **C++17**: Leverages modern C++ features for clean, efficient implementation
- **Standard-compliant**: Follows established C++ idioms and best practices

## Usage

```cpp
#include <stlab/copy_on_write.hpp>
#include <string>
#include <vector>

// Basic usage with primitives
stlab::copy_on_write<int> cow_int(42);
auto copy = cow_int;  // Shares the same underlying data
cout << *cow_int << endl;  // 42

// Modify through write() - triggers copy-on-write
cow_int.write() = 100;
cout << *cow_int << endl;  // 100
cout << *copy << endl;     // 42 (unchanged)

// Usage with complex types
stlab::copy_on_write<std::vector<int>> cow_vec(1000, 42);
auto shared_vec = cow_vec;

// Both instances share the same data until write
assert(cow_vec.identity(shared_vec));  // Same underlying object

// Write access triggers copy
cow_vec.write().push_back(99);
assert(!cow_vec.identity(shared_vec)); // Now different objects
```

## API Reference

### Construction
- `copy_on_write()` - Default construction
- `copy_on_write(T&& value)` - Value construction
- `copy_on_write(Args&&... args)` - In-place construction

### Access
- `const T& read() const` - Read-only access
- `T& write()` - Write access (triggers copy if not unique)
- `const T& operator*() const` - Dereference operator
- `const T* operator->() const` - Arrow operator
- `operator const T&() const` - Implicit conversion

### Utilities
- `bool unique() const` - Check if this is the only reference
- `bool identity(const copy_on_write& other) const` - Check if sharing data
- `void swap(copy_on_write& other)` - Efficient swap

## Building and Testing

This library uses CMake with CPM for dependency management:

```bash
# Configure and build
cmake --preset=test
cmake --build --preset=test

# Run tests
ctest --preset=test
```

### Building Documentation

Documentation is generated using Doxygen with the modern [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css) theme:

```bash
# Configure with documentation enabled
cmake --preset=test -DBUILD_DOCS=ON

# Build documentation
cmake --build --preset=test --target docs

# View documentation (opens in browser)
open build/test/docs/html/index.html
```

**Requirements for documentation:**
- Doxygen 1.9.1 or later
- Internet connection (for downloading doxygen-awesome-css theme)

## Requirements

- C++17 compatible compiler
- CMake 4.0 or later (for building tests)

## Dependencies

This project uses the following external dependencies:

| Dependency | Version | Repository | File Location | Update Instructions |
|------------|---------|------------|---------------|-------------------|
| **CMake** | 4.0+ | [Kitware/CMake](https://github.com/Kitware/CMake) | `CMakeLists.txt` line 1 | Update `VERSION` in `cmake_minimum_required()` |
| **CPM.cmake** | v0.40.8 | [cpm-cmake/CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) | `CMakeLists.txt` lines 9-14 | Update version in download URL and SHA256 hash |
| **doctest** | v2.4.12 | [doctest/doctest](https://github.com/doctest/doctest) | `CMakeLists.txt` lines 17-23 | Update `GIT_TAG` in `CPMAddPackage` call |
| **Doxygen** | Latest | [doxygen/doxygen](https://github.com/doxygen/doxygen) | Optional for docs | Install via package manager or from source |
| **doxygen-awesome-css** | v2.3.4 | [jothepro/doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css) | `CMakeLists.txt` lines 76-81 | Update `GIT_TAG` in `CPMAddPackage` call |

### Updating Dependencies

#### CMake
To install or update CMake:

**Option 1: Official Installer**
1. Visit the [CMake download page](https://cmake.org/download/)
2. Download the installer for your platform
3. Follow the installation instructions

**Option 2: Package Manager**
- **macOS (Homebrew)**: `brew install cmake`
- **Ubuntu/Debian**: `sudo apt update && sudo apt install cmake`
- **Windows (Chocolatey)**: `choco install cmake`
- **Windows (Scoop)**: `scoop install cmake`

**Verify Installation**: `cmake --version`

#### CPM.cmake
To update CPM.cmake:
1. Visit the [CPM.cmake releases page](https://github.com/cpm-cmake/CPM.cmake/releases)
2. Find the desired version and copy the download URL
3. Update the version in the URL on line 11 of `CMakeLists.txt`
4. Update the SHA256 hash on line 13 (found in the release assets)

**CPM Caching**: This project enables CPM's caching feature to avoid re-downloading dependencies. 
The cache is stored in `.cpm-cache/` and is automatically ignored by git. To customize the cache 
location, set the `CPM_SOURCE_CACHE` environment variable or CMake variable.

#### doctest
To update doctest:
1. Visit the [doctest releases page](https://github.com/doctest/doctest/releases)
2. Find the desired version tag (e.g., `v2.4.12`)
3. Update the `GIT_TAG` value on line 21 of `CMakeLists.txt`

#### Doxygen
To install or update Doxygen:

**Option 1: Package Manager**
- **macOS (Homebrew)**: `brew install doxygen`
- **Ubuntu/Debian**: `sudo apt update && sudo apt install doxygen`
- **Windows (Chocolatey)**: `choco install doxygen.install`
- **Windows (Scoop)**: `scoop install doxygen`

**Option 2: Official Installer**
1. Visit the [Doxygen download page](https://www.doxygen.nl/download.html)
2. Download the installer for your platform
3. Follow the installation instructions

**Verify Installation**: `doxygen --version`

#### doxygen-awesome-css
This dependency is automatically downloaded via CPM when building documentation. 
To update the theme version, update the `GIT_TAG` value in the `CPMAddPackage` call in `CMakeLists.txt`.

## License

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt 
