/*
 * regulacija.c
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#include "regulacija.h"
#include "../../periphery/timer/timer.h"
#include "../pwm/pwm.h"
#include "../odometrija/odometrija.h"
#include "math.h"

static float ref_speed = 0;

static float kp = 0;
static float ki = 0;
static float kd = 0;

static float e = 0;
static float ei = 0;
static float ed = 0;
static float e_p = 0;

static float const EI_MAX = 0; // narednih 10 do 100 iteracija kp vrednosti, ei ne sme preko toga ??????????
static float const EI_MIN = 0;

static float u = 0;			// TODO: sve u inkremente!!!!!!!!!!

static float p1x = 0;
static float p1y = 0;
static float p0x = 0;
static float p0y = 0;
static float theta_0 = 0;
static float theta_1 = 0;
static float theta_2 = 0;

void
regulacija_init ()
{

}

void
regulacija_speed ()		//TODO: sve radi u inkrementima!!!!!!!!!!!!!!!!!
{
  e = ref_speed - timer_speed_of_encoder1 (); //TODO: OBRATI PAZNJU DA SU OBA U ISTIM JEDINICAMA!!!!!!!!!
  ei += e;
  /*
   * antiwind up
   * saturacija!
   * TODO: funkcija za ovo
   */
  if (ei > EI_MAX)
    ei = EI_MAX;
  else if (ei < EI_MIN)
    ei = EI_MIN;
  ed = e - e_p;

  u = kp * e + ki * ei + kd * ed;

  if (u > 0)
// smer 1
    ;
  else
// smer 2
    ;

  //uint16_t u_saturated;
  // u_saturated = saturation(u)			//U_MAX

  //pwm_duty_cycle((uint16_t)fabs(u_saturated));	//fabs je za float apsolutnu vrednost

  e_p = e;
}

void
regulacija_position () //TODO: preimenuj ovo, ovo je pronalazenje zeljene pozicije i ugla, MOZDA I NEMOJ, jer uvek treba da trazi
{			//TODO: PROMENLJIVE
  /*
   * p1 - zeljena pozicija u koordinatno sistemu robota
   * p0 - zeljena pozicija koju smo mi zadali (koordinatni sistem stola)
   */
  p1x = cos (get_theta ()) * (p0x - get_x ())
      - sin (get_theta ()) * (p0y - get_y ());
  p1y = sin (get_theta ()) * (p0x - get_x ())
      + cos (get_theta ()) * (p0y - get_y ());
  //prva rotacija
  theta_1 = atan2 (p1y, p1x);
  //druga rotacija
  theta_2 = theta_0 - get_theta ();

  //TODO: funkcije za rotaciju i translaciju
  // isto kao gore ...
  if (state_regulation == 0)
    {
      theta_1_i += theta_1;
      theta_1_d = theta_1 - theta_1_pre;
      //ANTIWIND UP!!!!!!!!!!!!!
      u_rot1 = kp_rot * theta_1 + ki_rot * theta_1_i + kd_rot * theta_1_d;
      // na jedan tocak salji plus, na drugo kao minus, brzina_desnog tocka => +u_rot1, brzina_levog_tocka => -u_rot1

      theta_1_pre = theta_1;

      if (fabs (theta_1) < epsilon_theta_1)
	{
	  theta_1_i = 0;
	  theta_1_d = 0;
	  theta_1_pre = 0;
	  state_regulation++;
	}
    }
  if (state_regulation == 1)
    {
      e_tran = sqrt (p1x * p1x + p1y * p1y);
      e_tran_i += e_tran;
      e_tran_d = e_tran - e_tran_pre;

      //ANTIWIND UP!!!!!!!!!!!!!
      u_tran = kp_tran * e_tran + ki_tran * e_tran_i + kd_tran * e_tran_d;
      if (fabs(e_tran) > epsilon)
      u_rot1 = kp_rot * theta_1 + ki_rot * theta_1_i + kd_rot * theta_1_d;//radi i reg rotacije dok translira
      else
      e_rot1 = 0
      // brzina_desnog = u_tran + u_rot1 * faktor (npr. 0.5)		//OVO SU REF BRZINE ZA REG BRZINE
      // brzina_levog = u_tran - u_rot1 * faktor (npr. 0.5)		//DODAJES ROTACIJU SAMO DOK NIJE BLIZU CILJNE TACKE
      e_tran_pre = e_tran;

      if (fabs (e_tran) < epsilon_e_tran)
	{
	  e_tran_i = 0;
	  e_tran_d = 0;
	  e_tran_pre = 0;
	  state_regulation++;
	}
    }
  //treca rotacija, isto kao prva samo theta_2

}
