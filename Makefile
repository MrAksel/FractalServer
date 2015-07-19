CC      := cc
PKGS	:= 
CFLAGS  := -std=gnu99 -g -O0 -Wall -DMP_FLOATS	# `pkg-config --cflags $(PKGS)`
LIBS    := -lgmp								# `pkg-config --libs $(PKGS)` 

TARGET	:= fractals
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS	:= $(OBJECTS:.o=.deps)
 
$(TARGET): $(OBJECTS)
	@echo "  Linking..."; $(CC) $^ -o $(TARGET) $(LIBS)
 
build/%.o: src/%.c
	@mkdir -p build/
	@echo "  CC $<"; $(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<
 
clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET)
 
-include $(DEPS)
 
.PHONY: clean
