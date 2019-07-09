/* 
Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master.. 
Ported to Arduino ESP32 by Evandro Copercini 
*/ 

#include <BLEDevice.h> 
#include <BLEUtils.h> 
#include <BLEScan.h> 
#include <BLEAdvertisedDevice.h> 
#include <BLEUUID.h> 


#define bleBeacon1Name "SadCat1" 

int RSSI_1m[10]={-65,-62,-68,-67};


int countTrilateration;
float * itogArray=  new float[2];

//*****************************************
//Kalman 
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





//****************************************************


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





//***************************************************



String key="beacon";

int scanTime = 1; //In seconds 
BLEScan* pBLEScan; 





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




bool Auth(String NameBeacon)
{
NameBeacon=NameBeacon.substring(0,6);
return NameBeacon.equals(key);

}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





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








////////////////////////////////////////////////////////////////////////////////////////////////////////////////////







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
  Serial.printf("\n%d,%d,%f",beacon[id].rssi[i],beacon[id].filtered_RSSI[i],beacon[id].distance[i]); 
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




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










void setup() 
{ 
Serial.begin(115200); 

for(int i=0;i<10;i++)
{
beacon[i].id=i;

}
beacon[0].myX=0;
beacon[0].myY=0;

beacon[1].myX=0;
beacon[1].myY=3;

beacon[2].myX=2;
beacon[2].myY=3;

beacon[3].myX=0;
beacon[3].myY=0;

itogArray[0]=0;
	 itogArray[1]=0;


Serial.println("Kalman Create"); 
Serial.printf("%d,%d,%d,%d",filters[2].F_kalman,filters[2].H_kalman,filters[2].R_kalman,filters[2].Q_kalman); 

Serial.println("Scanning..."); 

BLEDevice::init(""); 
pBLEScan = BLEDevice::getScan(); //create new scan 
pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); 
pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster 
pBLEScan->setInterval(100); 
pBLEScan->setWindow(99); // less or equal setInterval value 
} 








////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() { 
// put your main code here, to run repeatedly: 
BLEScanResults foundDevices = pBLEScan->start(scanTime, false); 

//Serial.println(foundDevices.getCount()); 
//Serial.println("Scan done!"); 
Combinating();
pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory 
}



int Combinating()
{
	
int *activeID;	
int countActive=0;
int countCombin=0;

activeID=new int[10];
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
Serial.printf("\n111");
buf=Trilateration(countCombin,activeID);
Serial.printf("\n%f",buf[0]);
Serial.printf("\n%f",buf[1]);
Serial.printf("\n222"); 

itogArray[0]=itogArray[0]+buf[0];
Serial.printf("\n...");
itogArray[1]=itogArray[1]+buf[1];
Serial.printf("\n333");
countTrilateration++;
Serial.printf("\n444");
Serial.printf("\ncountTrilateration=%d",countTrilateration);
if(countTrilateration==3)
{
	itogArray[0]=itogArray[0]/3.0;
	itogArray[1]=itogArray[1]/3.0;
	 Serial.printf("\nItogovuiX=%f,\nItogovuiY=%f",itogArray[0],itogArray[1]);
	 countTrilateration=0;
	 itogArray[0]=0;
	 itogArray[1]=0;
	 delay(5000);
}

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
	  Serial.printf("\sum X=%f,Y=%f",sumX,sumY);
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
 Serial.printf("\nItogX=%f,\nItogY=%f",itogX,itogY);
  return itogArray;
}











