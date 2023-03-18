/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 08.05.20
 */

#pragma once

#include <ncurses.h>
#include <string>
#include <menu.h>


/**
 * Ncurses window made into class.
 */
class CWindow {
public:
    /**
     * Creates window with given parameters
     * @param[in] height Height of window.
     * @param[in] width Width of window.
     * @param[in] y Y coordinate of upper left-hand corner of the window
     * @param[in] x X coordinate of upper left-hand corner of the window
     * @param[in] drawBox Should box be drawn around the window?
     */
    CWindow(unsigned int height, unsigned int width, unsigned int y = 0, unsigned int x = 0, bool drawBox = false);
    ~CWindow();
    CWindow(const CWindow &) = delete ;
    CWindow & operator = (const CWindow &) = delete;

    /**
     * Calls wrefres on m_Window
     */
    void refreshWindow();

    /**
     * Reads one characer from user.
     * @return Character from user (ncurses represents characters as integers).
     */
    int readChar() const;

    /**
     * Print given wstring to given coordinates in m_Window.
     * @param[in] text WString to print.
     * @param[in] y Y coordinate (in m_Window)
     * @param[in] x X coordinate (in m_WIndows)
     */
    void printText(const std::wstring & text, unsigned int y = 0, unsigned int x = 0);

    /**
     * Print given string to given coordinates in m_Window. If text is too long for window, it will be cut off.
     * @param[in] text String to print.
     * @param[in] y Y coordinate (in m_Window)
     * @param[in] x X coordinate (in m_WIndows)
     */
    void printText(const std::string & text, unsigned int y = 0, unsigned int x = 0, int attr = A_NORMAL);

     /**
      * Prints given wchar_t to window with given attributes/colors.
      * @param[in] c Wchar_t to print.
      * @param[in] attr Attribute to use.
      * @param[in] color Color to use.
      */
    void printChar(const wchar_t & c, int attr = A_NORMAL, short color = 0);

    /**
     * Prints given char to window with given attributes/colors.
     * @param[in] c Char to print.
     */
    void printChar(const char & c);

    /**
     * Erases the window (fills it with spaces), erases box as well - can be restored using redrawBox()
     */
    void eraseWindow();

    /**
     * Draws box around window
     */
    void redrawBox();

    enum class EDirection{UP, DOWN, RIGHT, LEFT};

    /**
     * Moves cursor in given direction by given distance. If distance is too big, cursor will be moved to as far as
     * possible (to be still inside the window). For example moveCur(up, INT_MAX) will move cursor to first line.
     * @param[in] direction Which direction should cursor move.
     * @param[in] distance How much should cursor move.
     */
    void moveCur(EDirection direction, unsigned int distance = 1);

    /**
     * Moves cursor to given coordinates.
     * @param[in] y Desired cursor Y coordinate.
     * @param[in] x Desired cursor X coordinate.
     */
    void moveCur(unsigned int y, unsigned int x);

    /**
     * Scrolls window in given dirrecition.
     * @param[in] direction Direction window should be scrolled to.
     */
    void scr(EDirection direction);

    /**
     * Deletes char cursor is currently on.
     */
    void deleteCurChar();

    /**
     * Getter for cursor X coordinate.
     * @return Cursor X coordinate.
     */
    unsigned int getCurX() const;

    /**
     * Getter for cursor Y coordinate.
     * @return Cursor Y coordinate.
     */
    unsigned int getCurY() const;

    /*
     * Inserts new line on current cursor Y coordinate.
     */
    void insertNewLine();

    /**
     * Deletes line on current cursor Y coordinate
     */
    void deleteCurLine();

    /**
     * Erases y-th line in the window (counting from 0), will erase part of box as well (use redrawBox() to redraw it).
     * Does note change cursors position.
     * @param[in] y Position of line that should be erased on the screen.
     */
    void eraseLine(unsigned int y);

    /**
     * Replaces text of given line with give text
     * @param[in] y Line to replace.
     * @param[in] text Text to print.
     */
    void replaceLine(unsigned int y, const std::wstring & text);

    /**
     * Reads input from user (one wide char)
     * @return Input from user.
     */
    wint_t readWch() const;

    /**
     * Prints horizontal line at given line, will erase part of box as well (use redrawBox() to redraw it).
     * @param y Line in window, where horizontal line should be printed.
     */
    void printHLine(unsigned int y);

    /**
     * @return Height of window, including box lines (if box is used).
     */
    unsigned int getHeight() const;

    /**
     * @return Width of window, including box lines (if box is used).
     */
    unsigned int getWidth() const;

    /**
     * Sets given attribute to given position in the window. (Any already used attributes will be replaced)
     * @param[in] y Y coordinate.
     * @param[in] x X coordinate.
     * @param attr Attribute to set (as defined by ncurses).
     */
    void setAtr(unsigned int y, unsigned int x, int attr);

    /**
     * Adds given attribute to given position in the window (New attribute will be combined with already used ones)
     * @param[in] y Y coordinate.
     * @param[in] x X coordinate.
     * @param attr Attribute to add (as defined by ncurses).
     */
    void addAtr(unsigned int y, unsigned int x, int attr);

    /**
     * Sets given color to given position in the window.
     * @param[in] y Y coordinate.
     * @param[in] x X coordinate.
     * @param color Color pair that should be applied (as specified in CDisplay).
     */
    void setColor(unsigned int y, unsigned int x, int color) ;

    /**
     * Sets given color to the entire line.
     * @param y Line to which color should be set.
     * @param color Color pair that should be applied (as specified in CDisplay).
     */
    void setLineColor(unsigned int y, int color);

    /**
     * Sets given attribute to the entire line.
     * @param y Line to which attribute should be set.
     * @param attr Attribute that should be applied (as specified by ncurses).
     */
    void setLineAttr(unsigned int y, int attr);

    /**
     * This method attaches menu to window, using ncurses functions.
     * @param menu Menu as defined by ncurses.
     * @param height Menu height.
     * @param width Menu width.
     */
    void setupMenu(MENU * menu, unsigned int height, unsigned int width);

protected:
    // for basic windows initialization
    WINDOW * m_Window;
    WINDOW * m_Derwin = nullptr; // window derived from main window (used in menu)
    unsigned int m_Height;
    unsigned int m_Width;
    bool m_HasBox;
    std::pair<int, int> m_tempCurPos = std::make_pair(0, 0); // used to store cur pos in saveCurPos()

    // for resizing
    int m_LINES;
    int m_COLS;

    /**
     * Saves current cursor position.
     */
    void saveCurPos();
    /**
     * Loads stored cursor position.
     */
    void loadCurPos();
};


