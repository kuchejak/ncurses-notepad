/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 30.05.20
 */

#pragma once
#include <codecvt>
#include <string>
#include <locale>


/**
 * Class used for converting to/from wstring.
 */
class CConverter {
public:
    CConverter() = delete;
    ~CConverter() = delete;
    CConverter(const CConverter &) = delete;
    CConverter & operator = (const CConverter &) = delete;

    /**
     * Converts given wstring to string.
     * @param[in] wstring Wstring that should be converted.
     * @return Converted string.
     */
    static std::string toString(const std::wstring & wstring);

    /**
     * Converts given string to wstring.
     * @param[in] string String that should be converted.
     * @return Converted wstring.
     */
    static std::wstring toWString(const std::string & string);

    /**
     * Class that handles the conversions.
     */
    static std::wstring_convert<std::codecvt_utf8<wchar_t> , wchar_t> Converter;
};


