# C++ ncurses terminal notepad
## Description
This is a simple terminal note taking application written in C++ designed to work with plain text and markdown files. The application has been created as a semester project at Czech Technical University in Prague. 

## Requirements for compilation
- `ncurses` library 
- c++ compilation tool (make by default uses g++, but it can be changed in the Makefile)

## Compile/Run
- for compilation run `make`, which will create the application: `notepad` (name can be changed in Makefile)
- for compilation and/or run use `make run`

## Usage
The application contains a simple UI composed mainly of different menus. It includes a basic text editor that supports markdown formatting, such as text written between * being displayed in italics. Headings are displayed in color, as terminal display does not allow for changing of font size. Users can assign categories (via UI) or tags (by typing "!tags: a b c" on the last line, where "a," "b," and "c" will be assigned as tags) to created notes. The application also allows users to search for notes based on text, categories, and tags (via UI).

Notes can be saved as classic files in a folder that the application creates. The simplest form of export is to move/copy the desired file from this folder. However, the application also allows for export based on categories, tags, or text using the UI (implemented as a multichoice menu). The application copies the selected files to CWD (the folder in which the application executable file is located). Importing is also possible via UI from CWD (so you can add categories to your files and open the files in the app).

## Showcase
### Main menu
![MainMenu](https://user-images.githubusercontent.com/94016085/226111946-038ef46a-fa00-4f25-a878-93d00f5fcd10.png)

### Markdown formatting
![Markdown](https://user-images.githubusercontent.com/94016085/226111956-20b9d5b1-1d65-40ce-9f94-76e26041ebd3.png)

### Search
![Search](https://user-images.githubusercontent.com/94016085/226111966-54222a8a-b074-43ee-8546-fb512bece5a6.png)

## Memory leaks, Valgrind
The Ncurses library causes a lot of memory problems, mainly with memory leaks and uninitialized internal variables. All available functions to release used memory resources are used in the program. To run the program with Valgrind, use the following command: "valgrind --leak-check=full --show-leak-kinds=all --log-file=ValgrindLog ./notepad" (= make valgrind) (the output is redirected to the file ValgrindLog, because if the application is interrupted, the ncurses terminal mode may not be properly terminated, which could lead to various problems - the terminal would probably need to be restarted). 

From https://invisible-island.net/ncurses/ncurses.faq.html#config_leaks
>    Perhaps you used a tool such as dmalloc or valgrind to check for memory leaks. It will normally report 
>    a lot of memory still in use. That is normal.
>    ...
>    Any implementation of curses must not free the memory associated with a screen, since (even after calling endwin()),
>    it must be available for use in the next call to refresh(). There are also chunks of memory held for performance reasons...
