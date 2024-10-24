/**
 * @file fsfloaterteleporthistory.cpp
 * @brief Class for the standalone teleport history in qikfox3D
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

#include "llviewerprecompiledheaders.h"

#include "fsfloaterteleporthistory.h"

#include "fscommon.h"
#include "llpanelteleporthistory.h"
#include "llbutton.h"
#include "llfiltereditor.h"
#include "llmenubutton.h"

FSFloaterTeleportHistory::FSFloaterTeleportHistory(const LLSD& seed)
    : LLFloater(seed),
    mHistoryPanel(nullptr)
{
}

FSFloaterTeleportHistory::~FSFloaterTeleportHistory()
{
    delete mHistoryPanel;
}

bool FSFloaterTeleportHistory::postBuild()
{
    mHistoryPanel = new LLTeleportHistoryPanel;

    if (mHistoryPanel)
    {
        mHistoryPanel->setIsStandAlone(true);

        mHistoryPanel->mTeleportBtn = getChild<LLButton>("teleport_btn");
        mHistoryPanel->mShowOnMapBtn = getChild<LLButton>("map_btn");
        mHistoryPanel->mShowProfile = getChild<LLButton>("profile_btn");

        mHistoryPanel->mTeleportBtn->setClickedCallback(boost::bind(&LLTeleportHistoryPanel::onTeleport, mHistoryPanel));
        mHistoryPanel->mShowProfile->setClickedCallback(boost::bind(&LLTeleportHistoryPanel::onShowProfile, mHistoryPanel));
        mHistoryPanel->mShowOnMapBtn->setClickedCallback(boost::bind(&LLTeleportHistoryPanel::onShowOnMap, mHistoryPanel));


        mFilterEditor = getChild<LLFilterEditor>("Filter");
        if (mFilterEditor)
        {
            //when list item is being clicked the filter editor looses focus
            //committing on focus lost leads to detaching list items
            //BUT a detached list item cannot be made selected and must not be clicked onto
            mFilterEditor->setCommitOnFocusLost(false);

            mFilterEditor->setCommitCallback(boost::bind(&FSFloaterTeleportHistory::onFilterEdit, this, _2, false));
        }

        getChildView("history_placeholder")->addChild(mHistoryPanel);
        mHistoryPanel->onSearchEdit("");

        mGearMenuButton = getChild<LLMenuButton>("options_gear_btn");
        mGearMenuButton->setMouseDownCallback(boost::bind(&FSFloaterTeleportHistory::onGearMenuClick, this));

        mSortingMenuButton = getChild<LLMenuButton>("sorting_menu_btn");
        mSortingMenuButton->setMouseDownCallback(boost::bind(&FSFloaterTeleportHistory::onSortingMenuClick, this));
    }
    else
    {
        return false;
    }

    return true;
}

void FSFloaterTeleportHistory::onFilterEdit(const std::string& search_string, bool force_filter)
{
    if (!mHistoryPanel)
    {
        return;
    }

    if (force_filter || mHistoryPanel->getFilterString() != search_string)
    {
        std::string string = search_string;

        // Searches are case-insensitive
        // but we don't convert the typed string to upper-case so that it can be fed to the web search as-is.

        mHistoryPanel->onSearchEdit(string);
    }
}

void FSFloaterTeleportHistory::resetFilter()
{
    mFilterEditor->clear();
    onFilterEdit("", true);
}

bool FSFloaterTeleportHistory::handleKeyHere(KEY key, MASK mask)
{
    if (FSCommon::isFilterEditorKeyCombo(key, mask))
    {
        mFilterEditor->setFocus(true);
        return true;
    }

    return LLFloater::handleKeyHere(key, mask);
}

void FSFloaterTeleportHistory::onGearMenuClick()
{
    mGearMenuButton->setMenu(mHistoryPanel->getSelectionMenu(), LLMenuButton::MP_BOTTOM_LEFT);
}

void FSFloaterTeleportHistory::onSortingMenuClick()
{
    mSortingMenuButton->setMenu(mHistoryPanel->getSortingMenu(), LLMenuButton::MP_BOTTOM_LEFT);
}
