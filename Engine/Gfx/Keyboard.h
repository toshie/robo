#include <unordered_map>

class Keyboard
{
  enum class State
  {
    Down,
    Up
  };

public:
  Keyboard() = default;

  void keyDown(unsigned char key, int /*x*/, int /*y*/)
  {
    _keyStates[key] = State::Down;
  }

  void keyUp(unsigned char key, int /*x*/, int /*y*/)
  {
    _keyStates[key] = State::Up;
  }

  bool isKeyDown(char key) const
  {
    auto found = _keyStates.find(key);
    if (found != _keyStates.end())
      return found->second == State::Down;

    return false;
  }

private:
  std::unordered_map<unsigned char, State> _keyStates;
};
