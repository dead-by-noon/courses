# Define the compiler and flags
CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Werror
TARGET := aesdsocket

# Default target to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

# Clean target to remove the executable
clean:
	rm -f $(TARGET)

# Declare the phony targets
.PHONY: all clean
