// ---------------------------------------------------------------------------------------------------------------------
// MpcInfo_userinterface.h
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
// Development Electronics for SSL
// Parkring 33
// 85748 Garching
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: Joachim Eisenberg, 2016-04
// $Author: j.eisenberg $
// $Revision: 4533 $ 
// $Date: 2016-06-23 21:50:08 +0800 (Thu, 23 Jun 2016) $
// $Id: MpcInfo_Services.h 4533 2016-06-23 13:50:08Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Info/tags/V3_3/Source/MpcInfo_Services.h $
// ---------------------------------------------------------------------------------------------------------------------

// services functions for device parameter monitoring in General Info Page
bool MpcInfoServices_GetLightLevelState( void );
uint8_t MpcInfoServices_GetActualTemperature( void );          
uint8_t MpcInfoServices_GetSupplyVoltage( void );       
bool MpcInfoServices_GetOvervoltageShutDownState( void );     



