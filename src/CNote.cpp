/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 15.05.20
 */

#include "CNote.h"
#include "CConverter.h"

CNote::CNote(const std::wstring & name) : m_Name(name) {}

const std::wstring & CNote::getName() const {
    return m_Name;
}

const std::wstring & CNote::getCategory() const {
    return m_Category;
}

const std::unordered_set<std::wstring> & CNote::getTags() const {
    return m_Tags;
}

void CNote::setTags(const std::wstring & tagLine) {
    std::wstring tags = extractTags(tagLine);
    m_Tags.clear();
    std::wstring tag;
    for (const auto & letter : tags) {
        if (letter == L' ' && tag.length() != 0) {
            m_Tags.emplace(tag);
            tag = L"";
        }
        else if (letter != L' ') {
            tag += letter;
        }
    }
    if (tag.length() != 0)
        m_Tags.emplace(tag);
}

void CNote::setCategory(const std::wstring & category) {
    m_Category = category;
}

bool CNote::hasTag(const std::wstring & tag) const {
    return m_Tags.find(tag) != m_Tags.end();
}

bool CNote::hasCategory(const std::wstring & category) const {
   return category == m_Category;
}

std::string CNote::getFileExt() const {
    return CConverter::toString(m_Name.substr(m_Name.find_last_of(L'.')));
}

std::wstring CNote::extractTags(const std::wstring & tagLine) {
    std::wstring tagFlag = L"!tags:";
    size_t indexOf = tagLine.find(tagFlag);

    if (indexOf == std::wstring::npos)  // if tagFlag was not found
        return L"";

    for (const auto & letter : tagLine.substr(0, indexOf))
        if (letter != ' ') // line is not in correct format
            return L"";

    return tagLine.substr(indexOf + tagFlag.length());
}

CNote & CNote::operator =(const CNote & src) {
    m_Name = src.getName();
    m_Tags = src.getTags();
    m_Category = src.getCategory();
    return *this;
}


