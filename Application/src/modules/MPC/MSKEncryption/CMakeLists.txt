# ----------------------------------------------------------------------------------------------------------------------
#  Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
# ----------------------------------------------------------------------------------------------------------------------
#  (c) Osram GmbH
#      DS D EC - MCC
#      Parkring 33
#      85748 Garching
#
#
#  The content of this file is intellectual property of OSRAM GmbH, BU DS.
#  It is confidential and not intended for any public release.
#  All rights reserved.
#
#  Within the company the policy of "Who needs to know?" applies: Store this only at
#  locations, where EXCLUSIVELY people who really NEED it for their DAY-TO-DAY business
#  can access.
#   - Convenience or to save it for occasion are no valid arguments.
#   - "Departments shares" or other potential local practices normally don't fulfil
#     this policy.
#   - Please, especially be careful about backups, they are often forgotten and an usual
#     origin of diffusion.
#   - Also be careful with code coverage reporting tools: They normally include the full
#     sources, but the results often are published widely.
#
#
#  Indent style: Replace tabs by spaces, 2 spaces per indentation level
#
#  Change history, major milestones and credits:
#    2017-10, W.Limmer@osram.de: Initial version
#
#
#  $Author: w.limmer $
#  $Revision: 10148 $
#  $Date: 2017-10-06 18:33:28 +0800 (Fri, 06 Oct 2017) $
#  $Id: CMakeLists.txt 10148 2017-10-06 10:33:28Z w.limmer $
#  $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Encryption/releases/Rel1.2/CMakeLists.txt $
#



# ----------------------------------------------------------------------------------------------------------------------
# Pre-compiled encryption library

add_library( Module_Encryption STATIC IMPORTED GLOBAL)

# Binary
if ( NOT (${CMAKE_SYSTEM_NAME} MATCHES "Linux") )
  set_property(TARGET Module_Encryption
    PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/MskEncryption-i386-win.a"
  )
else()
  set_property(TARGET Module_Encryption
    PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/MskEncryption-i386-linux.a"
  )
endif()

# Header file
set_property ( TARGET Module_Encryption
  PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}"  # unfortunately absolute path needed
)
