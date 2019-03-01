/******************** (C) COPYRIGHT 2007 - 2013 OSRAM AG *********************************************************************************
* File Name       : XMC1xxx.h
* Description     : Hardware-dependent definitions to be included ONLY in hardware dependent c-files.
*                   Support of infineon XMC1xxx-family
* Author(s)       : Jutta Avril (JA)
* Created         : 20.09.2013 (JA)
* Modified by     : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
**********************************************************************************************************************************/

#ifdef XMC1200
	#include "XMC1200.h"
#else
	#include "XMC1300.h"
#endif

#include "intrinsics.h"
