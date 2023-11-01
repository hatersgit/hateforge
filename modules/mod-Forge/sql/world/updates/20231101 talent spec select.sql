DROP TABLE IF EXISTS `acore_world`.`forge_character_spec_spells`;
CREATE TABLE `acore_world`.`forge_character_spec_spells` (
	`classId` INT(10) UNSIGNED NOT NULL,
	`tabId` INT(10) UNSIGNED NOT NULL,
	`level` INT(10) UNSIGNED NOT NULL,
	`spell` INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY (`tabId`, `level`)
) COLLATE='utf8_general_ci' ENGINE=InnoDB;