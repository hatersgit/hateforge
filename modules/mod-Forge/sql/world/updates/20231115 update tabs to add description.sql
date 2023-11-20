Alter table `acore_world`.`forge_talent_tabs` add `description` VARCHAR(1000) after `background`;
Alter table `acore_world`.`forge_talent_tabs` add `role` tinyint(1) default 0 after `description`;
Alter table `acore_world`.`forge_talent_tabs` add `spellString` varchar(255) after `role`;