# Code Style Guide

## Introduction

This document outlines coding standards for our project that uses C99 with C++20 compatibility, utilizing SDL3 and flecs, along with our toolchain configuration.

## Language Standards

- Primary: C99
- Compatibility: C++20
- Compilation must succeed with both C and C++ compilers
  - gcc
  - clang
  - MSVC

## Platforms supported

- Windows x64
- macOS x64
- Web (Emscripten) wasm-32

## File Structure

```txt
mvn/
├── docs/             # Documentation
├── src/              # Source files
│   ├── core/         # Core functionality
│   ├── components/   # ECS components
│   ├── systems/      # ECS systems
│   └── platform/     # Platform-specific code
├── include/          # Public headers
│   └── mvn/          # Namespaced headers
├── tests/            # Tests
├── examples/         # Example code
└── tools/            # Build/dev tools and scripts
```

## Naming Conventions

-**Files**: `dash-case.{c,h,cpp,hpp}` -**Functions**:
-C: `snake_case()`
-C++: `PascalCase()` for classes, `camelCase()` for methods -**Variables**: `snake_case` -**Constants/Macros**: `UPPER_SNAKE_CASE` -**Types**: `PascalCase` for structs, enums, unions, typedefs

## Formatting

### General Rules

- Indentation: 4 spaces (no tabs)
- Line length: 100 characters maximum
- UTF-8 encoding
- LF line endings

### Braces

```c
// Always use braces for control structures and have braces on new line
if (condition) 
{
    do_something();
} 
else 
{
    do_something_else();
}

// Function braces on new line
void function_name(int param)
{
    // Implementation
}
```

## C99/C++20 Compatibility

```c
// Use ifdef guards for C++ compatibility
#ifdef __cplusplus
extern "C" {
#endif

// C99 code here

#ifdef __cplusplus
}
#endif

// Avoid C++ keywords as identifiers (new, class, template, etc.)
// Use void* instead of auto in C99 code
```

## SDL3 and flecs Best Practices

- Keep SDL initialization and shutdown in well-defined lifecycle functions
- Group related flecs components in sensible files
- Follow entity-component-system pattern strictly
- Use flecs systems for behavior, not raw loops

## Documentation

- Use Doxygen-style comments for all public APIs
- Document parameters and return values
- Include usage examples for complex functions

## CMake Standards

```cmake
# Enforce C99/C++20 standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use modern CMake practices
target_include_directories()
target_link_libraries()
target_compile_options()
```

## Version Control

- Use atomic commits with descriptive messages
- Reference issue numbers in commits when applicable
- Keep pull requests focused on a single task
