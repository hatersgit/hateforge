drop table if exists `acore_world`.`forge_character_spec_strarter_talents`;
CREATE TABLE `acore_world`.`forge_character_spec_strarter_talents` (
	`class` INT(10) UNSIGNED NOT NULL,
	`tab` INT(10) UNSIGNED NOT NULL,
	`spell` INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY (`class`,`tab`,`spell`)
) COLLATE='utf8_general_ci' ENGINE=InnoDB;

drop table if exists `acore_world`.`forge_character_spec_spells`;
CREATE TABLE `acore_world`.`forge_character_spec_spells` (
	`class` INT(10) UNSIGNED NOT NULL,
	`tab` INT(10) UNSIGNED NOT NULL,
	`level` INT(10) UNSIGNED NOT NULL,
	`spell` INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY (`class`,`tab`,`spell`)
) COLLATE='utf8_general_ci' ENGINE=InnoDB;