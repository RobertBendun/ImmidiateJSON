#ifndef IMM_JSON_GUARD
#define IMM_JSON_GUARD

#include <cstdint>
#include <string_view>
#include <iostream>
#include <stack>

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

	template<typename ...Args>
	inline void print(Args const& ...args)
	{
		(void)(*out << ... << args);
	}

	auto end(std::string_view end) -> void;

	std::ostream *out = &std::cout;
	std::stack<State> state;
};

#endif

#ifdef IMM_JSON_IMPLEMENTATION

#include <iomanip>

auto Json::write_to(std::ostream& out) -> void
{
	this->out = &out;
}

auto Json::array() -> Array_Scope
{
	print("[");
	state.push(State::Array);
	return Array_Scope{this};
}

auto Json::object() -> Object_Scope
{
	print("{");
	state.push(State::Object);
	return Object_Scope{this};
}

auto Json::key(std::string_view key) -> Json&
{
	if (!state.empty() && state.top() == State::Print_Comma) {
		print(",");
		state.pop();
	}
	print(std::quoted(key), ":");
	return *this;
}

auto Json::end(std::string_view sv) -> void
{
	print(sv);
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
			print(","); \
			state.pop(); \
		}
	
#define End	\
	if (!state.empty()) \
		state.push(State::Print_Comma); \
	return *this; \
	}

Begin(std::nullptr_t) print("null"); End
Begin(bool value)	    print(value ? "true" : "false"); End
Begin(double value)   print(value); End
Begin(std::string_view value) print(std::quoted(value)); End

#endif