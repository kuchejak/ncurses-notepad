/**
 * @author Jakub Kuchejda <kuchejak.fit.cvut.cz> 
 * @date 05.05.2020
 */


#include "CDisplay.h"
#include <ncurses.h>


void CDisplay::init() {
    initscr(); // inicializes ncurses screen called stdscr
    start_color(); // enables colors displaying on the screen
    keypad(stdscr, true); // activates keypad input, stdscr = basic window, created by initscr
    raw(); // disable input buffering and some unwanted inputs, like ctrl + C
    noecho(); // user input won't be shown on the screen
    setlocale(LC_ALL, ""); // sets locales according to system
    setupColorPairs();
}

void CDisplay::end() {
    endwin(); // ends ncurses screen
}

void CDisplay::clear() {
    erase();
    refresh();
}

void CDisplay::setupColorPairs() {
    // white is defined by default
    createPair(ECol::Blue, 0, 599, 999);
    createPair(ECol::Green, 19, 980, 274);
    createPair(ECol::Orangered, 999, 150, 0);
    createPair(ECol::Tomato, 999, 388, 278);
    createPair(ECol::Coral, 999, 497, 313);
    createPair(ECol::Darkorange, 999, 548, 0);
    createPair(ECol::Orange, 999, 646, 0);
    createPair(ECol::Gold, 999, 743, 0);
    createPair(ECol::Purple, 799, 0, 999);
    createPair(ECol::LightBlue, 7, 960, 831);
    createPair(ECol::Gray, 500, 500, 500);
}

void CDisplay::createPair(short col, short R, short G, short B) {
    auto bColor = COLOR_BLACK; // background color
    init_color(col, R, G, B);
    init_pair(col, col, bColor);
}

