# Outputting Strings

### Outputting text at an absolute position

Let's add a `put_str` function to the screen module to output a string at a particular position on screen.

```c
// screen.c

...

void put_str(char* str, char attr, int row, int col) {
    int offset = row * SCREEN_COLS + col;
    for (char* p = str; *p; p++) {
        *(video_memory + offset++) = (attr << 8) | *p*;
    }
}
```
Let's also define all the text mode colors so that callers can use them individually if needed.

```c
// screen.h

#pragma once

// text mode colors
#define BLACK       0x0
#define BLUE        0x1
#define GREEN       0x2
#define CYAN        0x3
#define RED         0x4
#define MAGENTA     0x5
#define BROWN       0x6
#define GRAY_LT     0x7
#define GRAY_DK     0x8
#define BLUE_LT     0x9
#define GREEN_LT    0xA
#define CYAN_LT     0xB
#define RED_LT      0xC
#define MAGENTA_LT  0xD
#define YELLOW      0xE
#define WHITE       0xF

// predefined foreground and background color combinations
#define GRAY_LT_ON_BLACK (BLACK << 4 | GRAY_LT)
#define WHITE_ON_BLUE    (BLUE << 4 | WHITE)
```

Let's make use of this new functionality in the kernel to display a "Booting kernel..." message.

```c
// kernel.c

void kmain() {
    ...

    put_str("Booting kernel", GRAY_LT_ON_BLACK, 0 /* row */, 0 /* col */);
    ...
}
```

Running this should produce the expected result.

```
Booting kernel
A
B
```

### Outputting text in a "flow"

We are able to output strings at an absolute position on screen. However, having to specify the screen position every time we just want to output something is going to be tedious. What we need is a way to output text in a "flow" mode; i.e. keep track of the next position to output text, and update it after every output operation. Let's add two new functions equivalent to `write_char` and `write_str` without the position arguments.

```c
// screen.h

uint8_t current_offset = 0;

...

void write_char(char ch, char attr) {
    *(video_memory + current_offset++) = (attr << 8) | ch;
}

void write_str(char* str, char attr) {
    for (char* p = str; *p; p++) {
        write_char(*p);
    }
}
```

Notice that we're starting to have duplication between the `put_*` functions and their equivalent `write_*`. Let's refactor things a bit to remove that duplication.

```c
// screen.h

#define OFFSET(row, col) (row * SCREEN_COLS + col)
...

/**
 * Private functions
 */

int put_char_at(char ch, char attr, int offset) {
    *(video_memory + offset++) = (attr << 8) | ch;
    return offset;
}

int put_str_at(char* str, char attr, int offset) {
    for (char* p = str; *p; p++) {
        offset = put_char_at(*p, attr, offset);
    }
    return offset;
}


/**
 * Public functions
 */

void clear_screen() {
    for (int i = 0; i < (SCREEN_ROWS * SCREEN_COLS); i++) {
        put_char_at(0, 0, i);
    }
    current_offset = 0;
}

void put_char(char ch, char attr, int row, int col) {
    put_char_at(ch, attr, OFFSET(row, col));
}

void put_str(char* str, char attr, int row, int col) {
    put_str_at(str, attr, OFFSET(row, col));
}

void write_char(char ch, char attr) {
    current_offset = put_char_at(ch, attr, current_offset);
}

void write_str(char* str, char attr) {
    current_offset = put_str_at(str, attr, current_offset);
}
```

Now we should be able to simplify our `kmain` function a bit more:

```c
// kernel.c

void kmain() {
    ...

    write_str("Booting kernel", GRAY_LT_ON_BLACK);
    ...
}
```

At some point we will come back and handle scrolling, but this should be enough for now.

### Recap

* We added a screen function to output a string at a particular position.
* We added the ability to track the current output position for position-free output calls.
* We refactored the screen functions to reduce code duplication.

