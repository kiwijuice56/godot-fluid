#include "simulation.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Simulation::_bind_methods() {
    ClassDB::bind_method(D_METHOD("step"), &Simulation::step);
    ClassDB::bind_method(D_METHOD("set_size", "width", "height"), &Simulation::set_size);
    ClassDB::bind_method(D_METHOD("get_render_texture"), &Simulation::get_render_texture);
}

Simulation::Simulation() {
    render_image = Image::create_empty(width, height, false, Image::FORMAT_RGBA8);
}

Simulation::~Simulation() {

}

void Simulation::step() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            render_image->set_pixel(x, y, Color(0.5, 0.8, 0.1));
        }
    }
}

void Simulation::set_size(int width, int height) {
    this->width = width;
    this->height = height;
    render_image = Image::create_empty(width, height, false, Image::FORMAT_RGBA8);
}

Ref<ImageTexture> Simulation::get_render_texture() {
    return ImageTexture::create_from_image(render_image);
}