/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 21.06.20
 */

#pragma once

/**
 * Class for for easier ignoring of unsupported user input.
 */
class CUnsupportedInput {
public:
    CUnsupportedInput() = delete;
    ~CUnsupportedInput() = delete;
    CUnsupportedInput(const CUnsupportedInput &) = delete;
    CUnsupportedInput & operator = (const CUnsupportedInput &) = delete;

    /**
     * Determines if given char is supported (application can properly display it).
     * @param input Input which should be checked.
     * @return True if input is supported, False if it is not.
     */
    static bool isSupported(wchar_t input);
};


