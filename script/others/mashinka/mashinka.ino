/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*******/
#define pY 1
#define mY -1
#define pX 10
#define mX -10

  int sizeMapX=0;
 int sizeMapY=0;
 
 class Kletka
        {  
			public:
			//номер клетки по X/Y
            int index_x=-1;
            int index_y=-1;
            //вес клектки для алгоритма Ли
            int W=-1;
            //Является ли клетка стеной 
			bool stena = false;
            //Является ли клетка финишем
            bool finish = false;
        };


 



	
//передача данных
String inString ;
int route[100][2];
int startX=0;
int startY=0;
//движение
int aim=pY;
int myCordX;
int myCordY;
int waycount=0;
Kletka pole[100];

int FinishY;
int FinishX;


// Motor A
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 

// Motor B
int motor2Pin1 = 33;
int motor2Pin2 = 25;
int enable2Pin = 32;

//длительность маневров
int moveForword=2800;
int moveRight=2300;
int moveLeft=2300;
int moveBack=2800;
int moveStop=1000;

//параметры управления dc двигателями
const int freqMove = 20000;
const int freqTurn = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycleMove = 180;
int dutyCycleTurn = 200;

void Turn(int reqAim)
{
int aX;
int aY;
int bX;
int bY;

aX=aim/10;
aY=aim%10;

bX=reqAim/10;
bY=reqAim%10;

int vector=aX*bY-bX*aY;
Serial.printf("vector:%d",vector);
if(vector<0) {MoveLeft();}
if(vector>0){MoveRight();}
if(vector==0)
{
if(aX!=bX&&aY!=bY)
{
MoveRight();
MoveRight();
}
else {Serial.println("Not turning");}  
  
}
}


void DebugPole()
{
	for(int i=0;i<sizeMapX*sizeMapY;i++)
	{Serial.printf("\nPole\n");
		
			Serial.printf("\nKletka [%d] X:[%d]Y:[%d] STENA:%d Wes:%d",i,pole[i].index_x,pole[i].index_y,pole[i].stena,pole[i].W);
	}
	delay(5000);
}



//Li
void Li(int StartX,int StartY)
{
	Serial.printf("\n!!!Protocol LI activated ");
int indexStart;
int indexFinish;
int indexSosed;

DebugPole();

	for(int i=0;i<sizeMapX*sizeMapY;i++)
	{
		
			if(pole[i].stena==true){Serial.printf("\nKletka STENA %d %d",pole[i].index_x,pole[i].index_y);}
			
			if(pole[i].index_x==StartX&&pole[i].index_y==StartY){indexStart=i;Serial.printf("\nKletkaStart %d",indexStart);}
			if(pole[i].index_x==FinishX&&pole[i].index_y==FinishY){indexFinish=i;Serial.printf("\nKletkaFinish %d   [%d][%d]",indexFinish,FinishX,FinishY);}
	}
	
			int countW = 0;
			
			

            pole[indexStart].W = 0;
			
			
			//пока не помеченна финишная точка 
			   while (pole[indexFinish].W==-1)
        {       
					for (Kletka kletka: pole)
                {
					
					DebugPole();
					 if (kletka.W==countW)
                    {
						Serial.printf("\n!Kletka! %d,%d",kletka.index_x,kletka.index_y);
						
						
						
						//справа
						 if (kletka.index_x < sizeMapX-1)
                        {
							//ищем индекс соседа
							for (int i=0;i<sizeMapX*sizeMapY;i++)
							{
								if(pole[i].index_x-1==kletka.index_x){indexSosed=i;Serial.printf("\Sosed Sparava %d, [%d][%d]",indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y);break;}
								
								
							}
							if(pole[indexSosed].stena!=true&&pole[indexSosed].W==-1)
							{
								
								int newW = countW + 1;
								Serial.printf("\Sparava Sosed newWes %d",newW);
								pole[indexSosed].W=newW;
								
							}
							else
							{
								Serial.printf("\nSpravo zanyato %d");
							}

						}
						else
						{
							Serial.printf("\nSoseda sprava net %d");
						}
						
						
						
						//слева
							 if (kletka.index_x >0)
                        {
							//ищем индекс соседа
							for (int i=0;i<sizeMapX*sizeMapY;i++)
							{
								if(pole[i].index_x+1==kletka.index_x){indexSosed=i;Serial.printf("\Sosed Sleva %d, [%d][%d]",indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y);break;}
								
								
							}
							if(pole[indexSosed].stena!=true&&pole[indexSosed].W==-1)
							{
								
								int newW = countW + 1;
								Serial.printf("\Sleva Sosed newWes %d",newW);
								pole[indexSosed].W=newW;
								
							}
							else
							{
								Serial.printf("\nSleva zanyato %d");
							}

						}
						else
						{
							Serial.printf("\nSoseda Sleva net %d");
						}
						
						
						//verkh
					
					if (kletka.index_y <sizeMapY-1)
                        {
							//ищем индекс соседа
							for (int i=0;i<sizeMapX*sizeMapY;i++)
							{
								if(pole[i].index_y-1==kletka.index_y){indexSosed=i;Serial.printf("\Sosed Verh %d, [%d][%d]",indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y);break;}
								
								
							}
							if(pole[indexSosed].stena!=true&&pole[indexSosed].W==-1)
							{
								
								int newW = countW + 1;
								Serial.printf("\Verh Sosed newWes %d",newW);
								pole[indexSosed].W=newW;
								
							}
							else
							{
								Serial.printf("\nSleva zanyato %d");
							}

						}
						else
						{
							Serial.printf("\nSoseda Verh net %d");
						}
						
						
						
						//niz
							if (kletka.index_y >0)
                        {
							//ищем индекс соседа
							for (int i=0;i<sizeMapX*sizeMapY;i++)
							{
								if(pole[i].index_y+1==kletka.index_y){indexSosed=i;Serial.printf("\Sosed Niz %d, [%d][%d]",indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y);break;}
								
								
							}
							if(pole[indexSosed].stena!=true&&pole[indexSosed].W==-1)
							{
								
								int newW = countW + 1;
								Serial.printf("\Niz Sosed newWes %d",newW);
								pole[indexSosed].W=newW;
								
							}
							else
							{
								Serial.printf("\nNiz zanyato %d");
							}

						}
						else
						{
							
							Serial.printf("\nSoseda Verh net %d");
						}
						
						
						
						
						
						
					}
				}
				
				countW++;
				
				Serial.printf("\n WES: %d\n",countW);
		}
}












//маневры
void MoveForword()
{
 // Вперед
   ledcSetup(pwmChannel, freqMove, resolution);
   ledcWrite(pwmChannel, dutyCycleMove); 
   ledcAttachPin(enable1Pin, pwmChannel);
   ledcAttachPin(enable2Pin, pwmChannel);
   Serial.println("Moving Forward");
   digitalWrite(motor1Pin1, LOW);
   digitalWrite(motor1Pin2, HIGH);
   digitalWrite(motor2Pin1, LOW);
   digitalWrite(motor2Pin2, HIGH);
   delay(moveForword); 
   MoveStop();
}

void MoveRight()
{
  //  "Вправо"
  ledcSetup(pwmChannel, freqTurn, resolution);
  ledcWrite(pwmChannel, dutyCycleTurn); 
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable2Pin, pwmChannel);
  Serial.println("Right");
  digitalWrite(motor1Pin1, LOW);  
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW); 
  delay(moveRight);  
  MoveStop();
}

void MoveLeft()
{ 
 //  "Влево"
  ledcSetup(pwmChannel, freqTurn, resolution); 
  ledcWrite(pwmChannel, dutyCycleTurn); 
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable2Pin, pwmChannel);
  Serial.println("Left");  
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
  delay(moveLeft);
  MoveStop();
 }
 
void MoveBack()
{
  // Назад
  ledcSetup(pwmChannel, freqMove, resolution); 
  ledcWrite(pwmChannel, dutyCycleMove); 
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable2Pin, pwmChannel);
  Serial.println("Moving Backwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW); 
  delay(moveBack);
  MoveStop();
  }

void MoveStop()
{
 // // Stop 
   Serial.println("Motor stopped");
   digitalWrite(motor1Pin1, LOW);
   digitalWrite(motor1Pin2, LOW);
   digitalWrite(motor2Pin1, LOW);
   digitalWrite(motor2Pin2, LOW);
   delay(moveStop);  
 }
 



void setup() {
  
  
  //задание пинов как выходов
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

//инициализация кнала связи
  Serial.begin(115200);
  Serial.flush();
  // testing
  Serial.print("Testing DC Motor...");
  
}

void loop() {



//Активация протоколов передачи координат
while (Serial.available() > 0) {
  int inChar = Serial.read();
    inString += (char)inChar; 
  if (inChar == '\n') 
  {
	  	
		Serial.println("New str:"); 
		Serial.println(inString); 

 }

      
  }
      
 if(inString.equals("Hello\n"))
 {

 Serial.println("Darou"); 
 inString="\0";
}





if(inString.equals("StartMap\n"))
{
	 Serial.println("StartMapOK"); 
	inString="\0";
	
	while(!inString.equals("EndSize\n"))
{

while (Serial.available() > 0) 
{
	
   int inChar = Serial.read();
  inString += (char)inChar; 
  if (inChar == '\n') 
  {
	   if(inString.equals("EndSize\n")){break;}
	   sizeMapX=int( inString[0])-48;
		sizeMapY=int(inString[2])-48;
		//Serial.println(inString); 
	inString="\0";
  }
  
}
  
  
}

	
Serial.printf("Size map: %d,%d",sizeMapX,sizeMapY);

	int countX=0;
	int countY=0;
	
int countKletka=0;

inString="\0";

	while(!inString.equals("EndMap\n"))
  {

while (Serial.available() > 0) 
{
 
  int inChar = Serial.read();
  inString += (char)inChar; 
  if (inChar == '\n') 
  {
	if(countX>sizeMapX-1){countX=0;countY++;}
	if(inString.equals("EndMap\n")){break;}
    pole[countKletka].index_x=countX;
	pole[countKletka].index_y=countY;
	if(inString.equals("Stena\n"))
	{
	pole[countKletka].stena=true;
	Serial.printf("Kletka: %d,%d-stena\n",pole[countKletka].index_x,pole[countKletka].index_y);
	}
	else
	{
	Serial.printf("Kletka: %d,%d-nestena\n",pole[countKletka].index_x,pole[countKletka].index_y);
	}
	countX++;
	countKletka++;
    inString="\0";
 }
	
}

  }
  Serial.printf("map OK\n");

}



if(inString.equals("FinishPoint\n"))
 {
	  Serial.println("FinishReady"); 
	inString="\0";


while(!inString.equals("GO\n"))
  {
	 

while (Serial.available() > 0) 
{   
	 
	
int inChar = Serial.read();
  inString += (char)inChar; 
   if (inChar == '\n') 
  {
	  
   if(inString.equals("GO\n")){break;}

   FinishX=int( inString[0])-48;
   FinishY=int(inString[2])-48;
   inString="\0";
  }
  
}


  }
Serial.printf("Finish in point: %d,%d\n",FinishX,FinishY);
Li(1,0);
 }
 
 
















//отправка стартовых координат
if(inString.equals("StartCoordinat\n"))
 {
 Serial.println("CordBegin"); 
 Serial.println("0.0");
 Serial.println("CordEnd");  
}

//прием координат пути
 if(inString.equals("StartMarshrut\n"))
 {

for(int i=0;i<100;i++)
{
route[i][0]=-1;
route[i][1]=-1;
}

 int countcoord=0; 

inString="\0";
  while(!inString.equals("EndMarshrut\n"))
  {

while (Serial.available() > 0) 
{
 
  int inChar = Serial.read();
    inString += (char)inChar; 
  if (inChar == '\n') 
  {
    if(inString.equals("EndMarshrut\n")){break;}
    route[countcoord][0]=int( inString[0])-48;
    route[countcoord][1]=int(inString[2])-48;
    countcoord++; 
    Serial.print("String: ");
    Serial.println(inString);
    inString="\0";
 }
}


   
  
}

for(int i=0;i<100;i++)
{
Serial.printf("%d.%d\n",route[i][0],route[i][1]);
if(route[i+1][0]==-1 && route[i+1][1]==-1)
{
  waycount=i;
  break;

}
}


Serial.println("Go ehat?"); 
} 
 

//
 if(inString.equals("Go\n"))
 {
  
int index=0;

myCordX=startX;
myCordY=startY;



while(myCordX!=route[waycount][0] || myCordY!=route[waycount][1])
{

int bufX=route[index][0]-myCordX;  
int bufY=route[index][1]-myCordY;

if(bufX!=0)
{
if(bufX<0)
{
Turn(mX);
aim=mX;
}

if(bufX>0)
{
Turn(pX);
aim=pX;
}  
}

///
if(bufY!=0)
{
  
if(bufY<0)
{
Turn(mY);
aim=mY;
}

if(bufY>0)
{
Turn(pY);
aim=pY;
}  
}  


MoveForword();

myCordX=route[index][0];
myCordY=route[index][1];
Serial.printf("myCoord:%d,%d:",myCordX,myCordY);


index++; 
}
Serial.printf("\n\nmyCoordFinish:%d,%d:",myCordX,myCordY);
MoveStop();

 }
 
 
 
 inString = "\0"; 
 // MoveStop();
  //MoveForword();
 // MoveStop();
  //MoveLeft(); 
  
  
  // Move DC motor forward with increasing speed
/*
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  while (dutyCycle <= 255){
    ledcWrite(pwmChannel, dutyCycle);   
    Serial.print("Forward with duty cycle: ");
    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    delay(500);

  }
 */
  //dutyCycle = 200;
}

