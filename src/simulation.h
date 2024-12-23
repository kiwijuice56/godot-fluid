
#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/gradient_texture1_d.hpp>

namespace godot {

class Simulation : public Node {
	GDCLASS(Simulation, Node);

private:
    Vector2i size = Vector2i(350, 256);
    int width = 256;
    int height = 256;

    float viscosity = 0.4;

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

    // Whether a cell contains a solid wall
    std::vector<bool> wall;

    Ref<Image> render_image;

    // Render settings
    Ref<GradientTexture1D> palette;
    float visual_density_cap = 8.0;

protected:
	static void _bind_methods();

    // Main interface methods
    void initialize();
    void step();
    void pulse(int pulse_x, int pulse_y, int pulse_radius, float strength);

    // Simulation helper methods
    inline bool in_bounds(int x, int y);
    inline int get_index(int x, int y);

    // Boilerplate getters/setters
    void set_size(Vector2i size);
    void set_palette(Ref<GradientTexture1D> palette);
    void set_visual_density_cap(float val);

    Vector2i get_size();
    Ref<GradientTexture1D> get_palette();
    float get_visual_density_cap();
    Ref<ImageTexture> get_render_texture();

public:
	Simulation();
	~Simulation();
};
}