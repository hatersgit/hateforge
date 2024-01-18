CREATE TABLE IF NOT EXISTS `acore_characters`.`forge_character_talent_loadouts` (
  `guid` int unsigned NOT NULL,
  `id` int unsigned NOT NULL,
  `talentTabId` int unsigned NOT NULL,
  `name` varchar(45) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `talentString` varchar(100) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `active` tinyint NOT NULL,
  PRIMARY KEY (`guid`,`id`,`talentTabId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;