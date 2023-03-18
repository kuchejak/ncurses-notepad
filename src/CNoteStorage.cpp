/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 15.05.20
 */

#include "CNoteStorage.h"
#include "CConverter.h"
#include "CFile.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>

CNoteStorage::CNoteStorage() {
    // create folder
    std::string folderCmd = "mkdir -p " + m_Folder;
   system(folderCmd.c_str());
}

CNoteStorage::~CNoteStorage() {
    for (auto note : m_Notes)
        delete note;
}

void CNoteStorage::save() const {
    std::string filename = m_Folder + '/' + m_SaveFile;
    std::ofstream out(filename);
    if (!out.is_open()) {
        return;
    }

    for (const auto & note : m_Notes) {
        out << CConverter::toString(note -> getName() + L'|');
        out << CConverter::toString(note -> getCategory() + L'|');
        for (const auto & tag : note -> getTags())
            out << CConverter::toString(tag) << ' ';
        out << '\n';
    }
    out.close();
}

void CNoteStorage::load() {
    std::ifstream in(m_Folder + '/' + m_SaveFile);
    if(!in.is_open())
        return;

    std::string line;
    std::string name;
    while (getline(in, line)) {
        size_t nameEnd = line.find_first_of('|');
        size_t catEnd = line.find_first_of('|', nameEnd + 1);
        if (catEnd == std::wstring::npos) // line is not in correct format and it's content will not be added
            continue;
        name = line.substr(0, nameEnd);
        if (!CFile::fileExist(m_Folder + '/' + name))
            continue;
        auto note = new CNote(CConverter::toWString(name));
        note -> setCategory(CConverter::toWString(line.substr(nameEnd + 1, catEnd - nameEnd - 1)));
        note -> setTags(CConverter::toWString(line.substr(catEnd + 1)));
        addOrReplaceNote(note);
    }

    in.close();
}

void CNoteStorage::addOrReplaceNote(const CNote & note) {
    auto newNote = new CNote(note);
    addOrReplaceNote(newNote);
    save();
}

std::string CNoteStorage::getFolder() const {
    return m_Folder;
}

std::vector<std::wstring> CNoteStorage::getNotesWithCategory(const std::wstring & category) const {
    std::vector<std::wstring> notes;
    auto it = std::lower_bound(m_Categories.begin(), m_Categories.end(), category, compareCategoriesName);
    if (it == m_Categories.end() || it -> m_Name != category) // category was not found
        return notes;

    for (const auto & note : it -> m_Notes) {
        notes.push_back(note -> getName());
    }
    return notes;
}

std::vector<std::wstring> CNoteStorage::getNotesWithTag(const std::wstring & tag) const {
    std::vector<std::wstring> notes;
    auto it = std::lower_bound(m_Tags.begin(), m_Tags.end(), tag, compareTagsName);
    if (it == m_Tags.end() || it -> m_Name != tag) // tag was not found
        return notes;

    for (const auto & note : it -> m_Notes) {
       notes.push_back(note -> getName());
    }
    return notes;
}

std::vector<std::wstring> CNoteStorage::getNotesWithText(const std::wstring & textW) const {
    std::vector<std::wstring> notes;
    std::string text = CConverter::toString(textW);
    for (const auto & note : m_Notes) {
        if (note -> getName().find(textW) != std::wstring::npos) {
            notes.emplace_back(note -> getName());
            continue; // string was found in name, no need to search the file itself
        }
       if (CFile::textIsInFile(text, m_Folder + '/' + CConverter::toString(note -> getName()))) // search for text in file
           notes.emplace_back(note -> getName());
    }
    return notes;
}

std::vector<std::wstring> CNoteStorage::getAllNotes() const {
    std::vector<std::wstring> notes;
    for (const auto & note : m_Notes)
        notes.push_back(note -> getName());
    return notes;
}

void CNoteStorage::addOrReplaceNote(CNote * note) {
    auto it = std::lower_bound(m_Notes.begin(), m_Notes.end(), note, compareNotes);

    if (it == m_Notes.end() || (*it) -> getName() != note -> getName()) { // note is not already stored
        m_Notes.insert(it, note);
        addToCategories(note, note -> getCategory());
        addToTags(note, note -> getTags());
        return;
    }

    // note is already stored
    if ((*it) -> getCategory() != note -> getCategory())
        changeCategory((*it), (*it) -> getCategory(), note -> getCategory()); // if categories differ, update them
    if ((*it) -> getTags() != note -> getTags()) // if tags differ, update them
        changeTags((*it), (*it) -> getTags(), note -> getTags());

    *(*it) = *note; // old note is replaced by new note
    delete note;
}

void CNoteStorage::changeCategory(CNote * note, const std::wstring & oldCat, const std::wstring & newCat) {
    removeFromCategories(note, oldCat);
    addToCategories(note, newCat);
}

void CNoteStorage::addToCategories(CNote * note, const std::wstring & category) {
    auto itC = std::lower_bound(m_Categories.begin(), m_Categories.end(), category, compareCategoriesName);

    if (itC == m_Categories.end() || itC -> m_Name != category) { // category does not exist -> create it
        TCategory newCat;
        newCat.m_Name = category;
        newCat.m_Notes.push_back(note);
        m_Categories.insert(itC, newCat);
        return;
    }

    auto itN = std::lower_bound(itC -> m_Notes.begin(), itC -> m_Notes.end(), note, compareNotes);

    if (itN == itC -> m_Notes.end() || (*itN) -> getName() != note -> getName()) { // note is not in given category -> add it
        itC -> m_Notes.insert(itN, note);
        return;
    }
    // note is already in category -> do nothing
}

void CNoteStorage::removeFromCategories(CNote * note, const std::wstring & category) {
    auto itC = std::lower_bound(m_Categories.begin(), m_Categories.end(), category, compareCategoriesName);

    if (itC == m_Categories.end() || itC -> m_Name != category) { // category does not exist -> do nothing
        return;
    }

    auto itN = std::lower_bound(itC -> m_Notes.begin(), itC -> m_Notes.end(), note, compareNotes);

    if (itN == itC -> m_Notes.end() || (*itN) -> getName() != note -> getName()) { // note is not in given category -> do nothing
        return;
    }

    itC -> m_Notes.erase(itN);
    if (itC -> m_Notes.empty())
        m_Categories.erase(itC); // category is empty, erase it
}

void CNoteStorage::changeTags(CNote * note, const std::unordered_set<std::wstring> & oldTags,
                              const std::unordered_set<std::wstring> & newTags) {

    std::unordered_set<std::wstring> toRemove;
    std::unordered_set<std::wstring> toAdd;

    for (const auto & tag : oldTags) {
       if (newTags.find(tag) == newTags.end()) // find is on unordered_set is on average constant, so this is fairly efficient
           toRemove.insert(tag);
    }
    for (const auto & tag : newTags) {
        if (oldTags.find(tag) == oldTags.end())
            toAdd.insert(tag);
    }
    removeFromTags(note, toRemove);
    addToTags(note, toAdd);
}

void CNoteStorage::addToTags(CNote * note, const std::unordered_set<std::wstring> & tags) {
    for (const auto & tag : tags) {
        auto itT = std::lower_bound(m_Tags.begin(), m_Tags.end(), tag, compareTagsName);
        if (itT == m_Tags.end() || itT -> m_Name != tag) { // tag was not found -> add it
            TTag newTag;
            newTag.m_Name = tag;
            newTag.m_Notes.push_back(note);
            m_Tags.insert(itT, newTag);
            continue;
        }

        auto itN = std::lower_bound(itT -> m_Notes.begin(), itT -> m_Notes.end(), note, compareNotes);
        if (itN == itT -> m_Notes.end() || (*itN) -> getName() != note -> getName()) { // note is not in tag -> add it
           itT -> m_Notes.insert(itN, note);
        }
        // note is in tag -> don't do anything
    }
}

void CNoteStorage::removeFromTags(CNote * note, const std::unordered_set<std::wstring> & tags) {
    for (const auto & tag : tags) {
        auto itT = std::lower_bound(m_Tags.begin(), m_Tags.end(), tag, compareTagsName);
        if (itT == m_Tags.end() || itT -> m_Name != tag) { // tag was not found -> do nothing
            continue;
        }

        auto itN = std::lower_bound(itT -> m_Notes.begin(), itT -> m_Notes.end(), note, compareNotes);
        if (itN == itT -> m_Notes.end() || (*itN) -> getName() != note -> getName()) { // note is not in tag -> do nothing
            continue;
        }
        itT -> m_Notes.erase(itN);
        if (itT -> m_Notes.empty())
            m_Tags.erase(itT); // category is empty, erase it
    }
}

bool CNoteStorage::compareNotes(const CNote * a, const CNote * b) {
    return a -> getName() < b -> getName();
}

bool CNoteStorage::compareNotesName(const CNote * a, const std::wstring & b) {
    return a -> getName() < b;
}

bool CNoteStorage::compareCategoriesName(const TCategory & a, const std::wstring & b) {
    return a.m_Name < b;
}


bool CNoteStorage::compareTagsName(const TTag & a, const std::wstring & b) {
    return a.m_Name < b;
}

CNote CNoteStorage::getNote(const std::wstring & name) const {
    auto it = std::lower_bound(m_Notes.begin(), m_Notes.end(), name, compareNotesName);
    if (it == m_Notes.end() || (*it) -> getName() != name)
        throw std::invalid_argument("Note does not exist");
    CNote res(**it);
    return res;
}

std::vector<std::wstring> CNoteStorage::getCategories() const {
    std::vector<std::wstring> categories;
    categories.reserve(m_Categories.size());
    for (const auto & cat : m_Categories)
        categories.emplace_back(cat.m_Name);
    return categories;
}

std::vector<std::wstring> CNoteStorage::getTags() const {
    std::vector<std::wstring> tags;
    tags.reserve(m_Tags.size());
    for (const auto & tag : m_Tags)
        tags.emplace_back(tag.m_Name);
    return tags;
}
