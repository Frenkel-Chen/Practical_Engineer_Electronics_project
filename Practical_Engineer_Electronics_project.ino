
#include <LiquidCrystal.h>
//#include "AH_MCP4921.h"
#include <SD.h>

const int chipSelect = 4;   

LiquidCrystal lcd(12, 11, 8, 7, 6, 5);

const int numRows =4;
const int numCols = 3;
const int debounceTime = 50;
const char keymap[numRows][numCols] = {                         
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'.','0','#'}
};

const int rowPins[numRows] = {33,43,41,37};                    
const int colPins[numCols] = {35,31,39};                           

char corrent_lcd_view='0';                                                 
unsigned long previous_update_time=0;                      
int lcd_update_delay=500;
double LM_Temp_Const = 10.0;                                   
int LM_gain =1;
int Thermocouple_gain = 1001;                                   


int Thermocouple1 = A0;                                         
int Thermocouple2 = A1;                    
int Thermocouple3 = A2;
int Thermocouple4 = A3;

int lm1 = A4;                                               
int lm2 = A5;                    
int lm3 = A6;   
int lm4 = A7;

int LDAC_mpc=40;                
 
int Thermocouple_And_LM_pairs[4][2] = {{Thermocouple1,lm1},         
                                       {Thermocouple2,lm2},   
                                       {Thermocouple3,lm3},
                                       {Thermocouple4,lm4}};
   

double Thermocouple_Comp_arr[9][5]={                                            
{-121.47164,-8.7935962,310.18976,605.72562,1018.4705},                  
{-4.1790858,-0.34489914,12.631386,25.148718,41.993851},
{36.069513,25.678719,24.061949,23.539401,25.783239},
{30.722076,-0.49887904,4.0158622,0.046547228,-1.8363403},
{7.7913860,-0.44705222,0.26853917,0.013444400,0.056176662},
{0.52593991,-0.044869203,-0.0097188544,0.00059236853,0.00018532400},
{0.93939547,0.00023893439,0.16995872,0.00083445513,-0.074803355},
{0.27791285,-0.020397750,0.011413069,0.00046121445,0.0023841860},
{0.025163349,-0.0018424107,-0.00039275155,0.000025488122,0.0}};
                                                                           

double Resolution,Division; 
int Location_of_the_point,i;            
char Array_of_resolution[7];

int sdi_mosi=51;                      
int sck=52;   
                              
AH_MCP4921 AnalogOutput1(sdi_mosi,sck,42);       
AH_MCP4921 AnalogOutput2(sdi_mosi,sck,44); 
AH_MCP4921 AnalogOutput3(sdi_mosi,sck,46);
AH_MCP4921 AnalogOutput4(sdi_mosi,sck,48);

void setup() 
{                
  lcd.begin(16,2);
   Serial.begin(9600); 
  for(int row=0;row<numRows;row++){                       
   pinMode(rowPins[row],INPUT);
  digitalWrite(rowPins[row],HIGH); 
  }
    for(int col=0;col<numCols;col++){
    pinMode(colPins[col],OUTPUT);
    digitalWrite(colPins[col],HIGH);
   }
 pinMode(LDAC_mpc,OUTPUT);
 digitalWrite(LDAC_mpc,HIGH);
 
previous_update_time=millis();      
 
 lcd.print("Initializing SD card...");
 delay(1000);
  lcd.clear();
  pinMode(53, OUTPUT);

  if (!SD.begin(chipSelect)) {                         
    lcd.print("SD Card failed");                       
    delay(5000);
    lcd.clear();
    return;
  }
  else
  {
   lcd.print("SD Card OK");
   delay(2000);
   lcd.clear();
   return;
  }    
 lcd.print("card initialized.");
 lcd.clear();
delay(1000); 
}
  


void loop()                              
{
double final_temp1,final_temp2,final_temp3,final_temp4;

  final_temp1 = read_temp(Thermocouple_And_LM_pairs,0,10); 
  final_temp2 = read_temp(Thermocouple_And_LM_pairs,1,10);  
  final_temp3 = read_temp(Thermocouple_And_LM_pairs,2,10);
  final_temp4 = read_temp(Thermocouple_And_LM_pairs,3,10);
  
  File SdCard = SD.open("data.txt", FILE_WRITE);               
  if (SdCard )  {       
     SdCard .print(" temperature of Thermocouple 1:");
    SdCard .print(final_temp1);
    SdCard .print("       temperature of Thermocouple 2:");
    SdCard .print(final_temp2);
    SdCard .print("       temperature of Thermocouple 3:");
    SdCard .print(final_temp3);
    SdCard .print("       temperature of Thermocouple 4:");
    SdCard .print(final_temp4);
    SdCard .println();  
 }
  char key = getkey();          
  if(key !=0 ){                          
  corrent_lcd_view=key;
  }
if (previous_update_time+lcd_update_delay<millis())  {
   lcd.clear();
  switch (corrent_lcd_view)              
  {
    case '0':
                   lcd.setCursor(3,0);
                    lcd.print("Welcome (;");
                    break;
    case '1':
                    lcd.setCursor(0,0);
                    lcd.print("Thermocouple 1:");
                    lcd.setCursor(0, 1);
                    lcd.print(final_temp1);
                    break;

   case '2':
                    lcd.setCursor(0,0);
                    lcd.print("Thermocouple 2:");
                    lcd.setCursor(0, 1);
                    lcd.print(final_temp2);
                    break;
   case '3':
                    lcd.setCursor(0,0);
                    lcd.print("Thermocouple 3:");
                    lcd.setCursor(0, 1);
                    lcd.print(final_temp3);
                    break;
   case '4':
                    lcd.setCursor(0,0);
                    lcd.print("Thermocouple 4:");
                    lcd.setCursor(0, 1);
                    lcd.print(final_temp4);
                    break;
        case '#':  do
                   {
                    lcd.setCursor(0,0);
                    lcd.print("Resolution:");
                    key = getkey();                       
                    if (key!=0)  {
                    if (i<6)  {
                    Array_of_resolution[i]=key;            
                    i++;
                    }
                    lcd.setCursor(0, 1);
                    lcd.print(Array_of_resolution);     
                    delay(500);
                    }
                    }
                    while(key!='#');
                    lcd.clear();
                    Division=10;
                    for (i=0;i<6;i++)  {
=                   if(Array_of_resolution[i]==46)  {       
                    Location_of_the_point=i;
                    }
                    }
                     Resolution=0;
                     for (i=0;i<Location_of_the_point;i++)   {
                     Resolution=Resolution*10+(Array_of_resolution[i]-48);  
                    }
                     for (i=Location_of_the_point+1;i<6;i++)   {
                     Resolution=Resolution+(Array_of_resolution[i]-48)/Division;
                     Division=Division*10;
                    }
                    lcd.print("Resolution OK");
                    break;
   }
 for (i=0;i<6;i++)    {
 Array_of_resolution[i]=0;
 }
 i=0;
 previous_update_time=millis();
} 
AnalogOutput1.setValue(final_temp1*Resolution);        
AnalogOutput2.setValue(final_temp2*Resolution);
AnalogOutput3.setValue(final_temp3*Resolution);
AnalogOutput4.setValue(final_temp4*Resolution);
updateAnlogOut();
SdCard.close();                          
}


 char getkey(){                                  
 char key=0;
 for(int col=0;col<numCols;col++){
 digitalWrite(colPins[col],LOW);
    for(int row=0;row<numRows;row++){
    if(digitalRead( rowPins[row] ) ==  LOW ){
    delay(debounceTime);
    while ( digitalRead( rowPins[row] ) ==  LOW  );
     key = keymap[row][col];
      }  
    }
  digitalWrite(colPins[col],HIGH);
  }
 return key;                                     
}


 void updateAnlogOut()             
 {
 digitalWrite(LDAC_mpc,LOW);
 digitalWrite(LDAC_mpc,HIGH);
 }



double sample (int analog_pin, double num_of_avg, double gain)   
{
double avg =0;
double tmp=0;
for(int c =0;c<num_of_avg;c++)  {
tmp = analogRead(analog_pin);     
tmp=((4.882812*tmp)/gain);                             
avg+=tmp;
 }
return (avg/num_of_avg);
}

double read_temp (int Thermocouple_arr[4][2], int pair_num , double num_of_avg)
{
    double Thermocouple_volt, Thermocouple_temp;
    double LM_volt, LM_temp;
    double CJ_Comp_Volt;
    double Real_Thermocouple_volt;
    double Tempature;
    int col_select;
    Thermocouple_volt = sample( Thermocouple_arr[pair_num][0], num_of_avg,
    Thermocouple_gain);
    LM_volt = sample( Thermocouple_arr[pair_num][1],num_of_avg,LM_gain);
    LM_temp = LM_volt/ LM_Temp_Const;
    CJ_Comp_Volt=1.0003453+((LM_temp-25.00)*(0.040514854+(LM_temp-25.00)*
(-0.000038789+(LM_temp-25.00)*(-0.000002860847-0.00000000095367041*(LM_temp
-25.00)))))/(1+(LM_temp-25.00)*(-0.0013948675+-0.000067976627*(LM_temp-25.00)));
    Real_Thermocouple_volt = Thermocouple_volt + CJ_Comp_Volt;
    col_select = find_comp_col (Real_Thermocouple_volt);
    
    

 Tempature =  Thermocouple_Comp_arr[0][col_select]+((Real_Thermocouple_volt-
Thermocouple_Comp_arr[1][col_select])*(Thermocouple_Comp_arr[2][col_select]+
(Real_Thermocouple_volt-Thermocouple_Comp_arr[1][col_select])*(Thermocouple_Comp_arr[3]
[col_select]+(Real_Thermocouple_volt-Thermocouple_Comp_arr[1][col_select])*
(Thermocouple_Comp_arr[4][col_select]+Thermocouple_Comp_arr[5][col_select]*
(Real_Thermocouple_volt-Thermocouple_Comp_arr[1][col_select])))))/
(1+(Real_Thermocouple_volt-Thermocouple_Comp_arr[1][col_select])*(Thermocouple_Comp_arr[6]
[col_select]+(Real_Thermocouple_volt-Thermocouple_Comp_arr[1][col_select])*
(Thermocouple_Comp_arr[7][col_select]+Thermocouple_Comp_arr[8][col_select]*
(Real_Thermocouple_volt-Thermocouple_Comp_arr[1][col_select]))));
 return Tempature; 
 }

int find_comp_col (double Real_Thermocouple_volt)   
   {                                                                                                             
         if(Real_Thermocouple_volt >= -6.404 && Real_Thermocouple_volt < -3.554)
               return 0;
         if(Real_Thermocouple_volt >= -3.554 && Real_Thermocouple_volt < 4.096)
               return 1;
          if(Real_Thermocouple_volt >= 4.096 && Real_Thermocouple_volt < 16.397)
               return 2;
          if(Real_Thermocouple_volt >= 16.397 && Real_Thermocouple_volt < 33.275)
               return 3;
          if(Real_Thermocouple_volt >= 33.275 && Real_Thermocouple_volt < 693553)
               return 4;
     }
