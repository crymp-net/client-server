--------------------------------------------------------------------------------
-- Fix missing US commander mission end warning in PowerStruggle
--------------------------------------------------------------------------------

CPPAPI.AddLocalizedLabel("mp_american/us_commander_mission_2_minute_warning_01", {
	english_text = "2 minutes until mission termination.",
	sound_volume = 0.6,
	sound_event = "radio2d",
	sound_radio_ratio = 0.05,
	sound_radio_background = 0.1,
	sound_radio_squelch = 0.2,
	sound_ducking = 0.0,
	use_subtitle = false,
	languages = {
		czech = {
			localized_text = "2 minuty do ukončení mise.",
		},
		english = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		french = {
			localized_text = "2 minutes avant la fin de la mission.",
		},
		german = {
			localized_text = "2 Minuten bis zum Abbruch der Mission.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		hungarian = {
			localized_text = "Az akció lefújásáig 2 perc van hátra.",
		},
		italian = {
			localized_text = "2 minuti al termine della missione.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		japanese = {
			localized_text = "ミッション終了まで、あと2分",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		polish = {
			localized_text = "Dwie minuty do przerwania misji.",
		},
		russian = {
			localized_text = "2 минуты до окончания операции.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		spanish = {
			localized_text = "2 minutos para la finalización de la misión.",
		},
		turkish = {
			localized_text = "Görev bitimine 2 dakika kaldı.",
		},
	},
})

CPPAPI.AddLocalizedLabel("mp_american/us_commander_mission_1_minute_warning_02", {
	english_text = "1 minute until mission termination.",
	sound_volume = 0.6,
	sound_event = "radio2d",
	sound_radio_ratio = 0.05,
	sound_radio_background = 0.1,
	sound_radio_squelch = 0.2,
	sound_ducking = 0.0,
	use_subtitle = false,
	languages = {
		czech = {
			localized_text = "1 minuta do ukončení mise.",
		},
		english = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		french = {
			localized_text = "1 minute avant la fin de la mission.",
		},
		german = {
			localized_text = "1 Minute bis zum Abbruch der Mission.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		hungarian = {
			localized_text = "Az akció lefújásáig 1 perc van hátra.",
		},
		italian = {
			localized_text = "1 minuto al termine della missione.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		japanese = {
			localized_text = "ミッション終了まで、あと1分",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		polish = {
			localized_text = "Jedna minuta do przerwania misji.",
		},
		russian = {
			localized_text = "Минута до окончания операции.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		spanish = {
			localized_text = "1 minuto para la finalización de la misión.",
		},
		turkish = {
			localized_text = "Görev bitimine 1 dakika kaldı.",
		},
	},
})

CPPAPI.AddLocalizedLabel("mp_american/us_commander_mission_30_second_03", {
	english_text = "30 seconds until mission termination.",
	sound_volume = 0.6,
	sound_event = "radio2d",
	sound_radio_ratio = 0.05,
	sound_radio_background = 0.1,
	sound_radio_squelch = 0.2,
	sound_ducking = 0.0,
	use_subtitle = false,
	languages = {
		czech = {
			localized_text = "30 vteřin do ukončení mise.",
		},
		english = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		french = {
			localized_text = "30 secondes avant la fin de la mission.",
		},
		german = {
			localized_text = "30 Sekunden bis zum Abbruch der Mission.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		hungarian = {
			localized_text = "Az akció lefújásáig 30 másodperc van hátra.",
		},
		italian = {
			localized_text = "30 secondi al termine della missione.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		japanese = {
			localized_text = "ミッション終了まで、あと30秒",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		polish = {
			localized_text = "Trzydzieści sekund do przerwania misji.",
		},
		russian = {
			localized_text = "30 секунд.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		spanish = {
			localized_text = "30 segundos para la finalización de la misión.",
		},
		turkish = {
			localized_text = "Görev bitimine 30 saniye kaldı.",
		},
	},
})

CPPAPI.AddLocalizedLabel("mp_american/us_commander_endgame_countdown_01", {
	english_text = "5, 4, 3, 2, 1",
	localized_text = "5, 4, 3, 2, 1",
	sound_volume = 0.6,
	sound_event = "radio2d",
	sound_radio_ratio = 0.05,
	sound_radio_background = 0.1,
	sound_radio_squelch = 0.2,
	sound_ducking = 0.0,
	use_subtitle = false,
	languages = {
		english = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		german = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		italian = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		japanese = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		russian = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
	},
})

--------------------------------------------------------------------------------
-- Fix various missing stuff
--------------------------------------------------------------------------------

CPPAPI.AddLocalizedLabel("Detonator", {
	english_text = "DETONATOR",
	languages = {
		czech = {
			localized_text = "DETONÁTOR",
		},
		french = {
			localized_text = "DÉTONATEUR",
		},
		german = {
			localized_text = "AUSLÖSER",
		},
		hungarian = {
			localized_text = "DETONÁTOR",
		},
		italian = {
			localized_text = "DETONATORE",
		},
		japanese = {
			localized_text = "雷管",
		},
		polish = {
			localized_text = "DETONATOR",
		},
		russian = {
			localized_text = "ДЕТОНАТОР",
		},
		spanish = {
			localized_text = "DETONADOR",
		},
		turkish = {
			localized_text = "DETONATÖR",
		},
	},
})
