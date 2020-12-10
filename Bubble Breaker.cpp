#include <GLFW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ctime>
#include <Windows.h>
#include <algorithm>

//int width = 512;
//int height = 512;

static unsigned int CompileShader(unsigned int type, const std::string& source){
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // TODO: Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        //std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        //std::cout << message << std::endl;
        glDeleteShader(id);

        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

#define mapW 10
#define mapH 10

GLfloat up = 0.9f;
GLfloat center = 0.8f;
GLfloat down = 1.0f;

void Show_Red_Tile() {
    glBegin(GL_QUADS);
    glColor3f(up, 0.0, 0.0); glVertex2f(0, 0);
    glColor3f(center, 0.0, 0.0); glVertex2f(1, 0);
    glColor3f(center, 0.0, 0.0); glVertex2f(1, 1);
    glColor3f(down, 0.0, 0.0); glVertex2f(0, 1);
    glEnd();
}
void Show_Green_Tile() {
    glBegin(GL_QUADS);
    glColor3f(0.0, up, 0.0); glVertex2f(0, 0);
    glColor3f(0.0, center, 0.0); glVertex2f(1, 0);
    glColor3f(0.0, center, 0.0); glVertex2f(1, 1);
    glColor3f(0.0, down, 0.0); glVertex2f(0, 1);
    glEnd();
}
void Show_Blue_Tile() {
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, up); glVertex2f(0, 0);
    glColor3f(0.0, 0.0, center); glVertex2f(1, 0);
    glColor3f(0.0, 0.0, center); glVertex2f(1, 1);
    glColor3f(0.0, 0.0, down); glVertex2f(0, 1);
    glEnd();
}
void Show_Yellow_Tile() {
    glBegin(GL_QUADS);
    glColor3f(up, up, 0.0); glVertex2f(0, 0);
    glColor3f(center, center, 0.0); glVertex2f(1, 0);
    glColor3f(center, center, 0.0); glVertex2f(1, 1);
    glColor3f(down, down, 0.0); glVertex2f(0, 1);
    glEnd();
}
void Show_Violet_Tile() {
    glBegin(GL_QUADS);
    glColor3f(up, 0.0, up); glVertex2f(0, 0);
    glColor3f(center, 0.0, center); glVertex2f(1, 0);
    glColor3f(center, 0.0, center); glVertex2f(1, 1);
    glColor3f(down, 0.0, down); glVertex2f(0, 1);
    glEnd();
}
void Show_Field_Destr() {
    glBegin(GL_QUADS);
    glColor3f(0, 0, 0); glVertex2f(0, 0);
    glColor3f(0, 0, 0); glVertex2f(1, 0);
    glColor3f(0, 0, 0); glVertex2f(1, 1);
    glColor3f(0, 0, 0); glVertex2f(0, 1);
    glEnd();
}

typedef struct {
    bool redTile;
    bool greenTile;
    bool blueTile;
    bool yellowTile;
    bool violetTile;
    bool whiteTile;
    bool destr;
} TCell;

int num2;
int num1;
int score = 0;
int sscore = 0;
int progress = 0;
int progressCap = 5;
int width = 512;
int height = 512;
TCell map[mapW][mapH];
int tiles;

BOOL Is_Cell_In_Map(int x, int y) {
    return (x >= 0) && (y >= 0) && (x < mapW) && (y < mapH);
}

// Тут мы приводим систему координат к нашим плиткам
void Screen_To_OpenGL(int x, int y, float* ox, float* oy) {
    *ox = x / (float)width * mapW;
    *oy = mapH - y / (float)height * mapH;
}

bool Is_Equal(int x1, int y1, int x2, int y2) {
    if (
        map[x1][y1].redTile == map[x2][y2].redTile
        && map[x1][y1].greenTile == map[x2][y2].greenTile
        && map[x1][y1].blueTile == map[x2][y2].blueTile
        && map[x1][y1].yellowTile == map[x2][y2].yellowTile
        && map[x1][y1].violetTile == map[x2][y2].violetTile) {
        return true;
    }
    else
        return false;
}

// Ебаная рекурсия как я её ненавижу
void Open_Tiles(int x, int y) {
    if (!Is_Cell_In_Map || map[x][y].destr) return;
    if (Is_Equal(x, y, max(1, x) - 1, y) && x != max(1, x) - 1) {
        map[x][y].destr = true;
        Open_Tiles(max(1, x) - 1, y);
    }
    if (Is_Equal(x, y, min(8, x) + 1, y) && x != min(8, x) + 1) {
        map[x][y].destr = true;
        Open_Tiles(min(8, x) + 1, y);
    }
    if (Is_Equal(x, y, x, max(1, y) - 1) && y != max(1, y) - 1) {
        map[x][y].destr = true;
        Open_Tiles(x, max(1, y) - 1);
    }
    if (Is_Equal(x, y, x, min(8, y) + 1) && y != min(8, y) + 1) {
        map[x][y].destr = true;
        Open_Tiles(x, min(8, y) + 1);
    }
}

void Game_New() {
    srand(time(NULL));
    memset(map, 0, sizeof(map));
    tiles = 100;

    for (int i = 0; i < tiles; i++) {
        int x = rand() % mapW;
        int y = rand() % mapH;

        if (map[x][y].redTile || map[x][y].greenTile || map[x][y].blueTile || map[x][y].yellowTile || map[x][y].violetTile) {
            i--;
        }
        else {
            int choise = rand() % 10 + 1;
            if (choise == 1 || choise == 2) {
                map[x][y].redTile = true;
            }
            else if (choise == 3 || choise == 4) {
                map[x][y].greenTile = true;
            }
            else if (choise == 5 || choise == 6) {
                map[x][y].blueTile = true;
            }
            else if (choise == 7 || choise == 8) {
                map[x][y].yellowTile = true;
            }
            else if (choise == 9 || choise == 10) {
                map[x][y].violetTile = true;
            }
        }
    }
}

void Game_Show() {
    glLoadIdentity();
    glScalef(2.0 / mapW, 2.0 / mapH, 1.0);
    glTranslated(-mapW * 0.5, -mapH * 0.5, 0.0);
    for (int s = 0; s < 9; s++) {
        for (int i = 0; i < mapW - 1; i++) {
            for (int j = 0; j < mapH; j++) {
                if (map[j][i].destr && !map[j][i + 1].destr) {
                    for (int k = i; k < mapW - 1; k++) {
                        std::swap(map[j][k], map[j][k + 1]);
                    }
                }
            }
        }
    }

    // Снова заполнение уничтоженных блоков разными цветами
    if (progress <= progressCap) {
        for (int j = 0; j < mapH; j++) {
            for (int i = 0; i < mapW; i++) {
                if (map[i][j].destr) {
                    map[i][j].destr = false;
                    map[i][j].redTile = false;
                    map[i][j].greenTile = false;
                    map[i][j].blueTile = false;
                    map[i][j].yellowTile = false;
                    map[i][j].violetTile = false;

                    //score += 1;
                    
                    
                    int choise = rand() % 10 + 1;
                    if (choise == 1 || choise == 2) {
                        map[i][j].redTile = true;
                        score += 1;
                    }
                    else if (choise == 3 || choise == 4) {
                        map[i][j].greenTile = true;
                        score += 1;
                    }
                    else if (choise == 5 || choise == 6) {
                        map[i][j].blueTile = true;
                        score += 1;
                    }
                    else if (choise == 7 || choise == 8) {
                        map[i][j].yellowTile = true;
                        score += 1;
                    }
                    else if (choise == 9 || choise == 10) {
                        map[i][j].violetTile = true;
                        score += 1;
                    }
                }
            }
        }
    }
    else
    {
        for (int j = 0; j < mapH; j++) {
            for (int i = 0; i < mapW; i++) {
                if (map[i][j].redTile || map[i][j].greenTile || map[i][j].blueTile || map[i][j].yellowTile || map[i][j].violetTile) {
                    map[i][j].redTile = false;
                    map[i][j].greenTile = false;
                    map[i][j].blueTile = false;
                    map[i][j].yellowTile = false;
                    map[i][j].violetTile = false;
                    map[i][j].destr = true;
                }
            }
        }
    }

    for (int j = 0; j < mapH; j++) {
        for (int i = 0; i < mapW; i++) {
            glPushMatrix();
            glTranslatef(i, j, 0);
            if (map[i][j].redTile) {
                Show_Red_Tile();
            }
            else if (map[i][j].greenTile) {
                Show_Green_Tile();
            }
            else if (map[i][j].blueTile) {
                Show_Blue_Tile();
            }
            else if (map[i][j].yellowTile) {
                Show_Yellow_Tile();
            }
            else if (map[i][j].violetTile) {
                Show_Violet_Tile();
            }
            else if (map[i][j].destr) {
                Show_Field_Destr();
            }
            glPopMatrix();
        }
    }
    sscore = score;
    num2 = sscore % 10;
    num1 = (sscore % 100) / 10;
}

void Mouse_Callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action != GLFW_PRESS) {
        POINTFLOAT pf;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        Screen_To_OpenGL(xpos, ypos, &pf.x, &pf.y);
        int x = (int)pf.x;
        int y = (int)pf.y;
        if (Is_Cell_In_Map(x, y)) {
            Open_Tiles(x, y);
            Game_Show();
            progress++;
            if (progress <= progressCap)
                std::cout << "Ход " << progress << " из " << progressCap << ". Ваш счет: " << sscore << "." << std::endl;
        }
    }
}

void Draw_END_And_SCORE() {
    glPushMatrix();

    glLineWidth(1);
    glBegin(GL_LINES);

    // E
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    glVertex2f(2.5, 9.5);
    glVertex2f(0.5, 9.5);
    glVertex2f(0.5, 7);
    glVertex2f(2.5, 7);
    glVertex2f(0.5, 7);
    glVertex2f(0.5, 4.5);
    glVertex2f(2.5, 4.5);
    glEnd();
    glPopMatrix();

    // N
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    glVertex2f(4, 4.5);
    glVertex2f(4, 9.5);
    glVertex2f(6, 4.5);
    glVertex2f(6, 9.5);
    glEnd();
    glPopMatrix();

    // E
    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    glVertex2f(7.5, 9.5);
    glVertex2f(7.5, 4.5);
    glVertex2f(9, 4.5);
    glVertex2f(9.5, 5);
    glVertex2f(9.5, 9);
    glVertex2f(9, 9.5);
    glEnd();
    glPopMatrix();

    // S
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    glVertex2f(2.7, 4);
    glVertex2f(1.7, 4);
    glVertex2f(1.7, 3.25);
    glVertex2f(2.7, 3.25);
    glVertex2f(2.7, 2.5);
    glVertex2f(1.7, 2.5);
    glEnd();
    glPopMatrix();

    // C
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    glVertex2f(4.1, 4);
    glVertex2f(3.1, 4);
    glVertex2f(3.1, 2.5);
    glVertex2f(4.1, 2.5);
    glEnd();
    glPopMatrix();

    // O
    glBegin(GL_LINE_LOOP);
    glColor3f(1, 1, 1);
    glVertex2f(4.5, 4);
    glVertex2f(4.5, 2.5);
    glVertex2f(5.5, 2.5);
    glVertex2f(5.5, 4);
    glEnd();
    glPopMatrix();

    // R
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    glVertex2f(5.9, 2.5);
    glVertex2f(5.9, 4);
    glVertex2f(6.9, 4);
    glVertex2f(6.9, 3.25);
    glVertex2f(5.9, 3.25);
    glVertex2f(6.9, 2.5);
    glEnd();
    glPopMatrix();

    // E
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    glVertex2f(8.3, 4);
    glVertex2f(7.3, 4);
    glVertex2f(7.3, 3.25);
    glVertex2f(8.3, 3.25);
    glVertex2f(7.3, 3.25);
    glVertex2f(7.3, 2.5);
    glVertex2f(8.3, 2.5);
    glEnd();
    glPopMatrix();
}

void Draw_NUMBER1(int num) {
    switch (num)
    {
    case 0:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_LOOP);
        glColor3f(1, 1, 1);
        glVertex2f(4.8, 0.5);
        glVertex2f(4.8, 2);
        glVertex2f(3.8, 2);
        glVertex2f(3.8, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 1:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glVertex2f(4.8, 0.5);
        glVertex2f(4.8, 2);
        glEnd();
        glPopMatrix();
        break;

    case 2:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(3.8, 2);
        glVertex2f(4.8, 2);
        glVertex2f(4.8, 1.5);
        glVertex2f(3.8, 0.5);
        glVertex2f(4.8, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 3:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(3.8, 2);
        glVertex2f(4.8, 2);
        glVertex2f(4.8, 1.25);
        glVertex2f(3.8, 1.25);
        glVertex2f(4.8, 1.25);
        glVertex2f(4.8, 0.5);
        glVertex2f(3.8, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 4:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(3.8, 2);
        glVertex2f(3.8, 1.25);
        glVertex2f(4.8, 1.25);
        glVertex2f(4.8, 2);
        glVertex2f(4.8, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 5:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(4.8, 2);
        glVertex2f(3.8, 2);
        glVertex2f(3.8, 1.25);
        glVertex2f(4.8, 1.25);
        glVertex2f(4.8, 0.5);
        glVertex2f(3.8, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 6:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(4.8, 2);
        glVertex2f(3.8, 2);
        glVertex2f(3.8, 0.5);
        glVertex2f(4.8, 0.5);
        glVertex2f(4.8, 1.25);
        glVertex2f(3.8, 1.25);
        glEnd();
        glPopMatrix();
        break;

    case 7:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(3.8, 2);
        glVertex2f(4.8, 2);
        glVertex2f(4.8, 1.75);
        glVertex2f(3.8, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 8:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(4.8, 2);
        glVertex2f(3.8, 2);
        glVertex2f(3.8, 0.5);
        glVertex2f(4.8, 0.5);
        glVertex2f(4.8, 2);
        glVertex2f(4.8, 1.25);
        glVertex2f(3.8, 1.25);
        glEnd();
        glPopMatrix();
        break;

    case 9:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glColor3f(1, 1, 1);
        glVertex2f(4.8, 2);
        glVertex2f(3.8, 2);
        glVertex2f(3.8, 1.25);
        glVertex2f(4.8, 1.25);
        glVertex2f(4.8, 2);
        glVertex2f(4.8, 0.5);
        glVertex2f(3.8, 0.5);
        glEnd();
        glPopMatrix();
        break;

    default:
        break;
    }
}
void Draw_NUMBER2(int num) {

    
    glColor3f(1, 1, 1);
    switch (num)
    {
    case 0:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_LOOP);
        glVertex2f(5.2, 0.5);
        glVertex2f(5.2, 2);
        glVertex2f(6.2, 2);
        glVertex2f(6.2, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 1:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINES);
        glColor3f(1, 1, 1);
        glVertex2f(6.2, 0.5);
        glVertex2f(6.2, 2);
        glEnd();
        glPopMatrix();
        break;

    case 2:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(5.2, 2);
        glVertex2f(6.2, 2);
        glVertex2f(6.2, 1.5);
        glVertex2f(5.2, 0.5);
        glVertex2f(6.2, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 3:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(5.2, 2);
        glVertex2f(6.2, 2);
        glVertex2f(6.2, 1.25);
        glVertex2f(5.2, 1.25);
        glVertex2f(6.2, 1.25);
        glVertex2f(6.2, 0.5);
        glVertex2f(5.2, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 4:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(5.2, 2);
        glVertex2f(5.2, 1.25);
        glVertex2f(6.2, 1.25);
        glVertex2f(6.2, 2);
        glVertex2f(6.2, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 5:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(6.2, 2);
        glVertex2f(5.2, 2);
        glVertex2f(5.2, 1.25);
        glVertex2f(6.2, 1.25);
        glVertex2f(6.2, 0.5);
        glVertex2f(5.2, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 6:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(6.2, 2);
        glVertex2f(5.2, 2);
        glVertex2f(5.2, 0.5);
        glVertex2f(6.2, 0.5);
        glVertex2f(6.2, 1.25);
        glVertex2f(5.2, 1.25);
        glEnd();
        glPopMatrix();
        break;

    case 7:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(5.2, 2);
        glVertex2f(6.2, 2);
        glVertex2f(6.2, 1.75);
        glVertex2f(5.2, 0.5);
        glEnd();
        glPopMatrix();
        break;

    case 8:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(6.2, 2);
        glVertex2f(5.2, 2);
        glVertex2f(5.2, 0.5);
        glVertex2f(6.2, 0.5);
        glVertex2f(6.2, 2);
        glVertex2f(6.2, 1.25);
        glVertex2f(5.2, 1.25);
        glEnd();
        glPopMatrix();
        break;

    case 9:
        glPushMatrix();
        glLineWidth(1);
        glBegin(GL_LINE_STRIP);
        glVertex2f(6.2, 2);
        glVertex2f(5.2, 2);
        glVertex2f(5.2, 1.25);
        glVertex2f(6.2, 1.25);
        glVertex2f(6.2, 2);
        glVertex2f(6.2, 0.5);
        glVertex2f(5.2, 0.5);
        glEnd();
        glPopMatrix();
        break;
    default:
        break;
    }
}

void Show_Rules() {
    std::cout << "Bubble Breaker - увлекательная казуальная игра." << std::endl;
    std::cout << "Игровое поле заполнено шариками пяти различных цветов(красный, синий, зеленый, желтый и фиолетовый), расположенными в случайном порядке." << std::endl;
    std::cout << "Игрок может выбрать два или более расположенных рядом шарика одного цвета для того, чтобы убрать их с игрового поля. Чем больше шаров будет убрано за один ход, тем больше очков будет начислено. У игрока " << progressCap << " ходов." << std::endl;
    std::cout << "Опустевшие клетки заполняются шариками сверху (шарики смещаются вниз)." << std::endl;
    std::cout << std::endl;
}

int main(void)
{
    setlocale(LC_ALL, "rus");
    GLFWwindow* window;

    // Подключаем библиотеку
    if (!glfwInit())
        return -1;

    // Создаем окно
    window = glfwCreateWindow(width, height, "Bubble Breaker", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    Game_New();
    Show_Rules();    

    // Цикл обработки окна
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSetMouseButtonCallback(window, Mouse_Callback);

        Game_Show();
        if (progress >= progressCap) {

            std::string vertexShader =
                "#version 330 core\n"
                "\n"
                "layout(location = 0) in vec2 position;\n"
                "\n"
                "void main()\n"
                "{\n"
                "   gl_Position = position;\n"
                "}\n";
                ;

            std::string fragmentShader =
                "#version 330 core\n"
                "\n"
                "out vec3 color;\n"
                "\n"
                "void main()\n"
                "{\n"
                "   color = vec3(1.0, 0.0, 0.0);\n"
                "}\n";

            unsigned int shader = CreateShader(vertexShader, fragmentShader);
            glUseProgram(shader);
            
            glClearColor(0, 0, 0, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Draw_END_And_SCORE();
            Draw_NUMBER1(num1);
            Draw_NUMBER2(num2);
            glDeleteShader(shader);
            //glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    
    

    glfwTerminate();
    return 0;
}