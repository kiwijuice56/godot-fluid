class_name Main extends Node

var simulation: Simulation

func _ready() -> void:
	simulation = Simulation.new()
	simulation.step()
	%FluidCanvas.texture = simulation.get_render_texture()
