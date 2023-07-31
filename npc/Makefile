VERILATOR = verilator
VFLAGS = --cc --exe -Wno-fatal --build
VFLAGS_TRACE = --cc --exe -Wno-fatal --build --trace
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
MAKE = make
OBJ_DIR = $(NPC_HOME)/obj_dir
VCD_DIR = $(NPC_HOME)/vcd
MKFILE =  $(OBJ_DIR)/Vtop.mk
MKFLAGS = -C $(OBJ_DIR) -f $(MKFILE)
BINARY = Vtop

all:
	$(VERILATOR) $(VFLAGS) $(CSRC) $(VSRC)
	$(MAKE) $(MKFLAGS)

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	$(VERILATOR) $(VFLAGS_TRACE) $(CSRC) $(VSRC)
	$(MAKE) $(MKFLAGS)
	$(OBJ_DIR)/$(BINARY)

clean:
	rm -rf $(OBJ_DIR) $(VCD_DIR)/*.vcd

include ../Makefile
