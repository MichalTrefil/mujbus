#ifndef MUJBUS_H
#define MUJBUS_H


#include <Arduino.h>

#define POCET b_in[3]
#define SERIALN buffer[2]
#define ID b_in[0]
#define D1 b_in[4]
#define D2 b_in[5]
#define D3 b_in[6]
#define D4 b_in[7]
#define D5 b_in[8]
#define D6 b_in[9]
#define TYP  b_in[1]
#define CRCLO b_in[POCET + 4]
#define CRCHI b_in[POCET + 5]
#define DATA_NA_UART port485->available()
#define DEJ_DATA port485->read()
class casovac
{
private:
    long START;
    long KONEC;
    long CAS;
    bool povoleno;

public:
  casovac (void) {}
    void set (long milis); /// zakladni cas
    void plus (long milis); /// pridat cas bez restartu
    void start ();  /// odstartuje casovac
    bool ok(void);  /// kontrola casovace
    void stop (void); /// zastaveni casovace
};


class mujbus
{
private:
    HardwareSerial* port485;
    uint8_t* g_data;
    uint8_t s_pole;
    uint8_t SLAVE_ID;
    uint16_t rs_dat;
    bool proMne;
    bool zmena;
    uint8_t b_in_p;
    uint8_t b_in [64];
    casovac Casovac;
    void zpracovaniZpravy(void);

public:
    mujbus();
    bool change();
    void initSlave (uint8_t,HardwareSerial*, uint32_t, uint8_t*, uint8_t );
    void loop();

};





#endif
