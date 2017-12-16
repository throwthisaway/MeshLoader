#include "StringUtil.h"
#include <codecvt>

//https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string/18374698#18374698
std::wstring s2ws(const std::string& str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converter;
	return converter.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converter;
	return converter.to_bytes(wstr);
}

int istrcmp(const std::wstring& str1, size_t offset, const std::wstring& str2) {
	std::wstring::const_iterator it1 = std::begin(str1) + offset,
		it2 = std::begin(str2);
	std::locale loc;
	while (it1 != std::end(str1) && it2 != std::end(str2)) {
		auto ch1 = std::tolower(*it1, loc), ch2 = std::tolower(*it2, loc);
		if (ch1 < ch2) return -1;
		if (ch1 > ch2) return 1;
		++it1; ++it2;
	}
	if (it1 != std::end(str1)) return -1;
	if (it2 != std::end(str2)) return 1;
	return 0;
}

std::wstring extractFilename(const std::wstring& path) {
	auto offset = path.rfind(L'\\');
	if (offset == std::wstring::npos) offset = path.rfind(L'/');
	if (offset == std::wstring::npos) offset = 0;
	else ++offset;
	return path.substr(offset);
}
