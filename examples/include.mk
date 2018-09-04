# ----------------------------------------------------------------------
# Copyright (c) 2018, The Regents of the University of California All
# rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#
#     * Neither the name of The Regents of the University of California
#       nor the names of its contributors may be used to endorse or
#       promote products derived from this software without specific
#       prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL REGENTS OF THE
# UNIVERSITY OF CALIFORNIA BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
# ----------------------------------------------------------------------
# **********************************************************************
# FILENAME :
#       include.mk
#
# DESCRIPTION :
#       Makefile fragment for compiling testbenches, running tests,
#       synthesizing Vivado HLS IP, and exporting IP files.
#
# NOTES :
#       Vivado HLS cannot parse command-line .tcl arguments. Instead,
#       arguments are passed as environment variables. This makefile
#       conditionally sets three environment variables:
#           
#       FUNCTIONS : String
#
#           FUNCTIONS specifies which functions in test.cpp Vivado HLS
#           should synthesize and export as Vivado IP. It is formatted
#           as a tcl list -- A string with individual elements
#           separated by spaces. 
#
#           See the makefiles in each testing directory for more
#           examples
#       
#       LIBRARY_PATH : String
#
#           LIBRARY_PATH specifies the path of the HOPS library. This
#           can be a relative or absolute path. If a relative path is
#           used, the relative path must be with respect to the
#           makefile builds the test functions.
#
#       UTILITY_PATH : String
#
#           UTILITY_PATH specifies the path of utility.hpp file. It
#           can be a relative or absolute path. If a relative path is
#           used, the relative path must be with respect to the
#           makefile builds the test functions.
#
#       SYNTH_PATH : String
#
#           SYNTH_PATH specifies the path of hls.tcl file. It can
#           be a relative or absolute path. If a relative path is
#           used, the relative path must be with respect to the
#           makefile builds the test functions.
#
#       TEST_FILE : String
#
#           TEST_FILES specifies the name of the top-level testbench
#           file. 
#
#       INCLUDES : String
#
#           A list of all paths to be searched for included header files
#
#       LIBRARY_FILES : String
#
#           Paths to any of the library header files included.
#
# AUTHOR : Dustin Richmond (drichmond)
#
# **********************************************************************

# Variables expected by the hls.tcl script used by Vivado HLS
LIBRARY_PATH  ?= ../../include
UTILITY_PATH  ?= ..
FUNCTIONS     ?= ""

# Other variables used by this makefile: 
SYNTH_PATH    ?= ..
TEST_FILE     ?= test.cpp
INCLUDES      +=-I$(LIBRARY_PATH) -I$(UTILITY_PATH)
LIBRARY_FILES ?= 

all: sw_test csim

sw_test: sw_test.log
sw_test.log: sw_test.run
	./sw_test.run | tee sw_test.log

sw_test.run: $(TEST_FILE) $(LIBRARY_FILES) $(HEADER_FILES)	\
		$(UTILITY_PATH)/utility.hpp
	g++ -std=c++11 $(TEST_FILE) $(INCLUDES) -o $@

csim: csim.log
csim.log: $(TEST_FILE) $(LIBRARY_FILES) $(HEADER_FILES)	\
		$(UTILITY_PATH)/utility.hpp

	RUN_TYPE=csim FUNCTIONS=$(FUNCTIONS)		\
		LIBRARY_PATH=$(LIBRARY_PATH)		\
		UTILITY_PATH=$(UTILITY_PATH) vivado_hls	\
		$(SYNTH_PATH)/hls.tcl | tee csim.log

synth: synth.log
synth.log: $(TEST_FILE) $(LIBRARY_FILES) $(HEADER_FILES)	\
		$(UTILITY_PATH)/utility.hpp

	RUN_TYPE=synth FUNCTIONS=$(FUNCTIONS)		\
		LIBRARY_PATH=$(LIBRARY_PATH)		\
		UTILITY_PATH=$(UTILITY_PATH) vivado_hls	\
		$(SYNTH_PATH)/hls.tcl | tee synth.log

synth.rpt: synth
	@echo "|       Name      | BRAM_18K| DSP48E|   FF   |   LUT  |"\ > synth.rpt
	@find synth/*/syn -name "*.rpt" | xargs -I{} grep -H '^|Total' {} >> synth.rpt
	@find synth/*/syn -name "*.rpt" | xargs -I{} grep -A 10 -H 'Latency' {} >> synth.rpt

.PHONY:clean 
clean: 
	rm -rf sw_test.run
	rm -rf *.log *.rpt
	rm -rf hls
