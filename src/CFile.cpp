/** 
 * @author Jakub Kuchejda <kuchejak@fit.cvut.cz>
 * @date 31.05.20
 */

#include "CFile.h"
#include "CConverter.h"
#include <fstream>

bool CFile::copyFile(const std::string & from, const std::string & to) {
    // inspired by from https://stackoverflow.com/questions/10195343/copy-a-file-in-a-sane-safe-and-efficient-way
    std::ifstream src(from, std::ios::binary);
    if (!src.is_open())
        return false;

    std::ofstream dst(to, std::ios::binary);
    if (!dst.is_open()) {
        src.close();
        return false;
    }
    dst << src.rdbuf(); // the actual copying
    src.close();
    dst.close();
    return true;
}

std::wstring CFile::readLastLine(const std::string & file) {
    // inspired by http://www.programmersbook.com/page/7/Get-last-line-from-a-file-in-C/'
    std::ifstream in(file, std::ios_base::ate); // seeks to the end of the stream
    std::string lastLine;
    if (!in.is_open())
        return L"";

    int len = in.tellg(); // return "current position" (because of std::ios_base::ate "current position" is last position)

    for (int i = len - 2; i > 0; i--) { // -2 for EOF
        in.seekg(i);
        if (in.get() == '\n') // reading backwards until /n is found
            break;
    }
    std::getline(in, lastLine);
    return CConverter::toWString(lastLine);
}

bool CFile::fileExist(const std::string & name) {
        if (FILE *file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        } else {
            return false;
        }
}

bool CFile::textIsInFile(const std::string & text, const std::string & file) {
   std::ifstream in(file);
   if (!in.is_open())
       return false;

   std::string line;
   while (std::getline(in, line)) {
       if (line.find(text) != std::wstring::npos)  // if string was found
           return true;
   }
   return false;
}

