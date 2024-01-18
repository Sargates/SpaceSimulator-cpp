# Recursive wildcard function
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# Source files
SRC_FILES := $(call rwildcard,src/,*.cpp) main.cpp

# Object files directory
OBJ_DIR := obj

# Object files, I do not understand this
OBJ_FILES := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(filter src/%.cpp,$(SRC_FILES))) \
			 $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(filter-out src/%.cpp,$(SRC_FILES)))


# Run `make clean` before changing name of program
PROGRAM := main


all: windows_dynam run


program: $(OBJ_FILES)
	@# Use `g++ -print-search-dirs` to see default search paths or clang equiv.
	$(CXX) $^ -o $(PROGRAM) $(CXXFLAGS)



# Rule for compiling source files
$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

# # Rule for compiling source files outside of source
# $(OBJ_DIR)/%.o: %.cpp
# 	@mkdir -p $(@D)
# 	@$(CXX) -c $< -o $@ $(CXXFLAGS)

windows:
	@echo "Making game for Windows"
	@$(MAKE) program CXX=x86_64-w64-mingw32-g++ CXXFLAGS="-I include -Llib/x86_64-w64-mingw32 -std=c++20 -lraylib -Wall -Wno-narrowing -Wcomment -lopengl32 -lgdi32 -lwinmm -static -static-libgcc -static-libstdc++" --no-print-directory

windows_dynam:
	@echo "Making game for Windows-Dynamic"
	@$(MAKE) program CXX=x86_64-w64-mingw32-g++ CXXFLAGS="-I include -Llib/x86_64-w64-mingw32 -std=c++20 -lraylibdll -Wall -Wno-narrowing -Wcomment -lopengl32 -lgdi32 -lwinmm" --no-print-directory

linux:
	@echo "Making game for Linux"
	@$(MAKE) program CXX=g++ CXXFLAGS="-I include -Llib/x86_64-linux-gnu -std=c++20 -lraylib-s -Wall -Wno-narrowing -Wcomment" --no-print-directory

run:
	@# Use * to autocomplete program regardless of extension -- might cause unintended behavior
	./$(PROGRAM)*

test:
	@echo $(OBJ_FILES)

clean:
	@rm -rf $(PROGRAM) $(PROGRAM).exe $(OBJ_DIR)
	@echo Project cleaned!
