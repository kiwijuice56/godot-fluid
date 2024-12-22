class_name Main extends Node

var simulation: Simulation

func _ready() -> void:
	simulation = Simulation.new()
	simulation.set_size(350, 350)
	simulation.initialize()

func _process(delta: float) -> void:
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		var mouse_position: Vector2 = get_viewport().get_mouse_position()
		simulation.pulse(mouse_position.x, mouse_position.y, 16, 1.5)
	simulation.step()
	%FluidCanvas.texture = simulation.get_render_texture()
