CC = /usr/bin/gcc
CFLAGS += -Wall
LDFLAGS += -lcrypto

SRC := server.c
SRC2 := client.c

MD5_SRC := md5sum.c

MD5_OBJ := $(MD5_SRC:.c=.o)

MD5_OBJ2 := $(MD5_SRC:.c=.o)

TARGET := $(SRC:.c=)
TARGET2 := $(SRC2:.c=)

.PHONY: all clean

all: $(TARGET) client

$(TARGET): $(MD5_OBJ) md5sum.h
	$(CC) $(CFLAGS) -o $@ $< $(SRC) $(LDFLAGS)

$(TARGET2): $(MD5_OBJ2) md5sum.h
	$(CC) $(CFLAGS) -o $@ $< $(SRC2) $(LDFLAGS)


clean:
	-$(RM) $(TARGET) $(MD5_OBJ) $(TARGET2) $(MD5_OBJ2)
