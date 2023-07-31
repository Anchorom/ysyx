TOPNAME = top
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

# rules for verilator
VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast \
				--noassert --exe
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
LDFLAGS += -lSDL2 -lSDL2_image

#directory
BUILD_DIR = $(NPC_HOME)/build
OBJ_DIR = $(BUILD_DIR)/obj_dir
VCD_DIR = $(NPC_HOME)/vcd
BIN = $(BUILD_DIR)/$(TOPNAME)

VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
CSRCS += $(SRC_AUTO_BIND)

MAKE = make

MKFILE =  $(OBJ_DIR)/Vtop.mk
MKFLAGS = -C $(OBJ_DIR) -f $(MKFILE)
BINARY = Vtop

# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

all:default

run: $(BIN)
	@$^


sim:$(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
    		--top-module $(TOPNAME) $^ \
    		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
    		--Mdir $(OBJ_DIR) --exe --trace -o $(abspath $(BIN))
	@$(BIN)
#	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
#	$(VERILATOR) $(VERILATOR_CFLAGS) --trace $(CSRC) $(VSRC)
#	$(MAKE) $(MKFLAGS)
#	$(OBJ_DIR)/$(BINARY)
clean:
	rm -rf $(BUILD_DIR) $(VCD_DIR)/*.vcd

include ../Makefile

.PHONY: default all clean run
