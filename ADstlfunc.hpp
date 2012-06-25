// ADstlfunc.hpp:  Include File
//
// Copyright 2007 Ian Gordon
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__ADSTLFUNC_HPP__)
#define __ADSTLFUNC_HPP__


namespace ad
{

	//Meyers, Effective STL, Item 7
	struct DeleteObject
	{
		template<typename T>
		void operator()(const T* ptr) const
		{
			delete ptr;
			ptr = NULL; //IEG
		}
	};

} //namespace ad

#endif // !defined(__ADSTLFUNC_HPP__)
