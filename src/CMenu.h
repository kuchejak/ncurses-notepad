/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 05.05.20
 */

#pragma once

#include "CWindow.h"

#include <menu.h>
#include <vector>
#include <string>


/**
 * Ncurses menu handler.
 */
class CMenu {
public:
    CMenu(const std::vector<std::string> & menuItems, const std::vector<std::string> & descriptions = {},
          bool centreWindow = true);

    CMenu(const std::vector<std::wstring> & menuItems, const std::vector<std::string> & descriptions = {},
          bool centreWindow = true);
    ~CMenu();

    CMenu(const CMenu &) = delete;
    CMenu & operator=(const CMenu &) = delete;

    /**
     * Shows menu to the user and lets him choose one item.
     * @return Index of element, that was chosen by user.
     */
    int runOneChoice();

    /**
     * Shows menu to the user nad lets him choose multiple items.
     * @return Indexes of elements, that were chosen by the user in ascending order.
     */
    std::vector<unsigned int> runMultiChoice();

    /**
     * Hides the menu from screen.
     */
    void hideMenu();

    /**
     * Creates window in centre of the screen (with box around it) and attaches menu to it.
     */
    void attachToWindowInCentre();

private:
    // variables
    std::vector<std::string> m_Items;
    std::vector<std::string> m_Descriptions;
    bool m_HasDescriptions = false;
    CWindow * m_Window = nullptr;

    // ncurses
    ITEM ** m_MenuItems;
    MENU *  m_Menu;

    // functions
    void storeDescriptions(const std::vector<std::string> & menu_descriptions);
    void display();

    /**
     * Allows user to choose from items.
     * @param multichoice If true, user can pick more than one item)
     */
    void letUserChoose(bool multichoice = false);
    void refreshWindows();

    /**
     * Returns length of the longest line (item + description) in menu.
     */
    unsigned int getMenuWidth() const;

    /**
     * Setup menu (build's the menu as Ncurses demands). Basically a constructor (this function is called from both
     * constructors).
     * @param[in] descriptions Vector of descriptions.
     * @param[in] centreWindow Determines whether or not menu should be attached to window in the middle of the scr.
     */
    void setup(const std::vector<std::string> & descriptions, bool centreWindow);

    /**
     * @return Indexes of all selected items in multichoice menus.
     */
    std::vector<unsigned int> getSelected();

    /**
     * Called from setup(), stores descriptions to each item.
     */
    void setupMenuWithDesc();

    /**
     * Called from setup(), creates menu without description.
     */
    void setupMenuWithoutDesc();

    /**
     * @return Maximum width of menu to fit the screen.
     */
    unsigned int getMaxWidth() const;

    /**
     * Maximum widht of each line in menu to fit the screen.
     * @return
     */
    unsigned int getWidthForShortened() const;

    /**
     * @return How empty string item should be named in menu.
     */
    static std::string emptyItem();

    /**
     * Calculates length of longest string in given vector.
     * @param vector Vector of string.
     * @return length of longest string in vector.
     */
    static unsigned int getVectorWidth(const std::vector<std::string> & vector);
};


