#include <TankAndFlowSensorController.h>

#define SENSOR_INPUT_1 5
#define SENSOR_INPUT_2  4

// #define SENSOR_INPUT_1 5
// #define SENSOR_INPUT_2 4
//
// fowMeter1 variables
//
long flowMeterCurrentMillis = 0;
long flowMeterPreviousMillis = 0;
float calibrationFactor = 4.5;
volatile int flowMeterPulseCount;
int pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
uint8_t samples=10;
//
// end of flow meter1 variables
//

//
// fowMeter2 variables
//
long flowMeterCurrentMillis2 = 0;
long flowMeterPreviousMillis2 = 0;

volatile int flowMeterPulseCount2;
int pulse2Sec = 0;
float flowRate2;
unsigned int flowMilliLitres2;
unsigned long totalMilliLitres2;
//
// end of flow meter1 variables
//



TankAndFlowSensorController::TankAndFlowSensorController(HardwareSerial &serial,  PanchoTankFlowData& tf, TM1637Display d1,TM1637Display d2) :
 _HardSerial(serial), panchoTankFlowData(tf) , display1(d1), display2(d2)  {}


 void   TankAndFlowSensorController::pulseCounter()
{
	flowMeterPulseCount++;
}
 void   TankAndFlowSensorController::pulseCounter2()
{
	flowMeterPulseCount2++;
}

/*
static void IRAM_ATTR TankAndFlowSensorController::pulseCounter()
{
	flowMeterPulseCount++;
}
static void IRAM_ATTR TankAndFlowSensorController::pulseCounter2()
{
	flowMeterPulseCount2++;
}
*/

void TankAndFlowSensorController::begin(uint8_t m){
    mode=m;
    pinMode(SENSOR_INPUT_1, INPUT_PULLUP);
    pinMode(SENSOR_INPUT_2, INPUT_PULLUP);

    //
	// flowMeter variables init
	//
	flowMeterPulseCount = 0;
	flowRate = 0.0;
	flowMilliLitres = 0;
	totalMilliLitres = 0;
	flowMeterPreviousMillis = 0;
	if(mode<3)attachInterrupt(digitalPinToInterrupt(SENSOR_INPUT_1), TankAndFlowSensorController::pulseCounter, FALLING);
	if(mode==2)attachInterrupt(digitalPinToInterrupt(SENSOR_INPUT_2), TankAndFlowSensorController::pulseCounter2, FALLING);
}


/*
 * Mode=0  1 Flowmeter
 * Mode=1  2 Flowmeters
 * Mode=2  1 Flow 1 Tank
 * Mode 3  1 Tank
 * Mode 4  2 Tanks
*/
void TankAndFlowSensorController::process(){
	//_HardSerial.print("processing, mode=");
	//_HardSerial.println(mode);
	
    switch(mode){
        case 0:
            readFlowMeter1();
            break;
        case 1:
            readFlowMeter1();
            readFlowMeter2();
            break;
        case 2:
            readFlowMeter1();
            readTank1();
            break;
        case 3:
            readTank1();
            break;
        case 4:
            readTank1();
            readTank2();
            break;
    }
}

void TankAndFlowSensorController::refreshDisplays(){
    switch(mode){
        case 0:
            
			if(panchoTankFlowData.flowRate==(int)panchoTankFlowData.flowRate){
    			display1.showNumberDec(panchoTankFlowData.flowRate, false);
  			}else{
				display1.showNumberDecEx(panchoTankFlowData.flowRate, (0x80 >> 1), false);
			}
			display2.showNumberDec(panchoTankFlowData.totalMilliLitres, false);
            break;
        case 1:
            if(panchoTankFlowData.flowRate==(int)panchoTankFlowData.flowRate){
    			display1.showNumberDec(panchoTankFlowData.flowRate, false);
  			}else{
				display1.showNumberDecEx(panchoTankFlowData.flowRate, (0x80 >> 1), false);
			}
			

			if(panchoTankFlowData.flowRate2==(int)panchoTankFlowData.flowRate2){
    			display2.showNumberDec(panchoTankFlowData.flowRate2, false);
  			}else{
				display2.showNumberDecEx(panchoTankFlowData.flowRate2, (0x80 >> 1), false);
			}
			
            break;
        case 2:
            if(panchoTankFlowData.flowRate==(int)panchoTankFlowData.flowRate){
    			display1.showNumberDec(panchoTankFlowData.flowRate, false);
  			}else{
				display1.showNumberDecEx(panchoTankFlowData.flowRate, (0x80 >> 1), false);
			}

			if(panchoTankFlowData.tankWaterLevel==(int)panchoTankFlowData.tankWaterLevel){
    			display2.showNumberDec(panchoTankFlowData.tankWaterLevel, false);
  			}else{
				display2.showNumberDecEx(panchoTankFlowData.tankWaterLevel, (0x80 >> 1), false);
			}
            break;
        case 3:
            if(panchoTankFlowData.tankWaterLevel==(int)panchoTankFlowData.tankWaterLevel){
    			display1.showNumberDec(panchoTankFlowData.tankWaterLevel, false);
  			}else{
				display1.showNumberDecEx(panchoTankFlowData.tankWaterLevel, (0x80 >> 1), false);
			}
            break;
        case 4:
            if(panchoTankFlowData.tankWaterLevel==(int)panchoTankFlowData.tankWaterLevel){
    			display1.showNumberDec(panchoTankFlowData.tankWaterLevel, false);
  			}else{
				display1.showNumberDecEx(panchoTankFlowData.tankWaterLevel, (0x80 >> 1), false);
			}
            
			if(panchoTankFlowData.tank2WaterLevel==(int)panchoTankFlowData.tank2WaterLevel){
    			display2.showNumberDec(panchoTankFlowData.tank2WaterLevel, false);
  			}else{
				display2.showNumberDecEx(panchoTankFlowData.tank2WaterLevel, (0x80 >> 1), false);
			}

            break;
    }
}
void TankAndFlowSensorController::readTank1(){
    //
    // tank level
    //
    // Output: 0.5-4.5V linear voltage output. 0 psi outputs 0.5V, 2.5 psi outputs 2.5V, 5 psi outputs 4.5V.
    // 1PSI=.7m of head
     float total=0.0;
    float average=0.0;
    const float  OffSet = 0.483 ;
    for (int x = 0; x < samples; x++) {           // multiple analogue readings for averaging
      total = total + analogRead(SENSOR_INPUT_1);  // add each value to a total
     }
    average = total / samples;

    panchoTankFlowData.tankPressureVolts=average;
    panchoTankFlowData.tankPressurePsi=panchoTankFlowData.qfactor*average/4095;
    panchoTankFlowData.tankWaterLevel=average*.7;
}

float tank2PressurePsi=0.0;
	float tank2PressureVolts=0.0;
	float tank2WaterLevel=0.0;
	float tank2HeightMeters=.3;


void TankAndFlowSensorController::readTank2(){
    //
    // tank level
    //
    // Output: 0.5-4.5V linear voltage output. 0 psi outputs 0.5V, 2.5 psi outputs 2.5V, 5 psi outputs 4.5V.
    // 1PSI=.7m of head
     float total=0.0;
    float average=0.0;
    const float  OffSet = 0.483 ;
    for (int x = 0; x < samples; x++) {           // multiple analogue readings for averaging
      total = total + analogRead(SENSOR_INPUT_2);  // add each value to a total
     }
    average = total / samples;
    panchoTankFlowData.tank2PressureVolts=average;
    panchoTankFlowData.tank2PressurePsi=panchoTankFlowData.qfactor*average/4095;
    panchoTankFlowData.tank2WaterLevel=average*.7;
}

void TankAndFlowSensorController::readFlowMeter1(){
		pulse1Sec = flowMeterPulseCount;

		flowMeterPulseCount = 0;
		  _HardSerial.print(" pulse1Sec=");
		  _HardSerial.print(pulse1Sec);

		// Because this loop may not complete in exactly 1 second intervals we calculate
		// the number of milliseconds that have passed since the last execution and use
		// that to scale the output. We also apply the calibrationFactor to scale the output
		// based on the number of pulses per second per units of measure (litres/minute in
		// this case) coming from the sensor.
		long lo = millis() - flowMeterPreviousMillis;
		flowRate = (1000.0 / lo) * pulse1Sec / panchoTankFlowData.qfactor;
		flowMeterPreviousMillis = millis();
		_HardSerial.print(" lo=");
		  _HardSerial.print(lo);

		  _HardSerial.print(" flow=");
		  _HardSerial.println(flowRate);
		// Divide the flow rate in litres/minute by 60 to determine how many litres have
		// passed through the sensor in this 1 second interval, then multiply by 1000 to
		// convert to millilitres.
		flowMilliLitres = (flowRate / 60) * 1000;
		// Add the millilitres passed in this second to the cumulative total
		totalMilliLitres += flowMilliLitres;
		//
		// end of checking flow meter
		//
		panchoTankFlowData.flowRate=flowRate;
		panchoTankFlowData.totalMilliLitres=totalMilliLitres;
    }

    void TankAndFlowSensorController::readFlowMeter2(){
		pulse2Sec = flowMeterPulseCount2;

		flowMeterPulseCount2 = 0;
		//  Serial.print(" pulse1Sec=");
		//  Serial.print(pulse1Sec);

		// Because this loop may not complete in exactly 1 second intervals we calculate
		// the number of milliseconds that have passed since the last execution and use
		// that to scale the output. We also apply the calibrationFactor to scale the output
		// based on the number of pulses per second per units of measure (litres/minute in
		// this case) coming from the sensor.
		long lo = millis() - flowMeterPreviousMillis2;
		flowRate = (1000.0 / lo) * pulse2Sec / panchoTankFlowData.qfactor;
		flowMeterPreviousMillis2 = millis();
		//Serial.print("pulse1Sec=");
		//  Serial.print(pulse1Sec);

		//  Serial.print(" flow=");
		//  Serial.println(flowRate);
		// Divide the flow rate in litres/minute by 60 to determine how many litres have
		// passed through the sensor in this 1 second interval, then multiply by 1000 to
		// convert to millilitres.
		flowMilliLitres2 = (flowRate2 / 60) * 1000;
		// Add the millilitres passed in this second to the cumulative total
		totalMilliLitres2 += flowMilliLitres2;
		//
		// end of checking flow meter
		//
		panchoTankFlowData.flowRate2=flowRate2;
		panchoTankFlowData.totalMilliLitres2=totalMilliLitres2;
    }


 TankAndFlowSensorController::~TankAndFlowSensorController() {}
