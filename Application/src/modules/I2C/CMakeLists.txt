# ----------------------------------------------------------------------------------------------------------------------
#  Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
# ----------------------------------------------------------------------------------------------------------------------
# (c) Osram GmbH
#     DS D EC - MCC
#     Parkring 33
#     85748 Garching
#
#
# The content of this file is intellectual property of OSRAM GmbH. It is
# confidential and not intended for any public release. All rights reserved.
#
# Indent style: CMakeLists.txt specific, replace tabs by spaces, 2 spaces per indentation level
#
# Change history, major milestones and credits:
#   2017-05, W.Limmer@osram.de: Initial version
#
#
# $Author: m.nordhausen $
# $Revision: 12902 $
# $Date: 2018-04-17 21:05:15 +0800 (Tue, 17 Apr 2018) $
# $Id: CMakeLists.txt 12902 2018-04-17 13:05:15Z m.nordhausen $
# $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.4/Src/CMakeLists.txt $
#


cmake_minimum_required(VERSION 3.2)
project( Module_I2c )

# ATTENTION ATTENTION VERALTET
#-----------------------------------------------------------------------------------------------------------------------
# Toolbox, Development versions (head revision for those modules under deeper investigation, others mostly tagged)
#
# The Toolbox for the Virtual ECG provides all basic modules incl:
#  - Crc
#  - Crc16
#  - Nvm + fake for Nvm hardware driver ( still inside, but to be separated )
#  - Fake for I2C HW driver / NFC-Tag (to be removed too: Dependency on module NFC for the headers is anoying )
#
# Removed:
# - Kernel and configuration dir


add_library ( ${PROJECT_NAME}
  i2c_config.c
  i2c_module.c
)

target_compile_options( ${PROJECT_NAME}
  PRIVATE
)

target_compile_definitions( ${PROJECT_NAME}
  PRIVATE
)

target_include_directories ( ${PROJECT_NAME}
  PRIVATE
  PUBLIC
    .
)

target_link_libraries ( ${PROJECT_NAME}
  Config_h
  Module_Kernel
  Module_HWdrivers
)
