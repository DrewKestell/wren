#ifndef UIINPUT_H
#define UIINPUT_H

#include <d2d1_3.h>

class UIInput
{
    char inputValue[32];
    bool active;
    int locationX;
    int locationY;
    int width;
    int height;
    ID2D1SolidColorBrush* labelBrush;
    ID2D1SolidColorBrush* inputBrush;
    ID2D1SolidColorBrush* inputBorderBrush;
    ID2D1SolidColorBrush* inputValueBrush;
    ID2D1RoundedRectangleGeometry* inputGeometry;
public:
    UIInput(
        const int locationX,
        const int locationY,
        const int width,
        const int height,
        ID2D1SolidColorBrush* labelBrush,
        ID2D1SolidColorBrush* inputBrush,
        ID2D1SolidColorBrush* inputBorderBrush,
        ID2D1SolidColorBrush* inputValueBrush) :
        locationX{ locationX },
        locationY{ locationY },
        width{ width },
        height{ height },
        labelBrush{ labelBrush },
        inputBrush{ inputBrush },
        inputBorderBrush{ inputBorderBrush },
        inputValueBrush{ inputValueBrush },
        inputGeometry{ inputGeometry }
    {
        ZeroMemory(inputValue, sizeof(inputValue));
    }
    void Draw();
};

#endif