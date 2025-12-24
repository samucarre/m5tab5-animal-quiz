# M5Stack Tab5 Quiz Game - v2.0

An educational, touch-based quiz game for children, designed for the **M5Stack Tab5** (ESP32-P4) using **PlatformIO** with Arduino framework.

Touch the correct image that matches the name shown. Features multiple quiz categories, TTS audio, score tracking, and runs fully offline.

---

## Screenshots

<img src="Sources/readme/1.png" alt="Screenshot" width="250"/> <img src="Sources/readme/2.png" alt="Screenshot" width="250"/>

---

## Version

**Current Version: v2.0**

New in v2.0:
- Multiple quiz categories: Animals, Fruits, Autumn, MIX
- Menu screen to select quiz type
- MIX mode combines all 82 items
- TTS audio for each item name
- Splash screen on startup
- PlatformIO/Arduino framework (was MicroPython)
- SD card for images and audio

v0.1 features retained:
- Full-screen vertical layout (720x1280)
- Large images (340x340)
- Touch input for 3 options
- Battery, score, and fail indicators

---

## Features

- 4 quiz categories (82 total items):
  - Animals (46 items)
  - Fruits (16 items)
  - Autumn objects (20 items)
  - MIX (all combined)
- Touchscreen-based guessing game
- TTS audio pronunciation
- Colorful illustrated background
- Battery level display
- Score tracking (correct / wrong)
- Works completely offline

---

## How it works

1. Select a quiz category from the menu.
2. The game shows a random item name and plays audio.
3. The child taps one of the three images.
4. If correct, it displays "CORRECT!" and loads a new challenge.
5. If wrong, it shows "Try again" and allows another chance.
6. Press BACK to return to menu.

---

## File Structure

### Project (PlatformIO)
```
platformio_quiz/
├── src/
│   └── main.cpp
├── platformio.ini
└── ...
```

### SD Card
```
/sdcard/
├── splash/
│   ├── splash.png
│   └── background.png
├── images/
│   ├── animals/     (46 PNG files)
│   ├── fruits/      (16 PNG files)
│   └── autumn/      (20 PNG files)
└── audio/
    ├── animals/     (46 WAV files)
    ├── fruits/      (16 WAV files)
    ├── autumn/      (20 WAV files)
    ├── Correct.wav
    └── TryAgain.wav
```

---

## Requirements

- **M5Stack Tab5** (ESP32-P4)
- **PlatformIO** with pioarduino platform
- SD card with images and audio files

---

## Installation

### Step 1: Flash the firmware

```bash
# Clone the repository
git clone https://github.com/samucarre/m5tab5-animal-quiz.git
cd m5tab5-animal-quiz/platformio_quiz

# Build
pio run

# Put Tab5 in download mode:
# Hold Reset button until green LED flashes rapidly, then release

# Upload
pio run -t upload
```

### Step 2: Prepare SD card

1. Format SD card as FAT32
2. Copy contents of `Sources/` to SD card root:
   - `splash/` folder
   - `images/` folder (with animals, fruits, autumn subfolders)
   - `audio/` folder (with animals, fruits, autumn subfolders)
3. Insert SD card into Tab5

### PlatformIO configuration (platformio.ini)

```ini
[env:m5tab5]
platform = https://github.com/pioarduino/platform-espressif32.git#54.03.21
framework = arduino
board = esp32-p4-evboard
board_build.mcu = esp32p4
board_build.flash_mode = qio
upload_speed = 1500000
monitor_speed = 115200

build_flags =
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DARDUINO_USB_MODE=1

lib_deps =
    https://github.com/M5Stack/M5Unified.git
    https://github.com/M5Stack/M5GFX.git
```

---

## Perfect For

- Toddlers aged 2-6
- Montessori-style activities
- Vocabulary learning (English)
- Offline educational interaction

---

## Future Plans

- More categories (vehicles, shapes, colors...)
- Multiple language support
- Learn mode before quiz
- Animated feedback

---

## License

MIT License - free to use, learn, modify, and share.
