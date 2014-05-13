CC=gcc
LD=gcc

PVMINC=$(PVM_ROOT)/include
PVMLIB=$(PVM_ROOT)/lib/$(PVM_ARCH) 

LIBS=-L$(PVMLIB)
FLAGS=-Wall -lpvm3 -lgpvm3 -I$(PVMINC)

CFLAGS=$(FLAGS)
LDFLAGS=$(FLAGS) $(LIBS)

TARGET_MASTER=master
TARGET_SLAVE=slave

SRC_MASTER=master
SRC_SLAVE=slave

SRCS_MASTER=$(addsuffix .c, $(SRC_MASTER)) 
OBJS_MASTER=$(addprefix obj/master/, $(addsuffix .o, $(SRCS_MASTER)))

SRCS_SLAVE=$(addsuffix .c, $(SRC_SLAVE)) 
OBJS_SLAVE=$(addprefix obj/slave/, $(addsuffix .o, $(SRCS_SLAVE)))


all:  $(TARGET_SLAVE) $(TARGET_MASTER)

obj/master/%.c.o: src/master/%.c src/master/%.h
	$(CC) -c $< $(CFLAGS) -o $@
	
obj/slave/%.c.o: src/slave/%.c src/slave/%.h
	$(CC) -c $< $(CFLAGS) -o $@

$(TARGET_MASTER): $(OBJS_MASTER)
	$(LD) $(OBJS_MASTER) $(LDFLAGS) -o $(TARGET_MASTER)

$(TARGET_SLAVE): $(OBJS_SLAVE)
	$(LD) $(OBJS_SLAVE) $(LDFLAGS) -o $(TARGET_SLAVE)

clean:
	rm -f $(TARGET_MASTER) $(TARGET_SLAVE) obj/master/* obj/slave/*

