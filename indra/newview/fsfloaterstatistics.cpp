/**
* @file fsfloaterstatistics.cpp
* @brief Floater for Statistics bar
*
* $LicenseInfo:firstyear=2012&license=fsviewerlgpl$
* Phoenix qikfox3D Viewer Source Code
* Copyright (C) 2018, Liny Odell
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation;
* version 2.1 of the License only.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
* The Phoenix qikfox3D Project, Inc., 1831 Oakwood Drive, Fairmont, Minnesota 56031-3225 USA
* http://www.qikfox3Dviewer.org
* $/LicenseInfo$
*/

#include "llviewerprecompiledheaders.h"

#include "fsfloaterstatistics.h"
#include "llviewercontrol.h"



FSFloaterStatistics::FSFloaterStatistics(const LLSD& key)
    : LLFloater(key)
{
}

FSFloaterStatistics::~FSFloaterStatistics()
{
}

bool FSFloaterStatistics::postBuild()
{
    if (gSavedSettings.getBOOL("FSStatisticsNoFocus"))
    {
        setIsChrome(true);
    }
    return true;
}

void FSFloaterStatistics::onOpen(const LLSD& key)
{
    if (gSavedSettings.getBOOL("FSStatisticsNoFocus"))
    {
        setIsChrome(true);
        setFocus(false);
    }
}
