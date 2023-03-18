/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 31.05.20
 */

#include "CInform.h"

CInform::CInform(const std::string & prompt)
: CWindow(3, prompt.size() + 2, (LINES - 3) / 2, (COLS - prompt.size() - 2) / 2, true) {
   printText(prompt, 1, 1);
   redrawBox();
}

void CInform::run() {
   refreshWindow();
   readWch(); // wait for user to press any key
}
