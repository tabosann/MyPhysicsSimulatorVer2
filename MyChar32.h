#pragma once

class MyChar32 {
public:
	static constexpr int _kSize = 32;

	MyChar32(const char str[_kSize] = "");
	MyChar32(const MyChar32& str);

	MyChar32& operator= (const MyChar32& str);
	bool      operator==(const MyChar32& str) const;
	bool      operator!=(const MyChar32& str) const;

	operator const char* () const;

	char* data() const;

	static bool Compare(const MyChar32& a, const MyChar32& b);
	static void Copy(MyChar32& dst, const char* src, ...);

private:
	char _str[_kSize];
};

using Char32 = MyChar32;