/*********
Министерство науки и высшего образования Российской Федерации
ФГАОУ ВО «УрФУ имени первого Президента России Б.Н. Ельцина»
Институт Радиоэлектроники и информационных технологий
Департамент Радиоэлектроники и связи

Разработанно в ходе работы над ВКР на тему 
Разработка алгоритмов и выбор аппаратных средств для системы навигации на основе технологии Bluetooth 

Студентами 
Берзоном Д.И
Медниковым А.А
Направление подготовки Инфокоммуникационные технологии и системы связи

Данный документ является интеллектуальной собственностью. 
Охраняется в соответствии с Законом о защите прав интеллектуальной собственности. 
Любое копирование и несанкционированное использование документа без согласия авторов ЗАПРЕЩЕНО.
*******/

#include <BLEDevice.h> 
#include <BLEUtils.h> 
#include <BLEScan.h> 
#include <BLEAdvertisedDevice.h> 
#include <BLEUUID.h> 

//вектора направления 
#define pY 1
#define mY -1
#define pX 10
#define mX -10

//Выходы мотора А
#define motor1Pin1 27
#define motor1Pin2 26
#define enable1Pin 14

//Выходы мотора Б
#define motor2Pin1 33
#define motor2Pin2 25
#define enable2Pin 32


//ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ИСПОЛЬЗУЕТСЯ ДЛЯ УПРОЩЕНИЯ АРХИТЕКТУРЫ КОДА
//ГЛОБАЛЬНО ОБЪЯВЛЕНЫ ТЕ ПЕРЕМЕННЫЕ КОТОРЫЕ ИСПОЛЬЗУЮТСЯ ВО МНОГИХ ЧАСТЯХ ИСХОДНОГО КОДА  

//Массив эталонных значений показателя сигналов от маячков
int RSSI_1m[10]={-65,-65,-65,-65,-65,-65,-65,-65,-65,-65};

//Счетчик циклов трилатерации 
int countTrilateration;
//Массви итоговых координат полученных с помощью алгоритма трилатерации  0-X 1-Y
float * itogArray=  new float[2];

//Переменные для хранения текущих координат обьекта 
float curentX;
float curentY;
//Переменные для хранения стартовых координат обьекта 
int startX;
int startY;

//Данные стартовой конфигурации 
int sizeMapX=0;
int sizeMapY=0;
int FinishY;
int FinishX;
bool Configure=false;

//КЛАССЫ
//В ДАННОМ ИСХОДНОМ КОДЕ ОПИСАННЫ 7 КЛАССОВ

//POINT-Класс для работы с декартовыми координатами 
//X-координата X
//Y-координата Y
int route[100][2];
int aim=pY;
class Point
{
 public:   
  int X;
  int Y;
  Point(){}
  Point(int x,int y)
  {
    X=x;
    Y=y;
    }
};

//WAY-обьект класса описывает путь
//Используется для хранения множетсва путей 
//coord[30]- массив координат пути
//countTurn- кол-во поворотов в пути
//countCoor- кол-во координат в пути
class Way
{   //номер клетки по X/Y
	public:
		Point coord[30];
		int countTurn;
		int countCoord;
};

//NABOR-обьект класса описывает набор координат с одним весом  
//используется для хранения множетсва наборов координат с различными вессами
//coord[30]- массив координат клеток
//W-все клеток в наборе
//countKletka- кол-во клеток в наборе
class Nabor
{
	public:
		Point coord[20];
		int W=-1;
		int countKletka = 0;
		Nabor()
		{
			for (int i = 0; i < 20; i++)
			{
				coord[i].X = -1;
				coord[i].Y = -1;

			}

		}

};


//KLETKA - обьект класса оописывает клетку локльной области навигации  
//используется для хранения множетсва клеток составляющих карту
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

class Combination
{
  public :
  int id1;
  int id2;
  int id3;
};

Combination combin[10];

class KalmanFilter
{
  public:
  int F_kalman;
  int H_kalman;
  int R_kalman;
  int Q_kalman;
  double  X0_kalman;
  double  P0_kalman;
  int State;
  double  Covariance;
  KalmanFilter(int F,int H, int R,int Q)
  {
   F_kalman=F;
   H_kalman=H;
   R_kalman=R;
   Q_kalman=Q;
   
  }
  void SetState(double state,double covariance)
  {
    State=state;
    Covariance=covariance;
  }
  int Correct(int RSSI_data)
  {
    X0_kalman=F_kalman*State;
    P0_kalman=F_kalman*Covariance*F_kalman+Q_kalman;
    double K= H_kalman*P0_kalman/(H_kalman*P0_kalman*H_kalman+R_kalman);
    State=round(X0_kalman+K*(RSSI_data-H_kalman*X0_kalman));
    Covariance=(1-K*H_kalman)*P0_kalman; 
    return State;
  } 


};

KalmanFilter filters[10]= KalmanFilter(1,1,150,10);

class Beacons
{
public:
int id;
bool active;
int rssi[10];
int filtered_RSSI[10];
double distance[10];
int count;
int myX;
int myY;
Beacons()
{
  id=0;
  for(int i=0;i<10;i++)
  {
    rssi[i]=0;
  }
 count=0;
}
} ;

Beacons beacon[10];

String key="beacon";

int scanTime = 1; //In seconds 
BLEScan* pBLEScan; 

bool Auth(String NameBeacon)
{
NameBeacon=NameBeacon.substring(0,6);
return NameBeacon.equals(key);

}


double GetDistance(int RSSI_beacon,int RSSI_1m)
{
    
    double stepen=(RSSI_1m-RSSI_beacon)/20.0;
    double  distance=pow(10,stepen);
  return distance;


}


int GetId(String NameBeacon)
{
int id; 
char charBufVar[8]; 
NameBeacon.toCharArray(charBufVar, 8); 
charBufVar[7]='\0';
id=charBufVar[6]-'0'; 
return id;
}


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks { 
void onResult(BLEAdvertisedDevice advertisedDevice) 
{ 

String NameBeacon= advertisedDevice.getName().c_str();

if(Auth(NameBeacon))
{
  
Serial.println("\nauth success");
int id=GetId(NameBeacon); 
Serial.print("Find:");
Serial.println(NameBeacon);
beacon[id].count++;
Serial.printf("count: %d\n", beacon[id].count);
Serial.printf("ID: %d", id); 
for(int i=9;i>0;i--)
{
  beacon[id].rssi[i]=beacon[id].rssi[i-1];
  beacon[id].filtered_RSSI[i]=beacon[id].filtered_RSSI[i-1];
  beacon[id].distance[i]=beacon[id].distance[i-1];
}
int beaconRSSI=advertisedDevice.getRSSI();
if(beacon[id].count==1){filters[id].SetState(beaconRSSI,0.1); }

beacon[id].rssi[0]=beaconRSSI;
beacon[id].filtered_RSSI[0]=filters[id].Correct(beaconRSSI);
beacon[id].distance[0]=GetDistance(beacon[id].filtered_RSSI[0],RSSI_1m[id]);
beacon[id].active=true;

for(int i=0;i<10;i++)
{
  Serial.printf("\nRSSI[%d]=%d",i,beacon[id].rssi[i]); 
  Serial.printf("\n[%d],%d,%d,%f",RSSI_1m[id],beacon[id].rssi[i],beacon[id].filtered_RSSI[i],beacon[id].distance[i]); 
}
//Serial.printf("\nRSSI: %d ", beaconRSSI); 
//Serial.printf("\nMAC: %s \n", advertisedDevice.getAddress().toString().c_str()); 
 

}



//BLEUUID uuid=advertisedDevice.getServiceUUID(); 

//Serial.printf("uuid: %s \n", advertisedDevice.getServiceUUID().toString().c_str()); 

// Serial.printf("UUID: %s \n", advertisedDevice.getServiceUUID().toString().c_str()); 
} 

//Serial.printf("Name: %s \n", advertisedDevice.getName().c_str()); 

}; 

//<GLOBAL VARIABLE>


Kletka pole[100];
//Параметры пути

Way listway[40];

//</GLOBAL VARIABLE>

//функция расчета направелния поворота и его выполнения
void Turn(int reqAim,int aim)
{
int moveStop=1000;
int moveRight=2300;
int moveLeft=2300;
const int freqTurn = 30000;
int dutyCycleTurn = 200;
const int pwmChannel = 0;
const int resolution = 8;
int aX;
int aY;
int bX;
int bY;

aX=aim/10;
aY=aim%10;

bX=reqAim/10;
bY=reqAim%10;

int vector=aX*bY-bX*aY;
Serial.printf("\nA vector:%d,%d",aX,aY);
Serial.printf("\nB vector:%d,%d",bX,bY);
Serial.printf("\nvector:%d\n",vector);
if(vector<0) {MoveLeft( pwmChannel, freqTurn, resolution, dutyCycleTurn, moveLeft,moveStop);}
if(vector>0){MoveRight( pwmChannel, freqTurn, resolution, dutyCycleTurn, moveRight,moveStop);}
if(vector==0)
{
if(aX!=bX||aY!=bY)
{
MoveRight( pwmChannel, freqTurn, resolution, dutyCycleTurn, moveRight,moveStop);
MoveRight( pwmChannel, freqTurn, resolution, dutyCycleTurn, moveRight,moveStop);
}
else {Serial.println("Not turning");}  
  
}
}

//служебная функция вывода в последовательный порт информации о карте
void DebugPole()
{
  Serial.printf("\nPole\n");
	for(int i=0;i<sizeMapX*sizeMapY;i++)
	{
		
			Serial.printf("\nKletka [%d] X:[%d]Y:[%d] STENA:%d Wes:%d",i,pole[i].index_x,pole[i].index_y,pole[i].stena,pole[i].W);
	}


}


//функция принятия данных по настройке системы через последовательный порт
void StartConfugure()
{
	
	//Передача данных
String inString ;
	while(true)
	{
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

	
Serial.printf("Size map: %d,%d\n",sizeMapX,sizeMapY);

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
inString="\0";






}


if(inString.equals("StartBeacon\n"))
{
	Serial.println("StartBeacon\n"); 
	inString="\0";
	int countBeacon=1;
while(!inString.equals("EndBeacon\n"))
{


while (Serial.available() > 0) 
{
	
   int inChar = Serial.read();
  inString += (char)inChar; 
  if (inChar == '\n') 
  {
	   if(inString.equals("EndBeacon\n")){break;}
	   beacon[countBeacon].id=countBeacon;
	   beacon[countBeacon].myX=int(inString[0])-48;
	   beacon[countBeacon].myY=int(inString[2])-48;
	   int decRSSI=int( inString[5])-48;
	   int edRSSI=int( inString[6])-48;
	   RSSI_1m[countBeacon]=-(decRSSI*10+edRSSI);  
	   
	   Serial.printf("Beacon,id:%d/x:%d/y:%d/rssi:%d\n",beacon[countBeacon].id,beacon[countBeacon].myX,beacon[countBeacon].myY,RSSI_1m[countBeacon]); 
	   inString="\0";
	   countBeacon++;
  }
  
}
  
  
}

break;
}



}//while
}//func



void FinishReceive()
{
	
	String inString ;
	while(true)
	{
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

for(int i=0;i<100;i++)
{
route[i][0]=-1;
route[i][1]=-1;
}

break;
}
inString="\0";
	}	
	
	
}


//Algoritm Li- функция релизующая алгоритм ЛИ и поиск оптимального пути из всех кртчайших возможных путей
void Li(int StartX,int StartY)
{
	
	int countX = 0;
	int countY = 0;
	int countK = 0;
	for (int j = 0; j < sizeMapY; j++)
	{
		for (int i = 0; i < sizeMapX; i++)
		{
			pole[countK].index_x = countX;
			pole[countK].index_y = countY;
			countX++;
			countK++;


		}
		countY++;
		countX = 0;
	}
	Serial.printf("\n!!!Protocol LI activated ");
	int indexStart;
	int indexFinish;
	int indexSosed;

	DebugPole();

	for (int i = 0; i < sizeMapX*sizeMapY; i++)
	{

		if (pole[i].stena == true) { Serial.printf("\nKletka STENA %d %d", pole[i].index_x, pole[i].index_y); }

		if (pole[i].index_x == StartX && pole[i].index_y == StartY) { indexStart = i; Serial.printf("\nKletkaStart %d", indexStart); }
		if (pole[i].index_x == FinishX && pole[i].index_y == FinishY) { indexFinish = i; Serial.printf("\nKletkaFinish %d   [%d][%d]", indexFinish, FinishX, FinishY); }
	}

	int countW = 0;



	pole[indexStart].W = 0;


	//пока не помеченна финишная точка 
	while (pole[indexFinish].W == -1)
	{
		for (Kletka kletka : pole)
		{


			if (kletka.W == countW)
			{
				DebugPole();
				Serial.printf("\n!Kletka! %d,%d", kletka.index_x, kletka.index_y);



				//справа
				if (kletka.index_x < sizeMapX - 1)
				{
					//ищем индекс соседа
					for (int i = 0; i < sizeMapX*sizeMapY; i++)
					{
						if (pole[i].index_x - 1 == kletka.index_x&&pole[i].index_y == kletka.index_y) { indexSosed = i; Serial.printf("\nSosed Sparava %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }


					}
					if (pole[indexSosed].stena != true && pole[indexSosed].W == -1)
					{

						int newW = countW + 1;
						Serial.printf("\Sparava Sosed newWes %d", newW);
						pole[indexSosed].W = newW;

					}
					else
					{
						Serial.printf("\nSpravo zanyato");
					}

				}
				else
				{
					Serial.printf("\nSoseda sprava net");
				}



				//слева
				if (kletka.index_x > 0)
				{
					//ищем индекс соседа
					for (int i = 0; i < sizeMapX*sizeMapY; i++)
					{
						if (pole[i].index_x + 1 == kletka.index_x&&pole[i].index_y == kletka.index_y) { indexSosed = i; Serial.printf("\nSosed Sleva %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }


					}
					if (pole[indexSosed].stena != true && pole[indexSosed].W == -1)
					{

						int newW = countW + 1;
						Serial.printf("\Sleva Sosed newWes %d", newW);
						pole[indexSosed].W = newW;

					}
					else
					{
						Serial.printf("\nSleva zanyato");
					}

				}
				else
				{
					Serial.printf("\nSoseda Sleva net");
				}


				//verkh

				if (kletka.index_y < sizeMapY - 1)
				{
					//ищем индекс соседа
					for (int i = 0; i < sizeMapX*sizeMapY; i++)
					{
						if (pole[i].index_y - 1 == kletka.index_y&&pole[i].index_x == kletka.index_x) { indexSosed = i; Serial.printf("\nSosed Verh %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }


					}
					if (pole[indexSosed].stena != true && pole[indexSosed].W == -1)
					{

						int newW = countW + 1;
						Serial.printf("\Verh Sosed newWes %d", newW);
						pole[indexSosed].W = newW;

					}
					else
					{
						Serial.printf("\nVerh zanyato");
					}

				}
				else
				{
					Serial.printf("\nSoseda Verh net");
				}



				//niz
				if (kletka.index_y > 0)
				{
					//ищем индекс соседа
					for (int i = 0; i < sizeMapX*sizeMapY; i++)
					{
						if (pole[i].index_y + 1 == kletka.index_y&&pole[i].index_x == kletka.index_x) { indexSosed = i; Serial.printf("\nSosed Niz %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }


					}
					if (pole[indexSosed].stena != true && pole[indexSosed].W == -1)
					{

						int newW = countW + 1;
						Serial.printf("\Niz Sosed newWes %d", newW);
						pole[indexSosed].W = newW;

					}
					else
					{
						Serial.printf("\nNiz zanyato");
					}

				}
				else
				{

					Serial.printf("\nSoseda Niz net");
				}






			}
		}

		countW++;

		Serial.printf("\n WES: %d\n", countW);
	}

	Serial.printf("\n Wesu Prostavlenun");

	Serial.printf("\n\n\nProtocol Vostanovlenie\n\n\n");

	Nabor naboru[25];
	bool smena = false;

	bool cycle = true;
	int countShag = 0;
	int countKletka = 0;
	int indexKletka = indexFinish;


	int indexX = FinishX;
	int indexY = FinishY;
	int countNabor = 0;
	//pole[countW].W;


	countNabor = 0;
	naboru[countNabor].W = countW;
	naboru[countNabor].coord[0].X = indexX;
	naboru[countNabor].coord[0].Y = indexY;
	naboru[countNabor].countKletka++;
	countNabor++;
	countW--;

	while (cycle)
	{


		for (int i = 0; i < sizeMapX*sizeMapY; i++)
		{
			if (pole[i].index_x  == indexX && pole[i].index_y == indexY) { indexKletka = i; Serial.printf("\nNovaya kletka  %d, [%d][%d]", indexKletka, pole[indexKletka].index_x, pole[indexKletka].index_y); break; }
		}




		//прав
		if (indexX < sizeMapX - 1)
		{
			for (int i = 0; i < sizeMapX*sizeMapY; i++)
			{
				if (pole[i].index_x - 1 == indexX && pole[i].index_y == indexY) { indexSosed = i; Serial.printf("\nSosed Sparava  %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }
			}

			if (pole[indexSosed].W == pole[indexKletka].W - 1)
			{
				bool find = false;
				Point kletkaCoord;
				kletkaCoord.X = indexX + 1;
				kletkaCoord.Y = indexY;
				for (Nabor n : naboru)
				{
					if (n.W == countW)
					{
						naboru[countNabor - 1].coord[naboru[countNabor - 1].countKletka] = kletkaCoord;
						naboru[countNabor - 1].countKletka++;
						find = true;
						break;
					}
				}
				if (!find) {
					naboru[countNabor].W = countW; naboru[countNabor].coord[0] = kletkaCoord; naboru[countNabor].countKletka++; countNabor++;
				}
			}

			
		}


		//левый
		if (indexX > 0)
		{

			//ищем индекс соседа
			for (int i = 0; i < sizeMapX*sizeMapY; i++)
			{
				if (pole[i].index_x + 1 == indexX && pole[i].index_y == indexY) { indexSosed = i; Serial.printf("\nSosed Sleva %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }

			}

			if (pole[indexSosed].W == pole[indexKletka].W - 1)
			{
				bool find = false;
				Point kletkaCoord;
				kletkaCoord.X = indexX - 1;
				kletkaCoord.Y = indexY;
				for (Nabor n : naboru)
				{
					if (n.W == countW) {
						naboru[countNabor - 1].coord[naboru[countNabor - 1].countKletka] = kletkaCoord;
						naboru[countNabor - 1].countKletka++;
						find = true;
						break;
					}
				}

				if (!find) {
					naboru[countNabor].W = countW; naboru[countNabor].coord[0] = kletkaCoord; naboru[countNabor].countKletka++; countNabor++;

				}



			}
		}
		//верх
		if (indexY < sizeMapY - 1)
		{
			//ищем индекс соседа
			for (int i = 0; i < sizeMapX*sizeMapY; i++)
			{
				if (pole[i].index_y - 1 == indexY && pole[i].index_x == indexX) { indexSosed = i;Serial.printf("\nSosed Verh %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }


			}

			if (pole[indexSosed].W == pole[indexKletka].W - 1)
			{
				bool find = false;
				Point kletkaCoord;
				kletkaCoord.X = indexX;
				kletkaCoord.Y = indexY + 1;
				for (Nabor n : naboru)
				{
					if (n.W == countW) {
						naboru[countNabor - 1].coord[naboru[countNabor - 1].countKletka] = kletkaCoord;
						naboru[countNabor - 1].countKletka++;
						find = true;
						break;
					}
				}
				if (!find) {
					naboru[countNabor].W = countW; naboru[countNabor].coord[0] = kletkaCoord; naboru[countNabor].countKletka++; countNabor++;

				}
			}


		}


		//низ
		if (indexY > 0)
		{
			//ищем индекс соседа
			for (int i = 0; i < sizeMapX*sizeMapY; i++)
			{
				if (pole[i].index_y + 1 == indexY && pole[i].index_x == indexX) { indexSosed = i; Serial.printf("\nSosed Niz %d, [%d][%d]", indexSosed, pole[indexSosed].index_x, pole[indexSosed].index_y); break; }


			}

			if (pole[indexSosed].W == pole[indexKletka].W - 1)
			{
				bool find = false;
				Point kletkaCoord;
				kletkaCoord.X = indexX;
				kletkaCoord.Y = indexY - 1;
				for (Nabor n : naboru)
				{
					if (n.W == countW) {
						naboru[countNabor - 1].coord[naboru[countNabor - 1].countKletka] = kletkaCoord;
						naboru[countNabor - 1].countKletka++;
					
						find = true;
						break;
					}
				}
				if (!find) {
					naboru[countNabor].W = countW; naboru[countNabor].coord[0] = kletkaCoord; naboru[countNabor].countKletka++; countNabor++;

				}
			}


		}

		countKletka++;
		Serial.printf("countNABOR:%d\n", countNabor);

		for (Nabor n : naboru)
		{

			
			if (countW == 0) {
				cycle = false; break;
			}

			if (n.W == countW + 1)
			{
				if (n.countKletka == countKletka) {
					countW--;

					for (Nabor n2 : naboru)
					{
						if (n2.W == countW + 1)
						{
							
							countKletka = 0;
							indexX = n2.coord[countKletka].X;
							indexY = n2.coord[countKletka].Y;
							


						}



					}


				}

				else {
					indexX = n.coord[countKletka].X;
					indexY = n.coord[countKletka].Y;

				}




			}

		}


	}
	/////////////////////////////////////////////////////////////////////
	
	
	Serial.printf("\nWhileEnd");
	countNabor = 0;
	for(Nabor n : naboru)
	{
		
		for (int i = 0; i < n.countKletka - 1; i++)
		{
			for (int j = 0; j < n.countKletka; j++)
			{
				if (j != i)
				{
					if (n.coord[i].X == n.coord[j].X&&n.coord[i].Y == n.coord[j].Y) 
					{
						n.coord[j].X = -1;
						n.coord[j].Y = -1;
						naboru[countNabor].coord[j].X =-1;
						naboru[countNabor].coord[j].Y =-1;
					}
				}
			}
		}
		countNabor++;
	}

	for (Nabor n : naboru)
	{
		Serial.printf("\nNabor wesov %d\n", n.W);
		int i = 0;
		while (i<n.countKletka)
		{
			Serial.printf("[%d][%d]", n.coord[i].X, n.coord[i].Y);
			i++;
		}


	}



	countNabor = 0;

	for(Nabor n : naboru)
	{
		int NewCount = 0;
		int lastElement = 0;
		
		for (int i = 0; i < n.countKletka; i++)
		{
			if (n.coord[i].X != -1&& n.coord[i].Y != -1)
			{
				if (lastElement != i)
				{
					naboru[countNabor].coord[lastElement].X = naboru[countNabor].coord[i].X;
					naboru[countNabor].coord[lastElement].Y = naboru[countNabor].coord[i].Y;
					naboru[countNabor].coord[i].X = -1;
					naboru[countNabor].coord[i].Y = -1;

					n.coord[lastElement] = n.coord[i];
					n.coord[i].X = -1;
					n.coord[i].Y = -1;
				}
				lastElement++;
			}

		}
		for (int i = 0; i < n.countKletka; i++)
		{
			if (n.coord[i].X == -1 && n.coord[i].Y == -1)
			{
				break;
			}
			else
			{
				NewCount++;
			}
		}

		n.countKletka = NewCount;
		naboru[countNabor].countKletka = NewCount;
		countNabor++;
	}


	for (Nabor n : naboru)
	{
		Serial.printf("Nabor wesov %d\n",n.W);
		
		int i = 0;
		while (n.coord[i].X != -1)
		{
			Serial.printf("[%d][%d]", n.coord[i].X, n.coord[i].Y);
			i++;
		}
	

	}

	int indexW = 0;
	int indexKletkiW = 0;
	int indexKletfiF = 0;
	bool Zanovo = true;


for(int i=0;i<40;i++)
{
	for(int j=0;j<30;j++)
	{
		listway[i].coord[j].X=-1;
		listway[i].coord[j].Y=-1;
	}
	
}


	int schWay = 0;
	int a=0;
	countNabor = 0;


	//start
	Point coord;
	coord.X = StartX;
	coord.Y = StartY;
	
	while (true)
	{
		if (naboru[countNabor].coord[0].X == -1) { break; }
		countNabor++;

	}

	for (int j = 0; j < naboru[countNabor - 1].countKletka; j++)
	{

		Point coordKletki;
		coordKletki.X = naboru[countNabor - 1].coord[j].X;
		coordKletki.Y = naboru[countNabor - 1].coord[j].Y;
		Serial.printf("coordKletki:%d,%d\n",coordKletki.X,coordKletki.Y);
		if ((coord.X == coordKletki.X && abs(coord.Y - coordKletki.Y) == 1) || (coord.Y == coordKletki.Y && abs(coord.X - coordKletki.X) == 1))
		{
			
			
			
			listway[schWay].coord[0].X = coord.X;
			listway[schWay].coord[0].Y = coord.Y;
			
			listway[schWay].coord[1].X = coordKletki.X;
			listway[schWay].coord[1].Y = coordKletki.Y;
			listway[schWay].countCoord = 2;
			
			
			schWay++;
			
		
		}

	}
	
Serial.printf("count:%d\n",countNabor);
	int indexStroki = 2;


	while (Zanovo == true)

	{
		bool first = true;
		int countWay = 0;
		int c=0;
		bool end=false;
		
		

		while(end!=true)
		{
			
			if (listway[c].coord[0].X == -1) { end=true;}
			else{countWay++;}
			c++;		
		}
	
Serial.printf("countWay:%d\n",countWay);

		for (int i = 0; i < countWay; i++)
		{
			Way strWay = listway[i];
			coord.X = strWay.coord[strWay.countCoord - 1].X;
			coord.Y = strWay.coord[strWay.countCoord - 1].Y;
			
			for (int j = 0; j < naboru[countNabor - indexStroki].countKletka; j++)
			{
				Point coordKletki;
				coordKletki.X = naboru[countNabor - indexStroki].coord[j].X;
				coordKletki.Y = naboru[countNabor - indexStroki].coord[j].Y;
				
				if ((coord.X == coordKletki.X && abs(coord.Y - coordKletki.Y) == 1) || (coord.Y == coordKletki.Y && abs(coord.X - coordKletki.X) == 1))
				{
					
					if (first)
						
					{
						
					
						listway[i].coord[listway[i].countCoord].X = coordKletki.X;
						listway[i].coord[listway[i].countCoord].Y = coordKletki.Y;
						
						listway[i].countCoord++;
						
						strWay.coord[strWay.countCoord].X = coordKletki.X;
						strWay.coord[strWay.countCoord].Y = coordKletki.Y;
						
						strWay.countCoord++;
						first = false;
					}
					
					else
					{

						for (int k = 0; k < strWay.countCoord; k++)
						{
							listway[schWay].coord[k].X = strWay.coord[k].X;
							listway[schWay].coord[k].Y = strWay.coord[k].Y;
							

						}
						listway[schWay].countCoord = strWay.countCoord;
						listway[schWay].countTurn = strWay.countTurn;
						listway[schWay].coord[strWay.countCoord - 1].X = coordKletki.X;
						listway[schWay].coord[strWay.countCoord - 1].Y = coordKletki.Y;
						schWay++;

					}

				}
				

			}
			
			first = true;
			//indexStroki++;
			
		}
		indexStroki++;
		if (indexStroki < countNabor+1) {  }
		else {
			Zanovo = false; 
		}
	}
	
Serial.printf("debug\n");

	
	for (int i = 0; i < schWay; i++)
	{
		Serial.printf("\nWay %d\n", i);
		for (int j = 0; j < listway[i].countCoord; j++)
		{
			Serial.printf("[%d][%d];", listway[i].coord[j].X, listway[i].coord[j].Y);
		}
	}
	
	
	
	int countRoute=0;
	

	for(Way Route : listway)
	{
		listway[countRoute].countTurn = 0;
		bool plusY = true;
		bool minusY = false;
		bool plusX = false;
		bool minusX = false;
		for (int i = 0; i < Route.countCoord - 1; i++)
		{
			//y+
			if (Route.coord[i].Y < Route.coord[i + 1].Y)
			{
				if (!plusY)
				{
					listway[countRoute].countTurn++;
					Route.countTurn++;
				}
				plusY = true;
				minusY = false;
				plusX = false;
				minusX = false;
			}

			//y-
			if (Route.coord[i].Y > Route.coord[i + 1].Y)
			{
				if (!minusY)
				{
					listway[countRoute].countTurn++;
					Route.countTurn++;
				}
				plusY = false;
				minusY = true;
				plusX = false;
				minusX = false;
			}

			//x+
			if (Route.coord[i].X < Route.coord[i + 1].X)
			{
				if (!plusX)
				{
					listway[countRoute].countTurn++;
					Route.countTurn++;
				}
				plusY = false;
				minusY = false;
				plusX = true;
				minusX = false;
			}

			//x-
			if (Route.coord[i].X > Route.coord[i + 1].X)
			{
				if (!plusY)
				{
					listway[countRoute].countTurn++;
					Route.countTurn++;
				}
				plusY = false;
				minusY = false;
				plusX = false;
				minusX = true;
			}



		}
		countRoute++;


	}
	
	int turnIndex = 0;
	for (int i = 0; i < schWay; i++)
	{

		if (listway[turnIndex].countTurn > listway[i].countTurn)
		{
			turnIndex = i;
		}

	}

	int coordCount=1;
		Serial.printf("\n\n\n Optimal Way %d; CountTurn:%d\n", turnIndex,listway[turnIndex].countTurn);
		for (int j = 0; j < listway[turnIndex].countCoord-1; j++)
		{
			    route[j][0]=listway[turnIndex].coord[coordCount].X;
				route[j][1]=listway[turnIndex].coord[coordCount].Y;
				coordCount++;
			Serial.printf("[%d][%d];", listway[turnIndex].coord[j].X, listway[turnIndex].coord[j].Y);
		}




}


//функция управления движением объекта  
void Move()
{

	
	
//длительность маневров
int moveForwords=2800;
int moveBack=2800;
int moveStop=1000;

//параметры управления dc двигателями
const int freqMove = 20000;

const int pwmChannel = 0;
const int resolution = 8;
int dutyCycleMove = 180;
	
	
//движение

int myCordX;
int myCordY;
int waycount=0;

// Motor A

	
for(int i=0;i<100;i++)
{
Serial.printf("%d.%d\n",route[i][0],route[i][1]);
if(route[i+1][0]==-1 && route[i+1][1]==-1)
{
  waycount=i;
  break;

}
}

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
Turn(mX,aim);
aim=mX;
Serial.printf("\nnewaim:%d\n",aim);
}

if(bufX>0)
{
Turn(pX,aim);
aim=pX;
Serial.printf("\nnewaim:%d\n",aim);
}  
}

///
if(bufY!=0)
{
  
if(bufY<0)
{
Turn(mY,aim);
aim=mY;
Serial.printf("\nnewaim:%d\n",aim);
}

if(bufY>0)
{
Turn(pY,aim);
aim=pY;
Serial.printf("\nnewaim:%d\n",aim);
}  
}  


MoveForword( pwmChannel, freqMove, resolution, dutyCycleMove, moveForwords,moveStop);

myCordX=route[index][0];
myCordY=route[index][1];
Serial.printf("myCoord:%d,%d:",myCordX,myCordY);
Serial.printf("myaim:%d",aim);
detectCurentCoord();
		for(int i=0;i<10;i++)
{
	beacon[i].count=0;
	for(int j=0;j<10;j++)
{
beacon[i].rssi[j]=0;
beacon[i].filtered_RSSI[j]=0;
beacon[i].distance[j]=0;
}
}
index++; 
}
Serial.printf("\n\nmyCoordFinish:%d,%d:",myCordX,myCordY);
startX=myCordX;
startY=myCordY;
MoveStop(moveStop);
}


//маневры- фуекции отвечающие ха управление моторами объекта
void MoveForword(int pwmChannel,int freqMove,int resolution,int dutyCycleMove,int moveForword,int moveStop)
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
   MoveStop(moveStop);
}

void MoveRight(int pwmChannel,int freqTurn,int resolution,int dutyCycleTurn,int moveRight,int moveStop)
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
  MoveStop(moveStop);
}

void MoveLeft(int pwmChannel,int freqTurn,int resolution,int dutyCycleTurn,int moveLeft,int moveStop)
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
  MoveStop(moveStop);
 }
 
void MoveBack(int pwmChannel,int freqMove,int resolution,int dutyCycleMove,int moveBack,int moveStop)
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
  MoveStop(moveStop);
  }

void MoveStop(int moveStop)
{
 // // Stop 
   Serial.println("Motor stopped");
   digitalWrite(motor1Pin1, LOW);
   digitalWrite(motor1Pin2, LOW);
   digitalWrite(motor2Pin1, LOW);
   digitalWrite(motor2Pin2, LOW);
   delay(moveStop);  
 }
 
void ClearMemory()
{ 
Serial.println("\n!!!Clear!!!\n");
	//очистка весов клеток
	for(int i=0;i<100;i++)
	{
		pole[i].W=-1;
	}
	for(int i=0;i<40;i++)
	{
		listway[i].countTurn=0;
		listway[i].countCoord=0;
		for(int j=0;j<30;j++)
		{
		listway[i].coord[j].X=-1;
		listway[i].coord[j].Y=-1;
		}
	}
	
	
	
}


void setup() {

//инициализация кнала связи
Serial.begin(115200);
	  Serial.flush();
BLEDevice::init(""); 

pBLEScan = BLEDevice::getScan(); //create new scan 

pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); 

pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster 



	
	
   //задание пинов как выходов
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);



  // testing
  StartConfugure();
  
  
  
  

Serial.println("Kalman Create"); 
Serial.printf("%d,%d,%d,%d",filters[2].F_kalman,filters[2].H_kalman,filters[2].R_kalman,filters[2].Q_kalman); 

Serial.println("Scanning..."); 



while(countTrilateration<5)
{
BLEScanResults foundDevices = pBLEScan->start(scanTime, false); 
Combinating();
pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory 
}
itogArray[0]=itogArray[0]/5.0;
itogArray[1]=itogArray[1]/5.0;
Serial.printf("StartCoordinatFind\n");
Serial.printf("\nStartX=%f,\nStartY=%f",itogArray[0],itogArray[1]);
curentX=itogArray[0];
curentY=itogArray[1];
startX=curentX;
startY=curentY;
countTrilateration=0;
itogArray[0]=0;
itogArray[1]=0;
delay(2000);

	for(int i=0;i<10;i++)
{
	beacon[i].count=0;
	for(int j=0;j<10;j++)
{
beacon[i].rssi[j]=0;
beacon[i].filtered_RSSI[j]=0;
beacon[i].distance[j]=0;
}
}
//!!!!!!!!!!!!!!!!!!!
startX=0;
startY=0;

 


  
}




int Combinating()
{
Serial.println("Combinating"); 
int *activeID=new int[10];
int countActive=0;
int countCombin=0;

for(int i=0;i<10;i++)
{
	
if(beacon[i].active){activeID[countActive]=beacon[i].id,countActive++;}
beacon[i].active=false;
}
Serial.printf("\nActive beacon: %d\n",countActive);


for(int i=0;i<countActive;i++)
{
	Serial.printf("-%d-",activeID[i]);
	
}


if(countActive>=3)
{
  int *a;
  a = new int[countActive];
  for (int i = 0; i < countActive; i++)
    a[i] = i + 1;
  combin[countCombin].id1=a[0];
  combin[countCombin].id2=a[1];
  combin[countCombin].id3=a[2];
  countCombin++;
  if (countActive >= 3)
  {
    while (NextSet(a, countActive, 3))
    {
  combin[countCombin].id1=a[0];
  combin[countCombin].id2=a[1];
  combin[countCombin].id3=a[2];
  countCombin++;
    }
  }
}
else{Serial.println("pizdec incoming");}



for(int i=0;i<countCombin;i++)
{
  Serial.printf("\n%d-%d-%d",combin[i].id1,combin[i].id2,combin[i].id3);
}
countActive=0; 



////////////////////////////////////////////////////////
float * buf=  new float[2];

buf=Trilateration(countCombin,activeID);
itogArray[0]=itogArray[0]+buf[0];
itogArray[1]=itogArray[1]+buf[1];
countTrilateration++;
Serial.printf("\ncountTrilateration=%d",countTrilateration);
Serial.printf("\nX:%f",buf[0]);
Serial.printf("\nY:%f",buf[1]);

Serial.printf("\nsumX:%f",itogArray[0]);
Serial.printf("\nsumY:%f",itogArray[1]);

}


bool NextSet(int *a, int n, int m)
{
  int k = m;
  for (int i = k - 1; i >= 0; --i)
  if (a[i] < n - k + i + 1)
  {
    ++a[i];
    for (int j = i + 1; j < k; ++j)
      a[j] = a[j - 1] + 1;
    return true;
  }
  return false;
  
}


float * Trilateration(int countCombin,int  *activeID)
{
	int h_podvesa=0.5;
	float sumX=0;
	float sumY=0;
	float itogX=0;
	float itogY=0;
	
  for(int i=0; i<countCombin;i++)
  {
	float x;
	float y;
	float buf;
	float buf2;
	  Serial.printf("\n\n!!!Trilateration!!!\n");
	  Serial.printf("\ncountCombin%d",countCombin);
	  int id1=activeID[combin[i].id1-1];
	  int id2=activeID[combin[i].id2-1];
	  int id3=activeID[combin[i].id3-1];
	  Serial.printf("\nBeacon:%d-%d-%d\n",id1,id2,id3);
	  double distance1=beacon[id1].distance[0];
	  double distance2=beacon[id2].distance[0];
	  double distance3=beacon[id3].distance[0];
	  
	  buf = distance1 * distance1 - distance2 * distance2 - beacon[id1].myX * beacon[id1].myX - beacon[id1].myY * beacon[id1].myY + beacon[id2].myX * beacon[id2].myX + beacon[id2].myY * beacon[id2].myY;

      buf2 = distance2 * distance2 - distance3 * distance3 - beacon[id2].myX * beacon[id2].myX - beacon[id2].myY * beacon[id2].myY + beacon[id3].myX * beacon[id3].myX + beacon[id3].myY * beacon[id3].myY;

      x = (buf * ( beacon[id3].myY -  beacon[id2].myY) - buf2 * ( beacon[id2].myY -  beacon[id1].myY)) / (2 * ((beacon[id2].myX - beacon[id1].myX) * (beacon[id3].myY - beacon[id2].myY) - (beacon[id3].myX - beacon[id2].myX) * (beacon[id2].myY - beacon[id1].myY)));

      if (beacon[id2].myY != beacon[id1].myY)
            {
                y = (buf - 2 * x * (beacon[id2].myX - beacon[id1].myX)) / (2 * (beacon[id2].myY - beacon[id1].myY));
            }
            else
            {
                y = (buf2 - 2 * x * (beacon[id3].myX - beacon[id2].myX)) / (2 * (beacon[id3].myY - beacon[id2].myY));
            }
	  
	  sumX=sumX+x;
	  sumY=sumY+y;
	  Serial.printf("\sum for Combins X=%f,Y=%f",sumX,sumY);
	   Serial.printf("\nCombin N%d,X=%f,Y=%f",i,x,y);
	  
	  // Serial.printf("\nDistance:%f-%f-%f\n",distance1,distance2,distance3);
	  // double localdistance1=sqrt(distance1*distance1-h_podvesa*h_podvesa);
	  // double localdistance2=sqrt(distance2*distance2-h_podvesa*h_podvesa);
	  // double localdistance3=sqrt(distance3*distance3-h_podvesa*h_podvesa);
	  //Serial.printf("\nLocalDistance:%f-%f-%f\n",localdistance1,localdistance2,localdistance3);
  }
  itogX=sumX/countCombin;
  itogY=sumY/countCombin;
  
  float* itogArray= new float[2];
  itogArray[0]=itogX;
  itogArray[1]=itogY;
  return itogArray;
}

void detectCurentCoord()
{
	while(countTrilateration<5)
{
BLEScanResults foundDevices = pBLEScan->start(scanTime, false); 
Combinating();
pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory 
}
itogArray[0]=itogArray[0]/5.0;
itogArray[1]=itogArray[1]/5.0;
Serial.printf("\nCurentX=%f,\nCurenY=%f",itogArray[0],itogArray[1]);
curentX=itogArray[0];
curentY=itogArray[1];
//startX=curentX;
//startY=curentY;
countTrilateration=0;
itogArray[0]=0;
itogArray[1]=0;
delay(2000);	
	
}


void loop() 
{

FinishReceive();
ClearMemory();
Li(startX,startY);
Move();

}




 
 

