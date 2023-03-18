/**
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 03.05.2020
 */

#pragma once

#include "CDisplay.h"
#include "CNoteStorage.h"

/**
 * This class is a controller for the entire application, it starts all processes depending on users needs.
 */
class CApplication {
public:
    CApplication();
    ~CApplication();
    CApplication(const CApplication &) = delete;
    CApplication & operator = (const CApplication &) = delete;

    /**
     * Main menu of the application, can be used to start the application.
     * @return 0 if application encountered no errors (different number if not).
     */
     void run();

private:
    // variables
    CNoteStorage m_Storage;

    //functions
    void mainMenu();
    void newFileMenu();
    void search(char type);
    void searchByText(char type);
    void searchCategory(char type);
    void searchTag(char type);
    void exportNotes(std::vector<std::wstring> & notes);
    void fileImport();
    bool importNote(const std::wstring & fileW);
    void searchAll(char type);
    void chooseNotesToOpen(std::vector<std::wstring> & notes);
    void openNote(const std::wstring & noteFile);
};



