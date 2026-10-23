# net: networking stubs and host tests. See docs/DESIGN.md for the plan.
CC     ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Werror -Iinclude

BUILD := build
SRC   := src/driver/virtio_net_stub.c src/stack/stack_stub.c src/service/service_stub.c

all: $(BUILD)/libnet.a

$(BUILD)/libnet.a: $(SRC:src/%.c=$(BUILD)/%.o)
	ar rcs $@ $^

$(BUILD)/%.o: src/%.c $(wildcard include/net/*.h)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/test_stubs: tests/test_stubs.c $(BUILD)/libnet.a
	$(CC) $(CFLAGS) $< $(BUILD)/libnet.a -o $@

test: $(BUILD)/test_stubs
	./$(BUILD)/test_stubs

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
