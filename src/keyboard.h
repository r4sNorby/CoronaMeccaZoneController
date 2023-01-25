#include <M5Stack.h>

class Keyboard
{
public:
    bool showkeyboard = false;
    char keymap[2][4][10] = {{{'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'}, {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '~'}, {'^', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ' ', '<'}, {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'}}, {{'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'}, {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '~'}, {'^', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ' ', '<'}, {'<', '>', ',', '.', '-', '_', '+', '*', '/', '\\'}}};
    int shift = 1;
    int csel = 0;
    int rsel = 0;
    int oldcsel = -1;
    int oldrsel = -1;
    int textLength = 0;
    bool pressedShift = false;
    bool deletingSecondKeystring = false;

#define KEYSTRING_BUFFER_SIZE (31)
    char keystring[KEYSTRING_BUFFER_SIZE];
    // Keystring length including string terminator
#define KEYSTRINGS_BUFFER_SIZE (17)
    char firstKeystring[KEYSTRINGS_BUFFER_SIZE];
    char secondKeystring[KEYSTRINGS_BUFFER_SIZE];

    char *start()
    {
        // Setup
        M5.Lcd.drawRect(0, 208, 106, 32, BLUE);
        M5.Lcd.drawRect(106, 208, 107, 32, BLUE);
        M5.Lcd.drawRect(213, 208, 106, 32, BLUE);
        M5.Lcd.drawString("Kolonne", 30, 215, 2); // Default first num 80
        M5.Lcd.drawString("Vaelg", 143, 215, 2);  // Default first num 80
        M5.Lcd.drawString("Raekke", 245, 215, 2); // Default first num 80

        // Pre-fill keystrings with null terminator
        memset(keystring, 0x00, 31);
        memset(secondKeystring, 0x00, 17);
        memset(firstKeystring, 0x00, 17);

        // Set cursor and update text and keyboard
        firstKeystring[0] = '|';
        updateText();
        firstKeystring[0] = 0;
        updateKeyboard();

        // Keyboard while loop
        while (showkeyboard)
        {
            if (M5.BtnA.wasPressed())
            {
                csel = csel + 1;
                if (csel > 9)
                {
                    csel = 0;
                }
                updateKeyboard();
            }
            if (M5.BtnC.wasPressed())
            {
                rsel = rsel + 1;
                if (rsel > 3)
                {
                    rsel = 0;
                }
                updateKeyboard();
            }

            if (M5.BtnB.wasPressed())
            {
                // Cursor pipe index
                int cursorPipe;

                if ((rsel == 2) && (csel == 0))
                {
                    // CAPS LOCK / SHIFT
                    pressedShift = true;
                    shift++;
                    if (shift > 1)
                        shift = 0;
                    updateKeyboard();
                }
                else if ((rsel == 1) && (csel == 9))
                {
                    // RETURN / ENTER
                    M5.Lcd.clear();
                    mergeKeystrings();
                    showkeyboard = false;
                }
                else if ((rsel == 2) && (csel == 9))
                {
                    // Backspace
                    if (textLength > 0)
                    {
                        // If more than 15 characters have been typed in
                        if (strlen(secondKeystring) > 0)
                        {
                            cursorPipe = strlen(secondKeystring) - 1;
                        }
                        else
                        // If less than 15 characters have been typed in
                        {
                            // If  the second row was just removed
                            if (strlen(secondKeystring) == 0)
                            {
                                deletingSecondKeystring = true;
                            }
                            cursorPipe = strlen(firstKeystring) - 1;
                        }
                    }
                    else
                    {
                        cursorPipe = textLength;
                    }
                }
                else
                {
                    // Max input is 30 characters
                    if (textLength < 30)
                    {
                        // If less than 15 characters have been typed in
                        if (strlen(firstKeystring) < 15)
                        {
                            firstKeystring[strlen(firstKeystring)] = keymap[shift][rsel][csel];
                            // Let go of shift after first character
                            if (textLength == 0 && shift == 1)
                            {
                                shift = 0;
                                updateKeyboard();
                            }

                            // If exactly 15 characters have been typed in
                            if (strlen(firstKeystring) == 15)
                            {
                                cursorPipe = 0;
                            }
                            else
                            {
                                cursorPipe = strlen(firstKeystring);
                            }
                        }
                        // If more than 15 characters have been typed in
                        else
                        {
                            secondKeystring[strlen(secondKeystring)] = keymap[shift][rsel][csel];
                            cursorPipe = strlen(secondKeystring);
                        }
                    }
                }

                if (!pressedShift)
                {
                    // Insert cursorPipe and update text
                    // If  the second row was just removed
                    if (deletingSecondKeystring)
                    {
                        firstKeystring[cursorPipe] = '|';
                        updateText();
                        firstKeystring[cursorPipe] = 0x00;
                    }
                    else
                    {
                        if (textLength >= 14)
                        {
                            secondKeystring[cursorPipe] = '|';
                            updateText();
                            secondKeystring[cursorPipe] = 0x00;
                        }
                        else
                        {
                            firstKeystring[cursorPipe] = '|';
                            updateText();
                            firstKeystring[cursorPipe] = 0x00;
                        }
                    }
                } else {
                    pressedShift = false;
                }
                // TextLength is calculated
                textLength = strlen(firstKeystring) + strlen(secondKeystring);
            }
            delay(1);
            M5.update();
        }

        return keystring;
    }

    void mergeKeystrings()
    {
        int j, i;
        for (i = 0; i < strlen(firstKeystring); i++)
        {
            keystring[i] = firstKeystring[i];
        }

        for (j = 0; j < strlen(secondKeystring); j++)
        {
            keystring[i] = secondKeystring[j];
            i++;
        }
    }

    void updateText()
    {
        // Write the updated text on the screen
        if (showkeyboard)
        {
            M5.Lcd.fillRect(0, 18, 320, 60, BLACK); // Clear text

            // Create pointer to char array "firstKeystring" or "secondKeystring"
            // Write new text
            // If secondKeystring was just deleted
            if (deletingSecondKeystring)
            {
                deletingSecondKeystring = false;
                char *firstText = firstKeystring;
                M5.Lcd.drawString(firstText, 0, 48, 4);
            }
            else
            {
                // "More than or equal to" half the length of the text
                if (textLength >= 14)
                {
                    char *firstText = firstKeystring;
                    M5.Lcd.drawString(firstText, 0, 18, 4);

                    char *secondText = secondKeystring;
                    M5.Lcd.drawString(secondText, 0, 48, 4);
                }
                else
                {
                    char *firstText = firstKeystring;
                    M5.Lcd.drawString(firstText, 0, 48, 4);
                }
            }
        }
    }

    void updateKeyboard()
    {
        // M5.Speaker.tone(561, 40); //frequency 3000, with a duration of 200ms
        int x, y;
        M5.Lcd.fillRect(0, 80, 320, 128, BLACK); // Default for second num was 112
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 10; c++)
            {
                x = (c * 32);
                y = (80 + (r * 32)); // Default for first num was 112
                if ((csel == c) && (rsel == r))
                {
                    M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
                    M5.Lcd.fillRect(x, y, 32, 32, WHITE);
                }
                else
                {
                    M5.Lcd.drawRect(x, y, 32, 32, BLUE);
                }

                if ((c == 9) && (r == 1))
                {
                    M5.Lcd.drawString("<-'", x + 10, y + 7, 2);
                }
                else if ((c == 8) && (r == 2))
                {
                    M5.Lcd.drawString("__", x + 7, y + 7, 2);
                }
                else if ((c == 9) && (r == 2))
                {
                    M5.Lcd.drawString("<-", x + 10, y + 7, 2);
                }
                else
                {
                    M5.Lcd.drawString(String(keymap[shift][r][c]), x + 12, y + 7, 2);
                }
                M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
            }
        }
        // oldcsel = csel;
        // oldrsel = rsel;
    }
};