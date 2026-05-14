# Catch The Egg Deluxe 🎮

A fun 2D arcade-style egg catching game built using **C++**, **OpenGL**, and **GLUT**.
Control the basket, catch valuable eggs, avoid poop, and survive changing wind effects while chasing the highest score!

---

## 📌 Game Overview

**Catch The Egg Deluxe** is a lightweight OpenGL-based desktop game where chickens move across the screen and drop different types of falling objects.

The player controls a basket at the bottom of the screen and must:

* Catch eggs to earn points
* Avoid poop to prevent losing points
* Use special power-ups to increase basket size
* Fight against dynamic wind movement affecting falling objects
* Score as high as possible before the timer ends

The game includes:

* Main Menu
* Help Screen
* Pause/Resume System
* Power-ups
* High Score Tracking
* Mouse + Keyboard Controls
* Animated Objects and Wind Physics

---

# 🖼 Gameplay Features

## 🥚 Different Object Types

| Object              | Description                 | Score Effect |
| ------------------- | --------------------------- | ------------ |
| Normal Egg          | Basic collectible egg       | +1           |
| Blue Egg            | Rare bonus egg              | +5           |
| Golden Egg          | Rotating premium egg        | +10          |
| Poop                | Dangerous obstacle          | -10          |
| Big Basket Power-Up | Enlarges basket temporarily | Utility      |

---

## 🌪 Dynamic Wind System

The game includes a wind mechanic that continuously changes direction.

Effects:

* Falling objects drift left or right
* Makes catching objects harder
* Adds unpredictability to gameplay

---

## 🐔 Animated Chickens

Three chickens move horizontally across the screen at different speeds and heights.

Each chicken:

* Changes direction dynamically
* Drops random objects
* Adds movement and life to the game scene

---

# 🎮 Controls

## Keyboard Controls

| Key | Action            |
| --- | ----------------- |
| `1` | Start Game        |
| `2` | Open Help Menu    |
| `3` | Exit Game         |
| `P` | Pause Game        |
| `R` | Resume Game       |
| `M` | Return to Menu    |
| `←` | Move Basket Left  |
| `→` | Move Basket Right |

---

## Mouse Controls

| Action     | Result      |
| ---------- | ----------- |
| Move Mouse | Move Basket |

---

# 🧠 Game Workflow

```text
Application Start
        │
        ▼
Initialize OpenGL + GLUT
        │
        ▼
Display Main Menu
        │
 ┌──────┼──────┐
 ▼      ▼      ▼
Start  Help   Exit
Game   Menu
 │
 ▼
Gameplay Loop Starts
 │
 ▼
Spawn Falling Objects
 │
 ▼
Move Chickens + Wind
 │
 ▼
Update Object Positions
 │
 ▼
Collision Detection
 │
 ▼
Update Score & Power-ups
 │
 ▼
Timer Countdown
 │
 ▼
Game Over Screen
 │
 ▼
Return to Menu
```

---

# 🏗 Project Structure

Since this project is contained in a single source file, the architecture is organized into logical sections.

```text
main.cpp
│
├── ENUMS
│   ├── ObjectType
│   └── GameState
│
├── STRUCTS
│   └── FallingObject
│
├── GLOBAL VARIABLES
│
├── DRAWING FUNCTIONS
│   ├── drawText()
│   ├── drawCircle()
│   ├── drawChicken()
│   ├── drawBasket()
│   └── drawObject()
│
├── GAME SCREENS
│   ├── drawMenu()
│   ├── drawHelp()
│   ├── drawPause()
│   └── GAME OVER screen
│
├── GAME LOGIC
│   ├── spawnObject()
│   ├── checkCollision()
│   ├── update()
│   └── timer()
│
├── INPUT HANDLERS
│   ├── keyboard()
│   ├── specialKeyboard()
│   └── mouseMotion()
│
├── OPENGL INITIALIZATION
│   └── init()
│
└── MAIN FUNCTION
    └── GLUT setup + game loop
```

---

# ⚙️ Technologies Used

| Technology | Purpose                      |
| ---------- | ---------------------------- |
| C++        | Core programming language    |
| OpenGL     | Graphics rendering           |
| GLUT       | Windowing and input handling |
| STL Vector | Dynamic object storage       |

---

# 🖥 Requirements

Before running the game, install:

* C++ Compiler (`g++`)
* OpenGL
* GLUT / FreeGLUT

---

# 🚀 How To Compile & Run

---

## 🐧 Linux (Ubuntu/Debian)

### Install Dependencies

```bash
sudo apt update
sudo apt install freeglut3-dev g++
```

### Compile

```bash
g++ main.cpp -o game -lGL -lGLU -lglut
```

### Run

```bash
./game
```

---

## 🪟 Windows (CodeBlocks / MinGW)

### Requirements

Install:

* MinGW
* FreeGLUT

### Compile

```bash
g++ main.cpp -o game.exe -lfreeglut -lopengl32 -lglu32
```

### Run

```bash
game.exe
```

---

## 🍎 macOS

### Install GLUT

```bash
brew install freeglut
```

### Compile

```bash
g++ main.cpp -framework OpenGL -framework GLUT -o game
```

### Run

```bash
./game
```

---

# 🔄 Core Game Logic Explained

---

## 1️⃣ Object Spawning

Objects are randomly generated from chickens.

Probability system:

```cpp
50%  -> Normal Egg
20%  -> Blue Egg
15%  -> Golden Egg
10%  -> Poop
5%   -> Big Basket
```

---

## 2️⃣ Collision Detection

The game checks:

* If falling objects intersect with basket area
* If player catches or misses an object
* Updates score or activates power-up

---

## 3️⃣ Basket Power-Up

When collecting the green power-up:

```cpp
basketWidth = 45;
basketPowerTimer = 10;
```

Effects:

* Basket becomes larger
* Easier to catch objects
* Automatically resets after timer ends

---

## 4️⃣ Game Timer

The game lasts:

```cpp
60 seconds
```

When timer reaches zero:

* Game enters GAME_OVER state
* High score updates if exceeded

---

# 🎨 Rendering Pipeline

The game uses:

* `GLUT_DOUBLE`
* Orthographic 2D projection
* Polygon rendering
* Circle approximation using vertices
* Real-time redraw updates with timers

---

# 🧩 Important Functions

| Function           | Purpose                   |
| ------------------ | ------------------------- |
| `display()`        | Renders game scene        |
| `update()`         | Main game update loop     |
| `timer()`          | Handles countdown timer   |
| `spawnObject()`    | Generates falling objects |
| `checkCollision()` | Detects catches           |
| `keyboard()`       | Handles keyboard input    |
| `mouseMotion()`    | Mouse movement control    |

---

# 📈 Future Improvements

Possible future upgrades:

* Sound effects and background music
* Multiple levels
* Increasing difficulty
* Better animations
* Particle effects
* Save/load high scores
* Multiplayer mode
* Mobile version
* Texture-based graphics
* Pause menu UI improvements

---

# 🐞 Known Limitations

* Single source file architecture
* No external assets
* No persistent high-score saving
* Simple collision detection
* Fixed screen resolution

---

# 📚 Learning Outcomes

This project is excellent for learning:

* OpenGL basics
* GLUT event handling
* Game loops
* Collision systems
* Animation
* Real-time rendering
* Input management
* State management in games

---

# 🤝 Contributing

Contributions are welcome!

You can improve:

* Graphics
* Physics
* UI/UX
* Optimization
* Code structure
* Additional gameplay mechanics

---

# 📄 License

This project is open-source and free to use for educational purposes.

---

# 👨‍💻 Author

Developed using:

* C++
* OpenGL
* GLUT

---

# ⭐ Final Notes

**Catch The Egg Deluxe** is a beginner-friendly OpenGL game project demonstrating real-time rendering, animation, and gameplay mechanics in C++.

It is ideal for:

* Academic projects
* OpenGL practice
* Game programming beginners
* Graphics programming demonstrations

Enjoy the game and keep catching those eggs! 🥚
