/**
 * @file fsfloaterradar.h
 * @brief qikfox3D radar floater implementation
 *
 * $LicenseInfo:firstyear=2013&license=viewerlgpl$
 * Copyright (c) 2013 Ansariel Hiller @ Second Life
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

#ifndef FS_FLOATERRADAR_H
#define FS_FLOATERRADAR_H

#include "llfloater.h"

#include "fspanelradar.h"
#include "fsradar.h"

class LLButton;
class LLFilterEditor;

class FSFloaterRadar
    : public LLFloater
{
    LOG_CLASS(FSFloaterRadar);
public:
    FSFloaterRadar(const LLSD &);
    ~FSFloaterRadar() override;

    bool postBuild() override;
    void onOpen(const LLSD& key) override;

private:
    FSPanelRadar*           mRadarPanel;
};

#endif // FS_FLOATERRADAR_H
