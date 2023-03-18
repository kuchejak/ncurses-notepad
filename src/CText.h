/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 07.05.20
 */

#pragma once
#include "CFormat.h"
#include "CTextStorage.h"


/**
 * Plain text (no format) CFormat.
 */
class CText : public CFormat {
public:
    CText() = default;
    ~CText() = default;
    CText(const CText &) = delete;
    CText & operator = (const CText &) = delete;

    void setFormat(const CTextStorage & storage, CWindow & ediotorWin) const override;
    std::string getFileExt() const override;
    bool needsColor() const override;
};


