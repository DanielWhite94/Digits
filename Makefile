CPP = gcc
CFLAGS = -std=gnu11 -Wall -O0 -ggdb3
LFLAGS = -lSDL2 -lSDL2_ttf

OBJS = ./src/bin.o ./src/box.o ./src/button.o ./src/container.o ./src/digits.o ./src/label.o ./src/main.o ./src/textbutton.o ./src/util.o ./src/widget.o ./src/window.o

ALL: $(OBJS)
	$(CPP) $(CFLAGS) $(OBJS) -o ./main $(LFLAGS)

%.o: %.c %.h
	$(CPP) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CPP) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)
