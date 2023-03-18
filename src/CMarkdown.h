/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 07.05.20
 */

#pragma once
#include "CFormat.h"
#include "CTextStorage.h"

/**
 * Markdown CFormat.
 */
class CMarkdown : public CFormat {
public:
    CMarkdown() = default;
    ~CMarkdown() override = default;
    CMarkdown(const CMarkdown &) =  delete;
    CMarkdown & operator = (const CMarkdown &) = delete;

    void setFormat(const CTextStorage & storage, CWindow & window) const override;
    std::string getFileExt() const override;
    bool needsColor() const override;

private:
    enum EType {
        beg = 0,
        mid,
        end
    };
    struct TDivider {
        wchar_t div; // char representing divider
        int cnt; // how many of them are in row (in markdown it is only 1 or 2)
        size_t idx;
        EType type; // 0 = divider is a beginning of format block, 1 = divider can be both, 2 = divider is end of block
    };

    /**
     * Determines if line has special format.
     * @return True if line has special format and no other formatting should be applied.
     */
    static bool specialFormat(const CTextStorage & storage, CWindow & window, unsigned int line);

    // all these functions are part of special format
    static bool heading(const CTextStorage & storage, CWindow & window, unsigned int line);
    static bool numList(const CTextStorage & storage, CWindow & window, unsigned int line);
    static bool quote(const CTextStorage & storage, CWindow & window, unsigned int line);
    static bool bulletList(const CTextStorage & storage, CWindow & window, unsigned int line);
    static bool lineSep(const CTextStorage & storage, CWindow & window, unsigned int line);

    /**
     * Applies bold and italic formatting.
     */
    static void boldItalic(const CTextStorage & storage, CWindow & window, unsigned int line);

    /**
     * Finds all dividers (for bold and italic formatting) on line.
     * @param[in, out] dividers Vector of TDividers, that will be filled.
     * @param[in] text Line on which dividers should be searched.
     */
    static void fillDividers(std::vector<TDivider> & dividers, const std::wstring & text);

    /**
     * Determines if two dividers match.
     * @param[in] a Beginning divider.
     * @param[in] b Ending divider.
     * @return True if dividers match.
     */
    static bool divMatch(TDivider & a, TDivider & b);

    /**
     * Applies bold and italic formatting.
     * @param b Beginning divider.
     * @param e Ending divider.
     * @param count Dividers count (* or _ = 1, ** or __ = 2)
     */
    static void
    applyBoldItalic(size_t b, size_t e, int count, const CTextStorage & storage, CWindow & window,
                    unsigned int line);

    /**
     * @param[in] count Dividers count (* or _ = 1, ** or __ = 2)
     * @return Formatting that should be applied (as defined by ncurses)
     */
    static int getFormat(int count);

    /**
     * "Increases" or "decreases" given type, in order beg -> mid -> end
     * @param[in,out] type Type that should be changed.
     * @param[in] increase Whether or not type should be increased (if false, it will be decreased)
     */
    static void changeType(EType & type, bool increase);
};


