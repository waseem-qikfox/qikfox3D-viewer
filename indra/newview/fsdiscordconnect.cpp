/**
 * @file fsdiscordconnect.cpp
 * @brief Connection to Discord
 * @author liny@pinkfox.xyz
 *
 * $LicenseInfo:firstyear=2013&license=viewerlgpl$
 * Phoenix qikfox3D Viewer Source Code
 * Copyright (C) 2019 Liny Odell @ Second Life
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
 */

#include "llviewerprecompiledheaders.h"

#include "fsdiscordconnect.h"

#include "llagent.h"
#include "llcallingcard.h"          // for LLAvatarTracker
#include "llcommandhandler.h"
#include "llnotificationsutil.h"
#include "lltrans.h"
#include "llevents.h"
#include "llviewerregion.h"
#include "llavatarnamecache.h"
#include "llregioninfomodel.h"

#include "rlvactions.h"
#include "rlvhandler.h"

#include "llfloaterreg.h"

#include <discord-rpc/discord_rpc.h>
#include <boost/algorithm/string/case_conv.hpp>

#include "fsdiscordkey.h"

#include "llviewernetwork.h"

std::unique_ptr<LLEventPump> FSDiscordConnect::sStateWatcher = std::make_unique<LLEventStream>("DiscordConnectState");
std::unique_ptr<LLEventPump> FSDiscordConnect::sInfoWatcher = std::make_unique<LLEventStream>("DiscordConnectInfo");


// Returns false when the file exists and has not our UUID
// Or, put simply, returns true if someone else is using it
bool FSDiscordConnect::checkMarkerFile() const
{
    if (!LLFile::isfile(mMarkerFilename))
    {
        return false;
    }
    LLUUID fileID;
    llifstream file(mMarkerFilename);
    file >> fileID;
    if ((fileID == gAgentID) || fileID.isNull())
    {
        return false;
    }
    return true;
}

void FSDiscordConnect::setMarkerFile()
{
    if (!checkMarkerFile())
    {
        return; // dont over-write another instances file
    }
    llofstream file(mMarkerFilename.c_str());
    file << gAgentID << std::endl;
    file.close();
}

void FSDiscordConnect::clearMarkerFile()
{
    if (!checkMarkerFile())
    {
        return; // dont remove another instances file
    }
    LLFile::remove(mMarkerFilename);
}

static void handleDiscordReady(const DiscordUser *request)
{
    LLSD info;
    info["name"] = request->username;
    FSDiscordConnect::getInstance()->storeInfo(info);
    FSDiscordConnect::getInstance()->setConnectionState(FSDiscordConnect::DISCORD_CONNECTED);
}

static void handleDiscordError(int errorCode, const char* message)
{
    LL_WARNS("DiscordConnect") << "Discord error, errorCode: \"" << errorCode << "\", message: \"" << message << "\"" << LL_ENDL;
}

static void handleDiscordDisconnected(int errorCode, const char* message)
{
    LL_INFOS("DiscordConnect") << "Discord disconnected, errorCode: \"" << errorCode << "\", message: \"" << message << "\"" << LL_ENDL;
    FSDiscordConnect::getInstance()->setConnectionState(FSDiscordConnect::DISCORD_NOT_CONNECTED);
}

///////////////////////////////////////////////////////////////////////////////
//
void FSDiscordConnect::discordConnectCoro()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.errored = handleDiscordError;
    handlers.disconnected = handleDiscordDisconnected;
    //TODO: Get this working and sending TPs
    /*handlers.joinGame = handleDiscordJoinGame;
    handlers.spectateGame = handleDiscordSpectateGame;
    handlers.joinRequest = handleDiscordJoinRequest;*/

    Discord_Initialize(DISCORD_API_KEY, &handlers, 1, "");
}

///////////////////////////////////////////////////////////////////////////////
//
void FSDiscordConnect::discordDisconnectCoro()
{
    Discord_Shutdown();
    setConnectionState(FSDiscordConnect::DISCORD_NOT_CONNECTED);
}

///////////////////////////////////////////////////////////////////////////////
//
void FSDiscordConnect::discordConnectedCoro(bool autoConnect)
{
    if (autoConnect)
    {
        setConnectionState(FSDiscordConnect::DISCORD_CONNECTION_IN_PROGRESS);
        if (!checkMarkerFile())
        {
            connectToDiscord();
        }
        else
        {
            setConnectionState(FSDiscordConnect::DISCORD_CONNECTION_FAILED);
        }
    }

}

static bool isRegionVisible(LLViewerRegion* region)
{
    U8 rating = region->getSimAccess();
    bool visible = true;
    if (rating > gSavedPerAccountSettings.getU32("FSMaxSharedMaturity"))
    {
        visible = false;
    }
    else
    {
        std::string name = region->getName();
        LLSD list = gSavedPerAccountSettings.getLLSD("FSBlacklistedRegionNames");
        for (LLSD::array_const_iterator iter = list.beginArray();
            iter != list.endArray();
            iter++)
        {
            if (boost::algorithm::to_lower_copy((*iter).asString()) == boost::algorithm::to_lower_copy(name))
            {
                visible = false;
                break;
            }
        }
    }
    return visible;
}

void FSDiscordConnect::updateRichPresence() const
{
    LLViewerRegion* region = gAgent.getRegion();
    if (!isConnected() || !region)
    {
        return;
    }
    std::string region_name;
    if (RlvActions::canShowLocation() && isRegionVisible(region))
    {
        region_name = gAgent.getRegion()->getName();
        region_name += " ";
        LLVector3 pos = gAgent.getPositionAgent();

        region_name += llformat("(%.0f, %.0f, %.0f)", pos.mV[VX], pos.mV[VY], pos.mV[VZ]);
    }
    else
    {
        region_name = "Hidden Region";
    }

    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.state = region_name.c_str();

    std::string name;
    if (RlvActions::canShowName(RlvActions::SNC_DEFAULT, gAgentID) && gSavedPerAccountSettings.getBOOL("FSShareNameToDiscord"))
    {
        LLAvatarName av_name;
        if (LLAvatarNameCache::get(gAgentID, &av_name))
        {
            name = av_name.getCompleteName(true, true);
        }
        else
        {
            name = gAgentUsername;
        }
        discordPresence.details = name.c_str();
    }

    discordPresence.startTimestamp = mConnectTime;

#ifdef OPENSIM
    if (LLGridManager::getInstance()->isInSecondLife())
    {
        discordPresence.largeImageKey = "secondlife_512";
    }
    else
    {
        discordPresence.largeImageKey = "opensimulator_512";
    }
#else
    discordPresence.largeImageKey = "secondlife_512";
#endif

    auto gridLabel = LLGridManager::getInstance()->getGridLabel();
    discordPresence.largeImageText = gridLabel.c_str();
    discordPresence.smallImageKey = "qikfox3D_512";
    std::string appName = std::string("via " + APP_NAME);
    discordPresence.smallImageText = appName.c_str();

    std::string regionId = gAgent.getRegion()->getRegionID().asString();
    discordPresence.partyId = regionId.c_str();
    discordPresence.partySize = static_cast<int>(gAgent.getRegion()->mMapAvatars.size());
    discordPresence.partyMax = LLRegionInfoModel::instance().mAgentLimit;
    Discord_UpdatePresence(&discordPresence);
}

///////////////////////////////////////////////////////////////////////////////
//
FSDiscordConnect::FSDiscordConnect()
:   mConnectionState(DISCORD_NOT_CONNECTED),
    mConnected(false),
    mInfo(),
    mRefreshInfo(false),
    mConnectTime(0)
{
    mMarkerFilename = gDirUtilp->getExpandedFilename(LL_PATH_USER_SETTINGS, "discord_in_use_marker");
    LLEventPumps::instance().obtain("mainloop").listen("FSDiscordConnect", std::bind(&FSDiscordConnect::Tick, this, std::placeholders::_1));
}

FSDiscordConnect::~FSDiscordConnect()
{
    disconnectFromDiscord();
}

void FSDiscordConnect::connectToDiscord()
{
    LLCoros::instance().launch("FSDiscordConnect::discordConnectCoro",
        std::bind(&FSDiscordConnect::discordConnectCoro, this));
}

void FSDiscordConnect::disconnectFromDiscord()
{
    setConnectionState(FSDiscordConnect::DISCORD_DISCONNECTING);

    LLCoros::instance().launch("FSDiscordConnect::discordDisconnectCoro",
        std::bind(&FSDiscordConnect::discordDisconnectCoro, this));
}

void FSDiscordConnect::checkConnectionToDiscord(bool auto_connect)
{
    LLCoros::instance().launch("FSDiscordConnect::discordConnectedCoro",
        std::bind(&FSDiscordConnect::discordConnectedCoro, this, auto_connect));
}

bool FSDiscordConnect::Tick(const LLSD&)
{
    Discord_RunCallbacks();
    updateRichPresence();

    return false;
}

void FSDiscordConnect::storeInfo(const LLSD& info)
{
    mInfo = info;
    mRefreshInfo = false;

    sInfoWatcher->post(info);
}

const LLSD& FSDiscordConnect::getInfo() const
{
    return mInfo;
}

void FSDiscordConnect::clearInfo()
{
    mInfo = LLSD();
}

void FSDiscordConnect::setConnectionState(FSDiscordConnect::EConnectionState connection_state)
{
    if(connection_state == DISCORD_CONNECTED)
    {
        setMarkerFile();
        setConnected(true);
        mConnectTime = time_corrected();
    }
    else if(connection_state == DISCORD_NOT_CONNECTED)
    {
        clearMarkerFile();
        setConnected(false);
    }

    if (mConnectionState != connection_state)
    {
        // set the connection state before notifying watchers
        mConnectionState = connection_state;

        LLSD state_info;
        state_info["enum"] = connection_state;
        sStateWatcher->post(state_info);
    }
}

void FSDiscordConnect::setConnected(bool connected)
{
    mConnected = connected;
}
