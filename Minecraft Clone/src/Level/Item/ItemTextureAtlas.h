#pragma once
#include "Level/Item/Item.h"
#include "RenderEngine/OpenGL//Texture/Types/Texture2D.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/ItemRender/BlockItemRender.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"

#include <glm/vec2.hpp>

struct ItemUVMapping {
    glm::vec2 uv_1_;
    glm::vec2 uv_2_;
};

class ItemTextureAtlas {
    Texture2D individual_item_;

    VertexArray vao_;
    Buffer ebo_;
    Buffer vbo_;

    BlockItemRender block_item_renderer_;
    Shader stitching_shader_;
    int individual_size_ = 0;
    int atlas_size_ = 0;
    TexturedFrameBuffer atlas_framebuffer_;
    TexturedFrameBuffer framebuffer_single_block_render_;

    void RenderBlockItem(Item item);

    void StitchTexture(size_t index, ItemID ItemID);

    
public:
    FastHashMap<int, ItemUVMapping> items_uv_map_;
    
    Texture2D atlas_;

    FastHashMap<ItemID, size_t> offsets_;
    ItemTextureAtlas();
    void Initialize(int atlasItemSize, int individualItemSize);

    void AddItem(Item item);
};

extern ItemTextureAtlas g_item_atlas;