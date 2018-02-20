#include "mbed.h"
#include "TextLCD.h"
//Init LCD display-a
TextLCD lcd(PB_0, PA_4, PC_0, PC_1, PC_2, PC_3); // RS, E, D4-D7  RW na GND

//Init tajmera
Timer _timer1;
Timer _timer2;

//Init za interupt i pomocni GPIO za enkoder
InterruptIn menuChange(PC_10);
DigitalIn menuChangeHelp(PC_12);
InterruptIn enkoderTaster(PC_11);//Init interupt za taster na enkoderu


//****************************PIN INIT******************************************
DigitalOut _injector1(PB_2); //digitalni izlaz na brizgaljku1
DigitalOut _injector2(PB_1); //digitalni izlaz na brizgaljku2
DigitalOut _injector3(PB_15); //digitalni izlaz na brizgaljku3
DigitalOut _injector4(PB_14); //digitalni izlaz na brizgaljku4
DigitalOut _pump(PB_13);      //digitalni izlaz za pumpu
//******************************************************************************
int rowLCD = 2; //trenutni red kursora
int trenutniMenu = 1;//trenutni depth menija
int promenaMenija = 0; //da li se desila promena menija
int _vremeUbrizgavanja = 1; //vreme ubrizagavanja u ms
int _vremeSimulacije = 5000;// vreme simulacije u ms
int n = 0;
const char* _menu1[4] = {"  Testiranje dizni","Rezimi testiranja:","*  Kontinualni ","   Diskretni"};
const char* _menu2[2] ={"*Pokreni testiranje", " Izadji"};
const char* _menu3[4] ={"*Prazan hod"," Pun gas", " Zeljeni rezim"," Izadji"};
const char* _menu4[2] = {"Vreme ubrizgavanja:","Vreme simulacije:"};

//funkcija za menu change interrupt 
void menuChangeInt()
    {
//*************Menu 1***********************************************************
        if(trenutniMenu == 1){
            if(menuChangeHelp ==1){
                    rowLCD++;
                    if(rowLCD > 3) rowLCD = 2;
                }
                else{
                    rowLCD--;
                    if(rowLCD <2) rowLCD = 3;     
                }
                for(int i = 2; i<4;i++){
                        lcd.locate(0,i);
                        lcd.printf(" ");
                        }
                        lcd.locate(0,rowLCD);
                        lcd.printf("*");
                        wait_ms(200);
            }
//*************Menu 2 **********************************************************  
       else if(trenutniMenu == 2){
            if(menuChangeHelp == 1){
                    rowLCD++;
                    if(rowLCD > 1) rowLCD = 0;
            }
            else{
                    rowLCD--;
                    if(rowLCD <0) rowLCD = 1;            
                    }
            for(int i = 0; i<2;i++){
                            lcd.locate(0,i);
                            lcd.printf(" ");
                            }
                            lcd.locate(0,rowLCD);
                        lcd.printf("*");
                        wait_ms(200);
            }
//************Menu 3************************************************************
        else if(trenutniMenu == 3){
                    if(menuChangeHelp == 1){
                            rowLCD++;
                            if(rowLCD >3) rowLCD = 0;
                        }
                    else{
                        rowLCD--;
                        if(rowLCD < 0) rowLCD = 3;        
                        }
                    for(int i = 0; i<4; i++){
                            lcd.locate(0,i);
                            lcd.printf(" ");
                                            }
                    lcd.locate(0,rowLCD);
                    lcd.printf("*");
                    wait_ms(200);
            } 
//********Menu 4****************************************************************
            else if(trenutniMenu == 4){
                if (n == 0){
                    if(menuChangeHelp == 1)
                    {
                     if(_vremeUbrizgavanja <20) _vremeUbrizgavanja++;
                     else _vremeUbrizgavanja = 20;
                     
                    }
                    else{
                     if(_vremeUbrizgavanja > 1) _vremeUbrizgavanja--;
                     else _vremeUbrizgavanja = 1;
                     }
                lcd.locate(0,1);
                lcd.printf("%d ms ", _vremeUbrizgavanja);
                     }
                if(n == 1){
                    if(menuChangeHelp ==1){
                        if(_vremeSimulacije <20000) _vremeSimulacije = _vremeSimulacije + 1000;
                        else _vremeSimulacije = 20000;
                        }
                    else{
                        if(_vremeSimulacije > 5000) _vremeSimulacije = _vremeSimulacije - 1000;
                        else _vremeSimulacije = 5000;
                        }
                    lcd.locate(0,3);
                    lcd.printf("%d s ", _vremeSimulacije/1000);
                    }     
    }
}
//Interrupt za taster
void udjiUMenu(){
    promenaMenija = 1;
    if(trenutniMenu == 4)
    {
        n++;
        
    }
    }
//Funkcija za kontinualni rezim testiranja
void kontinualniTest(){
    lcd.cls();
    wait_ms(500);
    lcd.locate(2,0);
    lcd.printf("Testiranje dizni");
    lcd.locate(2,1);
    lcd.printf("Priprema pumpe:");
    _pump = 1;
    wait_ms(5000); //cekanje 5 sekundi da pumpa postigne pritisak
    lcd.locate(18,1);
    lcd.printf("OK");
    lcd.locate(0,2);
    lcd.printf("Testiranje u toku...");
    _timer1.reset();
    _timer1.start();
    _injector1 = 1;
    _injector2 = 1;
    _injector3 = 1;
    _injector4 = 1;
    while(_timer1.read_ms() <10000);
        _timer1.stop();
        _injector1 = 0;
        _injector2 = 0;
        _injector3 = 0;
        _injector4 = 0;
        _pump = 0;
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("Testiranje je");
    lcd.locate(0,1);
    lcd.printf("zavrseno!");
    }

//Funckija za diskretni sistem testiranja
void diskretniTest(int vremeUbrizgavanja, int vremeTestiranja){
    int counter;
    lcd.cls();
    wait_ms(1000);
    lcd.locate(2,0);
    lcd.printf("Testiranje dizni");
    lcd.locate(2,1);
    lcd.printf("Priprema pumpe:");
    _pump = 1;
    wait_ms(5000); //cekanje 5 sekundi da pumpa postigne pritisak
    lcd.locate(18,1);
    lcd.printf("OK");
    lcd.locate(0,2);
    lcd.printf("Testiranje u toku...");
    _timer1.reset();
    _timer1.start();
        while(_timer1.read_ms() < vremeTestiranja)
        {
         wait_ms(vremeUbrizgavanja);
         switch(counter){
             case 1:
                _injector1 = 1;
                _injector2 = 0;
                _injector3 = 0;
                _injector4 = 0;
                break;
             case 2:
                _injector1 = 0;
                _injector2 = 0;
                _injector3 = 1;
                _injector4 = 0;
                break;
            case 3:
                _injector1 = 0;
                _injector2 = 0;
                _injector3 = 0;
                _injector4 = 1;
                break;
            case 4:
                _injector1 = 0;
                _injector2 = 1;
                _injector3 = 0;
                _injector4 = 0;
                break;
             
             }
             counter++;
            if(counter>4) counter = 1;  
        
        }
        _timer1.stop();
        _injector1 = 0;
        _injector2 = 0;
        _injector3 = 0;
        _injector4 = 0;
        _pump = 0;
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Testiranje je");
        lcd.locate(0,1);
        lcd.printf("zavrseno!");
        wait(2);
    }
/******************************************************************************/

//****main function*************************************************************
int main() {
    lcd.cls(); 
    wait_ms(500);
    lcd.locate(0,0);
    lcd.printf("Diplomski rad");
    lcd.locate(0,1);
    wait_ms(50);
    lcd.printf("Mihailo Stefanovic");
    lcd.locate(0,2);
    wait_ms(50);
    lcd.printf("Mikroprocesorski");
    lcd.locate(0,3);
    wait_ms(50);
    lcd.printf("Softver");
    wait(3);
    lcd.cls();
    wait_ms(500);
    
    for(int i = 0; i<4;i++){
            lcd.locate(0,i);
            lcd.printf(_menu1[i]);
        }
        _injector1 = 0;
        _injector2 = 0;
        _injector3 = 0;
        _injector4 = 0;
        _pump = 0;
    wait(1);
    menuChange.fall(&menuChangeInt); //na opadajucu ivicu interrupt za kretanje po menu
    enkoderTaster.rise(&udjiUMenu); //na uzlaznu ivicu interrupt za dalje prolazenje kroz menu
    while(true){
        if(promenaMenija == 1){
                promenaMenija = 0;
                if(trenutniMenu == 1)
                    {
                        if(rowLCD == 2)
                        {
                            trenutniMenu = 2;
                            lcd.cls();
                            wait_ms(200);
                            for(int i = 0; i < 2; i++){
                                lcd.locate(0,i);
                                lcd.printf(_menu2[i]);
                                rowLCD = 0;
                                }  
                        }
                        else if(rowLCD == 3)
                        {
                            trenutniMenu = 3;
                            lcd.cls();
                            wait_ms(200);
                            for(int i = 0; i<4; i++){
                                lcd.locate(0,i);
                                lcd.printf(_menu3[i]);
                                }
                            rowLCD = 0;
                        }
                    }
                else if(trenutniMenu == 2)
                    {
                        if(rowLCD == 0)
                        {
                            kontinualniTest();
                            wait(2);
                            trenutniMenu = 1;
                            lcd.cls();
                            wait(1);
                            for(int i = 0; i<4;i++)
                            {
                                lcd.locate(0,i);
                                lcd.printf(_menu1[i]);
                            }
                            rowLCD = 2;
                        }
                        else if(rowLCD == 1)
                        {
                            trenutniMenu = 1;
                            lcd.cls();
                            wait(1);
                            for(int i = 0; i<4;i++)
                            {
                                lcd.locate(0,i);
                                lcd.printf(_menu1[i]);
                            }
                            rowLCD = 2;
                        }
                    }
                else if(trenutniMenu == 3){
                    if(rowLCD == 0){
                            diskretniTest(2,10000);
                            trenutniMenu = 1;
                            lcd.cls();
                            wait(1);
                            for(int i = 0; i<4;i++)
                            {
                                lcd.locate(0,i);
                                lcd.printf(_menu1[i]);
                            }
                            rowLCD = 2;
                        }
                    else if(rowLCD == 1){
                            diskretniTest(6,10000);
                            trenutniMenu = 1;
                            lcd.cls();
                            wait(1);
                            for(int i = 0; i<4;i++)
                            {
                                lcd.locate(0,i);
                                lcd.printf(_menu1[i]);
                            }
                            rowLCD = 2;
                        }
                    else if(rowLCD == 2){
                            trenutniMenu = 4;
                            lcd.cls();
                            lcd.locate(0,0);
                            lcd.printf(_menu4[0]);
                            lcd.locate(0,1);
                            
                            lcd.printf("%d ms", _vremeUbrizgavanja);
                            lcd.locate(0,2);
                            lcd.printf(_menu4[1]);
                            lcd.locate(0,3);
                            lcd.printf("%d s", _vremeSimulacije/1000);
                        
                        }
                    else if(rowLCD == 3){
                        trenutniMenu = 1;
                            lcd.cls();
                            wait(1);
                            for(int i = 0; i<4;i++)
                            {
                                lcd.locate(0,i);
                                lcd.printf(_menu1[i]);
                            }
                            rowLCD = 2;
                        }
                    
                    }
                else if(trenutniMenu == 4){
                    if(n ==2){
                        diskretniTest(_vremeUbrizgavanja,_vremeSimulacije);
                        trenutniMenu = 1;
                        n = 0;
                        _vremeUbrizgavanja = 1;
                        _vremeSimulacije = 5000;
                        lcd.cls();
                        wait(1);
                        for(int i = 0; i<4;i++)
                            {
                                lcd.locate(0,i);
                                lcd.printf(_menu1[i]);
                            }
                        rowLCD = 2;
                        }
                    }
            }
    }              
}
