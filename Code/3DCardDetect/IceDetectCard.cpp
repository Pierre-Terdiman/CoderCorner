///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains source code for card detection.
 *	\file		IceDetectCard.cpp
 *	\author		Pierre Terdiman
 *	\date		July, 07, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

using namespace IceRenderer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A function to detect the 3D card. Usage:
 *	
 *	// Detect the 3D card
 *	DDDEVICEIDENTIFIER2 devIdent;
 *	mDirectDraw->GetDeviceIdentifier(&devIdent, 0);
 *	
 *	// Catch IDs
 *	DWORD VID = devIdent.dwVendorId;
 *	DWORD DID = devIdent.dwDeviceId;
 *	DWORD Rev = devIdent.dwRevision;
 *	
 *	IHVCode Code = Detect3DCard(VID, DID, Rev);
 *	
 *	\fn			Detect3DCard(udword vid, udword did, udword rev)
 *	\return		IHVCode enum value
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IHVCode IceRenderer::Detect3DCard(DWORD vid, DWORD did, DWORD rev)
{
	// Here we go for the hell-ride....
				if(!vid)							return IHV_UNKNOWN;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else		if(vid==0x1002)
	{
				if(did==0x4742)						return IHV_ATI_RAGEPRO;
		else	if(did==0x4744)						return IHV_ATI_RAGEPRO;
		else	if(did==0x4749)						return IHV_ATI_RAGEPRO;
		else	if(did==0x4750)						return IHV_ATI_RAGEPRO;
		else	if(did==0x5245)						return IHV_ATI_RAGE128_GL;
		else	if(did==0x5246)						return IHV_ATI_RAGE128_GL;
		else	if(did==0x534B)						return IHV_ATI_RAGE128_GL;
		else	if(did==0x534C)						return IHV_ATI_RAGE128_GL;
		else	if(did==0x534D)						return IHV_ATI_RAGE128_GL;
		else	if(did==0x524B)						return IHV_ATI_RAGE128_VR;
		else	if(did==0x524C)						return IHV_ATI_RAGE128_VR;
		else	if(did==0x5345)						return IHV_ATI_RAGE128_VR;
		else	if(did==0x5346)						return IHV_ATI_RAGE128_VR;
		else	if(did==0x5347)						return IHV_ATI_RAGE128_VR;
		else	if(did>=0x5041 && did <=0x5046)		return IHV_ATI_RAGE128PRO_GL;
		else	if(did>=0x5047 && did <=0x5058)		return IHV_ATI_RAGE128PRO_VR;
		else	if(did==0x5345)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x5346)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x5347)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x5348)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x534B)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x534C)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x534D)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x534E)						return IHV_ATI_RAGE128_4X;
		else	if(did==0x5446)						return IHV_ATI_RAGE128PRO_ULTRA;
		else	if(did==0x544C)						return IHV_ATI_RAGE128PRO_ULTRA;
		else	if(did==0x5452)						return IHV_ATI_RAGE128PRO_ULTRA;
		else	if(did==0x5453)						return IHV_ATI_RAGE128PRO_ULTRA;
		else	if(did==0x5454)						return IHV_ATI_RAGE128PRO_ULTRA;
		else	if(did==0x5455)						return IHV_ATI_RAGE128PRO_ULTRA;
		else	if(did==0x4754)						return IHV_ATI_RAGEII;
		else	if(did==0x4755)						return IHV_ATI_RAGEII_PLUS;
		else	if(did==0x5656)						return IHV_ATI_RAGEIIC_PCI;
		else	if(did==0x4756)						return IHV_ATI_RAGEIIC_PCI;
		else	if(did==0x475A)						return IHV_ATI_RAGEIIC_AGP;
		else	if(did==0x4759)						return IHV_ATI_RAGEIIC_AGP;
		else	if(did==0x4757)						return IHV_ATI_RAGEIIC_AGP;
		else	if(did==0x474E)						return IHV_ATI_RAGEXC;
		else	if(did==0x474C)						return IHV_ATI_RAGEXC;
		else	if(did==0x4752)						return IHV_ATI_RAGEXL;
		else	if(did==0x474F)						return IHV_ATI_RAGEXL;
		else	if(did==0x474D)						return IHV_ATI_RAGEXL;
		else	if(did==0x4C47)						return IHV_ATI_RAGELT;
		else	if(did==0x4C50)						return IHV_ATI_RAGELTPRO;
		else	if(did==0x4C49)						return IHV_ATI_RAGELTPRO;
		else	if(did==0x4C42)						return IHV_ATI_RAGELTPRO;
		else	if(did==0x4C52)						return IHV_ATI_RAGE_MOBILITY;
		else	if(did==0x4C4D)						return IHV_ATI_RAGE_MOBILITY;
		else	if(did==0x4C46)						return IHV_ATI_RAGE128_MOBILITYM3;
		else	if(did==0x4C45)						return IHV_ATI_RAGE128_MOBILITYM3;
		else	if(did==0x4D4C)						return IHV_ATI_RAGE128_MOBILITYM4;
		else	if(did==0x4D46)						return IHV_ATI_RAGE128_MOBILITYM4;
		else	if(did==0x5147)						return IHV_ATI_RADEON;
		else	if(did==0x5146)						return IHV_ATI_RADEON;
		else	if(did==0x5145)						return IHV_ATI_RADEON;
		else	if(did==0x5144)						return IHV_ATI_RADEON;
		else	if(did==0x515A)						return IHV_ATI_RADEON_VE;
		else	if(did==0x5159)						return IHV_ATI_RADEON_VE;
		else	if(did==0x4C5A)						return IHV_ATI_RADEON_MOBILITY_M6;
		else	if(did==0x4C59)						return IHV_ATI_RADEON_MOBILITY_M6;
		else	if(did==0x4C57)						return IHV_ATI_RADEON_MOBILITY_7500;
		else	if(did==0x5157)						return IHV_ATI_RADEON_7500;
		else	if(did==0x514F)						return IHV_ATI_RADEON_8500;
		else	if(did==0x514E)						return IHV_ATI_RADEON_8500;
		else	if(did==0x514C)						return IHV_ATI_RADEON_8500;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x121A	&& did==0x0001)						return IHV_3DFX_VOODOO;
	else	if(vid==0x121A	&& did==0x0002)						return IHV_3DFX_VOODOO2;
	else	if(vid==0x121A	&& did==0x0003)						return IHV_3DFX_VOODOOBANSHEE;
	else	if(vid==0x121A	&& did==0x0005)						return IHV_3DFX_VOODOO3;
	else	if(vid==0x121A	&& did==0x0009)						return IHV_3DFX_VOODOO_5500_AGP;
	else	if(vid==0x1142	&& did==0x643D)						return IHV_3DFX_VOODOORUSH;			// Alliance AT25/AT3D based reference board
	else	if(vid==0x10D9	&& did==0x8626)						return IHV_3DFX_VOODOORUSHM;		// Macronix based reference board
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x8086	&& did==0x7800)						return IHV_INTEL_i740;
	else	if(vid==0x8086	&& did==0x7123)						return IHV_INTEL_i810;
	else	if(vid==0x8086	&& did==0x7212)						return IHV_INTEL_i810E;
	else	if(vid==0x8086	&& did==0x1132)						return IHV_INTEL_i815;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x102B	&& did==0x0519)						return IHV_MATROX_MILLENIUM;		// STORM
	else	if(vid==0x102B	&& did==0x051A)						return IHV_MATROX_MYSTIQUE;			// HURRICANE
	else	if(vid==0x102B	&& did==0x051B)						return IHV_MATROX_MILLENIUM_II_PCI;	// MISTRAL_PCI
	else	if(vid==0x102B	&& did==0x051F)						return IHV_MATROX_MILLENIUM_II_AGP;	// MISTRAL_AGP
	else	if(vid==0x102B	&& did==0x1000)						return IHV_MATROX_G100_PCI;			// TWISTER PCI
	else	if(vid==0x102B	&& did==0x1001)						return IHV_MATROX_G100_AGP;			// TWISTER AGP
	else	if(vid==0x102B	&& did==0x0520)						return IHV_MATROX_G200_PCI;			// ECLIPSE PCI
	else	if(vid==0x102B	&& did==0x0521)						return IHV_MATROX_G200_AGP;			// ECLIPSE AGP
	else	if(vid==0x102B	&& did==0x0525)						return IHV_MATROX_G400_G450;		// TOUCAN
	else	if(vid==0x102B	&& did==0x2527)						return IHV_MATROX_G550;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x10DE	&& did==0x0020)						return IHV_NVIDIA_RIVATNT;
	else	if(vid==0x10DE	&& did==0x0029)						return IHV_NVIDIA_RIVATNT2ULTRA;
	else	if(vid==0x10DE	&& did==0x002C)						return IHV_NVIDIA_RIVATNT2VANTA;
	else	if(vid==0x10DE	&& did==0x002D)						return IHV_NVIDIA_RIVATNT2M64;
	else	if(vid==0x10DE	&& did>=0x0028 && did<=0x00FF)		return IHV_NVIDIA_RIVATNT2;
	else	if(vid==0x10DE	&& did>=0x0100 && did<=0x0103)		return IHV_NVIDIA_GEFORCE256;		// NV10
	else	if(vid==0x10DE	&& did==0x0018)						return IHV_NVIDIA_RIVA128;
	else	if(vid==0x10DE	&& did==0x0019)						return IHV_NVIDIA_RIVA128;

	else	if(vid==0x10DE	&& did==0x0150 && rev==163)			return IHV_NVIDIA_GEFORCE2_GTS;
	else	if(vid==0x10DE	&& did==0x0151)						return IHV_NVIDIA_GEFORCE2_GTS;
	else	if(vid==0x10DE	&& did==0x0110 && rev==161)			return IHV_NVIDIA_GEFORCE2_MX;

	else	if(vid==0x10DE	&& did==0x0008)						return IHV_NVIDIA_NV1;
	else	if(vid==0x10DE	&& did==0x00A0)						return IHV_NVIDIA_TNT2_ALADDIN;
	else	if(vid==0x10DE	&& did==0x0101)						return IHV_NVIDIA_GEFORCEDDR;		// NV10
	else	if(vid==0x10DE	&& did==0x0103)						return IHV_NVIDIA_QUADRO;			// NV10
//	else	if(vid==0x10DE	&& did==0x0110)						return IHV_NVIDIA_GEFORCE256;
	else	if(vid==0x10DE	&& did==0x0152)						return IHV_NVIDIA_GEFORCE2_ULTRA;
	else	if(vid==0x10DE	&& did==0x0153)						return IHV_NVIDIA_QUADRO2_PRO;
	else	if(vid==0x10DE	&& did==0x0200)						return IHV_NVIDIA_GEFORCE3;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x12D2	&& did==0x0018)						return IHV_NVIDIA_RIVA128;
	else	if(vid==0x12D2	&& did==0x0019)						return IHV_NVIDIA_RIVA128;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x14AF	&& did==0x5008)						return IHV_GUILLEMOT_TNTVANTA;		// Maxi Gamer Phoenix 2
	else	if(vid==0x14AF	&& did==0x5810)						return IHV_GUILLEMOT_TNT2;			// Maxi Gamer Xentor
	else	if(vid==0x14AF	&& did==0x5820)						return IHV_GUILLEMOT_TNT2ULTRA;		// Maxi Gamer Xentor 32
	else	if(vid==0x14AF	&& did==0x4d20)						return IHV_GUILLEMOT_TNT2M64;		// Maxi Gamer Cougar
	else	if(vid==0x14AF	&& did==0x5620)						return IHV_GUILLEMOT_TNT2M64V;		// Maxi Gamer Gougar Video Edition
	else	if(vid==0x14AF	&& did==0x5020)						return IHV_GUILLEMOT_GEFORCE256;	// Maxi Gamer 3D Prophet
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x3D3D	&& did==0x0001 && rev==0x00)		return IHV_3DLABS_300SX;
	else	if(vid==0x3D3D	&& did==0x0001 && rev==0x02)		return IHV_3DLABS_300SXRO2;
	else	if(vid==0x3D3D	&& did==0x0003 && rev==0x01)		return IHV_3DLABS_DELTA;
	else	if(vid==0x3D3D	&& did==0x0002 && rev==0x01)		return IHV_3DLABS_500TX;
	else	if(vid==0x3D3D	&& did==0x0004 && rev==0x01)		return IHV_3DLABS_PERMEDIA;
	else	if(vid==0x3D3D	&& did==0x0007 && rev==0x01)		return IHV_3DLABS_P2;
	else	if(vid==0x3D3D	&& did==0x0006 && rev==0x01)		return IHV_3DLABS_MX;
	else	if(vid==0x3D3D	&& did==0x0008 && rev==0x01)		return IHV_3DLABS_GAMMA;
	else	if(vid==0x3D3D	&& did==0x0009 && rev==0x01)		return IHV_3DLABS_PS2_ST;
	else	if(vid==0x3D3D	&& did==0x000A && rev==0x01)		return IHV_3DLABS_P3;
	else	if(vid==0x3D3D	&& did==0x000B && rev==0x01)		return IHV_3DLABS_R3;
	else	if(vid==0x3D3D	&& did==0x000C && rev==0x01)		return IHV_3DLABS_P4;
	else	if(vid==0x3D3D	&& did==0x000D && rev==0x01)		return IHV_3DLABS_R4;
	else	if(vid==0x3D3D	&& did==0x000E && rev==0x01)		return IHV_3DLABS_G2;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x104C	&& did==0x3D04 && rev==0x01)		return IHV_3DLABS_TI_P1;
	else	if(vid==0x104C	&& did==0x3D07 && rev==0x01)		return IHV_3DLABS_TI_P2;
	else	if(vid==0x104C	&& did==0x3D07 && rev==0x11)		return IHV_3DLABS_TI_P2A;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x1013	&& did==0x00B8)						return IHV_CIRRUSLOGIC_5446;
	else	if(vid==0x1013	&& did==0x00D0)						return IHV_CIRRUSLOGIC_5462;
	else	if(vid==0x1013	&& did==0x00D4)						return IHV_CIRRUSLOGIC_5464;
	else	if(vid==0x1013	&& did==0x00D6)						return IHV_CIRRUSLOGIC_5465;
	else	if(vid==0x1013	&& did==0x1100)						return IHV_CIRRUSLOGIC_6729;
	else	if(vid==0x1013	&& did==0x6001)						return IHV_CIRRUSLOGIC_4610;
	else	if(vid==0x1013	&& did==0x6003)						return IHV_CIRRUSLOGIC_4614;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x1163	&& did==0x0001)						return IHV_RENDITION_V1000;
	else	if(vid==0x1163	&& did==0x2000)						return IHV_RENDITION_V2100_V2200;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x5333	&& did==0x8A22)						return IHV_S3_SAVAGE4;
	else	if(vid==0x5333	&& did==0x8C10)						return IHV_S3_SAVAGEMX;
	else	if(vid==0x5333	&& did==0x9102)						return IHV_S3_SAVAGE200;
	else	if(vid==0x5333	&& did==0x883D)						return IHV_S3_VIRGE_VX;
	else	if(vid==0x5333	&& did==0x8C03)						return IHV_S3_VIRGE_MX_MV;
	else	if(vid==0x5333	&& did==0x8C02)						return IHV_S3_VIRGE_MX_PLUS;
	else	if(vid==0x5333	&& did==0x8C01)						return IHV_S3_VIRGE_MX;
	else	if(vid==0x5333	&& did==0x8A10)						return IHV_S3_VIRGE_GX2;
	else	if(vid==0x5333	&& did==0x8A01)						return IHV_S3_VIRGE_DX_GX;
	else	if(vid==0x5333	&& did==0x8901)						return IHV_S3_TRIO64V2_DX_GX;
	else	if(vid==0x5333	&& did==0x8814)						return IHV_S3_TRIO64UV_PLUS;
	else	if(vid==0x5333	&& did==0x8812)						return IHV_S3_AURORA64V_PLUS;
	else	if(vid==0x5333	&& did==0x8815)						return IHV_S3_AURORA128;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else	if(vid==0x104A	&& did==0x0010)						return IHV_POWERVR_KYRO;

																return IHV_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A function to translate the 3D card code into a string.
 *	\fn			GetCardName(IHVCode code)
 *	\return		card name
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* IceRenderer::GetCardName(IHVCode code)
{
	static char CardNames[IHV_LAST][40] = {
		"unknown card",
		"ATI Rage Pro",
		"ATI Rage 128 Pro GL",
		"ATI Rage 128 Pro VR",
		"ATI Rage 128 Pro Ultra",
		"ATI Rage 128 GL",
		"ATI Rage 128 VR",
		"ATI Rage 128 4X",
		"ATI Rage II",
		"ATI Rage II Plus",
		"ATI Rage IIC PCI",
		"ATI Rage IIC AGP",
		"ATI Rage XC (Rage Pro)",
		"ATI Rage XL (Rage Pro)",
		"ATI Rage LT (Rage II)",
		"ATI Rage LT Pro (Rage Pro)",
		"ATI Rage Mobility M/M1/P(Rage Pro)",
		"ATI Rage 128 Mobility M3 (Rage 128 Pro)",
		"ATI Rage 128 Mobility M4 (Rage 128 Pro)",
		"ATI Radeon",
		"ATI Radeon VE (DualCRT, NoTCL)",
		"ATI Radeon Mobility M6 (Radeon VE)",
		"ATI Radeon Mobility 7500 (Radeon 7500)",
		"ATI Radeon 7500",
		"ATI Radeon 8500",
		"3DFX Voodoo 1",
		"3DFX Voodoo 2",
		"3DFX Voodoo Banshee",
		"3DFX Voodoo 3",
		"3DFX Voodoo 5500 AGP",
		"3DFX Voodoo Rush",
		"3DFX Voodoo Rush",
		"INTEL i740",
		"INTEL i810",
		"INTEL i810E",
		"INTEL i815",
		"MATROX Millenium",
		"MATROX Mystique",
		"MATROX Millenium II PCI",
		"MATROX Millenium II AGP",
		"MATROX G100 PCI",
		"MATROX G100 AGP",
		"MATROX G200 PCI",
		"MATROX G200 AGP",
		"MATROX G400/G450",
		"MATROX G550",
		"NVIDIA Riva TNT",
		"NVIDIA Riva TNT2 Ultra",
		"NVIDIA Riva TNT2 Vanta",
		"NVIDIA Riva TNT2 M64",
		"NVIDIA Riva TNT2",
		"NVIDIA GeForce 256",
		"NVIDIA GeForce 2 GTS",
		"NVIDIA GeForce 2 MX",
		"NVIDIA Riva 128",
		"NVIDIA NV1",
		"NVIDIA TNT2 Aladdin",
		"NVIDIA GeForce DDR",
		"NVIDIA Quadro",
		"NVIDIA GeForce2 Ultra",
		"NVIDIA Quadro2 Pro",
		"NVIDIA GeForce3",
		"GUILLEMOT TNT Vanta",
		"GUILLEMOT TNT2",
		"GUILLEMOT TNT2 Ultra",
		"GUILLEMOT TNT2 M64",
		"GUILLEMOT TNT2 M64",
		"GUILLEMOT GeForce 256",
		"3DLABS Glint 300 SX",
		"3DLABS Glint 300 SXRO2",
		"3DLABS Glint Delta",
		"3DLABS Glint 500 TX",
		"3DLABS PERMEDIA",
		"3DLABS PERMEDIA 2",
		"3DLABS Glint MX",
		"3DLABS GAMMA",
		"3DLABS PS2 ST",
		"3DLABS PERMEDIA 3 (OxygenVX1)",
		"3DLABS R3",
		"3DLABS PERMEDIA 4",
		"3DLABS R4",
		"3DLABS G2",
		"3DLABS TI P1",
		"3DLABS TI P2",
		"3DLABS TI P2A",
		"CIRRUS LOGIC 5446",
		"CIRRUS LOGIC 5462",
		"CIRRUS LOGIC 5464",
		"CIRRUS LOGIC 5465",
		"CIRRUS LOGIC 6729",
		"CIRRUS LOGIC 4610",
		"CIRRUS LOGIC 4614",
		"RENDITION V1000",
		"RENDITION V2100/V2200",
		"S3 SAVAGE4",
		"S3 SAVAGE MX",
		"S3 SAVAGE 200",
		"S3 VIRGE VX",
		"S3 VIRGE MX/MV",
		"S3 VIRGE MX+",
		"S3 VIRGE MX",
		"S3 VIRGE GX2",
		"S3 VIRGE DX/GX",
		"S3 TRIO64V2 DX/GX",
		"S3 TRIO64UV+",
		"S3 AURORA64V+",
		"S3 AURORA128",
		"POWERVR KYRO",
	};

	return CardNames[code];
}
