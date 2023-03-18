/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 21.06.20
 */

#include "CUnsupportedInput.h"
#include <ncurses.h>

bool CUnsupportedInput::isSupported(wchar_t input) {
    switch (input) {
        case KEY_EXIT:
        case 27: // Esc
        case KEY_COMMAND:
        case KEY_RESIZE:
        case KEY_SEXIT:
        case KEY_HOME:
        case KEY_PPAGE:
        case KEY_PRINT:
        case KEY_PREVIOUS:
        case 331: // insert
        case 338: // pgDown
        case 360: // Exit
            return false;
        default:
            return true;
    }
}
