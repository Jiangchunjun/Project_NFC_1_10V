# ----------------------------------------------------------------------------------------------------------------------
#  Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
# ----------------------------------------------------------------------------------------------------------------------
#  (c) Osram GmbH
#      DS D EC - MCC
#      Parkring 33
#      85748 Garching
#
#
#  The content of this file is intellectual property of OSRAM GmbH. It is
#  confidential and not intended for any public release. All rights reserved.
#
# Indent style: CMakeLists.txt specific, replace tabs by spaces, 2 spaces per indentation level
#
#  $Author: w.limmer $
#  $Revision: 16212 $
#  $Date: 2018-12-14 19:34:44 +0800 (Fri, 14 Dec 2018) $
#  $Id: CMakeLists.txt 16212 2018-12-14 11:34:44Z w.limmer $
#  $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nfc/tags/v2.7/Src/CMakeLists.txt $


cmake_minimum_required(VERSION 3.3)
set ( PROJECT_NAME_ORG ${PROJECT_NAME} ) # save name of the outer frame
set ( CMAKE_MODULE_PATH . "${CMAKE_SOURCE_DIR}/CMake" "${CMAKE_SOURCE_DIR}/Build" "${CMAKE_SOURCE_DIR}/Tools")

project("Module_Nfc${NFC_VARIANT}")
include ( Basics )


#-----------------------------------------------------------------------------------------------------------------------
# CMake Module

add_library ( ${PROJECT_NAME}
  EXCLUDE_FROM_ALL
  nfc_config.c
  nfc_module.c
)

target_compile_options( ${PROJECT_NAME}
  PRIVATE
)

target_compile_definitions( ${PROJECT_NAME}
  PRIVATE
    ${${PROJECT_NAME_UPPER}_DUT_DEFINE}    # Set by Basics.cmake when the parent frame is the unit test of the module
  PUBLIC
    ${NFC_VARIANT_DEFINES}
)

target_include_directories( ${PROJECT_NAME}
  PRIVATE
  PUBLIC
    .
)

target_link_libraries( ${PROJECT_NAME}
  Config_h
  Module_Kernel
  Module_Acl
  Module_Alloc
  Module_I2c

  # --------------------------------------------------------------------------------
  Module_Dali     # VERY UGLY HACK: Needed for the (variable) hardware_version, which currently is stored in DALI MB0
  # Remove this dependency as soon as ever possible

)
