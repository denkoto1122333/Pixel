#include <stdio.h>
#include <stdlib.h>
#include <conio.h> //_getch _khbit
#include <windows.h>// color , visual controll
#include <string.h> //strcat() filenames

#define WIDTH 30
#define HEIGHT 20
#define MAX_FILENAME 100

// windows console color codes
#define COLOR_BLACK 0
#define COLOR_RED 12
#define COLOR_GREEN 10
#define COLOR_YELLOW 14
#define COLOR_BLUE 9
#define COLOR_MAGENTA 13
#define COLOR_CYAN 11
#define COLOR_WHITE 15

typedef struct {  // each pixel is defined as a character and a color
    char pixel;
    int color;
} Cell;

// global variables
Cell canvas[HEIGHT][WIDTH];
int cursorX = 0;
int cursorY = 0;
int currentColor = COLOR_WHITE;
char currentPixel = 178; // brush , medium shade solid block


void initCanvas();
void clearScreen();
void setCursorPosition(int x, int y);
void setColor(int color);
void drawCanvas();
void drawUI();
void handleInput();
void saveToFile();
void loadFromFile();
void showHelp();

int main() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);// hide blinking cursor 
    
    initCanvas();
    
    clearScreen();
    printf("Welcome to Pixel Art Editor!\n");
    printf("Press any key to start (except ctrl+c)...\n");
    _getch();//waits until users press key , without getch we will have to press enter every time we issue a 
    //command to the terminal and it wouldnt work the same way
    
    while (1) {
        clearScreen();//clear   
        drawCanvas();//draw grid
        drawUI();//show help ui
    
        while (!_kbhit()) {   // wait for key press
            Sleep(10); //repeats while program is running
        }
        handleInput();
    }
    
    return 0;
}

void initCanvas() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            canvas[i][j].pixel = ' ';
            canvas[i][j].color = COLOR_BLACK;
        }
    }
}

void clearScreen() {
    system("cls");
}

void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void drawCanvas() {
    // drawing top border so the canvas is fixed
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("-");
    printf("+\n");
    
    for (int i = 0; i < HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < WIDTH; j++) {
            if (i == cursorY && j == cursorX) {
                // cursor is with inverted color to help knowing where it is 
                setColor(COLOR_BLACK | (COLOR_YELLOW << 4));
                printf("%c", currentPixel);
                setColor(COLOR_WHITE);
            } else {
                setColor(canvas[i][j].color);
                printf("%c", canvas[i][j].pixel);
                setColor(COLOR_WHITE);
            }
        }
        printf("|\n");
    }
    
    // draw bottom border
    printf("+");
    for (int i = 0; i < WIDTH; i++) printf("-");
    printf("+\n");
}

void drawUI() { // prints ui below the canvas
    printf("\nPosition: (%d, %d) | Color: ", cursorX, cursorY);
    setColor(currentColor);
    printf("███");
    setColor(COLOR_WHITE);
    printf(" | Pixel: %c\n", currentPixel);
    printf("\nControls:\n");
    printf("Arrow Keys - Move cursor | SPACE - Draw | E - Erase\n");
    printf("1-8 - Change color | B - Change brush | R - Fill Row | O - Fill Column\n");
    printf("S - Save | L - Load | C - Clear canvas | H - Help | Q - Quit\n");
}

void handleInput() {
    if (!_kbhit()) return; // only process if key was pressed
    
    int key = _getch();
    
   // arrow keys
    if (key == 224 || key == 0) {
            key = _getch();
            switch (key) {
                case 72: // up        
                    if (cursorY > 0) cursorY--;
                    break;
                case 80: // down
                    if (cursorY < HEIGHT - 1) cursorY++; 
                    break;
                case 75: // left
                    if (cursorX > 0) cursorX--;
                    break;
                case 77: // right
                    if (cursorX < WIDTH - 1) cursorX++;
                    break;
            }
        }
    // other keys
    else {
        switch (key) {
            case ' ': // draw
                canvas[cursorY][cursorX].pixel = currentPixel;
                canvas[cursorY][cursorX].color = currentColor;
                break;
            case 'e':
            case 'E': // erase
                canvas[cursorY][cursorX].pixel = ' ';
                canvas[cursorY][cursorX].color = COLOR_BLACK;
                break;
            case '1': currentColor = COLOR_BLACK; break;
            case '2': currentColor = COLOR_RED; break;
            case '3': currentColor = COLOR_GREEN; break;
            case '4': currentColor = COLOR_YELLOW; break;
            case '5': currentColor = COLOR_BLUE; break;
            case '6': currentColor = COLOR_MAGENTA; break;
            case '7': currentColor = COLOR_CYAN; break;
            case '8': currentColor = COLOR_WHITE; break;
            case 'b':
            case 'B': // change brush
                clearScreen();
                printf("Choose brush:\n");
                printf("1 - Rectangle (solid)\n");
                printf("2 - Hash (#)\n");
                printf("3 - Asterisk (*)\n");
                printf("4 - Plus (+)\n");
                printf("5 - Dot (.)\n");
                int choice = _getch();
                switch (choice) {
                    case '1': currentPixel = 178; break;  //symbol178: �  , 219 had visualizing issues and cursor issues
                    case '2': currentPixel = '#'; break;
                    case '3': currentPixel = '*'; break;
                    case '4': currentPixel = '+'; break;
                    case '5': currentPixel = '.'; break;
                }
                break;
            case 'r':
            case 'R': // fill Row
                for (int j = 0; j < WIDTH; j++) {
                    canvas[cursorY][j].pixel = currentPixel;
                    canvas[cursorY][j].color = currentColor;
                }
                break;
            case 'o':
            case 'O': // fill Column
                for (int i = 0; i < HEIGHT; i++) {
                    canvas[i][cursorX].pixel = currentPixel;
                    canvas[i][cursorX].color = currentColor;
                }
                break;
            case 's':
            case 'S': // save
                saveToFile();
                break;
            case 'l':
            case 'L': // load
                loadFromFile();
                break;
            case 'c':
            case 'C':{ // clear canvas
                clearScreen();
                printf("Clear canvas? (Y/N): ");
                char confirm = _getch();
                if (confirm == 'y' || confirm == 'Y') {
                    initCanvas();
                }
                break;
            }
            case 'h':
            case 'H': // help
                showHelp();
                break;
            case 'q':
            case 'Q': // quit
                clearScreen();
                printf("Quit without saving? (Y/N): ");
                char quit = _getch();
                if (quit == 'y' || quit == 'Y') {
                    exit(0);
                }
                break;
        }
    }
}

void saveToFile() {
    char filename[MAX_FILENAME];
    char ch;
    int i = 0;
    
    clearScreen();
    printf("Enter filename to save (without extension): ");
    
    // read character by character
    while (1) {
        ch = _getch();
        if (ch == '\r') { // enter key
            filename[i] = '\0';
            break;
        } else if (ch == '\b' && i > 0) { // backspace
            i--;
            printf("\b \b");
        } else if (ch >= 32 && ch <= 126 && i < MAX_FILENAME - 5) {
            filename[i++] = ch;
            printf("%c", ch);
        }
    }
    printf("\n");
    
    if (i == 0) {
        printf("No filename entered!\n");
        printf("Press any key to continue...");
        _getch();
        return;
    }
    
    strcat(filename, ".txt");
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error saving file!\n");
        printf("Press any key to continue...");
        _getch();
        return;
    }
    
    // save dimensions
    fprintf(file, "%d %d\n", WIDTH, HEIGHT);
    
    // save canvas data
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            fprintf(file, "%d %d ", (int)canvas[i][j].pixel, canvas[i][j].color);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("Saved successfully to %s!\n", filename);
    printf("Press any key to continue...");
    _getch();
}

void loadFromFile() {
    char filename[MAX_FILENAME];
    char ch;
    int i = 0;
    
    clearScreen();
    printf("Enter filename to load (without extension): ");
    
  
    while (1) {
        ch = _getch();
        if (ch == '\r') { // enter
            filename[i] = '\0';
            break;
        } else if (ch == '\b' && i > 0) { // bcksp
            i--;
            printf("\b \b");
        } else if (ch >= 32 && ch <= 126 && i < MAX_FILENAME - 5) {
            filename[i++] = ch;
            printf("%c", ch);
        }
    }
    printf("\n");
    
    if (i == 0) {
        printf("No filename entered!\n");
        printf("Press any key to continue..");
        _getch();
        return;
    }
    
    strcat(filename, ".txt");
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error loading file! File not found.\n");
        printf("Press any key to continue..");
        _getch();
        return;
    }
    
    int w, h;
    fscanf(file, "%d %d", &w, &h);
    
    if (w != WIDTH || h != HEIGHT) {
        printf("Warning: File dimensions don't match! Loading anyway...\n");
    }
    
   // load
    for (int i = 0; i < HEIGHT && i < h; i++) {
        for (int j = 0; j < WIDTH && j < w; j++) {
            int pixel, color;
            fscanf(file, "%d %d", &pixel, &color);
            canvas[i][j].pixel = (char)pixel;
            canvas[i][j].color = color;
        }
    }
    
    fclose(file);
    printf("Loaded successfully from %s!\n", filename);
    printf("Press any key to continue...");
    _getch();
}

void showHelp() {
    clearScreen();
    printf("=== PIXEL ART EDITOR HELP ===\n\n");
    printf("NAVIGATION:\n");
    printf("  Arrow Keys - Move the cursor around the canvas\n\n");
    printf("DRAWING:\n");
    printf("  SPACE - Draw at current position with current color/brush\n");
    printf("  E - Erase (clear) the current pixel\n\n");
    printf("COLORS (Press number to select):\n");
    printf("  1 - Black    2 - Red      3 - Green    4 - Yellow\n");
    printf("  5 - Blue     6 - Magenta  7 - Cyan     8 - White\n\n");
    printf("BRUSHES:\n");
    printf("  B - Open brush selection menu\n");
    printf("      Choose from: Block, #, *, +, .\n\n");
    printf("FILL OPERATIONS:\n");
    printf("  R - Fill entire row with current color/brush\n");
    printf("  O - Fill entire column with current color/brush\n\n");
    printf("FILE OPERATIONS:\n");
    printf("  S - Save your artwork to a file\n");
    printf("  L - Load artwork from a file\n\n");
    printf("OTHER:\n");
    printf("  C - Clear the entire canvas\n");
    printf("  H - Show this help screen\n");
    printf("  Q - Quit the program\n\n");
    printf("Press any key to return to editor...");
    _getch();
}