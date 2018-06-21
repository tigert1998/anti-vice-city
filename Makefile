CC = clang++
FLAGS = -lGL -lGLU -lglut -lglfw -lassimp

main: clean
	$(CC) main.cpp -o main $(FLAGS)

clean:
	-rm main