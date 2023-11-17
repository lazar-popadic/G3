/*
 * regulation.c
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#include "regulation.h"
#include <stdbool.h>
#include "../../periphery/timer/timer.h"
#include "../pwm/pwm.h"
#include "../odometrija/odometrija.h"
#include "math.h"

#define EPSILON_THETA		0.1
#define EPSILON_DISTANCE	0.1
#define EPSILON_DISTANCE_ROT	0.1
#define KP_SPEED		0
#define KI_SPEED		0
#define KD_SPEED		0
#define KP_ROT			0
#define KI_ROT			0
#define KD_ROT			0
#define KP_TRAN			0
#define KI_TRAN			0
#define KD_TRAN			0

#define EI_MAX 			0 // narednih 10 do 100 iteracija  vrednosti, ei ne sme preko toga ??????????
#define EI_MIN 			0
#define U_ROT_MAX		0
#define U_ROT_MIN		0

static float
regulation_rotation (float theta);

static float
regulation_translation (float distance, float theta);

static float
saturation (float signal, float MAX, float MIN);

static float ref_speed = 0;

static float e = 0;
static float e_i = 0;
static float e_d = 0;
static float e_previous = 0;

					// TODO: izbaci sve promenljive iz funkcija ovde, proveri da li su tako radili +381

static float u = 0;			// TODO: sve u inkremente!!!!!!!!!!

static float p1x = 0;
static float p1y = 0;
static float p0x = 0;
static float p0y = 0;
static float distance = 0;
static float theta_0 = 0;
static float theta_1 = 0;
static float theta_2 = 0;

static uint8_t state_regulation = 0;

void
regulation_init ()
{

}

static float
saturation (float signal, float MAX, float MIN)
{
  if (signal > MAX)
    return MAX;
  if (signal < MIN)
    return MIN;
  return signal;
}

void
regulation_speed ()		//TODO: sve radi u inkrementima!!!!!!!!!!!!!!!!!
{
  e = ref_speed - timer_speed_of_encoder1 (); //TODO: OBRATI PAZNJU DA SU OBA U ISTIM JEDINICAMA!!!!!!!!!
  e_i += e;
  e_d = e - e_previous;

  e_i = saturation (e_i, EI_MAX, EI_MIN);

  u = KP_SPEED * e + KI_SPEED * e_i + KD_SPEED * e_d;

  if (u > 0)
// smer 1
    ;
  else
// smer 2
    ;

  //uint16_t u_saturated;
  // u_saturated = saturation(u)			//U_MAX

  //pwm_duty_cycle((uint16_t)fabs(u_saturated));	//fabs je za float apsolutnu vrednost

  e_previous = e;
}

void
regulation_position () //TODO: preimenuj ovo, ovo je pronalazenje zeljene pozicije i ugla, MOZDA I NEMOJ, jer uvek treba da trazi
{			//TODO: PROMENLJIVE
  /*
   * p1 - zeljena pozicija u koordinatno sistemu robota
   * p0 - zeljena pozicija koju smo mi zadali (koordinatni sistem stola)
   */
  p1x = cos (get_theta ()) * (p0x - get_x ())
      - sin (get_theta ()) * (p0y - get_y ());
  p1y = sin (get_theta ()) * (p0x - get_x ())
      + cos (get_theta ()) * (p0y - get_y ());
  distance = sqrt (p1x * p1x + p1y * p1y);
  //prva rotacija
  theta_1 = atan2 (p1y, p1x);
  //druga rotacija
  theta_2 = theta_0 - get_theta ();

  //TODO: funkcije za rotaciju i translaciju
  // isto kao gore ...
  if (state_regulation == 0)
    {
      regulation_rotation (theta_1);
      state_regulation++;
    }
  if (state_regulation == 1)
    {
      regulation_translation (distance, theta_1);
      state_regulation++;
    }
  //treca rotacija, isto kao prva samo theta_2
  if (state_regulation == 2)
    {
      regulation_rotation (theta_2);
      state_regulation++;
    }
}

static float
regulation_rotation (float theta)
{
  float theta_previous;
  float theta_i;
  float theta_d;
  float u_rot;

  theta_i += theta;
  theta_d = theta - theta_previous;
  u_rot = KP_ROT * theta + KI_ROT * theta_i + KD_ROT * theta_d;
  u_rot=saturation(u_rot, U_ROT_MAX, U_ROT_MIN);
  //TODO: na jedan tocak salji plus, na drugo kao minus, brzina_desnog tocka => +u_rot1, brzina_levog_tocka => -u_rot1

  theta_previous = theta;

  if (fabs (theta) < EPSILON_THETA)
    {
      theta_i = 0;
      theta_d = 0;
      theta_previous = 0;
      return u_rot;
    }
}

static float
regulation_translation (float distance, float theta)
{
  float distance_previous;
  float distance_i;
  float distance_d;
  float u_tran;
  float u_rot;
  distance_i += distance;
  distance_d = distance - distance_previous;

  //ANTIWIND UP!!!!!!!!!!!!!
  u_tran = KP_TRAN * distance + KI_TRAN * distance_i + KD_TRAN * distance_d;
  if (fabs (distance) > EPSILON_DISTANCE_ROT)
    regulation_rotation(theta); //radi i reg rotacije dok translira
  else
    u_rot = 0;
  // brzina_desnog = u_tran + u_rot1 * faktor (npr. 0.5)		//OVO SU REF BRZINE ZA REG BRZINE
  // brzina_levog = u_tran - u_rot1 * faktor (npr. 0.5)		//DODAJES ROTACIJU SAMO DOK NIJE BLIZU CILJNE TACKE
  distance_previous = distance;

  if (fabs (distance) < EPSILON_DISTANCE)
    {
      distance_i = 0;
      distance_d = 0;
      distance_previous = 0;
      return u_tran;						//TODO: vidi da li da vraca ili da u funkciji vec uradi sta treba!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
}
