/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 26.05.20
 */

#pragma once

#include "CWindow.h"

/**
 * Special window used to get input from the user.
 */
class CInputWindow : public CWindow {
public:
    /**
     * Creates new window in the middle of the screen, setting prompt that will displayed to user (window is not shown)
     * @param prompt Prompt to user, that shoud be attached to window.
     * @param fileName If set to true, disables input of majority of special characters, that can cause problems with
     * file names (for example czech characters).
     */
    CInputWindow(const std::string & prompt, bool fileName = false);
    ~CInputWindow() = default;

    CInputWindow(const CInputWindow &) = delete;
    const CInputWindow & operator = (const CInputWindow &) = delete;

    /**
     * Changes displayed prompt
     * @param[in] prompt Prompt that should be displayed.
     */
    void changePrompt(const std::string & prompt);

    /**
     * Shows window to user, and reads input from him.
     * @return Users input.
     */
    std::wstring run();

    /**
     * Enables change of input mode.
     * @param fileName Whether or not input is filename.
     */
    void setFileName(bool fileName);


private:
    unsigned int m_XOffset;
    std::wstring m_UserInput;
    const unsigned int m_PromptLine = 1;
    const unsigned int m_DividerLine = 2;
    const unsigned int m_TextLine = 3;
    bool m_FileName;


    std::wstring userInput();
    void inputKeyAction(wint_t input);
    void rightKeyAction();
    void leftKeyAction();
    void moveTextRight();
    void moveTextLeft();
    void refreshText();
    std::wstring getCurText() const;
    void backspaceKeyAction();
    void resetInput();
};


