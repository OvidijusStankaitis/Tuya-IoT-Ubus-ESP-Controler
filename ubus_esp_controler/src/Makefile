CFLAGS=-Wall
LDFLAGS=-lubus -lubox -lblobmsg_json -lserialport
SOURCES=main.c tls.c ubus_tls.c
OBJECTS=$(SOURCES:.c=.o)
BIN=ubus_esp_controler

all: $(SOURCES) $(BIN)

$(BIN): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJECTS) $(BIN)