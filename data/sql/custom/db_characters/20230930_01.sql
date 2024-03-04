-- acore_characters.character_spell_charges definition
DROP TABLE IF EXISTS `character_spell_charges`;
CREATE TABLE `character_spell_charges` (
  `guid` int unsigned NOT NULL DEFAULT '0' COMMENT 'Global Unique Identifier, Low part',
  `classMask0` int unsigned NOT NULL DEFAULT '0',
  `classMask1` int unsigned NOT NULL DEFAULT '0',
  `classMask2` int unsigned NOT NULL DEFAULT '0',
  `maxCharges` int unsigned NOT NULL DEFAULT '0',
  `currentCharges` int unsigned NOT NULL DEFAULT '0',
  `maxDuration` int unsigned NOT NULL DEFAULT '0',
  `currentDuration` int unsigned NOT NULL DEFAULT '0',
  `chargeAura` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`guid`,`classMask0`,`classMask1`,`classMask2`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;