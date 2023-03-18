/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 08.05.20
 */

#include "CWindow.h"
#include <ncurses.h>

CWindow::CWindow(unsigned int height, unsigned int width, unsigned int y, unsigned int x, bool drawBox)
                : m_Height(height), m_Width(width), m_HasBox(drawBox), m_LINES(LINES), m_COLS(COLS) {

    m_Window = newwin(m_Height, m_Width, y, x);
    keypad(m_Window, true);

    if (m_HasBox)
        redrawBox();

    scrollok(m_Window, true); // enables scrolling for window
}

CWindow::~CWindow() {
    eraseWindow();
    refreshWindow();
    delwin(m_Derwin);
    delwin(m_Window);
}

void CWindow::refreshWindow() {
    wrefresh(m_Window);
}

int CWindow::readChar() const {
    return wgetch(m_Window);
}

void CWindow::printText(const std::wstring & text, unsigned int y, unsigned int x) {
    saveCurPos();
    moveCur(y, x);
    unsigned int width = m_HasBox ? m_Width - 1 : m_Width;
    for (const auto & c  : text) {
        printChar(c);
        moveCur(EDirection::RIGHT);
        if (getCurX() >= width) {
            loadCurPos();
            return;
        }
    }
    loadCurPos();
}

void CWindow::printText(const std::string & text, unsigned int y, unsigned int x, int attr) {
    saveCurPos();
    moveCur(y, x);
    unsigned int width = m_HasBox ? m_Width - 1 : m_Width;
    for (const auto & letter : text) {
        printChar((wchar_t)letter, attr);
        moveCur(EDirection::RIGHT);
        if (getCurX() >= width) {
            loadCurPos();
            return;
        }
    }
    loadCurPos();
}

void CWindow::printChar(const wchar_t & c, int attr, short color) {
    cchar_t toPrint;
    setcchar(&toPrint, &c, attr, color, nullptr);
    wins_wch(m_Window, &toPrint);
}

void CWindow::printChar(const char & c) {
    winsch(m_Window, c);
}

void CWindow::eraseWindow() {
    saveCurPos();
    werase(m_Window);
    loadCurPos();
}

void CWindow::redrawBox() {
    box(m_Window, 0, 0);
}

void CWindow::moveCur(CWindow::EDirection direction, unsigned int distance) {
    unsigned int curY = getCurY();
    unsigned int curX = getCurX();

    switch (direction) {
        case EDirection::UP:
            wmove(m_Window, curY - distance < 0 ? 0 : curY - distance, curX);
            break;
        case EDirection::DOWN:
            wmove(m_Window, (unsigned) curY + distance > m_Height ? m_Height : curY + distance, curX);
            break;
        case EDirection::RIGHT:
            wmove(m_Window, curY, (unsigned) curX + distance > m_Width ? m_Width : curX + distance);
            break;
        case EDirection::LEFT:
            wmove(m_Window, curY, curX - distance < 0 ? 0 : curX - distance);
            break;
    }
}

void CWindow::moveCur(unsigned int y, unsigned int x) {
    wmove(m_Window, y, x);
}

void CWindow::deleteCurChar() {
    wdelch(m_Window);
}

unsigned int CWindow::getCurX() const {
    return getcurx(m_Window);
}

unsigned int CWindow::getCurY() const {
    return getcury(m_Window);
}

void CWindow::insertNewLine() {
    winsertln(m_Window);
}

void CWindow::scr(CWindow::EDirection direction) {
    switch (direction) {
        case EDirection::UP:
            wscrl(m_Window, -1);
            break;
        case EDirection::DOWN:
            wscrl(m_Window, 1);
            break;
        case EDirection::RIGHT:
        case EDirection::LEFT:
            break;
    }
}

void CWindow::deleteCurLine() {
    wdeleteln(m_Window);
}

void CWindow::eraseLine(unsigned int y) {
    saveCurPos();
    moveCur(y, 0);
    wclrtoeol(m_Window); // erase to end of line
    loadCurPos();
}

void CWindow::replaceLine(unsigned int y, const std::wstring & text) {
    eraseLine(y);
    printText(text, y, 0);
}

wint_t CWindow::readWch() const {
    wint_t input;
    wget_wch(m_Window, &input);
    return input;
}

void CWindow::printHLine(unsigned int y) {
    mvwhline(m_Window, y, 0, ACS_HLINE, m_Width);
}

void CWindow::saveCurPos() {
    m_tempCurPos = std::make_pair(getCurY(), getCurX());
}

void CWindow::loadCurPos() {
    moveCur(m_tempCurPos.first, m_tempCurPos.second);
}

unsigned int CWindow::getHeight() const {
    return m_Height;
}

unsigned int CWindow::getWidth() const {
    return m_Width;
}

void CWindow::setAtr(unsigned int y, unsigned int x, int attr) {
    saveCurPos();
    mvwchgat(m_Window, y, x, 1, attr, 0, NULL);
    loadCurPos();
}

void CWindow::addAtr(unsigned int y, unsigned int x, int attr) {
    saveCurPos();
    auto atributes = (mvwinch(m_Window, y, x) & A_ATTRIBUTES) | attr;   // & A_ATTRIBUTES is used to get only attributes
                                                                        // attributes are combined using logical OR
    mvwchgat(m_Window, y, x, 1, atributes, 0, NULL);
    loadCurPos();
}

void CWindow::setColor(unsigned int y, unsigned int x, int color) {
    saveCurPos();
    mvwchgat(m_Window, y, x, 1, 0, color, NULL);
    loadCurPos();
}

void CWindow::setLineColor(unsigned int y, int color) {
    saveCurPos();
    mvwchgat(m_Window, y, 0, m_Width, 0, color, NULL);
    loadCurPos();
}

void CWindow::setLineAttr(unsigned int y, int attr) {
    saveCurPos();
    mvwchgat(m_Window, y, 0, m_Width, attr, 0, NULL);
    loadCurPos();
}

void CWindow::setupMenu(MENU * menu, unsigned int height, unsigned int width) {
    set_menu_win(menu, m_Window);
    m_Derwin = derwin(m_Window, height - 2, width - 2, 1, 1);
    set_menu_sub(menu, m_Derwin); // creating subwindow for menu
    set_menu_format(menu, height - 2, 1); // menu will have height - 2 lines, and 1 column
}

