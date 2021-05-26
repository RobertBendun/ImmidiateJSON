#ifndef IMM_JSON_GUARD
#define IMM_JSON_GUARD

#include <cstdint>
#include <string_view>
#include <ostream>
#include <stack>
#include <functional>

struct Json
{
#define Scope(Type, End) \
	struct Type ## _Scope \
	{ \
		~Type ## _Scope () { if (json) end(); } \
		void end() { json->end(End); json = nullptr; } \
		Json *json; \
	}

	Scope(Object, "}");
	Scope(Array, "]");
#undef Scope

	auto write_to(std::ostream& os) -> void;
	auto end() -> void;

	auto operator=(std::nullptr_t) -> Json&;
	auto operator=(bool) -> Json&;
	auto operator=(double) -> Json&;
	auto operator=(std::string_view) -> Json&;

	inline auto operator=(char const *s) -> Json& { return *this = std::string_view(s); }

	auto key(unsigned) -> Json&;
	auto key(std::string_view) -> Json&;
	auto operator[](unsigned) -> Json&;
	auto operator[](std::string_view) -> Json&;

	auto object() -> Object_Scope;
	auto array() -> Array_Scope;

private:
	enum class State : std::uint8_t
	{
		None,
		Array,
		Object,
		Print_Comma,
	};

	auto end(std::string_view end) -> void;
	static auto default_writer(std::string_view) -> void;

	std::function<void(std::string_view)> writer = &Json::default_writer;
	std::stack<State> state;
};

#endif

#ifdef IMM_JSON_IMPLEMENTATION

#include <array>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <limits>

static void write_quoted(std::function<void(std::string_view)> &writer, std::string_view value)
{
	writer("\"");
	unsigned start = 0;
	for (unsigned i = 0; i < value.size(); ++i) {
		if (value[i] == '\"') {
			writer(value.substr(start, i - start));
			writer("\\\"");
			start = i + 1;
		}
	}

	if (start < value.size() - 1) {
		writer(value.substr(start));
	}
	writer("\"");
}

auto Json::default_writer(std::string_view sv) -> void
{
	std::cout << sv;
}

auto Json::array() -> Array_Scope
{
	writer("[");
	state.push(State::Array);
	return Array_Scope{this};
}

auto Json::object() -> Object_Scope
{
	writer("{");
	state.push(State::Object);
	return Object_Scope{this};
}

auto Json::key(std::string_view key) -> Json&
{
	if (!state.empty() && state.top() == State::Print_Comma) {
		writer(",");
		state.pop();
	}
	write_quoted(writer, key);
	writer(":");
	return *this;
}

auto Json::end(std::string_view sv) -> void
{
	writer(sv);
	if (state.top() == State::Print_Comma)
		state.pop();
	state.pop();
	if (!state.empty()) \
		state.push(State::Print_Comma);
}

#define Begin(param) \
	auto Json::operator=(param) -> Json& \
	{ \
		if (!state.empty() && state.top() == State::Print_Comma) { \
			writer(","); \
			state.pop(); \
		}

#define End	\
	if (!state.empty()) \
		state.push(State::Print_Comma); \
	return *this; \
	}

Begin(std::nullptr_t) writer("null"); End
Begin(bool value)	    writer(value ? "true" : "false"); End

Begin(double value)
	constexpr auto Size = std::numeric_limits<decltype(value)>::max_digits10;
	std::array<char, Size + 1> buffer = {};

	auto [ptr, ec] = std::to_chars(std::begin(buffer), std::end(buffer), value);
	if (ptr == std::end(buffer)) {
		return *this;
	}
	writer({ std::begin(buffer), ptr });
End

Begin(std::string_view value)
	write_quoted(writer, value);
End

#endif
