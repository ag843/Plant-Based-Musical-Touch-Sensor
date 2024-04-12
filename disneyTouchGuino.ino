
//                              10n
// PIN 9 --[10k]-+-----10mH---+--||-- OBJECT
//               |            |
//              3.3k          |
//               |            V 1N4148 diode
//              GND           |
//                            |
//Analog 0 ---+------+--------+
//            |      |
//          100pf   1MOmhm
//            |      |
//           GND    GND



#define SET(x,y) (x |=(1<<y))				//-Bit set/clear macros
#define CLR(x,y) (x &= (~(1<<y)))       		// |
#define CHK(x,y) (x & (1<<y))           		// |
#define TOG(x,y) (x^=(1<<y))            		//-+


#define N 120  //How many frequencies

float results[N];            //-Filtered result buffer
float freq[N];            //-Filtered result buffer
int sizeOfArray = N;
int fixedGraph = 0;
int topPoint = 0;
int topPointValue = 0;
int topPointInterPolated = 0;
int baseline = 639;
int value = 0;
int contactLevelID = 0;
int r = 250;
int g = 0;
int b = 0;


  
//This is the part that moderates the voltages in the circuit in order to sense touch
void setup(){
  pinMode(13, OUTPUT);
  digitalWrite(13,LOW);
  // Start the guino dashboard interface.
  // The number is your personal key for saving data. This should be unique for each sketch
  // This key should also be changed if you change the gui structure. Hence the saved data vill not match.
  gBegin(34526); 
  
  TCCR1A=0b10000010;        //-Set up frequency generator
  TCCR1B=0b00011001;        //-+

  ICR1=110;
  OCR1A=55;


  pinMode(9,OUTPUT);        //-Signal generator pin
  pinMode(8,OUTPUT);        //-Sync (test) pin


  for(int i=0;i<N;i++)      //-Preset results
    results[i]=0;         //-+
}

//This updates all the values in the guino interface
//It also processes the values we get from the pins into quantifiable numbers and resets the pin frequencies to keep sensing touch
void loop(){
  // **** Main update call for the guino
  unsigned int d;

  int counter = 0;
   topPoint = 0;
   topPointValue = 0;

  //This updates pin values
  for(unsigned int d=0;d<N;d++)
  {
   
    int v=analogRead(0);    //-Read response signal
    CLR(TCCR1B,0);          //-Stop generator
    TCNT1=0;                //-Reload new frequency
    ICR1=d;                 // |
    OCR1A=d/2;              //-+
    SET(TCCR1B,0);          //-Restart generator

    delayMicroseconds(1);
   //This processes the values we get from the circuit
    results[d]=results[d]*0.5+(float)(v)*0.5; //Filter results
    if (topPointValue < results[d]) 
    {
      topPointValue = results[d];
      topPoint =d;
    }
    
  freq[d] = d;
  fixedGraph = round(results[d]);
  gUpdateValue(&fixedGraph);
  delayMicroseconds(1);
  }
  topPointInterPolated =topPointInterPolated * 0.5f + 
    ((topPoint+ results[topPoint]/results[topPoint+1]*results[topPoint-1]/results[topPoint])*10.0f)*0.5f;
  
  value = topPointInterPolated - baseline;

  //This updates all the values for guino
  guino_update();
  gUpdateValue(&topPoint);
  gUpdateValue(&value);
  gUpdateValue(&topPointInterPolated);

  //This categorizes the value into a few categories of touch and then updates the label accordingly
  if(value > 350)
  {
    gUpdateLabel(contactLevelID, "High Contact");
  }
  else if(value > 300)
  {
    gUpdateLabel(contactLevelID, "Moderate to High Contact");
  }
  else if(value > 200)
  {
    gUpdateLabel(contactLevelID, "Moderate Contact");
  }
  else if(value > 100)
  {
    gUpdateLabel(contactLevelID, "Light to Moderate Contact");
  }
  else if(value > 20)
  {
    gUpdateLabel(contactLevelID, "Light Contact");
  }
  else
  {
    gUpdateLabel(contactLevelID, "No Contact");
  }
  
  b = value * (250.0/700.0);
  r = 250 - b;
  gSetColor(r,g,b);

  //This plays a tone off of pin 8 with an adjusted frequency scaled to 5000 instead of 800 (which is the range of value)
  tone(8, value*(5000.0/800.0));
}

// This is where you setup your interface and explain what types of displays you want on the guino dashboard
void gInit()
{
  
  gAddLabel("Touch Sensor",1);
  
   gAddSpacer(1);
   
 
   gAddSpacer(1);
   gAddFixedGraph("FIXED GRPAPH",-500,1000,N,&fixedGraph,40);
   gAddSlider(0,N,"TOP",&topPoint);
   gAddSlider(0,N*10,"Interpolated",&topPointInterPolated);
   gAddSlider(0,800,"Baseline",&baseline);
   gAddSlider(0,500,"Value",&value);
   contactLevelID = gAddLabel("",1);
   gSetColor(250,0,0);
   
   
  /*
  gAddLabel("SLIDERS",1);
  gAddSpacer(1);
  gAddSlider(3,200,"WIDTH",&width);
  gAddSlider(3,200,"HEIGHT",&height);
  
  // The rotary sliders 
  gAddLabel("ROTARY SLIDERS",1);
  gAddSpacer(1);
  
  gAddRotarySlider(0,255,"R",&r);
  gAddRotarySlider(0,255,"G",&g);
  gAddRotarySlider(0,255,"B",&b);
  
  gAddLabel("BUTTONS",1);
  gAddSpacer(1);
  buttonId = gAddButton("HEIGHT TO 100"); 
  gAddToggle("PAUSE",&pause);
  gAddSpacer(1);
  
  
  
  gAddSpacer(1);
  flexLabelId = gAddLabel("LIVE LABEL",2);
  gAddSpacer(1);
  
  gAddColumn();

  gAddLabel("GRAPHS",1);
  gAddSpacer(1);
  
  // Last parameter in moving graph defines the size 10 = normal
  gAddMovingGraph("SINUS",-100,100, &graphValue, 20);
  gAddSlider(-100,100,"VALUE",&graphValue);
  gAddFixedGraph("FIXED GRPAPH",-100,100,100,&fixedGraph,20);
  // The graphs take up two columns we are going to add two
  gAddColumn();
  gAddColumn();
  // Add more stuff here.
 */
}

// Method called everytime a button has been pressed in the interface.
void gButtonPressed(int id)
{
 // if(buttonId == id)
  {
  
  }
}







