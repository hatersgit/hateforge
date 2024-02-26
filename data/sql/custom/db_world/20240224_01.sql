-- acore_world.playercreateinfo definition
DROP TABLE IF EXISTS `forge_starting_zone`;
CREATE TABLE `forge_starting_zone` (
  `race` tinyint unsigned NOT NULL DEFAULT '0',
  `map` smallint unsigned NOT NULL DEFAULT '0',
  `position_x` float NOT NULL DEFAULT '0',
  `position_y` float NOT NULL DEFAULT '0',
  `position_z` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`race`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

DELETE FROM `forge_starting_zone`;
INSERT INTO `forge_starting_zone` (`race`, `map`, `position_x`, `position_y`, `position_z`, `orientation`) VALUES
	(1, 0, -8949.95, -132.493, 83.5312, 0),
	(2, 1, -618.518, -4251.67, 38.718, 0),
	(3, 0, -6240.32, 331.033, 382.758, 6.17716),
	(4, 1, 10311.3, 832.463, 1326.41, 5.69632),
	(5, 0, 1676.71, 1678.31, 121.67, 2.70526),
	(6, 1, -2917.58, -257.98, 52.9968, 0),
	(7, 1, -6240.32, 331.033, 382.758, 0),
	(8, 1, -618.518, -4251.67, 38.718, 0),
	(10, 530, 10349.6, -6357.29, 33.4026, 5.31605),
	(11, 530, -3961.64, -13931.2, 100.615, 2.08364);