/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 07.05.20
 */

#include "CMarkdown.h"
#include "CFormat.h"
#include "CTextStorage.h"
#include "CDisplay.h"

void CMarkdown::setFormat(const CTextStorage & storage, CWindow & window) const {
    if (!has_colors() || !can_change_color()) // terminal does not needed support colors, formatting is turned off.
        return;

    unsigned int winH = window.getHeight();
    for (unsigned int line = 0; line < winH; line++) {
        window.setLineColor(line, CDisplay::White); // reset format
        window.setLineAttr(line, A_NORMAL); // reset format
        if (!specialFormat(storage, window, line)) { // returns true when first set of chars determine line's formatting
            boldItalic(storage, window, line); // sets bold and italic formatting on given line
        }
    }
}

std::string CMarkdown::getFileExt() const {
    return ".md";
}

bool CMarkdown::specialFormat(const CTextStorage & storage, CWindow & window, unsigned int line) {
    // returns true when first few chars determines given line formatting (so that check for bold/italic text on given
    //line can be avoided.

    wchar_t first = storage.getChar(line, 0, true);
    wchar_t second = storage.getChar(line, 1, true);

    if ((first == L'*' || first == L'-') && second == L' ') { // Bullet Points
        return bulletList(storage, window, line);
    }
    if (first == L'#') { // Headings
        return heading(storage, window, line);
    }
    if (first == L'>' && second == L' ') { // Quotes
        return quote(storage, window, line);
    }
    if (first >= L'0' && first <= L'9') { // Numbered lists
        return numList(storage, window, line);
    }

    if (first == L'*' || first == L'_')
        return lineSep(storage, window, line);

    return false;
}

bool CMarkdown::heading(const CTextStorage & storage, CWindow & window, unsigned int line) {
    int level = 0;
    int column = 1;
    while (true) {
        wchar_t ch = storage.getChar(line, column++, true);
        if (level > 5) {
            break;
        }
        if (ch == L'#') {
            ++level;
            continue;
        }
        if (ch == L' ') {
            window.setLineColor(line, CDisplay::Orangered + level);
            return true;
        }
        break;
    }
    return false;
}

bool CMarkdown::numList(const CTextStorage & storage, CWindow & window, unsigned int line) {
    int column = 1;
    while (true) {
        wchar_t ch = storage.getChar(line, column++, true);
        if (ch >= L'0' && ch <= L'9') {
            continue;
        }
        if (ch == L'.') {
            ch = storage.getChar(line, column, true);
            if (ch == L' ')
                for (int i = 0; i <= column; ++i) {
                    if (storage.isOnScreen(storage.convertScreenY(line), i))
                        window.setColor(line, storage.convertGlobalX(i), CDisplay::LightBlue);
                }
        }
        break;
    }
    return false;
}

bool CMarkdown::quote(const CTextStorage & storage, CWindow & window, unsigned int line) {
    if (!storage.canScrollLeft()) {
        window.setColor(line, 0, CDisplay::Purple);
    }
    return false;
}

bool CMarkdown::bulletList(const CTextStorage & storage, CWindow & window, unsigned int line) {
    if (!storage.canScrollLeft()) {
        window.setColor(line, 0, CDisplay::Green);
    }
    return false;
}

void CMarkdown::boldItalic(const CTextStorage & storage, CWindow & window, unsigned int line) {
    const std::wstring & text = storage.getLine(line);
    if (text.size() < 3) // no need to do anything (no formatting will be applied)
        return;
    std::vector<TDivider> divs;
    fillDividers(divs, text);

    for (size_t i = 0; i < divs.size(); ++i) { // loop through dividers
        if (divs[i].type == EType::end)
            continue;
        for (size_t j = i + 1; j < divs.size(); ++j) { // find matching divider
            if (divMatch(divs[i], divs[j])) {
                applyBoldItalic(divs[i].idx, divs[j].idx + divs[j].cnt, divs[i].cnt, storage, window, line);
                divs.erase(divs.begin() + j);
                divs.erase(divs.begin() + i);
                i = -1;
                break;
            }
        }
    }
}

void CMarkdown::fillDividers(std::vector<TDivider> & dividers, const std::wstring & text) {
    const wchar_t d1 = L'*';
    const wchar_t d2 = L'_';
    const wchar_t space = L' ';
    auto max = text.size();
    bool lastIsEnd = true;

    size_t start = 1;
    EType curT = beg;

    if (text[0] == d1 || text[0] == d2) { // first character is divider
        if(text[0] == text[1]) { // first two characters are dividers
            start = 2;
            if ((text[2] != space || max <= 3)) {
                dividers.emplace_back(TDivider{text[0], 2, 0, curT});
            }
        }
        else if (text[1] != space) // only first one is divider
            dividers.emplace_back(TDivider{text[0], 1, 0, curT});
    }

    for (size_t i = start; i < max - 2; ++i) { // find all dividers and store their position
        lastIsEnd = false;
        curT = EType::mid;
        if (text[i - 1] != space)
            changeType(curT, true); // mid will be changed to end (this divider can be at the end of bold/italic text)
        if (text[i] == d1 || text[i] == d2) { // find divider
            if (text[i] == text[i + 1]) { // check if divider is 2 chars wide
                lastIsEnd = true;
                if (text[i + 2] != space) {
                    changeType(curT, false); // divider can also be mid/beg
                }
                dividers.emplace_back(TDivider{text[i], 2, i++, curT});
            }
            else {
                changeType(curT, text[i + 1] == space); // if next char is space, type will be increased (can
                                                                  // can be mid/end, if it is false it will be decreased
                                                                  // (can be beg/mid)
                dividers.emplace_back(TDivider{text[i], 1, i, curT});
            }
        }
    }

    curT = EType::end;
    if (!lastIsEnd) {
        if ((text[max - 2] == d1 || text[max - 2] == d2) && text[max - 3] != space) { // test for the last 2 chars
            if (text[max - 2] == text[max - 1])
                dividers.emplace_back(TDivider{text[max - 2], 2, max - 2, curT}); // both are dividers
            else
                dividers.emplace_back(TDivider{text[max - 2], 1, max - 2, curT}); // penultimate is divider
        }
        else if ((text[max - 1] == d1 || text[max - 1] == d2) && text[max - 2] != space)
            dividers.emplace_back(TDivider{text[max - 1], 1, max - 1, curT}); // last is divider
    }
    else if ((text[max - 1] == d1 || text[max - 1] == d2) && text[max - 2] != space)
        dividers.emplace_back(TDivider{text[max - 1], 1, max - 1, curT}); // last is divider
    else if ((text[max - 2] == d1 || text[max - 2] == d2) && text[max - 3] != space)
        dividers.emplace_back(TDivider{text[max - 2], 1, max - 2, curT}); // last is divider
}

bool CMarkdown::divMatch(CMarkdown::TDivider & a, CMarkdown::TDivider & b) {
   return b.type != EType::beg && a.div == b.div && a.cnt == b.cnt;
}

void CMarkdown::applyBoldItalic(size_t b, size_t e, int count, const CTextStorage & storage, CWindow & window,
                                unsigned int line) {
    auto attr = getFormat(count);
    for (size_t i = b; i < e; ++i) {
        if (storage.isOnScreen(storage.convertScreenY(line), i))
            window.addAtr(line, storage.convertGlobalX(i), attr);
    }
}

int CMarkdown::getFormat(int count) {
    switch (count) {
        case(1):
            return A_ITALIC;
        case(2):
            return A_BOLD;
        default:
            return A_NORMAL;
    }
}

void CMarkdown::changeType(EType & type, bool increase) {
    switch (type) {
        case (EType::beg):
            if (increase)
                type = EType::mid;
            return;
        case (EType::mid):
            if (increase)
                type = EType::end;
            else
                type = EType::beg;
            return;
        case (EType::end):
            if (!increase)
                type = EType::mid;
            return;
    }
}

bool CMarkdown::lineSep(const CTextStorage & storage, CWindow & window, unsigned int line) {
    int x = 0;
    int count = 1;
    wchar_t first = storage.getChar(line, x++, false);
    wchar_t next;
    while ((next = storage.getChar(line, x++, false))) {
        count++;
        if (next != first)
            return false;
    }
    if (count > 2) {
        window.setLineColor(line, CDisplay::Gray);
        return true;
    }
    return false;
}

bool CMarkdown::needsColor() const {
    return true;
}
