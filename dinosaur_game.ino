/*
 * Dinosaur Game (16x2 LCD edition)
 *
 * An Arduino take on the Chrome "offline dino" runner, played on a 16x2
 * character LCD. Press the button to jump over cacti scrolling in from the
 * right; survive as long as you can. Speed ramps up with your score.
 *
 * Wiring:
 *   Push button -> one leg to Arduino pin 2, the other leg to GND
 *                  (uses the internal pull-up, no external resistor needed)
 *   Buzzer (optional) -> + to pin 8, - to GND
 *
 *   LCD (16x2, I2C backpack):
 *     VCC -> 5V, GND -> GND, SDA -> A4, SCL -> A5 (Uno/Nano)
 *
 * NOTE: this sketch has been written and reviewed for correctness but not
 * flashed to physical hardware - double-check wiring and the LCD's I2C
 * address (0x27 or 0x3F) before relying on it as-is.
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int JUMP_BUTTON_PIN = 2;
const int BUZZER_PIN = 8; // set to -1 if you don't have a buzzer wired up

const int COLUMNS = 16;
const int DINO_COLUMN = 1;
const int GROUND_ROW = 1;
const int AIR_ROW = 0;

const byte CHAR_DINO = 1;
const byte CHAR_DINO_RUN = 2;
const byte CHAR_CACTUS = 3;

byte dinoGlyph[8] = {
  B00011,
  B00111,
  B00110,
  B01110,
  B11111,
  B01110,
  B01010,
  B00000
};

byte dinoRunGlyph[8] = {
  B00011,
  B00111,
  B00110,
  B01110,
  B11111,
  B01010,
  B01001,
  B00000
};

byte cactusGlyph[8] = {
  B00100,
  B00100,
  B10101,
  B10101,
  B01110,
  B00100,
  B00100,
  B00000
};

char ground[COLUMNS + 1]; // 'X' = obstacle, ' ' = empty, one slot per column

bool isJumping = false;
unsigned long jumpStartedAt = 0;
const unsigned long JUMP_DURATION_MS = 400;

unsigned long lastTickAt = 0;
unsigned long tickIntervalMs = 220; // shrinks as score grows -> game speeds up
const unsigned long MIN_TICK_INTERVAL_MS = 90;

unsigned long score = 0;
bool gameOver = true; // starts on the title screen
bool animFrame = false;

int lastButtonState = HIGH;
unsigned long lastButtonChangeAt = 0;
const unsigned long DEBOUNCE_MS = 30;

void setup() {
  pinMode(JUMP_BUTTON_PIN, INPUT_PULLUP);
  if (BUZZER_PIN >= 0) pinMode(BUZZER_PIN, OUTPUT);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.createChar(CHAR_DINO, dinoGlyph);
  lcd.createChar(CHAR_DINO_RUN, dinoRunGlyph);
  lcd.createChar(CHAR_CACTUS, cactusGlyph);

  randomSeed(analogRead(A0));
  showTitleScreen();
}

void loop() {
  bool jumpPressed = readJumpButtonEdge();

  if (gameOver) {
    if (jumpPressed) resetGame();
    return;
  }

  if (jumpPressed && !isJumping) {
    isJumping = true;
    jumpStartedAt = millis();
  }

  if (isJumping && millis() - jumpStartedAt >= JUMP_DURATION_MS) {
    isJumping = false;
  }

  if (millis() - lastTickAt >= tickIntervalMs) {
    lastTickAt = millis();
    tick();
  }
}

// Returns true exactly once per debounced button press (falling edge).
bool readJumpButtonEdge() {
  int state = digitalRead(JUMP_BUTTON_PIN);
  bool pressedEdge = false;

  if (state != lastButtonState && millis() - lastButtonChangeAt > DEBOUNCE_MS) {
    lastButtonChangeAt = millis();
    if (state == LOW) pressedEdge = true;
    lastButtonState = state;
  }

  return pressedEdge;
}

void tick() {
  // Check collision against the current frame before scrolling the world.
  if (ground[DINO_COLUMN] == 'X' && !isJumping) {
    endGame();
    return;
  }

  scrollGround();

  score++;
  if (tickIntervalMs > MIN_TICK_INTERVAL_MS) {
    tickIntervalMs -= 1;
  }

  animFrame = !animFrame;
  render();
}

void scrollGround() {
  memmove(ground, ground + 1, COLUMNS - 1);

  bool spawnObstacle = random(100) < 18;
  ground[COLUMNS - 1] = spawnObstacle ? 'X' : ' ';

  // Never stack two obstacles back-to-back - there'd be no way to react.
  if (ground[COLUMNS - 1] == 'X' && ground[COLUMNS - 2] == 'X') {
    ground[COLUMNS - 1] = ' ';
  }
}

void render() {
  lcd.setCursor(0, AIR_ROW);
  for (int col = 0; col < COLUMNS; col++) {
    if (col == DINO_COLUMN && isJumping) {
      lcd.write(CHAR_DINO);
    } else {
      lcd.print(' ');
    }
  }

  lcd.setCursor(0, GROUND_ROW);
  for (int col = 0; col < COLUMNS; col++) {
    if (col == DINO_COLUMN && !isJumping) {
      lcd.write(animFrame ? CHAR_DINO : CHAR_DINO_RUN);
    } else if (ground[col] == 'X') {
      lcd.write(CHAR_CACTUS);
    } else {
      lcd.print(' ');
    }
  }
}

void endGame() {
  gameOver = true;

  if (BUZZER_PIN >= 0) {
    tone(BUZZER_PIN, 220, 150);
    delay(180);
    tone(BUZZER_PIN, 110, 250);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

void resetGame() {
  memset(ground, ' ', COLUMNS);
  ground[COLUMNS] = '\0';
  isJumping = false;
  score = 0;
  tickIntervalMs = 220;
  gameOver = false;
  lastTickAt = millis();
  lcd.clear();
}

void showTitleScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Dino Game");
  lcd.setCursor(0, 1);
  lcd.print("Btn = jump/start");

  while (digitalRead(JUMP_BUTTON_PIN) == HIGH) {
    // Wait for the first press before the game begins.
  }
}
