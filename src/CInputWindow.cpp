/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 26.05.20
 */

#include "CInputWindow.h"
#include "CUnsupportedInput.h"


CInputWindow::CInputWindow(const std::string & prompt, bool fileName)
        : CWindow(5, prompt.size() + 2, (LINES - 5) / 2, (COLS - prompt.size() - 2) / 2, true),
          m_XOffset(0), m_FileName(fileName) {
    printText(prompt, m_PromptLine, 1); // 1, 1 because of box
    printHLine(m_DividerLine);
    redrawBox();
}

void CInputWindow::changePrompt(const std::string & prompt) {
    eraseLine(m_PromptLine);
    m_XOffset = 0;
    redrawBox(); // erase line erases box as well
    printText(prompt, m_PromptLine, 1, A_NORMAL); // 1, 1 because of box
}

std::wstring CInputWindow::run() {
    resetInput();
    moveCur(m_TextLine, 1);
    refreshWindow();
    return userInput();
}

std::wstring CInputWindow::userInput() {
    auto input = readWch();
    while (input != KEY_ENTER && input != '\r' && input != '\n') {
        if (m_FileName && ((input >= L'A' && input <= L'Z') || (input >= L'a' && input <= L'z')
                            || (input >= L'0' && input <= L'9') || input == L'_')) {
            inputKeyAction(input);
        }
        else {
            switch (input) {
                case KEY_RIGHT:
                    rightKeyAction();
                    break;
                case KEY_LEFT:
                    leftKeyAction();
                    break;
                case KEY_BACKSPACE:
                case 8: // ctrl + backspace
                case 127: // also possibly a backspace key
                    backspaceKeyAction();
                    break;
                case KEY_UP:
                case KEY_DOWN:
                case '|':
                    break;
                default:
                    if (!m_FileName && CUnsupportedInput::isSupported(input))
                        inputKeyAction(input);
            }
        }
        input = readWch();
        refreshWindow();
    }
    return m_UserInput;
}

void CInputWindow::inputKeyAction(wint_t input) {
    printChar((wchar_t) input, A_NORMAL, 0);
    m_UserInput.insert(m_XOffset + getCurX() - 1, 1, input);
    if (getCurX() != m_Width - 2)
        moveCur(CWindow::EDirection::RIGHT);
    else {
        moveTextRight();
    }
    redrawBox();
}

void CInputWindow::rightKeyAction() {
    if (getCurX() != m_Width - 2 && getCurX() + m_XOffset <= m_UserInput.length()) { // if cursor is not at the end of displayed line
        moveCur(getCurY(), getCurX() + 1);
    }
    else if (m_XOffset + m_Width - 2 + 1 <= m_UserInput.length()) { // + 1 because we want to move right, without adding new char
        moveTextRight();
    }
}

void CInputWindow::leftKeyAction() {
    if (getCurX() != 1) { // if cursor is not at the beginning of line
        moveCur(getCurY(), getCurX() - 1);
    }
    else if (m_XOffset != 0) {
        moveTextLeft();
    }
}

void CInputWindow::moveTextRight() {
    m_XOffset += 1;
    refreshText();
}

void CInputWindow::moveTextLeft() {
    m_XOffset -= 1;
    refreshText();
}

void CInputWindow::refreshText() {
    eraseLine(m_TextLine);
    printText(getCurText(), m_TextLine, 1);
    redrawBox();
}

std::wstring CInputWindow::getCurText() const {
    return m_UserInput.substr(m_XOffset, m_Width - 2);
}

void CInputWindow::backspaceKeyAction() {
    unsigned int curX = getCurX();
    unsigned int curY = getCurY();

    if (getCurX() != 1) {
        moveCur(m_TextLine, m_Width - 1);
        printChar(' ');
        moveCur(curY, curX);

        moveCur(EDirection::LEFT);
        deleteCurChar();

        redrawBox();
        m_UserInput.erase(getCurX() - 1 + m_XOffset, 1);
    }
}

void CInputWindow::resetInput() {
    m_UserInput = L"";
    eraseLine(m_TextLine);
    redrawBox();
}

void CInputWindow::setFileName(bool fileName) {
    m_FileName = fileName;
}
