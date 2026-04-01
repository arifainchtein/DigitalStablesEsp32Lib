


#ifndef LIBRARIES_DIGITALSTABLES_CHINAMPAWIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLES_CHINAMPAWIFIMANAGER_H_
#include "Arduino.h"
#include <WifiManager.h>
// #include <ChinampaData.h>
//#include <RosieData.h>
#include <ChinampaData.h>


class ChinampaWifiManager : public WifiManager{



public:
	ChinampaData& chinampaData;
    ChinampaConfigData& chinampaConfigData;
	const char* sumpSSID = "SumpTrough";
	const char* sumpPassword = ""; // Assuming no password based on your logic
	const char* sumpUrl = "http://192.168.4.1/DaffodilServlet?formName=GetWebData";

	ChinampaWifiManager(HardwareSerial& serial , FS &fs, PCF8563TimeManager& t, Esp32SecretManager& e, ChinampaData& tf, ChinampaConfigData& p) ;
	void start( );
	void setWifiActiveSwitchStatus(bool b);
	void generateWebData(DynamicJsonDocument& json, String s);
	int uploadDataToDigitalStables();
	bool pullSumpDataViaWifi();

    virtual ~ChinampaWifiManager();

private:
	String okString="Ok";

};
#endif /* LIBRARIES_DIGITALSTABLES_CHINAMPAWIFIMANAGER_H_ */