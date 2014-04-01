SRC= $(wildcard ./*.cpp)
OBJ= $(SRC:.cpp=.o)
INC= $(PWD)
NAME=mjolnir
TESTNAME=test
CXX=@g++
RM=@rm -f
ECHO=@echo


CXXFLAGS=-O0 -pipe -g3 -Werror -MMD -pedantic -ansi -std=c++11 -Wall -Wextra \
-fno-builtin -Wstrict-overflow=5 -Wsign-conversion -Wshadow -Wuseless-cast \
-Wzero-as-null-pointer-constant -Wpadded -Wsign-conversion -Wsign-promo \
-Weffc++

TESTDIR=./tests/

TESTSRC=$(wildcard $(TESTDIR)*.cpp)
TESTOBJ=$(TESTSRC:.cpp=.o)

$(NAME): $(OBJ)
	$(ECHO) Creating $(NAME)
	$(CXX) $(OBJ) -o $(NAME) $(CXXFLAGS)

all: $(NAME) $(TESTNAME)

$(TESTNAME): $(OBJ) $(TESTOBJ)
	$(ECHO) Creating $(TESTNAME)
	$(CXX) $(OBJ) $(TESTOBJ) -o $(TESTNAME) $(CXXFLAGS)


.cpp.o: %.cpp
	$(ECHO) Compilling $<
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(foreach dir, $(INC), -I $(dir))


clean:
	$(ECHO) deleting temporary files
	$(RM) $(foreach dir, $(INC), $(dir)/*~)
	$(RM) $(TESTDIR)*~


fclean: clean
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


-include $(SRC:.cpp=.d) main.d
-include $(TESTSRC:.cpp=.d)
