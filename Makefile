NAME = scop

C_FILES = main.c scop.c stb_image.c


all: $(NAME)

$(NAME):
	cc $(C_FILES) -lm -lGLEW -lGL -lglfw -o $(NAME)

clean:
	rm scop

.PHONY: all clean $(NAME)