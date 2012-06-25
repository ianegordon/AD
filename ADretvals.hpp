// ADretvals.hpp:  Return Value Include File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADRETVALS_HPP__)
#define __ADRETVALS_HPP__

namespace ad {

	//! Define list of error codes
	/*!
	ERetVal == 0 : Success
	ERetVal > 0 : Success w/ Warning
	ERetVal < 0 : ERROR
	*/

	enum ERetVal
	{
		// SUCCESS
		SOK = 0,
		SNOWORK,
		SDB, // Temporary value used when debugging.  These should not exist in release code
		// ERRORS
		EFAIL = -0x1fffffff,
		ENOTIMPL,
		EINIT_FIRST,
		EINIT_ALREADY,
		EPARAMETER,
		EPOINTER,
		EDATA,
		EMEM_ALLOC,
		EMEM_DEALLOC,
		EOUTOFBOUNDS,
		EOUTOFSPACE,
		EPERMISSION,
		EDUPLICATION,
		EWRONG_STATE,
		EINVALID,
		EINVALID_NUMBER,
		EINVALID_TOKEN,
		ESYSTEMLIMIT,
		//NOTE EIO is already defined
		EIO_NOTFOUND,
		ENET_WOULDBLOCK,
		ENET_CONNABORTED,
		ENET_CONNRESET
		//NOTE If you edit this list of values you MUST add it to RetValToSZ()
	};

	//! Test eRetVal for success
	/*!
	Test for success on an ERetVal
	*/
	extern bool Succeeded(const ERetVal eStatus);

	//! Test eRetVal for failure
	/*!
	Test for failure on an ERetVal
	*/
	extern bool Failed(const ERetVal eStatus);
	
	const char* RetValToSZ(const ERetVal eRetVal);

} // namespace ad

#endif // !defined(__ADRETVALS_HPP__)
