#ifndef INPUT_H_
#define INPUT_H_

struct Input {
    bool  forward;
    bool  backward;
    bool  strafe_left;
    bool  strafe_right;
    float mouse_dx; // horizontal pixels this frame, right = positive
    bool  quit;     // window close or Escape
};

#endif
