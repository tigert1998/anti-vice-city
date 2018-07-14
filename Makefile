CC = clang++
STD_FLAG = -std=c++11

OSFLAG :=
ifeq ($(shell uname -s), Linux)
	OSFLAG += LINUX
endif
ifeq ($(shell uname -s), Darwin)
	OSFLAG += OSX
endif

FLAGS = -lglfw -lassimp
ifeq ($(OSFLAG), LINUX)
	FLAGS += -lGL
endif
ifeq ($(OSFLAG), OSX)
	FLAGS += -framework OpenGL
endif

main: clean
	$(CC) $(STD_FLAG) main.cpp glad.c -o main $(FLAGS)

clean:
	-rm main
