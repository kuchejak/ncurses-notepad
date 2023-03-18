/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 15.05.20
 */

#pragma once

#include "CNote.h"
#include <string>
#include <vector>


/**
 * Handles notes/files (CNote), used to search files by Text/Category/Tags. It is also capable of saving info about files
 * to special file.
 */
class CNoteStorage {
public:
    CNoteStorage();
    ~CNoteStorage();
    CNoteStorage(const CNoteStorage &) = delete;
    CNoteStorage & operator = (const CNoteStorage &) = delete;

    /**
     * Saves information about files (tags, categories) to a file.
     */
    void save() const;

    /**
     * Loads information about files (tags, categories) from a file created by .save()
     */
    void load();

    /**
     * Adds new note to the storage. If note with the same name (therefore representing the same file) exists, stored note
     * is replaced by the new one.
     * @param[in] note Note to add.
     */
    void addOrReplaceNote(const CNote & note);

    /**
     * Return name of folder in which notes should be saved.
     */
    std::string getFolder() const;

    /**
     * @param[in] category
     * @return Names of all notes with given category, in alphabetical order.
     */
    std::vector<std::wstring> getNotesWithCategory(const std::wstring & category) const;

    /**
     * @param[in] tag
     * @return Names of all notes with given tag, in alphabetical order.
     */
    std::vector<std::wstring> getNotesWithTag(const std::wstring & tag) const;

    /**
     * @param[in] textW
     * @return All notes containing given text (also searches names of the notes).
     */
    std::vector<std::wstring> getNotesWithText(const std::wstring & textW) const;


    /**
     * @return Names of all stored notes.
     */
    std::vector<std::wstring> getAllNotes() const;

    /**
     * @param[in] name Name of note.
     * @return Note with given name.
     * @throw std::invalid_argument if given note does not exist
     */
    CNote getNote(const std::wstring & name) const;

    /**
     * @return All used categories.
     */
    std::vector<std::wstring> getCategories() const;

    /**
     * @return All used tags.
     */
    std::vector<std::wstring> getTags() const;

private:
    struct TCategory {
       std::wstring m_Name;
       std::vector<CNote *> m_Notes;
    };

    struct TTag {
        std::wstring m_Name;
        std::vector<CNote *> m_Notes;
    };

    std::string m_Folder = "Notes"; // since this is not users's input depended, wstring is not needed
    std::string m_SaveFile = "notepad.data"; // name of file, in which information about note's are stored. (created by save())
    std::vector<CNote *> m_Notes; // notes ordered by name
    std::vector<TCategory> m_Categories; // stores names of categories and pointers to all notes associated with them
    std::vector<TTag> m_Tags; // stores names of tags and pointers to all notes associated with them

    /**
     * Adds given note to storage. If note with given name already exists, it is replaced.
     * @param[in] note Note that should be added to storage.
     */
    void addOrReplaceNote(CNote * note);

    /**
     * Changes note category.
     * @param[in] note Note which category should be changed.
     * @param[in] oldCat Old note's category.
     * @param[in] newCat New note's category.
     */
    void changeCategory(CNote * note, const std::wstring & oldCat, const std::wstring & newCat);

    /**
     * Adds given note to m_Categories.
     * @param[in] note Note to add to m_Categories.
     * @param[in] category Category that should be associated with note.
     */
    void addToCategories(CNote * note, const std::wstring & category);

    /**
     * Removes given note from from given category.
     * @param[in] note Note which should be removed from category.
     * @param[in] category Category from which note should be removed.
     */
    void removeFromCategories(CNote * note, const std::wstring & category);

    /**
     * Changes tags associated with given note in m_Tags.
     * @param[in] note Note which tags should be changed.
     * @param[in] oldTags Tags that Note currently has.
     * @param[in] newTags Note's new tags.
     */
    void changeTags(CNote * note, const std::unordered_set<std::wstring> & oldTags, const std::unordered_set<std::wstring> & newTags);

    /**
     * Adds given note to m_Tags.
     * @param[in] note Note to add to m_Tags.
     * @param[in] tags All tags to which note should be added.
     */
    void addToTags(CNote * note, const std::unordered_set<std::wstring> & tags);

    /**
     * Removes given note from m_Tags.
     * @param[in] note Note to remove.
     * @param[in] tags Tags from which note should be removed.
     */
    void removeFromTags(CNote * note, const std::unordered_set<std::wstring> & tags);

    static bool compareNotes (const CNote * a, const CNote *  b);
    static bool compareNotesName (const CNote * a, const std::wstring & b);
    static bool compareCategoriesName (const TCategory & a, const std::wstring & b);
    static bool compareTagsName(const TTag & a, const std::wstring & b);
};



