#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20,16,2);
#include <Keypad.h>

// Дефинираме червената и зелената лампа
#define redDenied 3
#define greenAllowed 2

//Парола
char* password = "1234";

//Позиция на парола и лимит за невалидни опити
int passwordPosition = 0;
int invalidtries = 5;

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8}; //свързваме пиновете на редовете с клавиатурата
byte colPins[COLS] = {7, 6, 5, 4}; //свързваме пиновете на колоните с клавиатурата

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

Servo servo;
int servoPosition = 0;


//Булева променлива дали кода е грешен
bool isPasswordWrong = false;

void setup() 
{
  pinMode(redDenied, OUTPUT);
  pinMode(greenAllowed, OUTPUT);

  Serial.begin(9600);
  servo.attach(13);
  
  // LCD екран
  lcd.begin(16,1);
  defaultdisplay();
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.clear();
  
}

void loop() 
{
  if(passwordPosition == 0)
  {
    defaultdisplay();
  }
  
  int index;
  char code = keypad.getKey();
  
  if (code != NO_KEY)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PASSWORD: (----)");
    lcd.setCursor(11,0);
    for(index = 0;index <= passwordPosition;++index)
    {
      lcd.print("*");
    }
    
     if(code == password[passwordPosition]) //Проверява въведения код със зададената парола
  		{
    		passwordPosition++;
  		}
    
  	else
  		{
    		isPasswordWrong = true; //Променя булевата променлива на true заради неправилено въведен код
      		passwordPosition++;
  		}
    
    if(passwordPosition == 4)
    	{
            if(isPasswordWrong == true) //Проверка дали кода е грешен
                  {
                      invalidtries--;
                      if (invalidtries == 0)
                		{
                            invalidtries = 5;
                            attempsPunish();
                            passwordPosition = 0;
                            isPasswordWrong = false;
                            return;
                		}
                      incorrect();
                      passwordPosition = 0;
                      isPasswordWrong = false;
                      return;
                  }
      		unlock(); //Отключване на ключалката
      		passwordPosition = 0;
    	}
  	}
}

//Главен дисплей на програмата
void defaultdisplay()
{
  lcd.setCursor(1,0);
  lcd.print("Enter password");
  lcd.setCursor(2,1);
  lcd.print("to open lock");
}

//Метод, за правилно въведен код
void unlock()
{
  lcd.clear();
  digitalWrite(greenAllowed, HIGH);
  lcd.setCursor(0,0);
  lcd.print(" ");
  lcd.setCursor(1,0);
  lcd.print("Access Allowed");
  lcd.setCursor(2,1);
  lcd.print("Please Enter");
  
  //Servo се отключва
  for(servoPosition = 180; servoPosition >= 0; servoPosition-=5)
  {
    servo.write(servoPosition);
    delay(5);
  }
  delay(4000);
  digitalWrite(greenAllowed, LOW);
  
  delay(1000);
  //Извикваме метода за текст при повторното заключване
  automatedLock();
  
  //Servo се заключва
  for(servoPosition = 0; servoPosition <= 180; servoPosition +=5)
	{ 
		servo.write(servoPosition); 
		delay(5);
  	}
  	lcd.clear();
  	lcd.setCursor(5,0);
  	lcd.print("LOCKED");
  	delay(2000);
  
  lcd.clear();
  defaultdisplay();
}

//Метод при въвеждане на неправилна парола, където се изчаква 3 секунди
void incorrect()
{
  delay(500);
  digitalWrite(redDenied, 255);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("PASSWORD");
  lcd.setCursor(6,1);
  lcd.print("INCORRECT");
  delay(2000);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("ACCESS DENIED");
  lcd.setCursor(3,1);
  lcd.print("PLEASE WAIT");
  delay(3000);
  lcd.clear();
  digitalWrite(redDenied, LOW);
  defaultdisplay();
}

//Метод за текст при повторното заключване
void automatedLock()
{
  delay(1000);
  
  lcd.clear();
  digitalWrite(redDenied, 255);
  delay(200);
  lcd.setCursor(3,0);
  lcd.print("!WARNING!");
  delay(2000);
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Relocking in...");
  lcd.setCursor(7,1);
  lcd.print("5");
  delay(1000);
  
  lcd.setCursor(7,1);
  lcd.print("4");
  delay(1000);
  
  lcd.setCursor(7,1);
  lcd.print("3");
  delay(1000);
 
  lcd.setCursor(7,1);
  lcd.print("2");
  delay(1000);

  lcd.setCursor(7,1);
  lcd.print("1");
  delay(1000);
  
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("RELOCKING");
  digitalWrite(redDenied, 0);
  delay(1000);
}

// Метод за наказание след 5 грешни опита
void attempsPunish()
{
  delay(1000);
  digitalWrite(redDenied, HIGH);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("TOO MANY TRIES");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PLEASE WAIT FOR");
  lcd.setCursor(3,1);
  lcd.print("20 SECONDS");
  delay(20000);
  digitalWrite(redDenied, LOW);
  delay(2000);
  lcd.clear();
}