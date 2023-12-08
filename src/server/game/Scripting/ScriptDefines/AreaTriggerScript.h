/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ScriptObject.h"

class AreaTriggerScript : public ScriptObject
{
protected:
    AreaTriggerScript(const char* name);

public:

    [[nodiscard]] bool IsDatabaseBound() const override { return true; }

    // Called when the area trigger is activated by a player.
    [[nodiscard]] virtual bool OnTrigger(Player* /*player*/, AreaTrigger const* /*trigger*/) { return false; }
};

class OnlyOnceAreaTriggerScript : public AreaTriggerScript
{
    using AreaTriggerScript::AreaTriggerScript;
public:
    bool OnTrigger(Player* player, AreaTrigger const* trigger) override { return _OnTrigger(player, trigger); };
    virtual bool _OnTrigger(Player* player, AreaTrigger const* trigger) { return false; }

protected:
    void ResetAreaTriggerDone(InstanceScript* /*instance*/, uint32 /*triggerId*/);
    void ResetAreaTriggerDone(Player const* /*player*/, AreaTrigger const* /*trigger*/);
};

