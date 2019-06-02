// Hangmanduino
// =======================================================================================
// This sketch allows you to play hangman on your Arduino. There is a small list of words
// defined in a variable, which the program randomly selects from. A potentiometer is used
// for scrolling through the alphabet and a tactile switch for making selections. When you
// select the letter, the program displays a * instead of the letter and you aren't allowed
// to make that selection again. If you guess the word, you win and the board is reset. If
// you guess too many wrong, game over and the board is reset. If you hold the select button
// for 2 seconds, the board is reset.
//
// For more information, please visit my website: www.nerdybynature.com
//
// Written By: Dan Wagoner
// Date: 08/22/2009
//
//
// References
// ----------------------------------------------------------------------------------------
// Debounce code from David A. Mellis' / Limor Fried example in the Arduino examples
// String library written by Hernando Barragan - http://www.arduino.cc/en/Tutorial/TextString
//
//
// modified version the famous hangman by mvelsmid
//
// - modified hangman graphic characters
// - display the search word
// - direct insert word
// - SK version


#include <LiquidCrystal.h>

#define SPEAKERPIN 3
#define NUMWORDS 10

//define notes for buzzer
#define LOWNOTE 100
#define ALOW 440
#define CLOW 261
#define ELOW 329
#define FLOW 349
#define CHIGH 523
#define EHIGH 659
#define GHIGH 784
#define FSHIGH 740
#define AHIGH 880

#define btnRight     0
#define btnUp        1
#define btnDown      2
#define btnLeft      3
#define btnSelect    4
#define btnNone      5


void(* resetFunc) (void) = 0;             //declare reset function @ address 0

const char* words[] = {"atmosfera", "pocasie", "ceruzka", "lizatko", "pondelok", "monitor", "opica", "kvetinac", "dilema", "prhlava"};
const char letterVal[] = "abcdefghijklmnopqrstuvwxyz";
char guessLetter;
char guessLast;
char newword[25];
char guessed[25];
char minus = '0'; 
char* secretWord;
int guessedCount = 1;
int wordSize;
int gotOne = 0;
int alreadyGuessed = 0;
int showAsterisk = 0;
int buttonState;
int hangman = 0;
int totalRight = 0;
int sel = 0;
int prevKey = btnNone;
unsigned long lastDebounceTime = 0;
int c = 1000;
int m = 1000;
int v = 0;
int n = 1000;
int pocet = 0;
int lcdpos = 0;

//LiquidCrystal lcd(12, 11, 2, 7, 8, 9, 10);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

String guessWord = String(10);

// hangman graphic characters
byte topleft[] = { 0x1F, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8 };
byte topright[] = { 0x1E, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte bottomleft[] = { 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x1F, 0x1F };
byte bottomright[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x1F, 0x1F };
byte head[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x00, 0x00, 0x00 };
byte topbody[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x04, 0x04, 0x04 };
byte bottombody[] = { 0x04, 0x00, 0x00, 0x00, 0x00,0x0, 0x1F, 0x1F  };
byte rightarm[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x05, 0x06, 0x04 };
byte leftarm[] = { 0x1C, 0x04, 0x04, 0x0E, 0x0E, 0x15, 0x0E, 0x04 };
byte rightleg[] = { 0x04, 0x02, 0x02, 0x00, 0x00, 0x0, 0x1F, 0x1F  };
byte leftleg[] = { 0x04, 0x0a, 0x0A, 0x00, 0x0, 0x0, 0x1F, 0x1F  };
byte leftarrow[] = { 0x10, 0x18, 0x1C, 0x1E, 0x1E, 0x1C, 0x18, 0x10 };
byte rightarrow[] = { 0x01, 0x03, 0x07, 0x0F, 0x0F, 0x07, 0x03, 0x01 };
byte smilec[] = { 0x7, 0x8, 0x11, 0x14, 0x14, 0x13, 0x8, 0x7 };
byte smiled[] = { 0x1C, 0x2, 0x11, 0x5, 0x5, 0x19, 0x2, 0x1C };
byte dlhea[] = {0x02,0x04,0x0E,0x01,0x0F,0x11,0x0F,0x00};
byte smilea[] = { 0x00, 0x11, 0x00, 0x00, 0x11, 0x0E, 0x00, 0x00 };

void setup()
{

  Serial.begin(9600);
  pinMode (SPEAKERPIN, OUTPUT);

  lcd.begin(16, 2);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("OBESENEC");
  delay(1500);

  // pick a random word using analog 5 for random data
  randomSeed(analogRead(5));
  newWord();

  //draw the board
  draw_board();
}

void newWord() {

  //pick a random word from the list
  int pick = random(NUMWORDS);
  const char* pickWord = words[pick];
  guessWord = pickWord;
  //secretWord = guessWord.getChars();
  wordSize = guessWord.length();
  Serial.println(guessWord);          // print the word to serial for cheaters like me ;)
}

void draw_board() {

  // define the custom characters
  lcd.createChar(0, topleft);
  lcd.createChar(1, topright);
  lcd.createChar(2, bottomleft);
  lcd.createChar(3, bottomright);
  lcd.createChar(4, leftarrow);
  lcd.createChar(5, rightarrow);

  // draw blank hangman table
  lcd.clear();
  lcd.home();
  lcd.write(byte(0));
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.write(3);

  // print underlines
  lcd.setCursor(3, 1);
  for (int x = 0; x < wordSize; x++) {
    lcd.print("_");
  }
}

void loop() {

if (v == 1)  {
    v = 1;
    }
      else if (v == 0) {
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Zobrazit slovo?");
          lcd.setCursor(2, 1);
          lcd.print("dole=N/hore=A");
          
      while (m > 90) {
      int c = analogRead(A0);
      if (c < 396 and c > 221) {
      m = 20;
      delay(500);
       }
       if (c < 221 and c > 66) { //ak bolo vybrane ano
      m = 10;
       }
    }  

  if (m == 10){//ak bolo vybrane ano zobraz slovo
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hadane slovo je:");
      lcdpos= (16 - wordSize) / 2;
      lcd.setCursor(lcdpos, 1);
      lcd.print(guessWord);
      delay(2000);
       m = 1000;
       c = 1000;
             }
  else {
        m = 1000; 
        c = 1000;
        }
     
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Vlozit nove?");
      lcd.setCursor(2, 1);
      lcd.print("dole=N/hore=A");
  while (n > 90) {
  int c = analogRead(A0);
  if (c < 396 and c > 221) {
  n = 20;
   }
  if (c < 221 and c > 66) { //ak bolo vybrane ano
  n = 10;
   }
}  

  if (n == 10){//ak bolo vybrane ano zadaj vlastne slovo
     lcd.clear(); 
     lcd.setCursor(0, 0);
     lcd.print("Po zadani noveho");
     lcd.setCursor(0, 1);
     lcd.print("slova, podrz UP!");
      delay(4000);
   // ********************************************************************** new word ***************************   
       m = 1000;
       c = 1000;
       delay(500);
       lcd.clear(); 
       while (m > 90) {
       int c = analogRead(A0);
       if (c < 221 and c > 66){
       m = 20;
       }
  
                      int potVal = sel;
                      guessLetter = letterVal[potVal];
                    
               
                      if (guessLetter != guessLast) {
                        guessLast = guessLetter;
                      
                  
                        // vypise abecedu vlavo od stredu
                        lcd.setCursor(3, 0);
                        for (int x = 5; x >= 1 ; x--) {
                          if (potVal - x >= 0) {
                            lcd.print(letterVal[potVal - x]);
                          }
                          else {
                            lcd.print(" ");
                          }
                        }
                    
                        // lava sipka
                        lcd.write(4);
                    
                        // print the letter
                        if (showAsterisk == 0) {
                          lcd.setCursor(9, 0);
                          lcd.print(guessLetter);
                          
                        }
                        //stred guessLetter = vybrane pismeno
                        else {
                          lcd.setCursor(9, 0);
                          lcd.print(guessLetter);
                         // alreadyGuessed = 1;
                        }
                    
                        // prava sipka
                        lcd.write(5);
                    
                        // vypise abecedu vpravo od stredu
                        lcd.setCursor(11, 0);
                        for (int x = 1; x <= 5 ; x++) {
                          if (potVal + x <= 25) {
                            lcd.print(letterVal[potVal + x]);
                          }
                          else {
                            lcd.print(" ");
                          }
                        }
                      }
                            
                            lcdpos= (16 - pocet) / 2;
                            lcd.setCursor(lcdpos, 1);
                            lcd.print(newword);
                           int k = inkeys();
                            if ( k == btnSelect ) {
                              //tu sa zapisuju pismena do noveho slova 
                                 
                                 newword[pocet] = guessLetter;
                                 pocet++;
                                 lcd.setCursor(0, 1);
                                 lcd.print("                ");
                                 lcdpos= (16 - pocet) / 2;
                                 lcd.setCursor(lcdpos, 1);
                                 lcd.print(newword);
                                 
                                 
                                 buzz(FSHIGH, 10);
                                 buzz(AHIGH, 25);
                                 delay(20);
                                 if (pocet == 26) {
                                   lcd.setCursor(0, 0);
                                    lcd.print("Presiahol si max");
                                    lcd.setCursor(0, 1);
                                    lcd.print("pocet znakov =25");
                                    delay(5000);
                                  resetFunc();   
                                 }
                              if (alreadyGuessed == 0) {
                                alreadyGuessed = 1;
                                lcd.setCursor(9, 0);
                                lcd.print(guessLetter);
                                             
                              }
                              
                              } else if (k == btnRight) {
                              if (sel > 24) {
                                sel = 0;
                              } else {
                                sel++;
                              }
                            } else if (k == btnLeft) {
                              if (sel <= 0) {
                                sel = 25;
                              } else {
                                sel--;
                              }
                            
                            }
                             if ( k == btnDown ) { //zmaz pismeno
                                 pocet = pocet - 1;
                                                                  
                                 if ( pocet < 0 ){
                                 pocet = 0;
                                 }
                                 
                                 for (int e = 0; e < pocet; e++) {
                                    Serial.println(newword[e]);
                                  }
                                 newword[pocet] = newword[pocet+1];
                                 lcd.setCursor(0, 1);
                                 lcd.print("                ");
                                 lcdpos= (16 - pocet) / 2;
                                 lcd.setCursor(lcdpos, 1);
                                 lcd.print(newword);
                                 buzz(CLOW, 100);  
                              }
                              
     
    }  
    if (pocet == 0) {
                                    lcd.setCursor(0, 0);
                                    lcd.print("Nezadal si slovo");
                                    lcd.setCursor(0, 1);
                                    lcd.print("vyberam povodne!");
                                    buzz(ELOW, 250);                  
                                    buzz(CLOW, 500);                 
                                    delay(4000);
                                  
                     }
       
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zadal si slovo:");
      if (pocet == 0) {
                                     lcd.clear();
                                     lcd.setCursor(1, 0);
                                     lcd.print("Nove slovo je:");
                       }
      if (pocet > 0) {
                                     //lguessWord = newword;
                                     buzz(EHIGH, 50);
                                     buzz(AHIGH, 100);
                                     guessWord = newword;
                                     wordSize = guessWord.length();
                       }
                                    
      lcdpos= (16 - wordSize) / 2;
      lcd.setCursor(lcdpos, 1);
      lcd.print(guessWord);
      Serial.println(guessWord);
      delay(3000);
      sel = 0;  
      guessLast = "0";
             }

  // ********************************************************************** end new word ***************************   
  v = 1;
  lcd.createChar(0, topleft);
  lcd.createChar(1, topright);
  lcd.createChar(2, bottomleft);
  lcd.createChar(3, bottomright);
  lcd.createChar(4, leftarrow);
  lcd.createChar(5, rightarrow);
  lcd.createChar(6, smilea);
 
  // draw blank hangman table
  lcd.clear();
  lcd.home();
  lcd.write(byte(0));
  lcd.write(1);
  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.write(3);

   lcd.setCursor(3, 1);
  for (int x = 0; x < wordSize; x++) {
    lcd.print("_");
  }
  
 }
  
  // letter selection via potentiometer
  //int potVal = analogRead(POTPIN) / 40;      // 1024 / 26 ~= 39
  int potVal = sel; //Hack to use lcd keypad shield buttons
  guessLetter = letterVal[potVal];

  // if letter is different from last, print to lcd
  // this prevents from printing the same char over and over
  if (guessLetter != guessLast) {
    guessLast = guessLetter;
    showAsterisk = 0;

    // cycle through all guessed letters and determine whether to show * or the letter
    for (int x = 0; x < guessedCount; x++) {
      if (guessLetter == guessed[x]) {
        showAsterisk = 1;
      }
    }
    // print letters to the left of selected letter
    lcd.setCursor(3, 0);
    for (int x = 5; x >= 1 ; x--) {
      if (potVal - x >= 0) {
        lcd.print(letterVal[potVal - x]);
      }
      else {
        lcd.print(" ");
      }
    }

    // print left arrow
    lcd.write(4);

    // print the letter
    if (showAsterisk == 0) {
      lcd.setCursor(9, 0);
      lcd.print(guessLetter);
      alreadyGuessed = 0;
    }
    // print a *
    else {
      lcd.setCursor(9, 0);
      lcd.print("*");
      alreadyGuessed = 1;
    }

    // print right arrow
    lcd.write(5);

    // print letters to the right of selected letter
    lcd.setCursor(11, 0);
    for (int x = 1; x <= 5 ; x++) {
      if (potVal + x <= 25) {
        lcd.print(letterVal[potVal + x]);
      }
      else {
        lcd.print(" ");
      }
    }
  }

  int k = inkeys();
  if ( k == btnSelect ) {
    gotOne = 0;
    if (alreadyGuessed == 0) {
      alreadyGuessed = 1;
      lcd.setCursor(9, 0);
      lcd.print("*");
      char buf[wordSize+1];
      guessWord.toCharArray(buf,wordSize+1);
      for (int i = 0; i < wordSize; i++) {
        if (buf[i] == guessLetter) {
          lcd.setCursor(i + 3, 1);
          lcd.print(guessLetter);
          gotOne = 1;
          totalRight = totalRight + 1;
        }
      }
      // add letter to guessed letter array
      guessed[guessedCount] = guessLetter;
      guessedCount++;

      // none of the letters match, draw the next body part on the hangman
      if (gotOne == 0) {
        buzz(LOWNOTE, 500);
        hangman++;
        draw_hangman(hangman);
      }
      else {
        // letter is in word, sound buzzer
        buzz(FSHIGH, 30);
        buzz(AHIGH, 50);
      }

      //all letters have been guessed...WIN!
      if (totalRight == wordSize) {
        gameOver(1);
      }
    }
    // this letter has already been guessed, sound buzzer
  } else if (k == btnRight) {
    if (sel > 24) {
      sel = 0;
    } else {
      sel++;
    }
  } else if (k == btnLeft) {
    if (sel <= 0) {
      sel = 25;
    } else {
      sel--;
    }
  }
  
}

void draw_hangman(int var) {

  switch (var) {
    case 1:
      lcd.createChar(1, head);           // head
      break;
    case 2:
      lcd.createChar(1, topbody);        // body
      lcd.createChar(3, bottombody);
      break;
    case 3:
      lcd.createChar(1, rightarm);       // right arm
      break;
    case 4:
      lcd.createChar(1, leftarm);        // left arm
      break;
    case 5:
      lcd.createChar(3, rightleg);       // right leg
      break;
    case 6:
      lcd.createChar(3, leftleg);        // left leg
      break;
    case 7:
      gameOver(0);
    default:
      break;
  }
}

void gameOver(int whatToDo) {

 // decide whether win, lose or restart game
  switch (whatToDo) {
    case 0:  // GAME OVER
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("KONIEC");
      lcd.setCursor(6, 1);
      lcd.print("HRY");

      //buzzer sound
      buzz(ELOW, 500);                  // GAME OVER!
      buzz(CLOW, 1000);                 // sound buzzer
      break;
    case 1:  // WINNER
        // buzzer sound
      buzz(ALOW, 150);
      buzz(CHIGH, 150);
      buzz(EHIGH, 150);
      buzz(AHIGH, 150);
      delay(150);
      buzz(GHIGH, 150);
      buzz(AHIGH, 500);
      
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("VYHRAL SI!!!");
      lcd.setCursor(2, 1);
      lcd.print("SI MAJSTER!");
      delay(3000);

      for (int b = 0; b <= 1; b++){
      for (int a = 0; a <= 15; a++){
      
      lcd.setCursor(a, b);
      lcd.write(byte(6));
   //   lcd.write(byte(7));
      delay(155);
      lcd.clear();
      }
      }      
      
  }
  delay(1000);
  resetFunc();      // reset arduino for a new game
}

void buzz (int frequencyInHertz, long timeInMilliseconds) {
  Serial.println(frequencyInHertz);
  long delayAmount = (long)(1000000 / frequencyInHertz);
  long loopTime = (long)((timeInMilliseconds * 1000) / (delayAmount * 2));
  for (int x = 0; x < loopTime; x++) {
    digitalWrite(SPEAKERPIN, HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(SPEAKERPIN, LOW);
    delayMicroseconds(delayAmount);
  }
  delay(20);
}

int getKey() {
  int b = analogRead(A0);
  if (b > 1000) return btnNone;
  delay(8);
  if (b < 66) return btnRight;
  if (b < 221) return btnUp;
  if (b < 396) return btnDown;
  if (b < 602) return btnLeft;
  if (b < 902) return btnSelect;
}


int inkeys() {
  int k = getKey();
  while (k == btnNone) {
    k = getKey();
    delay(10);
  }
  delay(200); //debounce
  return k;
}
