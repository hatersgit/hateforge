drop table if exists `acore_world`.`forge_character_spec_spells`;
CREATE TABLE `acore_world`.`forge_character_spec_spells` (
	`class` INT(10) UNSIGNED NOT NULL,
	`tab` INT(10) UNSIGNED NOT NULL,
	`level` INT(10) UNSIGNED NOT NULL,
	`spell` INT(10) UNSIGNED NOT NULL,
	PRIMARY KEY (`class`,`tab`,`spell`)
) COLLATE='utf8_general_ci' ENGINE=InnoDB;

INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 834, 10, 1);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 834, 10, 3);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 834, 10, 4);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 835, 10, 1);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 835, 10, 3);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 835, 10, 4);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 836, 10, 1);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 836, 10, 3);
INSERT INTO acore_world.forge_character_spec_spells (class, tab, `level`, spell) VALUES(6, 836, 10, 4);
