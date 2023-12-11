drop table if exists acore_world.`forge_talent_spell_flagged_unlearn`;
CREATE TABLE acore_world.`forge_talent_spell_flagged_unlearn` (
  `guid` int unsigned NOT NULL,
  `spell` int unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

drop table if exists acore_world.`forge_talent_learn_additional_spell`;
CREATE TABLE acore_world.`forge_talent_learn_additional_spell` (
  `spell` int unsigned NOT NULL,
  `addedSpell` int unsigned NOT NULL,
  PRIMARY KEY (`spell`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;