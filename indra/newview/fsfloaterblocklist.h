/**
 * @file fsfloaterblocklist.h
 * @brief Class for the standalone blocklist in qikfox3D
 *
 * $LicenseInfo:firstyear=2012&license=viewerlgpl$
 * Phoenix qikfox3D Viewer Source Code
 * Copyright (c) 2012 Ansariel Hiller @ Second Life
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

#ifndef FS_FLOATERBLOCKLIST_H
#define FS_FLOATERBLOCKLIST_H

#include "llfloater.h"

class FSFloaterBlocklist : public LLFloater
{
public:
    FSFloaterBlocklist(const LLSD& seed);
    /*virtual*/ ~FSFloaterBlocklist();
    void onOpen(const LLSD& key) override;

    bool postBuild() override;

private:
    LLPanel* mBlockedListPanel;
};

#endif // FS_FLOATERBLOCKLIST_H
