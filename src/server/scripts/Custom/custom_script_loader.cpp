/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// This is where scripts' loading functions should be declared:
// void MyExampleScript()
void AddSC_spell_custom_mana_scaling_damage();
void AddSC_spell_custom_mana_scaling_heal();
void AddSC_spell_custom_chemist_spike();
void AddSC_spell_custom_cleric_heal();
void AddSC_spell_custom_pirate_impale();
void AddSC_spell_custom_priest_photon();
void AddSC_spell_custom_trickster_coup();
void AddSC_spell_custom_warlock_void();
void AddSC_spell_custom_monk_rising_kick_knockup();
void AddSC_spell_custom_templar_bonus_holy();

// The name of this function should match:
// void Add${NameOfDirectory}Scripts()
void AddCustomScripts()
{
    // MyExampleScript()
	AddSC_spell_custom_mana_scaling_damage();
	AddSC_spell_custom_mana_scaling_heal();
	AddSC_spell_custom_chemist_spike();
	AddSC_spell_custom_cleric_heal();
	AddSC_spell_custom_pirate_impale();
	AddSC_spell_custom_priest_photon();
	AddSC_spell_custom_trickster_coup();
	AddSC_spell_custom_warlock_void();
	AddSC_spell_custom_monk_rising_kick_knockup();
	AddSC_spell_custom_templar_bonus_holy();
}
