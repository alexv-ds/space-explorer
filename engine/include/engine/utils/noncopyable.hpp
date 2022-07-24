#pragma once

namespace engine::utils {

//спизжено с буста
class noncopyable {
protected:
  constexpr noncopyable() = default;
  ~noncopyable() = default;
  noncopyable(const noncopyable &) = delete;
  noncopyable & operator=(const noncopyable &) = delete;
};

}