CREATE TABLE IF NOT EXISTS `mod_tbc_phase_status` (
  `phase` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `phase_date_one` varchar(10) NOT NULL DEFAULT '미정',
  `phase_date_two` varchar(10) NOT NULL DEFAULT '미정',
  `phase_date_three` varchar(10) NOT NULL DEFAULT '미정',
  `phase_date_four` varchar(10) NOT NULL DEFAULT '미정',
  `phase_date_five` varchar(10) NOT NULL DEFAULT '미정',
  PRIMARY KEY (`phase`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

INSERT IGNORE INTO `mod_tbc_phase_status` (`phase`, `phase_date_one`, `phase_date_two`, `phase_date_three`, `phase_date_four`, `phase_date_five`) VALUES (1, '미정', '미정', '미정', '미정', '미정');

CREATE TABLE IF NOT EXISTS `mod_tbc_npc_phasemask_backup` (
`guid` INT UNSIGNED NOT NULL PRIMARY KEY,
`original_phasemask` INT UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=UTF8MB4;
