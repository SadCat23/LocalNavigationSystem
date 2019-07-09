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

class Way
	{   //номер клетки по X/Y
	public:
		Point coord[30];
		int countTurn;
		int countCoord;
	};
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

Way listway[40];

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
  Serial.printf("\nPole\n");
	for(int i=0;i<sizeMapX*sizeMapY;i++)
	{
		
			Serial.printf("\nKletka [%d] X:[%d]Y:[%d] STENA:%d Wes:%d",i,pole[i].index_x,pole[i].index_y,pole[i].stena,pole[i].W);
	}


}



//Li
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

	
		Serial.printf("\n\n\n Optimal Way %d; CountTurn:%d\n", turnIndex,listway[turnIndex].countTurn);
		for (int j = 0; j < listway[turnIndex].countCoord; j++)
		{
			    route[j][0]=listway[turnIndex].coord[j].X;
				route[j][1]=listway[turnIndex].coord[j].Y;
    
			Serial.printf("[%d][%d];", listway[turnIndex].coord[j].X, listway[turnIndex].coord[j].Y);
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
inString="\0";
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
Li(0,0);

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



}
 
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


