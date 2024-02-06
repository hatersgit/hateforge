drop table if exists `acore_world`.`forge_item_slot_value`;
CREATE TABLE `acore_world`.`forge_item_slot_value` (
  `id` int unsigned NOT NULL,
  `value` decimal(10,2) NOT NULL,
  `name` varchar(45) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(1, 1.00, 'Head');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(2, 0.56, 'Neck');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(3, 0.75, 'Shoulders');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(5, 1.00, 'Chest');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(6, 0.75, 'Waist');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(7, 1.00, 'Legs');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(8, 0.75, 'Feet');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(9, 0.56, 'Wrists');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(10, 0.75, 'Hands');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(11, 0.56, 'Finger');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(12, 0.68, 'Trinket');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(13, 0.42, '1-Hander');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(14, 0.56, 'Shield');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(15, 0.32, 'Ranged Weapon');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(16, 0.56, 'Cloak');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(17, 1.00, '2-Hander');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(20, 1.00, 'Robe');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(21, 0.42, 'Mainhand Weapon');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(22, 0.42, 'Offhand Weapon');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(23, 0.56, 'Held Offhand');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(25, 0.32, 'Thrown');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(26, 0.32, 'Wand');
INSERT INTO acore_world.forge_item_slot_value (id, value, name) VALUES(28, 0.30, 'Relic');


drop table if exists `acore_world`.`forge_item_stat_value`;
CREATE table `acore_world`.`forge_item_stat_value` (
  `id` int unsigned NOT NULL,
  `value` decimal(10,2) NOT NULL,
  `name` varchar(45) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(3, 1.00, 'Agility');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(7, 0.66, 'Stamina');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(4, 1.00, 'Strength');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(5, 1.00, 'Intellect');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(12, 1.00, 'Defense');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(13, 1.00, 'Dodge');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(14, 1.00, 'Parry');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(15, 1.00, 'Block');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(31, 1.00, 'Hit');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(32, 1.00, 'Crit');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(35, 0.07, 'Armor');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(36, 1.00, 'Haste');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(38, 0.50, 'AP');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(43, 2.50, 'MP5');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(45, 0.86, 'SP');
INSERT INTO acore_world.forge_item_stat_value (id, value, name) VALUES(48, 0.33, 'Block Value');


drop table if exists `acore_world`.`forge_item_stat_pool`;
CREATE TABLE `acore_world`.`forge_item_stat_pool` (
  `id` int unsigned NOT NULL,
  `stat` int unsigned NOT NULL,
  `name` varchar(45) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`,`stat`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(4, 31, 'STR DPS HIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(4, 36, 'STR DPS HASTE');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(4, 32, 'STR DPS CRIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(4, 38, 'STR DPS AP');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(3, 31, 'AGI DPS HIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(3, 36, 'AGI DPS HASTE');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(3, 32, 'AGI DPS CRIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(3, 38, 'AGI DPS AP');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(5, 31, 'INT DPS HIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(5, 36, 'INT DPS HASTE');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(5, 32, 'INT DPS CRIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(5, 45, 'INT DPS SP');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(5, 43, 'INT DPS MP5');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(10, 31, 'AGI TANK HIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(10, 36, 'AGI TANK HASTE');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(10, 32, 'AGI TANK CRIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(10, 38, 'AGI TANK AP');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(10, 13, 'AGI TANK DODGE');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(11, 13, 'STR TANK DODGE');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(11, 14, 'STR TANK PARRY');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(11, 15, 'STR TANK BLOCK');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(11, 12, 'STR TANK DEF');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(11, 31, 'STR TANK HIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(11, 32, 'STR TANK CRIT');
INSERT INTO acore_world.forge_item_stat_pool (id, stat, name) VALUES(11, 48, 'STR TANK BLOCK VALUE');
