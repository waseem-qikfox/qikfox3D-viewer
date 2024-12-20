/**
 * @file fsfloaterprotectedfolders.h
 * @brief Class for the protected folders floater
 *
 * $LicenseInfo:firstyear=2020&license=viewerlgpl$
 * Phoenix qikfox3D Viewer Source Code
 * Copyright (c) 2020 Ansariel Hiller @ Second Life
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

#ifndef FS_FLOATERPROTECTEDFOLDERS_H
#define FS_FLOATERPROTECTEDFOLDERS_H

#include "llfloater.h"

class LLButton;
class LLFilterEditor;
class LLScrollListCtrl;

class FSFloaterProtectedFolders : public LLFloater
{
public:
    FSFloaterProtectedFolders(const LLSD& key);
    virtual ~FSFloaterProtectedFolders();

    bool postBuild() override;
    void onOpen(const LLSD& info) override;
    void draw() override;
    bool handleKeyHere(KEY key, MASK mask) override;
    bool hasAccelerators() const override { return true; }

private:
    void updateList();

    void handleRemove();
    void onFilterEdit(const std::string& search_string);
    void onDoubleClick();

    bool mInitialized;

    std::string mFilterSubString;
    std::string mFilterSubStringOrig;

    boost::signals2::connection mProtectedCategoriesChangedCallbackConnection;

    LLScrollListCtrl*   mFolderList;
    LLButton*           mRemoveFolderBtn;
    LLFilterEditor*     mFilterEditor;
};

#endif // FS_FLOATERPROTECTEDFOLDERS_H
