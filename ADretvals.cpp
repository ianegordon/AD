// ADretvals.cpp:  Return Value Source File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#include "ADretvals.hpp"

namespace ad {

	bool Succeeded(const ERetVal eStatus)
	{ 
		return (0 <= static_cast<int>(eStatus) );
	}

	bool Failed(const ERetVal eStatus)
	{ 
		return (0 > static_cast<int>(eStatus) );
	}

	const char* RetValToSZ(const ERetVal eRetVal)
	{
		switch (eRetVal)
		{
		case SOK: return "SOK";
		case SNOWORK: return "SNOWORK";
		case SDB: return "SDB";
		case EFAIL:return "EFAIL";
		case ENOTIMPL: return "ENOTIMPL";
		case EINIT_FIRST: return "EINIT_FIRST";
		case EINIT_ALREADY: return "EINIT_ALREADY";
		case EPARAMETER: return "EPARAMETER";
		case EPOINTER: return "EPOINTER";
		case EDATA: return "EDATA";
		case EMEM_ALLOC: return "EMEM_ALLOC";
		case EMEM_DEALLOC: return "EMEM_DEALLOC";
		case EOUTOFBOUNDS: return "EOUTOFBOUNDS";
		case EOUTOFSPACE: return "EOUTOFSPACE";
		case EPERMISSION: return "EPERMISSION";
		case EDUPLICATION: return "EDUPLICATION";
		case EWRONG_STATE: return "EWRONG_STATE";
		case EINVALID: return "EINVALID";
		case EINVALID_NUMBER: return "EINVALID_NUMBER";
		case EINVALID_TOKEN: return "EINVALID_TOKEN";
		case ESYSTEMLIMIT: return "ESYSTEMLIMIT";
		case EIO_NOTFOUND: return "EIO_NOTFOUND";
		case ENET_WOULDBLOCK: return "ENET_WOULDBLOCK";
		case ENET_CONNABORTED: return "ENET_CONNABORTED";
		case ENET_CONNRESET: return "ENET_CONNRESET";
		default:
			return "???RetVal";
		}
	}
	
} // namespace ad

