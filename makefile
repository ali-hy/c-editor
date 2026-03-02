src_files := $(shell find src/ -type f -name '*.cpp')

build_dir := build/
debug_dest := $(build_dir)debug
program_name := editor
build_dest := $(build_dir)$(program_name)

all: $(src_files) $(build_dir)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(src_files) -o $(build_dest) -lX11 -lGL
	chmod +x $(build_dest)
	@echo "\nRuning $(build_dest)"
	@./$(build_dest)
	@echo ""

debug: $(src_files) $(build_dir)
	$(CXX) -ggdb -O0 $(CPPFLAGS) $(CXXFLAGS) $(src_files) -o $(debug_dest) -lX11 -lGL
	chmod +x $(debug_dest)
	@echo "\nRuning $(debug_dest)"
	gdb -se ./$(debug_dest)
	@echo "\nDebugging completed successfully"

$(build_dir):
	mkdir $(build_dir)
