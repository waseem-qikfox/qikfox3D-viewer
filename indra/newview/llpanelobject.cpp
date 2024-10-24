/**
 * @file llpanelobject.cpp
 * @brief Object editing (position, scale, etc.) in the tools floater
 *
 * $LicenseInfo:firstyear=2001&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
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
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

// file include
#include "llpanelobject.h"

// linden library includes
#include "llerror.h"
#include "llfontgl.h"
#include "llpermissionsflags.h"
#include "llstring.h"
#include "llvolume.h"
#include "m3math.h"

// project includes
#include "llagent.h"
#include "llbutton.h"
#include "llcalc.h"
#include "llcheckboxctrl.h"
#include "llcombobox.h"
#include "llfocusmgr.h"
#include "llmanipscale.h"
#include "llmenubutton.h"
#include "llpreviewscript.h"
#include "llresmgr.h"
#include "llselectmgr.h"
#include "llspinctrl.h"
#include "lltexturectrl.h"
#include "lltextbox.h"
#include "lltool.h"
#include "lltoolcomp.h"
#include "lltoolmgr.h"
#include "llui.h"
#include "llviewerobject.h"
#include "llviewerregion.h"
#include "llviewerwindow.h"
#include "llvovolume.h"
#include "llworld.h"
#include "pipeline.h"
#include "llviewercontrol.h"
#include "lluictrlfactory.h"
//#include "llfirstuse.h"
// [RLVa:KB] - Checked: 2011-05-22 (RLVa-1.3.1a)
#include "rlvhandler.h"
#include "llvoavatarself.h"
// [/RLVa:KB]

//
// Constants
//
enum {
    MI_BOX,
    MI_CYLINDER,
    MI_PRISM,
    MI_SPHERE,
    MI_TORUS,
    MI_TUBE,
    MI_RING,
    MI_SCULPT,
    MI_PATH_LINE_PROFILE_CIRCLE_HALF,
    MI_PATH_CIRCLE_PROFILE_CIRCLE_HALF,
    MI_PATH_CIRCLE2_PROFILE_SQUARE,
    MI_PATH_CIRCLE2_PROFILE_TRI,
    MI_PATH_CIRCLE2_PROFILE_CIRCLE,
    MI_PATH_CIRCLE2_PROFILE_CIRCLE_HALF,
    MI_PATH_TEST_PROFILE_SQUARE,
    MI_PATH_TEST_PROFILE_TRI,
    MI_PATH_TEST_PROFILE_CIRCLE,
    MI_PATH_TEST_PROFILE_CIRCLE_HALF,
    //<-- Working33 by Gregory Maurer
    MI_PATH_33_PROFILE_CIRCLE,
    MI_PATH_33_PROFILE_SQUARE,
    MI_PATH_33_PROFILE_TRIANGLE,
    MI_PATH_33_PROFILE_HALFCIRCLE,
    //Working33 -->
    MI_NONE,
    MI_VOLUME_COUNT
};

enum {
    MI_HOLE_SAME,
    MI_HOLE_CIRCLE,
    MI_HOLE_SQUARE,
    MI_HOLE_TRIANGLE,
    MI_HOLE_COUNT
};

// <FS:Ansariel> Defined in llviewerjointattachment.h
//const F32 MAX_ATTACHMENT_DIST = 3.5f; // meters

//static const std::string LEGACY_FULLBRIGHT_DESC =LLTrans::getString("Fullbright");

bool    LLPanelObject::postBuild()
{
    setMouseOpaque(false);

    //--------------------------------------------------------
    // Top
    //--------------------------------------------------------

    // Lock checkbox
    mCheckLock = getChild<LLCheckBoxCtrl>("checkbox locked");
    childSetCommitCallback("checkbox locked",onCommitLock,this);

    // Physical checkbox
    mCheckPhysics = getChild<LLCheckBoxCtrl>("Physical Checkbox Ctrl");
    childSetCommitCallback("Physical Checkbox Ctrl",onCommitPhysics,this);

    // Temporary checkbox
    mCheckTemporary = getChild<LLCheckBoxCtrl>("Temporary Checkbox Ctrl");
    childSetCommitCallback("Temporary Checkbox Ctrl",onCommitTemporary,this);

    // Phantom checkbox
    mCheckPhantom = getChild<LLCheckBoxCtrl>("Phantom Checkbox Ctrl");
    childSetCommitCallback("Phantom Checkbox Ctrl",onCommitPhantom,this);

    // Position
    //mMenuClipboardPos = getChild<LLMenuButton>("clipboard_pos_btn"); // <FS> Extended copy & paste buttons
    mLabelPosition = getChild<LLTextBox>("label position");
    mCtrlPosX = getChild<LLSpinCtrl>("Pos X");
    childSetCommitCallback("Pos X",onCommitPosition,this);
    mCtrlPosY = getChild<LLSpinCtrl>("Pos Y");
    childSetCommitCallback("Pos Y",onCommitPosition,this);
    mCtrlPosZ = getChild<LLSpinCtrl>("Pos Z");
    childSetCommitCallback("Pos Z",onCommitPosition,this);

    // Scale
    //mMenuClipboardSize = getChild<LLMenuButton>("clipboard_size_btn"); // <FS> Extended copy & paste buttons
    mLabelSize = getChild<LLTextBox>("label size");
    mCtrlScaleX = getChild<LLSpinCtrl>("Scale X");
    childSetCommitCallback("Scale X",onCommitScale,this);

    // Scale Y
    mCtrlScaleY = getChild<LLSpinCtrl>("Scale Y");
    childSetCommitCallback("Scale Y",onCommitScale,this);

    // Scale Z
    mCtrlScaleZ = getChild<LLSpinCtrl>("Scale Z");
    childSetCommitCallback("Scale Z",onCommitScale,this);

    // Rotation
    //mMenuClipboardRot = getChild<LLMenuButton>("clipboard_rot_btn"); // <FS> Extended copy & paste buttons
    mLabelRotation = getChild<LLTextBox>("label rotation");
    mCtrlRotX = getChild<LLSpinCtrl>("Rot X");
    childSetCommitCallback("Rot X",onCommitRotation,this);
    mCtrlRotY = getChild<LLSpinCtrl>("Rot Y");
    childSetCommitCallback("Rot Y",onCommitRotation,this);
    mCtrlRotZ = getChild<LLSpinCtrl>("Rot Z");
    childSetCommitCallback("Rot Z",onCommitRotation,this);

    // Copy/paste pos
    mBtnCopyPos = getChild<LLButton>("copy_pos_btn");
    mBtnCopyPos->setCommitCallback(boost::bind(&LLPanelObject::onCopyPos, this));
    mBtnPastePos = getChild<LLButton>("paste_pos_btn");
    mBtnPastePos->setCommitCallback(boost::bind(&LLPanelObject::onPastePos, this));
    // <FS> Extended copy & paste buttons
    mBtnPastePosClip = getChild<LLButton>("paste_pos_clip_btn");
    mBtnPastePosClip->setCommitCallback(boost::bind(&LLPanelObject::onPastePosClip, this));
    // </FS>

    // Copy/paste size
    mBtnCopySize = getChild<LLButton>("copy_size_btn");
    mBtnCopySize->setCommitCallback(boost::bind(&LLPanelObject::onCopySize, this));
    mBtnPasteSize = getChild<LLButton>("paste_size_btn");
    mBtnPasteSize->setCommitCallback(boost::bind(&LLPanelObject::onPasteSize, this));
    // <FS> Extended copy & paste buttons
    mBtnPasteSizeClip = getChild<LLButton>("paste_size_clip_btn");
    mBtnPasteSizeClip->setCommitCallback(boost::bind(&LLPanelObject::onPasteSizeClip, this));
    // </FS>

    // Copy/paste rot
    mBtnCopyRot = getChild<LLButton>("copy_rot_btn");
    mBtnCopyRot->setCommitCallback(boost::bind(&LLPanelObject::onCopyRot, this));
    mBtnPasteRot = getChild<LLButton>("paste_rot_btn");
    mBtnPasteRot->setCommitCallback(boost::bind(&LLPanelObject::onPasteRot, this));;
    // <FS> Extended copy & paste buttons
    mBtnPasteRotClip = getChild<LLButton>("paste_rot_clip_btn");
    mBtnPasteRotClip->setCommitCallback(boost::bind(&LLPanelObject::onPasteRotClip, this));
    // </FS>

    // Copy/paste obj prams
    mBtnCopyParams = getChild<LLButton>("copy_params_btn");
    mBtnCopyParams->setCommitCallback(boost::bind(&LLPanelObject::onCopyParams, this));
    mBtnPasteParams = getChild<LLButton>("paste_params_btn");
    mBtnPasteParams->setCommitCallback(boost::bind(&LLPanelObject::onPasteParams, this));

    //--------------------------------------------------------

    // Base Type
    mComboBaseType = getChild<LLComboBox>("comboBaseType");
    childSetCommitCallback("comboBaseType",onCommitParametric,this);

    //mMenuClipboardParams = getChild<LLMenuButton>("clipboard_obj_params_btn"); // <FS> Extended copy & paste buttons

    // Cut
    mLabelCut = getChild<LLTextBox>("text cut");
    mSpinCutBegin = getChild<LLSpinCtrl>("cut begin");
    childSetCommitCallback("cut begin",onCommitParametric,this);
    mSpinCutBegin->setValidateBeforeCommit( precommitValidate );
    mSpinCutEnd = getChild<LLSpinCtrl>("cut end");
    childSetCommitCallback("cut end",onCommitParametric,this);
    mSpinCutEnd->setValidateBeforeCommit( &precommitValidate );

    // Hollow / Skew
    mLabelHollow = getChild<LLTextBox>("text hollow");
    mLabelSkew = getChild<LLTextBox>("text skew");
    mSpinHollow = getChild<LLSpinCtrl>("Scale 1");
    childSetCommitCallback("Scale 1",onCommitParametric,this);
    mSpinHollow->setValidateBeforeCommit( &precommitValidate );
    mSpinSkew = getChild<LLSpinCtrl>("Skew");
    childSetCommitCallback("Skew",onCommitParametric,this);
    mSpinSkew->setValidateBeforeCommit( &precommitValidate );
    mLabelHoleType = getChild<LLTextBox>("Hollow Shape");

    // Hole Type
    mComboHoleType = getChild<LLComboBox>("hole");
    childSetCommitCallback("hole",onCommitParametric,this);

    // Twist
    mLabelTwist = getChild<LLTextBox>("text twist");
    mSpinTwistBegin = getChild<LLSpinCtrl>("Twist Begin");
    childSetCommitCallback("Twist Begin",onCommitParametric,this);
    mSpinTwistBegin->setValidateBeforeCommit( precommitValidate );
    mSpinTwist = getChild<LLSpinCtrl>("Twist End");
    childSetCommitCallback("Twist End",onCommitParametric,this);
    mSpinTwist->setValidateBeforeCommit( &precommitValidate );

    // Scale
    mSpinScaleX = getChild<LLSpinCtrl>("Taper Scale X");
    childSetCommitCallback("Taper Scale X",onCommitParametric,this);
    mSpinScaleX->setValidateBeforeCommit( &precommitValidate );
    mSpinScaleY = getChild<LLSpinCtrl>("Taper Scale Y");
    childSetCommitCallback("Taper Scale Y",onCommitParametric,this);
    mSpinScaleY->setValidateBeforeCommit( &precommitValidate );

    // Shear
    mLabelShear = getChild<LLTextBox>("text topshear");
    mSpinShearX = getChild<LLSpinCtrl>("Shear X");
    childSetCommitCallback("Shear X",onCommitParametric,this);
    mSpinShearX->setValidateBeforeCommit( &precommitValidate );
    mSpinShearY = getChild<LLSpinCtrl>("Shear Y");
    childSetCommitCallback("Shear Y",onCommitParametric,this);
    mSpinShearY->setValidateBeforeCommit( &precommitValidate );

    // Path / Profile
    mCtrlPathBegin = getChild<LLSpinCtrl>("Path Limit Begin");
    childSetCommitCallback("Path Limit Begin",onCommitParametric,this);
    mCtrlPathBegin->setValidateBeforeCommit( &precommitValidate );
    mCtrlPathEnd = getChild<LLSpinCtrl>("Path Limit End");
    childSetCommitCallback("Path Limit End",onCommitParametric,this);
    mCtrlPathEnd->setValidateBeforeCommit( &precommitValidate );

    // Taper
    mLabelTaper = getChild<LLTextBox>("text taper2");
    mSpinTaperX = getChild<LLSpinCtrl>("Taper X");
    childSetCommitCallback("Taper X",onCommitParametric,this);
    mSpinTaperX->setValidateBeforeCommit( precommitValidate );
    mSpinTaperY = getChild<LLSpinCtrl>("Taper Y");
    childSetCommitCallback("Taper Y",onCommitParametric,this);
    mSpinTaperY->setValidateBeforeCommit( precommitValidate );

    // Radius Offset / Revolutions
    mLabelRadiusOffset = getChild<LLTextBox>("text radius delta");
    mLabelRevolutions = getChild<LLTextBox>("text revolutions");
    mSpinRadiusOffset = getChild<LLSpinCtrl>("Radius Offset");
    childSetCommitCallback("Radius Offset",onCommitParametric,this);
    mSpinRadiusOffset->setValidateBeforeCommit( &precommitValidate );
    mSpinRevolutions = getChild<LLSpinCtrl>("Revolutions");
    childSetCommitCallback("Revolutions",onCommitParametric,this);
    mSpinRevolutions->setValidateBeforeCommit( &precommitValidate );

    // Sculpt
    mCtrlSculptTexture = getChild<LLTextureCtrl>("sculpt texture control");
    if (mCtrlSculptTexture)
    {
        mCtrlSculptTexture->setDefaultImageAssetID(SCULPT_DEFAULT_TEXTURE);
        mCtrlSculptTexture->setCommitCallback( boost::bind(&LLPanelObject::onCommitSculpt, this, _2 ));
        mCtrlSculptTexture->setOnCancelCallback( boost::bind(&LLPanelObject::onCancelSculpt, this, _2 ));
        mCtrlSculptTexture->setOnSelectCallback( boost::bind(&LLPanelObject::onSelectSculpt, this, _2 ));
        mCtrlSculptTexture->setDropCallback( boost::bind(&LLPanelObject::onDropSculpt, this, _2 ));
        // Don't allow (no copy) or (no transfer) textures to be selected during immediate mode
        mCtrlSculptTexture->setImmediateFilterPermMask(PERM_COPY | PERM_TRANSFER);
        mCtrlSculptTexture->setDnDFilterPermMask(PERM_COPY | PERM_TRANSFER);
        LLAggregatePermissions texture_perms;
        if (LLSelectMgr::getInstance()->selectGetAggregateTexturePermissions(texture_perms))
        {
            bool can_copy =
                texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_EMPTY ||
                texture_perms.getValue(PERM_COPY) == LLAggregatePermissions::AP_ALL;
            bool can_transfer =
                texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_EMPTY ||
                texture_perms.getValue(PERM_TRANSFER) == LLAggregatePermissions::AP_ALL;
            mCtrlSculptTexture->setCanApplyImmediately(can_copy && can_transfer);
        }
        else
        {
            mCtrlSculptTexture->setCanApplyImmediately(false);
        }
    }

    mLabelSculptType = getChild<LLTextBox>("label sculpt type");
    mCtrlSculptType = getChild<LLComboBox>("sculpt type control");
    childSetCommitCallback("sculpt type control", onCommitSculptType, this);
    mCtrlSculptMirror = getChild<LLCheckBoxCtrl>("sculpt mirror control");
    childSetCommitCallback("sculpt mirror control", onCommitSculptType, this);
    mCtrlSculptInvert = getChild<LLCheckBoxCtrl>("sculpt invert control");
    childSetCommitCallback("sculpt invert control", onCommitSculptType, this);
    //<FS:Beq> FIRE-21445 + Mesh Info in object panel
    mComboLOD = getChild<LLComboBox>("LOD_show_combo");
    mComboLOD->setCommitCallback(boost::bind(&LLPanelObject::onCommitLOD, this));
    //</FS:Beq>
    // Start with everyone disabled
    clearCtrls();

// <FS:CR> Aurora Sim
    updateLimits(false);    // default to non-attachment
// </FS:CR> Aurora Sim

    changePrecision(gSavedSettings.getS32("FSBuildToolDecimalPrecision"));  // <FS:CR> Adjustable decimal precision

    return true;
}

LLPanelObject::LLPanelObject()
:   LLPanel(),
    mIsPhysical(false),
    mIsTemporary(false),
    mIsPhantom(false),
    mSelectedType(MI_BOX),
    mSculptTextureRevert(LLUUID::null),
    mSculptTypeRevert(0),
    mHasClipboardPos(false),
    mHasClipboardSize(false),
    mHasClipboardRot(false),
    mSizeChanged(false)
{
    // <FS> Extended copy & paste buttons
    //mCommitCallbackRegistrar.add("PanelObject.menuDoToSelected", boost::bind(&LLPanelObject::menuDoToSelected, this, _2));
    //mEnableCallbackRegistrar.add("PanelObject.menuEnable", boost::bind(&LLPanelObject::menuEnableItem, this, _2));
    // </FS>
}


LLPanelObject::~LLPanelObject()
{
    // Children all cleaned up by default view destructor.
}

// <AW: opensim-limits>
void LLPanelObject::updateLimits(bool attachment)
{
// <FS:CR> Aurora Sim
    //mRegionMaxHeight = LLWorld::getInstance()->getRegionMaxHeight();
    //mCtrlPosZ->setMaxValue(mRegionMaxHeight);
//<FS:TS> FIRE-8205: Unable to edit attachments to negative coordinates
//  Only apply region size limits to position spinners if editing something
//    that's not an attachment. Attachments have their own limits.
    if (attachment)
    {
        mCtrlPosX->setMinValue(-MAX_ATTACHMENT_DIST);
        mCtrlPosX->setMaxValue(MAX_ATTACHMENT_DIST);
        mCtrlPosY->setMinValue(-MAX_ATTACHMENT_DIST);
        mCtrlPosY->setMaxValue(MAX_ATTACHMENT_DIST);
        mCtrlPosZ->setMinValue(-MAX_ATTACHMENT_DIST);
        mCtrlPosZ->setMaxValue(MAX_ATTACHMENT_DIST);
    }
    else
    {
        mCtrlPosX->setMinValue(LLWorld::getInstance()->getMinPrimXPos());
        mCtrlPosX->setMaxValue(LLWorld::getInstance()->getMaxPrimXPos());
        mCtrlPosY->setMinValue(LLWorld::getInstance()->getMinPrimYPos());
        mCtrlPosY->setMaxValue(LLWorld::getInstance()->getMaxPrimYPos());
        mCtrlPosZ->setMinValue(LLWorld::getInstance()->getMinPrimZPos());
        mCtrlPosZ->setMaxValue(LLWorld::getInstance()->getMaxPrimZPos());
    }
//</FS:TS> FIRE-8205: Unable to edit attachments to negative coordinates
// </FS:CR> Aurora Sim
    mMinScale = LLWorld::getInstance()->getRegionMinPrimScale();
    mMaxScale = LLWorld::getInstance()->getRegionMaxPrimScale();
    mCtrlScaleX->setMinValue(mMinScale);
    mCtrlScaleX->setMaxValue(mMaxScale);
    mCtrlScaleY->setMinValue(mMinScale);
    mCtrlScaleY->setMaxValue(mMaxScale);
    mCtrlScaleZ->setMinValue(mMinScale);
    mCtrlScaleZ->setMaxValue(mMaxScale);

    mMaxHollowSize = LLWorld::getInstance()->getRegionMaxHollowSize();
    mSpinHollow->setMaxValue(mMaxHollowSize);

    mMinHoleSize = LLWorld::getInstance()->getRegionMinHoleSize();
    mSpinScaleX->setMinValue(mMinHoleSize);
    mSpinScaleY->setMinValue(mMinHoleSize);
// <FS:CR> Aurora Sim
    mCheckPhysics->setEnabled(LLWorld::getInstance()->getAllowPhysicalPrims());
// </FS:CR> Aurora Sim
}
// </AW: opensim-limits>

// <FS:CR> Adjustable decimal precision
void LLPanelObject::changePrecision(S32 decimal_precision)
{
    mSpinCutBegin->setPrecision(decimal_precision);
    mSpinCutEnd->setPrecision(decimal_precision);
    mSpinScaleX->setPrecision(decimal_precision);
    mSpinScaleY->setPrecision(decimal_precision);
    mSpinSkew->setPrecision(decimal_precision);
    mSpinShearX->setPrecision(decimal_precision);
    mSpinShearY->setPrecision(decimal_precision);
    mSpinTaperX->setPrecision(decimal_precision);
    mSpinTaperY->setPrecision(decimal_precision);
    mSpinRadiusOffset->setPrecision(decimal_precision);
    mSpinRevolutions->setPrecision(decimal_precision);
    mSpinHollow->setPrecision(decimal_precision);
    mCtrlPathBegin->setPrecision(decimal_precision);
    mCtrlPathEnd->setPrecision(decimal_precision);

    mCtrlPosX->setPrecision(decimal_precision);
    mCtrlPosY->setPrecision(decimal_precision);
    mCtrlPosZ->setPrecision(decimal_precision);
    mCtrlScaleX->setPrecision(decimal_precision);
    mCtrlScaleY->setPrecision(decimal_precision);
    mCtrlScaleZ->setPrecision(decimal_precision);
    mCtrlRotX->setPrecision(decimal_precision);
    mCtrlRotY->setPrecision(decimal_precision);
    mCtrlRotZ->setPrecision(decimal_precision);
}
// </FS:CR>

void LLPanelObject::getState( )
{
    LLViewerObject* objectp = LLSelectMgr::getInstance()->getSelection()->getFirstRootObject();
    LLViewerObject* root_objectp = objectp;
    if(!objectp)
    {
        objectp = LLSelectMgr::getInstance()->getSelection()->getFirstObject();
        // *FIX: shouldn't we just keep the child?
        if (objectp)
        {
            LLViewerObject* parentp = objectp->getRootEdit();

            if (parentp)
            {
                root_objectp = parentp;
            }
            else
            {
                root_objectp = objectp;
            }
        }
        deactivateMeshFields();
    }

    LLCalc* calcp = LLCalc::getInstance();

    LLVOVolume *volobjp = NULL;
    if ( objectp && (objectp->getPCode() == LL_PCODE_VOLUME))
    {
        volobjp = (LLVOVolume *)objectp;
    }

    if( !objectp )
    {
        //forfeit focus
        if (gFocusMgr.childHasKeyboardFocus(this))
        {
            gFocusMgr.setKeyboardFocus(NULL);
        }

        // Disable all text input fields
        clearCtrls();
        calcp->clearAllVariables();
        return;
    }

    S32 selected_count = LLSelectMgr::getInstance()->getSelection()->getObjectCount();
    bool single_volume = (LLSelectMgr::getInstance()->selectionAllPCode( LL_PCODE_VOLUME ))
                         && (selected_count == 1);

//<FS:TS> FIRE-8205: Unable to edit attachment to negative coordinates
//  Update the position limits depending on whether this is an attachment.
    updateLimits(objectp->isAttachment());
//</FS:TS> FIRE-8205

    bool enable_move;
    bool enable_modify;

    LLSelectMgr::getInstance()->selectGetEditMoveLinksetPermissions(enable_move, enable_modify);

    bool enable_scale = enable_modify;
    bool enable_rotate = enable_move; // already accounts for a case of children, which needs permModify() as well

    LLVector3 vec;
    if (enable_move)
    {
        vec = objectp->getPositionEdit();
        mCtrlPosX->set( vec.mV[VX] );
        mCtrlPosY->set( vec.mV[VY] );
        mCtrlPosZ->set( vec.mV[VZ] );
        calcp->setVar(LLCalc::X_POS, vec.mV[VX]);
        calcp->setVar(LLCalc::Y_POS, vec.mV[VY]);
        calcp->setVar(LLCalc::Z_POS, vec.mV[VZ]);
    }
    else
    {
        mCtrlPosX->clear();
        mCtrlPosY->clear();
        mCtrlPosZ->clear();
        calcp->clearVar(LLCalc::X_POS);
        calcp->clearVar(LLCalc::Y_POS);
        calcp->clearVar(LLCalc::Z_POS);
    }

    //mMenuClipboardPos->setEnabled(enable_move); // <FS> Extended copy & paste buttons
    mLabelPosition->setEnabled( enable_move );
    mCtrlPosX->setEnabled(enable_move);
    mCtrlPosY->setEnabled(enable_move);
    mCtrlPosZ->setEnabled(enable_move);
    // <FS> Extended copy & paste buttons
    mBtnCopyPos->setEnabled(enable_move);
    mBtnPastePos->setEnabled(enable_move && mHasClipboardPos);
    mBtnPastePosClip->setEnabled( enable_move );
    // </FS>

    if (enable_scale)
    {
        vec = objectp->getScale();
        mCtrlScaleX->set( vec.mV[VX] );
        mCtrlScaleY->set( vec.mV[VY] );
        mCtrlScaleZ->set( vec.mV[VZ] );
        calcp->setVar(LLCalc::X_SCALE, vec.mV[VX]);
        calcp->setVar(LLCalc::Y_SCALE, vec.mV[VY]);
        calcp->setVar(LLCalc::Z_SCALE, vec.mV[VZ]);
    }
    else
    {
        mCtrlScaleX->clear();
        mCtrlScaleY->clear();
        mCtrlScaleZ->clear();
        calcp->setVar(LLCalc::X_SCALE, 0.f);
        calcp->setVar(LLCalc::Y_SCALE, 0.f);
        calcp->setVar(LLCalc::Z_SCALE, 0.f);
    }

    //mMenuClipboardSize->setEnabled(enable_scale); // <FS> Extended copy & paste buttons
    mLabelSize->setEnabled( enable_scale );
    mCtrlScaleX->setEnabled( enable_scale );
    mCtrlScaleY->setEnabled( enable_scale );
    mCtrlScaleZ->setEnabled( enable_scale );
    // <FS> Extended copy & paste buttons
    mBtnCopySize->setEnabled( enable_scale );
    mBtnPasteSize->setEnabled( enable_scale && mHasClipboardSize );
    mBtnPasteSizeClip->setEnabled( enable_scale );
    // </FS>

    LLQuaternion object_rot = objectp->getRotationEdit();
    object_rot.getEulerAngles(&(mCurEulerDegrees.mV[VX]), &(mCurEulerDegrees.mV[VY]), &(mCurEulerDegrees.mV[VZ]));
    mCurEulerDegrees *= RAD_TO_DEG;
    mCurEulerDegrees.mV[VX] = fmod(ll_round(mCurEulerDegrees.mV[VX], OBJECT_ROTATION_PRECISION) + 360.f, 360.f);
    mCurEulerDegrees.mV[VY] = fmod(ll_round(mCurEulerDegrees.mV[VY], OBJECT_ROTATION_PRECISION) + 360.f, 360.f);
    mCurEulerDegrees.mV[VZ] = fmod(ll_round(mCurEulerDegrees.mV[VZ], OBJECT_ROTATION_PRECISION) + 360.f, 360.f);

    if (enable_rotate)
    {
        mCtrlRotX->set( mCurEulerDegrees.mV[VX] );
        mCtrlRotY->set( mCurEulerDegrees.mV[VY] );
        mCtrlRotZ->set( mCurEulerDegrees.mV[VZ] );
        calcp->setVar(LLCalc::X_ROT, mCurEulerDegrees.mV[VX]);
        calcp->setVar(LLCalc::Y_ROT, mCurEulerDegrees.mV[VY]);
        calcp->setVar(LLCalc::Z_ROT, mCurEulerDegrees.mV[VZ]);
    }
    else
    {
        mCtrlRotX->clear();
        mCtrlRotY->clear();
        mCtrlRotZ->clear();
        calcp->clearVar(LLCalc::X_ROT);
        calcp->clearVar(LLCalc::Y_ROT);
        calcp->clearVar(LLCalc::Z_ROT);
    }

    //mMenuClipboardRot->setEnabled(enable_rotate); // <FS> Extended copy & paste buttons
    mLabelRotation->setEnabled( enable_rotate );
    mCtrlRotX->setEnabled( enable_rotate );
    mCtrlRotY->setEnabled( enable_rotate );
    mCtrlRotZ->setEnabled( enable_rotate );
    // <FS> Extended copy & paste buttons
    mBtnCopyRot->setEnabled( enable_rotate );
    mBtnPasteRot->setEnabled( enable_rotate && mHasClipboardRot );
    mBtnPasteRotClip->setEnabled( enable_rotate );

    mBtnCopyParams->setEnabled( single_volume && enable_modify );
    mBtnPasteParams->setEnabled( single_volume && enable_modify && !mClipboardParams.emptyMap() && (mClipboardParams["volume_params"] || mClipboardParams["sculpt"]));
    // </FS>

    LLUUID owner_id;
    std::string owner_name;
    LLSelectMgr::getInstance()->selectGetOwner(owner_id, owner_name);

    // BUG? Check for all objects being editable?
    S32 roots_selected = LLSelectMgr::getInstance()->getSelection()->getRootObjectCount();
    bool editable = root_objectp->permModify();

    bool is_flexible = volobjp && volobjp->isFlexible();
    bool is_permanent = root_objectp->flagObjectPermanent();
    bool is_permanent_enforced = root_objectp->isPermanentEnforced();
    bool is_character = root_objectp->flagCharacter();
    llassert(!is_permanent || !is_character); // should never have a permanent object that is also a character

    // Lock checkbox - only modifiable if you own the object.
    bool self_owned = (gAgent.getID() == owner_id);
    mCheckLock->setEnabled( roots_selected > 0 && self_owned && !is_permanent_enforced);

    // More lock and debit checkbox - get the values
    bool valid;
    U32 owner_mask_on;
    U32 owner_mask_off;
    valid = LLSelectMgr::getInstance()->selectGetPerm(PERM_OWNER, &owner_mask_on, &owner_mask_off);

    if(valid)
    {
        if(owner_mask_on & PERM_MOVE)
        {
            // owner can move, so not locked
            mCheckLock->set(false);
            mCheckLock->setTentative(false);
        }
        else if(owner_mask_off & PERM_MOVE)
        {
            // owner can't move, so locked
            mCheckLock->set(true);
            mCheckLock->setTentative(false);
        }
        else
        {
            // some locked, some not locked
            mCheckLock->set(false);
            mCheckLock->setTentative(true);
        }
    }

    // Physics checkbox
    mIsPhysical = root_objectp->flagUsePhysics();
    llassert(!is_permanent || !mIsPhysical); // should never have a permanent object that is also physical

    mCheckPhysics->set( mIsPhysical );
    mCheckPhysics->setEnabled( roots_selected>0
                                && (editable || gAgent.isGodlike())
                                && !is_flexible && !is_permanent);

    mIsTemporary = root_objectp->flagTemporaryOnRez();
    llassert(!is_permanent || !mIsTemporary); // should never has a permanent object that is also temporary

    mCheckTemporary->set( mIsTemporary );
    mCheckTemporary->setEnabled( roots_selected>0 && editable && !is_permanent);

    mIsPhantom = root_objectp->flagPhantom();
    bool is_volume_detect = root_objectp->flagVolumeDetect();
    llassert(!is_character || !mIsPhantom); // should never have a character that is also a phantom
    mCheckPhantom->set( mIsPhantom );
    mCheckPhantom->setEnabled( roots_selected>0 && editable && !is_flexible && !is_permanent_enforced && !is_character && !is_volume_detect);

    //----------------------------------------------------------------------------

    S32 selected_item   = MI_BOX;
    S32 selected_hole   = MI_HOLE_SAME;
    bool enabled = false;
    bool hole_enabled = false;
    F32 scale_x=1.f, scale_y=1.f;
    bool isMesh = false;

    if( !objectp || !objectp->getVolume() || !editable || !single_volume)
    {
        // Clear out all geometry fields.
        mComboBaseType->clear();
        mSpinHollow->clear();
        mSpinCutBegin->clear();
        mSpinCutEnd->clear();
        mCtrlPathBegin->clear();
        mCtrlPathEnd->clear();
        mSpinScaleX->clear();
        mSpinScaleY->clear();
        mSpinTwist->clear();
        mSpinTwistBegin->clear();
        mComboHoleType->clear();
        mSpinShearX->clear();
        mSpinShearY->clear();
        mSpinTaperX->clear();
        mSpinTaperY->clear();
        mSpinRadiusOffset->clear();
        mSpinRevolutions->clear();
        mSpinSkew->clear();

        mSelectedType = MI_NONE;
    }
    else
    {
        // Only allowed to change these parameters for objects
        // that you have permissions on AND are not attachments.
        enabled = root_objectp->permModify() && !root_objectp->isPermanentEnforced();

        // Volume type
        const LLVolumeParams &volume_params = objectp->getVolume()->getParams();
        U8 path = volume_params.getPathParams().getCurveType();
        U8 profile_and_hole = volume_params.getProfileParams().getCurveType();
        U8 profile  = profile_and_hole & LL_PCODE_PROFILE_MASK;
        U8 hole     = profile_and_hole & LL_PCODE_HOLE_MASK;

        // Scale goes first so we can differentiate between a sphere and a torus,
        // which have the same profile and path types.

        // Scale
        scale_x = volume_params.getRatioX();
        scale_y = volume_params.getRatioY();

        bool linear_path = (path == LL_PCODE_PATH_LINE) || (path == LL_PCODE_PATH_FLEXIBLE);
        if ( linear_path && profile == LL_PCODE_PROFILE_CIRCLE )
        {
            selected_item = MI_CYLINDER;
        }
        else if ( linear_path && profile == LL_PCODE_PROFILE_SQUARE )
        {
            selected_item = MI_BOX;
        }
        else if ( linear_path && profile == LL_PCODE_PROFILE_ISOTRI )
        {
            selected_item = MI_PRISM;
        }
        else if ( linear_path && profile == LL_PCODE_PROFILE_EQUALTRI )
        {
            selected_item = MI_PRISM;
        }
        else if ( linear_path && profile == LL_PCODE_PROFILE_RIGHTTRI )
        {
            selected_item = MI_PRISM;
        }
        else if (path == LL_PCODE_PATH_FLEXIBLE) // shouldn't happen
        {
            selected_item = MI_CYLINDER; // reasonable default
        }
        else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_CIRCLE && scale_y > 0.75f)
        {
            selected_item = MI_SPHERE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_CIRCLE && scale_y <= 0.75f)
        {
            selected_item = MI_TORUS;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_CIRCLE_HALF)
        {
            selected_item = MI_SPHERE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_CIRCLE )
        {
            // Spirals aren't supported.  Make it into a sphere.  JC
            selected_item = MI_SPHERE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_EQUALTRI )
        {
            selected_item = MI_RING;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE && profile == LL_PCODE_PROFILE_SQUARE && scale_y <= 0.75f)
        {
            selected_item = MI_TUBE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_EQUALTRI )
        {
            selected_item = MI_PATH_CIRCLE2_PROFILE_TRI;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_SQUARE )
        {
            selected_item = MI_PATH_CIRCLE2_PROFILE_SQUARE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE2 && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
        {
            selected_item = MI_PATH_CIRCLE2_PROFILE_CIRCLE_HALF;
        }
        else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_SQUARE )
        {
            selected_item = MI_PATH_TEST_PROFILE_SQUARE;
        }
        else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_EQUALTRI )
        {
            selected_item = MI_PATH_TEST_PROFILE_TRI;
        }
        else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_CIRCLE )
        {
            selected_item = MI_PATH_TEST_PROFILE_CIRCLE;
        }
        else if ( path == LL_PCODE_PATH_TEST && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
        {
            selected_item = MI_PATH_TEST_PROFILE_CIRCLE_HALF;
        }
        else if ( path == LL_PCODE_PATH_LINE && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
        {
            selected_item = MI_PATH_LINE_PROFILE_CIRCLE_HALF;
        }
        //<-- Working33 by Gregory Maurer
        else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_CIRCLE )
        {
            selected_item = MI_PATH_33_PROFILE_CIRCLE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_SQUARE )
        {
            selected_item = MI_PATH_33_PROFILE_SQUARE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_ISOTRI )
        {
            selected_item = MI_PATH_33_PROFILE_TRIANGLE;
        }
        else if ( path == LL_PCODE_PATH_CIRCLE_33 && profile == LL_PCODE_PROFILE_CIRCLE_HALF )
        {
            selected_item = MI_PATH_33_PROFILE_HALFCIRCLE;
        }
        //Working33 -->
        else
        {
            LL_INFOS("FloaterTools") << "Unknown path " << (S32) path << " profile " << (S32) profile << " in getState" << LL_ENDL;
            selected_item = MI_BOX;
        }


        if (objectp->getParameterEntryInUse(LLNetworkData::PARAMS_SCULPT))
        {
            selected_item = MI_SCULPT;
            //LLFirstUse::useSculptedPrim();
        }


        mComboBaseType  ->setCurrentByIndex( selected_item );
        mSelectedType = selected_item;

        // Grab S path
        F32 begin_s = volume_params.getBeginS();
        F32 end_s   = volume_params.getEndS();

        // Compute cut and advanced cut from S and T
        F32 begin_t = volume_params.getBeginT();
        F32 end_t   = volume_params.getEndT();

        // Hollowness
        F32 hollow = 100.f * volume_params.getHollow();
        mSpinHollow->set( hollow );
        calcp->setVar(LLCalc::HOLLOW, hollow);
        // All hollow objects allow a shape to be selected.
        if (hollow > 0.f)
        {
            switch (hole)
            {
            case LL_PCODE_HOLE_CIRCLE:
                selected_hole = MI_HOLE_CIRCLE;
                break;
            case LL_PCODE_HOLE_SQUARE:
                selected_hole = MI_HOLE_SQUARE;
                break;
            case LL_PCODE_HOLE_TRIANGLE:
                selected_hole = MI_HOLE_TRIANGLE;
                break;
            case LL_PCODE_HOLE_SAME:
            default:
                selected_hole = MI_HOLE_SAME;
                break;
            }
            mComboHoleType->setCurrentByIndex( selected_hole );
            hole_enabled = enabled;
        }
        else
        {
            mComboHoleType->setCurrentByIndex( MI_HOLE_SAME );
            hole_enabled = false;
        }

        // Cut interpretation varies based on base object type
        F32 cut_begin, cut_end, adv_cut_begin, adv_cut_end;

        if ( selected_item == MI_SPHERE || selected_item == MI_TORUS ||
             selected_item == MI_TUBE   || selected_item == MI_RING )
        {
            cut_begin       = begin_t;
            cut_end         = end_t;
            adv_cut_begin   = begin_s;
            adv_cut_end     = end_s;
        }
        else
        {
            cut_begin       = begin_s;
            cut_end         = end_s;
            adv_cut_begin   = begin_t;
            adv_cut_end     = end_t;
        }

        mSpinCutBegin   ->set( cut_begin );
        mSpinCutEnd     ->set( cut_end );
        mCtrlPathBegin  ->set( adv_cut_begin );
        mCtrlPathEnd    ->set( adv_cut_end );
        calcp->setVar(LLCalc::CUT_BEGIN, cut_begin);
        calcp->setVar(LLCalc::CUT_END, cut_end);
        calcp->setVar(LLCalc::PATH_BEGIN, adv_cut_begin);
        calcp->setVar(LLCalc::PATH_END, adv_cut_end);

        // Twist
        F32 twist       = volume_params.getTwist();
        F32 twist_begin = volume_params.getTwistBegin();
        // Check the path type for conversion.
        if (path == LL_PCODE_PATH_LINE || path == LL_PCODE_PATH_FLEXIBLE)
        {
            twist       *= OBJECT_TWIST_LINEAR_MAX;
            twist_begin *= OBJECT_TWIST_LINEAR_MAX;
        }
        else
        {
            twist       *= OBJECT_TWIST_MAX;
            twist_begin *= OBJECT_TWIST_MAX;
        }

        mSpinTwist      ->set( twist );
        mSpinTwistBegin ->set( twist_begin );
        calcp->setVar(LLCalc::TWIST_END, twist);
        calcp->setVar(LLCalc::TWIST_BEGIN, twist_begin);

        // Shear
        F32 shear_x = volume_params.getShearX();
        F32 shear_y = volume_params.getShearY();
        mSpinShearX->set( shear_x );
        mSpinShearY->set( shear_y );
        calcp->setVar(LLCalc::X_SHEAR, shear_x);
        calcp->setVar(LLCalc::Y_SHEAR, shear_y);

        // Taper
        F32 taper_x = volume_params.getTaperX();
        F32 taper_y = volume_params.getTaperY();
        mSpinTaperX->set( taper_x );
        mSpinTaperY->set( taper_y );
        calcp->setVar(LLCalc::X_TAPER, taper_x);
        calcp->setVar(LLCalc::Y_TAPER, taper_y);

        // Radius offset.
        F32 radius_offset = volume_params.getRadiusOffset();
        // Limit radius offset, based on taper and hole size y.
//KC: Phoenix capability
#if 0
        F32 radius_mag = fabs(radius_offset);
        F32 hole_y_mag = fabs(scale_y);
        F32 taper_y_mag  = fabs(taper_y);
        // Check to see if the taper effects us.
        if ( (radius_offset > 0.f && taper_y < 0.f) ||
             (radius_offset < 0.f && taper_y > 0.f) )
        {
            // The taper does not help increase the radius offset range.
            taper_y_mag = 0.f;
        }
        F32 max_radius_mag = 1.f - hole_y_mag * (1.f - taper_y_mag) / (1.f - hole_y_mag);
        // Enforce the maximum magnitude.
        if (radius_mag > max_radius_mag)
        {
            // Check radius offset sign.
            if (radius_offset < 0.f)
            {
                radius_offset = -max_radius_mag;
            }
            else
            {
                radius_offset = max_radius_mag;
            }
        }
#endif
        mSpinRadiusOffset->set( radius_offset);
        calcp->setVar(LLCalc::RADIUS_OFFSET, radius_offset);

        // Revolutions
        F32 revolutions = volume_params.getRevolutions();
        mSpinRevolutions->set( revolutions );
        calcp->setVar(LLCalc::REVOLUTIONS, revolutions);

        // Skew
        F32 skew    = volume_params.getSkew();
        // Limit skew, based on revolutions hole size x.
//KC: Phoenix capability
#if 0
        F32 skew_mag= fabs(skew);
        F32 min_skew_mag = 1.0f - 1.0f / (revolutions * scale_x + 1.0f);
        // Discontinuity; A revolution of 1 allows skews below 0.5.
        if ( fabs(revolutions - 1.0f) < 0.001)
            min_skew_mag = 0.0f;

        // Clip skew.
        if (skew_mag < min_skew_mag)
        {
            // Check skew sign.
            if (skew < 0.0f)
            {
                skew = -min_skew_mag;
            }
            else
            {
                skew = min_skew_mag;
            }
        }
#endif
        mSpinSkew->set( skew );
        calcp->setVar(LLCalc::SKEW, skew);
    }

    // Compute control visibility, label names, and twist range.
    // Start with defaults.
    // <FS:Beq> FIRE-21772 mComboBaseType remians invisible after editing a mesh
    mComboBaseType->setVisible(true);
    // </FS:Beq>
    bool cut_visible                = true;
    bool hollow_visible             = true;
    bool top_size_x_visible         = true;
    bool top_size_y_visible         = true;
    bool top_shear_x_visible        = true;
    bool top_shear_y_visible        = true;
    bool twist_visible              = true;
    //KC: Phoenix capability, allow all transforms
    bool advanced_cut_visible       = true;
    bool taper_visible              = true;
    bool skew_visible               = true;
    bool radius_offset_visible      = true;
    bool revolutions_visible        = true;
    bool sculpt_texture_visible     = false;
    F32  twist_min                  = OBJECT_TWIST_LINEAR_MIN;
    F32  twist_max                  = OBJECT_TWIST_LINEAR_MAX;
    F32  twist_inc                  = OBJECT_TWIST_LINEAR_INC;

    bool advanced_is_dimple = false;
    bool advanced_is_slice = false;
    bool size_is_hole = false;

    // Tune based on overall volume type
    switch (selected_item)
    {
    case MI_SPHERE:
        top_size_x_visible      = true;
        top_size_y_visible      = true;
        top_shear_x_visible     = true;
        top_shear_y_visible     = true;
        twist_visible           = true;
        advanced_cut_visible    = true;
        advanced_is_dimple      = true;
        twist_min               = OBJECT_TWIST_MIN;
        twist_max               = OBJECT_TWIST_MAX;
        twist_inc               = OBJECT_TWIST_INC;
        break;

    case MI_TORUS:
    case MI_TUBE:
    case MI_RING:
        //top_size_x_visible        = false;
        //top_size_y_visible        = false;
        size_is_hole            = true;
        skew_visible            = true;
        advanced_cut_visible    = true;
        taper_visible           = true;
        radius_offset_visible   = true;
        revolutions_visible     = true;
        twist_min               = OBJECT_TWIST_MIN;
        twist_max               = OBJECT_TWIST_MAX;
        twist_inc               = OBJECT_TWIST_INC;

        break;

    case MI_SCULPT:
        cut_visible             = false;
        hollow_visible          = false;
        twist_visible           = false;
        top_size_x_visible      = false;
        top_size_y_visible      = false;
        top_shear_x_visible     = false;
        top_shear_y_visible     = false;
        skew_visible            = false;
        advanced_cut_visible    = false;
        taper_visible           = false;
        radius_offset_visible   = false;
        revolutions_visible     = false;
        sculpt_texture_visible  = true;

        break;

    case MI_BOX:
        advanced_cut_visible    = true;
        advanced_is_slice       = true;
        taper_visible           = false; //KC: does nothing for boxes
        break;

    case MI_CYLINDER:
        advanced_cut_visible    = true;
        advanced_is_slice       = true;
        break;

    case MI_PRISM:
        advanced_cut_visible    = true;
        advanced_is_slice       = true;
        break;

    default:
        break;
    }

    // Check if we need to change top size/hole size params.
    switch (selected_item)
    {
    case MI_SPHERE:
        mSpinScaleX->set( scale_x );
        mSpinScaleY->set( scale_y );
        calcp->setVar(LLCalc::X_HOLE, scale_x);
        calcp->setVar(LLCalc::Y_HOLE, scale_y);
        mSpinScaleX->setMinValue(0.0f);
        mSpinScaleX->setMaxValue(1.0f);
        mSpinScaleY->setMinValue(0.0f);
        mSpinScaleY->setMaxValue(1.0f);
        break;
    case MI_TORUS:
    case MI_TUBE:
    case MI_RING:
        mSpinScaleX->set( scale_x );
        mSpinScaleY->set( scale_y );
        mSpinScaleX->setMinValue(mMinHoleSize);// <AW: opensim-limits>
        calcp->setVar(LLCalc::X_HOLE, scale_x);
        calcp->setVar(LLCalc::Y_HOLE, scale_y);
//      mSpinScaleX->setMinValue(OBJECT_MIN_HOLE_SIZE);
        mSpinScaleX->setMinValue(mMinHoleSize);// <AW: opensim-limits>
        mSpinScaleX->setMaxValue(OBJECT_MAX_HOLE_SIZE_X);
//      mSpinScaleY->setMinValue(OBJECT_MIN_HOLE_SIZE);
        mSpinScaleY->setMinValue(mMinHoleSize);// <AW: opensim-limits>
        mSpinScaleY->setMaxValue(OBJECT_MAX_HOLE_SIZE_Y);
        break;
    default:
        if (editable && single_volume)
        {
            // <FS> Make sure to use the correct values for the LL default types. These
            //      should be handled by the explicit cases and the ones in this if-condition.
            //      Everything else are the special FS types and profiles
            if (selected_item == MI_BOX || selected_item == MI_CYLINDER || selected_item == MI_PRISM)
            {
                mSpinScaleX->set( 1.f - scale_x );
                mSpinScaleY->set( 1.f - scale_y );
                mSpinScaleX->setMinValue(-1.f);
                mSpinScaleX->setMaxValue(1.f);
                mSpinScaleY->setMinValue(-1.f);
                mSpinScaleY->setMaxValue(1.f);
            }
            else
            {
                mSpinScaleX->set( scale_x );
                mSpinScaleY->set( scale_x );
                mSpinScaleX->setMinValue(-4000.f);
                mSpinScaleX->setMaxValue(4000.f);
                mSpinScaleY->setMinValue(-4000.f);
                mSpinScaleY->setMaxValue(4000.f);
            }

            // Torus' Hole Size is Box/Cyl/Prism's Taper
            calcp->setVar(LLCalc::X_TAPER, 1.f - scale_x);
            calcp->setVar(LLCalc::Y_TAPER, 1.f - scale_y);

            // Box/Cyl/Prism have no hole size
            calcp->setVar(LLCalc::X_HOLE, 0.f);
            calcp->setVar(LLCalc::Y_HOLE, 0.f);
        }
        break;
    }

    // Check if we need to limit the hollow based on the hole type.
//KC: Phoenix capability
#if 0
    if (  selected_hole == MI_HOLE_SQUARE &&
          ( selected_item == MI_CYLINDER || selected_item == MI_TORUS ||
            selected_item == MI_PRISM    || selected_item == MI_RING  ||
            selected_item == MI_SPHERE ) )
    {
        mSpinHollow->setMinValue(0.f);
        mSpinHollow->setMaxValue(70.f);
    }
    else
#endif
    {
        mSpinHollow->setMinValue(0.f);
//      mSpinHollow->setMaxValue(100.f);
        mSpinHollow->setMaxValue(mMaxHollowSize);// <AW: opensim-limits>
    }

    // Update field enablement
    mComboBaseType  ->setEnabled( enabled );
    //mMenuClipboardParams->setEnabled(enabled); // <FS> Extended copy & paste buttons

    mLabelCut       ->setEnabled( enabled );
    mSpinCutBegin   ->setEnabled( enabled );
    mSpinCutEnd     ->setEnabled( enabled );

    mLabelHollow    ->setEnabled( enabled );
    mSpinHollow     ->setEnabled( enabled );
    mLabelHoleType  ->setEnabled( hole_enabled );
    mComboHoleType  ->setEnabled( hole_enabled );

    mLabelTwist     ->setEnabled( enabled );
    mSpinTwist      ->setEnabled( enabled );
    mSpinTwistBegin ->setEnabled( enabled );

    mLabelSkew      ->setEnabled( enabled );
    mSpinSkew       ->setEnabled( enabled );

    getChildView("scale_hole")->setVisible( false);
    getChildView("scale_taper")->setVisible( false);
    if (top_size_x_visible || top_size_y_visible)
    {
        if (size_is_hole)
        {
            getChildView("scale_hole")->setVisible( true);
            getChildView("scale_hole")->setEnabled(enabled);
        }
        else
        {
            getChildView("scale_taper")->setVisible( true);
            getChildView("scale_taper")->setEnabled(enabled);
        }
    }

    mSpinScaleX     ->setEnabled( enabled );
    mSpinScaleY     ->setEnabled( enabled );

    mLabelShear     ->setEnabled( enabled );
    mSpinShearX     ->setEnabled( enabled );
    mSpinShearY     ->setEnabled( enabled );

    getChildView("advanced_cut")->setVisible( false);
    getChildView("advanced_dimple")->setVisible( false);
    getChildView("advanced_slice")->setVisible( false);

    if (advanced_cut_visible)
    {
        if (advanced_is_dimple)
        {
            getChildView("advanced_dimple")->setVisible( true);
            getChildView("advanced_dimple")->setEnabled(enabled);
        }

        else if (advanced_is_slice)
        {
            getChildView("advanced_slice")->setVisible( true);
            getChildView("advanced_slice")->setEnabled(enabled);
        }
        else
        {
            getChildView("advanced_cut")->setVisible( true);
            getChildView("advanced_cut")->setEnabled(enabled);
        }
    }

    mCtrlPathBegin  ->setEnabled( enabled );
    mCtrlPathEnd    ->setEnabled( enabled );

    mLabelTaper     ->setEnabled( enabled );
    mSpinTaperX     ->setEnabled( enabled );
    mSpinTaperY     ->setEnabled( enabled );

    mLabelRadiusOffset->setEnabled( enabled );
    mSpinRadiusOffset ->setEnabled( enabled );

    mLabelRevolutions->setEnabled( enabled );
    mSpinRevolutions ->setEnabled( enabled );

    // Update field visibility
    mLabelCut       ->setVisible( cut_visible );
    mSpinCutBegin   ->setVisible( cut_visible );
    mSpinCutEnd     ->setVisible( cut_visible );

    mLabelHollow    ->setVisible( hollow_visible );
    mSpinHollow     ->setVisible( hollow_visible );
    mLabelHoleType  ->setVisible( hollow_visible );
    mComboHoleType  ->setVisible( hollow_visible );

    mLabelTwist     ->setVisible( twist_visible );
    mSpinTwist      ->setVisible( twist_visible );
    mSpinTwistBegin ->setVisible( twist_visible );
    mSpinTwist      ->setMinValue(  twist_min );
    mSpinTwist      ->setMaxValue(  twist_max );
    mSpinTwist      ->setIncrement( twist_inc );
    mSpinTwistBegin ->setMinValue(  twist_min );
    mSpinTwistBegin ->setMaxValue(  twist_max );
    mSpinTwistBegin ->setIncrement( twist_inc );

    mSpinScaleX     ->setVisible( top_size_x_visible );
    mSpinScaleY     ->setVisible( top_size_y_visible );

    mLabelSkew      ->setVisible( skew_visible );
    mSpinSkew       ->setVisible( skew_visible );

    mLabelShear     ->setVisible( top_shear_x_visible || top_shear_y_visible );
    mSpinShearX     ->setVisible( top_shear_x_visible );
    mSpinShearY     ->setVisible( top_shear_y_visible );

    mCtrlPathBegin  ->setVisible( advanced_cut_visible );
    mCtrlPathEnd    ->setVisible( advanced_cut_visible );

    mLabelTaper     ->setVisible( taper_visible );
    mSpinTaperX     ->setVisible( taper_visible );
    mSpinTaperY     ->setVisible( taper_visible );

    mLabelRadiusOffset->setVisible( radius_offset_visible );
    mSpinRadiusOffset ->setVisible( radius_offset_visible );

    mLabelRevolutions->setVisible( revolutions_visible );
    mSpinRevolutions ->setVisible( revolutions_visible );

    mCtrlSculptTexture->setVisible(sculpt_texture_visible);
    mLabelSculptType->setVisible(sculpt_texture_visible);
    mCtrlSculptType->setVisible(sculpt_texture_visible);
    //<FS:Beq> FIRE-21445 + Mesh Info in object panel
    deactivateMeshFields();
    //</FS:Beq>

    // sculpt texture
    //<FS:Beq> extend mesh info to no-edit items
    if (selected_item == MI_SCULPT || mSelectedType == MI_NONE)
    //</FS:Beq>
    {

        LLUUID id;
        LLSculptParams *sculpt_params = (LLSculptParams *)objectp->getParameterEntry(LLNetworkData::PARAMS_SCULPT);


        if (sculpt_params) // if we have a legal sculpt param block for this object:
        {
            if (mObject != objectp)  // we've just selected a new object, so save for undo
            {
                mSculptTextureRevert = sculpt_params->getSculptTexture();
                mSculptTypeRevert = sculpt_params->getSculptType();
            }

            U8 sculpt_type = sculpt_params->getSculptType();
            U8 sculpt_stitching = sculpt_type & LL_SCULPT_TYPE_MASK;
            bool sculpt_invert = sculpt_type & LL_SCULPT_FLAG_INVERT;
            bool sculpt_mirror = sculpt_type & LL_SCULPT_FLAG_MIRROR;
            isMesh = (sculpt_stitching == LL_SCULPT_TYPE_MESH);

            // <FS:Beq> FIRE-21445 - Show specific LOD + Mesh Info in object panel
            if (isMesh)
            {
                deactivateStandardFields();
                activateMeshFields(objectp);
            }
            else if (!isMesh && mSelectedType == MI_NONE)
            {
                // Do nothing as the perms are off.
            }
            else
            //</FS:Beq>
            {
                LLTextureCtrl*  mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");
                if (mTextureCtrl)
                {
                    mTextureCtrl->setTentative(false);
                    mTextureCtrl->setEnabled(editable && !isMesh);
                    if (editable)
                        mTextureCtrl->setImageAssetID(sculpt_params->getSculptTexture());
                    else
                        mTextureCtrl->setImageAssetID(LLUUID::null);
                }

                mComboBaseType->setEnabled(!isMesh);
                //mMenuClipboardParams->setEnabled(!isMesh); // <FS> Extended copy & paste buttons

                if (mCtrlSculptType)
                {
                    if (sculpt_stitching == LL_SCULPT_TYPE_NONE)
                    {
                        // since 'None' is no longer an option in the combo box
                        // use 'Plane' as an equivalent sculpt type
                        mCtrlSculptType->setSelectedByValue(LLSD(LL_SCULPT_TYPE_PLANE), true);
                    }
                    else
                    {
                        mCtrlSculptType->setSelectedByValue(LLSD(sculpt_stitching), true);
                    }
                    mCtrlSculptType->setEnabled(editable && !isMesh);
                }

                if (mCtrlSculptMirror)
                {
                    mCtrlSculptMirror->set(sculpt_mirror);
                    mCtrlSculptMirror->setEnabled(editable && !isMesh);
                }

                if (mCtrlSculptInvert)
                {
                    mCtrlSculptInvert->set(sculpt_invert);
                    mCtrlSculptInvert->setEnabled(editable);
                }

                if (mLabelSculptType)
                {
                    mLabelSculptType->setEnabled(true);
                }
            //<FS:Beq> FIRE-21445
            }
            //</FS:Beq>
        }
    }
    else
    {
        mSculptTextureRevert = LLUUID::null;
    }

    mCtrlSculptMirror->setVisible(sculpt_texture_visible && !isMesh);
    mCtrlSculptInvert->setVisible(sculpt_texture_visible && !isMesh);

    //----------------------------------------------------------------------------
    mObject = objectp;
    mRootObject = root_objectp;
}
//<FS:Beq> FIRE-21445 + Mesh Info in object panel
// Helper function duplicating the inline switch statements which ideally we'd refactor but...ugh MAINT
void LLPanelObject::deactivateStandardFields()
{

    // Update field visibility
    mComboBaseType->setEnabled(false);
    mComboBaseType->setVisible(false);

    mLabelCut->setVisible(false);
    mSpinCutBegin->setVisible(false);
    mSpinCutEnd->setVisible(false);

    mLabelHollow->setVisible(false);
    mSpinHollow->setVisible(false);
    mLabelHoleType->setVisible(false);
    mComboHoleType->setVisible(false);

    mLabelTwist->setVisible(false);
    mSpinTwist->setVisible(false);
    mSpinTwistBegin->setVisible(false);
    mSpinTwist->setMinValue(false);
    mSpinTwist->setMaxValue(false);
    mSpinTwist->setIncrement(false);
    mSpinTwistBegin->setMinValue(false);
    mSpinTwistBegin->setMaxValue(false);
    mSpinTwistBegin->setIncrement(false);

    mSpinScaleX->setVisible(false);
    mSpinScaleY->setVisible(false);

    mLabelSkew->setVisible(false);
    mSpinSkew->setVisible(false);

    mLabelShear->setVisible(false);
    mSpinShearX->setVisible(false);
    mSpinShearY->setVisible(false);

    mCtrlPathBegin->setVisible(false);
    mCtrlPathEnd->setVisible(false);

    mLabelTaper->setVisible(false);
    mSpinTaperX->setVisible(false);
    mSpinTaperY->setVisible(false);

    mLabelRadiusOffset->setVisible(false);
    mSpinRadiusOffset->setVisible(false);

    mLabelRevolutions->setVisible(false);
    mSpinRevolutions->setVisible(false);

    mCtrlSculptTexture->setVisible(false);
    mLabelSculptType->setVisible(false);
    mCtrlSculptType->setVisible(false);

    getChildView("scale_hole")->setVisible(false);
    getChildView("scale_taper")->setVisible(false);

    getChildView("advanced_cut")->setVisible(false);
    getChildView("advanced_dimple")->setVisible(false);
    getChildView("advanced_slice")->setVisible(false);
}

void LLPanelObject::activateMeshFields(LLViewerObject* objectp)
{
    LLStringUtil::format_map_t args;
    static const char * dataFields[4] = { "LOWESTTRIS", "LOWTRIS", "MIDTRIS", "HIGHTRIS" };

    LLTextBox* num_tris = getChild<LLTextBox>("mesh_lod_num_tris");
    for (int i = 0; i < 4; i++)
    {
        args[dataFields[i]] = llformat("%d", objectp->mDrawable->getVOVolume()->getLODTriangleCount(i));
    }
    num_tris->setText(getString("mesh_lod_num_tris_values",args));
    num_tris->setVisible(true);

    childSetVisible("mesh_info_label", true);
    childSetVisible("lod_label", true);
    childSetVisible("lod_num_tris", true);
    childSetVisible("mesh_lod_label", true);
    // Mesh specific display
    mComboLOD->setEnabled(true);
    mComboLOD->setVisible(true);

    F32 radius;

    if (objectp->mDrawable->isState(LLDrawable::RIGGED))
    {
        LLVOAvatar* avatar = objectp->getAvatar();

        // Not sure how this can really happen, but alas it does. Better exit here than crashing.
        if (!avatar || !avatar->mDrawable)
        {
            return;
        }
        radius = avatar->getBinRadius();
    }
    else
    {
        radius = objectp->getVolume() ? objectp->getVolume()->mLODScaleBias.scaledVec(objectp->getScale()).length() : objectp->getScale().length();
    }

    constexpr F32 max_distance = 512.f;
    F32 factor;
    F32 dlowest = llmin(radius / 0.03f, max_distance);
    F32 dlow = llmin(radius / 0.06f, max_distance);
    F32 dmid = llmin(radius / 0.24f, max_distance);

    childSetVisible("object_radius", true);
    LLTextBox* tb = getChild<LLTextBox>("object_radius_value");
    tb->setText(llformat("%.3f", radius));
    tb->setVisible(true);

    childSetVisible("LOD_swap_defaults_label", true);
    childSetVisible("LOD_swap_usr_label", true);
    childSetVisible("LOD_swap_factors_label", true);
    childSetVisible("LOD_swap_label", true);
    childSetVisible("LOD_swap_LOD_Change_label", true);
    childSetVisible("LODSwapTableDscriptionsText", true);
    childSetVisible("ObjectLODbehaviourLabel", true);

    // Setup the LL defaults
    factor = 1.25f; // LL default for most people http://wiki.qikfox3Dviewer.org/support:whirly_fizzle#lod_comparison
    args["FACTOR"] = llformat("%.3f", factor);
    tb = getChild<LLTextBox>("LOD_swap_ll_default");
    tb->setToolTip(getString("ll_lod_tooltip_msg",args));
    tb->setVisible(true);

    tb = getChild<LLTextBox>("LOD_swap_ll_values");
    setLODDistValues(tb, factor, dmid, dlow, dlowest);

    // now the FS defaults
    factor = 2.0f;
    args["FACTOR"] = llformat("%.3f", factor);
    tb = getChild<LLTextBox>("LOD_swap_fs_default");
    tb->setToolTip(getString("fs_lod_tooltip_msg", args));
    tb->setVisible(true);

    tb = getChild<LLTextBox>("LOD_swap_fs_values");
    setLODDistValues(tb, factor, dmid, dlow, dlowest);

    // finally the user's own LODFactor
    factor = LLVOVolume::sLODFactor;
    args["FACTOR"] = llformat("%.3f", factor);
    tb = getChild<LLTextBox>("LOD_swap_usr_current");
    tb->setText(getString("user_lod_label_string", args));// Note: here we are setting the label not the tooltip
    tb->setVisible(true);

    tb = getChild<LLTextBox>("LOD_swap_usr_values");
    setLODDistValues(tb, factor, dmid, dlow, dlowest);
}

void LLPanelObject::setLODDistValues(LLTextBox* tb, F32 factor, F32 dmid, F32 dlow, F32 dlowest)
{
    if (tb)
    {
        LLStringUtil::format_map_t args;
        args["HIGH2MED"] = llformat("%.1f", factor*dmid);
        args["MED2LOW"] = llformat("%.1f", factor*dlow);
        args["LOW2LOWEST"] = llformat("%.1f", factor*dlowest);
        tb->setText(getString("LODSwapFormatString",args));
        tb->setVisible(true);
        tb->setEnabled(true);
    }
}

void LLPanelObject::deactivateMeshFields()
{
    childSetVisible("mesh_info_label", false);
    childSetVisible("lod_label", false);
    childSetVisible("lod_num_tris", false);
    childSetVisible("mesh_lod_num_tris", false);
    childSetVisible("mesh_lod_label", false);
    // reset the debug setting as we are editing a new object
    gSavedSettings.setS32("ShowSpecificLODInEdit", -1);
    // </FS:Beq>

    mComboLOD->setCurrentByIndex(0);
    mComboLOD->setEnabled(false);
    mComboLOD->setVisible(false);

    childSetVisible("object_radius", false);
    LLTextBox* tb = getChild<LLTextBox>("object_radius_value");
    tb->setVisible(false);

    childSetVisible("ObjectLODbehaviourLabel", false);
    childSetVisible("LOD_swap_defaults_label", false);
    childSetVisible("LOD_swap_factors_label", false);
    childSetVisible("LOD_swap_usr_label", false);
    childSetVisible("LOD_swap_label", false);
    childSetVisible("LOD_swap_LOD_Change_label", false);
    childSetVisible("LODSwapTableDscriptionsText", false);
    childSetVisible("LOD_swap_ll_default", false);
    childSetVisible("LOD_swap_fs_default", false);
    childSetVisible("LOD_swap_usr_current", false);
    childSetVisible("LOD_swap_ll_values", false);
    childSetVisible("LOD_swap_fs_values", false);
    childSetVisible("LOD_swap_usr_values", false);
}
//</FS:Beq>

// static
bool LLPanelObject::precommitValidate( const LLSD& data )
{
    // TODO: Richard will fill this in later.
    return true; // false means that validation failed and new value should not be commited.
}

void LLPanelObject::sendIsPhysical()
{
    bool value = mCheckPhysics->get();
    if( mIsPhysical != value )
    {
        LLSelectMgr::getInstance()->selectionUpdatePhysics(value);
        mIsPhysical = value;

        LL_INFOS("FloaterTools") << "update physics sent" << LL_ENDL;
    }
    else
    {
        LL_INFOS("FloaterTools") << "update physics not changed" << LL_ENDL;
    }
}

void LLPanelObject::sendIsTemporary()
{
    bool value = mCheckTemporary->get();
    if( mIsTemporary != value )
    {
        LLSelectMgr::getInstance()->selectionUpdateTemporary(value);
        mIsTemporary = value;

        LL_INFOS("FloaterTools") << "update temporary sent" << LL_ENDL;
    }
    else
    {
        LL_INFOS("FloaterTools") << "update temporary not changed" << LL_ENDL;
    }
}


void LLPanelObject::sendIsPhantom()
{
    bool value = mCheckPhantom->get();
    if( mIsPhantom != value )
    {
        LLSelectMgr::getInstance()->selectionUpdatePhantom(value);
        mIsPhantom = value;

        LL_INFOS("FloaterTools") << "update phantom sent" << LL_ENDL;
    }
    else
    {
        LL_INFOS("FloaterTools") << "update phantom not changed" << LL_ENDL;
    }
}

//<FS:Beq> FIRE-21445 + Mesh Info in object panel
void LLPanelObject::onCommitLOD()
{
    if (mObject.isNull())
    {
        return;
    }
    // We use the setting to pass down the override because overriding at this point get reset by other code in the render pipeline.
    // the actual forceLOD call is made in llviewerwindow.cpp
    gSavedSettings.setS32("ShowSpecificLODInEdit", mComboLOD->getValue());
}
//</FS:Beq>

// static
void LLPanelObject::onCommitParametric( LLUICtrl* ctrl, void* userdata )
{
    LLPanelObject* self = (LLPanelObject*) userdata;

    if (self->mObject.isNull())
    {
        return;
    }

    if (self->mObject->getPCode() != LL_PCODE_VOLUME)
    {
        // Don't allow modification of non-volume objects.
        return;
    }

    LLVolume *volume = self->mObject->getVolume();
    if (!volume)
    {
        return;
    }

    LLVolumeParams volume_params;
    self->getVolumeParams(volume_params);



    // set sculpting
    S32 selected_type = self->mComboBaseType->getCurrentIndex();

    if (selected_type == MI_SCULPT)
    {
        self->mObject->setParameterEntryInUse(LLNetworkData::PARAMS_SCULPT, true, true);
        LLSculptParams *sculpt_params = (LLSculptParams *)self->mObject->getParameterEntry(LLNetworkData::PARAMS_SCULPT);
        if (sculpt_params)
            volume_params.setSculptID(sculpt_params->getSculptTexture(), sculpt_params->getSculptType());
    }
    else
    {
        LLSculptParams *sculpt_params = (LLSculptParams *)self->mObject->getParameterEntry(LLNetworkData::PARAMS_SCULPT);
        if (sculpt_params)
            self->mObject->setParameterEntryInUse(LLNetworkData::PARAMS_SCULPT, false, true);
    }

    // Update the volume, if necessary.
    self->mObject->updateVolume(volume_params);


    // This was added to make sure thate when changes are made, the UI
    // adjusts to present valid options.
    // *FIX: only some changes, ie, hollow or primitive type changes,
    // require a refresh.
    self->refresh();

}

void LLPanelObject::getVolumeParams(LLVolumeParams& volume_params)
{
    // Figure out what type of volume to make
    S32 was_selected_type = mSelectedType;
    S32 selected_type = mComboBaseType->getCurrentIndex();
    U8 profile;
    U8 path;
    switch ( selected_type )
    {
    case MI_CYLINDER:
        profile = LL_PCODE_PROFILE_CIRCLE;
        path = LL_PCODE_PATH_LINE;
        break;

    case MI_BOX:
        profile = LL_PCODE_PROFILE_SQUARE;
        path = LL_PCODE_PATH_LINE;
        break;

    case MI_PRISM:
        profile = LL_PCODE_PROFILE_EQUALTRI;
        path = LL_PCODE_PATH_LINE;
        break;

    case MI_SPHERE:
        profile = LL_PCODE_PROFILE_CIRCLE_HALF;
        path = LL_PCODE_PATH_CIRCLE;
        break;

    case MI_TORUS:
        profile = LL_PCODE_PROFILE_CIRCLE;
        path = LL_PCODE_PATH_CIRCLE;
        break;

    case MI_TUBE:
        profile = LL_PCODE_PROFILE_SQUARE;
        path = LL_PCODE_PATH_CIRCLE;
        break;

    case MI_RING:
        profile = LL_PCODE_PROFILE_EQUALTRI;
        path = LL_PCODE_PATH_CIRCLE;
        break;

    case MI_SCULPT:
        profile = LL_PCODE_PROFILE_CIRCLE;
        path = LL_PCODE_PATH_CIRCLE;
        break;

    case MI_PATH_LINE_PROFILE_CIRCLE_HALF:
        profile = LL_PCODE_PROFILE_CIRCLE_HALF;
        path = LL_PCODE_PATH_LINE;
        break;

    case MI_PATH_CIRCLE_PROFILE_CIRCLE_HALF:
        profile = LL_PCODE_PROFILE_CIRCLE_HALF;
        path = LL_PCODE_PATH_CIRCLE;
        break;

    case MI_PATH_CIRCLE2_PROFILE_SQUARE:
        profile = LL_PCODE_PROFILE_SQUARE;
        path = LL_PCODE_PATH_CIRCLE2;
        break;

    case MI_PATH_CIRCLE2_PROFILE_TRI:
        profile = LL_PCODE_PROFILE_EQUALTRI;
        path = LL_PCODE_PATH_CIRCLE2;
        break;

    case MI_PATH_CIRCLE2_PROFILE_CIRCLE:
        profile = LL_PCODE_PROFILE_CIRCLE;
        path = LL_PCODE_PATH_CIRCLE2;
        break;

    case MI_PATH_CIRCLE2_PROFILE_CIRCLE_HALF:
        profile = LL_PCODE_PROFILE_CIRCLE_HALF;
        path = LL_PCODE_PATH_CIRCLE2;
        break;

    case MI_PATH_TEST_PROFILE_SQUARE:
        profile = LL_PCODE_PROFILE_SQUARE;
        path = LL_PCODE_PATH_TEST;
        break;

    case MI_PATH_TEST_PROFILE_TRI:
        profile = LL_PCODE_PROFILE_EQUALTRI;
        path = LL_PCODE_PATH_TEST;
        break;

    case MI_PATH_TEST_PROFILE_CIRCLE:
        profile = LL_PCODE_PROFILE_CIRCLE;
        path = LL_PCODE_PATH_TEST;
        break;

    case MI_PATH_TEST_PROFILE_CIRCLE_HALF:
        profile = LL_PCODE_PROFILE_CIRCLE_HALF;
        path = LL_PCODE_PATH_TEST;
        break;

//<-- Working33 by Gregory Maurer
    case MI_PATH_33_PROFILE_CIRCLE:
        profile = LL_PCODE_PROFILE_CIRCLE;
        path = LL_PCODE_PATH_CIRCLE_33;
        break;

    case MI_PATH_33_PROFILE_SQUARE:
        profile = LL_PCODE_PROFILE_SQUARE;
        path = LL_PCODE_PATH_CIRCLE_33;
        break;

    case MI_PATH_33_PROFILE_TRIANGLE:
        profile = LL_PCODE_PROFILE_ISOTRI;
        path = LL_PCODE_PATH_CIRCLE_33;
        break;

    case MI_PATH_33_PROFILE_HALFCIRCLE:
        profile = LL_PCODE_PROFILE_CIRCLE_HALF;
        path = LL_PCODE_PATH_CIRCLE_33;
        break;
//Working33 -->

    default:
        LL_WARNS("FloaterTools") << "Unknown base type " << selected_type
            << " in getVolumeParams()" << LL_ENDL;
        // assume a box
        selected_type = MI_BOX;
        profile = LL_PCODE_PROFILE_SQUARE;
        path = LL_PCODE_PATH_LINE;
        break;
    }


    if (path == LL_PCODE_PATH_LINE)
    {
        LLVOVolume *volobjp = (LLVOVolume *)(LLViewerObject*)(mObject);
        if (volobjp->isFlexible())
        {
            path = LL_PCODE_PATH_FLEXIBLE;
        }
    }

    S32 selected_hole = mComboHoleType->getCurrentIndex();
    U8 hole;
    switch (selected_hole)
    {
    case MI_HOLE_CIRCLE:
        hole = LL_PCODE_HOLE_CIRCLE;
        break;
    case MI_HOLE_SQUARE:
        hole = LL_PCODE_HOLE_SQUARE;
        break;
    case MI_HOLE_TRIANGLE:
        hole = LL_PCODE_HOLE_TRIANGLE;
        break;
    case MI_HOLE_SAME:
    default:
        hole = LL_PCODE_HOLE_SAME;
        break;
    }

    volume_params.setType(profile | hole, path);
    mSelectedType = selected_type;

    // Compute cut start/end
    F32 cut_begin   = mSpinCutBegin->get();
    F32 cut_end     = mSpinCutEnd->get();

    // Make sure at least OBJECT_CUT_INC of the object survives
    if (cut_begin > cut_end - OBJECT_MIN_CUT_INC)
    {
        cut_begin = cut_end - OBJECT_MIN_CUT_INC;
        mSpinCutBegin->set(cut_begin);
    }

    F32 adv_cut_begin   = mCtrlPathBegin->get();
    F32 adv_cut_end     = mCtrlPathEnd->get();

    // Make sure at least OBJECT_CUT_INC of the object survives
    if (adv_cut_begin > adv_cut_end - OBJECT_MIN_CUT_INC)
    {
        adv_cut_begin = adv_cut_end - OBJECT_MIN_CUT_INC;
        mCtrlPathBegin->set(adv_cut_begin);
    }

    F32 begin_s, end_s;
    F32 begin_t, end_t;

    if (selected_type == MI_SPHERE || selected_type == MI_TORUS ||
        selected_type == MI_TUBE   || selected_type == MI_RING)
    {
        begin_s = adv_cut_begin;
        end_s   = adv_cut_end;

        begin_t = cut_begin;
        end_t   = cut_end;
    }
    else
    {
        begin_s = cut_begin;
        end_s   = cut_end;

        begin_t = adv_cut_begin;
        end_t   = adv_cut_end;
    }

    volume_params.setBeginAndEndS(begin_s, end_s);
    volume_params.setBeginAndEndT(begin_t, end_t);

    // Hollowness
    F32 hollow = mSpinHollow->get() / 100.f;

//KC: Phoenix capability
#if 0
    if (  selected_hole == MI_HOLE_SQUARE &&
        ( selected_type == MI_CYLINDER || selected_type == MI_TORUS ||
          selected_type == MI_PRISM    || selected_type == MI_RING  ||
          selected_type == MI_SPHERE ) )
    {
        if (hollow > 0.7f) hollow = 0.7f;
    }
#endif

    volume_params.setHollow( hollow );

    // Twist Begin,End
    F32 twist_begin = mSpinTwistBegin->get();
    F32 twist       = mSpinTwist->get();
    // Check the path type for twist conversion.
    if (path == LL_PCODE_PATH_LINE || path == LL_PCODE_PATH_FLEXIBLE)
    {
        twist_begin /= OBJECT_TWIST_LINEAR_MAX;
        twist       /= OBJECT_TWIST_LINEAR_MAX;
    }
    else
    {
        twist_begin /= OBJECT_TWIST_MAX;
        twist       /= OBJECT_TWIST_MAX;
    }

    volume_params.setTwistBegin(twist_begin);
    volume_params.setTwist(twist);

    // Scale X,Y
    F32 scale_x = mSpinScaleX->get();
    F32 scale_y = mSpinScaleY->get();
    if ( was_selected_type == MI_BOX || was_selected_type == MI_CYLINDER || was_selected_type == MI_PRISM)
    {
        scale_x = 1.f - scale_x;
        scale_y = 1.f - scale_y;
    }

    // Skew
    F32 skew = mSpinSkew->get();

    // Taper X,Y
    F32 taper_x = mSpinTaperX->get();
    F32 taper_y = mSpinTaperY->get();

    // Radius offset
    F32 radius_offset = mSpinRadiusOffset->get();

    // Revolutions
    F32 revolutions   = mSpinRevolutions->get();

//KC: Phoenix capability
#if 0
    if ( selected_type == MI_SPHERE )
    {
        // Snap values to valid sphere parameters.
        scale_x         = 1.0f;
        scale_y         = 1.0f;
        skew            = 0.0f;
        taper_x         = 0.0f;
        taper_y         = 0.0f;
        radius_offset   = 0.0f;
        revolutions     = 1.0f;
    }
    else if ( selected_type == MI_TORUS || selected_type == MI_TUBE ||
              selected_type == MI_RING )
    {
        scale_x = llclamp(
            scale_x,
//          OBJECT_MIN_HOLE_SIZE,
            mMinHoleSize,// <AW: opensim-limits>
            OBJECT_MAX_HOLE_SIZE_X);
        scale_y = llclamp(
            scale_y,
//          OBJECT_MIN_HOLE_SIZE,
            mMinHoleSize,// <AW: opensim-limits>
            OBJECT_MAX_HOLE_SIZE_Y);

        // Limit radius offset, based on taper and hole size y.
        F32 radius_mag = fabs(radius_offset);
        F32 hole_y_mag = fabs(scale_y);
        F32 taper_y_mag  = fabs(taper_y);
        // Check to see if the taper effects us.
        if ( (radius_offset > 0.f && taper_y < 0.f) ||
             (radius_offset < 0.f && taper_y > 0.f) )
        {
            // The taper does not help increase the radius offset range.
            taper_y_mag = 0.f;
        }
        F32 max_radius_mag = 1.f - hole_y_mag * (1.f - taper_y_mag) / (1.f - hole_y_mag);
        // Enforce the maximum magnitude.
        if (radius_mag > max_radius_mag)
        {
            // Check radius offset sign.
            if (radius_offset < 0.f)
            {
                radius_offset = -max_radius_mag;
            }
            else
            {
                radius_offset = max_radius_mag;
            }
        }

        // Check the skew value against the revolutions.
        F32 skew_mag= fabs(skew);
        F32 min_skew_mag = 1.0f - 1.0f / (revolutions * scale_x + 1.0f);
        // Discontinuity; A revolution of 1 allows skews below 0.5.
        if ( fabs(revolutions - 1.0f) < 0.001)
            min_skew_mag = 0.0f;

        // Clip skew.
        if (skew_mag < min_skew_mag)
        {
            // Check skew sign.
            if (skew < 0.0f)
            {
                skew = -min_skew_mag;
            }
            else
            {
                skew = min_skew_mag;
            }
        }
    }
#endif

    volume_params.setRatio( scale_x, scale_y );
    volume_params.setSkew(skew);
    volume_params.setTaper( taper_x, taper_y );
    volume_params.setRadiusOffset(radius_offset);
    volume_params.setRevolutions(revolutions);

    // Shear X,Y
    F32 shear_x = mSpinShearX->get();
    F32 shear_y = mSpinShearY->get();
    volume_params.setShear( shear_x, shear_y );

    if (selected_type == MI_SCULPT)
    {
        volume_params.setSculptID(LLUUID::null, 0);
        volume_params.setBeginAndEndT   (0, 1);
        volume_params.setBeginAndEndS   (0, 1);
        volume_params.setHollow         (0);
        volume_params.setTwistBegin     (0);
        volume_params.setTwistEnd       (0);
        volume_params.setRatio          (1, 0.5);
        volume_params.setShear          (0, 0);
        volume_params.setTaper          (0, 0);
        volume_params.setRevolutions    (1);
        volume_params.setRadiusOffset   (0);
        volume_params.setSkew           (0);
    }

}

// BUG: Make work with multiple objects
void LLPanelObject::sendRotation(bool btn_down)
{
    if (mObject.isNull()) return;

    LLVector3 new_rot(mCtrlRotX->get(), mCtrlRotY->get(), mCtrlRotZ->get());
    new_rot.mV[VX] = ll_round(new_rot.mV[VX], OBJECT_ROTATION_PRECISION);
    new_rot.mV[VY] = ll_round(new_rot.mV[VY], OBJECT_ROTATION_PRECISION);
    new_rot.mV[VZ] = ll_round(new_rot.mV[VZ], OBJECT_ROTATION_PRECISION);

    // Note: must compare before conversion to radians
    LLVector3 delta = new_rot - mCurEulerDegrees;

    if (delta.magVec() >= 0.0005f)
    {
        mCurEulerDegrees = new_rot;
        new_rot *= DEG_TO_RAD;

        LLQuaternion rotation;
        rotation.setQuat(new_rot.mV[VX], new_rot.mV[VY], new_rot.mV[VZ]);

        if (mRootObject != mObject)
        {
            // ## Zi: Building spin controls for attachments
            if(mObject->isAttachment())
                rotation = rotation * ~mRootObject->getRotation();
            else
                rotation = rotation * ~mRootObject->getRotationRegion();
            // ## Zi: Building spin controls for attachments
        }

        // To include avatars into movements and rotation
        // If false, all children are selected anyway - move avatar
        // If true, not all children are selected - save positions
        bool individual_selection = gSavedSettings.getBOOL("EditLinkedParts");
        std::vector<LLVector3>& child_positions = mObject->mUnselectedChildrenPositions ;
        std::vector<LLQuaternion> child_rotations;
        if (mObject->isRootEdit() && individual_selection)
        {
            mObject->saveUnselectedChildrenRotation(child_rotations) ;
            mObject->saveUnselectedChildrenPosition(child_positions) ;
        }

        mObject->setRotation(rotation);
        LLManip::rebuild(mObject) ;

        // for individually selected roots, we need to counterrotate all the children
        if (mObject->isRootEdit() && individual_selection)
        {
            mObject->resetChildrenRotationAndPosition(child_rotations, child_positions) ;
        }

        if(!btn_down)
        {
            child_positions.clear() ;
            LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_ROTATION | UPD_POSITION);
        }
    }
}

// patch up for mesh, to be removed during real mesh merge -Zi
F32 llpanelobject_max_prim_scale()
{
// <AW: opensim-limits>
//  if(gAgent.getRegion()->getCapability("GetMesh").empty())
//      return DEFAULT_MAX_PRIM_SCALE;
//  return 64.f;
    return LLWorld::getInstance()->getRegionMaxPrimScale();
// </AW: opensim-limits>
}

// BUG: Make work with multiple objects
void LLPanelObject::sendScale(bool btn_down)
{
    if (mObject.isNull()) return;

    // make sure not to send larger/smaller values than permitted -Zi
// <AW: opensim-limits>
//  LLVector3 newscale(llclamp(mCtrlScaleX->get(), MIN_PRIM_SCALE, llpanelobject_max_prim_scale()),
//                     llclamp(mCtrlScaleY->get(), MIN_PRIM_SCALE, llpanelobject_max_prim_scale()),
//                     llclamp(mCtrlScaleZ->get(), MIN_PRIM_SCALE, llpanelobject_max_prim_scale()));
    LLVector3 newscale(llclamp(mCtrlScaleX->get(), mMinScale, llpanelobject_max_prim_scale()),
                       llclamp(mCtrlScaleY->get(), mMinScale, llpanelobject_max_prim_scale()),
                       llclamp(mCtrlScaleZ->get(), mMinScale, llpanelobject_max_prim_scale()));
// </AW: opensim-limits>

    LLVector3 delta = newscale - mObject->getScale();
    if (delta.magVec() >= 0.00005f || (mSizeChanged && !btn_down))
    {
        // scale changed by more than 1/20 millimeter
        mSizeChanged = btn_down;

        // check to see if we aren't scaling the textures
        // (in which case the tex coord's need to be recomputed)
        bool dont_stretch_textures = !LLManipScale::getStretchTextures();
        if (dont_stretch_textures)
        {
            LLSelectMgr::getInstance()->saveSelectedObjectTransform(SELECT_ACTION_TYPE_SCALE);
        }

        mObject->setScale(newscale, true);

        if(!btn_down)
        {
            LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_SCALE | UPD_POSITION);
        }

        LLSelectMgr::getInstance()->adjustTexturesByScale(true, !dont_stretch_textures);
//      LL_INFOS() << "scale sent" << LL_ENDL;
    }
    else
    {
//      LL_INFOS() << "scale not changed" << LL_ENDL;
    }
}


void LLPanelObject::sendPosition(bool btn_down)
{
    if (mObject.isNull()) return;

    LLVector3 newpos(mCtrlPosX->get(), mCtrlPosY->get(), mCtrlPosZ->get());

    LLViewerRegion* regionp = mObject->getRegion();

    if (!regionp) return;

    // <FS:Zi> Building spin controls for attachments
    LLVector3d new_pos_global;

    if (!mObject->isAttachment())
    {
        // Clamp the Z height
        const F32 height = newpos.mV[VZ];
        // <FS:Ansariel> FIRE-12478: Clamp min Z height at new position instead of current if new position is valid
        //const F32 min_height = LLWorld::getInstance()->getMinAllowedZ(mObject, mObject->getPositionGlobal());
        LLVector3d height_check_pos = mObject->getPositionGlobal();
        if (LLWorld::getInstance()->positionRegionValidGlobal(regionp->getPosGlobalFromRegion(newpos)))
        {
            height_check_pos = regionp->getPosGlobalFromRegion(newpos);
        }
        const F32 min_height = LLWorld::getInstance()->getMinAllowedZ(mObject, height_check_pos);
        // </FS:Ansariel>
        const F32 max_height = LLWorld::getInstance()->getRegionMaxHeight();

        if ( height < min_height)
        {
            newpos.mV[VZ] = min_height;
            mCtrlPosZ->set( min_height );
        }
        else if ( height > max_height )
        {
            newpos.mV[VZ] = max_height;
            mCtrlPosZ->set( max_height );
        }

        // Grass is always drawn on the ground, so clamp its position to the ground
        if (mObject->getPCode() == LL_PCODE_LEGACY_GRASS)
        {
            mCtrlPosZ->set(LLWorld::getInstance()->resolveLandHeightAgent(newpos) + 1.f);
        }
        // Make sure new position is in a valid region, so the object
        // won't get dumped by the simulator.
        new_pos_global = regionp->getPosGlobalFromRegion(newpos);
    }
    else
    {
        if (newpos.length() > MAX_ATTACHMENT_DIST)
        {
            newpos.clampLength(MAX_ATTACHMENT_DIST);
            mCtrlPosX->set(newpos.mV[VX]);
            mCtrlPosY->set(newpos.mV[VY]);
            mCtrlPosZ->set(newpos.mV[VZ]);
        }
    }

    // <FS:Zi> Building spin controls for attachments
    // partly copied from llmaniptranslate.cpp to get the positioning right
    //bool is_valid_pos = true;
    //if (mObject->isAttachment())
    //{
    //    LLVector3 delta_pos = mObject->getPositionEdit() - newpos;
    //    LLVector3d attachment_pos = regionp->getPosGlobalFromRegion(mObject->getPositionRegion() + delta_pos);
    //    is_valid_pos = LLWorld::getInstance()->positionRegionValidGlobal(attachment_pos);
    //}
    //else
    //{
    //    is_valid_pos = LLWorld::getInstance()->positionRegionValidGlobal(new_pos_global);
    //}
    //if (is_valid_pos)
    if (mObject->isAttachment())
    {
        LLVector3 old_position_local = mObject->getPosition();

        if (mRootObject != mObject)
        {
            newpos = newpos - mRootObject->getPosition();
            newpos = newpos * ~mRootObject->getRotation();
            mObject->setPositionParent(newpos);
        }
        else
        {
            mObject->setPosition(newpos);
        }
        LLManip::rebuild(mObject);

        gAgentAvatarp->clampAttachmentPositions();

        LLVector3 new_position_local = mObject->getPosition();

        // for individually selected roots, we need to counter-translate all unselected children
        if (mObject->isRootEdit())
        {
            // counter-translate child objects if we are moving the root as an individual
            mObject->resetChildrenPosition(old_position_local - new_position_local, true);
        }

        if (!btn_down)
        {
            LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_POSITION);
        }

        LLSelectMgr::getInstance()->updateSelectionCenter();
    }
    else if (LLWorld::getInstance()->positionRegionValidGlobal(new_pos_global) )
    // </FS:Zi> Building spin controls for attachments
    {
        // send only if the position is changed, that is, the delta vector is not zero
        LLVector3d old_pos_global = mObject->getPositionGlobal();
        LLVector3d delta = new_pos_global - old_pos_global;
        // moved more than 1/20 millimeter
        if (delta.magVec() >= 0.00005f)
        {
            if (mRootObject != mObject)
            {
                newpos = newpos - mRootObject->getPositionRegion();
                newpos = newpos * ~mRootObject->getRotationRegion();
                mObject->setPositionParent(newpos);
            }
            else
            {
                mObject->setPositionEdit(newpos);
            }

            LLManip::rebuild(mObject) ;

            // for individually selected roots, we need to counter-translate all unselected children
            if (mObject->isRootEdit())
            {
                // only offset by parent's translation
                mObject->resetChildrenPosition(LLVector3(-delta), true, true) ;
            }

            if(!btn_down)
            {
                LLSelectMgr::getInstance()->sendMultipleUpdate(UPD_POSITION);
            }

            LLSelectMgr::getInstance()->updateSelectionCenter();
        }
    }
    else
    {
        // move failed, so we update the UI with the correct values
        LLVector3 vec = mRootObject->getPositionRegion();
        mCtrlPosX->set(vec.mV[VX]);
        mCtrlPosY->set(vec.mV[VY]);
        mCtrlPosZ->set(vec.mV[VZ]);
    }
}

void LLPanelObject::sendSculpt()
{
    if (mObject.isNull())
        return;

    LLSculptParams sculpt_params;
    LLUUID sculpt_id = LLUUID::null;

    if (mCtrlSculptTexture)
    {
        sculpt_id = mCtrlSculptTexture->getImageAssetID();
    }

    U8 sculpt_type = 0;

    if (mCtrlSculptType)
    {
        sculpt_type |= mCtrlSculptType->getValue().asInteger();
    }

    bool enabled = sculpt_type != LL_SCULPT_TYPE_MESH;

    if (mCtrlSculptMirror)
    {
        mCtrlSculptMirror->setEnabled(enabled);
    }

    if (mCtrlSculptInvert)
    {
        mCtrlSculptInvert->setEnabled(enabled);
    }

    if ((mCtrlSculptMirror) && (mCtrlSculptMirror->get()))
    {
        sculpt_type |= LL_SCULPT_FLAG_MIRROR;
    }

    if ((mCtrlSculptInvert) && (mCtrlSculptInvert->get()))
    {
        sculpt_type |= LL_SCULPT_FLAG_INVERT;
    }

    sculpt_params.setSculptTexture(sculpt_id, sculpt_type);
    mObject->setParameterEntry(LLNetworkData::PARAMS_SCULPT, sculpt_params, true);
}

void LLPanelObject::refresh()
{
    getState();
    if (mObject.notNull() && mObject->isDead())
    {
        mObject = NULL;
    }

    if (mRootObject.notNull() && mRootObject->isDead())
    {
        mRootObject = NULL;
    }

    F32 max_scale = get_default_max_prim_scale(LLPickInfo::isFlora(mObject));

    // <FS:Ansariel> Performance improvement
    //getChild<LLSpinCtrl>("Scale X")->setMaxValue(max_scale);
    //getChild<LLSpinCtrl>("Scale Y")->setMaxValue(max_scale);
    //getChild<LLSpinCtrl>("Scale Z")->setMaxValue(max_scale);
    mCtrlScaleX->setMaxValue(max_scale);
    mCtrlScaleY->setMaxValue(max_scale);
    mCtrlScaleZ->setMaxValue(max_scale);
    // </FS:Ansariel>
}


void LLPanelObject::draw()
{
    const LLColor4  white(  1.0f,   1.0f,   1.0f,   1);
    const LLColor4  red(    1.0f,   0.25f,  0.f,    1);
    const LLColor4  green(  0.f,    1.0f,   0.f,    1);
    const LLColor4  blue(   0.f,    0.5f,   1.0f,   1);

    // Tune the colors of the labels
    LLTool* tool = LLToolMgr::getInstance()->getCurrentTool();

    if (tool == LLToolCompTranslate::getInstance())
    {
        mCtrlPosX   ->setLabelColor(red);
        mCtrlPosY   ->setLabelColor(green);
        mCtrlPosZ   ->setLabelColor(blue);

        mCtrlScaleX ->setLabelColor(white);
        mCtrlScaleY ->setLabelColor(white);
        mCtrlScaleZ ->setLabelColor(white);

        mCtrlRotX   ->setLabelColor(white);
        mCtrlRotY   ->setLabelColor(white);
        mCtrlRotZ   ->setLabelColor(white);
    }
    else if ( tool == LLToolCompScale::getInstance() )
    {
        mCtrlPosX   ->setLabelColor(white);
        mCtrlPosY   ->setLabelColor(white);
        mCtrlPosZ   ->setLabelColor(white);

        mCtrlScaleX ->setLabelColor(red);
        mCtrlScaleY ->setLabelColor(green);
        mCtrlScaleZ ->setLabelColor(blue);

        mCtrlRotX   ->setLabelColor(white);
        mCtrlRotY   ->setLabelColor(white);
        mCtrlRotZ   ->setLabelColor(white);
    }
    else if ( tool == LLToolCompRotate::getInstance() )
    {
        mCtrlPosX   ->setLabelColor(white);
        mCtrlPosY   ->setLabelColor(white);
        mCtrlPosZ   ->setLabelColor(white);

        mCtrlScaleX ->setLabelColor(white);
        mCtrlScaleY ->setLabelColor(white);
        mCtrlScaleZ ->setLabelColor(white);

        mCtrlRotX   ->setLabelColor(red);
        mCtrlRotY   ->setLabelColor(green);
        mCtrlRotZ   ->setLabelColor(blue);
    }
    else
    {
        mCtrlPosX   ->setLabelColor(white);
        mCtrlPosY   ->setLabelColor(white);
        mCtrlPosZ   ->setLabelColor(white);

        mCtrlScaleX ->setLabelColor(white);
        mCtrlScaleY ->setLabelColor(white);
        mCtrlScaleZ ->setLabelColor(white);

        mCtrlRotX   ->setLabelColor(white);
        mCtrlRotY   ->setLabelColor(white);
        mCtrlRotZ   ->setLabelColor(white);
    }

    LLPanel::draw();
}

// virtual
void LLPanelObject::clearCtrls()
{
    LLPanel::clearCtrls();

    mCheckLock      ->set(false);
    mCheckLock      ->setEnabled( false );
    mCheckPhysics   ->set(false);
    mCheckPhysics   ->setEnabled( false );
    mCheckTemporary ->set(false);
    mCheckTemporary ->setEnabled( false );
    mCheckPhantom   ->set(false);
    mCheckPhantom   ->setEnabled( false );

    // Disable text labels
    mLabelPosition  ->setEnabled( false );
    mLabelSize      ->setEnabled( false );
    mLabelRotation  ->setEnabled( false );
    mLabelCut       ->setEnabled( false );
    mLabelHollow    ->setEnabled( false );
    mLabelHoleType  ->setEnabled( false );
    mLabelTwist     ->setEnabled( false );
    mLabelSkew      ->setEnabled( false );
    mLabelShear     ->setEnabled( false );
    mLabelTaper     ->setEnabled( false );
    mLabelRadiusOffset->setEnabled( false );
    mLabelRevolutions->setEnabled( false );

    getChildView("scale_hole")->setEnabled(false);
    getChildView("scale_taper")->setEnabled(false);
    getChildView("advanced_cut")->setEnabled(false);
    getChildView("advanced_dimple")->setEnabled(false);
    getChildView("advanced_slice")->setVisible( false);
}

//
// Static functions
//

// static
void LLPanelObject::onCommitLock(LLUICtrl *ctrl, void *data)
{
    // Checkbox will have toggled itself
    LLPanelObject *self = (LLPanelObject *)data;

    if(self->mRootObject.isNull()) return;

    bool new_state = self->mCheckLock->get();

    LLSelectMgr::getInstance()->selectionSetObjectPermissions(PERM_OWNER, !new_state, PERM_MOVE | PERM_MODIFY);
}

// static
void LLPanelObject::onCommitPosition( LLUICtrl* ctrl, void* userdata )
{
    LLPanelObject* self = (LLPanelObject*) userdata;
    bool btn_down = ((LLSpinCtrl*)ctrl)->isMouseHeldDown() ;
    self->sendPosition(btn_down);
}

// static
void LLPanelObject::onCommitScale( LLUICtrl* ctrl, void* userdata )
{
    LLPanelObject* self = (LLPanelObject*) userdata;
    bool btn_down = ((LLSpinCtrl*)ctrl)->isMouseHeldDown() ;
    self->sendScale(btn_down);
}

// static
void LLPanelObject::onCommitRotation( LLUICtrl* ctrl, void* userdata )
{
    LLPanelObject* self = (LLPanelObject*) userdata;
    bool btn_down = ((LLSpinCtrl*)ctrl)->isMouseHeldDown() ;
    self->sendRotation(btn_down);
}

// static
void LLPanelObject::onCommitPhysics( LLUICtrl* ctrl, void* userdata )
{
    LLPanelObject* self = (LLPanelObject*) userdata;
    self->sendIsPhysical();
}

// static
void LLPanelObject::onCommitTemporary( LLUICtrl* ctrl, void* userdata )
{
    LLPanelObject* self = (LLPanelObject*) userdata;
    self->sendIsTemporary();
}

// static
void LLPanelObject::onCommitPhantom( LLUICtrl* ctrl, void* userdata )
{
    LLPanelObject* self = (LLPanelObject*) userdata;
    self->sendIsPhantom();
}

void LLPanelObject::onSelectSculpt(const LLSD& data)
{
    LLTextureCtrl* mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");

    if (mTextureCtrl)
    {
        mSculptTextureRevert = mTextureCtrl->getImageAssetID();
    }

    sendSculpt();
}


void LLPanelObject::onCommitSculpt( const LLSD& data )
{
    sendSculpt();
}

bool LLPanelObject::onDropSculpt(LLInventoryItem* item)
{
    LLTextureCtrl* mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");

    if (mTextureCtrl)
    {
        LLUUID asset = item->getAssetUUID();

        mTextureCtrl->setImageAssetID(asset);
        mSculptTextureRevert = asset;
    }

    return true;
}


void LLPanelObject::onCancelSculpt(const LLSD& data)
{
    LLTextureCtrl* mTextureCtrl = getChild<LLTextureCtrl>("sculpt texture control");
    if(!mTextureCtrl)
        return;

    if(mSculptTextureRevert == LLUUID::null)
    {
        mSculptTextureRevert = SCULPT_DEFAULT_TEXTURE;
    }
    mTextureCtrl->setImageAssetID(mSculptTextureRevert);

    sendSculpt();
}

// static
void LLPanelObject::onCommitSculptType(LLUICtrl *ctrl, void* userdata)
{
    LLPanelObject* self = (LLPanelObject*) userdata;

    self->sendSculpt();
}

// <FS> Extended copy & paste buttons
//void LLPanelObject::menuDoToSelected(const LLSD& userdata)
//{
//    std::string command = userdata.asString();
//
//    // paste
//    if (command == "psr_paste")
//    {
//        onPastePos();
//        onPasteSize();
//        onPasteRot();
//    }
//    else if (command == "pos_paste")
//    {
//        onPastePos();
//    }
//    else if (command == "size_paste")
//    {
//        onPasteSize();
//    }
//    else if (command == "rot_paste")
//    {
//        onPasteRot();
//    }
//    else if (command == "params_paste")
//    {
//        onPasteParams();
//    }
//    // copy
//    else if (command == "psr_copy")
//    {
//        onCopyPos();
//        onCopySize();
//        onCopyRot();
//    }
//    else if (command == "pos_copy")
//    {
//        onCopyPos();
//    }
//    else if (command == "size_copy")
//    {
//        onCopySize();
//    }
//    else if (command == "rot_copy")
//    {
//        onCopyRot();
//    }
//    else if (command == "params_copy")
//    {
//        onCopyParams();
//    }
//}
//
//bool LLPanelObject::menuEnableItem(const LLSD& userdata)
//{
//    std::string command = userdata.asString();
//
//    // paste options
//    if (command == "psr_paste")
//    {
//        S32 selected_count = LLSelectMgr::getInstance()->getSelection()->getObjectCount();
//        bool single_volume = (LLSelectMgr::getInstance()->selectionAllPCode(LL_PCODE_VOLUME))
//            && (selected_count == 1);
//
//        if (!single_volume)
//        {
//            return false;
//        }
//
//        bool enable_move;
//        bool enable_modify;
//
//        LLSelectMgr::getInstance()->selectGetEditMoveLinksetPermissions(enable_move, enable_modify);
//
//        return enable_move && enable_modify && mHasClipboardPos && mHasClipboardSize && mHasClipboardRot;
//    }
//    else if (command == "pos_paste")
//    {
//        // assumes that menu won't be active if there is no move permission
//        return mHasClipboardPos;
//    }
//    else if (command == "size_paste")
//    {
//        return mHasClipboardSize;
//    }
//    else if (command == "rot_paste")
//    {
//        return mHasClipboardRot;
//    }
//    else if (command == "params_paste")
//    {
//        return mClipboardParams.isMap() && (mClipboardParams.size() != 0);
//    }
//    // copy options
//    else if (command == "psr_copy")
//    {
//        S32 selected_count = LLSelectMgr::getInstance()->getSelection()->getObjectCount();
//        bool single_volume = (LLSelectMgr::getInstance()->selectionAllPCode(LL_PCODE_VOLUME))
//            && (selected_count == 1);
//
//        if (!single_volume)
//        {
//            return false;
//        }
//
//        bool enable_move;
//        bool enable_modify;
//
//        LLSelectMgr::getInstance()->selectGetEditMoveLinksetPermissions(enable_move, enable_modify);
//
//        // since we forbid seeing values we also should forbid copying them
//        return enable_move && enable_modify;
//    }
//    return false;
//}

void copy_vector_to_clipboard(const LLVector3& vec)
{
    S32 precision = gSavedSettings.getS32("FSBuildToolDecimalPrecision");
    std::string format = llformat("<%%.%df, %%.%df, %%.%df>", precision, precision, precision);
    std::string stringVec = llformat(format.c_str(), vec.mV[VX], vec.mV[VY], vec.mV[VZ]);
    LLView::getWindow()->copyTextToClipboard(utf8str_to_wstring(stringVec));
}
// </FS>

void LLPanelObject::onCopyPos()
{
    mClipboardPos = LLVector3(mCtrlPosX->get(), mCtrlPosY->get(), mCtrlPosZ->get());

    std::string stringVec = llformat("<%g, %g, %g>", mClipboardPos.mV[VX], mClipboardPos.mV[VY], mClipboardPos.mV[VZ]);
    // <FS> Extended copy & paste buttons
    //LLView::getWindow()->copyTextToClipboard(utf8str_to_wstring(stringVec));
    copy_vector_to_clipboard(mClipboardPos);

    LLStringUtil::format_map_t args;
    args["VALUE"] = stringVec;
    mBtnPastePos->setToolTip(getString("paste_position", args));

    mBtnPastePos->setEnabled(true);
    // </FS>

    mHasClipboardPos = true;
}

void LLPanelObject::onCopySize()
{
    mClipboardSize = LLVector3(mCtrlScaleX->get(), mCtrlScaleY->get(), mCtrlScaleZ->get());

    std::string stringVec = llformat("<%g, %g, %g>", mClipboardSize.mV[VX], mClipboardSize.mV[VY], mClipboardSize.mV[VZ]);
    // <FS> Extended copy & paste buttons
    //LLView::getWindow()->copyTextToClipboard(utf8str_to_wstring(stringVec));
    copy_vector_to_clipboard(mClipboardSize);

    LLStringUtil::format_map_t args;
    args["VALUE"] = stringVec;
    mBtnPasteSize->setToolTip(getString("paste_size", args));

    mBtnPasteSize->setEnabled(true);
    // </FS>

    mHasClipboardSize = true;
}

void LLPanelObject::onCopyRot()
{
    mClipboardRot = LLVector3(mCtrlRotX->get(), mCtrlRotY->get(), mCtrlRotZ->get());

    std::string stringVec = llformat("<%g, %g, %g>", mClipboardRot.mV[VX], mClipboardRot.mV[VY], mClipboardRot.mV[VZ]);
    // <FS> Extended copy & paste buttons
    //LLView::getWindow()->copyTextToClipboard(utf8str_to_wstring(stringVec));
    copy_vector_to_clipboard(mClipboardRot);

    LLStringUtil::format_map_t args;
    args["VALUE"] = stringVec;
    mBtnPasteRot->setToolTip(getString("paste_rotation", args));

    mBtnPasteRot->setEnabled(true);
    // </FS>

    mHasClipboardRot = true;
}

void LLPanelObject::onPastePos()
{
    if (!mHasClipboardPos) return;
    if (mObject.isNull()) return;

    LLViewerRegion* regionp = mObject->getRegion();
    if (!regionp) return;

    // Clamp pos on non-attachments, just keep the prims within the region
    if (!mObject->isAttachment())
    {
        F32 max_width = regionp->getWidth(); // meters
        mClipboardPos.mV[VX] = llclamp(mClipboardPos.mV[VX], 0.f, max_width);
        mClipboardPos.mV[VY] = llclamp(mClipboardPos.mV[VY], 0.f, max_width);
        //height will get properly clamped by sendPosition
    }
    else
    {
        mClipboardPos.clampLength(MAX_ATTACHMENT_DIST);
    }

    mCtrlPosX->set( mClipboardPos.mV[VX] );
    mCtrlPosY->set( mClipboardPos.mV[VY] );
    mCtrlPosZ->set( mClipboardPos.mV[VZ] );

    sendPosition(false);
}

void LLPanelObject::onPasteSize()
{
    if (!mHasClipboardSize) return;

    // <AW: opensim-limits>
    //mClipboardSize.mV[VX] = llclamp(mClipboardSize.mV[VX], MIN_PRIM_SCALE, DEFAULT_MAX_PRIM_SCALE);
    //mClipboardSize.mV[VY] = llclamp(mClipboardSize.mV[VY], MIN_PRIM_SCALE, DEFAULT_MAX_PRIM_SCALE);
    //mClipboardSize.mV[VZ] = llclamp(mClipboardSize.mV[VZ], MIN_PRIM_SCALE, DEFAULT_MAX_PRIM_SCALE);
    mClipboardSize.mV[VX] = llclamp(mClipboardSize.mV[VX], mMinScale, llpanelobject_max_prim_scale());
    mClipboardSize.mV[VY] = llclamp(mClipboardSize.mV[VY], mMinScale, llpanelobject_max_prim_scale());
    mClipboardSize.mV[VZ] = llclamp(mClipboardSize.mV[VZ], mMinScale, llpanelobject_max_prim_scale());
    // </AW: opensim-limits>

    mCtrlScaleX->set(mClipboardSize.mV[VX]);
    mCtrlScaleY->set(mClipboardSize.mV[VY]);
    mCtrlScaleZ->set(mClipboardSize.mV[VZ]);

    sendScale(false);
}

void LLPanelObject::onPasteRot()
{
    if (!mHasClipboardRot) return;

    mCtrlRotX->set(mClipboardRot.mV[VX]);
    mCtrlRotY->set(mClipboardRot.mV[VY]);
    mCtrlRotZ->set(mClipboardRot.mV[VZ]);

    sendRotation(false);
}

void LLPanelObject::onCopyParams()
{
    LLViewerObject* objectp = mObject;
    if (!objectp || objectp->isMesh())
    {
        return;
    }

    mClipboardParams.clear();

    // Parametrics
    LLVolumeParams params;
    getVolumeParams(params);
    mClipboardParams["volume_params"] = params.asLLSD();

    // Sculpted Prim
    if (objectp->getParameterEntryInUse(LLNetworkData::PARAMS_SCULPT))
    {
        LLSculptParams *sculpt_params = (LLSculptParams *)objectp->getParameterEntry(LLNetworkData::PARAMS_SCULPT);

        LLUUID texture_id = sculpt_params->getSculptTexture();
        if (get_can_copy_texture(texture_id))
        {
            LL_DEBUGS("FloaterTools") << "Recording texture" << LL_ENDL;
            mClipboardParams["sculpt"]["id"] = texture_id;
        }
        else
        {
            mClipboardParams["sculpt"]["id"] = SCULPT_DEFAULT_TEXTURE;
        }

        mClipboardParams["sculpt"]["type"] = sculpt_params->getSculptType();
    }

    // <FS> Extended copy & paste buttons
    mBtnPasteParams->setEnabled(!mClipboardParams.emptyMap() && (mClipboardParams["volume_params"] || mClipboardParams["sculpt"]));
}

void LLPanelObject::onPasteParams()
{
    LLViewerObject* objectp = mObject;
    if (!objectp)
    {
        return;
    }

    // Sculpted Prim
    if (mClipboardParams.has("sculpt"))
    {
        LLSculptParams sculpt_params;
        LLUUID sculpt_id = mClipboardParams["sculpt"]["id"].asUUID();
        U8 sculpt_type = (U8)mClipboardParams["sculpt"]["type"].asInteger();
        sculpt_params.setSculptTexture(sculpt_id, sculpt_type);
        objectp->setParameterEntry(LLNetworkData::PARAMS_SCULPT, sculpt_params, true);
    }
    else
    {
        LLSculptParams *sculpt_params = (LLSculptParams *)objectp->getParameterEntry(LLNetworkData::PARAMS_SCULPT);
        if (sculpt_params)
        {
            objectp->setParameterEntryInUse(LLNetworkData::PARAMS_SCULPT, false, true);
        }
    }

    // volume params
    // make sure updateVolume() won't affect flexible
    if (mClipboardParams.has("volume_params"))
    {
        LLVolumeParams params;
        params.fromLLSD(mClipboardParams["volume_params"]);
        LLVOVolume *volobjp = (LLVOVolume *)objectp;
        if (volobjp->isFlexible())
        {
            if (params.getPathParams().getCurveType() == LL_PCODE_PATH_LINE)
            {
                params.getPathParams().setCurveType(LL_PCODE_PATH_FLEXIBLE);
            }
        }
        else if (params.getPathParams().getCurveType() == LL_PCODE_PATH_FLEXIBLE)
        {
            params.getPathParams().setCurveType(LL_PCODE_PATH_LINE);
        }

        objectp->updateVolume(params);
    }
}

// <FS> Extended copy & paste buttons
bool get_vector_from_clipboard(LLVector3* value)
{
    LLWString temp_string;
    LLView::getWindow()->pasteTextFromClipboard(temp_string);
    const std::string stringVec = wstring_to_utf8str(temp_string);

    if (stringVec.empty() || !value)
    {
        return false;
    }

    LLVector3 vec;
    S32 count = sscanf(stringVec.c_str(), "<%f, %f, %f>", vec.mV + 0, vec.mV + 1, vec.mV + 2);
    if (count == 3)
    {
        value->setVec(vec);
        return true;
    }

    return false;
}

void LLPanelObject::onPastePosClip()
{
    if (get_vector_from_clipboard(&mClipboardPos))
    {
        mHasClipboardPos = true;
        onPastePos();
    }
    else
    {
        LL_INFOS("FloaterTools") << "Couldn't get position vector from clipboard" << LL_ENDL;
    }
}

void LLPanelObject::onPasteSizeClip()
{
    if (get_vector_from_clipboard(&mClipboardSize))
    {
        mHasClipboardSize = true;
        onPasteSize();
    }
    else
    {
        LL_INFOS("FloaterTools") << "Couldn't get size vector from clipboard" << LL_ENDL;
    }
}

void LLPanelObject::onPasteRotClip()
{
    if (get_vector_from_clipboard(&mClipboardRot))
    {
        mHasClipboardRot = true;
        onPasteRot();
    }
    else
    {
        LL_INFOS("FloaterTools") << "Couldn't get rotation vector from clipboard" << LL_ENDL;
    }
}
// </FS>
