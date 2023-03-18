/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 07.05.20
 */

#include "CTextEditor.h"
#include "CWindow.h"
#include "CInputWindow.h"
#include "CNote.h"
#include "CMarkdown.h"
#include "CText.h"
#include "CConverter.h"
#include "CInform.h"
#include "CUnsupportedInput.h"

#include <ncurses.h>


CTextEditor::CTextEditor() : m_TxtStor(4), m_ContHght(3), m_EWin(LINES - m_ContHght, COLS, 0, 0, false),
                                              m_ControlsWindow(m_ContHght, COLS, LINES - m_ContHght, 0, false)
                                              {}

CNote CTextEditor::run(const CNote & note, const std::string & folder) {
    if (!m_TxtStor.load(folder + '/' + CConverter::toString(note.getName())))
        return note;

    m_Note = &note;

    std::string noteExt = note.getFileExt();
    if (noteExt == CMarkdown().getFileExt()) {
        CMarkdown md;
        return run(&md, folder);
    }
    CText txt;
    return run(&txt, folder);
}

CNote CTextEditor::run(CFormat * format, const std::string & folder) {
    m_Format = format;
    printControlWindow();
    checkColors();
    loadFromStorage(); // Does not do anything if new note is being created.
                       // If already existing note is opened, it will print it's content to the scr.

    wint_t input = m_EWin.readWch(); // get_wch (ncurses function) returns wint_t instead of wchar_t, it is probably a bug.
    while (true) {
        switch (input) {
            case KEY_RIGHT:
            case 560: // ctrl + right
                rightKeyAction();
                break;
            case KEY_LEFT:
            case 545: // ctrl + left
                leftKeyAction();
                break;
            case KEY_UP:
            case 566: // ctrl + up
                upKeyAction();
                break;
            case KEY_DOWN:
            case 525: // ctrl + down
                downKeyAction();
                break;
            case KEY_DC: // Delete ketempCurPosy
                deleteKeyAction();
                break;
            case KEY_BACKSPACE:
            case 8: // ctrl + backspace
            case 127: // also possibly a backspace key
                backspaceKeyAction();
                break;
            case KEY_ENTER:
            case '\n':
            case '\r':
                enterKeyAction();
                break;
            case KEY_STAB:
            case '\t':
                tabKeyAction();
                break;
            case KEY_F(1):
                if (!m_Note)
                    return saveNewFile(format -> getFileExt(), folder);
                return saveExistingFile(folder);
            case KEY_F(2):
                return CNote(L"/"); // "Null" note (user can not create note with this name)
            default:
                if (CUnsupportedInput::isSupported(input))
                    inputKeyAction(input);
        }
        m_EWin.refreshWindow();
        input = m_EWin.readWch();
    }
}

void CTextEditor::printControlWindow() {
    m_ControlsWindow.printHLine(0);
    m_ControlsWindow.printText("f1 = SAVE & EXIT", 1, 1);
    m_ControlsWindow.printText("f2 = DISCARD & EXIT", 1, 20);
    m_ControlsWindow.printText("write !tags: <tags> on the last line to add tags (separated by spaces)", 2, 1);
    m_ControlsWindow.refreshWindow();
}

void CTextEditor::rightKeyAction() {
    if (m_EWin.getCurX() != m_EWin.getWidth() - 1 && m_TxtStor.canMoveRight(m_EWin.getCurY(), m_EWin.getCurX()))
        m_EWin.moveCur(CWindow::EDirection::RIGHT);
    else if (m_EWin.getCurX() == m_EWin.getWidth() - 1 && m_TxtStor.canScrollLineRight(m_EWin.getCurY())) {
        m_TxtStor.scrollRight();
        redrawScreen();
    }
}

void CTextEditor::leftKeyAction() {
    if (m_EWin.getCurX() > 0)
        m_EWin.moveCur(CWindow::EDirection::LEFT);
    else if (m_TxtStor.canScrollLeft()) {
       m_TxtStor.scrollLeft();
       redrawScreen();
    }
}

void CTextEditor::upKeyAction() {
    int x;
    bool redraw;
    if ((x = m_TxtStor.MoveUp(redraw, m_EWin.getCurY(), m_EWin.getCurX())) != -1) { // can move up on curr scr
        m_EWin.moveCur(m_EWin.getCurY() > 0 ? m_EWin.getCurY() - 1 : 0, x);
        if (redraw)
            redrawScreen();
    }
}

void CTextEditor::downKeyAction() {
    int x;
    bool redraw;
    if ((x = m_TxtStor.MoveDown(redraw, m_EWin.getCurY(), m_EWin.getCurX())) != -1) {
        m_EWin.moveCur(m_EWin.getCurY() < m_EWin.getHeight() - 2 ? m_EWin.getCurY() + 1 : m_EWin.getHeight() - 1, x);
        if (redraw)
            redrawScreen();
    }
}

void CTextEditor::deleteKeyAction() {
    m_EWin.deleteCurChar();
    m_TxtStor.delChar(m_EWin.getCurY(), m_EWin.getCurX());
    m_Format -> setFormat(m_TxtStor, m_EWin);
}

void CTextEditor::backspaceKeyAction() {
    if (m_EWin.getCurX() == 0 && !m_TxtStor.canScrollLeft()) { // cursor is in first column
        if (m_EWin.getCurY() == 0 && !m_TxtStor.canScrollUp()) // cursor is on first line, and can not move up
            return;
        m_EWin.deleteCurLine(); // deletes line from screen
        unsigned int end = m_TxtStor.endOfPrevLine(m_EWin.getCurY());
        const std::wstring & text = m_TxtStor.moveLineUp(m_EWin.getCurY()); // join line to line above
        if (end >= m_EWin.getWidth()) {
            m_EWin.moveCur(m_EWin.getCurY() > 0 ? m_EWin.getCurY() - 1 : 0, m_TxtStor.moveScreenToHorPos(end));
            redrawScreen();
        }
        else  {
            m_EWin.moveCur(m_EWin.getCurY() >= 1 ? m_EWin.getCurY() - 1 : 0, end);
            m_EWin.replaceLine(m_EWin.getCurY(), text);
            m_Format -> setFormat(m_TxtStor, m_EWin);
        }
    }
    else { // cursors is not in first column
        if (m_EWin.getCurX() == 0 && m_TxtStor.canScrollLeft()) { // can move left, but scr must be scrolled
            leftKeyAction(); // this will move screen left
        }
        else
            m_EWin.moveCur(CWindow::EDirection::LEFT);
        m_EWin.deleteCurChar();
        m_TxtStor.delChar(m_EWin.getCurY(), m_EWin.getCurX());
        m_Format -> setFormat(m_TxtStor, m_EWin);
    }
}

void CTextEditor::enterKeyAction() {
    m_TxtStor.moveEndOfLineDown(m_EWin.getCurY(), m_EWin.getCurX());
    if (m_EWin.getCurY() == (unsigned) m_EWin.getHeight() - 1) { // cursor is at the bottom of the window -> scrl window
        m_TxtStor.scrollDown();
        m_EWin.moveCur(m_EWin.getCurY(), 0);
    }
    else {
        m_EWin.moveCur(m_EWin.getCurY() + 1, 0);
    }
    redrawScreen();
}

void CTextEditor::inputKeyAction(wint_t input) {
    m_EWin.printChar((wchar_t) input, A_NORMAL, 0);
    m_TxtStor.inputChar((wchar_t) input, m_EWin.getCurY(), m_EWin.getCurX());
    if (m_EWin.getCurX() == m_EWin.getWidth() - 1) {
        rightKeyAction();
    }
    m_EWin.moveCur(CWindow::EDirection::RIGHT);
    m_Format -> setFormat(m_TxtStor, m_EWin);
}

void CTextEditor::tabKeyAction() {
    for (int i = 0; i < 4; i++) {
        m_EWin.printChar(' ');
        m_TxtStor.inputChar(' ', m_EWin.getCurY(), m_EWin.getCurX());
        m_EWin.moveCur(CWindow::EDirection::RIGHT);
    }
}

CNote CTextEditor::saveNewFile(const std::string & fileExt, const std::string & folder) {
    bool fileCreated = false;
    controlsForSaving();

    CInputWindow inputWindow("Enter file name (" + fileExt + " will be added automatically)", true);
    std::wstring fileName = inputWindow.run();
    while (true) {
        switch (m_TxtStor.saveToFile(CConverter::toString(fileName) + fileExt, folder)) {
            case 0: // file has been successfully created
                fileCreated = true;
                break;
            case 1:
                inputWindow.changePrompt("File could not be created, try different name:");
                fileName = inputWindow.run();
                break;
            case 2: // file cannot be created
                inputWindow.changePrompt("Note already exists, enter a different name:");
                fileName = inputWindow.run();
                break;
        }
        if (fileCreated)
            break;
    }

    CNote note(fileName + CConverter::toWString(fileExt));

    std::wstring category;
    inputWindow.changePrompt("Add category (leave blank for no category):");
    category = inputWindow.run();

    note.setCategory(category);
    note.setTags(m_TxtStor.getLastLine());

    return note;
}

CNote CTextEditor::saveExistingFile(const std::string & folder) {
    m_TxtStor.forceSaveToFile(CConverter::toString(m_Note -> getName()), folder);
    CNote note(*m_Note);
    note.setTags(m_TxtStor.getLastLine()); // tags could have been changed
    return note;
}

void CTextEditor::loadFromStorage() {
    redrawScreen();
    m_EWin.moveCur(0, 0);
    m_EWin.refreshWindow();
}

void CTextEditor::controlsForSaving() {
    m_ControlsWindow.eraseLine(1);
    m_ControlsWindow.eraseLine(2);
    m_ControlsWindow.printText("Enter = Confirm", 1, 1, A_NORMAL);
    m_ControlsWindow.refreshWindow();
}

void CTextEditor::redrawScreen() {
    m_EWin.eraseWindow();
    int i = 0;
    for (const auto & line : m_TxtStor.getWindow())
        m_EWin.printText(line, i++, 0);
    m_Format -> setFormat(m_TxtStor, m_EWin);
}

void CTextEditor::checkColors() {
    if (!has_colors() || !can_change_color())
        CInform("Your terminal does not support colors, formatting is turned off.").run();
}


