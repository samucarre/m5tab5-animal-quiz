# 🐾 M5Stack Tab5 Animal Quiz Game – v0.1

An educational, touch-based quiz game for children, designed for the **M5Stack Tab5** using **UIFlow 2.3** (MicroPython).

Touch the correct animal that matches the name shown. Tracks correct and incorrect answers, shows battery status, and runs fully offline with a colorful background.

---

## 📸 Screenshots

<img src="Sources/readme/1.png" alt="Screenshot" width="250"/> <img src="Sources/readme/2.png" alt="Screenshot" width="250"/>

---

## 🧪 Version

**Current Version: v0.1**

🆕 First release for **M5Stack Tab5**:

- Full-screen vertical layout (720x1280)
- Large animal images
- Transparent-style text over background
- Touch input for 3 options
- Battery, score, and fail indicators

---

## 🖼️ Features

- Touchscreen-based animal guessing game
- 3 large images shown vertically
- Colorful illustrated background
- Battery level display
- Score tracking (✅ correct / ❌ wrong)
- All files run from `/flash`
- Works completely offline

---

## 🧠 How it works

1. The game shows a random animal name.
2. The child taps one of the three animal images.
3. If correct, it displays “Correct!” and loads a new challenge.
4. If wrong, it shows “Try again” and allows a second chance.
5. Battery level, score, and fails are always visible.

---

## 📂 File Structure

```
/flash/
├── Bear.png
├── Elephant.png
├── background.png
├── splash.png
├── ...
└── main.v.0.1.py
```

---

## 🔧 Requirements

- **M5Stack Tab5** with **UIFlow 2.3 firmware**
- MicroPython v1.25 or later
- PNG images preloaded in `/flash`

---

## 📥 Installation

### ✅ Method 1: M5Burner (Recommended)

The game is already available as firmware via **M5Burner**:

1. Open **M5Burner** on your computer.  
2. Select `tab5-animal-quiz` from the firmware list.  
3. Connect your **M5Stack Tab5** via USB.  
4. Click **Burn** to flash the firmware.  
5. The game will start automatically.

> 💡 No need to upload code or images manually — everything is embedded.

### 🛠 Method 2: Manual Upload

1. Upload `main.v.0.1.py` to `/flash/`  
2. Upload all PNG images (animals, background, splash) to `/flash/`  
3. Run the script

---

## 👶 Perfect For

- Toddlers aged 2–6
- Montessori-style activities
- Vocabulary learning (animals in English)
- Offline educational interaction

---

## 🚧 Future Plans & Collaboration

This is just the beginning!

Planned features:

- More animals and categories (jungle, sea, farm…)
- Sound-based or silhouette quizzes
- “Learn mode” before quiz
- Multi-language support
- Animated feedback

Want to help? ✨  
Contribute with images, ideas, translations or testing!

📧 **samuelcarre@mac.com**

---

## 📄 License

MIT License — free to use, learn, modify, and share.
