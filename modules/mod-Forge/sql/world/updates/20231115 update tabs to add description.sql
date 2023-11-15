Alter table `acore_world`.`forge_talent_tabs` add `description` VARCHAR(255) after `background`;
Alter table `acore_world`.`forge_talent_tabs` add `role` tinyint(1) default 0 after `description`;