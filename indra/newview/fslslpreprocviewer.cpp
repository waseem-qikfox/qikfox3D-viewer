/**
 * @file fslslpreprocviewer.cpp
 * @brief Specialized LLScriptEditor class for displaying LSL preprocessor output
 *
 * $LicenseInfo:firstyear=2016&license=viewerlgpl$
 * Phoenix qikfox3D Viewer Source Code
 * Copyright (c) 2016 Ansariel Hiller @ Second Life
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
#include "fslslpreprocviewer.h"

static LLDefaultChildRegistry::Register<FSLSLPreProcViewer> r("fs_lsl_preproc_viewer");

FSLSLPreProcViewer::FSLSLPreProcViewer(const Params& p)
:   LLScriptEditor(p)
{
}

bool FSLSLPreProcViewer::handleKeyHere(KEY key, MASK mask )
{
    // Normal key handling
    bool handled = handleNavigationKey( key, mask )
                    || handleSelectionKey(key, mask)
                    || handleControlKey(key, mask);

    if (handled)
    {
        resetCursorBlink();
        needsScroll();
    }

    return handled;
}

bool FSLSLPreProcViewer::handleUnicodeCharHere(llwchar uni_char)
{
    return false;
}
