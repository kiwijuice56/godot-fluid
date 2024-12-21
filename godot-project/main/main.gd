class_name Main extends Node

var simulation: Simulation

func _ready() -> void:
	simulation = Simulation.new()
	simulation.initialize()

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_accept"):
		simulation.step()
		simulation.step()
		simulation.step()

func _process(delta: float) -> void:
	simulation.step()
	%FluidCanvas.texture = simulation.get_render_texture()
