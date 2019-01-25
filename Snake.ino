#include "LedControl.h"
#include <LiquidCrystal.h>
//imports libraries

//declares the pins for input of the joystick, and sets the sensitivity
#define X_PIN 0
#define Y_PIN 1
#define LIMIT 300

//sets the light on/off for each LED on the screen
boolean screen[8][8];

//keeps data for the x and y coordinates of each segment of the snake
int nodes[64][2];

//sets whether the menu should be shown 
boolean menu=true;

//sets the x and y coordinates of the candy
int candyX=-1;
int candyY=-1;

int SPEED=500;// sets the speed of the snake
boolean lost=false;// checks if the person has died or not
int dir=2;// checks the direction (1-4) of the four directions
int score=0;// keeps score
LiquidCrystal lcd(7,8,3,4,5,6);// sets up the LCD
int page=0;// the page number, dictating which of the difficulties and faces to show
String hards[3]={"Easy","Medium", "Hard"};//writes the difficulties onto the LCD
boolean pogMode=false;// checks if the mode of the game is in HACK mode or normal mode
byte difficulties[3][8]={{B00010000,B00100000,B01000100,B01010000,B01010000,B01000100,B00100000,B00010000},
{B01000000,B01000000,B01000100,B01010000,B01010000,B01000100,B01000000,B01000000},
{B10000000,B01000000,B00100100,B00110000,B00110000,B00100100,B01000000,B10000000}};//sets the light patterns for the smile, frown, and neutral faces


LedControl lc=LedControl(12,10,11,1);//sets up the LED matrix

boolean candyB=false;//checks whether the snake just ate a candy (therefore flashing the lights)

void sendBytes(){// method that converts the boolean array of screen to a signal to send to the LED matrix
  for(int count=0;count<8;count++){
    byte fill=0;
    for(int count2=0;count2<8;count2++){
      if(screen[count][count2]){
//          Serial.print(count2);
//          Serial.print(" b");
          
          int add=pow(2,count2);
          if(count2>1){
            add++;
          }
//          Serial.print(add);
//          Serial.print(" ");
          fill+=add;
      }
    }
    lc.setRow(0,count,fill);
//    Serial.println(fill);
  }
}

void clearAll(){//clears the screen of all values
  for(int count=0;count<8;count++){
    for(int count2=0;count2<8;count2++){
      screen[count][count2]=false;
    }
  }
}
void printAll(){// for debugging, prints the screen to the console
  for(int count=0;count<8;count++){
    for(int count2=0;count2<8;count2++){
      if(screen[count][count2]){
        Serial.print(1);
      }
      else{
        Serial.print(0);
      }
      Serial.print(" ");
    }
    Serial.println();
  }
}
void printNodes(){//for debugging, prints each segment of the snake to the console
  for(int count=0;count<64&&nodes[count][0]!=-1;count++){
    Serial.print(count);
    Serial.println(":");
    Serial.print(nodes[count][0]);
    Serial.print(" ");
    Serial.print(nodes[count][1]);
  }
}
void drag(){//moves the head of the snake in the direction of the joystick, and makes each segment take the place of the one in front of it.
  int x,x2,y,y2;
  int lastX, lastY;
  for(int count=0;count<64&&nodes[count][0]!=-1;count++){
    lastX=nodes[count][0];
    lastY=nodes[count][1];
  }
  x=nodes[0][0];
  x2=nodes[0][0];
  y=nodes[0][1];
  y2=nodes[0][1];

  //changes the coordinates of the head based on the direction of the joystick
  if(dir==0){
   nodes[0][0]++;
  }
  else if(dir==1){
    nodes[0][0]--;
  }
  else if(dir==2){
    nodes[0][1]++;
  }
  else if(dir==3){
    nodes[0][1]--;
  }

  //checks if the head of the snake hits the edge of the map or not
  if(nodes[0][0]<0||nodes[0][0]>7||nodes[0][1]<0||nodes[0][1]>7){
    Serial.println("WALLLLLL");//why not
    lost=true;
    nodes[0][0]=x;
    nodes[0][1]=y;
    return;
  }
  boolean got=false;
  if(nodes[0][0]==candyX&&nodes[0][1]==candyY){//checks if the head ate a candy
    got=true;
  }
  else if(screen[nodes[0][0]][nodes[0][1]]&&(nodes[0][0]!=lastX||nodes[0][1]!=lastY)){//checks if the head hit its own body
    Serial.println("TAILLLL");//lol
    lost=true;
    nodes[0][0]=x;
    nodes[0][1]=y;
    return;
  }
  
  //have all other nodes follow
  int count=-1;
  for(count=1;count<64&&nodes[count][0]!=-1;count++){
    Serial.println(count);
    Serial.print("a ");
    Serial.print(x);
    Serial.print(" ");
    Serial.print(y);
    x2=nodes[count][0];
    y2=nodes[count][1];
    nodes[count][0]=x;
    nodes[count][1]=y;
    x=x2;
    y=y2;
    
  }
  if(got){//plays a sound, adds the score, and makes a new candy
    nodes[count][0]=x;
    nodes[count][1]=y;
    generateCandy();
    score++;
    lcd.setCursor(0,1);
        lcd.print("                 ");
        lcd.setCursor(0,1);
        lcd.print(score);
    candyB=true;
  }
  
}
void paint(){//refreshes the screen based on the new coordinates of the candy and snake
  clearAll();
 for(int count=0;count<64&&nodes[count][0]!=-1;count++){
  screen[nodes[count][0]][nodes[count][1]]=true;
 }
 screen[candyX][candyY]=true;
}
void generateCandy(){//randomizes the position of the new candy
  candyX=random(8);
  candyY=random(8);
  while(screen[candyX][candyY]){//makes sure the space wasn't already taken
    candyX=random(8);
  candyY=random(8);
  }

}
void candyBeep(){//flashes the new candy when it is created
  screen[candyX][candyY]=false;
  sendBytes();
  for(int count=0;count<10;count++){
    setDir();
    delay(10);
  }
  screen[candyX][candyY]=true;
  sendBytes();
  
  for(int count=0;count<10;count++){
    setDir();
    delay(10);
  }
  screen[candyX][candyY]=false;
  sendBytes();
  
  for(int count=0;count<10;count++){
    setDir();
    delay(10);
  }
  screen[candyX][candyY]=true;
  sendBytes();
  
  for(int count=0;count<10;count++){
    setDir();
    delay(10);
  }
  screen[candyX][candyY]=false;
  sendBytes();
  
  for(int count=0;count<10;count++){
    setDir();
    delay(10);
  }
  screen[candyX][candyY]=true;
  sendBytes();
  
  for(int count=0;count<10;count++){
    setDir();
    delay(10);
  }
}
void beep(){//flashes the entire screen
  clearAll();
  sendBytes();
  delay(100);
  paint();
  sendBytes();
  delay(500);
}
void setup(){// sets up all initial values
  Serial.begin(9600);//console
  for(int count=0;count<64;count++){// initializes all values of the snake segment.
    nodes[count][0]=-1;
    nodes[count][1]=-1;
  }
   lcd.begin(16,2);//lcd
   lc.shutdown(0,false);//sets up led
   
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
  randomSeed(analogRead(A7));//re-randomizes the values of the arduino (because arduino is bad and can't truly random)
  
  lcd.print("Choose level");
 
 
//  lcd.setCursor(0,1);
//  lcd.print("test");
//  pinMode(7,INPUT);
  lcd.setCursor(0,1);
  clearAll();
}
void setGame(){//sets the begining of the game
  clearAll();
  generateCandy();
//  Serial.println(candyX);
//  Serial.println(candyY);
  nodes[0][0]=4;
  nodes[0][1]=4;
  nodes[1][0]=4;
  nodes[1][1]=3;
  nodes[2][0]=4;
  nodes[2][1]=2;
  paint();
  printAll();
  sendBytes();
  dir=2;
}
void setDir(){//gets the direction from the joystick
   int x=analogRead(X_PIN);
    int y=analogRead(Y_PIN);
  if(x<LIMIT&&dir!=2){
      dir=3;
    } 
    else if(x>1024-LIMIT&&dir!=3){
      dir=2;
    }
    else if(y<LIMIT&&dir!=1){
      dir=0;
    }
    else if(y>1024-LIMIT&&dir!=0){
      dir=1;
    }
}
void setDir2(){//a slightly modified joystick input for the menu selection
   int x=analogRead(X_PIN);
    int y=analogRead(Y_PIN);
  if(x<LIMIT&&dir!=2){
      dir=3;
    } 
    else if(x>1024-LIMIT&&dir!=3){
      dir=2;
    }
    else if(y<LIMIT&&dir!=1){
      dir=0;
    }
    else if(y>1024-LIMIT&&dir!=0){
      dir=1;
    }
    else{
      dir=-1;
    }
}
void drawPage(int in){//draws the page of the face to the led matrix
  for(int count=0;count<8;count++){
    lc.setRow(0,count,difficulties[in][count]);
  }
}
int lastPage=-1;//stores the value for the previous page
void loop(){
//  lcd.clear();
 if(menu){//checks if the menu is still up (game hasn't started yet)
       
       int pastDir=dir;
       setDir2();

       //flips the page
       if(pastDir==-1&&pastDir!=dir){
        if(dir==0){
          page++;
        }
        else if(dir==1){
          page--;
        }
        if(page<0){
          page=2;
        }
        else if(page>2){
          page=0;
        }
       }

       
       if(lastPage!=page){//draws a new page if the page changed
      lcd.setCursor(0,1);
        lcd.print("                 ");
        lcd.setCursor(0,1);
        lcd.print(hards[page]);
       Serial.println("AHHHHHH");
       lastPage=page;
       }

       
       drawPage(page);
      if(dir==2){//starts the game if they go down the joystick
        boolean pog=true;
        for(int count=0;count<500;count++){
         setDir2();
          if(dir!=2){
            pog=false;
            break;
          }
          delay(10);
        }
        menu=!menu;
        SPEED-=page*100;
        if(pog){
          pogMode=true;
          SPEED/=10;
          SPEED+=20;
        }
        setGame();
        lcd.setCursor(0,0);
        lcd.print("                 ");
        lcd.setCursor(0,0);
        lcd.print("Score:");
        lcd.setCursor(0,1);
        lcd.print("                 ");
        lcd.setCursor(0,1);
        lcd.print(score);
      }
      
  }
  else{
    while(!lost){//sees if they lost right now
   
    setDir();//gets joystick direction
    
    drag();//moves all snake nodes that way
    
    if(lost){//ends loop if lost
      break;
    }
    
    paint();// much like the java applet paint
    
   
    if(candyB){//flashes the new candy if snake ate the old one
      candyBeep();
      candyB=false;
    }
    else{
    sendBytes(); //sends all screen values to the LED matrix
    }
    for(int count=0;count<SPEED/5;count++){//gets the direction (constant refreshing while the snake is paused between each step)
      setDir();
      delay(5);
    }
 }
 if(lost){
  beep();//flashes the entire screen when snake is dead
 }
  
  }
  
 
}

