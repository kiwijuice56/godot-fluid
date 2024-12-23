class_name Main extends Node

func _ready() -> void:
	%Simulation.initialize()

func _process(delta: float) -> void:
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		var mouse_position: Vector2 = get_viewport().get_mouse_position()
		%Simulation.pulse(mouse_position.x, mouse_position.y, 24, 1.25)
	%Simulation.step()
	%FluidCanvas.texture = %Simulation.get_render_texture()
