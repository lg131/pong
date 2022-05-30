


CFLAGS:=		\
	-lSDL2		\
	-Wall		\
	-lm		\
	-Og		\

.SUFFIXES: .c .o .elf .% .bin
.PHONY: clean all 
all: pong breakout bird

clean:
	rm *.d *.o pong breakout bird

pong:
	${CC} ${CFLAGS} pong.c -o $@ 

breakout:
	${CC} ${CFLAGS} breakout.c -o $@ 

bird:
	${CC} ${CFLAGS} bird.c -o $@ 









