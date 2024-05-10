DROP TABLE IF EXISTS acore_world.`perks`;
CREATE TABLE acore_world.`perks` (
  `id` int NOT NULL,
  `maxrank` int DEFAULT 1,
  `associatedClass` int DEFAULT -1,
  `isAura` tinyint DEFAULT NULL,
  `mutexgroup` int DEFAULT NULL,
  `tags` varchar(64) NOT NULL DEFAULT 'I need tags!',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;