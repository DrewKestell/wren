#ifndef UIBUTTON_H
#define UIBUTTON_H

#include <d2d1_3.h>

class UIButton
{
    const char* buttonText[32];
    int locationX;
    int locationY;
    int width;
    int height;
    bool pressed;
    ID2D1SolidColorBrush* buttonBrush;
    ID2D1SolidColorBrush* buttonBorderBrush;
    ID2D1SolidColorBrush* buttonTextBrush;
    ID2D1RoundedRectangleGeometry* buttonGeometry;
public:
    void Draw();
};

#endif