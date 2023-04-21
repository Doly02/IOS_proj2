#################
#
# nazev: 	Makefile pro projekt 2 do predmetu IOS
# vytvoril:	Tomas Dolak
# datum:	20.4.2023
#
#################

# kompilator
CC = gcc

# prepinace kompilatoru
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

# nazev prelozeneho souboru
TARGET = proj2

DEPS = header_file.h

# seznam zdrojovych souboru
SOURCES = test_version4.c
# seznam objektovych souboru
OBJECTS = $(SOURCES:.c=.o)


$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)


%.o : %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Pravidlo pro vyčištění adresáře
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)
