#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KEY_LEFT 0
#define KEY_RIGHT 1
#define KEY_UP 2
#define KEY_DOWN 3

struct World;

struct Keyboard
{
  struct World *world;
  int left;
  int right;
  int up;
  int down;
};

struct Keyboard *KeyboardCreate(struct World *world);
int KeyboardKeyDown(struct Keyboard *ctx, int keyCode);

#endif