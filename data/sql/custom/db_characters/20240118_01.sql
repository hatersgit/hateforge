DROP TABLE IF EXISTS `active_shards`;
CREATE TABLE `active_shards` (
  `char` int NOT NULL,
  `shard1` int NOT null default 0,
  `shard2` int NOT null default 0,
  `shard3` int NOT null default 0,
  `shard4` int NOT null default 0,
  `shard5` int NOT null default 0,
  PRIMARY KEY (`char`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

DROP TABLE IF EXISTS `character_worldtier`;
CREATE TABLE `character_worldtier` (
  `char` int NOT NULL,
  `tier` int NOT NULL,
  PRIMARY KEY (`char`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;