#include "RenderEngine/OpenGL/Buffers/BufferStorage.h"
#include "RenderEngine/OpenGL/Shader/ComputeShader.h"
#include "Utils/LogUtils.h"

BufferStorage::BufferStorage() : 
    copy_shader_{ std::make_unique<ComputeShader>("assets/shaders/Buffers/CopyData.glsl") },
    move_shader_{ std::make_unique<ComputeShader>("assets/shaders/Buffers/MoveData.glsl") } {}

BufferStorage::~BufferStorage() {
    if (buffer_storage_id_ != 0) {
        Delete();
    }
}

void BufferStorage::Delete() {
    if (buffer_storage_id_ != 0) {
        glDeleteBuffers(1, &buffer_storage_id_);
        buffer_storage_id_ = 0; // Use 0 to indicate deleted/uninitialized
        max_size_ = 0;
        target_ = 0;
        g_logger.LogDebug("BufferStorage::Delete", "Deleted buffer storage. ID was: " + std::to_string(buffer_storage_id_)); // Log before setting to 0
    }

}

void BufferStorage::Bind() {
    if (buffer_storage_id_ != 0) {
        glBindBuffer(target_, buffer_storage_id_);
    }
    else {
        g_logger.LogWarn("BufferStorage::Bind", "Attempted to bind an uninitialized/deleted buffer storage.");
    }
}

void BufferStorage::Unbind() {
    glBindBuffer(target_, 0);
}


void BufferStorage::Create(GLuint bufferTarget, uint64_t size, bool dynamic, const void* data) {
    if (buffer_storage_id_ != 0) {
        g_logger.LogWarn("BufferStorage::Create", "Create called on an already initialized buffer storage. Deleting old one. ID: " + std::to_string(buffer_storage_id_));
        Delete();
    }

    target_ = bufferTarget;
    max_size_ = size;

    glGenBuffers(1, &buffer_storage_id_);

    if (buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::Create", "glGenBuffers failed!");
        max_size_ = 0;
        target_ = 0;
        return;
    }

    g_logger.LogDebug("BufferStorage::Create", "Generated buffer storage. ID: " + std::to_string(buffer_storage_id_));
    Bind();

    // *** Use glBufferStorage ***
    GLbitfield flags = 0;
    if (dynamic) {
        flags |= GL_DYNAMIC_STORAGE_BIT; // Crucial for glBufferSubData/glCopyBufferSubData
    }
    // *** NO MAPPING FLAGS (GL_MAP_*) are included ***
    // This strongly hints the driver to keep the buffer in VRAM if possible.

    glBufferStorage(target_, static_cast<GLsizeiptr>(size), data, flags);

    // Check for OpenGL errors after buffer storage creation
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        g_logger.LogError("BufferStorage::Create", "OpenGL error after glBufferStorage: " + std::to_string(err));
        Delete(); // Clean up failed buffer
        return;
    }


    g_logger.LogInfo("BufferStorage::Create", "Created buffer storage ID: " + std::to_string(buffer_storage_id_)
        + " Size: " + std::to_string(size) + " bytes, Target: " + std::to_string(target_)
        + ", Dynamic: " + (dynamic ? "Yes" : "No"));

    /*glGenBuffers(1, &buffer_storage_id_);
    Bind();
    GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT;
    glBufferStorage(target_, size, nullptr, flags);*/
}

void BufferStorage::InsertData(uint64_t offset, uint64_t size, const void* data) {
    if (buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::InsertData", "Attempted InsertData on uninitialized buffer.");
        return;
    }

    if (offset + size > max_size_) {
        g_logger.LogError("BufferStorage::InsertData", "InsertData exceeds buffer bounds. Offset: " + std::to_string(offset)
            + ", Size: " + std::to_string(size) + ", MaxSize: " + std::to_string(max_size_));
        return;
    }

    Bind();
    glBufferSubData(target_, offset, size, data);
}

void BufferStorage::CopyFrom(BufferStorage& sourceBuffer, size_t readOffset, size_t writeOffset, size_t size) {
    CopyFrom(&sourceBuffer, readOffset, writeOffset, size);
}

void BufferStorage::CopyTo(BufferStorage& destinationBuffer, size_t readOffset, size_t writeOffset, size_t size) {
    CopyTo(&destinationBuffer, readOffset, writeOffset, size);
}

void BufferStorage::CopyFrom(BufferStorage* sourceBuffer, size_t readOffset, size_t writeOffset, size_t size) {
    if (buffer_storage_id_ == 0 || sourceBuffer->buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::CopyFrom", "Attempted CopyFrom with uninitialized buffer(s).");
        return;
    }
    if (writeOffset + size > max_size_ || readOffset + size > sourceBuffer->max_size_) {
        g_logger.LogError("BufferStorage::CopyFrom", "CopyFrom exceeds buffer bounds.");
        return;
    }

    copy_shader_->BindBufferAsSSBO(sourceBuffer->buffer_storage_id_, 0);
    copy_shader_->BindBufferAsSSBO(buffer_storage_id_, 1);
    copy_shader_->SetInt("srcOffset", readOffset / sizeof(unsigned int));
    copy_shader_->SetInt("dstOffset", writeOffset / sizeof(unsigned int));
    copy_shader_->SetInt("copySize", size / sizeof(unsigned int));
    copy_shader_->DispatchCompute((int)ceil((double)size / kWorkGroupSize / sizeof(unsigned int)), 1, 1);
    //copy_shader_->BindBufferAsSSBO(0, 0);
    //copy_shader_->BindBufferAsSSBO(0, 1);

    copy_shader_->SSBOMemoryBarrier();
}

void BufferStorage::CopyTo(BufferStorage* destinationBuffer, size_t readOffset, size_t writeOffset, size_t size) {
    if (buffer_storage_id_ == 0 || destinationBuffer->buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::CopyTo", "Attempted CopyTo with uninitialized buffer(s).");
        return;
    }
    if (readOffset + size > max_size_ || writeOffset + size > destinationBuffer->max_size_) {
        g_logger.LogError("BufferStorage::CopyTo", "CopyTo exceeds buffer bounds.");
        return;
    }
    copy_shader_->BindBufferAsSSBO(buffer_storage_id_, 0);
    copy_shader_->BindBufferAsSSBO(destinationBuffer->buffer_storage_id_, 1);
    copy_shader_->SetInt("srcOffset", readOffset / sizeof(unsigned int));
    copy_shader_->SetInt("dstOffset", writeOffset / sizeof(unsigned int));
    copy_shader_->SetInt("copySize", size / sizeof(unsigned int));
    copy_shader_->DispatchCompute((int)ceil((double)size / kWorkGroupSize / sizeof(unsigned int)), 1, 1);
    //copy_shader_->BindBufferAsSSBO(0, 0);
    //copy_shader_->BindBufferAsSSBO(0, 1);

    copy_shader_->SSBOMemoryBarrier();
}

void BufferStorage::MoveData(size_t readOffset, size_t writeOffset, size_t size) {
    if (buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::MoveData", "Attempted MoveData with uninitialized buffer(s).");
        return;
    }
    if (readOffset + size > max_size_ || writeOffset + size >max_size_) {
        g_logger.LogError("BufferStorage::MoveData", "MoveData exceeds buffer bounds.");
        return;
    }

    move_shader_->BindBufferAsSSBO(buffer_storage_id_, 0);
    move_shader_->SetInt("srcOffset", readOffset / sizeof(unsigned int));
    move_shader_->SetInt("dstOffset", writeOffset / sizeof(unsigned int));
    move_shader_->SetInt("copySize", size / sizeof(unsigned int));
    move_shader_->DispatchCompute((int)ceil((double)size / kWorkGroupSize / sizeof(unsigned int)), 1, 1);

    move_shader_->SSBOMemoryBarrier();
}

bool BufferStorage::IsInitialized() const { return buffer_storage_id_ != 0; }

size_t BufferStorage::GetMaxSize() const { return max_size_; }