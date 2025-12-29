#pragma once

#include <iterator>

#include "Core/Typenames.h"
#include "Utils/Assert.h"

/**
 * This class provides a bijective mapping of EntityUUID <---> EntityCompId
 * This class also provides a mapping for the component data
 **/

template <class ComponentType>
class ComponentMap {
 public:
  class Iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<EntityUUID, ComponentType>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

   private:
    const ComponentMap* map_;
    EntityComponentId current_id_;
    // Cache the current pair to return references to it
    mutable value_type current_pair_;

   public:
    // Default constructor
    Iterator() : map_(nullptr), current_id_(kInvalidComponentId) {}

    // Constructor with map and position
    Iterator(const ComponentMap* map, EntityComponentId id)
        : map_(map), current_id_(id) {
      if (map_ && current_id_ >= 0 &&
          current_id_ <
              static_cast<EntityComponentId>(map_->component_data_.size())) {
        UpdateCurrentPair();
      }
    }

    // Dereference operators
    reference operator*() const {
      UpdateCurrentPair();
      return current_pair_;
    }

    pointer operator->() const {
      UpdateCurrentPair();
      return &current_pair_;
    }

    // Prefix increment
    Iterator& operator++() {
      ++current_id_;
      return *this;
    }

    // Postfix increment
    Iterator operator++(int) {
      Iterator temp = *this;
      ++current_id_;
      return temp;
    }

    // Equality operators
    bool operator==(const Iterator& other) const {
      return map_ == other.map_ && current_id_ == other.current_id_;
    }

    bool operator!=(const Iterator& other) const { return !(*this == other); }

   private:
    void UpdateCurrentPair() const {
      if (map_ && current_id_ >= 0 &&
          current_id_ <
              static_cast<EntityComponentId>(map_->component_data_.size())) {
        current_pair_.first = map_->component_id_to_uuid_[current_id_];
        current_pair_.second = map_->component_data_[current_id_];
      }
    }
  };

  // Const Iterator
  class ConstIterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<EntityUUID, const ComponentType>;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

   private:
    const ComponentMap* map_;
    EntityComponentId current_id_;
    // Use non-const pair internally so we can assign to it
    mutable std::pair<EntityUUID, ComponentType> current_pair_cache_;

   public:
    ConstIterator() : map_(nullptr), current_id_(kInvalidComponentId) {}

    ConstIterator(const ComponentMap* map, EntityComponentId id)
        : map_(map), current_id_(id) {
      if (map_ && current_id_ >= 0 &&
          current_id_ <
              static_cast<EntityComponentId>(map_->component_data_.size())) {
        UpdateCurrentPair();
      }
    }

    // Allow construction from non-const iterator
    ConstIterator(const Iterator& it)
        : map_(it.map_), current_id_(it.current_id_) {
      if (map_ && current_id_ >= 0 &&
          current_id_ <
              static_cast<EntityComponentId>(map_->component_data_.size())) {
        UpdateCurrentPair();
      }
    }

    reference operator*() const {
      UpdateCurrentPair();
      return reinterpret_cast<reference>(current_pair_cache_);
    }

    pointer operator->() const {
      UpdateCurrentPair();
      return reinterpret_cast<pointer>(&current_pair_cache_);
    }

    ConstIterator& operator++() {
      ++current_id_;
      return *this;
    }

    ConstIterator operator++(int) {
      ConstIterator temp = *this;
      ++current_id_;
      return temp;
    }

    bool operator==(const ConstIterator& other) const {
      return map_ == other.map_ && current_id_ == other.current_id_;
    }

    bool operator!=(const ConstIterator& other) const {
      return !(*this == other);
    }

   private:
    void UpdateCurrentPair() const {
      if (map_ && current_id_ >= 0 &&
          current_id_ <
              static_cast<EntityComponentId>(map_->component_data_.size())) {
        current_pair_cache_.first = map_->component_id_to_uuid_[current_id_];
        current_pair_cache_.second = map_->component_data_[current_id_];
      }
    }

    friend class Iterator;
  };

  ComponentMap();

  ComponentMap(ComponentMap&&) = delete;

  ComponentMap& operator=(const ComponentMap&) = default;

  ComponentType& AddComponent(EntityUUID uuid);
  std::vector<ComponentType>& GetAllComponent() noexcept;
  const std::vector<ComponentType>& GetAllComponent() const noexcept;
  ComponentType& GetComponent(EntityUUID uuid) noexcept;
  const ComponentType& GetComponent(EntityUUID uuid) const noexcept;
  void RemoveComponent(EntityUUID uuid) noexcept;
  bool HasComponent(EntityUUID uuid) const noexcept;
  void ReplaceComponent(EntityUUID uuid,
                        const ComponentType& component) noexcept;

  ComponentType& operator[](EntityUUID uuid) noexcept;
  const ComponentType& operator[](EntityUUID uuid) const noexcept;

  // Iterator methods
  Iterator begin();
  Iterator end();

  ConstIterator begin() const;
  ConstIterator end() const;
  ConstIterator cbegin() const;
  ConstIterator cend() const;

  size_t size() const noexcept;

 private:
  static constexpr EntityComponentId kInvalidComponentId = -1;
  static constexpr EntityUUID kInvalidUUIDId = -1;

  EntityComponentId GetComponentId(EntityUUID uuid) const noexcept;
  EntityUUID GetEntityUUID(EntityComponentId component_id) const noexcept;

  // component_data_[component_id] -> component_data
  std::vector<ComponentType> component_data_;
  // uuid_to_component_id_[uuid] -> component_id
  std::vector<EntityComponentId> uuid_to_component_id_;
  // component_id_to_uuid_[component_id] -> uuid
  std::vector<EntityUUID> component_id_to_uuid_;
};

template <class ComponentType>
ComponentMap<ComponentType>::ComponentMap() = default;

template <class ComponentType>
ComponentType& ComponentMap<ComponentType>::AddComponent(EntityUUID uuid) {
  EntityComponentId component_id = component_data_.size();
  component_data_.emplace_back();

  if (uuid_to_component_id_.size() <= uuid) {
    uuid_to_component_id_.resize(uuid + 1, kInvalidComponentId);
  }

  GAME_ASSERT(uuid_to_component_id_[uuid] == kInvalidComponentId,
              "Entity already has component");

  uuid_to_component_id_[uuid] = component_id;
  component_id_to_uuid_.push_back(uuid);
  return component_data_.back();
}

template <class ComponentType>
std::vector<ComponentType>&
ComponentMap<ComponentType>::GetAllComponent() noexcept {
  return component_data_;
}

template <class ComponentType>
const std::vector<ComponentType>& ComponentMap<ComponentType>::GetAllComponent()
    const noexcept {
  return component_data_;
}

template <class ComponentType>
ComponentType& ComponentMap<ComponentType>::GetComponent(
    EntityUUID uuid) noexcept {
  EntityComponentId component_id = GetComponentId(uuid);
  GAME_ASSERT(component_id != kInvalidComponentId, "Invalid entity uuid");
  return component_data_[component_id];
}

template <class ComponentType>
const ComponentType& ComponentMap<ComponentType>::GetComponent(
    EntityUUID uuid) const noexcept {
  EntityComponentId component_id = GetComponentId(uuid);
  GAME_ASSERT(component_id != kInvalidComponentId, "Invalid entity uuid");
  return component_data_[component_id];
}

template <class ComponentType>
void ComponentMap<ComponentType>::RemoveComponent(EntityUUID uuid) noexcept {
  // swap and pop
  EntityComponentId component_id = GetComponentId(uuid);

  GAME_ASSERT(component_id != kInvalidComponentId,
              "Tried to remove an invalid component");
  GAME_ASSERT(!component_data_.empty(),
              "Tried to remove from an empty ComponentMap.");

  EntityComponentId component_id_back = component_data_.size() - 1;

  if (component_id_back != component_id) {
    EntityUUID uuid_back = GetEntityUUID(component_id_back);

    std::swap(component_data_[component_id],
              component_data_[component_id_back]);

    component_id_to_uuid_[component_id] = uuid_back;
    uuid_to_component_id_[uuid_back] = component_id;
  }

  component_data_.pop_back();
  component_id_to_uuid_.pop_back();

  uuid_to_component_id_[uuid] = kInvalidComponentId;
}

template <class ComponentType>
EntityComponentId ComponentMap<ComponentType>::GetComponentId(
    EntityUUID uuid) const noexcept {
  if (uuid_to_component_id_.size() <= uuid) {
    return kInvalidComponentId;
  }

  return uuid_to_component_id_[uuid];
}

template <class ComponentType>
EntityUUID ComponentMap<ComponentType>::GetEntityUUID(
    EntityComponentId component_id) const noexcept {
  if (component_id_to_uuid_.size() <= component_id) {
    return kInvalidUUIDId;
  }

  return component_id_to_uuid_[component_id];
}

template <class ComponentType>
bool ComponentMap<ComponentType>::HasComponent(EntityUUID uuid) const noexcept {
  return GetComponentId(uuid) != kInvalidComponentId;
}

template <class ComponentType>
ComponentMap<ComponentType>::Iterator ComponentMap<ComponentType>::begin() {
  // When empty, return same as end() so loop doesn't execute
  return Iterator(this, component_data_.empty() ? static_cast<EntityComponentId>(component_data_.size()) : 0);
}

template <class ComponentType>
ComponentMap<ComponentType>::Iterator ComponentMap<ComponentType>::end() {
  return Iterator(this, static_cast<EntityComponentId>(component_data_.size()));
}

template <class ComponentType>
ComponentMap<ComponentType>::ConstIterator ComponentMap<ComponentType>::begin()
    const {
  // When empty, return same as end() so loop doesn't execute
  return ConstIterator(this, component_data_.empty() ? static_cast<EntityComponentId>(component_data_.size()) : 0);
}

template <class ComponentType>
ComponentMap<ComponentType>::ConstIterator ComponentMap<ComponentType>::end()
    const {
  return ConstIterator(this,
                       static_cast<EntityComponentId>(component_data_.size()));
}

template <class ComponentType>
ComponentMap<ComponentType>::ConstIterator ComponentMap<ComponentType>::cbegin()
    const {
  // When empty, return same as cend() so loop doesn't execute
  return ConstIterator(this, component_data_.empty() ? static_cast<EntityComponentId>(component_data_.size()) : 0);
}

template <class ComponentType>
ComponentMap<ComponentType>::ConstIterator ComponentMap<ComponentType>::cend()
    const {
  return ConstIterator(this,
                       static_cast<EntityComponentId>(component_data_.size()));
}

template <class ComponentType>
void ComponentMap<ComponentType>::ReplaceComponent(
    EntityUUID uuid, const ComponentType& component) noexcept {
  EntityComponentId component_id = GetComponentId(uuid);
  if (component_id == kInvalidComponentId) {
    AddComponent(uuid);
    component_id = GetComponentId(uuid);  // Re-fetch after adding!
  }
  component_data_[component_id] = component;
}


template <class ComponentType>
ComponentType& ComponentMap<ComponentType>::operator[](
    EntityUUID uuid) noexcept {
  EntityComponentId component_id = GetComponentId(uuid);
  GAME_ASSERT(component_id != kInvalidComponentId, "Invalid entity uuid");
  return component_data_[component_id];
}

template <class ComponentType>
const ComponentType& ComponentMap<ComponentType>::operator[](
    EntityUUID uuid) const noexcept {
  EntityComponentId component_id = GetComponentId(uuid);
  GAME_ASSERT(component_id != kInvalidComponentId, "Invalid entity uuid");
  return component_data_[component_id];
}


template <class ComponentType>
size_t ComponentMap<ComponentType>::size() const noexcept {
  return component_data_.size();
}
