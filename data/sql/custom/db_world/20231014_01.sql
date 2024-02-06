CREATE TABLE IF NOT EXISTS `acore_characters`.`forge_character_node_choices` (
  `guid` INT UNSIGNED NOT NULL,
  `spec` INT UNSIGNED NOT NULL,
  `tabId` INT UNSIGNED NOT NULL,
  `node` TINYINT UNSIGNED NOT NULL,
  `choice` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`, `spec`, `tabId`, `node`));
  
CREATE TABLE IF NOT EXISTS `acore_world`.`forge_talent_choice_nodes` (
  `choiceNodeId` MEDIUMINT UNSIGNED NOT NULL,
  `talentTabId` INT UNSIGNED NOT NULL,
  `choiceIndex` int unsigned not null,
  `choiceSpellId` MEDIUMINT UNSIGNED NOT NULL,
  PRIMARY KEY (`choiceNodeId`, `talentTabId`, `choiceSpellId`));

INSERT IGNORE INTO acore_world.forge_talent_choice_nodes (choiceNodeId, talentTabId, choiceIndex, choiceSpellId) VALUES(1020011, 1, 1, 1020011);
INSERT IGNORE INTO acore_world.forge_talent_choice_nodes (choiceNodeId, talentTabId, choiceIndex, choiceSpellId) VALUES(1020011, 1, 2, 1020065);
INSERT IGNORE INTO acore_world.forge_talent_choice_nodes (choiceNodeId, talentTabId, choiceIndex, choiceSpellId) VALUES(1020052, 1, 1, 1020052);
INSERT IGNORE INTO acore_world.forge_talent_choice_nodes (choiceNodeId, talentTabId, choiceIndex, choiceSpellId) VALUES(1020052, 1, 2, 1020054);
