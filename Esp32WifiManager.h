#ifndef LIBRARIES_DIGITALSTABLESESP32LIB_ESP32WIFIMANAGER_H_
#define LIBRARIES_DIGITALSTABLESESP32LIB_ESP32WIFIMANAGER_H_


class Esp32WifiManager   {
	protected:
		HardwareSerial& _HardSerial;
		
	public:
		Esp32WifiManager(HardwareSerial& serial) ;
		void start();
		String  getIndexPage();
		void checkForClient();
		String getSSID();
		String getSoftAPSSID();
		String getRSSI();
		String getIPAddress();
		String getAPAddress();
		virtual ~Esp32WifiManager();
};

#endif /* LIBRARIES_ESP32ARDUINOLIB_ESP32WIFIMANAGER_H_ */
