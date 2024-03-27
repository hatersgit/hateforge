drop table if exists `crops`;
CREATE TABLE `crops` (
  `id` int NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT null,
  `group_one` int NOT null,
  `group_two` int NOT NULL,
  `model` int not null,
  `reward` int not null,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=latin1 ROW_FORMAT=DYNAMIC;