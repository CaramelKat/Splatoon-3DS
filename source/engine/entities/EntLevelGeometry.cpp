//
// Created by ash on 11/12/22.
//

#include "EntLevelGeometry.h"
#include <algorithm>
#include <random>

EntLevelGeometry::EntLevelGeometry(Model &model, std::string name) :
    Entity(model, std::move(name)) {

    auto mesh_size = m_model.VertexCount();

    colours = { (Model::colour_type*)linearAlloc(mesh_size * sizeof(Model::colour_type)), mesh_size };
    //std::fill(colours.begin(), colours.end(), Model::colour_type{ 1.0f, 1.0f, 1.0f });

    // random colours!
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(0.2f, 1.0f);
    std::generate(colours.begin(), colours.end(), [&]{
        return Model::colour_type { distribution(gen), distribution(gen), distribution(gen) };
    });
}

EntLevelGeometry::~EntLevelGeometry() {
    linearFree(colours.data());
    colours = {};
}
