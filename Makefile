


CFLAGS:=		\
	-lSDL2		\
	-lSDL2_ttf	\
	-Wall		\
	-lm		\
	-Og		\

.SUFFIXES: .c .o .elf .% .bin
.PHONY: clean all 
all: pong breakout bird

clean:
	rm *.d *.o pong breakout bird

pong: pong.c
	${CC} ${CFLAGS} pong.c -o $@ 

breakout: breakout.c
	${CC} ${CFLAGS} breakout.c -o $@ 

bird: bird.c
	${CC} ${CFLAGS} bird.c -o $@ 









