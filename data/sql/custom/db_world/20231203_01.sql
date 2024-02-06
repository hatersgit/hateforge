CREATE TABLE IF NOT EXISTS `acore_world`.`forge_spell_jump_charge_params` (
  `id` INT UNSIGNED NOT NULL,
  `speed` FLOAT NOT NULL DEFAULT '42',
  `treatSpeedAsMoveTimeSeconds` TINYINT(1) NOT NULL DEFAULT '0',
  `jumpGravity` FLOAT NOT NULL DEFAULT '19.2911',
  `comments` VARCHAR(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
