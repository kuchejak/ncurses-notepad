/**
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 14.05.20
 */

#include "CTextStorage.h"
#include "CConverter.h"
#include "CFile.h"

#include <ncurses.h>
#include <fstream>


CTextStorage::CTextStorage(int yDif, int xDif) : m_YDif(yDif), m_XDif(xDif), m_YOffset(0), m_XOffset(0) {
    m_Storage.emplace_back(); // initializing first line
    updateSize();
}


void CTextStorage::inputChar(wchar_t c, unsigned int curY, unsigned int curX) {
    updateSize();
    unsigned int line = m_YOffset + curY;
    unsigned int col = m_XOffset + curX;

    if (line >= m_Storage.size())
        m_Storage.emplace_back(1, c);
    else if (col <= m_Storage[line].size())
        m_Storage[line].insert(col, 1, c);
    else
        m_Storage[line] += c;
}

void CTextStorage::delChar(unsigned int curY, unsigned int curX) {
    updateSize();
    m_Storage[m_YOffset + curY].erase(m_XOffset + curX, 1);
}

void CTextStorage::insertLine(unsigned int y) {
    if (m_YOffset + y <= m_Storage.size())
        m_Storage.emplace(m_Storage.begin() + y + m_YOffset); // emplace requires iterator
    else
        m_Storage.emplace_back();
}

void CTextStorage::deleteLine(unsigned int y) {
    m_Storage.erase(m_Storage.begin() + y + m_YOffset); // erase requires iterator
}

const std::wstring & CTextStorage::scrollUp() {
    updateSize();
    m_YOffset -= 1;
    return m_Storage[m_YOffset];
}

const std::wstring & CTextStorage::scrollDown() {
    updateSize();
    m_YOffset += 1;
    if (m_Lines + m_YOffset < m_Storage.size())
        return m_Storage[m_Lines + m_YOffset];
    return m_Storage.back();
}

void CTextStorage::updateSize() const {
    m_Lines = LINES - m_YDif;
    m_Cols = COLS - m_XDif;
}

bool CTextStorage::canMoveRight(unsigned int curY, unsigned int curX) const {
    updateSize();
    return (curX + m_XOffset < m_Storage[m_YOffset + curY].length());
}

int CTextStorage::MoveUp(bool & redraw, unsigned int curY, unsigned int curX) {
    updateSize();
    redraw = false;
    if (curY == 0 && !canScrollUp())  // Cursor can not move up (window must be scrolled first)
        return -1;

    unsigned int size = m_Storage[m_YOffset + curY - 1].size();

    if (curY == 0 && canScrollUp()) {
        --m_YOffset;
        redraw = true;
    }
    return calculateLinePos(redraw, size, curX);
}

int CTextStorage::MoveDown(bool & redraw, unsigned int curY, unsigned int curX) {
    updateSize();
    redraw = false;
    if ((curY == m_Lines && !canScrollDown(false)) || m_YOffset + curY + 1 >= m_Storage.size())
        return -1;

    unsigned int size = m_Storage[m_YOffset + curY + 1].size();

    if (curY == m_Lines && canScrollDown(false)) {
        ++m_YOffset;
        redraw = true;
    }
    return calculateLinePos(redraw, size, curX);
}

bool CTextStorage::canScrollUp() const {
    updateSize();
    return m_YOffset; // if offset is 0, we can not move up (because first line is already dispalyed)
}

bool CTextStorage::canScrollDown(bool newLine) const {
    updateSize();
    return m_Storage.size() > m_YOffset + m_Lines + 1; // +1 because new line would be added (screen would scroll)
}

std::wstring CTextStorage::moveLineUp(unsigned int curY) {
    if (curY == 0 && !canScrollUp())
        throw std::logic_error("Can not move line UP, because there is no line above");

    m_Storage[m_YOffset + curY - 1] += m_Storage[m_YOffset + curY]; // segfault is prevented in previous if
    m_Storage.erase(m_Storage.begin() + m_YOffset + curY);

    if (curY == 0)
        return m_Storage[m_YOffset-- + curY - 1];
    return m_Storage[m_YOffset + curY - 1];
}

const std::wstring & CTextStorage::moveEndOfLineDown(unsigned int curY, unsigned int curX) {
    insertLine(curY + 1);

    auto & curLine = m_Storage[m_YOffset + curY];
    auto & nextLine = m_Storage[m_YOffset + curY + 1];

    nextLine = curLine.substr(m_XOffset + curX);
    curLine.erase(curLine.begin() + curX + m_XOffset, curLine.end());
    m_XOffset = 0; // screen will be moved to the left in CTextEditor
    return nextLine;
}

unsigned int CTextStorage::endOfCurLine(unsigned int curY) const {
    return m_Storage[m_YOffset + curY].size();
}

unsigned int CTextStorage::endOfPrevLine(unsigned int curY) const {
    return m_Storage[m_YOffset + curY - 1].size();
}

std::wstring CTextStorage::getLine(unsigned int y) const {
    if (m_YOffset + y >= m_Storage.size())
        return L"";

    return m_Storage[m_YOffset + y];
}

int CTextStorage::saveToFile(const std::string & fileName, const std::string & folderName) const {
    if (CFile::fileExist(folderName + '/' + fileName))
        return 2;

    return forceSaveToFile(fileName, folderName);
}

int CTextStorage::forceSaveToFile(const std::string & fileName, const std::string & folderName) const {
    std::ofstream out(folderName + "/" + fileName);
    if (!out.is_open())
        return 1;

    bool first = true;
    for (const auto & line : m_Storage) {
        if (!first)
            out << '\n';
        out << CConverter::toString(line);
        first = false;
    }
    out.close();
    return 0;
}

bool CTextStorage::load(const std::string & fileName) {
    std::ifstream in(fileName);
    if (!in.is_open())
        return false;
    m_Storage.clear(); // if m_Storage was already used, it will be cleared
    m_YOffset = 0;
    m_XOffset = 0;

    size_t i = 0;

    std::string temp;
    while (getline(in, temp)) {
        m_Storage.emplace_back();
        m_Storage[i++] = CConverter::toWString(temp);
    }

    in.close();
    return true;
}

unsigned int CTextStorage::getNumOfLines() const {
    return m_Storage.size();
}

const std::wstring & CTextStorage::getLastLine() const {
   return m_Storage.back();
}

bool CTextStorage::canScrollLineRight(unsigned int curY) const {
    return (m_XOffset + m_Cols <= m_Storage[m_YOffset + curY].length());
}

void CTextStorage::scrollRight() {
    ++m_XOffset;
}

std::vector<std::wstring> CTextStorage::getWindow() const {
    updateSize();
    size_t fin = ((m_Lines + m_YOffset) < m_Storage.size()) ? m_Lines + m_YOffset + 1 : m_Storage.size();
    std::vector<std::wstring> lines;
    for (size_t i = m_YOffset; i < fin; i++) {
        if (m_XOffset <= m_Storage[i].size())
            lines.emplace_back(m_Storage[i].substr(m_XOffset, m_Cols));
        else
            lines.emplace_back();
    }
    return lines;
}

bool CTextStorage::canScrollLeft() const {
    return m_XOffset > 0;
}

void CTextStorage::scrollLeft() {
    --m_XOffset;
}

unsigned int CTextStorage::calculateLinePos(bool & redraw, unsigned int size, unsigned int curX) {
    if (size < m_XOffset) {
        redraw = true; // window must be scrolled to the left
        m_XOffset = size >= m_Cols ? size - m_Cols : 0;
        return size;
    }

    if (size < curX + m_XOffset)
        return size - m_XOffset;
    return curX;
}

unsigned int CTextStorage::moveScreenToHorPos(unsigned int index) {
    m_XOffset = index - (m_Cols / 2);
    return (m_Cols / 2);
}

wchar_t CTextStorage::getChar(unsigned int y, unsigned int x, bool yToW, bool xToW) const {
    unsigned int yToS = yToW ? y + m_YOffset : y; // represent Y relative to storage
    unsigned int xToS = xToW ? x + m_XOffset : x; // represent X relative to storage

    if (yToS >= m_Storage.size())
        return L'\0';
    if (xToS >= m_Storage[yToS].size())
        return L'\0';

    return m_Storage[yToS][xToS];
}

bool CTextStorage::isOnScreen(unsigned int y, unsigned int x) const {
    if (y < m_YOffset || y > m_YOffset + LINES - m_YDif)
        return false;
    if (x < m_XOffset || x > m_XOffset + COLS - m_XDif)
        return false;
    return true;
}

unsigned int CTextStorage::convertGlobalY(unsigned int y) const {
    return y - m_YOffset;
}

unsigned int CTextStorage::convertGlobalX(unsigned int x) const {
    return x - m_XOffset;
}

unsigned int CTextStorage::convertScreenY(unsigned int y) const {
   return y + m_YOffset;
}

unsigned int CTextStorage::convertScreenX(unsigned int x) const {
    return x + m_XOffset;
}

