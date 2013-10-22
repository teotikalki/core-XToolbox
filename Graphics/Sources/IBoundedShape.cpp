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
#include "VGraphicsPrecompiled.h"
#include "IBoundedShape.h"


void IBoundedShape::ScaleSizeBy (GReal inHorizScale, GReal inVertScale)
{
	SetSizeBy(GetWidth() * (inHorizScale - 1), GetHeight() * (inVertScale - 1));
}


void IBoundedShape::ScalePosBy (GReal inHorizScale, GReal inVertScale)
{
	SetPosBy(GetX() * (inHorizScale - 1), GetY() * (inVertScale - 1));
}


void IBoundedShape::SetPosTo (const VPoint& inPos)
{
	SetPosBy(inPos.GetX() - GetX(), inPos.GetY() - GetY());
}


void IBoundedShape::SetSizeTo (GReal inWidth, GReal inHeight)
{
	assert(inWidth > -2);
	assert(inHeight > -2);
	
	SetSizeBy((inWidth != -1) ? inWidth - GetWidth() : 0, (inHeight != -1) ? inHeight - GetHeight() : 0);
}


void IBoundedShape::SetCoords (GReal inX, GReal inY, GReal inWidth, GReal inHeight)
{
	SetPosBy(inX - GetX(), inY - GetY());
	SetSizeBy(inWidth - GetWidth(), inHeight - GetHeight());
}

