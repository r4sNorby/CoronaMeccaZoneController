#include <M5Stack.h>

class Keyboard
{
public:
    bool showkeyboard = false;
    char keymap[2][4][10] = {{{'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'}, {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ' '}, {'z', 'x', 'c', 'v', 'b', 'n', 'm', '^', '<', '!'}, {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'}}, {{'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'}, {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ' '}, {'Z', 'X', 'C', 'V', 'B', 'N', 'M', '^', '<', ' '}, {'<', '>', ',', '.', '-', '_', '+', '*', '/', '\\'}}};
    int shift = 0;
    int csel = 0;
    int rsel = 0;
    int ocsel = -1;
    int orsel = -1;

#define KEYSTRING_BUFFER_SIZE (15)
    char keystring[KEYSTRING_BUFFER_SIZE];
    char oldkeystring[KEYSTRING_BUFFER_SIZE];

    void update()
    {
        if ((showkeyboard) && ((csel != ocsel) || (rsel != orsel) || (keystring != oldkeystring)))
        {
            // M5.Speaker.tone(561, 40); //frequency 3000, with a duration of 200ms

            M5.Lcd.fillRect(0, 80, 320, 30, BLACK);
            // M5.Lcd.drawString("                                                     ", 0, 80, 4);
            M5.Lcd.drawString(keystring, 0, 80, 4);

            int x, y;
            M5.Lcd.fillRect(0, 112, 320, 240, BLACK);
            for (int r = 0; r < 4; r++)
            {
                for (int c = 0; c < 10; c++)
                {
                    x = (c * 32);
                    y = (112 + (r * 32));
                    if ((csel == c) && (rsel == r))
                    {
                        M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
                        M5.Lcd.fillRect(x, y, 32, 32, WHITE);
                    }
                    else
                    {
                        M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
                        M5.Lcd.drawRect(x, y, 32, 32, BLUE);
                    }
                    M5.Lcd.drawString(String(keymap[shift][r][c]), x + 10, y + 7, 2);
                }
            }
            ocsel = csel;
            orsel = rsel;
        }
    }

    char *keyboard()
    {
        while (showkeyboard)
        {
            if (M5.BtnA.wasReleased())
            {
                csel = csel + 1;
                if (csel > 9)
                {
                    csel = 0;
                }
                update();
            }
            if (M5.BtnC.wasReleased())
            {
                rsel = rsel + 1;
                if (rsel > 3)
                {
                    rsel = 0;
                }
                update();
            }

            if (M5.BtnB.pressedFor(1000))
            {
                M5.Lcd.clear();
                showkeyboard = false;
            }
            else if (M5.BtnB.wasReleased())
            {
                if ((rsel == 2) && (csel == 7))
                {
                    shift++;
                    if (shift > 1)
                        shift = 0;

                    csel = 0;
                    rsel = 0;
                }
                else if ((rsel == 2) && (csel == 8))
                {
                    int len = strlen(keystring);
                    keystring[len - 1] = 0;
                }
                else
                    keystring[keymap[shift][rsel][csel]];
                update();
            }
            strcpy(oldkeystring, keystring);
            delay(1);
            M5.update();
        }
        return keystring;
    }
};