#pragma once

struct WorldParameters {
    size_t horizontal_ticking_distance_ = 16;
    size_t vertical_ticking_distance_ = 8;

    size_t spawn_chunk_horizontal_radius_ = 10;
    size_t spawn_chunk_vertical_radius_ = 10;

    float gravity_ = 10.f;

    WorldParameters() = default;
    ~WorldParameters() = default;
    WorldParameters(const WorldParameters&) = default;
};