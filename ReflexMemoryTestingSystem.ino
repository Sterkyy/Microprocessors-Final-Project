#include <Wire.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Buttons
const int redButton = 2;
const int blueButton = 4;
const int greenButton = 7;

// LEDs
const int redLED = 8;
const int blueLED = 9;
const int greenLED = 10;

const int buttonPins[3] = {redButton, blueButton, greenButton};
const int ledPins[3] = {redLED, blueLED, greenLED};

const char* colorNames[3] = {"RED", "BLUE", "GREEN"};

int menuIndex = 0;
int difficulty = 1; // 0 Easy, 1 Normal, 2 Hard

unsigned long bestReactionTimes[3] = {99999, 99999, 99999};
unsigned long reactionTime = 0;

const int reactionAttempts = 5;

const char* menuItems[] = {
  "Reaction Test",
  "Sequence Game",
  "Difficulty",
  "Best Time"
};

const int menuCount = 4;

const char* difficultyNames[] = {
  "Easy",
  "Normal",
  "Hard"
};

void setup() {
  pinMode(redButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);

  pinMode(redLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  turnOffLEDs();

  randomSeed(analogRead(A0));

  display.begin();
  display.setBusClock(100000);

  showMenu();
}

void loop() {
  if (digitalRead(blueButton) == LOW) {
    menuIndex--;
    if (menuIndex < 0) menuIndex = menuCount - 1;
    showMenu();
    waitForAllRelease();
  }

  if (digitalRead(greenButton) == LOW) {
    menuIndex++;
    if (menuIndex >= menuCount) menuIndex = 0;
    showMenu();
    waitForAllRelease();
  }

  if (digitalRead(redButton) == LOW) {
    waitForAllRelease();

    if (menuIndex == 0) {
      startReactionTest();
    } 
    else if (menuIndex == 1) {
      startSequenceGame();
    } 
    else if (menuIndex == 2) {
      changeDifficulty();
    } 
    else if (menuIndex == 3) {
      showBestTime();
    }

    showMenu();
  }
}

// =========================
// REACTION TEST
// =========================

void startReactionTest() {
  unsigned long totalTime = 0;
  unsigned long runBest = 99999;
  int correctAttempts = 0;

  for (int attempt = 1; attempt <= reactionAttempts; attempt++) {
    char text[25];
    sprintf(text, "Attempt %d/%d", attempt, reactionAttempts);
    showScreen(text, "Get Ready");
    delay(1000);

    bool validAttempt = runSingleReactionAttempt();

    if (validAttempt) {
      correctAttempts++;
      totalTime += reactionTime;

      if (reactionTime < runBest) runBest = reactionTime;

      if (reactionTime < bestReactionTimes[difficulty]) {
        bestReactionTimes[difficulty] = reactionTime;
      }
    }

    delay(1200);
  }

  showReactionSummary(correctAttempts, totalTime, runBest);
  delay(5000);
}

bool runSingleReactionAttempt() {
  int minDelay;
  int maxDelay;
  unsigned long reactionLimit;
  int maxButtons;

  if (difficulty == 0) {
    minDelay = 2500;
    maxDelay = 5000;
    reactionLimit = 2000;
    maxButtons = 1;
  } 
  else if (difficulty == 1) {
    minDelay = 1200;
    maxDelay = 3000;
    reactionLimit = 1200;
    maxButtons = 2;
  } 
  else {
    minDelay = 200;
    maxDelay = 900;
    reactionLimit = 500;
    maxButtons = 3;
  }

  unsigned long waitTime = random(minDelay, maxDelay);
  unsigned long waitStart = millis();

  while (millis() - waitStart < waitTime) {
    if (anyButtonPressed()) {
      showScreen("Too Early!", "Missed");
      waitForAllRelease();
      return false;
    }
  }

  int targetButton = random(0, maxButtons);

  turnOffLEDs();
  showColorStimulus(targetButton);

  unsigned long startTime = millis();

  while (true) {
    if (millis() - startTime > reactionLimit) {
      turnOffLEDs();
      showScreen("Too Slow!", "Missed");
      waitForAllRelease();
      return false;
    }

    int pressed = getPressedButton();

    if (pressed != -1) {
      reactionTime = millis() - startTime;
      turnOffLEDs();
      waitForAllRelease();

      if (pressed == targetButton) {
        showReactionResult(reactionTime);
        return true;
      } 
      else {
        showScreen("Wrong Button", "Missed");
        return false;
      }
    }
  }
}

// =========================
// SEQUENCE GAME
// =========================

void startSequenceGame() {
  int maxRounds;
  int showDelay;
  unsigned long inputLimit;

  if (difficulty == 0) {
    maxRounds = 5;
    showDelay = 900;
    inputLimit = 5000;
  } 
  else if (difficulty == 1) {
    maxRounds = 7;
    showDelay = 650;
    inputLimit = 4000;
  } 
  else {
    maxRounds = 10;
    showDelay = 450;
    inputLimit = 3000;
  }

  int sequence[10];

  for (int i = 0; i < maxRounds; i++) {
    sequence[i] = random(0, 3);
  }

  showScreen("Sequence Game", "Repeat pattern");
  delay(1500);

  for (int round = 1; round <= maxRounds; round++) {
    char roundText[25];
    sprintf(roundText, "Round %d/%d", round, maxRounds);
    showScreen(roundText, "Watch...");
    delay(1000);

    // Show sequence from start up to current round
    for (int i = 0; i < round; i++) {
      showColorStimulus(sequence[i]);
      delay(showDelay);

      turnOffLEDs();
      showScreen("Sequence", "...");
      delay(250);
    }

    showScreen("Your Turn", "Repeat it");
    delay(800);

    // User repeats current sequence
    for (int i = 0; i < round; i++) {
      char inputText[25];
      sprintf(inputText, "Input %d/%d", i + 1, round);
      showScreen(inputText, "Press color");

      int pressed = waitForButtonWithTimeout(inputLimit);

      if (pressed == -1) {
        showSequenceResult(round - 1, "Too Slow!");
        return;
      }

      if (pressed != sequence[i]) {
        showSequenceResult(round - 1, "Wrong!");
        return;
      }

      blinkLED(pressed, 150);
      waitForAllRelease();
      delay(250);
    }

    showScreen("Correct!", "Next round");
    delay(1000);
  }

  showSequenceResult(maxRounds, "Completed!");
}

void showSequenceResult(int score, const char* message) {
  char text[25];

  display.clearDisplay();
  delay(20);

  display.clearBuffer();
  display.setFont(u8g2_font_7x14B_tf);

  display.drawStr(0, 15, message);

  sprintf(text, "Score: %d", score);
  display.drawStr(0, 38, text);

  display.drawStr(0, 60, difficultyNames[difficulty]);

  display.sendBuffer();
  delay(4000);
}

// =========================
// MENU / DISPLAY
// =========================

void showMenu() {
  display.clearDisplay();
  delay(20);

  display.clearBuffer();
  display.setFont(u8g2_font_7x14B_tf);

  display.drawStr(0, 12, "MAIN MENU");

  for (int i = 0; i < menuCount; i++) {
    int y = 28 + (i * 12);

    if (i == menuIndex) {
      display.drawStr(0, y, ">");
      display.drawStr(15, y, menuItems[i]);
    } else {
      display.drawStr(15, y, menuItems[i]);
    }
  }

  display.sendBuffer();
  delay(20);
}

void changeDifficulty() {
  difficulty++;
  if (difficulty > 2) difficulty = 0;

  showScreen("Difficulty", difficultyNames[difficulty]);
  delay(1500);
}

void showBestTime() {
  char text[25];

  display.clearDisplay();
  delay(20);

  display.clearBuffer();
  display.setFont(u8g2_font_7x14B_tf);

  display.drawStr(0, 15, "Reaction Best:");
  display.drawStr(0, 35, difficultyNames[difficulty]);

  if (bestReactionTimes[difficulty] == 99999) {
    display.drawStr(0, 58, "No score yet");
  } else {
    sprintf(text, "%lu ms", bestReactionTimes[difficulty]);
    display.drawStr(0, 58, text);
  }

  display.sendBuffer();
  delay(3000);
}

void showReactionSummary(int correctAttempts, unsigned long totalTime, unsigned long runBest) {
  char text[25];

  display.clearDisplay();
  delay(20);

  display.clearBuffer();
  display.setFont(u8g2_font_7x14B_tf);

  sprintf(text, "Correct: %d/%d", correctAttempts, reactionAttempts);
  display.drawStr(0, 15, text);

  if (correctAttempts > 0) {
    sprintf(text, "Avg: %lu ms", totalTime / correctAttempts);
    display.drawStr(0, 38, text);

    sprintf(text, "Best: %lu ms", runBest);
    display.drawStr(0, 60, text);
  } else {
    display.drawStr(0, 38, "No valid score");
  }

  display.sendBuffer();
  delay(20);
}

void showReactionResult(unsigned long timeMs) {
  char text[25];

  display.clearDisplay();
  delay(20);

  display.clearBuffer();
  display.setFont(u8g2_font_7x14B_tf);

  display.drawStr(0, 15, "Correct!");

  sprintf(text, "%lu ms", timeMs);
  display.drawStr(0, 38, text);

  if (timeMs < 250) {
    display.drawStr(0, 60, "Excellent");
  } else if (timeMs < 350) {
    display.drawStr(0, 60, "Good");
  } else if (timeMs < 500) {
    display.drawStr(0, 60, "Average");
  } else {
    display.drawStr(0, 60, "Slow");
  }

  display.sendBuffer();
  delay(20);
}

void showScreen(const char* line1, const char* line2) {
  display.clearDisplay();
  delay(20);

  display.clearBuffer();
  display.setFont(u8g2_font_7x14B_tf);

  display.drawStr(0, 20, line1);
  display.drawStr(0, 45, line2);

  display.sendBuffer();
  delay(20);
}

void showColorStimulus(int colorIndex) {
  turnOffLEDs();

  digitalWrite(ledPins[colorIndex], HIGH);

  display.clearDisplay();
  delay(20);

  display.clearBuffer();
  display.setFont(u8g2_font_7x14B_tf);

  display.drawStr(0, 20, "GO!");
  display.drawStr(0, 45, colorNames[colorIndex]);

  display.sendBuffer();
  delay(20);
}

// =========================
// INPUT / LED HELPERS
// =========================

int getPressedButton() {
  if (digitalRead(redButton) == LOW) return 0;
  if (digitalRead(blueButton) == LOW) return 1;
  if (digitalRead(greenButton) == LOW) return 2;
  return -1;
}

int waitForButtonWithTimeout(unsigned long timeoutMs) {
  unsigned long start = millis();

  while (millis() - start < timeoutMs) {
    int pressed = getPressedButton();

    if (pressed != -1) {
      return pressed;
    }
  }

  return -1;
}

bool anyButtonPressed() {
  return getPressedButton() != -1;
}

void waitForAllRelease() {
  while (anyButtonPressed()) {
  }

  delay(100);
}

void turnOffLEDs() {
  digitalWrite(redLED, LOW);
  digitalWrite(blueLED, LOW);
  digitalWrite(greenLED, LOW);
}

void blinkLED(int ledIndex, int blinkTime) {
  digitalWrite(ledPins[ledIndex], HIGH);
  delay(blinkTime);
  digitalWrite(ledPins[ledIndex], LOW);
}
