#include "wrap/thread.h"
#include "wrap/synch.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Funktionen vi vill köra parallellt. Den finns definierad senare i filen.
int do_work(int param);

/**
 * Datastruktur som håller reda på en tråd som kör "do_work".
 *
 * I Pintos motsvaras denna av "struct thread".
 */
struct running_thread {
  // Den parameter som ska skickas till "do_work".
  int param;

  // Om tråden är klar: Resultatet som "do_work" har beräknat.
  int result;
};

// Första funktionen som körs i nya trådar.
void thread_main(struct running_thread *data) {
  data->result = do_work(data->param);
}

// Starta en ny tråd som kör funktionen "do_work" med "param" som
// parameter. Returnerar en "struct running_thread" som sedan kan skickas till
// "wait" för att få reda på resultatet. Systemanropen "exec" och "wait" ska
// fungera på samma sätt i Pintos sedan.
struct running_thread *exec(int param) {
  // Allokera en ny struktur för att hålla reda på tråden och initiera den.
  struct running_thread *data = malloc(sizeof(struct running_thread));
  data->param = param;

  // Skapa en ny tråd som kör "thread_main" och ge den tillgång till "data".
  thread_new(&thread_main, data);

  return data;
}

// Vänta på att en tråd som startades med "exec" blir klar och hämta resultatet
// från den. "wait" frigör också "data", så vi antar att "wait" bara anropas en
// gång för varje anrop till "exec".
int wait(struct running_thread *data) {
  // Hämta resultatet, frigör minnet och returnera resultatet.
  int result = data->result;
  free(data);
  return result;
}


/**
 * Testprogram.
 *
 * Koden nedanför ska du inte behöva ändra om implementationen ovan är korrekt.
 *
 * Det kan däremot vara en bra idé att modifera testprogrammet nedan för att
 * kunna testa din lösning. Tänk dock på att du inte ska behöva lägga någon
 * synkronisering i funktionerna nedan.
 */

// Funktion som gör de tunga beräkningarna. Denna vill vi köra parallellt i
// olika trådar.
int do_work(int param) {
  // Här sker tungt arbete...
  timer_msleep(param);

  // För enkelhets skull returnerar vi bara parametern i kvadrat.
  return param * param;
}

// Main-funktion. Startar två extra trådar som anropar "do_work". Kör även
// "do_work" i main-tråden.
int main(void) {
  struct running_thread *a = exec(10);
  struct running_thread *b = exec(100);

  int c = do_work(5);

  printf("Result for 'a': %d\n", wait(a));
  printf("Result for 'b': %d\n", wait(b));
  printf("Result for 'c': %d\n", c);

  return 0;
}
