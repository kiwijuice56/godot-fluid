
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

    float viscosity = 0.3;

    // Numerical density arrays in 9 directions
    std::vector<float> n0;
    std::vector<float> nN;
    std::vector<float> nNE;
    std::vector<float> nE;
    std::vector<float> nSE;
    std::vector<float> nS;
    std::vector<float> nSW;
    std::vector<float> nW;
    std::vector<float> nNW;

    Ref<Image> render_image;

protected:
	static void _bind_methods();

    void initialize();
    void step();
    bool in_bounds(int x, int y);
    int get_index(int x, int y);
    void set_size(int width, int height);
    Ref<ImageTexture> get_render_texture();

public:
	Simulation();
	~Simulation();


};
}