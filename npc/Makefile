VERILATOR = verilator
VFLAGS = --cc --exe -Wno-fatal --build
VFLAGS_TRACE = --cc --exe -Wno-fatal --build --trace
CSRC += $(NPC_HOME)/csrc/**
VSRC = $(NPC_HOME)/vsrc/**
MAKE = make
OBJ_DIR = $(NPC_HOME)/obj_dir
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
	$(OBJ_DIR)/Vtop
clean:
	rm -rf $(OBJ_DIR) *.vcd

include ../Makefile
