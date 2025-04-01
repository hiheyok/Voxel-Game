#include "RenderEngine/BlockModel/BlockModels.h"
#include "Core/Typenames.h"
#include "Utils/LogUtils.h"

void Model::BlockModel::AddDisplay(BlockDisplay display, DisplayPosition position) {
    display_[position] = display;
}

bool Model::BlockModel::CheckDisplay(DisplayPosition position) {
    return display_[position].initialized_;
}

void Model::BlockModel::AddElement(Cuboid element) {
    elements_.push_back(element);
}

void Model::BlockModel::GetVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    for (Cuboid& element : elements_) {
        glm::vec3 from = element.from_;
        glm::vec3 to = element.to_;
        from = from / 16.f;
        to = to / 16.f;
        for (uint8_t i = 0; i < 6; i++) {
            BlockFace face = element.faces_[i];
            if (face.reference_texture_.length() == 0) continue;

            glm::vec2 uv0{ face.uv_.x,  face.uv_.y };
            glm::vec2 uv1{ face.uv_.z,  face.uv_.w };

            uv0 = uv0 / 16.f;
            uv1 = uv1 / 16.f;

            uint8_t axis_ = i >> 1;

            uint32_t currIndex = static_cast<uint32_t>(vertices.size()) / 7;

            switch (axis_) {
            case 0:
                (i & 1) == 1 ?
                    vertices.insert(vertices.end(),
                        {
                            from.x,    from.y, from.z,        uv1.x, uv0.y,    (float)face.texture_id_,    8.f,
                            from.x, to.y,    from.z,        uv1.x, uv1.y,    (float)face.texture_id_,    8.f,
                            from.x, to.y,    to.z,        uv0.x, uv1.y,    (float)face.texture_id_,    8.f,
                            from.x, from.y,    to.z,        uv0.x, uv0.y,    (float)face.texture_id_,    8.f,
                        }
                        ) :
                    vertices.insert(vertices.end(),
                        {
                            to.x, from.y,   from.z,        uv1.x, uv0.y,    (float)face.texture_id_,    8.f,
                            to.x, to.y,        from.z,        uv1.x, uv1.y,    (float)face.texture_id_,    8.f,
                            to.x, to.y,        to.z,        uv0.x, uv1.y,    (float)face.texture_id_,    8.f,
                            to.x, from.y,    to.z,        uv0.x, uv0.y,    (float)face.texture_id_,    8.f,
                        }
                        );
                break;
            case 1:
                (i & 1) == 1 ?
                    vertices.insert(vertices.end(),
                        {
                            from.x, from.y, from.z,        uv1.x, uv0.y,    (float)face.texture_id_,    15.f,
                            to.x,    from.y, from.z,        uv0.x, uv0.y,    (float)face.texture_id_,    15.f,
                            to.x,    from.y, to.z,        uv0.x, uv1.y,    (float)face.texture_id_,    15.f,
                            from.x, from.y, to.z,        uv1.x, uv1.y,    (float)face.texture_id_,    15.f,
                        }
                        ) :
                    vertices.insert(vertices.end(),
                        {
                            from.x, to.y,    from.z,        uv1.x, uv0.y,    (float)face.texture_id_,    15.f,
                            to.x,    to.y,    from.z,        uv0.x, uv0.y,    (float)face.texture_id_,    15.f,
                            to.x,    to.y,    to.z,        uv0.x, uv1.y,    (float)face.texture_id_,    15.f,
                            from.x, to.y,    to.z,        uv1.x, uv1.y,    (float)face.texture_id_,    15.f,
                        }
                        );
                break;
            case 2:
                (i & 1) == 1 ?
                    vertices.insert(vertices.end(),
                        {
                            from.x, from.y, from.z,        uv1.x, uv0.y,    (float)face.texture_id_,    12.f,
                            to.x,    from.y, from.z,        uv0.x, uv0.y,    (float)face.texture_id_,    12.f,
                            to.x,    to.y,    from.z,        uv0.x, uv1.y,    (float)face.texture_id_,    12.f,
                            from.x, to.y,    from.z,        uv1.x, uv1.y,    (float)face.texture_id_,    12.f,
                        }
                        ) :
                    vertices.insert(vertices.end(),
                        {
                            from.x, from.y, to.z,        uv1.x, uv0.y,    (float)face.texture_id_,    12.f,
                            to.x,    from.y, to.z,        uv0.x, uv0.y,    (float)face.texture_id_,    12.f,
                            to.x,    to.y,    to.z,        uv0.x, uv1.y,    (float)face.texture_id_,    12.f,
                            from.x, to.y,    to.z,        uv1.x, uv1.y,    (float)face.texture_id_,    12.f,
                        }
                        );
                break;

            }

            if ((i & 1) == 0) {
                indices.insert(indices.end(),
                    {
                        0 + currIndex, 1 + currIndex, 2 + currIndex,
                        2 + currIndex, 3 + currIndex, 0 + currIndex
                    }
                );
            }
        }
    }
}

void Model::BlockModel::BakeTextureRotation() {
    for (Cuboid& element : elements_) {
        for (BlockFace& face : element.faces_) {
            if (face.reference_texture_.length() == 0) continue; //means not initialized

            int rotation_ = face.rotation_;

            face.uv_coord_nn = glm::ivec2(face.uv_.x, face.uv_.y);
            face.uv_coord_np = glm::ivec2(face.uv_.x, face.uv_.w);
            face.uv_coord_pp = glm::ivec2(face.uv_.z, face.uv_.w);
            face.uv_coord_pn = glm::ivec2(face.uv_.z, face.uv_.y);

            for (int r = 0; r < rotation_; r += 90) {
                std::swap(face.uv_coord_nn, face.uv_coord_np);
                std::swap(face.uv_coord_nn, face.uv_coord_pn);
                std::swap(face.uv_coord_pn, face.uv_coord_pp);
            }
        }
    }
}

void Model::BlockModel::FlattenVariables() {
    FastHashMap<std::string, std::string> variableMatcher{};
    FastHashSet<std::string> variableNames{};
    

    //This will set all of the texture variables

    for (const auto& [key, value] : texture_variable_) {
        //Check if there is a # at the beginning
        variableNames.insert('#' + key);
        if (value[0] == '#') {
            variableNames.insert(value);
        }

        //In this case, i am inserting both key and values because a variable could be referencing another variable
    }

    //Then it will find the absolute value corresponding to those variables


    for (const std::string& variable : variableNames) {
        std::string textureName = variable;
        std::vector<std::string> variableCorrspondingToSameTexture = {};

        FastHashSet<std::string> variableVisitMap{};

        

        int i = 0;
        while (textureName[0] == '#') {
            if (variableMatcher.count(textureName)) { //top down memorization
                textureName = variableMatcher[textureName];
                break;
            }
            if (i > 10000) { //too lazy to fix infinite loop  issues for now
                break;
            }
            i++;
            variableCorrspondingToSameTexture.push_back(textureName);
            textureName = texture_variable_[textureName.substr(1, textureName.length() - 1)];
        }

        for (const std::string& var : variableCorrspondingToSameTexture) {
            variableMatcher[var] = textureName;
        }
        variableCorrspondingToSameTexture.clear();
    }

    //After all of the texture are flatten it will go through the faces and set the textures
    for (Cuboid& element : elements_) {
        for (BlockFace& face : element.faces_) {
            if (face.reference_texture_.empty()) continue; //if there is no texture face is not initialized (skip)
            if (face.reference_texture_[0] != '#') continue;
            face.reference_texture_ = variableMatcher[face.reference_texture_];
        }
    }
}