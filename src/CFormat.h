/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 07.05.20
 */

#pragma once
#include "CTextStorage.h"
#include "CWindow.h"
#include <ncurses.h>
#include <string>

/**
 * This abstract class represents format of text, for example Markdown.
 */
class CFormat {
public:
    CFormat() = default;
    virtual ~CFormat() = default;

    /**
     * This function is used to set formatting in the editor window. It needs CTextStorage to determine which format to use
     * and window to set the format (because more characters can be affected at once). This function DOES NOT change text
     * displayed in the given window, but will change it's formatting (for example set some characters to be bold, or change
     * their color). If param storage does not correspond to param editorWin, the behaviour is undefined.
     * @param[in] storage TextStorage corresponding to given editorWindow.
     * @param[in, out] ediotorWin Window corresponding to given TextStorage.
     */
   virtual void setFormat(const CTextStorage & storage, CWindow & ediotorWin) const = 0;

   /**
    * Returns suffix for given format. For example markdown format will return .md *
    * @return File suffix associated with format.
    */
   virtual std::string getFileExt() const = 0;

   /**
    * Returns true if given format is dependent of terminal's ability to display colors. When colors are need, and terminal
    * does not support them, formatting must be turned off, because color formatting may cause text to became invisible.
    * @return True if given format is dependent of terminal's ability to display colors, False if not.
    */
   virtual bool needsColor() const = 0;
};


