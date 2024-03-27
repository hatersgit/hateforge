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


-- acore_world.forge_item_slot_value definition
drop table if exists `gen_item_slots`;
CREATE TABLE `gen_item_slots` (
  `id` int unsigned NOT NULL,
  `slots` int NOT NULL,
  `name` varchar(45) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

INSERT INTO gen_item_slots (id, slots, name) VALUES(1,   3, 'Head');
INSERT INTO gen_item_slots (id, slots, name) VALUES(2,   2, 'Neck');
INSERT INTO gen_item_slots (id, slots, name) VALUES(3,   3, 'Shoulders');
INSERT INTO gen_item_slots (id, slots, name) VALUES(5,   3, 'Chest');
INSERT INTO gen_item_slots (id, slots, name) VALUES(6,   1, 'Waist');
INSERT INTO gen_item_slots (id, slots, name) VALUES(7,   3, 'Legs');
INSERT INTO gen_item_slots (id, slots, name) VALUES(8,   1, 'Feet');
INSERT INTO gen_item_slots (id, slots, name) VALUES(9,   2, 'Wrists');
INSERT INTO gen_item_slots (id, slots, name) VALUES(10,  2, 'Hands');
INSERT INTO gen_item_slots (id, slots, name) VALUES(11,  1, 'Finger');
INSERT INTO gen_item_slots (id, slots, name) VALUES(12,  1, 'Trinket');
INSERT INTO gen_item_slots (id, slots, name) VALUES(13,  1, '1-Hander');
INSERT INTO gen_item_slots (id, slots, name) VALUES(14,  1, 'Shield');
INSERT INTO gen_item_slots (id, slots, name) VALUES(15,  2, 'Ranged Weapon');
INSERT INTO gen_item_slots (id, slots, name) VALUES(16,  1, 'Cloak');
INSERT INTO gen_item_slots (id, slots, name) VALUES(17,  2, '2-Hander');
INSERT INTO gen_item_slots (id, slots, name) VALUES(20,  3, 'Robe');
INSERT INTO gen_item_slots (id, slots, name) VALUES(21,  1, 'Mainhand Weapon');
INSERT INTO gen_item_slots (id, slots, name) VALUES(22,  1, 'Offhand Weapon');
INSERT INTO gen_item_slots (id, slots, name) VALUES(23,  1, 'Held Offhand');
INSERT INTO gen_item_slots (id, slots, name) VALUES(25,  2, 'Thrown');
INSERT INTO gen_item_slots (id, slots, name) VALUES(26,  2, 'Wand');
INSERT INTO gen_item_slots (id, slots, name) VALUES(28,  2, 'Relic');
