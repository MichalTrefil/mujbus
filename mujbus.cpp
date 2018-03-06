#include <mujbus.h>
#include <crc.h>

mujbus::mujbus() {}
void mujbus::zpracovaniZpravy()
{

    uint8_t bhi,blo;
    calcCrc(b_in, POCET + 6,&blo,&bhi); // crc

    if (bhi == CRCHI && blo == CRCLO) // Kontrola CRC
    {
        switch (TYP)
        {

        case 49: // odpoved na ping             6/6

            TYP++;
            calcCrc(b_in,6, &CRCLO, &CRCHI);
            port485->write(b_in,6);
            break;

        case 51: //B4 = id , B5 = data prichozi   8/8
            g_data[b_in[4]] = b_in[5];
            TYP++;
            calcCrc(b_in, 8, &CRCLO, &CRCHI);
            port485->write(b_in, 8);
            zmena = true;
            break;

        case 53: //B4 = id , B5 = byt odchozi     7/8
            TYP++;
            POCET = 2;
            b_in[5] = g_data[b_in[4]];
            calcCrc(b_in,8, &CRCLO, &CRCHI);
            port485->write(b_in,8);
            break;

        case 61: // data prichozi serie           9+/6  1byt
            for(int i = 0; i< b_in[5]; i++)
                g_data[i + b_in[4]] = b_in[6+i];
            TYP++;
            POCET = 0;
            calcCrc(b_in,6, &CRCLO, &CRCHI);
            port485->write(b_in,6);
            zmena = true;
            break;

        case 63: //B4 = id , B5 = data odchozi    8/7+  1byt
            uint8_t  start = b_in[4]; //start adresa
            uint8_t  pocet = b_in[5]; //pocet bytu
            for(int i = 0; i< pocet; i++)
                b_in[4+i] = g_data[start + i];
            TYP++;
            POCET = pocet;
            calcCrc(b_in, POCET + 6, &CRCLO, &CRCHI);
            port485->write(b_in, 6 + POCET);
            break;
        }
    }
    else // Spatna CRC
    {

    }
}

void mujbus::initSlave(uint8_t id,HardwareSerial* port, uint32_t baud, uint8_t* p, uint8_t v)
{
    port485 = port;
    port485->begin(baud);
    SLAVE_ID = id;
    g_data = p;
    s_pole = v;
    Casovac.set(6);
    Casovac.start();
}

bool mujbus::change(void)
{
    bool tmp = zmena;
    zmena = 0;
    return tmp;
}

void mujbus::loop(void)
{
    rs_dat = DATA_NA_UART;
    do
    {
        if ( rs_dat > 0)
        {
            if (Casovac.ok() == 0 )   // prichozi byt je po delsi dobe je tedy prvni
            {
                b_in_p = 0;
                Casovac.start();
                proMne = false;
            }

            b_in[b_in_p] = DEJ_DATA; // nacteni znaku

            /// //////////////////// v prvnim bytu je addressa  /////////////////////////////////
            if (b_in_p == 0   &&   b_in[b_in_p]== SLAVE_ID   ||   b_in[b_in_p] == 255 )
                proMne = true;

            /// //////////////////// docist co je dostupne pro mne //////////////////////////////
            if(proMne)
            {
                while (DATA_NA_UART > 0 && b_in_p < 64)
                {
                    b_in_p++;
                    b_in[b_in_p] = DEJ_DATA; // nacteni znaku
                }
            }
            /////////////////////////////////////////////////////

            ////////////////////////////////////////////zahodit zbytek
            else
            {
                uint8_t dira = DEJ_DATA;
            }
            ///////////////////////////////////////////////////////////

            // rozhodnuti
            if (   proMne   &&  (POCET + 5)== b_in_p   &&   DATA_NA_UART==0   )
            {
                b_in_p = 0;
                if (proMne && ID != 255)
                    zpracovaniZpravy(); // soukroma
                else if (ID == 255)
                    Serial.println("Broadcast"); //broadcast
            }

            b_in_p ++;
            if (b_in_p >= 64)
                b_in_p = 0;
        }
        rs_dat = DATA_NA_UART;
    }
    while (rs_dat > 0);
}

void casovac::start ()
{
    povoleno = 0;
    START = millis();
    KONEC = START + CAS;
}

void casovac::set (long milis)
{
    CAS = milis;
}
void casovac::plus (long milis)
{
    KONEC = KONEC + milis;
}

bool casovac::ok(void)
{
    if(povoleno == 0 && KONEC < millis())
        return false ;
    else
        return  true;
}
void casovac::stop (void)
{
    povoleno = 1;
}


