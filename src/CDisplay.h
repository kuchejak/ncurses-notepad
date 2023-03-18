/**
 * @author Jakub Kuchejda <kuchejak.fit.cvut.cz> 
 * @date 05.05.20
 */

#pragma once

#include <string>

/**
 * This class is used for initializing and ending Ncurses screen.
 */
class CDisplay {
public:
    CDisplay() = delete;
    ~CDisplay() = delete;
    CDisplay(const CDisplay &) = delete;
    CDisplay operator= (const CDisplay &) = delete;

    /**
     * Initializes the screen.
     */
    static void init();

    /**
     * Ends ncurses mode. (Must be called before end of the application, else terminal will not function properly.)
     */
    static void end();

    /**
     * Clears the entire screen.
     */
    static void clear();

    /**
     * Color pairs (only color of text is changed, background is always black).
     */
    enum ECol {
        White = 0, // Defined by default as 0
        Blue = 8, // to prevent overwriting of default colors
        Green,
        Orangered,
        Tomato,
        Coral,
        Darkorange,
        Orange,
        Gold,
        Purple,
        LightBlue,
        Gray
    };
private:
    static void setupColorPairs();
    static void createPair(short col, short R, short G, short B);
};



