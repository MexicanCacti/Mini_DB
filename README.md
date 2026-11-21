### Github Link: https://github.com/MexicanCacti/DB_Storage_Query_Processor

# Development Environment

This project is implemented in C++ using CMake.

# Build requirements

C++17-compatible compiler or newer

CMake 3.14 or newer

GoogleTest for unit testing

Windows or another supported operating system

# Run Instructions 
(From Top-Level Directory [...\DB_Storage_Query_Processor] )

- cmake -S . -B build
- cmake --build build --config Debug

# Running Tests
(From Top-Level Directory [...\DB_Storage_Query_Processor] )
- [Google_Tests : Expressions] <br> ./google_tests/expression_tests/Debug/all_expression_tests
- [Google_Tests : Executors] <br> ./google_tests/executor_tests/Debug/all_executor_tests
- [Query Demo] <br> ./tests/Debug/query_demo

# My Development Environment Info
- Operating System: Windows
- System Architecture: AMD64
- C++ Compiler: MSVC
- Compiler Version: 19.51.36246.0
- C++ Standard: 17
- CMake Version: 4.3.3
- Build Generator: Visual Studio 18 2026

# Project Organization
- query_processor/ - Contains the query processor library
- storage_engine/ - Contains the storage engine library
- google_tests/ - Unit tests written with GoogleTests for testing class functionality of query_processor expressions & executors
- tests/ - Hand-Written test for query_demo
