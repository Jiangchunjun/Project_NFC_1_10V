// ---------------------------------------------------------------------------------------------------------------------
// MpcPower_Services.h
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
// Initial version: Jutta Avril, 2016-06-06
//
// $Author: j.eisenberg $
// $Revision: 4904 $
// $Date: 2016-07-20 16:50:10 +0800 (Wed, 20 Jul 2016) $
// $Id: MpcPower_Services.h 4904 2016-07-20 08:50:10Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Power/tags/V2_0/Source/MpcPower_Services.h $
//
// MpcPower_Service.h declares prototype functions for data exchange between the power info memory bank 
// and user specific code 
// Reference document: "Power Consumption Page Description", 2D1 2784481-000-02, MPC ID =3. 
// To enable define:
// #define MPC_POWER_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------

uint32_t MpcPowerServices_GetPowerConsumption( uint8_t channel );