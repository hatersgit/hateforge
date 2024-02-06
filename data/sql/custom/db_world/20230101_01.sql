CREATE TABLE IF NOT EXISTS `acore_world`.`forge_talents` (
  `spellid` MEDIUMINT UNSIGNED NOT NULL,
  `talentTabId` INT UNSIGNED NOT NULL,
  `columnIndex` INT UNSIGNED NOT NULL,
  `rowIndex` INT UNSIGNED NOT NULL,
  `rankCost` TINYINT UNSIGNED NOT NULL,
  `minLevel` TINYINT UNSIGNED NOT NULL,
  `talentType` TINYINT UNSIGNED NOT NULL,
  `numberRanks` TINYINT UNSIGNED NOT NULL,
  `preReqType` TINYINT UNSIGNED NOT NULL,
  `tabPointReq` INT NOT null default 5,
  `nodeType` TINYINT NOT null default 0,
  `nodeIndex` INT NOT null,
  PRIMARY KEY (`spellid`, `talentTabId`),
  UNIQUE  KEY `UniqueKey` (`talentTabId`, `columnIndex`, `rowIndex`));

CREATE TABLE IF NOT EXISTS `acore_world`.`forge_talent_ranks` (
  `talentSpellId` MEDIUMINT UNSIGNED NOT NULL,
  `talentTabId` INT UNSIGNED NOT NULL,
  `rank` INT UNSIGNED NOT NULL,
  `spellId` MEDIUMINT UNSIGNED NULL,
  PRIMARY KEY (`talentSpellId`, `talentTabId`, `rank`));

CREATE TABLE IF NOT EXISTS `acore_world`.`forge_talent_tabs` (
  `id` INT UNSIGNED NOT NULL,
  `classMask` INT UNSIGNED NOT NULL,
  `raceMask` INT UNSIGNED NOT NULL,
  `name` VARCHAR(100) NOT NULL,
  `spellIcon` MEDIUMINT UNSIGNED NOT NULL,
  `background` TEXT NOT NULL,
  `description` VARCHAR(1000),
  `role` tinyint(1) default 0,
  `spellString` varchar(255),
  `tabType` INT UNSIGNED NOT NULL,
  `TabIndex` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`id`));

CREATE TABLE IF NOT EXISTS `acore_world`.`forge_talent_prereq` (
  `reqId` INT UNSIGNED NOT NULL,
  `spellid` MEDIUMINT UNSIGNED NOT NULL,
  `talentTabId` INT UNSIGNED NOT NULL,
  `reqTalent` MEDIUMINT UNSIGNED NOT NULL,
  `reqTalentTabId` INT UNSIGNED NOT NULL,
  `reqRank` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`reqId`, `spellid`, `talentTabId`));


CREATE TABLE IF NOT EXISTS `acore_world`.`forge_talent_exclusive` (
  `talentSpellId` MEDIUMINT UNSIGNED NOT NULL,
  `talentTabId` INT UNSIGNED NOT NULL,
  `exlusiveSpellId` MEDIUMINT UNSIGNED NOT NULL,
  PRIMARY KEY (`talentSpellId`, `talentTabId`, `exlusiveSpellId`));


 CREATE TABLE IF NOT EXISTS `acore_world`.`forge_talent_unlearn` (
  `talentTabId` INT UNSIGNED NOT NULL,
  `talentSpellId` MEDIUMINT UNSIGNED NOT NULL,
  `unlearnSpell` MEDIUMINT UNSIGNED NOT NULL,
  PRIMARY KEY (`talentTabId`, `talentSpellId`, `unlearnSpell`));

CREATE TABLE IF NOT EXISTS `acore_world`.`forge_config` (
  `cfgName` VARCHAR(80) NOT NULL,
  `cfgValue` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`cfgName`));

CREATE TABLE IF NOT EXISTS `acore_world`.`forge_prestige_ignored_spells` (
  `spellid` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`spellid`));

CREATE TABLE IF NOT EXISTS `acore_world`.`forge_player_spell_scale` (
    `ID` INT NOT NULL DEFAULT '0',
    `Data` FLOAT NOT NULL DEFAULT '0',
    PRIMARY KEY (`ID`)) ENGINE=MyISAM
    DEFAULT CHARSET=utf8; 

INSERT IGNORE INTO `acore_world`.`forge_config` (`cfgName`, `cfgValue`) VALUES ('scrapsPerLevelMod', 1);
INSERT IGNORE INTO `acore_world`.`forge_config` (`cfgName`, `cfgValue`) VALUES ('levelMod', 2);
INSERT IGNORE INTO `acore_world`.`forge_config` (`cfgName`, `cfgValue`) VALUES ('level10ForgePoints', 30);

DELETE FROM acore_world.playercreateinfo_skills WHERE comment LIKE '%Racial%';

INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (1,0.193); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (2,0.216); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (3,0.264); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (4,0.31); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (5,0.31); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (6,0.395); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (7,0.436); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (8,0.475); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (9,0.514); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (10,0.552); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (11,0.588); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (12,0.625); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (13,0.661); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (14,0.696); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (15,0.766); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (16,0.8); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (17,0.835); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (18,0.885); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (19,0.919); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (20,1); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (21,1.034); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (22,1.067); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (23,1.101); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (24,1.165); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (25,1.229); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (26,1.278); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (27,1.328); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (28,1.405); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (29,1.522); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (30,1.612); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (31,1.662); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (32,1.752); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (33,1.805); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (34,1.858); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (35,1.964); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (36,2.032); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (37,2.126); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (38,2.196); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (39,2.292); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (40,2.351); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (41,2.446); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (42,2.506); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (43,2.626); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (44,2.686); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (45,2.782); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (46,2.854); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (47,2.95); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (48,3.012); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (49,3.074); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (50,3.195); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (51,3.269); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (52,3.378); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (53,3.475); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (54,3.583); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (55,3.658); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (56,3.788); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (57,3.863); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (58,3.972); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (59,4.048); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (60,4.167); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (61,4.266); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (62,4.4); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (63,4.514); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (64,4.662); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (65,4.768); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (66,4.908); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (67,5.016); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (68,5.169); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (69,5.292); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (70,5.437); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (71,5.593); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (72,5.709); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (73,5.858); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (74,5.998); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (75,6.15); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (76,6.282); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (77,6.415); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (78,6.594); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (79,6.762); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (80,6.899); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (81,7.082); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (82,7.222); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (83,7.376); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (84,7.552); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (85,7.697); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (86,7.876); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (87,8.024); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (88,8.196); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (89,8.347); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (90,8.533); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (91,8.741); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (92,8.898); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (93,9.055); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (94,9.215); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (95,9.408); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (96,9.572); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (97,9.736); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (98,9.902); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (99,10.091); 
INSERT IGNORE INTO `acore_world`.`forge_player_spell_scale` VALUES (100,10.293); 
