#define IMM_JSON_IMPLEMENTATION
#include "imm_json.hh"

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
