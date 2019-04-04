//
// glDemo.h - definitions header file for glDemo.c
//

void RenderScene(void);

#define KEY_UP(key)   ((GetAsyncKeyState(key)&0x8000)? 0 : 1)
#define KEY_DOWN(key) ((GetAsyncKeyState(key)&0x8000)? 1 : 0)

LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
void LoadTexture(void);
void LoadMyCursor(void);

struct zz_color
{
        float red;
        float green;
        float blue;
        float alpha;
};
