#include <iostream>
#include <X11/Xlib.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

Display *dpy;
Window root_window;

int src_width = 0;
int src_height = 0;
int step_x = 0;
int step_y = 0;

struct CurPos
{
    int x = 0;
    int y = 0;
};

void initScreen()
{
    dpy = XOpenDisplay(nullptr);
    int s_num = XDefaultScreen(dpy);
    Screen *screen = ScreenOfDisplay(dpy, s_num);
    src_width = screen->width;
    src_height = screen->height;
    XCloseDisplay(dpy);
    dpy = nullptr;
    step_x = int((src_width / 14));
    step_y = int((src_height / 5));
}

CurPos calcPos(int key)
{
    CurPos pos = CurPos();
    pos.x = step_x * (key % 14);
    if(key < 15 && key > 2)
    {
        pos.y = step_y * 0;
    }
    else if(key < 29 && key >=15)
    {
        pos.y = step_y * 1;
    }
    else if(key < 44 && key >= 30)
    {
        pos.y = step_y * 2;
    }
    else if(key < 56  && key >= 45)
    {
        pos.y = step_y * 3;
    }
    else if(key >= 57)
    {
        pos.y = step_y * 4;
    }
    //pos.y = step_y * (key % 6);
    printf("CursorPos: %dx%d\n", pos.x, pos.y);
    return pos;
}

void moveCursor(int key)
{
    printf("Screen: %dx%d\n", src_width, src_height);
    printf("Steps: %dx%d\n", step_x, step_y);
    CurPos pos = calcPos(key);
    dpy = XOpenDisplay(0);
    root_window = XRootWindow(dpy, 0);
    XSelectInput(dpy, root_window, KeyReleaseMask);
    XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, pos.x, pos.y);
    XSync(dpy, False);
    XCloseDisplay(dpy);
    dpy = nullptr;
}

void keylog(const char *device)
{
    int fd = open(device, O_RDONLY);
    struct input_event ev{};
    while (true)
    {
        read(fd, &ev, sizeof(struct input_event));
        if(ev.type == 1)
        {
            printf("key %i state %i\n", ev.code, ev.value);
            moveCursor(ev.code);
            if(ev.code == 104)
            {
                break;
            }
        }
    }
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("usage: %s <device>\n", argv[0]);
        return 1;
    }
    initScreen();
    keylog(argv[1]);
    return 0;
}