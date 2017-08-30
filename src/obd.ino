/*
   //      _______ __  _________ _________
   //      \_     |  |/  .  \   |   \ ___/
   //        / :  |   \  /   \  |   / _>_
   //       /      \ | \ |   /  |  /     \
   //      /   |___/___/____/ \___/_     /
   //      \___/--------TECH--------\___/
   //       ==== ABOVE SCIENCE 1994 ====
   //
   //   Ab0VE TECH - ISO9141 K-Line controller
 */

/* Controller connections
   //            +-----------+
   // MC33290 <- • TX    Vin •
   // MC33290 -> • RX  A Gnd •  <- GND
   //            • RST R RST •
   //     GND -> • GND D  +5 •  <- +5V Reg. LM2596HV
   //            • 2   U  A7 •
   //            • 3   I  A6 •
   //            • 4   N  A5 •
   //            • 5   O  A4 •
   //            • 6      A3 •
   //            • 7   N  A2 •
   //            • 8   A  A1 •
   //            • 9   N  A0 •
   //  DIMING -> • 10  O Arf •
   //   Setup -> • 11    3V3 •
   //  Button -> • 12 ||| 13 •
   //            +-----------+
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <Wire.h>

#include "errors.h"
/*
   После загрузки скетча в микроконтроллер соединяем между собой провода с ножек 0 и 1 (TX / RX) и подключаем к ножке K-line диагностического разъема Хонды и провод массы на массу.
   В моем разъеме OBD1 нет +12В, питается пока от Кроны.
   Кнопками "Up" — "Down" переключаешься по 17 страницам, на каждой один параметр (в первой строке название, во второй величина.
   На кнопке "Right" уменьшение яркости подсветки (если сильно тускло — нажмите "Reset")
   На 18 странице (или с первой вверх) рисует шкалу мгновенного расхода топлива от 0 до 30 л/100км (сделал копию штатного расходомера на Тойота Виста 2001года).
 */

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); //такие ножки для "LCD1602 Keypad Shield"

int BrightnessPin=3; // нога, задающая яркость
int Brightness=200;  // Значение яркости

int NPage = 1; //какая страница печатается

int NPageMax = 18; //Максимальное количество страниц

int rpm=101;
int vss=60;
int temp_cool=103;
int temp_air=104;
float abs_pres=105;
float atm_pres=106;
int thr_pos=107;
float volt_lz1=108;
float volt_lz2=109;
float volt_net=110;
float load_gen=111;
float load_eld=112;
float pos_egr=113;
int st_cor=114;
int lt_cor=115;
float time_inj=1;
int ign=117;
int limit_ign=118;
float valv_idle=119;
float valve_egr=120;
float pos_valve_egr=121;
int fuelc = 1;

// define some values used by the panel and buttons
int lcd_key = 0;
int adc_key_in = 0;
#define btnRIGHT 0 // Уменьшить яркость подсветки
#define btnUP 1 // Предыдущий параметр
#define btnDOWN 2 // Следующий параметр
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

int i = 1;

// Это для рисования шкалы расхода на 18-й странице
int Rpm_min = 0; //Нижняя возможная граница
int Rpm_max = 30; //Верхняя возможная граница (задайте хоть 10, хоть 100л/100км)
int RightPos = 14; //с какого места печатать правую границу (16 минус количество цифр)

// Это пять символов для шкалы
byte S_1[8] =
{
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
};
byte S_2[8] =
{
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
};
byte S_3[8] =
{
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
};
byte S_4[8] =
{
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
};
byte S_5[8] =
{
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
};

void setup()
{
        pinMode(BrightnessPin, OUTPUT); //настроим порт для управления яркостью подсветки
        pinMode(0, INPUT); //настроим порт для чтения кнопок
        Serial.begin(9600); //скорость порта для связи с OBD-1

        lcd.begin(16, 2);
        lcd.setCursor(0, 0);
        lcd.print("HONDA OBD1"); // напечатаем приветствие
        lcd.setCursor(14, 1);

        for (i = 1; i < Brightness; i = i * 1.2 + 1) {
                analogWrite(BrightnessPin, i-1);
                delay(200);
        }
        analogWrite(BrightnessPin, Brightness); //зададим заданную яркость
        delay(1000);

// Регистрируем собственный символы с кодами 1-5 для линейной шкалы
        lcd.createChar(1, S_1);
        lcd.createChar(2, S_2);
        lcd.createChar(3, S_3);
        lcd.createChar(4, S_4);
        lcd.createChar(5, S_5);
        lcd.clear();
}

// пошли подпрограммы

// функция чтения
byte check_data24(int nbyte)
{
        int i;
        if (nbyte!=24) {
                for(i=0; i<nbyte; i++) Serial.read();
                return 0;
        }
        else {       //preamble
                for(i=0; i<5; i++) Serial.read();
                return 1;
        }
}

void honda_read_data_00_0f()
{
        int i;
        byte inData[19];
        for(i=0; i<19; i++)
                inData[i]=Serial.read();
        int srpm = (inData[2] << 8 | inData[3]);
        if (srpm > 0)
                rpm = 1875000/srpm;
        else
                rpm = 0;
        vss=(int)inData[4];
}

// функция вычисления температуры в градусах
int temp_function(float x)
{
        float result = 155.04149-x*3.0414878+x*x*0.03952185-x*x*x*0.00029383913+x*x*x*x*0.0000010792568-x*x*x*x*x*0.0000000015618437;
        return (int)result;
}

void honda_read_data_10_1f()
{
        int i;
        byte inData[19];
        for(i=0; i<19; i++)
                inData[i]=Serial.read();

        temp_cool=temp_function((float)inData[2]);
        temp_air=temp_function((float)inData[3]);
        abs_pres=(float)inData[4]*0.716-5.0;
        atm_pres=(float)inData[5]*0.716-5.0;
        thr_pos=(int)(((float)inData[6]-24.0)/2.0);
        volt_lz1=(float)inData[7]/51.3;
        volt_net=(float)inData[9]/10.45;
        load_gen=(float)inData[10]/2.55;
        load_eld=77.06-(float)inData[11]/2.5371;
        pos_egr=(float)inData[13]/51.3;
}

void honda_read_data_20_2f()
{
        int i;
        byte inData[19];
        for(i=0; i<19; i++)
                inData[i]=Serial.read();
        st_cor=((int)inData[2]/128-1)*100;
        lt_cor=((int)inData[4]/128-1)*100;
        time_inj=( (float)((inData[6] << 8) | inData[7]))/250.0;
        ign=((int)inData[8]-128)/2;
        limit_ign=((int)inData[9]-24)/4;
        valv_idle=(float)inData[10]/2.55;
        valve_egr=(float)inData[13]/2.55;
        pos_valve_egr=(float)inData[14]/2.55;
}

void honda_read_data_a0_af()
{
        int i;
        byte inData[19];
        for(i=0; i<19; i++)
                inData[i]=Serial.read();
        volt_lz2=(float)inData[2]/51.3;
}

// функция записи в порт
void honda_write_data(byte x, byte y)
{
        Serial.write(0x20); // command byte (read mem)
        Serial.write(0x05); // length in bytes
        Serial.write(x); // offset
        Serial.write(y); // Nr bytes to retun
        Serial.write(0x0100-(0x20+0x05+x+y)); //crc
}

// Функция опроса кнопок (они через резисторный делитель на нулевом аналоговом порту)
int read_LCD_buttons()
{
        adc_key_in = analogRead(0); // read the value from the sensor
// my buttons when read are centered at these valies: 0, 144, 329, 504, 741
// we add approx 50 to those values and check to see if we are close
        if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
        if (adc_key_in < 50) return btnRIGHT;
        if (adc_key_in < 195) return btnUP;
        if (adc_key_in < 380) return btnDOWN;
        if (adc_key_in < 555) return btnLEFT;
        if (adc_key_in < 790) return btnSELECT;
        return btnNONE; // when all others fail, return this…
}

//печать линейной шкалы в диапазоне от Rpm_min до Rpm_max
void show_scale ( int Rpm )
{
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(Rpm_min); //Печатаем левое значение шкалы
// lcd.print (Rpm); // или для проверки — абсолютное значение измеряемой величены
        lcd.setCursor(RightPos, 0);
        lcd.print(Rpm_max); //Печатаем правое значение шкалы
        int Pos = map (Rpm, Rpm_min, Rpm_max, 0, 160); // Приводим к шкале в 160 значений (хотя у нас каждая риска — "два")
        int End_all = int (Pos / 10); // количество полных символов
        int Dev = int ((Pos-End_all*10)/2);
        // сколько заполнять вертикальных черточек в неполной фигуре
        lcd.setCursor(0, 1);
        for (int i = 0; i < End_all; i++)
        {
                lcd.print("\5"); // печатаем полные квадратики
        }
        switch (Dev) { // выбираем какую неполную фигуру напечатать
        case 1:
                lcd.print("\1");
                break;
        case 2:
                lcd.print("\2");
                break;
        case 3:
                lcd.print("\3");
                break;
        case 4:
                lcd.print("\4");
                break;
        default:
                lcd.print (" ");
                lcd.print (" "); // забьем справа пробельчиками
        }
//а всё;
}


void loop()
{
        int nbyte;

//clear
        nbyte=Serial.available();
        check_data24(nbyte);

//00-0f
        honda_write_data(0x00,0x10);
        delay(50);
        nbyte=Serial.available();
        if (nbyte>0)
        {
                if (check_data24(nbyte))
                        honda_read_data_00_0f();
        }

//10-1f
        honda_write_data(0x10,0x10);
        delay(50);
        nbyte=Serial.available();
        if (nbyte>0)
        {
                if (check_data24(nbyte))
                        honda_read_data_10_1f();
        }

//20-2f
        honda_write_data(0x20,0x10);
        delay(50);
        nbyte=Serial.available();
        if (nbyte>0)
        {
                if (check_data24(nbyte))
                        honda_read_data_20_2f();
        }

//A0-AF
        honda_write_data(0xA0,0x10);
        delay(50);
        nbyte=Serial.available();
        if (nbyte>0)
        {
                if (check_data24(nbyte))
                        honda_read_data_a0_af();
        }

        float fuelsc = 230.0 * (float)rpm * time_inj * 0.000002;
        float fuelc = 0.0;
        if (vss>0)
                fuelc = fuelsc * 100.0 / (float)vss;

// Прочитаем кнопки
        lcd_key = read_LCD_buttons(); // read the buttons
        switch (lcd_key) // depending on which button was pushed, we perform an action
        {
        case btnRIGHT:
        {
                Brightness -=Brightness;          //уменьшим яркость (вернуть на максимум по кнопке RESET)
                analogWrite(BrightnessPin, Brightness); //зададим новую яркость
                delay (20);
                break;
        }
        case btnLEFT:
        {
// можно что-то сделать по нажатию и этой кнопки
                break;
        }
        case btnUP:
        {
                NPage--; // уменьшим номер выводимой страницы
                if (NPage < 1)
                {NPage = NPageMax;}
                delay (100);
                break;
        }
        case btnDOWN:
        {
                NPage++; // увеличим номер выводимой страницы
                if (NPage > NPageMax)
                {NPage = 1;}
                delay (100);
                break;
        }
        case btnSELECT:
        {
// можно что-то сделать по нажатию и этой кнопки
                break;
        }
        case btnNONE:
        {
// если ничего не нажато
                break;
        }
        }

        lcd.clear();
        lcd.setCursor(13, 0);
        lcd.print(".");
        lcd.print(NPage);
        lcd.setCursor(0, 0);
        switch (NPage)
        {
        case 1:
        {
                lcd.print("RPM:");
                lcd.setCursor(0, 1);
                lcd.print(rpm);
                break;
        }
        case 2:
        {
                lcd.print("Speed:");
                lcd.setCursor(0, 1);
                lcd.print(vss);
                break;
        }
        case 3:
        {
                lcd.print("TempCool:");
                lcd.setCursor(0, 1);
                lcd.print(temp_cool);
                break;
        }
        case 4:
        {
                lcd.print("TempAir:");
                lcd.setCursor(0, 1);
                lcd.print(temp_air);
                break;
        }
        case 5:
        {
                lcd.print("LZ1:");
                lcd.setCursor(0, 1);
                lcd.print(volt_lz1,2);
                break;
        }
        case 6:
        {
                lcd.print("LZ2:");
                lcd.setCursor(0, 1);
                lcd.print(volt_lz2,2);
                break;
        }
        case 7:
        {
                lcd.print("MAP:");
                lcd.setCursor(0, 1);
                lcd.print(abs_pres,2);
                break;
        }
        case 8:
        {
                lcd.print("Baro:");
                lcd.setCursor(0, 1);
                lcd.print(atm_pres,2);
                break;
        }
        case 9:
        {
                lcd.print("Thr pos:");
                lcd.setCursor(0, 1);
                lcd.print(thr_pos);
                break;
        }
        case 10:
        {
                lcd.print("L/100:");
                lcd.setCursor(0, 1);
                lcd.print(fuelc,2);
                break;
        }
        case 11:
        {
                lcd.print("L/h:");
                lcd.setCursor(0, 1);
                lcd.print(fuelsc,2);
                break;
        }
        case 12:
        {
                lcd.print("Volt:");
                lcd.setCursor(0, 1);
                lcd.print(volt_net,2);
                break;
        }
        case 13:
        {
                lcd.print("Load gen:");
                lcd.setCursor(0, 1);
                lcd.print(load_gen,2);
                break;
        }
        case 14:
        {
                lcd.print("Load eld:");
                lcd.setCursor(0, 1);
                lcd.print(load_eld,2);
                break;
        }
        case 15:
        {
                lcd.print("Pos egr:");
                lcd.setCursor(0, 1);
                lcd.print(pos_egr,2);
                break;
        }
        case 16:
        {
                lcd.print("St cor:");
                lcd.setCursor(0, 1);
                lcd.print(st_cor);
                break;
        }
        case 17:
        {
                lcd.print("It cor:");
                lcd.setCursor(0, 1);
                lcd.print(lt_cor);
                break;
        }
        case 18:
        {
                show_scale(fuelc);
                break;
        }
        }
}
