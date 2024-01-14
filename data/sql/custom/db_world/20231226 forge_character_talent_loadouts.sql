CREATE TABLE if not exists `acore_characters`.`forge_character_talent_loadouts` (
  `guid` int unsigned NOT NULL,
  `name` varchar(45) COLLATE utf8mb4_general_ci NOT NULL,
  `talentString` varchar(45) COLLATE utf8mb4_general_ci NOT NULL,
  `active` tinyint NOT NULL,
  PRIMARY KEY (`guid`,`name`,`talentString`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;