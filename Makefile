LIBUSB_SRC = ./depends/libusb-0.1.12 #change with your libusb sources folder
LUA_SRC = ./depends/lua-5.1.4/src    #change with your lua sources folder

OPTS=-Wall -D_GNU_SOURCE
LOPTS=-c -fPIC -fno-stack-protector
OBJS=lualibusb.o
OBJS_SO = lualibusb.so

CFLAGS=$(OPTS) -I$(LIBUSB_SRC) -I$(LUA_SRC) 
LDFLAGS=-shared -soname libluausb.so.1 -o


all: lualibusb so

lualibusb: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -c -fPIC -fno-stack-protector 

so: $(OBJS)
		$(LD) -shared -soname libluausb.so.1 -o $(OBJS_SO) -lusb $(OBJS)
clean:
	-rm $(OBJS) $(OBJS_SO)
