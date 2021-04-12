#include"MyChar32.h"
#include<stdarg.h>

MyChar32::MyChar32(const char str[MyChar32::_kLength])
{
	strcpy_s(_str, str);
}

MyChar32::MyChar32(const MyChar32& str) 
	:MyChar32(str._str)
{}

MyChar32& MyChar32::operator=(const MyChar32& str)
{
	strcpy_s(this->_str, str._str);
	return *this;
}

bool MyChar32::operator==(const MyChar32& str) const
{
	return std::strcmp(_str, str._str) == 0;
}

bool MyChar32::operator!=(const MyChar32& str) const
{
	return !(*this == str);
}

MyChar32::operator const char* () const
{
	return _str;
}

//NOTE: 文字列への生ポインタを渡すので、意図せず文字列を変更しないよう注意。
char* MyChar32::data() const
{
	return (char*)_str;
}

bool MyChar32::Compare(const MyChar32& a, const MyChar32& b)
{
	return std::strcmp(a._str, b._str) == 0;
}

//32文字まで可変長文字列を代入する
void MyChar32::Copy(MyChar32& dst, const char* src, ...)
{
	va_list args;
	va_start(args, src); //args = srcの次のパラメータ(= ...)
	
	//'_BufferCount'の文字数だけ'_Buffer'にコピー
	_vsprintf_p(dst._str, MyChar32::_kLength, src, args);

	va_end(args);
}
