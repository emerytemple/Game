OBJS = main.o scene.o
COMPILER_FLAGS = -Wall
LINKER_FLAGS = -lSDL2 -lSDL2_image -lm

main: $(OBJS)
	cc -o main $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS)

main.o:
	cc -c main.c

scene.o:
	cc -c scene.c

.PHONY: clean
clean:
	-rm main $(OBJS)
