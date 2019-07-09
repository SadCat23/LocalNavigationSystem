using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Diagnostics;
namespace WindowsFormsApp12
{
   
    public partial class Form1 : Form
    {//класс обьектом которого является клетка, поля содержат информацию необходимую для работы с массивом клекток
        SerialPort sp = new SerialPort("COM1", 115200, Parity.None, 8, StopBits.One);
        
       
        public class Kletka
        {   //номер клетки по X/Y
           public int index_x;
            public int index_y;
            //вес клектки для алгоритма Ли
            public int W;
            //Является ли клетка стеной 
            public bool stena = false;
            //Является ли клетка финишем
            public bool finish = true;
        };

        public class Nabor
        {
            public Point[] coord = new Point[100];
            public int W;
            public int countKletka=0;
            public Nabor(int w)
            {
                W = w;
                for (int i = 0; i < 100; i++)
                {
                    coord[i] = new Point(-1, -1);
                }

            }
        }

        public class Way : ICloneable
        {   //номер клетки по X/Y
            public Point [] coord=new Point[100];
            public int countTurn;
            public int countCoord;
            public Way()
            { 
                 
            for(int i=0;i<100;i++)
            {
                coord[i] = new Point(-1, -1);
            }

            }

            public object Clone()
            {
                return this.MemberwiseClone();

            }


           
        };


        string Message;

        //создаем двухмерный массив обьектов класса Клетка
        Kletka[,] pole;
        
        //размер клетки(локльный размер)
        float shagX;
        float shagY;
        //кол-во клеток на карте
        int countX;
        int countY;
        bool tapFinish = false;
        public Form1()
        {
          InitializeComponent();
        
        }

        //функция отрисовки начальной сетки 
        private void Paints(float sizeX, float sizeY, float diskret)
        {   
            //расчитываем количество клеток
            countX =(int)(sizeX / diskret);
            countY =(int) (sizeY / diskret);
            //создаем массив обьектов класса Kletka
            pole = new Kletka[countX, countY];
            for (int j = 0; j < countY; j++)
            {
                for (int i = 0; i < countX; i++)
                {
                    Kletka kletka = new Kletka();
                    kletka.index_x=i;
                    kletka.index_y = j;
                    kletka.W = -1;
                    kletka.stena = false;
                    kletka.finish = false;
                    pole[i,j] = kletka;
                }
            }            //расчитываем размер одной клетки
            shagX =800/countX;
            shagY = 800 /countY;
            // рисование 
            var g = CreateGraphics();
            var pen = new Pen(Color.Black);
            //чистим поле
            g.Clear(Color.White);
            //рисуем границы
            g.DrawLine(pen,0, 0, 0, 800);
            g.DrawLine(pen, 0, 800, 800, 800);
            g.DrawLine(pen, 0, 0, 800, 0);
            g.DrawLine(pen, 800, 800, 800, 0);
        
            //рисуем линии 
            for (float i=1;i<countX;i++)
            {
                g.DrawLine(pen, i * shagX, 800, i * shagX, 0);
            }
            //рисуем линии 
            for (float i = 1; i < countY; i++)
            {
                g.DrawLine(pen,0 ,i * shagY,800, i * shagY);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {//размер карты(в метрах)
            float sizeX;
            float sizeY;
            //шаг дискретизации(размер клекти в метрах)
            float diskret;
            //считываем даенные из формы
            sizeX = (float) numericUpDown1.Value;
            sizeY = (float)numericUpDown2.Value;
            diskret = (float)numericUpDown3.Value;
         //функция отрисовки поля(карты)
         Paints(sizeX, sizeY, diskret);

        }
       

        //функция при отпускании клавиши мыши- высчитывает на какую клекту нажали
    
        private void  Form1_MouseUp(object sender, MouseEventArgs e)
        {
            
            float cordinatX= e.X;
            float cordinatY= e.Y;
            
            //проверяем попали ли в поле(карту)
            if(cordinatX<800&&cordinatY<800)
            {
                //высчитываем на какую клетку нажали 
                int mouseClickX = (int) (cordinatX/shagX);
                int mouseClickY = (int)(cordinatY / shagY);
                //если клетка не финишная и не нажата кнопка размещеия финиша 
                if (!pole[mouseClickX, mouseClickY].finish && !tapFinish)
                {
                    Stena(mouseClickX, mouseClickY);
                }
                else { Finish(mouseClickX, mouseClickY); }
                
            }

            
            
        }
        //функция принимающая два индекса клетки и определяющая есть в данной клетке стена если есть 
        //то убираем стену если нет добавляем
        public void Stena(int indexX,int indexY)
        {
            if (pole[indexX, indexY].stena)
            {
                //убираем стену в массивк Поле у соответсвующей клетки
                pole[indexX, indexY].stena = false;
                //убираем серый квадрат
                var g = CreateGraphics();
                var pen = new Pen(Color.Black);
                SolidBrush br = new SolidBrush(Color.White);
                g.FillRectangle(br, shagX * indexX, shagY * indexY, shagX, shagY);
                g.DrawRectangle(pen, shagX * indexX, shagY * indexY, shagX, shagY);


            }
            else
            {   //добавляем стену в массив Поле соответсвующей клетке
                pole[indexX, indexY].stena = true;
                //рисуем серый квадрат 
                var g = CreateGraphics();
                var pen = new Pen(Color.Black);
                SolidBrush br= new SolidBrush(Color.Gray);
                g.FillRectangle(br, shagX * indexX, shagY * indexY, shagX, shagY);
                g.DrawRectangle(pen, shagX * indexX, shagY * indexY, shagX, shagY);
            }
        }

        public void Finish(int indexX, int indexY)
        {
            if (pole[indexX, indexY].finish)
            {
                pole[indexX, indexY].finish = false;
                //убираем зеленый квадрат
                var g = CreateGraphics();
                var pen = new Pen(Color.Black);
                SolidBrush br = new SolidBrush(Color.White);
                g.FillRectangle(br, shagX * indexX, shagY * indexY, shagX, shagY);
                g.DrawRectangle(pen, shagX * indexX, shagY * indexY, shagX, shagY);
                button2.Enabled = true;
            }
            else
            {
                pole[indexX, indexY].finish = true;
                //рисуем зеленый квадрат 
                var g = CreateGraphics();
                var pen = new Pen(Color.Black);
                SolidBrush br = new SolidBrush(Color.Green);
                g.FillRectangle(br, shagX * indexX, shagY * indexY, shagX, shagY);
                g.DrawRectangle(pen, shagX * indexX, shagY * indexY, shagX, shagY);
                tapFinish = false;
              
            }
        }



        private void button2_Click(object sender, EventArgs e)
        {
            tapFinish = true;
            button2.Enabled = false;
        }


        public Point FindFinish()
        {
            foreach ( Kletka kletka in pole)
            {
                if (kletka.finish)
                {
                    Point a = new Point(kletka.index_x, kletka.index_y);
                    return a;
                   
                }
            }
            Point b = new Point(0, 0);
            return b;

        }
       


        //Алгоритм ЛИ
        public Point[] AlgLIPoisk(int startX, int startY)
        {
            int countW = 0;
            pole[startX, startY].W = 0;
            var g = CreateGraphics();
            SolidBrush br = new SolidBrush(Color.Green);
            g.DrawString(0.ToString(), new System.Drawing.Font("Arial Black", 20, FontStyle.Regular, GraphicsUnit.Point), br, startX * shagX + shagX / 3, startY * shagY + shagY / 4);
            Point Finish = FindFinish();
           
            while (pole[Finish.X,Finish.Y].W==-1)
            {
                    foreach (Kletka kletka in pole)
                    {
                    if (kletka.W==countW)
                    {
                        //проверяем соседей и если это не стена помечаем их   
                        //справа(проверяем так же не самая ли правая клетка)
                        if (kletka.index_x < countX-1)
                        {
                            if (!pole[kletka.index_x + 1, kletka.index_y].stena&& pole[kletka.index_x + 1, kletka.index_y].W==-1)
                            {
                                var newW = countW + 1;
                                pole[kletka.index_x + 1, kletka.index_y].W = newW;
                                
                                g.DrawString(newW.ToString(), new System.Drawing.Font("Arial Black", 20, FontStyle.Regular, GraphicsUnit.Point), br, (kletka.index_x + 1) * shagX + shagX / 3, kletka.index_y * shagY + shagY / 4);

                            }
                        } 
                        //слева
                        if (kletka.index_x > 0)
                        {
                           
                            if (!pole[kletka.index_x-1, kletka.index_y].stena&& pole[kletka.index_x - 1, kletka.index_y].W==-1)
                            {
                                var newW = countW + 1;
                                pole[kletka.index_x - 1, kletka.index_y].W = newW;
                               
                                g.DrawString(newW.ToString(), new System.Drawing.Font("Arial Black", 20, FontStyle.Regular, GraphicsUnit.Point), br, (kletka.index_x - 1) * shagX + shagX / 3, kletka.index_y * shagY + shagY / 4);

                            }
                        }
                        //сверху
                        if (kletka.index_y > 0)
                        {
                            if (!pole[kletka.index_x, kletka.index_y - 1].stena&& pole[kletka.index_x, kletka.index_y - 1].W==-1)
                            {
                                var newW = countW + 1;
                                pole[kletka.index_x, kletka.index_y-1].W = newW;
                               
                                g.DrawString(newW.ToString(), new System.Drawing.Font("Arial Black", 20, FontStyle.Regular, GraphicsUnit.Point), br, kletka.index_x * shagX + shagX / 3, (kletka.index_y - 1) * shagY + shagY / 4);

                            }
                        }
                        //снизу

                        if (kletka.index_y < countY-1)
                        {
                            if (!pole[kletka.index_x, kletka.index_y + 1].stena&&pole[kletka.index_x, kletka.index_y + 1].W==-1)
                            {
                                var newW = countW + 1;
                                pole[kletka.index_x, kletka.index_y+1].W = newW;
                                
                                g.DrawString(newW.ToString(), new System.Drawing.Font("Arial Black", 20, FontStyle.Regular, GraphicsUnit.Point), br, kletka.index_x * shagX + shagX / 3, (kletka.index_y + 1) * shagY + shagY / 4);

                            }
                        }
                    }
                }

                countW++;

            }
            SolidBrush br2 = new SolidBrush(Color.Red);
            g.DrawString(pole[Finish.X, Finish.Y].W.ToString(), new System.Drawing.Font("Arial Black", 20, FontStyle.Regular, GraphicsUnit.Point), br2, Finish.X * shagX + shagX / 3, (Finish.Y) * shagY + shagY / 4);
          Point[] oneWay= AlgLIVostanovl(startX,startY);
          return oneWay;
        }
        public Point[] AlgLIVostanovl(int startX, int startY)
        {
            Point Finish = FindFinish();
            bool smena = false;
            int countShag = 0;
            int countKletka = 0;
            int indexX =Finish.X;
            int indexY = Finish.Y;
            int countW = pole[indexX, indexY].W;
            List<Nabor> ListNabor = new List<Nabor>();
            Nabor nabor = new Nabor(countW);
            nabor.coord[0] = new Point(indexX, indexY);
            nabor.countKletka++;
            ListNabor.Add(nabor);
            countW--;

            //общий цикл 
           
                cycle:
            //удаляем дубли
       




                //проверяем от  точки
                //справа
                if (indexX < countX-1)
                {
                    if (pole[indexX + 1, indexY].W == pole[indexX, indexY].W - 1)
                    {
                        bool find = false;
                        Point kletkaCoord = new Point(indexX + 1, indexY);
                        foreach (Nabor n in ListNabor)
                        {
                            if (n.W == countW) { n.coord[n.countKletka] = kletkaCoord; n.countKletka++; find = true; }

                        }
                        if (!find) { Nabor nabor2 = new Nabor(countW); nabor2.coord[0] = kletkaCoord; nabor2.countKletka++; ; ListNabor.Add(nabor2); }



                    }
                }
                //слева
                if (indexX > 0)
                {
                    if (pole[indexX - 1, indexY].W == pole[indexX, indexY].W - 1)
                    {
                        bool find = false;
                        Point kletkaCoord = new Point(indexX - 1, indexY);
                        foreach (Nabor n in ListNabor)
                        {
                            if (n.W == countW) { n.coord[n.countKletka] = kletkaCoord; n.countKletka++;find = true; }

                        }
                        if (!find) { Nabor nabor2 = new Nabor(countW); nabor2.coord[0] = kletkaCoord; nabor2.countKletka++; ; ListNabor.Add(nabor2); }

                    }
                }
                //вниз
                if (indexY < countY-1)
                {
                    if (pole[indexX, indexY + 1].W == pole[indexX, indexY].W - 1)
                    {
                        bool find = false;
                        Point kletkaCoord = new Point(indexX, indexY + 1);
                        foreach (Nabor n in ListNabor)
                        {
                            if (n.W == countW) { n.coord[n.countKletka] = kletkaCoord; n.countKletka++; find = true; }

                        }
                        if (!find) { Nabor nabor2 = new Nabor(countW); nabor2.coord[0] = kletkaCoord; nabor2.countKletka++; ; ListNabor.Add(nabor2); }
                    }
                }
                //вверх
                if (indexY > 0)
                {
                    if (pole[indexX, indexY - 1].W == pole[indexX, indexY].W - 1)
                    {

                        bool find = false;
                        Point kletkaCoord = new Point(indexX, indexY - 1);
                        foreach (Nabor n in ListNabor)
                        {
                            if (n.W == countW) { n.coord[n.countKletka] = kletkaCoord; n.countKletka++; find = true; }

                        }
                        if (!find) { Nabor nabor2 = new Nabor(countW); nabor2.coord[0] = kletkaCoord; nabor2.countKletka++; ; ListNabor.Add(nabor2); }


                    }
                }

                countKletka++;
                foreach (Nabor n in ListNabor)
                {
                    if (countW == 0) 
                    {
                        goto AllWayFound; 
                    }

                    if (n.W == countW+1)
                    {
                        //проверили из всех клеток в с данным вессом 
                        if (n.countKletka == countKletka)
                        {
                            countW--;
                            foreach (Nabor n2 in ListNabor)
                            {
                                if (n2.W == countW + 1)
                                {
                                    countKletka = 0;
                                    indexX = n2.coord[countKletka].X;
                                    indexY = n2.coord[countKletka].Y;
                                    goto cycle;
                                }

                            }



                        }
                        else
                        {
                            indexX = n.coord[countKletka].X;
                            indexY = n.coord[countKletka].Y;
                            goto cycle;
                        }

                    }

                }

AllWayFound:
                foreach (Nabor n in ListNabor)
                { 
                    
                     for(int i=0; i<n.countKletka-1;i++)
                     {
                         for (int j = 0; j < n.countKletka; j++)
                         {
                             if (j != i) 
                             {
                                 if (n.coord[i] == n.coord[j]) {
                                   
                                         n.coord[j] = new Point(-1, -1);
                                     
                                     
                                   
                                    
                                   
                                  
                                 }
                             }
                         }
                     }
                }


foreach (Nabor n in ListNabor)
{
    int NewCount=0;
   Point nullCoord= new Point  (-1,-1);
    for (int i = 0; i < n.countKletka; i++)
    {
        if (n.coord[i] == nullCoord) 
        { 
            n.coord[i] = n.coord[i+1];  
        }
        else 
        {
            NewCount++;
        }
    }
    n.countKletka = NewCount;

}



int indexW = 0;
int indexKletkiW=0;
int indexKletfiF = 0;




            //координата клетки старта
Point coord=new Point (startX,startY);


List<Way> listway = new List<Way>();

for (int j = 0; j < ListNabor[ListNabor.Count - 2].countKletka;j++)
{

    Point coordKletki=ListNabor[ListNabor.Count - 2].coord[j];
    if(     (coord.X==coordKletki.X && Math.Abs(coord.Y-coordKletki.Y)==1) || (coord.Y==coordKletki.Y && Math.Abs(coord.X-coordKletki.X)==1)   )
    {
        Way way=new Way();
        way.coord[0]=coord;
        way.coord[1]=coordKletki;
        way.countCoord=2;
        listway.Add(way);

    }

}


int indexStroki = 3;
Zanovo:

bool first = true;
int countWay=0;

foreach (Way strWay in listway)
{
    countWay++;
}



for( int i=0;i<countWay;i++)
{
    Way strWay = listway[i];
    coord = strWay.coord[strWay.countCoord - 1];
    for (int j = 0; j < ListNabor[ListNabor.Count - indexStroki].countKletka; j++)
    {
        Point coordKletki = ListNabor[ListNabor.Count - indexStroki].coord[j];
        if ((coord.X == coordKletki.X && Math.Abs(coord.Y - coordKletki.Y) == 1) || (coord.Y == coordKletki.Y && Math.Abs(coord.X - coordKletki.X) == 1))
        {
            if (first) 
            {
                strWay.coord[strWay.countCoord] = coordKletki;
                strWay.countCoord++;
                first = false;
            }
            else
            {
                
                Way way = new Way();
                way =(Way) strWay.Clone();
                way.coord = (Point[])strWay.coord.Clone();
                way.coord[strWay.countCoord-1] = coordKletki;
                listway.Add(way);
                
            }

        }

    }
    first = true;
    //indexStroki++;
}
indexStroki++;
if (indexStroki < ListNabor.Count + 1) { goto Zanovo; }


            
foreach(Way Route in listway) 
{
    bool plusY = true;
    bool minusY = false;
    bool plusX = false;
    bool minusX = false;
   for(int i=0;i<Route.countCoord-1;i++)
   {
       //y+
       if (Route.coord[i].Y < Route.coord[i + 1].Y)
       {
           if (!plusY) 
           {
               Route.countTurn++;
           }
       plusY = true;
       minusY=false;
       plusX=false;
       minusX=false;
       }

       //y-
       if (Route.coord[i].Y > Route.coord[i + 1].Y)
       {
           if (!minusY)
           {
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
               Route.countTurn++;
           }
           plusY = false;
           minusY = false;
           plusX = false;
           minusX = true;
       }



   }   


}
int turnIndex=0;
for (int i=0; i < listway.Count;i++)
{
 
        if (listway[turnIndex].countTurn > listway[i].countTurn)
        {
            turnIndex = i;
        }
 
}

 Point[] oneWay=new  Point[100];
 oneWay = (Point[])listway[turnIndex].coord.Clone();
 return oneWay;

}
            
           
       


        

        private void button3_Click(object sender, EventArgs e)
        {

            sp.Open();
            sp.Write("Hello\n");
            string str = "\0";
            while (sp.BytesToRead > 0)
            {
                str = sp.ReadLine();

                Message = Message + str;
                richTextBox1.Text = Message;
            }
            if (str.Equals("Darou\r")) { }
            else { MessageBox.Show("ERROR(HELLO)"); }
            System.Threading.Thread.Sleep(100);

            sp.Write("StartMap\n");
            sp.Write(countX.ToString()+','+countY.ToString());
            sp.Write("EndSize\n");

            while (sp.BytesToRead > 0)
            {
                str = sp.ReadLine();

                Message = Message + str;
                richTextBox1.Text = Message;
            }
            
            
            
            //AlgLIPoisk(6, 0);
       

            //sp.Write("StartCoordinat\n");
            //Point start_coord = new Point(-1, -1);
            //string start_coord_str = "\0";
            //System.Threading.Thread.Sleep(100);

            //while (sp.BytesToRead > 0)
            //{

            //    str = sp.ReadLine();

            //    if (str.Equals("CordBegin\r"))
            //    {
            //        Debug.WriteLine("CordBegin");
            //        string coordinate = "\0";
            //        do
            //        {
            //            coordinate = sp.ReadLine();
            //            start_coord_str = coordinate;
            //            coordinate = sp.ReadLine();
            //        }
            //        while (!coordinate.Equals("CordEnd\r"));
            //        Debug.WriteLine("CordEnd");
            //    }
            //}
            //System.Threading.Thread.Sleep(100);
            //string[] nabor_coordinate = start_coord_str.Split('.');
            //start_coord.X = int.Parse(nabor_coordinate[0]);
            //start_coord.Y = int.Parse(nabor_coordinate[1]);
            //Debug.WriteLine(start_coord.ToString());
            //Point[] oneWay = AlgLIPoisk(start_coord.X, start_coord.Y);
            //System.Threading.Thread.Sleep(100);
            //sp.Write("StartMarshrut\n");
            //Debug.WriteLine("StartMarshrut");
            //System.Threading.Thread.Sleep(100);
            //foreach (Point coord in oneWay)
            //{
            //    if (coord != oneWay[0])
            //    {
            //        if (coord != new Point(-1, -1))
            //        {
            //            string message = coord.X.ToString() + "." + coord.Y.ToString() + "\n";
            //            System.Threading.Thread.Sleep(100);
            //            sp.Write(message);
            //            System.Threading.Thread.Sleep(100);
            //        }
            //        else { break; }
            //    }

            //}
            //System.Threading.Thread.Sleep(100);
            //sp.Write("EndMarshrut\n");
            //Debug.WriteLine("EndMarshrut");
            //System.Threading.Thread.Sleep(100);


            //System.Threading.Thread.Sleep(100);
            //sp.Write("Go\n");
            //Debug.WriteLine("Go");
            //sp.DataReceived += new SerialDataReceivedEventHandler(OnDataReceived);
            //System.Threading.Thread.Sleep(100);


            //while (sp.Bytestoread > 0)
            //{

            //    str = sp.readline();
            //    debug.writeline(str);
            //    message = message + str;
            //    // richtextbox1.text = message;
            //}


        }

        //private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        //{
        //    string str = sp.ReadLine();
        //    richTextBox1.Text = str;
        //}

        private void OnDataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            
            string str = sp.ReadLine();
            Debug.WriteLine(str);
           
        }

        public void UpdateLog()
        {
            richTextBox1.Text = Message;

        }
    }
}
