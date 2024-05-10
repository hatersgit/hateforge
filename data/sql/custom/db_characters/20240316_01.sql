DROP TABLE IF EXISTS `encounter_loot_lockout`;
CREATE TABLE `encounter_loot_lockout` (
  `char` int unsigned NOT NULL DEFAULT '0',
  `map` smallint unsigned NOT NULL DEFAULT '0',
  `difficulty` tinyint unsigned NOT NULL DEFAULT '0',
  `encounters` int unsigned NOT NULL DEFAULT '0',
  `resettime` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`char`, `map`, `difficulty`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;



DELETE FROM acore_characters.forge_character_points where guid = 4294967295;
INSERT INTO acore_characters.forge_character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 8, 4294967295, 0, 21);
INSERT INTO acore_characters.forge_character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 7, 4294967295, 0, 25);
INSERT INTO acore_characters.forge_character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 6, 4294967295, 0, 0);
INSERT INTO acore_characters.forge_character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 4, 4294967295, 17, 17);
INSERT INTO acore_characters.forge_character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 3, 4294967295, 0, 20);
INSERT INTO acore_characters.forge_character_points (guid, `type`, spec, sum, max) VALUES(4294967295, 0, 4294967295, 1, 60);