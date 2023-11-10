drop table if exists `acore_world`.`forge_spell_charge`;
CREATE TABLE `acore_world`.`forge_spell_charge` (
	`spell` INT(10) UNSIGNED NOT NULL,
	`timer` INT(10) UNSIGNED NOT NULL,
	`item` INT(10) UNSIGNED NOT NULL,
	`max` INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY (`spell`)
) COLLATE='utf8_general_ci' ENGINE=InnoDB;