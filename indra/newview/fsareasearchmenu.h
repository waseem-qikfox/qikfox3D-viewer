/**
 * @file fsareasearchmenu.h
 * @brief Menu used by area search
 *
 * $LicenseInfo:firstyear=2014&license=viewerlgpl$
 * Phoenix qikfox3D Viewer Source Code
 * Copyright (c) 2014 Ansariel Hiller
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

#ifndef FS_AREASEARCHMENU_H
#define FS_AREASEARCHMENU_H

#include "lllistcontextmenu.h"

class FSAreaSearchMenu : public LLListContextMenu
{
public:
    /*virtual*/ LLContextMenu* createMenu();
private:
    void onContextMenuItemClick(const LLSD& userdata);
    bool onContextMenuItemEnable(const LLSD& userdata);
    bool onContextMenuItemVisibleRLV(const LLSD& userdata);
};

extern FSAreaSearchMenu gFSAreaSearchMenu;

#endif // FS_AREASEARCHMENU_H
