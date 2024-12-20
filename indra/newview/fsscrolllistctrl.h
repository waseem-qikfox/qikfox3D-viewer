/**
 * @file fsscrolllistctrl.h
 * @brief A qikfox3D specific implementation of scrolllist
 *
 * $LicenseInfo:firstyear=2014&license=viewerlgpl$
 * Phoenix qikfox3D Viewer Source Code
 * Copyright (c) 2014 Ansariel Hiller @ Second Life
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

#ifndef FS_SCROLLLISTCTRL_H
#define FS_SCROLLLISTCTRL_H

#include "lllistcontextmenu.h"
#include "llscrolllistctrl.h"

class FSScrollListCtrl
: public LLScrollListCtrl
{
public:
    using LLScrollListCtrl::setContextMenu;

    typedef enum e_content_type
    {
        AGENTS,
        MISC
    } EContentType;

    // provide names for enums
    struct ContentTypeNames : public LLInitParam::TypeValuesHelper<FSScrollListCtrl::EContentType, ContentTypeNames>
    {
        static void declareValues()
        {
            declare("Agents", FSScrollListCtrl::AGENTS);
            declare("Misc", FSScrollListCtrl::MISC);
        }
    };

    struct Params : public LLInitParam::Block<Params, LLScrollListCtrl::Params>
    {
        Optional<S32>                               desired_line_height;
        Optional<EContentType, ContentTypeNames>    content_type;

        Params()
        :   desired_line_height("desired_line_height", -1),
            content_type("content_type", MISC)
        {
        }
    };

    virtual ~FSScrollListCtrl() {};
    /*virtual*/ bool handleRightMouseDown(S32 x, S32 y, MASK mask);
    /*virtual*/ bool handleMouseDown(S32 x, S32 y, MASK mask);
    /*virtual*/ bool handleMouseUp(S32 x, S32 y, MASK mask);
    /*virtual*/ bool handleHover(S32 x, S32 y, MASK mask);
    /*virtual*/ bool handleDragAndDrop(S32 x, S32 y, MASK mask, bool drop,
                                      EDragAndDropType cargo_type,
                                      void* cargo_data,
                                      EAcceptance* accept,
                                      std::string& tooltip_msg);

    void    setContextMenu(LLListContextMenu* menu) { mContextMenu = menu; }
    void    refreshLineHeight();


    typedef boost::function<bool(S32, S32, MASK, bool, EDragAndDropType, void*, EAcceptance*, std::string&)> handle_dad_callback_signal_t;
    void setHandleDaDCallback(const handle_dad_callback_signal_t& func)
    {
        mHandleDaDCallback = func;
    }

protected:
    friend class LLUICtrlFactory;
    FSScrollListCtrl(const Params&);

    LLListContextMenu*  mContextMenu;
    S32                 mDesiredLineHeight;
    EContentType        mContentType;

    handle_dad_callback_signal_t    mHandleDaDCallback;
};

#endif // FS_SCROLLLISTCTRL_H
