CREATE TABLE IF NOT EXISTS `custom_druid_barbershop` (
  `guid` mediumint unsigned NOT NULL DEFAULT '0',
  `type` char(200) NOT NULL DEFAULT '',
  `name` char(200) NOT NULL DEFAULT '',
  `display` mediumint unsigned NOT NULL DEFAULT '0',
  `npc` mediumint unsigned NOT NULL DEFAULT '0',
  `racemask` mediumint unsigned NOT NULL DEFAULT '0',
  `SpellId` mediumint unsigned NOT NULL DEFAULT '0',
  `ReqSpellId` mediumint NOT NULL DEFAULT '0',
  `path` char(200) NOT NULL DEFAULT '',
  `ReqItemID` mediumint NOT NULL DEFAULT '0',
  `ReqItemCant` mediumint NOT NULL DEFAULT '0',
  `Comentario` char(200) NOT NULL DEFAULT '',
  PRIMARY KEY (`guid`,`display`,`SpellId`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;