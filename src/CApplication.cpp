/**
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 03.05.2020
 */

#include "CApplication.h"
#include "CMenu.h"
#include "CTextEditor.h"
#include "CMarkdown.h"
#include "CText.h"
#include "CInputWindow.h"
#include "CFile.h"
#include "CConverter.h"
#include "CInform.h"


CApplication::CApplication() {
    m_Storage.load();
}

CApplication::~CApplication() {
    CDisplay::end();
}

void CApplication::run() {
    CDisplay::init();
    mainMenu();
}

void CApplication::mainMenu() {
    CMenu mainMenu({"New", "Open", "Import", "Export", "Exit"},
                   {"Create new note", "Open existing note", "Import notes from CWD", "Export notes to CWD", ""});

    while (true) {
        int userChoice = mainMenu.runOneChoice();
        switch (userChoice) {
            case 0: // New
                newFileMenu();
                break;
            case 1: // Open
                search('o');
                break;
            case 2: // Import
                fileImport();
                break;
            case 3: // Export
                search('e');
                break;
            case 4: // Exit
                m_Storage.save();
                return;
        }
    }
}

void CApplication::newFileMenu() {
    CMenu newFileMenu({"Text file", "Markdown", "Back"},
                      {".txt file", ".md file", ""});

    int userChoice;
    while (true) {
        userChoice = newFileMenu.runOneChoice();
        switch (userChoice) {
            case 0: { // Text file
                CText txt;
                auto note = CTextEditor().run(&txt, m_Storage.getFolder());
                if (note.getName() != L"/") // if note was create, "/" represents "NULL" note
                    m_Storage.addOrReplaceNote(note);
                break;
            }
            case 1: { // Markdown
                CMarkdown md;
                auto note = CTextEditor().run(&md, m_Storage.getFolder());
                if (note.getName() != L"/") // if note was create, "/" represents "NULL" note
                    m_Storage.addOrReplaceNote(note);
                break;
            }
            case 2: // Back
                return;
        }
    }
}

void CApplication::search(char type) {
    CMenu typeMenu({"Text", "Category", "Tag", "Show all", "Back"},
                   {"Search notes containing given text", "Search notes with category", "Search notes with tag", "Choose from all notes"});
    int userChoice;
    while (true) {
        userChoice = typeMenu.runOneChoice();
        switch (userChoice) {
            case 0: // Text
                searchByText(type);
                break;
            case 1: // Category
                searchCategory(type);
                break;
            case 2: // Tag
                searchTag(type);
                break;
            case 3: // Show all
                searchAll(type);
                break;
            case 4: // back
                return;
        }
    }
}

void CApplication::searchByText(char type) {
    std::wstring text = CInputWindow("Enter searched text:").run();
    std::vector<std::wstring> notes = m_Storage.getNotesWithText(text);
    if (type == 'e')
        exportNotes(notes);
    if (type == 'o')
        chooseNotesToOpen(notes);
}

void CApplication::searchCategory(char type) {
    std::vector<std::wstring> categories = m_Storage.getCategories();
    std::vector<std::wstring> notes;
    categories.emplace_back(L"Back");
    CMenu catMenu(categories);
    unsigned int choice;
    while ((choice = catMenu.runOneChoice()) != categories.size() - 1) { // while not "back"
        notes = m_Storage.getNotesWithCategory(categories[choice]);
        if (type == 'e')
            exportNotes(notes);
        else if (type == 'o')
            chooseNotesToOpen(notes);
    }
}

void CApplication::searchTag(char type) {
    std::vector<std::wstring> tags = m_Storage.getTags();
    std::vector<std::wstring> notes;
    tags.emplace_back(L"Back");
    CMenu tagMenu(tags);
    unsigned int choice;
    while ((choice = tagMenu.runOneChoice()) != tags.size() - 1) { // while not "back"
        notes = m_Storage.getNotesWithTag(tags[choice]);
        if (type == 'e')
            exportNotes(notes);
        else if (type == 'o')
            chooseNotesToOpen(notes);
    }
}

void CApplication::exportNotes(std::vector<std::wstring> & notes) {
    notes.emplace_back(L"Back");
    CMenu noteMenu(notes);
    std::vector<unsigned int> selected = noteMenu.runMultiChoice();
    if (selected.back() == notes.size() - 1) // back was selected
        return;

    std::string noteName;
    for (int i : selected) {
        noteName = CConverter::toString(notes[i]);
        if (!CFile::copyFile(m_Storage.getFolder() + '/' + noteName, noteName)) {
            CInform("Failed to export the file " + noteName).run();
        }
    }
}

void CApplication::fileImport() {
   CInputWindow userInput("Enter file, that should be imported");
   std::wstring input = userInput.run();
   size_t index = input.find_last_of('.');
   if (index == std::wstring::npos) {
       CInform("Filename is missing extension.").run();
       return;
   }
   std::wstring ext = input.substr(input.find_last_of(L'.'));
   if (ext.empty() || (ext != L".txt" && ext != L".md")) {
       CInform("Unsupported file type.").run();
       return;
   }
   if (!CFile::fileExist(CConverter::toString(input))) {
       CInform("File does not exist.").run();
       return;
   }
   if (!importNote(input)) {
       CInform("File could not be imported.").run();
       return;
   }
   CNote note(input);

   // get category
   userInput.changePrompt("Enter imported file category");
   input = userInput.run();
   note.setCategory(input);

   // get tags
   note.setTags(CFile::readLastLine(m_Storage.getFolder() + '/' + CConverter::toString(note.getName())));
   m_Storage.addOrReplaceNote(note);
}

bool CApplication::importNote(const std::wstring & fileW) {
    std::string file = CConverter::toString(fileW);
    return CFile::copyFile(file, m_Storage.getFolder() + '/' + file);
}

void CApplication::searchAll(char type) {
    std::vector<std::wstring> notes = m_Storage.getAllNotes();
    if (type == 'e')
        exportNotes(notes);
    else if (type == 'o')
        chooseNotesToOpen(notes);
}

void CApplication::chooseNotesToOpen(std::vector<std::wstring> & notes) {
    notes.emplace_back(L"Back");
    CMenu NotesMenu(notes);
    while (true) {
        unsigned int choice = NotesMenu.runOneChoice();
        if (choice == notes.size() - 1) // exit
            return;
        openNote(notes[choice]);
    }
}

void CApplication::openNote(const std::wstring & noteFile) {
    CNote note = m_Storage.getNote(noteFile);
    CNote result = CTextEditor().run(note, m_Storage.getFolder());
    if (result.getName() != L"/")
        m_Storage.addOrReplaceNote(result);
}

