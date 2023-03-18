/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 31.05.20
 */

#pragma once

#include <string>

/**
 * Class for interactions with files.
 */
class CFile {
public:
    CFile() = default;
    ~CFile() = delete;
    CFile(const CFile &) = delete;
    CFile & operator = (const CFile &) = delete;

    /**
     * Copies given file to given destination (both from and to must contain full file name).
     * @param[in] from File that should be copied.
     * @param[in] to Destination where should be file copied (must include new file's name).
     * @return True if copying was successful, false if not.
     */
    static bool copyFile(const std::string & from , const std::string & to);

    /**
     * Reads last line of given file.
     * @param file File.
     * @return Last line of given file.
     */
    static std::wstring readLastLine(const std::string & file);

    /**
     * Checks if file with given name exist.
     * @param[in] name Name of file.
     * @return True if file exist, false if not.
     */
    static bool fileExist(const std::string & name);

    /**
     * Checks if given string is present in given file
     * @param[in] text Text to find.
     * @param[in] file File to search in.
     * @return True if string is present, false if not. (Also returns false if file can not be opened)
     */
    static bool textIsInFile(const std::string & text, const std::string & file);
};


