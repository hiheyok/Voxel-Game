#pragma once

#include <any>
#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>

#include "Core/Typenames.h"

class Screen;
class GameContext;
class ScreenManager;

template <typename T>
concept ScreenType = std::is_base_of_v<Screen, std::decay_t<T>>;

class ScreenRegistry {
 public:
  explicit ScreenRegistry(GameContext&);

  template <ScreenType, typename... ContextArgs>
  void RegisterScreen(const std::string& screen_name);

  template <typename... Args>
  std::unique_ptr<Screen> CreateScreen(const std::string& name,
                                       Args&&... args) const;

 private:
  GameContext& context_;
  FastHashMap<std::string, std::any> screens_;
};

template <ScreenType TScreen, typename... ContextArgs>
void ScreenRegistry::RegisterScreen(const std::string& screen_name) {
  GAME_ASSERT(!screens_.count(screen_name),
              "Attempted to register screen that already exist.");

  std::function<std::unique_ptr<Screen>(ScreenManager&, ContextArgs...)>
      factory = [&](ScreenManager& screen_manager,
                    ContextArgs... args) -> std::unique_ptr<Screen> {
    return std::make_unique<TScreen>(context_, screen_manager,
                                     std::forward<ContextArgs>(args)...);
  };

  screens_[screen_name] = factory;
}

template <typename... Args>
std::unique_ptr<Screen> ScreenRegistry::CreateScreen(const std::string& name,
                                                     Args&&... args) const {
  auto it = screens_.find(name);
  GAME_ASSERT(it != screens_.end(), "Invalid screen");
  using FactoryType = std::function<std::unique_ptr<Screen>(Args...)>;

  try {
    const auto& factory = std::any_cast<const FactoryType&>(it->second);
    return factory(std::forward<Args>(args)...);
  } catch (const std::bad_any_cast& e) {
    throw std::runtime_error(
        "ScreenRegistry::Create - Mismatched arguments for screen '" + name +
        "'. " + e.what());
  }
}