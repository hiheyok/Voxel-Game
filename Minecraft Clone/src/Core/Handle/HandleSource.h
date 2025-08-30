#pragma once

#include <memory>
#include <utility>

#include "Core/Handle/HandleControlBlock.h"

class HandleSource {
 public:
  HandleSource() = default;
  ~HandleSource();

  void TransferSourceTo(HandleSource& destination) {
    if (!handle_control_) {
      return;
    }

    destination.handle_control_ = std::move(handle_control_);
    destination.handle_control_->ptr_ = &destination;
    destination.handle_control_->version_++;
  }

 private:
  std::shared_ptr<HandleControlBlock> handle_control_;
};

