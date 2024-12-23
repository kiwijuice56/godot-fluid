#include "simulation.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Simulation::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &Simulation::initialize);
    ClassDB::bind_method(D_METHOD("step"), &Simulation::step);
    ClassDB::bind_method(D_METHOD("pulse"), &Simulation::pulse);

    ClassDB::bind_method(D_METHOD("set_size", "size"), &Simulation::set_size);
    ClassDB::bind_method(D_METHOD("set_palette", "palette"), &Simulation::set_palette);
    ClassDB::bind_method(D_METHOD("set_visual_density_cap", "val"), &Simulation::set_visual_density_cap);

    ClassDB::bind_method(D_METHOD("get_size"), &Simulation::get_size);
    ClassDB::bind_method(D_METHOD("get_palette"), &Simulation::get_palette);
    ClassDB::bind_method(D_METHOD("get_visual_density_cap"), &Simulation::get_visual_density_cap);
    ClassDB::bind_method(D_METHOD("get_render_texture"), &Simulation::get_render_texture);

    ADD_PROPERTY(
        PropertyInfo(Variant::VECTOR2I, "size"),
        "set_size",
        "get_size"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::FLOAT, "visual_density_cap"),
        "set_visual_density_cap",
        "get_visual_density_cap"
    );

    ADD_PROPERTY(
        PropertyInfo(
            Variant::OBJECT,
            "palette",
            PROPERTY_HINT_RESOURCE_TYPE,
            "GradientTexture1D"
        ),
        "set_palette",
        "get_palette"
    );
}

Simulation::Simulation() {

}

Simulation::~Simulation() {

}

void Simulation::initialize() {
    width = size.x;
    height = size.y;

    render_image = Image::create_empty(width, height, false, Image::FORMAT_RGBA8);

    n0.resize(width * height);
    nN.resize(width * height);
    nNE.resize(width * height);
    nE.resize(width * height);
    nSE.resize(width * height);
    nS.resize(width * height);
    nSW.resize(width * height);
    nW.resize(width * height);
    nNW.resize(width * height);
    wall.resize(width * height);

    // Setup initial state
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);
            n0[i] = 1.0;
            nN[i] =  1. / 9;
            nNE[i] = 1. / 36;
            nE[i] = 1. / 9.;
            nSE[i] = 1. / 2.;
            nS[i] = 1. / 9.;
            nSW[i] = 1. / 36.;
            nW[i] = 1. / 9.;
            nNW[i] = 1. / 36.;

            float r_x = 64 - x;
            float r_y = 64 - y;
            float r_x2 = 240 - x;
            float r_y2 = 128 - y;

            // Setup walls
            if (y <= 3 || x <= 3 || y >= height - 4 || x >= width - 4 || (r_x * r_x + r_y * r_y < 700) || (r_x2 * r_x2 + r_y2 * r_y2 < 900)) {
                wall[i] = true;
            }
        }
    }
}

void Simulation::step() {
    // Collision
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);

            if (wall[i]) {
                n0[i] =  0.;
                nN[i] =  0.;
                nNE[i] = 0.;
                nE[i] =  0.;
                nSE[i] = 0.;
                nS[i] =  0.;
                nSW[i] = 0.;
                nW[i] =  0.;
                nNW[i] = 0.;

                continue;
            }

            // Compute macroscopic density and velocity
            float density = n0[i] + nN[i] + nNE[i] + nE[i] + nSE[i] + nS[i] + nSW[i] + nW[i] + nNW[i];

            float vel_y = (nN[i] - nS[i] + nNE[i] - nSE[i] + nNW[i] - nSW[i]) / density;
            float vel_x = (nE[i] - nW[i] + nNE[i] - nNW[i] + nSE[i] - nSW[i]) / density;

            float vel_sq = vel_x * vel_x + vel_y * vel_y;

            // Compute equilibrium density
            float n0eq = density * (4./9.) *   (1. - (3./2.) * vel_sq);
            float nNeq = density * (1./9.) *   (1. + 3. * vel_y + (9./2.) * (vel_y * vel_y) - (3./2.) * vel_sq);
            float nNEeq = density * (1./36.) * (1. + 3. * (vel_x + vel_y) + (9./2.) * (vel_x + vel_y) * (vel_x + vel_y) - (3./2.) * vel_sq);
            float nEeq = density * (1./9.) *   (1. + 3. * vel_x + (9./2.) * vel_x * vel_x - (3./2.) * vel_sq);
            float nSEeq = density * (1./36.) * (1. + 3. * (vel_x - vel_y) + (9./2.) * (vel_x - vel_y) * (vel_x - vel_y) - (3./2.) * vel_sq);
            float nSeq = density * (1./9.) *   (1. + 3. * -vel_y + (9./2.) * vel_y * vel_y - (3./2.) * vel_sq);
            float nSWeq = density * (1./36.) * (1. + 3. * (-vel_x - vel_y) + (9./2.) * (-vel_x - vel_y) * (-vel_x - vel_y) - (3./2.) * vel_sq);
            float nWeq = density * (1./9.) *   (1. + 3. * -vel_x + (9./2.) * vel_x * vel_x - (3./2.) * vel_sq);
            float nNWeq = density * (1./36.) * (1. + 3. * (vel_y - vel_x) + (9./2.) * (vel_y - vel_x) * (vel_y - vel_x) - (3./2.) * vel_sq);

            // Update number densities
            n0[i] = n0[i] + viscosity * (n0eq - n0[i]);
            nN[i] = nN[i] + viscosity * (nNeq - nN[i]);
            nNE[i] = nNE[i] + viscosity * (nNEeq - nNE[i]);
            nE[i] = nE[i] + viscosity * (nEeq - nE[i]);
            nSE[i] = nSE[i] + viscosity * (nSEeq - nSE[i]);
            nS[i] = nS[i] + viscosity * (nSeq - nS[i]);
            nSW[i] = nSW[i] + viscosity * (nSWeq - nSW[i]);
            nW[i] = nW[i] + viscosity * (nWeq - nW[i]);
            nNW[i] = nNW[i] + viscosity * (nNWeq - nNW[i]);

            if (n0[i] < 0)  n0[i] = 0;
            if (nN[i] < 0)  nN[i] = 0;
            if (nNE[i] < 0) nNE[i] = 0;
            if (nE[i] < 0)  nE[i] = 0;
            if (nSE[i] < 0) nSE[i] = 0;
            if (nS[i] < 0)  nS[i] = 0;
            if (nSW[i] < 0) nSW[i] = 0;
            if (nW[i] < 0)  nW[i] = 0;
            if (nNW[i] < 0) nNW[i] = 0;
        }
    }

    // Streaming

    // NW corner
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (in_bounds(x + 0, y + 1)) nN[get_index(x, y)] = nN[get_index(x + 0, y + 1)];
            if (in_bounds(x + 1, y + 1)) nNW[get_index(x, y)] = nNW[get_index(x + 1, y + 1)];
        }
    }
    // NE corner
    for (int y = 0; y < height; y++) {
        for (int x = width - 1; x >= 0; x--) {
            if (in_bounds(x - 1, y + 0)) nE[get_index(x, y)] = nE[get_index(x - 1, y + 0)];
            if (in_bounds(x - 1, y + 1)) nNE[get_index(x, y)] = nNE[get_index(x - 1, y + 1)];
        }
    }
    // SE corner
    for (int y = height - 1; y >= 0; y--) {
        for (int x = width - 1; x >= 0; x--) {
            if (in_bounds(x + 0, y - 1)) nS[get_index(x, y)] = nS[get_index(x + 0, y - 1)];
            if (in_bounds(x - 1, y - 1)) nSE[get_index(x, y)] = nSE[get_index(x - 1, y - 1)];
        }
    }
    // SW corner
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            if (in_bounds(x + 1, y + 0)) nW[get_index(x, y)] = nW[get_index(x + 1, y + 0)];
            if (in_bounds(x + 1, y - 1)) nSW[get_index(x, y)] = nSW[get_index(x + 1, y - 1)];
        }
    }

    // Wall bounces
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);
            if (!wall[i]) continue;
            if (in_bounds(x + 0, y - 1)) nN[get_index(x + 0, y - 1)] += nS[i];
            if (in_bounds(x + 1, y - 1)) nNE[get_index(x + 1, y - 1)] += nSW[i];
            if (in_bounds(x + 1, y + 0)) nE[get_index(x + 1, y + 0)] += nW[i];
            if (in_bounds(x + 1, y + 1)) nSE[get_index(x + 1, y + 1)] += nNW[i];
            if (in_bounds(x + 0, y + 1)) nS[get_index(x + 0, y + 1)] += nN[i];
            if (in_bounds(x - 1, y + 1)) nSW[get_index(x - 1, y + 1)] += nNE[i];
            if (in_bounds(x - 1, y + 0)) nW[get_index(x - 1, y + 0)] += nE[i];
            if (in_bounds(x - 1, y - 1)) nNW[get_index(x - 1, y - 1)] += nSE[i];
        }
    }

    // Rendering
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);
            float density = n0[i] + nN[i] + nNE[i] + nE[i] + nSE[i] + nS[i] + nSW[i] + nW[i] + nNW[i];
            render_image->set_pixel(x, y, Color(density / 16.0, density / 8.0, density / 4.0));
        }
    }
}

void Simulation::pulse(int pulse_x, int pulse_y, int pulse_radius, float strength) {
    if (!in_bounds(pulse_x, pulse_y)) return;

    for (int y = pulse_y - pulse_radius; y < pulse_y + pulse_radius; y++) {
        for (int x = pulse_x - pulse_radius; x < pulse_x + pulse_radius; x++) {
            if (!in_bounds(x, y)) continue;

            float x_d = (x - pulse_x);
            float y_d = (y - pulse_y);
            float radius = x_d * x_d + y_d * y_d;

            if (radius > pulse_radius * pulse_radius) continue;

            int i = get_index(x, y);

            float cell_strength = strength * (1. - radius / (pulse_radius * pulse_radius));

            n0[i] -= cell_strength * (4. / 9.);
            nN[i] += cell_strength * (1. / 9.);
            nS[i] += cell_strength * (1. / 9.);
            nE[i] += cell_strength * (1. / 9.);
            nW[i] += cell_strength * (1. / 9.);
            nNE[i] += cell_strength * (1. / 36.);
            nSE[i] += cell_strength * (1. / 36.);
            nNW[i] += cell_strength * (1. / 36.);
            nSW[i] += cell_strength * (1. / 36.);
        }
    }
}


inline bool Simulation::in_bounds(int x, int y) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

inline int Simulation::get_index(int x, int y) {
    return y * width + x;
}

void Simulation::set_size(Vector2i size) {
    this->size = size;
}

void Simulation::set_palette(Ref<GradientTexture1D> palette) {
    this->palette = palette;
}

void Simulation::set_visual_density_cap(float val) {
    visual_density_cap = val;
}

Vector2i Simulation::get_size() {
    return size;
}

Ref<GradientTexture1D> Simulation::get_palette() {
    return palette;
}

float Simulation::get_visual_density_cap() {
    return visual_density_cap;
}


Ref<ImageTexture> Simulation::get_render_texture() {
    return ImageTexture::create_from_image(render_image);
}
