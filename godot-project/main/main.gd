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

func _process(delta: float) -> void:
	if start:
		simulation.step()
	%FluidCanvas.texture = simulation.get_render_texture()
