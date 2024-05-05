/////////////////////////////////////////////////////////////////////////////
//
//  A Simple Game of Snake
//  written by Tyler Edwards.
//
//  Modified by Alexandru903. 
//
//  Added a buzzer and some simple sounds. 
//  Added simple buttons as inputs, removed some bugs, made the food not spawn inside of the snake.
//
//  Tyler on GitHub: https://github.com/HailTheBDFL (original source)
//
//  To begin the game, press the select button
//
/////////////////////////////////////////////////////////////////////////////

float gameSpeed = 4;  //Higher numbers are faster

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

//Pinout for display (currently for ILI9341)
#define _cs    5
#define _dc    4
#define _mosi  23
#define _sclk  18
#define _rst   22
#define _miso  19

#define BUZZER_PIN 2 // pin for buzzer

boolean start = false;     //will not start without say-so
unsigned long offsetT = 0; //time delay for input
unsigned long offsetM = 0; //time delay for main loop

float gs;
int headX = 1;             //coordinates for head
int headY = 1;
long int beenHeadX[550];   //coordinates to clear later
long int beenHeadY[550];
int changeX = 0;           //the direction of the snake
int changeY = 1;
boolean lastMoveH = false; //to keep from going back on itself 
int score = 1;
int foodX;                 //coordinates of food
int foodY;
boolean eaten = true;      //if true a new food will be made
int loopCount = 0;         //number of times the loop has run
int clearPoint = 0;        //when the loopCount is reset
boolean clearScore = false;

Adafruit_ILI9341 tft = Adafruit_ILI9341(_cs, _dc, _mosi, _sclk, _rst, _miso); //initialize the display

void setup() {
  gs = 1000 / gameSpeed;                 //calculated gameSpeed in milliseconds
  
  memset(beenHeadX, 0, 550);             //initiate beenHead with a bunch of zeros
  memset(beenHeadY, 0, 550);
  
  tft.begin();                           //turn on display
  tft.setRotation(1);
  
  tft.fillScreen(ILI9341_WHITE);                       //sets background
  tft.fillRect(3, 21, 316, 226, ILI9341_BLUE);
  
  tft.setTextColor(ILI9341_WHITE);            //Start button 
  tft.setTextSize(3);
  tft.setCursor(80, 90);
  tft.print("START");

  tft.setTextColor(ILI9341_BLACK); 
  tft.setTextSize(2);
  tft.setCursor(5, 3);         // name of the game 
  tft.print("SNAKE");

  //tft.setTextColor(ILI9341_BLACK); 
  //tft.setTextSize(2);
  //tft.setCursor(5, 3);          // old score keeper
  //tft.print("SCORE: ");
  //printScore();

  randomSeed(analogRead(6)); //make every game unique

  pinMode(33, INPUT_PULLUP); //up
  pinMode(25, INPUT_PULLUP); //down
  pinMode(32, INPUT_PULLUP); //left
  pinMode(26, INPUT_PULLUP); //right
  pinMode(12, INPUT_PULLUP); //select/start
  pinMode(2, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  if (clearScore and start) { //resets score from last game, won't clear until new game starts so you can show off your own score
    score = 1;
    printScore();
    clearScore = false;
  }

  if (digitalRead(33) == LOW) { // up button pressed
    up();
  }
  
  if (digitalRead(25) == LOW) { // down button pressed
    down();
  }
  
  if (digitalRead(32) == LOW) { // left button pressed
    left();
  }
  
  if (digitalRead(26) == LOW) { // right button pressed
   right();
  }
  
  if (digitalRead(12) == LOW) { // select/start button pressed

   tft.fillRect(0, 0, 50, 20, ILI9341_WHITE);
   tft.setTextColor(ILI9341_BLACK); //new score keeper
   tft.setTextSize(2);
   tft.setCursor(5, 3);
   tft.print("SCORE: ");
   printScore();

    select();

  }

  if (millis() - offsetM > gs and start) {
    beenHeadX[loopCount] = headX;  //adds current head coordinates to be
    beenHeadY[loopCount] = headY;  //covered later
    
    headX = headX + changeX;
    headY = headY + changeY; 
    
    if (headX - foodX == 0 and headY - foodY == 0) { //food!
      score = score + 1;
      printScore();
      eaten = true;
      tone(BUZZER_PIN, 1000, 100);
      delay(100); 
    }

    loopCount += 1; //loopCount used for addressing, mostly
    
    if (loopCount > 467) {            //if loopCount exceeds size of
      clearPoint = loopCount - score; //beenHead arrays, reset to zero
      loopCount = 0;
    }
    
    //drawDot(headX, headY); //head is drawn old

    drawDot(headX, headY, true); // Draw the head of the snake in green new
    
    for (int i = loopCount - score + 1; i < loopCount; i++) { drawDot(beenHeadX[i], beenHeadY[i], false);}

    if (loopCount - score >= 0) { //if array has not been reset
      eraseDot(beenHeadX[loopCount - score], beenHeadY[loopCount - score]);
    }  //covers end of tail
    else {
      eraseDot(beenHeadX[clearPoint], beenHeadY[clearPoint]);
      clearPoint += 1;
    }
 /*
    if (eaten) {     //randomly create a new piece of food if last was eaten
      foodX = random(2, 26);
      foodY = random(2, 18);
      eaten = false;
    }
  */
    if (eaten) { // Randomly create a new piece of food if the last one was eaten
        generateFood(); //generates random food based on the location of the snake ( not on the snake )
        eaten = false;
    }

   // drawDotRed(foodX, foodY); //draw the food old

    if (headX > 26 or headX < 1 or headY < 1 or headY > 18) { //Boudaries
      endGame();
    }

    if (loopCount - score < 0) {         //check to see if head is on tail
      for (int j = 0; j < loopCount; j++) {
        if (headX == beenHeadX[j] and headY == beenHeadY[j]) {
          endGame();
        }
      }
      for (int k = clearPoint; k < 467; k++) {
        if (headX == beenHeadX[k] and headY == beenHeadY[k]) {
          endGame();
        }
      }
    }
    else {
      for (int i = loopCount - (score - 1); i < loopCount; i++) {
        if (headX == beenHeadX[i] and headY == beenHeadY[i]) {
          endGame();
        }
      }
    }
    
    offsetM = millis(); //reset game loop timer
  }
}

void endGame() {

  tft.setCursor(80, 90);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("GAME OVER");
  delay(1000);


  tft.fillRect(0, 0, 320, 21, ILI9341_WHITE);
  tft.fillRect(0, 0, 3, 240, ILI9341_WHITE);
  tft.fillRect(319, 0, 1, 240, ILI9341_WHITE); 
  tft.fillRect(3, 21, 316, 226, ILI9341_BLUE); //deletes the old game, redraws the game screen as eficiently as possible
  
  eaten = true; //new food will be created
  
  tone(BUZZER_PIN, 1000, 200);
  delay(250); 
  tone(BUZZER_PIN, 800, 200);
  delay(300); 
  tone(BUZZER_PIN, 700, 200);
  delay(300); 
  tone(BUZZER_PIN, 600, 200);
  delay(300); 

  tft.setCursor(80, 90);       //Retry message
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("RETRY?");
  tft.setTextColor(ILI9341_BLACK); //sets back to scoreboard settings
  tft.setTextSize(2);
  
  tft.setCursor(5, 3);
  tft.print("SCORE: ");
  printScore();
  
  headX = 1;              //reset snake
  headY = 1;
  changeX = 0;
  changeY = 1;
  lastMoveH = false;

  memset(beenHeadX, 0, 550); //clear the beenHead arrays
  memset(beenHeadY, 0, 550); //probably not necessary

  loopCount = 0;
  clearScore = true;
  start = false;     //stops game
}

/*
void drawDot(int x, int y) {
  tft.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_WHITE);
}
*/ // old drawdot

void drawDot(int x, int y, bool isHead) {
  if (isHead) {
    tft.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_GREEN);
  } else {
    tft.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_WHITE);
  }
} // with green head

void drawDotRed(int x, int y) {
  tft.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_RED);
}

void eraseDot(int x, int y) {
  tft.fillRect(12*(x-1)+5, 12*(y-1)+23, 10, 10, ILI9341_BLUE);
}

void printScore() {
  tft.fillRect(88, 3, 50, 16, ILI9341_WHITE);  //clears old score
  tft.setCursor(88, 3);
  tft.print(score);                            //prints current score
}

void up() {
  if (millis() - offsetT > gs and lastMoveH) { //lastMoveH makes sure you can't go back on yourself
    changeX = 0;    //changes the direction of the snake
    changeY = -1;
    offsetT = millis();
    lastMoveH = false;
  }
}

void down() {
  if (millis() - offsetT > gs and lastMoveH) {
    changeX = 0;
    changeY = 1;
    offsetT = millis();
    lastMoveH = false;
  }
}

void left() {
  if (millis() - offsetT > gs and !lastMoveH) {
    changeX = -1;
    changeY = 0;
    offsetT = millis();
    lastMoveH = true;
  }
}

void right() {
  if (millis() - offsetT > gs and !lastMoveH) {
    changeX = 1;
    changeY = 0;
    offsetT = millis();
    lastMoveH = true;
  }
}

void select() {
  if (millis() - offsetT > gs and !start) {
    tft.fillRect(80, 90, 126, 24, ILI9341_BLUE); //Erase start message
    start = true;                                //allows loop to start
    offsetT = millis();
  }
  playHappyTune();
    delay(500); // Delay to allow the tune to complete
}

void playHappyTune() {  //start beeps
  int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};
  int noteDurations[] = {200, 200, 200, 200, 200, 200, 200, 200};

  // Play each note of the melody
  for (int i = 0; i < 8; i++) {
    tone(BUZZER_PIN, melody[i], noteDurations[i]);
    delay(noteDurations[i] * 1.3); // Pause between notes
    noTone(BUZZER_PIN); // Stop the tone
  }
}

void generateFood() {
    boolean foodOnSnake = true;
    while (foodOnSnake) {
        // Generate random coordinates for the food
        foodX = random(2, 26);
        foodY = random(2, 18);

        // Check if the food coordinates are not on the snake
        foodOnSnake = isFoodOnSnake(foodX, foodY);
    }
    // Draw the food at the generated coordinates
    drawDotRed(foodX, foodY);
}

bool isFoodOnSnake(int x, int y) {
    // Check if the food coordinates overlap with any part of the snake
    for (int i = 0; i < loopCount; i++) {
        if (x == beenHeadX[i] && y == beenHeadY[i]) {
            return true; // Food is on the snake
        }
    }
    return false; // Food is not on the snake
}


