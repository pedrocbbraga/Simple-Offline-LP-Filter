CC = gcc
TARGET = lpf
SRC = main.c
INCLUDE = -I/opt/homebrew/opt/libsndfile/include
LIBS = -L/opt/homebrew/opt/libsndfile/lib -lsndfile
CFLAGS = -Wall -Wextra -std=c11

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(INCLUDE) $(LIBS)

clean:
	rm -f $(TARGET)
