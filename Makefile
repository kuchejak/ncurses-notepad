#  V Makefile smí být použity pouze tyto programy: rm, gcc, g++, mkdir, doxygen, cp, mv, cd, ar, make.
#  make all nebo make vytvoří veškeré výstupy, generované ze zdrojového kódu.
#  make compile vytvoří binární podobu semestrální práce, výsledek bude uložen do souboru <login>/<login>.
#  make run spustí binární podobu semestrální práce.
#  make clean smaže všechny vygenerované soubory a uvede adresář <login> do původního stavu.
#  make doc vygeneruje dokumentaci do adresáře <login>/doc. Dokumentace může být vytvořená staticky ve formátu HTML (pak make doc nebude nic) nebo dynamicky generovaná programem doxygen (generována pouze v HTML).

CXX 		  = g++
CXXFLAGS	= -Wall -pedantic -Werror -g -std=c++11
LIBLINK 	= -lncursesw -lmenuw
APP_NAME 	= notepad
SRCDIR		= src
BUILDIR   = build
MKDIR 		= mkdir -p
DOCDIR 		= doc

.PHONY: all # just a command, does not create anything
all: compile

.PHONY: compile
compile: $(APP_NAME)

.PHONY: run
run: $(APP_NAME)
	./$(APP_NAME)

#$^ stands for all dependecies
$(APP_NAME): $(BUILDIR)/main.o $(BUILDIR)/CApplication.o $(BUILDIR)/CDisplay.o $(BUILDIR)/CMenu.o $(BUILDIR)/CWindow.o $(BUILDIR)/CFormat.o $(BUILDIR)/CMarkdown.o $(BUILDIR)/CText.o $(BUILDIR)/CTextEditor.o $(BUILDIR)/CTextStorage.o $(BUILDIR)/CInputWindow.o $(BUILDIR)/CNote.o $(BUILDIR)/CNoteStorage.o $(BUILDIR)/CConverter.o $(BUILDIR)/CFile.o $(BUILDIR)/CInform.o $(BUILDIR)/CUnsupportedInput.o
	$(CXX) $(CXXFLAGS) $^ $(LIBLINK) -o $@ -g

# src/%.cpp will be replaced by dependecies listed below
# $@ will be repalced by what should be created ($(BUILDIR)/%.o),
# $< first thing in the dependecies part (here the same thing as $^)
$(BUILDIR)/%.o: src/%.cpp
	$(MKDIR) $(BUILDIR)
	$(CXX) $(CXXFLAGS) $< $(LIBLINK) -c -o $@ -g

.PHONY: clean
clean:
	rm -rf $(APP_NAME) $(BUILDIR) $(DOCDIR) 2>/dev/null

doc: src/*.h README.md
	doxygen Doxyfile

valgrind: $(APP_NAME)
	valgrind --leak-check=full --show-leak-kinds=all --log-file=ValgrindLog ./$(APP_NAME)




#dependecies (g++ -MM src/* | sed 'sx^x$(BUILDIR)/xg' >> Makefile)
$(BUILDIR)/CApplication.o: src/CApplication.cpp src/CApplication.h src/CDisplay.h \
 src/CNoteStorage.h src/CNote.h src/CTextStorage.h src/CFormat.h \
 src/CWindow.h src/CMenu.h src/CTextEditor.h src/CText.h src/CMarkdown.h \
 src/CInputWindow.h src/CFile.h src/CConverter.h src/CInform.h
$(BUILDIR)/CApplication.o: src/CApplication.h src/CDisplay.h src/CNoteStorage.h \
 src/CNote.h src/CTextStorage.h src/CFormat.h src/CWindow.h
$(BUILDIR)/CConverter.o: src/CConverter.cpp src/CConverter.h
$(BUILDIR)/CConverter.o: src/CConverter.h
$(BUILDIR)/CDisplay.o: src/CDisplay.cpp src/CDisplay.h
$(BUILDIR)/CDisplay.o: src/CDisplay.h
$(BUILDIR)/CFile.o: src/CFile.cpp src/CFile.h src/CConverter.h
$(BUILDIR)/CFile.o: src/CFile.h
$(BUILDIR)/CFormat.o: src/CFormat.cpp src/CFormat.h src/CTextStorage.h src/CWindow.h
$(BUILDIR)/CFormat.o: src/CFormat.h src/CTextStorage.h src/CWindow.h
$(BUILDIR)/CInform.o: src/CInform.cpp src/CInform.h src/CWindow.h
$(BUILDIR)/CInform.o: src/CInform.h src/CWindow.h
$(BUILDIR)/CInputWindow.o: src/CInputWindow.cpp src/CInputWindow.h src/CWindow.h \
 src/CUnsupportedInput.h
$(BUILDIR)/CInputWindow.o: src/CInputWindow.h src/CWindow.h
$(BUILDIR)/CMarkdown.o: src/CMarkdown.cpp src/CMarkdown.h src/CFormat.h \
 src/CTextStorage.h src/CWindow.h src/CDisplay.h
$(BUILDIR)/CMarkdown.o: src/CMarkdown.h src/CFormat.h src/CTextStorage.h \
 src/CWindow.h
$(BUILDIR)/CMenu.o: src/CMenu.cpp src/CMenu.h src/CWindow.h src/CConverter.h
$(BUILDIR)/CMenu.o: src/CMenu.h src/CWindow.h
$(BUILDIR)/CNote.o: src/CNote.cpp src/CNote.h src/CTextStorage.h src/CFormat.h \
 src/CWindow.h src/CConverter.h
$(BUILDIR)/CNote.o: src/CNote.h src/CTextStorage.h src/CFormat.h src/CWindow.h
$(BUILDIR)/CNoteStorage.o: src/CNoteStorage.cpp src/CNoteStorage.h src/CNote.h \
 src/CTextStorage.h src/CFormat.h src/CWindow.h src/CConverter.h \
 src/CFile.h
$(BUILDIR)/CNoteStorage.o: src/CNoteStorage.h src/CNote.h src/CTextStorage.h \
 src/CFormat.h src/CWindow.h
$(BUILDIR)/CText.o: src/CText.cpp src/CText.h src/CFormat.h src/CTextStorage.h \
 src/CWindow.h
$(BUILDIR)/CTextEditor.o: src/CTextEditor.cpp src/CTextEditor.h src/CFormat.h \
 src/CTextStorage.h src/CWindow.h src/CDisplay.h src/CNoteStorage.h \
 src/CNote.h src/CText.h src/CInputWindow.h src/CMarkdown.h \
 src/CConverter.h src/CInform.h src/CUnsupportedInput.h
$(BUILDIR)/CTextEditor.o: src/CTextEditor.h src/CFormat.h src/CTextStorage.h \
 src/CWindow.h src/CDisplay.h src/CNoteStorage.h src/CNote.h src/CText.h
$(BUILDIR)/CText.o: src/CText.h src/CFormat.h src/CTextStorage.h src/CWindow.h
$(BUILDIR)/CTextStorage.o: src/CTextStorage.cpp src/CTextStorage.h src/CConverter.h \
 src/CFile.h
$(BUILDIR)/CTextStorage.o: src/CTextStorage.h
$(BUILDIR)/CUnsupportedInput.o: src/CUnsupportedInput.cpp src/CUnsupportedInput.h
$(BUILDIR)/CUnsupportedInput.o: src/CUnsupportedInput.h
$(BUILDIR)/CWindow.o: src/CWindow.cpp src/CWindow.h
$(BUILDIR)/CWindow.o: src/CWindow.h
$(BUILDIR)/main.o: src/main.cpp src/CApplication.h src/CDisplay.h src/CNoteStorage.h \
 src/CNote.h src/CTextStorage.h src/CFormat.h src/CWindow.h
