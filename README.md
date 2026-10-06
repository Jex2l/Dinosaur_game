# 🦖 Dinosaur Game (16x2 LCD edition)

An Arduino take on the Chrome "offline dino" runner, played entirely on a
16x2 character LCD. Press a button to jump cacti scrolling in from the
right; the game speeds up the longer you survive.

## How to play

1. Power on — the LCD shows a title screen ("Dino Game / Btn = jump/start").
2. Press the button to start.
3. Cacti scroll toward the dino from the right edge of the display.
4. Press the button to jump over an obstacle in the dino's column. Miss, and
   it's game over — your score (ticks survived) is shown on the LCD.
5. Press the button again to play another round.

## Hardware

| Component | Notes |
|---|---|
| Arduino Uno / Nano (or compatible) | |
| 16x2 LCD with I2C backpack | address usually `0x27` or `0x3F` |
| Momentary push button | jump input |
| Passive buzzer (optional) | game-over sound |

**Wiring:**

| Part | Arduino pin |
|---|---|
| Push button leg A | D2 |
| Push button leg B | GND |
| Buzzer `+` (optional) | D8 |
| Buzzer `−` (optional) | GND |

The button uses the Arduino's internal pull-up resistor (`INPUT_PULLUP`), so
no external resistor is needed — just the button between the pin and GND.

**LCD wiring:**

| LCD (I2C) | Arduino (Uno/Nano) |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

## Setup

1. Wire the circuit as above.
2. Open [`dinosaur_game.ino`](dinosaur_game.ino) in the Arduino IDE.
3. Install the **LiquidCrystal I2C** library (Library Manager → search
   "LiquidCrystal I2C").
4. If the display stays blank, run an I2C scanner sketch to confirm the
   backpack's address and update `LiquidCrystal_I2C lcd(0x27, 16, 2);`
   accordingly.
5. If you don't have a buzzer wired up, set `BUZZER_PIN` to `-1` to disable
   the game-over sound.
6. Upload and play.

## How the game is implemented

- The "ground" is a 16-character array, one slot per LCD column; each tick
  it's shifted left by one and a new obstacle is randomly spawned (or not)
  at the rightmost column.
- The dino's column is fixed; jumping moves it to the top row for a fixed
  duration instead of moving it horizontally — matching how the real Chrome
  dino game plays.
- Collision is checked once per tick, before scrolling, against whatever
  landed in the dino's column.
- `tickIntervalMs` shrinks by 1ms every tick (down to a floor), so the game
  gradually speeds up as your score increases.
- The dino, its running-animation frame, and the cactus are custom LCD
  characters (`createChar`), not plain ASCII.

## Status

⚠️ This project previously shipped with the wrong source file entirely — the
`.ino` was a stray, mislabeled copy of the windmill voltage-monitor sketch
and had nothing to do with a dinosaur game. This version replaces it with an
actual, from-scratch implementation. It has been written and reasoned
through carefully but **has not been flashed to physical hardware** — double
check wiring and the LCD's I2C address before relying on it as-is, and treat
the custom character art as a starting point to tweak to taste.
