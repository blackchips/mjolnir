SRC= $(wildcard ./*.c)
OBJ= $(SRC:.c=.o)
INC= $(PWD)
NAME=mjolnir
CXX=@gcc
RM=@rm -f
ECHO=@echo


CXXFLAGS=-O0 -pipe -g3 -Werror -MMD -pedantic -ansi -Wall -Wextra \
-fno-builtin -Wstrict-overflow=5 -Wsign-conversion -Wshadow \
-Wpadded -Wsign-conversion



$(NAME): $(OBJ)
	$(ECHO) Creating $(NAME)
	$(CXX) $(OBJ) -o $(NAME) $(CXXFLAGS)

all: $(NAME)

.c.o: %.c
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
	$(RM) $(SRC:.c=.d) main.d
	$(RM) $(TESTSRC:.c=.d)
	$(ECHO) deleting executables
	$(RM) $(TESTNAME)
	$(RM) $(NAME)

re:	fclean all

check:
	valgrind ../cppcheck/cppcheck  --verbose --enable=all --inconclusive $(SRC)


-include $(SRC:.c=.d)
