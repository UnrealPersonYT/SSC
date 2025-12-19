# Target executable
TARGET = ssc_bench.exe
SRC = src/benchmark.c

# Default to GCC
CC = gcc
CFLAGS = -Wall -O3 -march=native -g
OUTFLAG = -o

# Detect OS
ifeq ($(OS),Windows_NT)
    # On Windows
    RM = del /Q
    # Check if gcc is available
    ifeq ($(shell where gcc >nul 2>nul && echo found),found)
        CC = gcc
        CFLAGS = -Wall -O3 -march=native -g
        OUTFLAG = -o
    else
        CC = cl
        CFLAGS = /nologo /W3 /O2
        OUTFLAG =
    endif
else
    # On Linux / Unix
    RM = rm -f
    ifeq ($(shell which gcc >/dev/null 2>&1 && echo found),found)
        CC = gcc
        CFLAGS = -Wall -O3 -march=native -g
        OUTFLAG = -o
    else
        CC = cc
        CFLAGS =
        OUTFLAG = -o
    endif
endif

# Build rule
$(TARGET): $(SRC)
ifeq ($(CC),gcc)
	$(CC) $(CFLAGS) $(SRC) $(OUTFLAG) $(TARGET)
else ifeq ($(CC),cl)
	$(CC) $(CFLAGS) $(SRC) /Fe$(TARGET)
else
	$(CC) $(CFLAGS) $(SRC) $(OUTFLAG) $(TARGET)
endif

# Clean rule
clean:
	$(RM) $(TARGET) *.o *.obj