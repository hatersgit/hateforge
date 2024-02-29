drop table if exists `forge_raid_rotation`;
CREATE TABLE `forge_raid_rotation` (
  `ID` int NOT NULL AUTO_INCREMENT,
  `raid1_loc` int NOT null,
  `raid2_loc` int NOT NULL,
  `raid3_loc` int NOT NULL,
  `timeOfStart` int unsigned NOT NULL,
  `active` tinyint(1) NOT null default 0,
  `sequence` int not null DEFAULT 1,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;

INSERT INTO acore_world.forge_raid_rotation (ID, raid1_loc, raid2_loc, raid3_loc, timeOfStart, active, sequence) VALUES(1, 531, 548, 615, 0, 0, 1);
