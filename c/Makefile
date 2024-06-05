#### BEGIN USER OPTIONS ####
# Enables debug mode: 0 (no debug) 1 (debug) 2 (debug with optimization)
DEBUG := 1
# Enable micro-benchmarking during tests, useful to check performance variation across builds: 0/1
MEASURE_PERFORMANCE := 1
# Enable OpenMP parallelization: O/1
MULTICORE := 0
# Override default compiler and flags: 0/1
OVERRIDE_DEFAULT_CC := 1
OVERRIDE_DEFAULT_CXX := 1
# Enable assembly code (improves performance, might be buggy): 0/1
USE_ASM := 0
# Enable CUDA acceleration (improves performance, requires an NVIDIA GPU): 0/1
USE_CUDA := 0
#### END USER OPTIONS ####

#### BEGIN TARGETS ####
# C executable targets
TARGETS_EXE_CC :=
TARGETS_EXE_CC += aes_encrypt
TARGETS_EXE_CC += binary_xor
TARGETS_EXE_CC += check_irred
TARGETS_EXE_CC += circulant_mul
TARGETS_EXE_CC += find_inverse
TARGETS_EXE_CC += fq_pow
TARGETS_EXE_CC += ideal_poker
TARGETS_EXE_CC += inner_prod
TARGETS_EXE_CC += inv_table
TARGETS_EXE_CC += mod_pow
TARGETS_EXE_CC += poly_mul
TARGETS_EXE_CC += test_aes_sbox

# C non-executable targets
TARGETS_LIB_CC :=
TARGETS_LIB_CC += aes
TARGETS_LIB_CC += fq
TARGETS_LIB_CC += fq_poly
TARGETS_LIB_CC += fq_mat
TARGETS_LIB_CC += fq_vec
TARGETS_LIB_CC += zp
TARGETS_LIB_CC += zp_mat
TARGETS_LIB_CC += zp_poly
TARGETS_LIB_CC += zp_vec

# C test targets
TARGETS_TEST_CC :=

# C++ executable targets
TARGETS_EXE_CXX :=
TARGETS_EXE_CXX += dif_tc00
TARGETS_EXE_CXX += ideal_cipher
TARGETS_EXE_CXX += lin_tc00
TARGETS_EXE_CXX += multicollision
TARGETS_EXE_CXX += tc05_lin_atk

# C++ non-executable targets
TARGETS_LIB_CXX :=
TARGETS_LIB_CXX += rand
TARGETS_LIB_CXX += tc05

# C++ test targets
TARGETS_TEST_CXX :=

# CUDA executable targets
TARGETS_EXE_CUDA :=

# CUDA non-executable targets
TARGETS_TEST_CUDA :=

# CUDA non-executable targets
TARGETS_LIB_CUDA :=
ifeq ($(USE_CUDA), 1)
    TARGETS_LIB_CUDA += cu_tc05
endif
# Target cross-dependencies, in the form of <target.dependency1+...+dependencyn>
TARGET_DEPENDS :=


# LIBRARY TARGET
LIB_NAME := liblib

# Put all targets together
TARGETS_LIB := $(TARGETS_LIB_CC) $(TARGETS_LIB_CXX) $(TARGETS_LIB_CUDA)
TARGETS_EXE := $(TARGETS_EXE_CC) $(TARGETS_EXE_CXX) $(TARGETS_EXE_CUDA)
TARGETS_TEST := $(TARGETS_TEST_CC) $(TARGETS_TEST_CXX) $(TARGETS_TEST_CUDA)
TARGETS_CC := $(TARGETS_EXE_CC) $(TARGETS_LIB_CC) $(TARGETS_TEST_CC)
TARGETS_CXX := $(TARGETS_EXE_CXX) $(TARGETS_LIB_CXX) $(TARGETS_TEST_CXX) 
TARGETS_CUDA := $(TARGETS_LIB_CUDA)
TARGETS := $(TARGETS_CC) $(TARGETS_CXX) $(TARGETS_CUDA)
TARGETS_NOTLIB := $(TARGETS_TEST) $(TARGETS_EXE)
#### END TARGETS ####


#### BEGIN DIRECTORIES SETUP ####
# Main directory for binaries
BIN_PATH := bin
# Main directory for build files
BUILD_PATH := build
# Main directory for include files
INC_PATH := include
# Main directory for library files
LIB_PATH := lib
# Main directory for dependency files
DEP_PATH := $(BUILD_PATH)/dep
# Main directory for source files
SRC_PATH := src
# Main directory for test files, also subdirectory path for test related outputs
TEST_PATH := test
# Subdirectory path for C related outputs
CC_PATH := cc
# Subdirectory path for C++ related outputs
CXX_PATH := cxx
# Subdirectory path for CUDA related outputs
CUDA_PATH := cuda

# Convenience variables that depend on the previous ones
BUILD_PATH_CC := $(BUILD_PATH)/$(CC_PATH)
BUILD_PATH_CXX := $(BUILD_PATH)/$(CXX_PATH)
BUILD_PATH_CUDA := $(BUILD_PATH)/$(CUDA_PATH)

TEST_BUILD_PATH_CC := $(BUILD_PATH_CC)/$(TEST_PATH)
TEST_BUILD_PATH_CXX := $(BUILD_PATH_CXX)/$(TEST_PATH)
TEST_BUILD_PATH_CUDA := $(BUILD_PATH_CUDA)/$(TEST_PATH)

TEST_BIN_PATH := $(BIN_PATH)/$(TEST_PATH)
TEST_DEP_PATH := $(DEP_PATH)/$(TEST_PATH)

# Include files
INC := $(wildcard $(INC_PATH)/*.h $(INC_PATH)/**/*.h $(INC_PATH)/*.hpp $(INC_PATH)/**/*.hpp $(INC_PATH)/*.cuh $(INC_PATH)/**/*.cuh)

# Source files
SRC_CC := $(wildcard $(SRC_PATH)/*.c $(SRC_PATH)/**/*.c)
SRC_CXX := $(wildcard $(SRC_PATH)/*.cpp $(SRC_PATH)/**/*.cpp)
SRC_CUDA := $(wildcard $(SRC_PATH)/*.cu $(SRC_PATH)/**/*.cu)
SRC := $(SRC_CC) $(SRC_CXX) $(SRC_CUDA)

# Test files
TEST_CC := $(wildcard $(TEST_PATH)/*.c $(TEST_PATH)/**/*.c)
TEST_CXX := $(wildcard $(TEST_PATH)/*.cpp $(TEST_PATH)/**/*.cpp)
TEST_CUDA := $(wildcard $(TEST_PATH)/*.cu $(TEST_PATH)/**/*.cu)
TEST := $(TEST_CC) $(TEST_CXX) $(TEST_CUDA)

# Source names
SRC_NAMES_CC := $(notdir $(SRC_CC))
SRC_NAMES_CXX := $(notdir $(SRC_CXX))
SRC_NAMES_CUDA := $(notdir $(SRC_CUDA))
SRC_NAMES := $(SRC_NAMES_CC) $(SRC_NAMES_CXX) $(SRC_NAMES_CUDA)

# Test names
TEST_NAMES_CC := $(notdir $(TEST_CC))
TEST_NAMES_CXX := $(notdir $(TEST_CXX))
TEST_NAMES_CUDA := $(notdir $(TEST_CUDA))
TEST_NAMES := $(TEST_NAMES_CC) $(TEST_NAMES_CXX) $(TEST_NAMES_CUDA)
#### END DIRECTORIES SETUP ####

#### BEGIN COMPILER/LINKER SETUP ####
# If there is no default compiler, or if we want to override it, set the compiler here
ifeq ($(CC), )
    CC := cc
    CFLAGS := 
endif
ifeq ($(CXX), )
    CXX := c++
    CXXFLAGS := 
endif
ifeq ($(NVCC), )
    NVCC := nvcc
    NVCCFLAGS := 
endif

ifeq ($(OVERRIDE_DEFAULT_CC), 1)
    ifeq ($(OS), Windows_NT)
        CC := icx
        CFLAGS :=
    else
        CC := icx
        CFLAGS :=
    endif
endif
ifeq ($(OVERRIDE_DEFAULT_CXX), 1)
    ifeq ($(OS), Windows_NT)
        CXX := icx
        CXXFLAGS :=
    else
        CXX := icpx
        CXXFLAGS :=
    endif
endif
ifeq ($(OVERRIDE_DEFAULT_NVCC), 1)
    NVCC := nvcc
    NVCCFLAGS :=
endif

# Override flags for debugging
ifneq ($(DEBUG), 0)
    NVCCFLAGS := -g
    ifeq ($(CC), cl)
        CFLAGS := -DEBUG -Zi
        CXXFLAGS := -DEBUG -Zi
    else
        CFLAGS := -g
        CXXFLAGS := -g
        ifeq ($(OS), Windows_NT)
            ifeq ($(CC), icx)
                CFLAGS := -debug
                ifeq ($(DEBUG), 1)
                    CFLAGS += -Rno-debug-disables-optimization
                endif
            endif
            ifeq ($(CXX), icpx)
                ifeq ($(DEBUG), 1)
                    CXXFLAGS += -Rno-debug-disables-optimization
                endif
            endif
            ifeq ($(CXX), icx)
                CXXFLAGS := -debug
                ifeq ($(DEBUG), 1)
                    CXXFLAGS += -Rno-debug-disables-optimization
                endif
            endif
        endif
    endif
endif
ifneq ($(DEBUG), 1)
    CFLAGS += -O3
    CXXFLAGS += -O3
    NVCCFLAGS += -O3
endif

# Additional compiler flags 
ifeq ($(CC), cl)
    OBJ_OUT_FLAG := -Fo:
    EXE_OUT_FLAG := -Fe:
    DEP_OUT_CFLAG := -MF:
    DEP_OUT_CXXFLAG := -MF:
    CFLAGS += -nologo -arch:AVX2 -std:clatest -Wall -I$(INC_PATH) -MP
    CXXFLAGS += -nologo -EHsc -arch:AVX2 -std:clatest -Wall -I$(INC_PATH) -MP
else
    OBJ_OUT_FLAG := -o
    EXE_OUT_FLAG := -o
    DEP_OUT_CFLAG := -MF
    DEP_OUT_CXXFLAG := -MF
    EXTRA_CFLAGS := -std=c2x -march=native -Wall -I$(INC_PATH) -MMD -MP
    EXTRA_CXXFLAGS := -std=c++23 -march=native -Wall -I$(INC_PATH) -MMD -MP
    ifeq ($(CC), icx)
        ifeq ($(OS), Windows_NT)
            EXTRA_CFLAGS := -Qstd=c2x -march=native -Qiopenmp -nologo -Wall -I$(INC_PATH) -QMMD -QMP
            DEP_OUT_CFLAG := -QMF
        else
            EXTRA_CFLAGS += -qopenmp
        endif
    else
        EXTRA_CFLAGS += -fopenmp
    endif
    ifeq ($(CXX), icx)
        ifeq ($(OS), Windows_NT)
            EXTRA_CXXFLAGS := -Qstd=c++23 -march=native -Qiopenmp -nologo -Wall -I$(INC_PATH) -QMMD -QMP
            DEP_OUT_CXXFLAG := -QMF
        else
            EXTRA_CXXFLAGS += -qopenmp
        endif
    else
        ifeq ($(CXX), icpx)
            EXTRA_CXXFLAGS += -qopenmp
        else
            EXTRA_CXXFLAGS += -fopenmp
        endif
    endif
    
    CFLAGS += $(EXTRA_CFLAGS)
    CXXFLAGS += $(EXTRA_CXXFLAGS)
endif

NVCCFLAGS += -allow-unsupported-compiler -ccbin=/usr/local/cuda_bin -std=c++20 -arch=compute_89 -code=compute_89 -I$(INC_PATH)
DEP_OUT_NVCCFLAG := -MMD

#### END COMPILER/LINKER SETUP ####

#### BEGIN FILES SETUP ####
# Executable file extenstion
EXE_EXT := exe

# Object file extenstion
ifeq ($(CC), cl)
    OBJ_EXT := obj
    DEP_EXT := d
else
    OBJ_EXT := o
    DEP_EXT := d
endif

# Object files
OBJ_CC := $(SRC_NAMES_CC:%.c=$(BUILD_PATH_CC)/%.$(OBJ_EXT)) 
OBJ_CXX := $(SRC_NAMES_CXX:%.cpp=$(BUILD_PATH_CXX)/%.$(OBJ_EXT)) 
OBJ_CUDA := $(SRC_NAMES_CUDA:%.cu=$(BUILD_PATH_CUDA)/%.$(OBJ_EXT)) 
OBJ := $(OBJ_CC) $(OBJ_CXX) $(OBJ_CUDA)

TEST_OBJ_CC := $(TEST_NAMES_CC:%.c=$(TEST_BUILD_PATH_CC)/%.$(OBJ_EXT))
TEST_OBJ_CXX := $(TEST_NAMES_CXX:%.cpp=$(TEST_BUILD_PATH_CXX)/%.$(OBJ_EXT))
TEST_OBJ_CUDA := $(TEST_NAMES_CUDA:%.cu=$(TEST_BUILD_PATH_CUDA)/%.$(OBJ_EXT))
TEST_OBJ := $(TEST_OBJ_CC) $(TEST_OBJ_CXX) $(TEST_OBJ_CUDA)

# Dependency files (allows recompiling when a header changes without `make clean`)
DEP := $(OBJ:%.$(OBJ_EXT)=%.$(DEP_EXT)) 
TEST_DEP := $(TEST_OBJ:%.$(OBJ_EXT)=%.$(DEP_EXT)) 

# Command to run all the tests at once, will be printed to a bash script
TEST_COMMAND := $(TARGETS_TEST:%=$$SCRIPT_PATH/$(TEST)/%;\n)
#### END FILES SETUP ####

#### BEGIN MACRO DEFINITION ARGUMENTS ####
# Enable performance measurament code in test files
ifeq ($(MEASURE_PERFORMANCE), 1)
    CFLAGS += -DMEASURE_PERFORMANCE
    CXXFLAGS += -DMEASURE_PERFORMANCE
endif

# Enable parallel code
ifeq ($(MULTICORE), 1)
    CFLAGS += -DMULTICORE
    CXXFLAGS += -DMULTICORE
endif

# Enable assembly code
ifeq ($(USE_ASM), 1)
    CFLAGS += -DUSE_ASM
    CXXFLAGS += -DUSE_ASM
endif

# Additional macro definitions
CFLAGS += 
CXXFLAGS += 
NVCCFLAGS += 
#### END MACRO DEFINITION ARGUMENTS ####

# Archive command and flags
ifeq ($(OS), Windows_NT)
    AR := lib
    ARFLAGS := -nologo /OUT:
    LIB_EXT := lib
    LDFLAGS := -link
    ifeq ($(USE_CUDA), 1)
        LDFLAGS += cudart.lib
    endif
else
    AR := ar
    ARFLAGS := r 
    LIB_EXT := a
    LDFLAGS := -lcudart -lstdc++
endif

# Debug file extension (for Windows only)
PDB :=
#### END COMPILER/LINKER SETUP ####


#### BEGIN COMMAND ALIASING ####
# Make useful commands OS independent
ifeq ($(OS), Windows_NT)
    MKDIR = if not exist $(subst /,\,$1) mkdir $(subst /,\, $1)
    RM = if exist $(1) rmdir /S /Q $(1)
    MV = mv $(1) $(2)
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)/*
    MV = mv $(1) $(2)/
endif
#### END COMMAND ALIASING ####

#### BEGIN MAIN RECIPES ####
# Build all targets
all: dirs .WAIT $(TARGETS_NOTLIB) testsuite
	

# Remove all build files
clean:
	$(call RM,$(BUILD_PATH))
	$(call RM,$(BIN_PATH))
	$(call RM,$(LIB_PATH))

# Create build directories
dirs:
	$(call MKDIR, $(BUILD_PATH))
	$(call MKDIR, $(BUILD_PATH_CC))
	$(call MKDIR, $(BUILD_PATH_CXX))
	$(call MKDIR, $(BUILD_PATH_CUDA))
	$(call MKDIR, $(TEST_BUILD_PATH_CC))
	$(call MKDIR, $(TEST_BUILD_PATH_CXX))
	$(call MKDIR, $(TEST_BUILD_PATH_CUDA))
	$(call MKDIR, $(DEP_PATH))
	$(call MKDIR, $(TEST_DEP_PATH))
	$(call MKDIR, $(BIN_PATH))
	$(call MKDIR, $(TEST_BIN_PATH))
	$(call MKDIR, $(LIB_PATH))

# Generate "run all tests" script
testsuite: library
	@echo -e '#!/bin/sh\nSCRIPT_PATH=$$(dirname "$$(readlink -f "$$0")")\n$(TEST_COMMAND)' > $(BIN_PATH)/test.sh

# Build library
library: $(LIB_PATH)/$(LIB_NAME).$(LIB_EXT)


$(LIB_PATH)/$(LIB_NAME).$(LIB_EXT): $(TARGETS_LIB_CC) $(TARGETS_LIB_CXX) $(TARGETS_LIB_CUDA)
	$(AR) $(ARFLAGS)$(LIB_PATH)/$(LIB_NAME).$(LIB_EXT) \
    $(foreach target, $(TARGETS_LIB_CC), $(BUILD_PATH_CC)/$(target).$(OBJ_EXT)) \
    $(foreach target, $(TARGETS_LIB_CXX), $(BUILD_PATH_CXX)/$(target).$(OBJ_EXT)) \
    $(foreach target, $(TARGETS_LIB_CUDA), $(BUILD_PATH_CUDA)/$(target).$(OBJ_EXT))

#### END MAIN RECIPES ####

#### BEGIN DEPENDENCIES RECIPES ####

# Updated with CUDA might be wrong now
define TARGET_DEPENDS_RULE
$(firstword $(subst ., ,$(1))): $(foreach dep, $(subst +, ,$(lastword $(subst ., ,$(1)))), $(if $(filter $(dep),$(TARGETS_CC)),$(BUILD_PATH_CC)/$(dep).$(OBJ_EXT),$(BUILD_PATH_CXX)/$(dep).$(OBJ_EXT),$(BUILD_PATH_CUDA)/$(dep).$(OBJ_EXT)))
endef

$(foreach target,$(TARGET_DEPENDS),$(eval $(call TARGET_DEPENDS_RULE,$(target))))

#### END DEPENDENCIES RECIPES ####

#### BEGIN GENERIC TARGET RECIPES ####
# Build C library targets
$(TARGETS_LIB_CC):  %: $(BUILD_PATH_CC)/%.$(OBJ_EXT)

# Build C++ library targets
$(TARGETS_LIB_CXX):  %: $(BUILD_PATH_CXX)/%.$(OBJ_EXT)

# Build CUDA library targets
ifeq ($(USE_CUDA), 1)
$(TARGETS_LIB_CUDA):  %: $(BUILD_PATH_CUDA)/%.$(OBJ_EXT)
endif

# Build C executable targets
$(TARGETS_EXE_CC):  %: $(BUILD_PATH_CC)/%.$(OBJ_EXT) $(LIB_PATH)/$(LIB_NAME).$(LIB_EXT)
	$(CC) $(CFLAGS) $^ $(EXE_OUT_FLAG) $(BIN_PATH)/$@.$(EXE_EXT) $(LDFLAGS)

# Build C test targets
$(TARGETS_TEST_CC): %: $(TEST_BUILD_PATH_CC)/%.$(OBJ_EXT) $(LIB_PATH)/$(LIB_NAME).$(LIB_EXT)
	$(CC) $(CFLAGS) $^ $(EXE_OUT_FLAG) $(BIN_PATH)/$(TEST_PRE)$@.$(EXE_EXT) $(LDFLAGS)
	
# Build C++ executable targets
$(TARGETS_EXE_CXX):  %: $(BUILD_PATH_CXX)/%.$(OBJ_EXT) $(LIB_PATH)/$(LIB_NAME).$(LIB_EXT)
	$(CXX) $(CXXFLAGS) $^ $(EXE_OUT_FLAG) $(BIN_PATH)/$@.$(EXE_EXT) $(LDFLAGS)

# Build C++ test targets
$(TARGETS_TEST_CXX): %: $(TEST_BUILD_PATH_CXX)/%.$(OBJ_EXT) $(LIB_PATH)/$(LIB_NAME).$(LIB_EXT)
	$(CXX) $(CXXFLAGS) $^ $(EXE_OUT_FLAG) $(TEST_BIN_PATH)/$@.$(EXE_EXT) $(LDFLAGS)
#### END GENERIC TARGET RECIPES ####

#### BEGIN GENERIC SOURCE RECIPES ####
# Compile C sources (main directory)
$(BUILD_PATH_CC)/%.$(OBJ_EXT): $(SRC_PATH)/%.c
	$(CC) $(CFLAGS) -c $< $(OBJ_OUT_FLAG) $@ $(DEP_OUT_CFLAG) $(DEP_PATH)/$(notdir $*).$(DEP_EXT)

# Compile C sources (recursive)
$(BUILD_PATH_CC)/%.$(OBJ_EXT): $(SRC_PATH)/**/%.c
	$(CC) $(CFLAGS) -c $< $(OBJ_OUT_FLAG) $@ $(DEP_OUT_CFLAG) $(DEP_PATH)/$(notdir $*).$(DEP_EXT)

# Compile C tests (main directory)
$(TEST_BUILD_PATH_CC)/%.$(OBJ_EXT): $(TEST_PATH)/%.c
	$(CC) $(CFLAGS) -c $< $(OBJ_OUT_FLAG) $@ $(DEP_OUT_CFLAG) $(TEST_DEP_PATH)/$(notdir $*).$(DEP_EXT)

# Compile C tests (recursive)
$(TEST_BUILD_PATH_CC)/%.$(OBJ_EXT): $(TEST_PATH)/**/%.c
	$(CC) $(CFLAGS) -c $< $(OBJ_OUT_FLAG) $@ $(DEP_OUT_CFLAG) $(TEST_DEP_PATH)/$(notdir $*).$(DEP_EXT)

# Compile C++ sources (main directory)
$(BUILD_PATH_CXX)/%.$(OBJ_EXT): $(SRC_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< $(OBJ_OUT_FLAG) $@ $(DEP_OUT_CXXFLAG) $(DEP_PATH)/$(notdir $*).$(DEP_EXT)

# Compile C++ sources (recursive)
$(BUILD_PATH_CXX)/%.$(OBJ_EXT): $(SRC_PATH)/**/%.cpp
	$(CXX) $(CXXFLAGS) -c $< $(OBJ_OUT_FLAG) $@-$(DEP_OUT_CXXFLAG) $(DEP_PATH)/$(notdir $*).$(DEP_EXT)

# Compile C++ tests (main directory)
$(TEST_BUILD_PATH_CXX)/%.$(OBJ_EXT): $(TEST_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< $(OBJ_OUT_FLAG) $@ $(DEP_OUT_CXXFLAG) $(TEST_DEP_PATH)/$(notdir $*).$(DEP_EXT)

# Compile C++ tests (recursive)
$(TEST_BUILD_PATH_CXX)/%.$(OBJ_EXT): $(TEST_PATH)/**/%.cpp
	$(CXX) $(CXXFLAGS) -c $< $(OBJ_OUT_FLAG) $@ $(DEP_OUT_CXXFLAG) $(TEST_DEP_PATH)/$(notdir $*).$(DEP_EXT)

ifeq ($(USE_CUDA), 1)
# Compile CUDA sources (main directory)
$(BUILD_PATH_CUDA)/%.$(OBJ_EXT): $(SRC_PATH)/%.cu
	$(NVCC) $(NVCCFLAGS) -c $< $(OBJ_OUT_FLAG) $@

# Compile CUDA sources (recursive)
$(BUILD_PATH_CUDA)/%.$(OBJ_EXT): $(SRC_PATH)/**/%.cu
	$(NVCC) $(NVCCFLAGS) -c $< $(OBJ_OUT_FLAG) $@

# Compile CUDA tests (main directory)
$(TEST_BUILD_PATH_CUDA)/%.$(OBJ_EXT): $(TEST_PATH)/%.cu
	$(NVCC) $(NVCCFLAGS) -c $< $(OBJ_OUT_FLAG) $@

# Compile CUDA tests (recursive)
$(TEST_BUILD_PATH_CUDA)/%.$(OBJ_EXT): $(TEST_PATH)/**/%.cu
	$(NVCC) $(NVCCFLAGS) -c $< $(OBJ_OUT_FLAG) $@
endif
#### END GENERIC SOURCE RECIPES ####

#### BEGIN SPECIFIC TARGET DEPENDENCIES ####
# Make on Windows has some problems...
ifneq ($(OS), Windows_NT)
    -include $(DEP)
endif
#### END SPECIFIC TARGET DEPENDENCIES ####
