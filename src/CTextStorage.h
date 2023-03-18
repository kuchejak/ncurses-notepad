/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 14.05.20
 */

#pragma once

#include <string>
#include <vector>
#include <codecvt>
#include <locale>

/**
 * Stores text written into CTextEditor (Ncurses store only currently displayed text)
 */
class CTextStorage {
public:
    /**
     * Creates new CTextStorage.
     * @param yDif[in] How many lines are NOT used by the text editor.
     * @param xDif[in] How many columns are NOT used by the text editor.
     */
    CTextStorage(int yDif = 0, int xDif = 0);
    CTextStorage(const CTextStorage &) = delete; // may be implemented later on
    CTextStorage & operator = (const CTextStorage &) = delete; // may be implemented later on

    /**
     * Stores given char. If there are other characters to the right of current cursor positions, they are moved to the
     * right. (Char at current position is not overwritten.)
     * @param[in] c Char typed in text editor.
     * @param[in] curY Cursor Y coordinate.
     * @param[in] curX Cursor X coordinate.
     */
    void inputChar(wchar_t c, unsigned int curY, unsigned int curX);

    /**
     * Deletes given char. If there are other characters to the right of current cursor positions, they are moved to the
     * left.
     * @param[in] c Char typed in text editor.
     * @param[in] curX Cursor X coordinate.
     * @param[in] curY Cursor Y coordinate.
     */
    void delChar(unsigned int curY, unsigned int curX);

    /**
     * Inserts new blank line at given Y position on screen, moving all following lines down.
     * @param[in] y Line after which newline should be included (counted from 0).
     */
    void insertLine(unsigned int y);

    /**
     * Deletes line at given Y position on screen.
     * @param[in] y Line to delete (counted from 0)
     */
    void deleteLine(unsigned int y);

    /**
     * Represents text editor scrolling up - first line is taken from storage, last line is hidden. canScrollUp()
     * should usually be called first.
     * @return New first line.
     */
    const std::wstring & scrollUp();

    /**
     * Represents text editor scrolling down - first line is hidden, last line is taken from storage.
     * @param[in] curX Cursor X coordinate.
     * @param[in] curY Cursor Y coordinate.
     * @return New last line.
     */
    const std::wstring & scrollDown();

    /**
     * Determines whether or not a cursor can move to right. (Cursor can not move on "void" - place without text)
     * @param[in] curY Cursor Y coordinate.
     * @param[in] curX Cursor X coordinate.
     * @return True if yes, False if no.
     */
    bool canMoveRight(unsigned int curY, unsigned int curX) const;

    /**
     * Determines whether or not a cursor can move up, and if yes, where.
     * @param[in,out] redraw If the window should be redrawn, because movement require scrolling left/right, this param
     * will be set to true, else it will be set to false. If redraw is set to true, representation of displayed text inside
     * CTextStorage will be changed accordingly (that's why this function is not const) - editor window MUST be redrawn.
     * @param[in] curY Cursor Y coordinate.
     * @param[in] curX Cursor X coordinate.
     * @return X coordinate of cursor on screen after movement or -1 if cursor can not move up on current screen.
     */
    int MoveUp(bool & redraw, unsigned int curY, unsigned int curX);

     /**
     * Determines whether or not a cursor can move down. (Cursor can not move on "void" - place without text)
     * @param[in,out] redraw If the window should be redrawn, because movement require scrolling left/right, this param
     * will be set to true, else it will be set to false. If redraw is set to true, representation of displayed text inside
     * CTextStorage will be changed accordingly (that's why this function is not const) - editor window MUST be redrawn.
     * @param[in] curY Cursor Y coordinate.
     * @param[in] curX Cursor X coordinate.
     * @return X coordinate of cursor on screen after movement or -1 if cursor can not move down on current screen.
     */
     int MoveDown(bool & redraw, unsigned int curY, unsigned int curX);

    /**
     * @return True if screen can be scrolled up, false otherwise
     */
    bool canScrollUp() const;

     /**
      * @param newLine Determines if this method should count with the fact, that newline will be added after scrolling
      * (for example when pressing enter). Default value is true.
      * @return True if screen can be scrolled down, false otherwise
      */
    bool canScrollDown(bool newLine = true) const;

    /**
     * Erases line at cursors position and moves it's content up (joining it to the line above).
     * @return New line formed by joining two lines together. Empty string if scrolling up would be required by it is not
     * possible.
     * @throw std::logical_error if there is no line above (can be prevented, by checking that cursor's Y coord
     * is not 0, or if it is 0, that screen can scroll up.
     */
    std::wstring moveLineUp(unsigned int curY);

    /**
     * Erases line from cursors position to the end of current line and moves it's content down (creates new line below current line).
     * @param[in] curY Cursor Y coordinate on screen.
     * @param[in] curX Cursor X coordinate on screen.
     * @return New line content.
     */
    const std::wstring & moveEndOfLineDown(unsigned int curY, unsigned int curX);

    /**
     * @param[in] curY Cursor Y coordinate.
     * @return Index of last letter on current line + 1
     */
    unsigned int endOfCurLine(unsigned int curY) const;

    /**
     * @param[in] curY Cursor Y coordinate. Behaviour is undefined when there is no previous line.
     * @return Index of last letter on prev line + 1. This may be larger then screen (so scrolling of display may be needed).
     */
    unsigned int endOfPrevLine(unsigned int curY) const;

    /**
     * Returns given line, that should be displayed on screen.
     * @param[in] y Line on screen that should be returned.
     * @return Text of given line.
     */
    std::wstring getLine(unsigned int y) const;

    /**
     * Attempts to save file with given name.
     * @param[in] fileName Name of file/note (with extension)
     * @param[in] folderName Name of folder in which note should be saved. Default value is "" (CWD)
     * @return 0 if saving was successful, 1 if file can not be created, 2 if file already exists,,
     */
    int saveToFile(const std::string & fileName, const std::string & folderName = "") const;

    /**
     * Saves file even if file with given name already exists.
     * @param[in] fileName Name of file/note (with extension)
     * @return 0 if saving was successful, 1 if file can not be created
     */
    int forceSaveToFile(const std::string & fileName, const std::string & folderName) const;

    /**
     * Clears the storage and load note from given file. If loading content from the file fails, storage is not cleared.
     * @param[in] fileName Name of file from which content should be loaded.
     * @return True if file was successfully loaded, false otherwise.
     */
    bool load(const std::string & fileName);

    /**
     * @return Number of lines stored.
     */
    unsigned int getNumOfLines() const;

    /**
     * @return Last line.
     */
    const std::wstring & getLastLine() const;

    /**
     * Checks if text can be scrolled to the right (current line continue to the right).
     * @param[in] curY Y coordinate of cursor.
     * @return True if screen can be scrolled to the right, false if not.
     */
    bool canScrollLineRight(unsigned int curY) const;

    /**
     * Moves screen representation inside CTextStorage to the right. Use .getWindow() to print new screen.
     */
    void scrollRight();

    /**
     * Returns all lines in current window (this is useful when lot's of changes are done at the same time - for example
     * enter key can scroll the entire screen to the left by many columns at once). Printing the entire output of this
     * function will synchronize displayed text with m_Storage. Does NOT return blank lines at the end, so erasing screen
     * before hand is a good idea.
     * @return All ines in current window.
     */
    std::vector<std::wstring> getWindow() const;

    /**
     * @return True if screen be scrolled left (one column);
     */
    bool canScrollLeft() const;

    /**
     * Inner representation of text will move to the left -> redrawing by using .getWindow() is needed.
     */
    void scrollLeft();

    /**
     * Adjusts inner representation of text horizontally, so that given index is roughly in the middle of the screen
     * This function must should be called, when moving screen to given position makes sense, otherwise many problems
     * may occur.
     * @param[in] index Index of the character, that should be in the ~middle of the screen.
     * @return Horizontal position of given index in inner representation of editor screen.
     */
    unsigned int moveScreenToHorPos(unsigned int index);

    /**
     * Returns char at given postition. If given coordinates are out of range, empty char is returned.
     * @param[in] y Y position of character, that should be returned.
     * @param[in] x X postiion of char, that should be returned.
     * @param[in] yToW If true, Y will be represented as relative to window. Default value is false.
     * @param[in] xToW If true, X will be represented as relative to window. Default value is false.
     * @return Char at given position, or empty char if given position is not valid.
     */
    wchar_t getChar(unsigned int y, unsigned int x, bool yToW = false, bool xToW = false) const;

    /**
     * Determines whether or not is given position (in "global" coordinates) visible on screen.
     * @param[in] y Y "Global" coordinate.
     * @param[in] x X "Global" coordinate.
     * @return True if given position is visible, false otherwise.
     */
    bool isOnScreen(unsigned int y, unsigned int x) const;

    /**
     * Converts given "global" y coordinate to screen coordinate. If given position is not on screen, the behaviour is
     * undefined, so isOnScreen should be called first.
     * @param[in] y Y "Global" coordinate.
     * @return Screen equivalent of given "global" y coordinate.
     */
    unsigned int convertGlobalY(unsigned int y) const;

    /**
     * Converts given "global" x coordinate to screen coordinate. If given position is not on screen, the behaviour is
     * undefined, so isOnScreen should be called first.
     * @param[in] x X "Global" coordinate.
     * @return Screen equivalent of given "global" x coordinate.
     */
    unsigned int convertGlobalX(unsigned int x) const;

    /**
     * Converts given screen y coordinate to "global" coordinate. If given position is not on screen, the behaviour is
     * undefined.
     * @param[in] y Y Screen coordinate.
     * @return "Global" equivalent of given screen y coordinate.
     */
    unsigned int convertScreenY(unsigned int y) const;

    /**
     * Converts given screen x coordinate to "global" coordinate. If given position is not on screen, the behaviour is
     * undefined.
     * @param[in] x X Screen coordinate.
     * @return "Global" equivalent of given screen x coordinate.
     */
    unsigned int convertScreenX(unsigned int x) const;

private:
    std::vector<std::wstring> m_Storage; // stores lines
    unsigned int m_YDif; // stores how many lines on screen are NOT used by the editor
    unsigned int m_XDif;
    unsigned int m_YOffset; // represents, how much vertical scrolling has been done
    unsigned int m_XOffset; // represents, how much horizontal scrolling has been done
    mutable unsigned int m_Lines; // stores how many lines editor has (must be updated before use - screen res can change)
    mutable unsigned int m_Cols; // stores how many columns editor has (must be updated before use - screen res can change)


    /**
     * For resizing. Not implemented.
     */
    void updateSize() const;

    /**
     * Calculates cursor position on line, after movement up or down.
     * @param[in, out]redraw Determines, whether or not window should be scrolled after movement.
     * This method expect, that redraw may have been previously set for some reason, therefore it will only
     * change redraw to true (changing it to false should be done before the function is called).
     * @param[in] size Length of line.
     * @param[in] curX Cursor X pos.
     * @return Position of cursor on new line.
     */
    unsigned int calculateLinePos(bool & redraw, unsigned int size, unsigned int curX);
};


