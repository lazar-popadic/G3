/*
 * timer.c
 *
 *  Created on: Oct 19, 2023
 *      Author: lazar
 */
/*
 TIM2 - za vreme
 TIM3 - enkoder 1
 TIM4 - enkoder 2
 TIM5 - pwm
 */

#include "timer.h"
#include "../../modules/odometrija/odometrija.h"
#include "stm32f4xx.h"

#define END_TIME 20*1000

static void
tim9_init ();
static void
tim3_init ();
static void
tim4_init ();
static void
tim2_init ();
static void
tim5_init ();

volatile uint32_t sys_time_ms = 0; //volatile da kompajler ne vrsi optimizaciju
bool flag_delay = true;
volatile int16_t state_enc1 = 0;
volatile int16_t state_enc2 = 0;

void
timer_init ()
{
  tim9_init ();			//vreme
  tim3_init ();			//enkoder 1 - desni pasivni tocak
  tim4_init ();			//enkoder 2 - levi pasivni tocak
  tim2_init ();			//enkoder 3 - desni maxon
  tim5_init ();			//enkoder 4 - levi maxon
  odometrija_init ();
}

static void
tim9_init ()
{
  RCC->APB2ENR |= (0b1 << 16);
  // 84MHz -> 1kHz
  // 1) 84MHz -> 1MHz
  TIM9->PSC = 84 - 1;		// -1 jer brojimo od 0
  // 2) 1MHz -> 1kHz
  TIM9->ARR = 1000 - 1;

  TIM9->CR1 &= ~((0b1 << 1) || (0b1 << 2)); //sta generise dogadjaj | dozvola dogadjaja ILI obrnuto
  TIM9->EGR |= (0b1 << 0);	// Reinicijalizacija timera
  TIM9->DIER |= (0b1 << 0);	//dozvola prekida
  while (!(TIM9->SR & (0b1 << 0)))
    ;		//cekanje da se izvrsi reinicijalizacija

  TIM9->SR &= ~(0b1 << 0);
  TIM9->CR1 |= (0b1 << 2);

  //odabir prekidne rutine
  uint8_t const TIM9_PREKID = 24;
  NVIC->ISER[0] |= (0b1 << TIM9_PREKID);

  //ne bi trebalo da je jos ukljucen
}

void
timer_start_sys_time ()
{
  TIM9->CR1 |= (0b1 << 0);	//tek ga ovo ukljucuje
}

bool
timer_end ()
{
  if (sys_time_ms == END_TIME)
    return true;
  return false;
}

bool
timer_delay_nonblocking (uint32_t delay_ms)
{
  static uint32_t start_sys_time_ms;
  if (flag_delay == true)				//da samo jednom udje
    {
      start_sys_time_ms = sys_time_ms;
      flag_delay = false;
    }

  if (sys_time_ms <= start_sys_time_ms + delay_ms)
    return false;
  flag_delay = true;
  return true;
}

static void
tim3_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 1);	//dozvola takta za tim3 pa za gpio port B

  TIM3->PSC = 0;				// zbog max rezolucije
  TIM3->ARR = 0xFFFF;// bitno je da najveci bit bude 1 zbog minusa i negativne brzine

  TIM3->SMCR &= ~(0b111 << 0);
  TIM3->SMCR |= (0b011 << 0);

  TIM3->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM3->CCMR1 |= (0b01 << 0);
  TIM3->CCMR1 &= ~(0b11 << 8);
  TIM3->CCMR1 |= (0b01 << 8);

  TIM3->CCER &= ~(0b101 << 1);			//neinvertovan kanal A
  TIM3->CCER &= ~(0b101 << 5);			//neinvertovan kanal B

  TIM3->CR1 |= (0b1 << 0);			//ukljucivanje timera
}

static void
tim4_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 2);	//dozvola takta za tim4 pa za gpio port B

  TIM4->PSC = 0;				// zbog max rezolucije
  TIM4->ARR = 0xFFFF;// bitno je da najveci bit bude 1 zbog minusa i negativne brzine

  TIM4->SMCR &= ~(0b111 << 0);
  TIM4->SMCR |= (0b011 << 0);

  TIM4->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM4->CCMR1 |= (0b01 << 0);
  TIM4->CCMR1 &= ~(0b11 << 8);
  TIM4->CCMR1 |= (0b01 << 8);

  TIM4->CCER &= ~(0b100 << 1);		//invertovan kanal A	0b xxxx 0x1x
  TIM4->CCER |= (0b001 << 1);
  //  TIM4->CCER &= ~(0b101 << 1);	//neinvertovan kanal A
  TIM4->CCER &= ~(0b101 << 5);		//neinvertovan kanal B	0b xxxx 0x0x

  TIM4->CR1 |= (0b1 << 0);			//ukljucivanje timera
}

static void
tim2_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 0);


  TIM2->PSC = 0;
  TIM2->ARR = 0xFFFF;

  TIM2->SMCR &= ~(0b111 << 0);
  TIM2->SMCR |= (0b011 << 0);

  TIM2->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM2->CCMR1 |= (0b01 << 0);
  TIM2->CCMR1 &= ~(0b11 << 8);
  TIM2->CCMR1 |= (0b01 << 8);

  TIM2->CCER &= ~(0b101 << 1);		//neinvertovan kanal A
  TIM2->CCER &= ~(0b101 << 5);		//neinvertovan kanal B

  TIM2->CR1 |= (0b1 << 0);			//ukljucivanje timera
}

static void
tim5_init ()					//ENKODER
{
  RCC->APB1ENR |= (0b1 << 3);

  TIM5->PSC = 0;
  TIM5->ARR = 0xFFFF;

  TIM5->SMCR &= ~(0b111 << 0);
  TIM5->SMCR |= (0b011 << 0);

  TIM5->CCMR1 &= ~(0b11 << 0);//povezujemo kanale enkodera timera sa kanalom samog timera
  TIM5->CCMR1 |= (0b01 << 0);
  TIM5->CCMR1 &= ~(0b11 << 8);
  TIM5->CCMR1 |= (0b01 << 8);

  TIM5->CCER &= ~(0b100 << 1);		//invertovan kanal A	0b xxxx 0x1x
  TIM5->CCER |= (0b001 << 1);
  TIM5->CCER &= ~(0b101 << 5);		//neinvertovan kanal B	0b xxxx 0x0x

  TIM5->CR1 |= (0b1 << 0);			//ukljucivanje timera
}

int16_t
timer_speed_of_encoder1 ()
{
  int16_t speed = (int16_t) TIM3->CNT - (int16_t) state_enc1;
  state_enc1 = (int16_t) TIM3->CNT;
  return speed;		//inkrementi, tj. impulsi
}

int16_t
timer_speed_of_encoder2 ()
{
  int16_t speed = (int16_t) TIM4->CNT - (int16_t) state_enc2;
  state_enc2 = (int16_t) TIM4->CNT;
  return speed;
}

void
TIM2_IRQHandler ()
{
  // poziva se svake milisekunde
  // proveri da li je stvarno TIM2 pozvao rutinu
  if ((TIM2->SR & (0b1 << 0)) == (0b1 << 0))
    {
      TIM2->SR &= ~(0b1 << 0);	// da bi sledeci put mogli da detektujemo prekid

      //if(sys_time_ms % 10)//svakih 10ms
      odometrija_robot ();//mozda probaj i brze od 1ms		TODO!!!!!!!!!!!!!!

      sys_time_ms++;
    }
}
