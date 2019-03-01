#-------------------------------------------------------------------------------
# include pre processing makefile
#-------------------------------------------------------------------------------
-include ../../../compile/pre_processing_makefile.mk

#-------------------------------------------------------------------------------
# module name
#-------------------------------------------------------------------------------
target_module := module_supplymonitor

#-------------------------------------------------------------------------------
# python generated file to support version infos
#-------------------------------------------------------------------------------
# build_version_file := $(project_root)/src/Config/build_version.h

#-------------------------------------------------------------------------------
# define all include directories
# should NOT be used, because it is required to add include dir also to cflags_include
# which is not a recursive variable
#-------------------------------------------------------------------------------
# include_dirs += .

#-------------------------------------------------------------------------------
# define test source directories
#-------------------------------------------------------------------------------
local_src_test_dir := $(source_dir)/tests

#-------------------------------------------------------------------------------
# define all source files
#-------------------------------------------------------------------------------
local_src := $(wildcard $(source_dir)/*.c)
local_src += ../../kernel/filter.c

local_src_test := $(wildcard $(source_dir)/tests/*.c)


#-------------------------------------------------------------------------------
# include post processing makefile
#-------------------------------------------------------------------------------
include $(common_dir)/post_processing_makefile.mk
