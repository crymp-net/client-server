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
			translated_text = "2 minuty do ukončení mise.",
		},
		english = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		french = {
			translated_text = "2 minutes avant la fin de la mission.",
		},
		german = {
			translated_text = "2 Minuten bis zum Abbruch der Mission.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		hungarian = {
			translated_text = "Az akció lefújásáig 2 perc van hátra.",
		},
		italian = {
			translated_text = "2 minuti al termine della missione.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		japanese = {
			translated_text = "ミッション終了まで、あと2分",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		polish = {
			translated_text = "Dwie minuty do przerwania misji.",
		},
		russian = {
			translated_text = "2 минуты до окончания операции.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		spanish = {
			translated_text = "2 minutos para la finalización de la misión.",
		},
		turkish = {
			translated_text = "Görev bitimine 2 dakika kaldı.",
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
			translated_text = "1 minuta do ukončení mise.",
		},
		english = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		french = {
			translated_text = "1 minute avant la fin de la mission.",
		},
		german = {
			translated_text = "1 Minute bis zum Abbruch der Mission.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		hungarian = {
			translated_text = "Az akció lefújásáig 1 perc van hátra.",
		},
		italian = {
			translated_text = "1 minuto al termine della missione.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		japanese = {
			translated_text = "ミッション終了まで、あと1分",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		polish = {
			translated_text = "Jedna minuta do przerwania misji.",
		},
		russian = {
			translated_text = "Минута до окончания операции.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		spanish = {
			translated_text = "1 minuto para la finalización de la misión.",
		},
		turkish = {
			translated_text = "Görev bitimine 1 dakika kaldı.",
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
			translated_text = "30 vteřin do ukončení mise.",
		},
		english = {
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		french = {
			translated_text = "30 secondes avant la fin de la mission.",
		},
		german = {
			translated_text = "30 Sekunden bis zum Abbruch der Mission.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		hungarian = {
			translated_text = "Az akció lefújásáig 30 másodperc van hátra.",
		},
		italian = {
			translated_text = "30 secondi al termine della missione.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		japanese = {
			translated_text = "ミッション終了まで、あと30秒",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.8,
			sound_radio_squelch = 0.6,
		},
		polish = {
			translated_text = "Trzydzieści sekund do przerwania misji.",
		},
		russian = {
			translated_text = "30 секунд.",
			sound_volume = 0.4,
			sound_radio_ratio = 0.0,
			sound_radio_background = 0.2,
			sound_radio_squelch = 0.6,
		},
		spanish = {
			translated_text = "30 segundos para la finalización de la misión.",
		},
		turkish = {
			translated_text = "Görev bitimine 30 saniye kaldı.",
		},
	},
})

CPPAPI.AddLocalizedLabel("mp_american/us_commander_endgame_countdown_01", {
	english_text = "5, 4, 3, 2, 1",
	translated_text = "5, 4, 3, 2, 1",
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

CPPAPI.AddLocalizedLabel("Detonator", {
	english_text = "Detonator",
	languages = {
		czech = {
			translated_text = "DETONÁTOR",
		},
		french = {
			translated_text = "DÉTONATEUR",
		},
		german = {
			translated_text = "AUSLÖSER",
		},
		hungarian = {
			translated_text = "DETONÁTOR",
		},
		italian = {
			translated_text = "DETONATORE",
		},
		japanese = {
			translated_text = "雷管",
		},
		polish = {
			translated_text = "DETONATOR",
		},
		russian = {
			translated_text = "ДЕТОНАТОР",
		},
		spanish = {
			translated_text = "DETONADOR",
		},
		turkish = {
			translated_text = "DETONATÖR",
		},
	},
})