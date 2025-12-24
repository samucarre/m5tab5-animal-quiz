#include <M5Unified.h>
#include <SD_MMC.h>

// Pines SD del M5Tab5 (ESP32-P4) - SDMMC modo 4-bit
#define SD_CLK  43
#define SD_CMD  44
#define SD_D0   39
#define SD_D1   40
#define SD_D2   41
#define SD_D3   42
#define SD_POWER_CHANNEL 4

// Configuracion de pantalla (720x1280 portrait)
#define SCREEN_W 720
#define SCREEN_H 1280

// Posiciones de las 3 imagenes (vertical)
#define IMG_W 340
#define IMG_H 340
#define IMG_X 190           // Centrado: (720-340)/2
#define IMG_Y1 200
#define IMG_Y2 550          // 200 + 340 + 10
#define IMG_Y3 900          // 550 + 340 + 10

// Estados del juego
enum GameState { STATE_MENU, STATE_QUIZ };
enum QuizType { QUIZ_ANIMALS, QUIZ_FRUITS, QUIZ_AUTUMN, QUIZ_MIX };

// Datos de cada quiz
const char* animals[] = {
    "Bear", "Beaver", "Cat", "Chick", "Chicken", "Cow", "Crocodile",
    "Deer", "Dog", "Dolphin", "Donkey", "Duck", "Eagle", "Elephant",
    "Flamingo", "Fox", "Frog", "Giraffe", "Goat", "Gorilla", "Hamster",
    "Hedgehog", "Hippo", "Horse", "Lion", "Monkey", "Mouse", "Octopus",
    "Owl", "Panda", "Parrot", "Penguin", "Piglet", "Rabbit",
    "Raccoon", "Rooster", "Shark", "Sheep", "Sloth", "Snail", "Squirrel",
    "Tiger", "Turtle", "Whale", "Wolf", "Zebra"
};
const int NUM_ANIMALS = 46;

const char* fruits[] = {
    "Apple", "Apricot", "Avocado", "Banana", "Blueberry", "Fig",
    "Grape", "Grapefruit", "Lime", "Melon", "Olive", "Orange",
    "Papaya", "Peach", "Pomegranate", "Watermelon"
};
const int NUM_FRUITS = 16;

const char* autumn[] = {
    "Acorn", "Blanket", "Boot", "Candle", "Chocolate", "Cinnamon",
    "Coffee", "Cookie", "Fireplace", "Glove", "Hat", "Leaf",
    "Mushroom", "Pie", "Pinecone", "Pumpkin", "Scarf", "Sock",
    "Squash", "Sweater"
};
const int NUM_AUTUMN = 20;

// MIX: indices que apuntan a categoria + posicion
// Formato: categoria (0=animals, 1=fruits, 2=autumn) * 100 + indice
const int NUM_MIX = NUM_ANIMALS + NUM_FRUITS + NUM_AUTUMN;  // 82 total

// Estado del juego
GameState gameState = STATE_MENU;
QuizType currentQuiz = QUIZ_ANIMALS;
int correctIndex = 0;
int currentCorrectItem = 0;
int options[3];
int score = 0;
int fails = 0;
bool sdReady = false;

// Punteros al quiz actual
const char** currentItems = animals;
int currentNumItems = NUM_ANIMALS;
const char* currentCategory = "animals";
const char* currentTitle = "Animals";

// Posiciones de botones del menu (4 botones)
#define BTN_W 300
#define BTN_H 150
#define BTN_X 210  // Centrado: (720-300)/2
#define BTN_Y1 200
#define BTN_Y2 400
#define BTN_Y3 600
#define BTN_Y4 800

// Para MIX: obtener categoria y nombre
void getMixItem(int mixIndex, const char** name, const char** category) {
    if (mixIndex < NUM_ANIMALS) {
        *name = animals[mixIndex];
        *category = "animals";
    } else if (mixIndex < NUM_ANIMALS + NUM_FRUITS) {
        *name = fruits[mixIndex - NUM_ANIMALS];
        *category = "fruits";
    } else {
        *name = autumn[mixIndex - NUM_ANIMALS - NUM_FRUITS];
        *category = "autumn";
    }
}

void setQuiz(QuizType quiz) {
    currentQuiz = quiz;
    score = 0;
    fails = 0;

    switch(quiz) {
        case QUIZ_ANIMALS:
            currentItems = animals;
            currentNumItems = NUM_ANIMALS;
            currentCategory = "animals";
            currentTitle = "Animals";
            break;
        case QUIZ_FRUITS:
            currentItems = fruits;
            currentNumItems = NUM_FRUITS;
            currentCategory = "fruits";
            currentTitle = "Fruits";
            break;
        case QUIZ_AUTUMN:
            currentItems = autumn;
            currentNumItems = NUM_AUTUMN;
            currentCategory = "autumn";
            currentTitle = "Autumn";
            break;
        case QUIZ_MIX:
            currentItems = nullptr;  // Usaremos getMixItem
            currentNumItems = NUM_MIX;
            currentCategory = "mix";
            currentTitle = "Mix";
            break;
    }
}

bool initSD() {
    if (!SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3)) {
        return false;
    }
#ifdef SOC_SDMMC_IO_POWER_EXTERNAL
    if (!SD_MMC.setPowerChannel(SD_POWER_CHANNEL)) {
        return false;
    }
#endif
    delay(100);
    return SD_MMC.begin("/sdcard", false, false, SDMMC_FREQ_DEFAULT);
}

void shuffle(int* arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = random(i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void drawBackground() {
    File file = SD_MMC.open("/splash/background.png");
    if (file) {
        size_t size = file.size();
        uint8_t* buf = (uint8_t*)heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!buf) buf = (uint8_t*)malloc(size);
        if (buf) {
            file.read(buf, size);
            file.close();
            M5.Display.drawPng(buf, size, 0, 0);
            free(buf);
            return;
        }
        file.close();
    }
    M5.Display.fillScreen(TFT_LIGHTGREY);
}

void drawMenuButton(int x, int y, int w, int h, const char* text, uint32_t color) {
    M5.Display.fillRoundRect(x, y, w, h, 20, color);
    M5.Display.drawRoundRect(x, y, w, h, 20, TFT_BLACK);
    M5.Display.drawRoundRect(x+1, y+1, w-2, h-2, 19, TFT_BLACK);

    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(4);
    int textW = strlen(text) * 24;
    M5.Display.setCursor(x + (w - textW) / 2, y + (h - 32) / 2);
    M5.Display.print(text);
}

void showMenu() {
    gameState = STATE_MENU;
    drawBackground();

    // Titulo
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextSize(5);
    M5.Display.setCursor(150, 80);
    M5.Display.print("SELECT QUIZ");

    // Bateria
    int battery = M5.Power.getBatteryLevel();
    M5.Display.setTextSize(3);
    M5.Display.setCursor(550, 10);
    M5.Display.printf("BAT:%d%%", battery);

    // Botones de seleccion (4 botones)
    drawMenuButton(BTN_X, BTN_Y1, BTN_W, BTN_H, "Animals", TFT_DARKGREEN);
    drawMenuButton(BTN_X, BTN_Y2, BTN_W, BTN_H, "Fruits", TFT_ORANGE);
    drawMenuButton(BTN_X, BTN_Y3, BTN_W, BTN_H, "Autumn", TFT_BROWN);
    drawMenuButton(BTN_X, BTN_Y4, BTN_W, BTN_H, "MIX", TFT_PURPLE);

    // Info
    M5.Display.setTextColor(TFT_DARKGREY);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(220, 1000);
    M5.Display.print("Touch to select");

    // Limpiar toques
    delay(200);
    while (M5.Touch.getCount() > 0) {
        M5.update();
        delay(10);
    }
}

int getMenuSelection(int x, int y) {
    if (x >= BTN_X && x <= BTN_X + BTN_W) {
        if (y >= BTN_Y1 && y <= BTN_Y1 + BTN_H) return 0;  // Animals
        if (y >= BTN_Y2 && y <= BTN_Y2 + BTN_H) return 1;  // Fruits
        if (y >= BTN_Y3 && y <= BTN_Y3 + BTN_H) return 2;  // Autumn
        if (y >= BTN_Y4 && y <= BTN_Y4 + BTN_H) return 3;  // MIX
    }
    return -1;
}

void playSoundForItem(int itemIndex) {
    char path[80];

    if (currentQuiz == QUIZ_MIX) {
        const char* name;
        const char* cat;
        getMixItem(itemIndex, &name, &cat);
        snprintf(path, sizeof(path), "/audio/%s/%s.wav", cat, name);
    } else {
        snprintf(path, sizeof(path), "/audio/%s/%s.wav", currentCategory, currentItems[itemIndex]);
    }

    File file = SD_MMC.open(path);
    if (file) {
        size_t size = file.size();
        uint8_t* buf = (uint8_t*)heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!buf) buf = (uint8_t*)malloc(size);
        if (buf) {
            file.read(buf, size);
            file.close();
            M5.Speaker.playWav(buf, size);
            free(buf);
            return;
        }
        file.close();
    }
}

void playResultSound(bool correct) {
    const char* path = correct ? "/audio/Correct.wav" : "/audio/TryAgain.wav";
    File file = SD_MMC.open(path);
    if (file) {
        size_t size = file.size();
        uint8_t* buf = (uint8_t*)heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!buf) buf = (uint8_t*)malloc(size);
        if (buf) {
            file.read(buf, size);
            file.close();
            M5.Speaker.playWav(buf, size);
            free(buf);
            return;
        }
        file.close();
    }
}

void drawImageForItem(int itemIndex, int x, int y) {
    char path[80];

    if (currentQuiz == QUIZ_MIX) {
        const char* name;
        const char* cat;
        getMixItem(itemIndex, &name, &cat);
        snprintf(path, sizeof(path), "/images/%s/%s.png", cat, name);
    } else {
        snprintf(path, sizeof(path), "/images/%s/%s.png", currentCategory, currentItems[itemIndex]);
    }

    File file = SD_MMC.open(path);
    if (file) {
        size_t size = file.size();
        uint8_t* buf = (uint8_t*)heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!buf) buf = (uint8_t*)malloc(size);
        if (buf) {
            file.read(buf, size);
            file.close();
            M5.Display.drawPng(buf, size, x, y);
            free(buf);
        } else {
            file.close();
        }
    }
}

const char* getItemName(int itemIndex) {
    if (currentQuiz == QUIZ_MIX) {
        const char* name;
        const char* cat;
        getMixItem(itemIndex, &name, &cat);
        return name;
    }
    return currentItems[itemIndex];
}

void drawBackButton() {
    // Boton BACK abajo a la izquierda
    M5.Display.fillRoundRect(10, 1220, 120, 50, 10, TFT_DARKGREY);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(30, 1230);
    M5.Display.print("BACK");
}

void showQuestion() {
    gameState = STATE_QUIZ;

    // Seleccionar item correcto
    int correct = random(currentNumItems);
    currentCorrectItem = correct;

    // Seleccionar 2 incorrectos diferentes
    int wrong1, wrong2;
    do { wrong1 = random(currentNumItems); } while (wrong1 == correct);
    do { wrong2 = random(currentNumItems); } while (wrong2 == correct || wrong2 == wrong1);

    // Mezclar posiciones
    options[0] = correct;
    options[1] = wrong1;
    options[2] = wrong2;
    shuffle(options, 3);

    // Encontrar donde quedo el correcto
    for (int i = 0; i < 3; i++) {
        if (options[i] == correct) {
            correctIndex = i;
            break;
        }
    }

    // Dibujar fondo
    drawBackground();

    // Mostrar pregunta
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextSize(4);
    M5.Display.setCursor(50, 15);
    M5.Display.print("Which one is the...");

    // Nombre del item
    M5.Display.setTextSize(7);
    M5.Display.setCursor(50, 70);
    M5.Display.setTextColor(TFT_BLUE);
    M5.Display.print(getItemName(correct));

    // Info arriba a la derecha
    int battery = M5.Power.getBatteryLevel();
    M5.Display.setTextSize(3);
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setCursor(550, 5);
    M5.Display.printf("BAT:%d%%", battery);

    // Puntuacion
    M5.Display.setTextColor(TFT_DARKGREEN);
    M5.Display.setCursor(550, 40);
    M5.Display.printf("Score:%d", score);
    M5.Display.setTextColor(TFT_RED);
    M5.Display.setCursor(550, 75);
    M5.Display.printf("Fails:%d", fails);

    // Boton BACK abajo a la izquierda
    drawBackButton();

    // Dibujar las 3 imagenes
    drawImageForItem(options[0], IMG_X, IMG_Y1);
    drawImageForItem(options[1], IMG_X, IMG_Y2);
    drawImageForItem(options[2], IMG_X, IMG_Y3);

    // Reproducir nombre
    playSoundForItem(correct);

    // Limpiar toques
    delay(200);
    while (M5.Touch.getCount() > 0) {
        M5.update();
        delay(10);
    }
}

void redrawScreen() {
    drawBackground();

    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setTextSize(4);
    M5.Display.setCursor(50, 15);
    M5.Display.print("Which one is the...");

    M5.Display.setTextSize(7);
    M5.Display.setCursor(50, 70);
    M5.Display.setTextColor(TFT_BLUE);
    M5.Display.print(getItemName(currentCorrectItem));

    int battery = M5.Power.getBatteryLevel();
    M5.Display.setTextSize(3);
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.setCursor(550, 5);
    M5.Display.printf("BAT:%d%%", battery);

    M5.Display.setTextColor(TFT_DARKGREEN);
    M5.Display.setCursor(550, 40);
    M5.Display.printf("Score:%d", score);
    M5.Display.setTextColor(TFT_RED);
    M5.Display.setCursor(550, 75);
    M5.Display.printf("Fails:%d", fails);

    // Boton BACK abajo a la izquierda
    drawBackButton();

    drawImageForItem(options[0], IMG_X, IMG_Y1);
    drawImageForItem(options[1], IMG_X, IMG_Y2);
    drawImageForItem(options[2], IMG_X, IMG_Y3);
}

void showResult(bool correct) {
    M5.Display.setTextSize(5);
    M5.Display.setCursor(180, 140);
    if (correct) {
        M5.Display.setTextColor(TFT_GREEN);
        M5.Display.print("CORRECT!");
        score++;
    } else {
        M5.Display.setTextColor(TFT_RED);
        M5.Display.print("Try again!");
        fails++;
    }

    playResultSound(correct);
    delay(1500);

    if (!correct) {
        redrawScreen();
    }
}

int getTouchedOption(int x, int y) {
    if (x >= IMG_X && x <= IMG_X + IMG_W) {
        if (y >= IMG_Y1 && y <= IMG_Y1 + IMG_H) return 0;
        if (y >= IMG_Y2 && y <= IMG_Y2 + IMG_H) return 1;
        if (y >= IMG_Y3 && y <= IMG_Y3 + IMG_H) return 2;
    }
    return -1;
}

bool isBackButtonPressed(int x, int y) {
    // BACK está en (10, 1220) con tamaño 120x50
    return (x >= 10 && x <= 130 && y >= 1220 && y <= 1270);
}

void waitForRelease() {
    while (true) {
        M5.update();
        if (M5.Touch.getCount() == 0) break;
        delay(10);
    }
    delay(100);
}

void showSplash() {
    // Intentar cargar splash desde SD
    File file = SD_MMC.open("/splash/splash.png");
    if (file) {
        size_t size = file.size();
        uint8_t* buf = (uint8_t*)heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!buf) buf = (uint8_t*)malloc(size);
        if (buf) {
            file.read(buf, size);
            file.close();
            M5.Display.drawPng(buf, size, 0, 0);
            free(buf);

            // Mostrar version
            M5.Display.setTextColor(TFT_WHITE);
            M5.Display.setTextSize(3);
            M5.Display.setCursor(300, 1200);
            M5.Display.print("v2.0");
            return;
        }
        file.close();
    }

    // Fallback si no hay splash
    M5.Display.fillScreen(TFT_NAVY);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(6);
    M5.Display.setCursor(200, 500);
    M5.Display.print("QUIZ");
    M5.Display.setTextSize(3);
    M5.Display.setCursor(250, 600);
    M5.Display.print("v2.0");
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200);

    M5.Speaker.begin();
    M5.Speaker.setVolume(255);

    randomSeed(analogRead(0) + millis());

    M5.Display.setRotation(0);
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.setTextColor(TFT_WHITE);
    M5.Display.setTextSize(3);
    M5.Display.setCursor(250, 620);
    M5.Display.print("Loading...");

    if (initSD()) {
        sdReady = true;
        showSplash();
        delay(2000);
        showMenu();
    } else {
        M5.Display.fillScreen(TFT_BLACK);
        M5.Display.setCursor(150, 600);
        M5.Display.setTextColor(TFT_RED);
        M5.Display.setTextSize(4);
        M5.Display.print("SD Card ERROR!");
    }
}

void loop() {
    if (!sdReady) {
        delay(100);
        return;
    }

    M5.update();

    if (M5.Touch.getCount() > 0) {
        auto touch = M5.Touch.getDetail();

        if (!touch.wasPressed()) {
            delay(10);
            return;
        }

        if (gameState == STATE_MENU) {
            int selection = getMenuSelection(touch.x, touch.y);
            if (selection >= 0) {
                waitForRelease();
                switch(selection) {
                    case 0: setQuiz(QUIZ_ANIMALS); break;
                    case 1: setQuiz(QUIZ_FRUITS); break;
                    case 2: setQuiz(QUIZ_AUTUMN); break;
                    case 3: setQuiz(QUIZ_MIX); break;
                }
                showQuestion();
            }
        } else if (gameState == STATE_QUIZ) {
            // Verificar boton BACK
            if (isBackButtonPressed(touch.x, touch.y)) {
                waitForRelease();
                showMenu();
                return;
            }

            int option = getTouchedOption(touch.x, touch.y);
            if (option >= 0) {
                bool isCorrect = (option == correctIndex);
                showResult(isCorrect);
                waitForRelease();
                if (isCorrect) {
                    showQuestion();
                }
                waitForRelease();
            }
        }
    }

    delay(10);
}
