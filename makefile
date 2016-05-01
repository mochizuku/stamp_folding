# 
OBJS = main.o check_fold.o

CC = g++
ifeq ($(OS),Windows_NT)
  TARGET = stamp_folding.exe 
  CFLAG = -std=c++11
else
  TARGET = stamp_folding
  CFLAG = -std=c++0x
endif

$(TARGET): $(OBJS)
	$(CC) $(CFLAG) -o $@ $(OBJS)

# suffix
.cpp.o :
	$(CC) $(CFLAG) -c $<

clean:
	rm -f *.o
	rm -f $(TARGET)
