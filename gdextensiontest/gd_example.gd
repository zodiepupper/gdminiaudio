extends Node

var miniaudio = GDMiniaudio.new()

func _ready():
	miniaudio.initialize_engine()
	miniaudio.start_engine()
#	play_wav("C:/Users/zodie/OneDrive/Documents/0assets/sunvox/exports/chill ghost zone very long.wav")
	get_window().files_dropped.connect(func(files):
		for file in files:
			if file.ends_with('.wav'):
				miniaudio.play_file(file)
			elif file.ends_with('.mp3'):
				miniaudio.play_file(file)
			elif file.ends_with('.flac'):
				miniaudio.play_file(file)
		)
