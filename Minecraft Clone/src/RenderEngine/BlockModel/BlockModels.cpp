#include "BlockModels.h"
#include "../../Level/typenames.h"
#include "../../Utils/LogUtils.h"

void ModelV2::BlockModelV2::BakeTextureRotation() {
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

void ModelV2::BlockModelV2::FlattenVariables() {
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