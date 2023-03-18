/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 31.05.20
 */

#pragma once

#include "CWindow.h"

/**
 * Simple prompt for user with given information (reads no input from user, just display information);
 */
class CInform : public CWindow {
public:
    /**
     * Creates new window in the middle of the screen, setting prompt that will displayed to user (window is not shown)
     * @param prompt Prompt to user, that shoud be attached to window.
     */
    CInform(const std::string & prompt);
    ~CInform() = default;

    CInform(const CInform &) = delete;
    const CInform & operator = (const CInform &) = delete;

    /**
     * Shows window to user, and reads input from him.
     * @return Users input.
     */
    void run();
};


