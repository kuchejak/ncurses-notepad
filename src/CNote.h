/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 15.05.20
 */

#pragma once

#include "CTextStorage.h"
#include "CFormat.h"

#include <initializer_list>
#include <string>
#include <unordered_set>

/**
 * Representation of note file. Stores tags, category and name of the file/note.
 */
class CNote {
public:
    /**
     * Creates new note with given name.
     * @param name Name of new note.
     */
    CNote(const std::wstring & name);
    ~CNote() = default;
    CNote(const CNote &) = default;
    CNote & operator = (const CNote & src);

    /**
     * @return Name of note/file.
     */
    const std::wstring & getName() const;

    /**
     * @return Note's category, if note's category has not been set returns empty string.
     */
    const std::wstring & getCategory() const;

    /**
     * @return unordered_set of tags associated with file.
     */
    const std::unordered_set<std::wstring> & getTags() const;

    /**
     * Parses given line and if it is in correct format sets note's tags accordingly.
     * @param[in] tagLine Wstring from which tags should be read. (format is "!tags: <tags>")
     */
    void setTags(const std::wstring & tagLine);

    /**
     * Sets category for note.
     */
    void setCategory(const std::wstring & category);

    /**
     * @return True if note has given tag, false if not.
     */
    bool hasTag(const std::wstring & tag) const;


    /**
     * @return True if note has given category, false if not.
     */
    bool hasCategory(const std::wstring & category) const;

    /**
     * @return Note's file extension.
     */
    std::string getFileExt() const;

private:
    std::wstring m_Name;
    std::unordered_set<std::wstring> m_Tags;
    std::wstring m_Category;

    /**
     * If given line is in correct format, extracts tags from it.
     * @param tagLine Text line where information about tags should be (right format is "!tags: <tags>")
     * @return Tags (without the "!tags:" part)
     */
    static std::wstring extractTags(const std::wstring & tagLine);
};



