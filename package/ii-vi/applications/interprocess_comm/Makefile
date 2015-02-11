
SRCDIR = \
	. \
	src

INCDIR = \
	. \
	inc

## set GDBFLAGS for GDB debugging
#GDBFLAGS += -g
GDBFLAGS +=

CFLAGS += $(patsubst %,-I%,$(INCDIR))
CFLAGS += -lpthread
CPPFLAGS += -lstdc++

CSRC = $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.c))
CPPSRC = $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.cpp))
COBJ = $(CSRC:.c=.o)
CPPOBJ = $(CPPSRC:.cpp=.o)

all: interprocess_comm

interprocess_comm: $(COBJ) $(CPPOBJ)
	$(CXX) $^ $(CFLAGS) $(CPPFLAGS) $(GDBFLAGS) -o ipc_demo

$(COBJ): %.o: %.c
	$(CC) -c $(CFLAGS) $(GDBFLAGS) $^ -o $@
	
$(CPPOBJ): %.o: %.cpp
	$(CXX)	-c $(CFLAGS) $(CPPFLAGS) $(GDBFLAGS) $^ -o $@
