SRC= $(wildcard ./*.cpp)
OBJ= $(SRC:.cpp=.o)
INC= $(PWD)
NAME=mjolnir
TESTNAME=test
CXX=@clang++
RM=@rm -f
ECHO=@echo

CXXFLAGS=-O0 -pipe -g3 -Werror -MMD -pedantic -ansi -std=c++11 -Weverything
# CXXFLAGS=-O0 -pipe -g3 -Werror -MMD -pedantic -ansi -std=c++11 -Wall -Wextra -fno-builtin -Wstrict-overflow=5 -Wsign-conversion -Wshadow -Wuseless-cast -Wzero-as-null-pointer-constant -Wpadded -Wsign-conversion -Wsign-promo

TESTDIR=./tests/

TESTSRC=$(wildcard $(TESTDIR)*.cpp)
TESTOBJ=$(TESTSRC:.cpp=.o)

$(NAME): $(OBJ)
	@if [ -e $(NAME) ]; then \
		mv $(NAME) $(NAME).old;\
	fi
	$(ECHO) Creating $(NAME)
	$(CXX) $(OBJ) -o $(NAME) $(CXXFLAGS)
	$(ECHO) `wc -c $(NAME) | rev | sed s/[0-9][0-9][0-9]/\\\0./g | rev | sed s/\^\\\.//` `wc -c $(NAME).old  | rev | sed s/[0-9][0-9][0-9]/\\\0./g | rev | sed s/\^\\\.//`

all: $(NAME) $(TESTNAME)

$(TESTNAME): $(OBJ) $(TESTOBJ)
	@if [ -e $(TESTNAME) ]; then \
		mv $(TESTNAME) $(TESTNAME).old;\
	fi
	$(ECHO) Creating $(TESTNAME)
	$(CXX) $(OBJ) $(TESTOBJ) -o $(TESTNAME) $(CXXFLAGS)
	$(ECHO) `wc -c $(TESTNAME) | rev | sed s/[0-9][0-9][0-9]/\\\0./g | rev | sed s/\^\\\.//` `wc -c $(TESTNAME).old | rev | sed s/[0-9][0-9][0-9]/\\\0./g | rev | sed s/\^\\\.//`

.cpp.o: %.cpp
	$(ECHO) Compilling $<
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(foreach dir, $(INC), -I $(dir))


clean:
	$(ECHO) deleting temporary files
	$(RM) $(foreach dir, $(INC), $(dir)/*~)
	$(RM) $(TESTDIR)*~
	@if [ -e $(NAME) ]; then \
		mv $(NAME) $(NAME).old;\
	fi
	@if [ -e $(TESTNAME) ]; then \
		mv $(TESTNAME) $(TESTNAME).old;\
	fi


fclean: clean
	rm $(NAME).old
	$(ECHO) deleting object files
	$(RM) $(OBJ) main.o
	$(RM) $(TESTOBJ)
	$(ECHO) deleting dependencies files
	$(RM) $(SRC:.cpp=.d) main.d
	$(RM) $(TESTSRC:.cpp=.d)
	$(ECHO) deleting executables
	$(RM) $(TESTNAME)
	$(RM) $(NAME)

re:	fclean all

check:
	valgrind ../cppcheck/cppcheck  --verbose --enable=all --inconclusive $(SRC)
#../cppcheck/cppcheck  --enable=all main.cpp --inconclusive $(foreach file, $(SRC), --append=$(file))

-include $(SRC:.cpp=.d) main.d
-include $(TESTSRC:.cpp=.d)
