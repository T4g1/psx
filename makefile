CXXFLAGS  = -std=c++17 -Wall -Wextra -I. -g -ggdb

ifeq ($(OS),Windows_NT)
    CC        = g++
    LINKER    = g++
    LFLAGS    = -LC:\MinGW\lib -static-libstdc++ -static-libgcc -lm -lSDL2main -lSDL2
else
    CC        = g++
    LINKER    = g++
    LFLAGS    = -lm -lSDL2 -lGL -ldl -lstdc++fs
endif

SRCDIR    = src
OBJDIR    = src

SOURCES  := $(wildcard $(SRCDIR)/*.cpp) \
            $(wildcard lib/imgui/*.cpp)
SOURCES  := $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
SOURCES  := $(filter-out $(SRCDIR)/test.cpp, $(SOURCES))

INCLUDES := -Ilib/imgui \
            -Ilib/imgui_club/ \
            -Ilib/gl3w

ifeq ($(OS),Windows_NT)
    INCLUDES += -IC:\MinGW\include
endif

CXXFLAGS += $(INCLUDES)

OBJECTS       := $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS_C     := lib/gl3w/GL/gl3w.o
PSX_OBJECTS   := $(OBJECTS) $(OBJECTS_C) $(OBJDIR)/main.o
TEST_OBJECTS  := $(OBJECTS) $(OBJECTS_C) $(OBJDIR)/test.o

debug: CXXFLAGS += -DDEBUG
debug: all

all: psx test

psx: CXXFLAGS +=
psx: $(PSX_OBJECTS)
	$(LINKER) $(PSX_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking psx complete!"

test: CXXFLAGS +=
test: $(TEST_OBJECTS)
	$(LINKER) $(TEST_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking test complete!"

$(OBJECTS): %.o : %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

$(OBJECTS_C): %.o : %.c
	$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	-@rm $(wildcard $(OBJDIR)/*.o)
	@echo "Cleanup complete!"

.PHONY: remove
remove:
ifneq (,$(wildcard psx))
	@rm psx
endif
ifneq (,$(wildcard test))
	@rm test
endif
	@echo "Executable removed!"
