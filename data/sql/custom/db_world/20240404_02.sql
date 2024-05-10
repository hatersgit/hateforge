DELETE FROM acore_world.spell_script_names where ScriptName in (
	'spell_pal_judgements_of_the_wise', 'spell_pal_judgement_of_light_heal', 'spell_pal_sacred_shield_dummy',
	'spell_warl_soul_leech', 'spell_hunter_intoxication', 'spell_snake_deadly_poison', 'spell_rog_killing_spree_SpellScript',
	'spell_rog_killing_spree_AuraScript', 'spell_rog_tricks_of_the_trade', 'spell_hun_chimera_shot',
	'spell_sha_totem_searing_bolt', 'spell_pri_atonement', 'spell_dru_elunes_fire', 'spell_dru_eclipse', 
	'spell_mage_glacial_advance', 'spell_dk_dancing_rune_weapon', 'spell_dk_pestilence'
);
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(-110092, 'spell_pal_judgements_of_the_wise');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(110096, 'spell_pal_judgement_of_light_heal');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100097, 'spell_pal_sacred_shield_dummy');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(-110178, 'spell_warl_soul_leech');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100056, 'spell_hunter_intoxication');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(34657, 'spell_snake_deadly_poison');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(3606, 'spell_sha_totem_searing_bolt');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(110247, 'spell_pri_atonement');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(110285, 'spell_dru_elunes_fire');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(-110282, 'spell_dru_eclipse');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100136, 'spell_mage_glacial_advance');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100068, 'spell_rog_killing_spree_SpellScript');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100068, 'spell_rog_killing_spree_AuraScript');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100075, 'spell_rog_tricks_of_the_trade');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100059, 'spell_hun_chimera_shot');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100038, 'spell_dk_dancing_rune_weapon');
INSERT INTO acore_world.spell_script_names (spell_id, ScriptName) VALUES(100034, 'spell_dk_pestilence');





DELETE FROM acore_world.spell_proc where SpellId in (
	-110107, -110130, -110184, -110188, -110256, -110271, -110272, -110282, 110285, -110335
);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110107, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110130, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110184, 0, 23, 17, 0, 0, 0, 0, 2, 2, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110188, 0, 23, 0, 0, 0, 0, 1, 2, 0, 0, 0.0, 0.0, 0, 1);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110256, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110271, 8, 0, 0, 0, 0, 0, 1, 2, 2, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110272, 0, 16, 0, 0, 0, 0, 1, 2, 2, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110282, 0, 16, 0, 0, 0, 0, 1, 2, 2, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(110285, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0.0, 0.0, 0, 0);
INSERT INTO acore_world.spell_proc (SpellId, SchoolMask, SpellFamilyName, SpellFamilyMask0, SpellFamilyMask1, SpellFamilyMask2, ProcFlags, SpellTypeMask, SpellPhaseMask, HitMask, AttributesMask, ProcsPerMinute, Chance, Cooldown, Charges) VALUES(-110335, 0, 0, 0, 0, 0, 0, 1, 0, 16, 0, 0.0, 0.0, 0, 0);





update acore_world.creature_template set `name` = 'Healing Ward' where entry = 34686;
delete from acore_world.creature_template_spell where CreatureID = 34686;
INSERT INTO acore_world.creature_template_spell (CreatureID, `Index`, Spell, VerifiedBuild) VALUES(34686, 0, 58765, 12340);





delete from acore_world.spell_ranks where first_spell_id in (
	110335, 110282, 110282, 110282, 110272, 110272, 110272, 110271, 110256, 110256, 110256, 110188, 
	110188, 110188, 110184, 110184, 110184, 110130, 110130, 110130, 110107, 110107, 110107, 110178
	);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110282, 110284, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110282, 110283, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110282, 110282, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110272, 110274, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110272, 110273, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110272, 110272, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110271, 110271, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110256, 110259, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110256, 110258, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110256, 110256, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110188, 110190, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110188, 110189, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110188, 110188, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110184, 110186, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110184, 110185, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110184, 110184, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110178, 110178, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110178, 110179, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110130, 110132, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110130, 110131, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110130, 110130, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110107, 110109, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110107, 110108, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110107, 110107, 1);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110335, 110337, 3);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110335, 110336, 2);
INSERT INTO acore_world.spell_ranks (first_spell_id, spell_id, `rank`) VALUES(110335, 110335, 1);
