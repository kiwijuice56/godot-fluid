
#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>

namespace godot {

class Simulation : public Node {
	GDCLASS(Simulation, Node);

private:
    int width = 256;
    int height = 256;

    float viscosity = 1.0;

    // Numerical density arrays in 9 directions
    float* n0;
    float* nN;
    float* nNE;
    float* nE;
    float* nSE;
    float* nS;
    float* nSW;
    float* nW;
    float* nNW;

    Ref<Image> render_image;

protected:
	static void _bind_methods();

public:
	Simulation();
	~Simulation();

    void step();
    void set_size(int width, int height);
    Ref<ImageTexture> get_render_texture();
};
}