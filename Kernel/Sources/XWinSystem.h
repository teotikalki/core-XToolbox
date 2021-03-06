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
#ifndef __XWinSystem__
#define __XWinSystem__

BEGIN_TOOLBOX_NAMESPACE

class VString;

typedef DWORD pid;

class XTOOLBOX_API XWinSystem 
{
public:

	// public api for VSystem
	static	bool			DisplayNotification( const VString& inTitle, const VString& inMessage, EDisplayNotificationOptions inOptions, ENotificationResponse *outResponse);

	static	bool			IsSystemVersionOrAbove( SystemVersion inSystemVersion);

	static	void			GetProcessList(std::vector<pid> &pids);
	static	void			GetProcessName(uLONG inProcessID, VString &outProcessName);
	static	void			GetProcessPath(uLONG inProcessID, VString &outProcessPath);
	static	sLONG8			GetApplicationPhysicalMemSize();
	static	bool			KillProcess(uLONG inProcessID);

	// private api
	static	bool			GUIDisplayNotification( HWND inParent, const VString& inTitle, const VString& inMessage, EDisplayNotificationOptions inOptions);
	static	bool			GetMajorAndMinorFromSystemVersion( SystemVersion inSystemVersion, DWORD *outMajor, DWORD *outMinor);
	static	SystemVersion	GetSystemVersion();
};

typedef XWinSystem XSystemImpl;

typedef	BOOL (WINAPI *ENUMPROCESSES)(DWORD *pProcessIds, DWORD cb, DWORD *pBytesReturned);

END_TOOLBOX_NAMESPACE

#endif
