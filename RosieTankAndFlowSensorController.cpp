#include <RosieTankAndFlowSensorController.h>
 #define SENSOR_INPUT_2 33
#define SENSOR_INPUT_1 32

static volatile int flowMeterPulseCount;
static volatile int flowMeterPulseCount2;


RosieTankAndFlowSensorController::RosieTankAndFlowSensorController(HardwareSerial &serial,  RosieData& tf) :
 _HardSerial(serial), rosieData(tf)   {}


 void   RosieTankAndFlowSensorController::pulseCounter()
{
	flowMeterPulseCount++;
}
 void   RosieTankAndFlowSensorController::pulseCounter2()
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

void RosieTankAndFlowSensorController::begin(uint8_t m){
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
	if(mode<3)attachInterrupt(digitalPinToInterrupt(SENSOR_INPUT_1), RosieTankAndFlowSensorController::pulseCounter, FALLING);
	if(mode==2)attachInterrupt(digitalPinToInterrupt(SENSOR_INPUT_2), RosieTankAndFlowSensorController::pulseCounter2, FALLING);
}


/*
 * Mode=1  1 Flowmeter
 * Mode=2  2 Flowmeters
 * Mode=3  1 Flow 1 Tank
 * Mode 4  1 Tank
 * Mode 5  2 Tanks
*/
void RosieTankAndFlowSensorController::process(){
	//_HardSerial.print("processing, mode=");
	//_HardSerial.println(mode);
	
    switch(mode){
        case 1:
            readFlowMeter1();
            break;
        case 2:
            readFlowMeter1();
            readFlowMeter2();
            break;
        case 3:
            readFlowMeter1();
            readTank1();
            break;
        case 4:
            readTank1();
            break;
        case 5:
            readTank1();
            readTank2();
            break;
    }
}

void RosieTankAndFlowSensorController::refreshDisplays(){
	// _HardSerial.print( " refresh //display2, mode=");
	// _HardSerial.print( mode);
	// _HardSerial.print( " anchoTankFlowData.flowRate=");
	// _HardSerial.print( rosieData.flowRate);
	// _HardSerial.print( " anchoTankFlowData.flowRate2=");
	// _HardSerial.println( rosieData.flowRate2);
	int scaledFlow, display2D;
	float liters;
    switch(mode){
        case 1:
			if(rosieData.flowRate==(int)rosieData.flowRate){
    		///	////display21.showNumberDec(rosieData.flowRate, false);
  			}else{
				scaledFlow=rosieData.flowRate*100;
				//////display21.showNumberDecEx(scaledFlow, (0x80 >> 1), false);
			}

			liters = rosieData.totalMilliLitres/1000.0;
			if(totalMilliLitres<1000){
				//display22.showNumberDec(rosieData.totalMilliLitres, false);
			}else{
				if(liters>1000000){
					//display2D = 100*totalMilliLitres/1000000;
					//display22.showNumberDecEx(//display2D, (0x80 >> 1), false);
				}else if(liters>100000){
					//display2D = 100*totalMilliLitres/100000;
					//display22.showNumberDecEx(//display2D, (0x80 >> 1), false);
				}else if(liters>10000){
					//display2D = 100*liters/10000;
					//display22.showNumberDec(//display2D, false);
					//display22.showNumberDecEx(//display2D, (0x80 >> 1), false);
				}else if(liters<10000){
					//display2D = (int)liters;
					//display22.showNumberDec(//display2D, false);
				}
			}
            break;
        case 2:
            if(rosieData.flowRate==(int)rosieData.flowRate){
    			////display21.showNumberDec(rosieData.flowRate, false);
  			}else{
				scaledFlow=rosieData.flowRate*100;
				////display21.showNumberDecEx(scaledFlow, (0x80 >> 1), false);
			}
			

			if(rosieData.flowRate2==(int)rosieData.flowRate2){
    			//display22.showNumberDec(rosieData.flowRate2, false);
  			}else{
				scaledFlow=rosieData.flowRate2*100;
				//display22.showNumberDecEx(scaledFlow, (0x80 >> 1), false);
			}
			
            break;
        case 3:
            if(rosieData.flowRate==(int)rosieData.flowRate){
    			////display21.showNumberDec(rosieData.flowRate, false);
  			}else{
				scaledFlow=rosieData.flowRate*100;
				////display21.showNumberDecEx(scaledFlow, (0x80 >> 1), false);
			}

			if(rosieData.tank1WaterLevel==(int)rosieData.tank1WaterLevel){
    			//display22.showNumberDec(rosieData.tank1WaterLevel, false);
  			}else{
				scaledFlow=rosieData.flowRate2*100;
				//display22.showNumberDecEx(rosieData.tank1WaterLevel, (0x80 >> 1), false);
			}
            break;
        case 4:
            if(rosieData.tank1WaterLevel==(int)rosieData.tank1WaterLevel){
    			////display21.showNumberDec(rosieData.tank1WaterLevel, false);
  			}else{
				////display21.showNumberDecEx(rosieData.tank1WaterLevel, (0x80 >> 1), false);
			}
            break;
        case 5:
            if(rosieData.tank1WaterLevel==(int)rosieData.tank1WaterLevel){
    			////display21.showNumberDec(rosieData.tank1WaterLevel, false);
  			}else{
				////display21.showNumberDecEx(rosieData.tank1WaterLevel, (0x80 >> 1), false);
			}
            
			if(rosieData.tank2WaterLevel==(int)rosieData.tank2WaterLevel){
    			//display22.showNumberDec(rosieData.tank2WaterLevel, false);
  			}else{
				//display22.showNumberDecEx(rosieData.tank2WaterLevel, (0x80 >> 1), false);
			}

            break;
    }
}
void RosieTankAndFlowSensorController::readTank1(){
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
	//
	// average wil return a value where 3.3 represents 4095
	// so get the voltage

	float vol33 = 3.3*average/4095;
	float vol45 = 4.5*vol33/3.3;
	//
	// since the psi is 5 for 4.5 then
	float psi = vol45*5/4.5;
	
    rosieData.tank1PressureVolts=average;
    rosieData.tank1PressurePsi=psi;
    rosieData.tank1WaterLevel=psi*.7;
}




void RosieTankAndFlowSensorController::readTank2(){
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
	//
	// average wil return a value where 3.3 represents 4095
	// so get the voltage

	float vol33 = 3.3*average/4095;
	float vol45 = 4.5*vol33/3.3;
	//
	// since the psi is 5 for 4.5 then
	float psi = vol45*5/4.5;
	
    rosieData.tank2PressureVolts=average;
    rosieData.tank2PressurePsi=psi;
    rosieData.tank2WaterLevel=psi*.7;
}

void RosieTankAndFlowSensorController::readFlowMeter1(){
		pulse1Sec = flowMeterPulseCount;

		flowMeterPulseCount = 0;
	//	  _HardSerial.print(" pulse1Sec=");
	//	  _HardSerial.print(pulse1Sec);

		// Because this loop may not complete in exactly 1 second intervals we calculate
		// the number of milliseconds that have passed since the last execution and use
		// that to scale the output. We also apply the calibrationFactor to scale the output
		// based on the number of pulses per second per units of measure (litres/minute in
		// this case) coming from the sensor.
		long lo = millis() - flowMeterPreviousMillis;
		flowRate = (1000.0 / lo) * pulse1Sec / rosieData.qfactor1;
		//flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    
		flowMeterPreviousMillis = millis();
		//_HardSerial.print(" lo=");
		//  _HardSerial.print(lo);

		//  _HardSerial.print(" flow=");
		//  _HardSerial.println(flowRate);
		// Divide the flow rate in litres/minute by 60 to determine how many litres have
		// passed through the sensor in this 1 second interval, then multiply by 1000 to
		// convert to millilitres.
		if(flowRate>0){
			flowMilliLitres = rosieData.dataSamplingSec*(flowRate / 60) * 1000;
			// Add the millilitres passed in this second to the cumulative total
				totalMilliLitres += flowMilliLitres;
		}else{
			totalMilliLitres=0;
		}
		
		//
		// end of checking flow meter
		//
		rosieData.flowRate=flowRate;
		rosieData.totalMilliLitres=totalMilliLitres;
    }

    void RosieTankAndFlowSensorController::readFlowMeter2(){
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
		flowRate2 = (1000.0 / lo) * pulse2Sec / rosieData.qfactor2;
		flowMeterPreviousMillis2 = millis();
		//Serial.print("pulse1Sec=");
		//  Serial.print(pulse1Sec);

		//  Serial.print(" flow=");
		//  Serial.println(flowRate);
		// Divide the flow rate in litres/minute by 60 to determine how many litres have
		// passed through the sensor in this 1 second interval, then multiply by 1000 to
		// convert to millilitres.
		if(flowRate2>0){
			flowMilliLitres2 = rosieData.dataSamplingSec*(flowRate2 / 60) * 1000;
			// Add the millilitres passed in this second to the cumulative total
				totalMilliLitres2 += flowMilliLitres2;
		}else{
			totalMilliLitres2=0;
		}
		//
		// end of checking flow meter
		//
		rosieData.flowRate2=flowRate2;
		rosieData.totalMilliLitres2=totalMilliLitres2;
    }


 RosieTankAndFlowSensorController::~RosieTankAndFlowSensorController() {}
