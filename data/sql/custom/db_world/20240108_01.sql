drop table if exists `acore_world`.`forge_item_slot_value`;
CREATE table `acore_world`.`forge_item_slot_value` (
  `id` int unsigned NOT NULL,
  `value` int unsigned not null,
  `name` varchar(45) COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

drop table if exists `acore_world`.`forge_item_stat_value`;
CREATE table `acore_world`.`forge_item_stat_value` (
  `id` int unsigned NOT NULL,
  `value` int unsigned not null,
  `name` varchar(45) COLLATE utf8mb4_general_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;