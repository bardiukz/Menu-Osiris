#pragma once

#include "../Memory.h"
#include "../Interfaces.h"
#include "../SDK/FrameStage.h"
#include "../SDK/UserCmd.h"
#include "../SDK/WeaponId.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/Entity.h"
#include "../SDK/Client.h"
#include "../SDK/GameEvent.h"
#include "../SDK/GlobalVars.h"
#include "../ButtonFlags.h"
#include "../Hitsound.h"
#include <windows.h>
#pragma comment(lib, "Winmm.lib") 
#include <stdio.h>

#include<iostream>
#include<mmsystem.h>
#include<stdlib.h>

namespace Misc {
    void inverseRagdollGravity() noexcept;
    void animateClanTag(const char* = nullptr) noexcept;
    void spectatorList() noexcept;
    void sniperCrosshair() noexcept;
    void recoilCrosshair() noexcept;
    void watermark() noexcept;
    void prepareRevolver(UserCmd*) noexcept;
	const char* _soundFileName = "Hitsound\\Hitsound.wav"; //make an folder in csgo called Hitsound and add your hitsound.wav into it.

    constexpr void fixAnimationLOD(FrameStage stage) noexcept
    {
        if (config.misc.fixAnimationLOD && stage == FrameStage::RENDER_START) {
            for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
                if (i == interfaces.engine->getLocalPlayer()) continue;
                Entity* entity = interfaces.entityList->getEntity(i);
                if (!entity || entity->isDormant() || !entity->isAlive()) continue;
                *reinterpret_cast<int*>(entity + 0xA28) = 0;
                *reinterpret_cast<int*>(entity + 0xA30) = memory.globalVars->framecount;
            }
        }
    }

    constexpr void autoPistol(UserCmd* cmd) noexcept
    {
        if (config.misc.autoPistol) {
            const auto activeWeapon = interfaces.entityList->getEntityFromHandle(
                interfaces.entityList->getEntity(
                    interfaces.engine->getLocalPlayer())->getProperty<int>("m_hActiveWeapon"));
            if (activeWeapon && activeWeapon->isPistol() && activeWeapon->getProperty<float>("m_flNextPrimaryAttack") > memory.globalVars->serverTime()) {
                if (activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex") == WeaponId::Revolver)
                    cmd->buttons &= ~UserCmd::IN_ATTACK2;
                else
                    cmd->buttons &= ~UserCmd::IN_ATTACK;
            }
        }
    }

    constexpr void chokePackets(bool& sendPacket) noexcept
    {
        sendPacket = interfaces.engine->getNetworkChannel()->chokedPackets >= config.misc.chokedPackets;
    }

    constexpr void autoReload(UserCmd* cmd) noexcept
    {
        if (config.misc.autoReload) {
            const auto activeWeapon = interfaces.entityList->getEntityFromHandle(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getProperty<int>("m_hActiveWeapon"));
            if (activeWeapon && getWeaponIndex(activeWeapon->getProperty<WeaponId>("m_iItemDefinitionIndex")) && !activeWeapon->getProperty<int>("m_iClip1"))
                cmd->buttons &= ~(UserCmd::IN_ATTACK | UserCmd::IN_ATTACK2);
        }
    }

    constexpr void revealRanks(UserCmd* cmd) noexcept
    {
        if (config.misc.revealRanks && cmd->buttons & UserCmd::IN_SCORE)
            interfaces.client->dispatchUserMessage(50, 0, 0, nullptr);
    }

    constexpr void autoStrafe(UserCmd* cmd) noexcept
    {
        if (auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            config.misc.autoStrafe
            && !(localPlayer->getProperty<int>("m_fFlags") & 1)
            && localPlayer->getProperty<MoveType>("m_nRenderMode", 1) != MoveType::NOCLIP) {
            if (cmd->mousedx < 0)
                cmd->sidemove = -450.0f;
            else if (cmd->mousedx > 0)
                cmd->sidemove = 450.0f;
        }
    }

	constexpr void SlideWalk(UserCmd* cmd) noexcept {
		if (config.misc.SlideWalk) {
			if (cmd->forwardmove > 0.0f)
				(cmd->buttons |= button_flags::button_in_back) && (cmd->buttons &= ~button_flags::button_in_forward);

			if (cmd->forwardmove < 0.0f)
				(cmd->buttons |= button_flags::button_in_forward) && (cmd->buttons &= ~button_flags::button_in_back);

			if (cmd->sidemove > 0.0f)
				(cmd->buttons |= button_flags::button_in_move_left) && (cmd->buttons &= ~button_flags::button_in_move_right);

			if (cmd->sidemove < 0.0f)
				(cmd->buttons |= button_flags::button_in_move_right) && (cmd->buttons &= ~button_flags::button_in_move_left);
		}
	}

    constexpr void bunnyHop(UserCmd* cmd) noexcept
    {
        if (auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
            config.misc.bunnyHop
            && !(localPlayer->getProperty<int>("m_fFlags") & 1)
            && localPlayer->getProperty<MoveType>("m_nRenderMode", 1) != MoveType::LADDER) {
            cmd->buttons &= ~UserCmd::IN_JUMP;
        }
    }

    constexpr void removeCrouchCooldown(UserCmd* cmd) noexcept
    {
        if (config.misc.fastDuck)
            cmd->buttons |= UserCmd::IN_BULLRUSH;
    }

    constexpr void setClanTag(const char* clanTag) noexcept
    {
        memory.setClanTag(clanTag, clanTag);
    }

    constexpr void playHitSound(GameEvent* event) noexcept
    {
		if (config.misc.hitSound
			&& !strcmp(event->getName(), "player_hurt")
			&& interfaces.engine->getPlayerForUserID(event->getInt("attacker")) == interfaces.engine->getLocalPlayer()) {

			switch (config.misc.iHitSound)
			{
			case 0: break;
			case 1: PlaySoundA(_soundFileName, NULL, SND_ASYNC); break; //custom one
			case 2: PlaySoundA(pew, NULL, SND_ASYNC | SND_MEMORY); break;
			}
		}
		
    }
}
