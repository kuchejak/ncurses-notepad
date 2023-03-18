/**
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 05.05.20
 */

#include "CMenu.h"
#include "CConverter.h"

#include <string>

CMenu::CMenu(const std::vector<std::string> & menuItems, const std::vector<std::string> & descriptions,
             bool centreWindow)
             : m_Items(menuItems) {
    setup(descriptions, centreWindow);
}


CMenu::CMenu(const std::vector<std::wstring> & menuItems, const std::vector<std::string> & descriptions,
             bool centreWindow)
             {
    m_Items.reserve(menuItems.size());
    for (const auto & item : menuItems)
        m_Items.push_back(CConverter::toString(item));
    setup(descriptions, centreWindow);
}

CMenu::~CMenu() {
    for (size_t i = 0; i < m_Items.size() + 1; i++) {
        free_item(m_MenuItems[i]);
    }
    free_menu(m_Menu);

    for (size_t i = 0; i < m_Items.size() + 1; i++) {
        free(m_MenuItems[i]); // free_item() does not really free the item.
        // This "second" free must be called after free_menu.
    }
    delete[] m_MenuItems;
    delete m_Window;
}

void CMenu::storeDescriptions(const std::vector<std::string> & menu_descriptions) {
    m_HasDescriptions = true;
    m_Descriptions = menu_descriptions;

    for (auto & desc : m_Descriptions){
        desc = std::string(10, ' ').append(desc); // adding spaces to the beginning of the description (looks better)
    }

    // blank descriptions are added to the m_Descriptions
    if (m_Descriptions.size() < m_Items.size()) {
        for (size_t i = m_Descriptions.size(); i < m_Items.size(); i++) {
            m_Descriptions.emplace_back();
        }
    }
}

int CMenu::runOneChoice() {
    curs_set(0); // hides cursor
    display();
    letUserChoose(false);
    curs_set(1); // shows cursor
    hideMenu();
    return current_item(m_Menu) -> index;
}

std::vector<unsigned int> CMenu::runMultiChoice() {
    CWindow controlsWindow(2, COLS, LINES - 2, 0);
    controlsWindow.printText("Use Space to select items");
    controlsWindow.printText("Use Enter to confirm", 1);
    controlsWindow.refreshWindow();
    curs_set(0);
    display();
    letUserChoose(true);
    curs_set(1);
    hideMenu();
    return getSelected();
}

void CMenu::hideMenu() {
    unpost_menu(m_Menu);
    if (m_Window)
        m_Window -> eraseWindow();
    refreshWindows();
}

void CMenu::display() {
    if (m_Window)
        m_Window -> redrawBox();
    post_menu(m_Menu);
    refreshWindows();
}

void CMenu::letUserChoose(bool multichoice) {
    int userInput;
    while ((userInput = (m_Window != nullptr) ? m_Window -> readChar() : getch())) {
        switch(userInput) {
            case KEY_UP:
            case 'k': // for vim lovers
                menu_driver(m_Menu, REQ_UP_ITEM);
                break;
            case KEY_DOWN:
            case 'j':
                menu_driver(m_Menu, REQ_DOWN_ITEM);
                break;
            case ' ': // space
                if (multichoice)
                    menu_driver(m_Menu, REQ_TOGGLE_ITEM);
                break;
            case KEY_RIGHT:
            case 'l':
            case KEY_ENTER: // probably an enter on numeric keyboard
            case '\n': // enter on regular keyboard
            case '\r': // also possibly an enter on regular keyboard
                unpost_menu(m_Menu);
                return;
            default:
                break;
        }
        refreshWindows();
    }
}

void CMenu::attachToWindowInCentre() {
    unsigned int width = getMenuWidth() + 2; // 2 for box around window
    unsigned int height = 10; // maximum columns of the window + 2 for box
    m_Window = new CWindow(height, width, (LINES - height) / 2, (COLS - width) / 2, true);
    m_Window -> setupMenu(m_Menu, height, width);
}

void CMenu::refreshWindows() {
    if (m_Window)
        m_Window -> refreshWindow();
    else
        refresh();
}

unsigned int CMenu::getMenuWidth() const {
    unsigned int descLen = 0;
    unsigned int itemLen = 0;

    if (m_HasDescriptions) {
        descLen = getVectorWidth(m_Descriptions);
    }
    itemLen = getVectorWidth(m_Items);
    return itemLen + descLen + 1; // +1 because ncurses automatically add 1 space divider
}

void CMenu::setup(const std::vector<std::string> & descriptions, bool centreWindow) {
    if (!descriptions.empty())
        storeDescriptions(descriptions);

    // creating menu items
    m_MenuItems = new ITEM*[m_Items.size() + 1];
    if (m_HasDescriptions) {
        setupMenuWithDesc();
    }
    else {
        setupMenuWithoutDesc();
    }
    m_MenuItems[m_Items.size()] = nullptr;

    // creating the actual menu
    m_Menu = new_menu(m_MenuItems);
    set_menu_mark(m_Menu, ""); // disables '-' displayed by default next to chosen item
    menu_opts_off(m_Menu, O_ONEVALUE); // allows selection of multiple items

    if (centreWindow)
        attachToWindowInCentre();
}

std::vector<unsigned int> CMenu::getSelected() {
    std::vector<unsigned int> choices;
    for (int i = 0; i < item_count(m_Menu); ++i) {
       if (item_value(m_MenuItems[i]) == TRUE)  { // item is selected
           choices.push_back(i);
       }
    }
    if (choices.empty())
        choices.push_back(current_item(m_Menu) -> index); // if user did not select anything, return current item
    return choices;
}

void CMenu::setupMenuWithDesc() {
    for (size_t i = 0; i < m_Items.size(); i++) {
        if (m_Items[i].empty()) // empty string would break menu
            m_Items[i] = emptyItem();
        if (m_Items[i].size() + m_Descriptions[i].size() > getMaxWidth()) { // Too long
            m_Descriptions[i] = ""; // no description will be shown
            if (m_Items[i].size() > getMaxWidth()) // Too long to display
                m_Items[i] = m_Items[i].substr(0, getWidthForShortened()) + "...";
        }
        m_MenuItems[i] = new_item(m_Items[i].c_str(), m_Descriptions[i].c_str()); // creates new choice in m_Menu
    }
}

void CMenu::setupMenuWithoutDesc() {
    for (size_t i = 0; i < m_Items.size(); i++) {
        if (m_Items[i].empty()) // empty string would break menu
            m_Items[i] = emptyItem();
        if(m_Items[i].size() > getMaxWidth()) // Too long to display
            m_Items[i] = m_Items[i].substr(0, getWidthForShortened()) + "...";
        m_MenuItems[i] = new_item(m_Items[i].c_str(), "");
    }
}

unsigned int CMenu::getMaxWidth() const {
    return m_Window ? m_Window -> getWidth() - 4 : COLS - 4;
}

unsigned int CMenu::getWidthForShortened() const {
   return m_Window ? m_Window -> getWidth() - 8 : COLS - 8;
}

std::string CMenu::emptyItem() {
    return "None";
}

unsigned int CMenu::getVectorWidth(const std::vector<std::string> & vector) {
    // return length of the longest item
    int curLen = 0;
    int longest = 0;
    for (const auto & item : vector) {
        curLen = item.length();
        if (curLen > longest)
            longest = curLen;
    }
    return longest;
}
