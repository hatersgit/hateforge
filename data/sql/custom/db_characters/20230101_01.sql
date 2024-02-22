<<<<<<< HEAD
drop table if exists acore_characters.character_accountwide_reputation;
=======
DROP TABLE IF EXiSTS `character_accountwide_reputation`;
>>>>>>> master
CREATE TABLE `acore_characters`.`character_accountwide_reputation` (
  `accountId` INT UNSIGNED NOT NULL,
  `factionGroup` INT UNSIGNED NOT NULL,
  `factionId` INT UNSIGNED NOT NULL,
  `rep` INT UNSIGNED NULL,
  PRIMARY KEY (`accountId`, `factionGroup`, `factionId`));

<<<<<<< HEAD
drop table if exists acore_characters.character_accountwide_taxi;
=======
DROP TABLE IF EXiSTS `character_accountwide_taxi`;
>>>>>>> master
CREATE TABLE `acore_characters`.`character_accountwide_taxi` (
  `accountId` INT UNSIGNED NOT NULL,
  `faction` INT UNSIGNED NOT NULL,
  `node` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`accountId`, `faction`, `node`));

<<<<<<< HEAD
drop table if exists acore_characters.character_accountwide_title;
=======
DROP TABLE IF EXiSTS `character_accountwide_title`;
>>>>>>> master
CREATE TABLE `acore_characters`.`character_accountwide_title` (
  `accountId` INT UNSIGNED NOT NULL,
  `title` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`accountId`, `title`));

<<<<<<< HEAD
drop table if exists acore_characters.character_accountwide_mount;
=======
DROP TABLE IF EXiSTS `character_accountwide_mount`;
>>>>>>> master
CREATE TABLE `acore_characters`.`character_accountwide_mount` (
  `accountId` INT UNSIGNED NOT NULL,
  `spellId` INT UNSIGNED NOT NULL,
  PRIMARY KEY (`accountId`, `spellId`));
