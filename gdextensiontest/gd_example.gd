extends Node

var miniaudio = GDMiniaudio.new()

var loaded_audio : MiniaudioSound = MiniaudioSound.new()

@onready var sound = $sound

func _ready():
	miniaudio.initialize_engine()
	miniaudio.start_engine()
#	play_wav("C:/Users/zodie/OneDrive/Documents/0assets/sunvox/exports/chill ghost zone very long.wav")
	get_window().files_dropped.connect(func(files):
		for file in files:
			if file.ends_with('.wav'):
				loaded_audio = MiniaudioSound.new()
				miniaudio.load_audio_file(file, loaded_audio)
				miniaudio.play_sound(loaded_audio)
			elif file.ends_with('.mp3'):
				loaded_audio = MiniaudioSound.new()
				miniaudio.load_audio_file(file, loaded_audio)
				miniaudio.play_sound(loaded_audio)
			elif file.ends_with('.flac'):
				loaded_audio = MiniaudioSound.new()
				miniaudio.load_audio_file(file, loaded_audio)
				miniaudio.play_sound(loaded_audio)
		)

func _process(delta):
	miniaudio.set_sound_position(loaded_audio, sin(Time.get_ticks_msec()/1000.0)*4.0, sin(Time.get_ticks_msec()/1000.0)*4.0, cos(Time.get_ticks_msec()/1000.0)*4.0)
	var sound_pos := miniaudio.get_sound_position(loaded_audio)
	sound.position = sound_pos
