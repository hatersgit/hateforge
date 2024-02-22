drop table if exists acore_characters.worldtier_unlocks_account;
create table acore_characters.worldtier_unlocks_account (
	`account` bigint NOT NULL,
	`unlocked` int NOT NULL DEFAULT 1,
  	PRIMARY KEY (`account`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

drop table if exists acore_characters.worldtier_unlocks_char;
create table acore_characters.worldtier_unlocks_char (
	`char` bigint NOT NULL,
	`unlocked` int NOT NULL DEFAULT 1,
  	PRIMARY KEY (`char`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;