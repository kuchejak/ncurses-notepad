/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 30.05.20
 */

#include "CConverter.h"

#include <string>

// declaration
std::wstring_convert<std::codecvt_utf8<wchar_t> , wchar_t> CConverter::Converter;

std::string CConverter::toString(const std::wstring & wstring) {
    return Converter.to_bytes(wstring);
}

std::wstring CConverter::toWString(const std::string & string) {
    return Converter.from_bytes(string);
}

