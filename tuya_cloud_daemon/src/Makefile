CFLAGS=-Wall
LDFLAGS=-llink_core -lutils_modules -lplatform_port -lmiddleware_implementation -lubus -lubox -lblobmsg_json -largp
SOURCES=main.c tuyaConnect.c argParser.c ubusInvoke.c
OBJECTS=$(SOURCES:.c=.o)
BIN=tuya_cloud_daemon

all: $(SOURCES) $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS) $(BIN)