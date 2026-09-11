CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Werror -g -O0 -Iinclude
PARSER_SRC = src/event_parser.c src/event.c
APP_SRC    = src/gateway.c src/diag_engine.c $(PARSER_SRC)
.PHONY: all test test-parser test-baseline run demo-arrays course-status clean
all: gateway
gateway: $(APP_SRC) include/*.h
	$(CC) $(CFLAGS) -o $@ $(APP_SRC)
build:
	mkdir -p build
build/test_event_parser: tests/test_event_parser.c $(PARSER_SRC) include/*.h | build
	$(CC) $(CFLAGS) -o $@ tests/test_event_parser.c $(PARSER_SRC)
build/array_forms: examples/array_forms.c $(PARSER_SRC) include/*.h | build
	$(CC) $(CFLAGS) -o $@ examples/array_forms.c $(PARSER_SRC)
test: test-parser
test-parser: build/test_event_parser
	./build/test_event_parser
test-baseline: build/test_event_parser
	./build/test_event_parser valid
run: gateway
	./gateway data/module_events.txt
demo-arrays: build/array_forms
	./build/array_forms
course-status:
	@echo "PC3914 participant starter repository (v2.17)"
	@echo "Current TODOs: TODO-L1.1 / TODO-L1.2 - bounded parsing and borrowed views"
clean:
	rm -rf build gateway
