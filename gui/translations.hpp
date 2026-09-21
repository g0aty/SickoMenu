#pragma once
#include "../user/state.hpp"
#include <string>

namespace SickoLang {
	const char* Get(const char* text);
	const char* Get(const std::string& text);
	void Reload();
}

inline const char* T_(const char* text) { return SickoLang::Get(text); }
inline const char* T_(const std::string& text) { return SickoLang::Get(text); }