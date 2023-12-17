DELETE FROM acore_world.spell_proc_event where entry = 1020038;
INSERT INTO acore_world.spell_proc_event (entry, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, procFlags, procEx, procPhase, ppmRate, CustomChance, Cooldown) VALUES(1020038, 0, 0, 0, 0, 0, 0, 2, 4, 0.0, 100.0, 1000);

delete from acore_world.spell_script_names where spell_id in (1020064, 1020092);
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(1020064, 'spell_warr_relentless_strikes');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(1020092, 'aura_warr_relentless_strikes');
