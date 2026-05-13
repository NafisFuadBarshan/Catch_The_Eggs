# 🥚 Catch the Eggs 

## CSE 426 – Computer Graphics Lab  
### Spring 2025 Term Project  
**University of Asia Pacific (UAP)**

---

# 📌 Project Overview

**Catch the Eggs** is a 2D OpenGL-based arcade game developed using **C++** and **GLUT/freeglut**.  
The player controls a basket and catches various falling eggs while avoiding harmful objects.

The project demonstrates:

- 2D Computer Graphics using OpenGL
- Animation and object movement
- Collision detection
- Event handling (keyboard & mouse)
- Particle effects and UI rendering
- Game state management

---

# 🎮 Gameplay

The player must catch eggs dropped by hens that are moving.

Different objects provide different scores and effects:

| Object | Effect |
|---|---|
| 🥚 White Egg | +1 Point |
| 🔵 Blue Egg | +5 Points |
| 🟡 Golden Egg | +10 Points |
| 💩 Poop | −10 Points |
| 🟩 Green Block | Bigger Basket |
| 🟦 Blue Block | Slow Falling Eggs |
| 🟨 Gold Block | +15 Extra Seconds |

The game lasts for **2 minutes**.

---

# ✨ Features

## Core Features
- Smooth basket movement
- Mouse and keyboard controls
- Multiple egg types
- Collision detection
- Score system
- Countdown timer
- Pause and resume system
- Main menu and help screen
- Game over screen
- High score tracking

---

## Visual Features
- Gradient sky background
- Animated hens
- Clouds and grass
- Particle explosion effects
- Popup score labels
- Screen flash effects
- Animated airflow/wind effect
- Styled buttons and HUD

---

## Bonus Features
- Power-up system
- Random airflow mechanics
- Bigger basket perk
- Slow-motion eggs perk
- Time extension perk

---

# 🛠 Technologies Used

- **C++**
- **OpenGL**
- **GLUT / freeglut**

---

# 📂 Project Structure

```text
catch_the_eggs.cpp   → Main source code
README.md            → Project documentation
