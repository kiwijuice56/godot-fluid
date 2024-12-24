class_name Main extends Node

func _ready() -> void:
	%Simulation.initialize()

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("reset"):
		%Simulation.initialize()

func _process(delta: float) -> void:
	var mouse_position: Vector2 = get_viewport().get_mouse_position()
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		%Simulation.pulse(mouse_position.x, mouse_position.y, 22, 1.15)
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		%Simulation.pulse(mouse_position.x, mouse_position.y, 22, -1.15)
	%Simulation.step()
	%FluidCanvas.texture = %Simulation.get_render_texture()
