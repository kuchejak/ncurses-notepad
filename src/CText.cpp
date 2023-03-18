/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 07.05.20
 */

#include "CText.h"

void CText::setFormat(const CTextStorage & storage, CWindow & ediotorWin) const {}

std::string CText::getFileExt() const {
    return ".txt";
}

bool CText::needsColor() const {
    return false;
}
