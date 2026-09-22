NAME = scop

C_FILES = main.c scop.c


all: $(NAME)

$(NAME):
	cc $(C_FILES) -lGLEW -lGL -lglfw -o $(NAME)

clean:
	rm scop

.PHONY: all clean $(NAME)