/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
#ifndef __VObject__
#define __VObject__

#include "Kernel/Sources/VKernelTypes.h"
#include "Kernel/Sources/VTextTypes.h"

BEGIN_TOOLBOX_NAMESPACE

// Needed declarations
class VString;
class VCppMemMgr;

class XTOOLBOX_API VObject
{
public:
							VObject()												{;}
	virtual					~VObject()												{;}
	
	// Operators
			void*			operator new( size_t inSize);
			void			operator delete( void* inAddr);

			void*			operator new( size_t /*inSize*/, void* inAddr)			{ return inAddr; }
			void			operator delete( void* /*inAddr*/, void* /*inAddr*/)	{};

	// Debug utility
			void			DebugGetClassName( VString& outName) const;
			void			DebugDump( VString& outDump) const;

	// Override to dump internal informations
	virtual	CharSet			DebugDump (char* inTextBuffer, sLONG& inBufferSize) const;
	
	// Allocator support
	static VCppMemMgr*		GetAllocator( bool inAlternateAllocator = false)		{ return (inAlternateAllocator ? sAlternateCppMemMgr : sCppMemMgr); }
	static void				SetAlternateAllocator( VCppMemMgr* inNewAllocator)		{ sAlternateCppMemMgr = inNewAllocator; }
	
	// Private init/deinit methods
	static void				_ProcessEntered( VCppMemMgr* inDefaultAllocator)		{ sCppMemMgr = inDefaultAllocator; }
	static void				_ProcessExited()										{ sCppMemMgr = NULL; sAlternateCppMemMgr = NULL; }

	static VCppMemMgr*		GetMainMemMgr()											{ return sCppMemMgr; }
	static VCppMemMgr*		GetAlternateMemMgr()									{ return sAlternateCppMemMgr; }

protected:
	static VCppMemMgr*		sCppMemMgr;
	static VCppMemMgr*		sAlternateCppMemMgr;
};

END_TOOLBOX_NAMESPACE

#endif
