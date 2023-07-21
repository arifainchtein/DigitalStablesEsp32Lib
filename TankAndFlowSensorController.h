/*
 * TankAndFlowSensorController.h
 *
 *  Created on: 5 Dec 2021
 *      Author: arifainchtein
 */

#ifndef LIBRARIES_DIGITALSTABLES_TANKANDFLOWSENSORCONTROLLER_H_
#define LIBRARIES_DIGITALSTABLES_TANKANDFLOWSENSORCONTROLLER_H_
#include "Arduino.h"
#include <PanchoTankFlowData.h>
#include <TM1637Display.h>

class TankAndFlowSensorController{


protected:
	HardwareSerial& _HardSerial;
    PanchoTankFlowData& panchoTankFlowData;
    TM1637Display& display1;
    TM1637Display& display2;
    uint8_t mode;
 
public:
	TankAndFlowSensorController(HardwareSerial& serial , PanchoTankFlowData& tf, TM1637Display& d1,TM1637Display& d2 );
	void begin(uint8_t m);
    void process();
    void readTank1();
    void readTank2();
    void readFlowMeter1();
    void readFlowMeter2();
    void refreshDisplays();
    void setQFactor1(float q1);
    virtual ~TankAndFlowSensorController();
    
private:

   
    //
    // fowMeter1 variables
    //
    long flowMeterCurrentMillis = 0;
    long flowMeterPreviousMillis = 0;
    float calibrationFactor = 4.5;

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

    int pulse2Sec = 0;
    float flowRate2;
    unsigned int flowMilliLitres2;
    unsigned long totalMilliLitres2;
    //
    // end of flow meter1 variables
    //
    float tank2PressurePsi=0.0;
	float tank2PressureVolts=0.0;
	float tank2WaterLevel=0.0;
	float tank2HeightMeters=.3;
    
    void readFlowMeter();
    static void IRAM_ATTR pulseCounter();
    static void IRAM_ATTR pulseCounter2();
    
};

#endif /* LIBRARIES_DIGITALSTABLES_WIFIMANAGER_H_ */