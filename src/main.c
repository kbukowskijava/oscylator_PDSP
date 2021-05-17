#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include "stdlib.h"

typedef struct{
    float amplituda; // wartość rzeczywista w mV w zakresie od 0 do 1650
    float czestotliwosc; // wartość rzeczywista w Hz w zakresie od 0 do Fs/2 (Fs - częstotliwość próbkowania)
    uint32_t n;
    uint8_t duty_cycle;
}Oscylator;

Oscylator Gen1;

void Oscylator_init (Oscylator * x_dane, float A, float f, uint8_t wypelnienie){
    x_dane -> amplituda = A;
    x_dane -> czestotliwosc = f;
    x_dane -> n = 0;
    x_dane -> duty_cycle = wypelnienie;
}

int8_t Oscylator_sinus(Oscylator * x_dane){
    float y;
    y = x_dane->amplituda*sin(((M_PI * 2.0f)/8000.0f)*x_dane->czestotliwosc*x_dane->n);
    x_dane->n++;
    return (int8_t)y;
} 

int sgn(double v) {
  if (v < 0) return -1;
  if (v > 0) return 1;
  return 0;
}

int8_t Oscylator_prostokat(Oscylator * x_dane){
    float y;
    y = sgn(Oscylator_sinus(x_dane))*x_dane->amplituda;
    return (int8_t)y;
}

int8_t Oscylator_trojkat (Oscylator * x_dane){
    float y1, y2, okres, at, bt;
    okres = 1/(x_dane->czestotliwosc);
    at = x_dane->amplituda;
    bt = (1/2)*okres*(1+(x_dane->duty_cycle/100));
    y1 = (at/bt)*x_dane->n;
    y2 = -1*(x_dane->amplituda/(okres-bt));
    x_dane->n++;
    if(y1>=x_dane->amplituda){
        if (y2<=0){
            x_dane->n=0;
        }
        return (int8_t)y2;
    }
    else
    {
        return (int8_t)y1;
    }
}

int8_t Oscylator_losowy(Oscylator * x_dane){
    float y;
    y = (-1*x_dane->amplituda) + ((x_dane->amplituda-(-1*x_dane->amplituda)+1) * rand()/(RAND_MAX+1.0));
    return (int8_t)y;
}

int8_t Oscylator_losowy_gauss(Oscylator * x_dane)
{
    float y, sigma, Mi, v1, v2;
    sigma = (x_dane->amplituda)/3; 
    Mi = 0; 
    v1 = ((float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    v2 = ((float)(rand()) + 1. )/( (float)(RAND_MAX) + 1. );
    y = cos(2*M_PI*v2)*sqrt(-2.*log(v1)) * sigma + Mi;
    return (int8_t)y;
}

int main (int argc, char const *argv[]){

    Oscylator_init(&Gen1,100,1000,50); // inicjalizacja struktury 
    /*
    1. harmoniczny (np. sinusoidalny) - powtórzenie z zajęć;
    2. prostokątny;
    3. trójkątny (wypełnienie wpływa na czas narastania zbocza, skrajnie dla wypełnienia 0 lub 100 mamy sygnał piłokształtny);
    4.losowy o rozkładzie równomiernym (losowa wartość z przedziału +- amplituda);
    5. losowy o rozkładzie normalnym (amplituda odpowiada 3 odchyleniom standardowym, wartość oczekiwana wynosi 0).
    */
    uint8_t wybor = 4;
    uint32_t liczba_probek = 20; // liczba zwróconych próbek
    switch (wybor){
        case 1:
            for (uint32_t i = 0; i < liczba_probek; i++)
                {
                    printf("%d ", Oscylator_sinus(&Gen1));
                }
            break;
        case 2:
            for (uint32_t i = 0; i < liczba_probek; i++)
                {
                    printf("%d ", Oscylator_prostokat(&Gen1));
                }
            break;
        case 3:
            for (uint32_t i = 0; i < liczba_probek; i++)
                {
                    printf("%d ", Oscylator_trojkat(&Gen1));
                }
            break;
        case 4:
            for (uint32_t i = 0; i < liczba_probek; i++)
                {
                    printf("%d ", Oscylator_losowy(&Gen1));
                }
            break;
        case 5:
            for (uint32_t i = 0; i < liczba_probek; i++)
                {
                    printf("%d ", Oscylator_losowy_gauss(&Gen1));
                }
            break;
    }
    return 0;
}
