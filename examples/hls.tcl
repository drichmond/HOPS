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
#       hls.tcl
#
# DESCRIPTION :
#       Vivado HLS compilation script for compiling, synthesizing, and
#       exporting HLS functions as Vivado IP. 
#
# NOTES :
#       Vivado HLS cannot parse command-line .tcl arguments. Instead,
#       arguments are passed as environment variables.
#
#       This file expects four environment variables to be set:
#       
#       RUN_TYPE : String
#
#           RUN_TYPE dictates the type of compilation Vivado HLS will
#           perform. It expects one of two values:
#
#               csim: Runs Vivado HLS C-Simulation (but not synthesis)
#
#               synth: Runs Vivado HLS C-Synthesis and IP Export
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
#       This file also defines BIT_ACCURATE when running Vivado HLS
#       Simulation and synthesis. This allows bit-accurate types to be
#       used when Vivado HLS is run, and standard C-Types when it is
#       not.
#
# AUTHOR : Dustin Richmond (drichmond)
#
# **********************************************************************

if {[info exists $::env(RUN_TYPE)] != 0} {
    puts ""
    set errmsg "Environment variable RUN_TYPE not defined"
    catch {common::send_msg_id "HOF-000" "ERROR" $errmsg}
    return 1
}

set RUN_TYPE $::env(RUN_TYPE)
if { ($::env(RUN_TYPE) != "csim") && ($::env(RUN_TYPE) != "synth") } {
    puts ""
    set errmsg "Environment variable RUN_TYPE must be 'csim' or 'synth'"
    catch {common::send_msg_id "HOF-000" "ERROR" $errmsg}
    return 1
}


if {[info exists $::env(FUNCTIONS)] != 0} {
    puts ""
    set errmsg "Environment variable FUNCTIONS not defined"
    catch {common::send_msg_id "HOF-000" "ERROR" $errmsg}
    return 1
}
set FUNCTIONS [split $::env(FUNCTIONS) " "]


if {[info exists $::env(LIBRARY_PATH)] != 0} {
    puts ""
    set errmsg "Environment variable LIBRARY_PATH not defined"
    catch {common::send_msg_id "HOF-000" "ERROR" $errmsg}
    return 1
}
set LIBRARY_PATH $::env(LIBRARY_PATH)


if {[info exists $::env(UTILITY_PATH)] != 0} {
    puts ""
    set errmsg "Environment variable UTILITY_PATH not defined"
    catch {common::send_msg_id "HOF-000" "ERROR" $errmsg}
    return 1
}
set UTILITY_PATH $::env(UTILITY_PATH)


set INCLUDES "-I$LIBRARY_PATH -I$UTILITY_PATH"
set DEFINES "-DBIT_ACCURATE"

set project hls
open_project -reset $project
add_files test.cpp -cflags "-std=c++11 $DEFINES $INCLUDES"
add_files -tb test.cpp -cflags "-std=c++11 $DEFINES $INCLUDES"

open_solution base
csim_design -clean -clang_sanitizer
if { [string equal "synth" $RUN_TYPE ] } {
    foreach fn $FUNCTIONS {
	puts "-------------------- NOTE: Compiling Function $fn --------------------"
	open_solution $fn
	set_part {xc7z020clg400-1}
	set_top $fn
	create_clock -period 2

	csynth_design
	export_design -format ip_catalog -description "Function $fn from HOPS" \
	    -vendor "UCSD" -version "1.0" -display_name "Function $fn from HOPS"
    }
}
