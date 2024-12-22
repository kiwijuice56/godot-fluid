class_name Main extends Node

var simulation: Simulation

var start = false

func _ready() -> void:
	simulation = Simulation.new()
	simulation.set_size(350, 350)
	simulation.initialize()

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_accept"):
		start = true
	if Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		var mouse_position: Vector2 = get_viewport().get_mouse_position()
		simulation.pulse(mouse_position.x, mouse_position.y, 64.0, .5)

func _process(delta: float) -> void:
	if start:
		simulation.step()
	%FluidCanvas.texture = simulation.get_render_texture()
