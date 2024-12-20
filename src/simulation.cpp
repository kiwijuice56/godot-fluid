#include "simulation.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void Simulation::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize"), &Simulation::initialize);
    ClassDB::bind_method(D_METHOD("step"), &Simulation::step);
    ClassDB::bind_method(D_METHOD("set_size", "width", "height"), &Simulation::set_size);
    ClassDB::bind_method(D_METHOD("get_render_texture"), &Simulation::get_render_texture);
}

Simulation::Simulation() {

}

Simulation::~Simulation() {

}

void Simulation::initialize() {
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

    bn0.resize(width * height);
    bnN.resize(width * height);
    bnNE.resize(width * height);
    bnE.resize(width * height);
    bnSE.resize(width * height);
    bnS.resize(width * height);
    bnSW.resize(width * height);
    bnW.resize(width * height);
    bnNW.resize(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);
            n0[i] =  UtilityFunctions::randf();
            nN[i] =  UtilityFunctions::randf();
            nNE[i] = UtilityFunctions::randf();
            nE[i] =  UtilityFunctions::randf();
            nSE[i] = UtilityFunctions::randf();
            nS[i] =  UtilityFunctions::randf();
            nSW[i] = UtilityFunctions::randf();
            nW[i] =  UtilityFunctions::randf();
            nNW[i] = UtilityFunctions::randf();
        }
    }
}

void Simulation::step() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);
            // Compute macroscopic density and velocity
            float density = n0[i] + nN[i] + nNE[i] + nE[i] + nSE[i] + nS[i] + nSW[i] + nW[i] + nNW[i];

            render_image->set_pixel(x, y, Color(0.0, 0.0, density / 32.0));

            float vel_y = (nN[i] - nS[i] + nNE[i] - nSE[i] + nNW[i] - nSW[i]) / density;
            float vel_x = (nE[i] - nW[i] + nNE[i] - nNW[i] + nSE[i] - nSW[i]) / density;
            float vel_sq = vel_x * vel_x + vel_y * vel_y;

            // Compute equilibrium density
            float n0eq = density * (4./9.) *   (1. - (3./2.) * vel_sq);
            float nNeq = density * (1./9.) *   (1. + 3. * (0. * vel_x + 1. * vel_y) + (9./2.) * (0. * vel_x + 1. * vel_y) * (0. * vel_x + 1. * vel_y) - (3./2.) * vel_sq);
            float nNEeq = density * (1./36.) * (1. + 3. * (1. * vel_x + 1. * vel_y) + (9./2.) * (1. * vel_x + 1. * vel_y) * (1. * vel_x + 1. * vel_y) - (3./2.) * vel_sq);
            float nEeq = density * (1./9.) *   (1. + 3. * (1. * vel_x + 0. * vel_y) + (9./2.) * (1. * vel_x + 0. * vel_y) * (1. * vel_x + 0. * vel_y) - (3./2.) * vel_sq);
            float nSEeq = density * (1./36.) * (1. + 3. * (1. * vel_x - 1. * vel_y) + (9./2.) * (1. * vel_x - 1. * vel_y) * (1. * vel_x - 1. * vel_y) - (3./2.) * vel_sq);
            float nSeq = density * (1./9.) *   (1. + 3. * (0. * vel_x - 1. * vel_y) + (9./2.) * (0. * vel_x - 1. * vel_y) * (0. * vel_x - 1. * vel_y) - (3./2.) * vel_sq);
            float nSWeq = density * (1./36.) * (1. + 3. * (-1. * vel_x - 1. * vel_y) + (9./2.) * (-1. * vel_x - 1. * vel_y) * (-1. * vel_x - 1. * vel_y) - (3./2.) * vel_sq);
            float nWeq = density * (1./9.) *   (1. + 3. * (-1. * vel_x + 0. * vel_y) + (9./2.) * (-1. * vel_x + 0. * vel_y) * (-1. * vel_x + 0. * vel_y) - (3./2.) * vel_sq);
            float nNWeq = density * (1./36.) * (1. + 3. * (-1. * vel_x + 1. * vel_y) + (9./2.) * (-1. * vel_x + 1. * vel_y) * (-1. * vel_x + 1. * vel_y) - (3./2.) * vel_sq);

            // Update number densities
            bn0[i] = n0[i] + viscosity * (n0eq - n0[i]);
            bnN[i] = nN[i] + viscosity * (nNeq - nN[i]);
            bnNE[i] = nNE[i] + viscosity * (nNEeq - nNE[i]);
            bnE[i] = nE[i] + viscosity * (nEeq - nE[i]);
            bnSE[i] = nSE[i] + viscosity * (nSEeq - nSE[i]);
            bnS[i] = nS[i] + viscosity * (nSeq - nS[i]);
            bnSW[i] = nSW[i] + viscosity * (nSWeq - nSW[i]);
            bnW[i] = nW[i] + viscosity * (nWeq - nW[i]);
            bnNW[i] = nNW[i] + viscosity * (nNWeq - nNW[i]);

            n0[i] = 0;
            nN[i] = 0;
            nNE[i] = 0;
            nE[i] = 0;
            nSE[i] = 0;
            nS[i] = 0;
            nSW[i] = 0;
            nW[i] = 0;
            nNW[i] = 0;
        }
    }

    // Streaming
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);
            n0[i] = bn0[i];
            if (in_bounds(x, y - 1)) {
                nN[get_index(x, y - 1)] += bnN[i];
            }
            if (in_bounds(x + 1, y - 1)) {
                nNE[get_index(x + 1, y - 1)] += bnNE[i];
            }
            if (in_bounds(x + 1, y)) {
                nE[get_index(x + 1, y)] += bnE[i];
            }
            if (in_bounds(x + 1, y + 1)) {
                nSE[get_index(x + 1, y + 1)] += bnSE[i];
            }
            if (in_bounds(x, y + 1)) {
                nS[get_index(x, y + 1)] += bnS[i];
            }
            if (in_bounds(x - 1, y + 1)) {
                nSW[get_index(x - 1, y + 1)] += bnSW[i];
            }
            if (in_bounds(x - 1, y)) {
                nW[get_index(x - 1, y)] += bnW[i];
            }
            if (in_bounds(x - 1, y - 1)) {
                nNW[get_index(x - 1, y - 1)] += bnNW[i];
            }
        }
    }
}


bool Simulation::in_bounds(int x, int y) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

int Simulation::get_index(int x, int y) {
    return y * width + x;
}

void Simulation::set_size(int width, int height) {
    this->width = width;
    this->height = height;

    initialize();
}

Ref<ImageTexture> Simulation::get_render_texture() {
    return ImageTexture::create_from_image(render_image);
}