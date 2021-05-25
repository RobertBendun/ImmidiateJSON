# Immediate JSON serialization library
This library is an exploration of writing [immediate mode](https://en.wikipedia.org/wiki/Immediate_mode_GUI) JSON serialization in C++.

## Example
The same as in [example.cc](example.cc) file.
```cpp
#define IMM_JSON_IMPLEMENTATION
#include "imm_json.hh"

#include <iostream>

auto main() -> int
{
	Json json;

	auto _object = json.object();

	json.key("null") = nullptr;

	json.key("booleans");
	{
		auto _array = json.array();
		json = false;
		json = true;
	}

	json.key("numbers");
	{
		auto _array = json.array();
		for (double i = -1; i < 2; ++i)
			json = i;
	}

	json.key("strings");
	{
		auto _object = json.object();
		json.key("normal") = "hello, world!";
		json.key("escaped quotes") = "hello, \"world\"";
	}

	_object.end();

	std::cout << '\n';
}
```

### Output
`python -m json.tool` is for pretty printing.
```
$ c++ example.cc -o example -std=c++20
$ ./example | python -m json.tool
{
    "null": null,
    "booleans": [
        false,
        true
    ],
    "numbers": [
        -1,
        0,
        1
    ],
    "strings": {
        "normal": "hello, world!",
        "escaped quotes": "hello, \"world\""
    }
}

```

