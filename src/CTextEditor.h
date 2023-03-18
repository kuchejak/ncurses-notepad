/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 07.05.20
 */

#pragma once

#include "CFormat.h"
#include "CDisplay.h"
#include "CTextStorage.h"
#include "CWindow.h"
#include "CNoteStorage.h"
#include "CText.h"

/**
 * Basic text editor. Createing
 */
class CTextEditor {
public:
    CTextEditor();
    ~CTextEditor() = default;
    CTextEditor(const CTextEditor &) = delete;
    CTextEditor & operator = (const CTextEditor &) = delete;

    /**
     * Starts text editor with given note.
     * @param[in] note Note which will be opened in text editor.
     * @param[in] folder Folder where note should be saved.
     * @return Created Cnote.
     */
    CNote run(const CNote & note, const std::string & folder);

    /**
     * Starts blank text editor.
     * @param[in] format Format of note that will be created in text editor.
     * @param[in] folder Folder where note should be saved.
     * @return Created CNote or "NULL" note = note with name "/", if user does not want to save changes.
     */
    CNote run(CFormat * format, const std::string & folder);

private:
    CTextStorage m_TxtStor;
    int m_ContHght; // height of control window
    CWindow m_EWin;
    CWindow m_ControlsWindow;
    const CNote * m_Note = nullptr; // stores pointer to existing note, when already created note is opened (will be used read-only)
    CFormat * m_Format = nullptr;

    /**
     * Displays control window.
     */
    void printControlWindow();
    void rightKeyAction();
    void leftKeyAction();
    void upKeyAction();
    void downKeyAction();
    void deleteKeyAction();
    void backspaceKeyAction();
    void enterKeyAction();

    /**
     * Action for any input key (letter, number,...)
     */
    void inputKeyAction(wint_t input);
    void tabKeyAction();
    CNote saveNewFile(const std::string & fileExt, const std::string & folder);
    CNote saveExistingFile(const std::string & folder);

    /**
     * Loads window content from storage.
     */
    void loadFromStorage();

    /**
     * Changes content of controls window to display info about saving.
     */
    void controlsForSaving();

    /**
     * Loads entire screen from storage. (Useful, when lot of lines change).
     */
    void redrawScreen();

    static void checkColors();
};


