CC := gcc
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
	-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
	-Wconversion -Wstrict-prototypes
CFLAGS := -g -std=gnu99 $(WARNINGS)

SRCFILES := $(wildcard *.c)
HDRFILES := $(wildcard *.h)
OBJFILES := $(patsubst %.c, %.o, $(SRCFILES))
AUXFILES := Makefile Readme.md
ALLFILES := $(SRCFILES) $(HDRFILES) $(AUXFILES)
DISTNAME := mandel-boin.tgz
ROOTPRJT := $(PWD)
EXEC := mandel

.PHONY: all clean realclean dist

all: $(EXEC)

$(EXEC): $(OBJFILES)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJFILES): $(SRCFILES)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	@rm $(OBJFILES)

realclean: clean
	@rm -f $(EXEC) $(DISTNAME)

dist:
	@tar czf $(DISTNAME) $(ALLFILES)
