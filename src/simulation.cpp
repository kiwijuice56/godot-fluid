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
    wall.resize(width * height);
    total_vel_x.resize(width * height);
    total_vel_y.resize(width * height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int i = get_index(x, y);
            n0[i] = 1.0;
            nN[i] =  y / float(height);
            nNE[i] = y / float(height);
            nE[i] = 1. / 9.;
            nSE[i] = 1. / 2.;
            nS[i] = 0;
            nSW[i] = 1. / 36.;
            nW[i] = 1. / 9.;
            nNW[i] = 1. / 36.;

            float r_x = 300 - x;
            float r_y = 300 - y;
            float r_x2 = 240 - x;
            float r_y2 = 128 - y;
            if (y <= 3 || x <= 3 || y >= height - 4 || x >= width - 4 || ((y/8) % 2 == 0 && x > 32 && x < 90 && y > 100 && y < 300) || (r_x * r_x + r_y * r_y < 700) || (r_x2 * r_x2 + r_y2 * r_y2 < 900)) {
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

            // Compute macroscopic density and velocity
            float density = n0[i] + nN[i] + nNE[i] + nE[i] + nSE[i] + nS[i] + nSW[i] + nW[i] + nNW[i];

            float vel_y = (nN[i] - nS[i] + nNE[i] - nSE[i] + nNW[i] - nSW[i]) / density;
            float vel_x = (nE[i] - nW[i] + nNE[i] - nNW[i] + nSE[i] - nSW[i]) / density;
            if (density < 0.001) {
                vel_x = 0;
                vel_y = 0;
            }

            total_vel_x[i] = vel_x;
            total_vel_y[i] = vel_y;

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

            if (wall[i] || n0[i] < 0)  n0[i] = 0;
            if (wall[i] || nN[i] < 0)  nN[i] = 0;
            if (wall[i] || nNE[i] < 0) nNE[i] = 0;
            if (wall[i] || nE[i] < 0)  nE[i] = 0;
            if (wall[i] || nSE[i] < 0) nSE[i] = 0;
            if (wall[i] || nS[i] < 0)  nS[i] = 0;
            if (wall[i] || nSW[i] < 0) nSW[i] = 0;
            if (wall[i] || nW[i] < 0)  nW[i] = 0;
            if (wall[i] || nNW[i] < 0) nNW[i] = 0;

            if (wall[i]) {
                total_vel_x[i] = 0;
                total_vel_y[i] = 0;
            }
        }
    }

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
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int i = get_index(x, y);
            float curl = (total_vel_y[get_index(x + 1, y)] - total_vel_y[get_index(x - 1, y)]) - (total_vel_x[get_index(x, y + 1) ] - total_vel_x[get_index(x, y - 1)]);
            float density = n0[i] + nN[i] + nNE[i] + nE[i] + nSE[i] + nS[i] + nSW[i] + nW[i] + nNW[i];
            render_image->set_pixel(x, y, Color(density / 16.0, density / 8.0, density / 4.0));
        }
    }
}


inline bool Simulation::in_bounds(int x, int y) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

inline int Simulation::get_index(int x, int y) {
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