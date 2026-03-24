/// @file input/input.h
/// @brief Platform-agnostic per-frame input snapshot.
///
/// Game::poll_events() fills one of these each frame by translating SDL events
/// into booleans and a mouse delta.  Domain code (Player) receives Input
/// directly and never touches SDL types.

#pragma once

/// @brief Aggregated input state for one frame.
///        Produced by Game::poll_events() and consumed by Player::handle_input().
struct Input {
    bool  forward;      ///< W key held
    bool  backward;     ///< S key held
    bool  strafe_left;  ///< A key held
    bool  strafe_right; ///< D key held
    float mouse_dx;     ///< Horizontal mouse delta in pixels (right = positive)
    bool  quit;         ///< True when the user requests exit (window close or Escape)
};
