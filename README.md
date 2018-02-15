# JSON With Schema

This library is a wrapper around [nlohmann/json](https://github.com/nlohmann/json/releases) for parsing
JSON and [pboettch/json-schema-validator](https://github.com/pboettch/json-schema-validator) for validating
JSON against a schema described by [JSON-Schema](http://json-schema.org) draft4.

## Getting Started

### Prerequisites

Add a snapshot of this repository to your source code or add it as a git submodule.

### Build

These instructions assume you are using [cmake](cmake.org).

In your CMakeLists.txt, add:
```CMake
add_subdirectory(path/to/jws jws)
add_executable(myapp main.cpp)
target_link_libraries(myapp jws)
```

### Example

```C++
#include <jws/json_with_schema.hpp>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

  if (argc != 3) {
    cout << "usage: validate_json <schema.json> <document.json>" << endl;
    return 1;
  }

  auto validator = load_validator(argv[1]);
  auto document = load_json(argv[2]);
  validator.validate(document); // throws on error

  cout << "document validated" << endl;

  return 0;
}
```

### Build Examples and Tests

To build the examples and tests run:
```bash
$ mkdir -p build && cd build
$ cmake -D BUILD_EXAMPLES=ON -D BUILD_TESTS=ON ..
$ make
```

### Run Tests
```bash
./test_jws --working_dir /path/to/jws
```
