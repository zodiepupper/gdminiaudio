extends SubViewport

@onready var node = $"../../../../.."

func _input(event):
	if event is InputEventMouse:
		node.cam_projection = get_camera_3d().project_position(event.position,8.0)
