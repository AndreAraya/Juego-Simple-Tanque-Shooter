//****************Librerías*****************

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h> 
#include <allegro5/allegro_ttf.h> 
#pragma warning (disable : 4996)

#include <iostream>
#include <fstream>

#include "BalaTanque.h"
#include "tanqueEnemigo.h"


using namespace std;

//******************************************


//**************Variables*******************

FILE* documento;

int const maxFilas = 20; //Medimos el eje x
int const maxColumnas = 31; //Medimos el eje y 

int direccionAzul = 1;
int ejeXAzul = 30 * 15;
int ejeYAzul = 30 * 17;
int ejeXAnterior = 0;
int ejeYAnterior = 0;
bool ciclo = true;
bool sentido = 0;
int movimiento = 30;
int disparo = -1;
int cantidadEnemigos = -1;

int puntaje = 0;
int balasDisparadas = 0;
int tanquesDestruidos = 0;
int bloquesDestruidos = 0;
char texto[];


ALLEGRO_DISPLAY* display = NULL;


ALLEGRO_BITMAP* buffer = NULL;
ALLEGRO_BITMAP* ladrillo = NULL;
ALLEGRO_BITMAP* tanqueAzul = NULL;
ALLEGRO_BITMAP* tanqueAzulBuffer = NULL;
ALLEGRO_BITMAP* tanqueEnemigo = NULL;
ALLEGRO_BITMAP* tanqueEnemigoBuffer = NULL;
ALLEGRO_BITMAP* bala = NULL;

ALLEGRO_KEYBOARD_STATE keyState;

ALLEGRO_SAMPLE* musicaJuego;
ALLEGRO_SAMPLE* disparoTanque;
ALLEGRO_SAMPLE* ladrilloRoto;
ALLEGRO_SAMPLE* explosionTanque;

ALLEGRO_SAMPLE_INSTANCE* instanciaSonido;

ALLEGRO_FONT* fuente;

BalaTanque* balasTanque[10];
TanqueEnemigo* tanquesEnemigos[10];


const ALLEGRO_COLOR transparente = al_map_rgb(0, 0, 0);


char mapa[maxFilas][maxColumnas] = {
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
	"X   X     XXXXXXXXXX     X   X",
	"X X XXXX  XXXXXXXXXX  XXXX X X",
	"X          X      X          X",
	"X          X XXXX X          X",
	"XXXXXXXXXXXX XXXX XXXXXXXXXXXX",
	"X                            X",
	"X XXX    XXXXXXXXXXXX    XXX X",
	"X XXX                    XXX X",
	"X   XXXXXXXXXXXXXXXXXXXXXX   X",
	"X   X                    X   X",
	"XXXXXXXXXXXXXXXXXXXX   XXXX  X",
	"XXXXXXXXXXXXXXXXXXXX   XXXX  X",
	"X               XXX          X",
	"X               XXX          X",
	"XXXX  XXXXXXXXXXXXXXXXXX  XXXX",
	"XXXX  XXXXXXXXXXXXXXXXXX  XXXX",
	"X  X       XXXX           X  X",
	"X  X       XXXX           X  X",
	"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
};

void dibujarMapa() {
	int fila, columna;

	for (fila = 0; fila < maxFilas; fila++) {
		for (columna = 0; columna < maxColumnas; columna++) {
			if (mapa[fila][columna] == 'X') {
				al_set_target_bitmap(buffer);
				al_draw_bitmap(ladrillo, columna * 30, fila * 30, NULL);

			}
		}
	}
}

void pantalla() {
	al_set_target_bitmap(al_get_backbuffer(display));
	al_draw_bitmap(buffer, 0, 0, 0);
	al_flip_display();
	al_clear_to_color(transparente);
}

void reproducirMusicaJuego() {
	instanciaSonido = al_create_sample_instance(musicaJuego);
	al_set_sample_instance_playmode(instanciaSonido, ALLEGRO_PLAYMODE_LOOP);
	al_set_sample_instance_gain(instanciaSonido, 0.4);
	al_attach_sample_instance_to_mixer(instanciaSonido, al_get_default_mixer());
	al_play_sample_instance(instanciaSonido);
	al_rest(2);
	al_set_sample_instance_gain(instanciaSonido, 0.2);
}

void reproducirDisparoTanque() {
	instanciaSonido = al_create_sample_instance(disparoTanque);
	al_set_sample_instance_playmode(instanciaSonido, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(instanciaSonido, 0.4);
	al_attach_sample_instance_to_mixer(instanciaSonido, al_get_default_mixer());
	al_play_sample_instance(instanciaSonido);
}

void reproducirLadrilloRoto() {
	instanciaSonido = al_create_sample_instance(ladrilloRoto);
	al_set_sample_instance_playmode(instanciaSonido, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(instanciaSonido, 0.4);
	al_attach_sample_instance_to_mixer(instanciaSonido, al_get_default_mixer());
	al_play_sample_instance(instanciaSonido);
}

void reproducirExplosionTanque() {
	instanciaSonido = al_create_sample_instance(explosionTanque);
	al_set_sample_instance_playmode(instanciaSonido, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(instanciaSonido, 0.2);
	al_attach_sample_instance_to_mixer(instanciaSonido, al_get_default_mixer());
	al_play_sample_instance(instanciaSonido);
}

void moverAzul() {
	if (direccionAzul == 0) {
		if (mapa[ejeYAzul / 30][(ejeXAzul / 30) - 1] == 'X' || ejeXAzul == 0) direccionAzul = 4;

		else ejeXAzul -= 30;
	}

	else if (direccionAzul == 1) {
		if (mapa[ejeYAzul / 30][(ejeXAzul / 30) + 1] == 'X' || ejeXAzul == 870) direccionAzul = 4;

		else ejeXAzul += 30;
	}

	else if (direccionAzul == 2) {
		if (mapa[(ejeYAzul / 30) - 1][ejeXAzul / 30] == 'X' || ejeYAzul == 0) direccionAzul = 4;


		else ejeYAzul -= 30;
	}

	else if (direccionAzul == 3) {
		if (mapa[(ejeYAzul / 30) + 1][ejeXAzul / 30] == 'X' || ejeYAzul == 570) direccionAzul = 4;

		else ejeYAzul += 30;
	}
}

void dibujarAzul() {
	al_set_target_bitmap(tanqueAzulBuffer);
	al_draw_bitmap_region(tanqueAzul, direccionAzul * 32, 0, 32, 32, 0, 0, 0);
	al_set_target_bitmap(buffer);
	al_draw_bitmap(tanqueAzulBuffer, ejeXAzul, ejeYAzul, 0);
	pantalla();

}

void dibujarPuntuacion() {
	char puntajeCadena[10];
	sprintf_s(puntajeCadena, "%i", puntaje);
	al_set_target_bitmap(buffer);
	al_draw_text(fuente, al_map_rgb(255, 255, 255), 420, 0, ALLEGRO_ALIGN_CENTRE, "Puntaje:");
	al_draw_text(fuente, al_map_rgb(255, 255, 255), 500, 0, ALLEGRO_ALIGN_CENTRE, puntajeCadena);
	pantalla();
}

int bandera1 = 0;

void finjuego() {
	if (bandera1 == 0) {
		reproducirExplosionTanque();
		bandera1 = 1;
	}
	ciclo = false;
	al_set_target_bitmap(buffer);
	al_draw_text(fuente, al_map_rgb(100, 0, 0), 450, 210, ALLEGRO_ALIGN_CENTRE, "Buen trabajo soldado!");
	al_draw_text(fuente, al_map_rgb(100, 0, 0), 450, 270, ALLEGRO_ALIGN_CENTRE, "Gracias por jugar");
	al_draw_text(fuente, al_map_rgb(100, 0, 0), 450, 350, ALLEGRO_ALIGN_CENTRE, "Presiona Esc para salir");
	dibujarPuntuacion();
	pantalla();
}

void moverEnemigo() {
	for (int i = 0; i < 10; i++) {

		if (tanquesEnemigos[i]!= NULL) {

			if (tanquesEnemigos[i]->direccion == 0) {
				if (tanquesEnemigos[i]->y == ejeYAzul && tanquesEnemigos[i]->x == ejeXAzul) {
					finjuego();
					return;
				}
				if (mapa[tanquesEnemigos[i]->y / 30][(tanquesEnemigos[i]->x / 30) - 1] == 'X' || tanquesEnemigos[i]->x == 0) tanquesEnemigos[i]->direccion = 2;

				else tanquesEnemigos[i]->x -= 30;
			}

			else if (tanquesEnemigos[i]->direccion == 1) {
				if (tanquesEnemigos[i]->y == ejeYAzul && tanquesEnemigos[i]->x == ejeXAzul) {
					finjuego();
					return;
				}
				if (mapa[tanquesEnemigos[i]->y / 30][(tanquesEnemigos[i]->x / 30) + 1] == 'X' || tanquesEnemigos[i]->x == 870) tanquesEnemigos[i]->direccion = 3;

				else tanquesEnemigos[i]->x += 30;
			}

			else if (tanquesEnemigos[i]->direccion == 2) {
				if (tanquesEnemigos[i]->y == ejeYAzul && tanquesEnemigos[i]->x == ejeXAzul) {
					finjuego();
					return;
				}
				if (mapa[(tanquesEnemigos[i]->y / 30) - 1][(tanquesEnemigos[i]->x / 30)] == 'X' || tanquesEnemigos[i]->y == 0) tanquesEnemigos[i]->direccion = 1;

				else tanquesEnemigos[i]->y -= 30;
			}

			else if (tanquesEnemigos[i]->direccion == 3) {
				if (tanquesEnemigos[i]->y == ejeYAzul && tanquesEnemigos[i]->x == ejeXAzul) {
					finjuego();
					return;
				}
				if (mapa[(tanquesEnemigos[i]->y / 30) + 1][(tanquesEnemigos[i]->x / 30)] == 'X' || tanquesEnemigos[i]->y == 570) tanquesEnemigos[i]->direccion = 0;

				else tanquesEnemigos[i]->y += 30;
			}




		}
	}
}

void dibujarTextoInicial() {;
	al_set_target_bitmap(buffer);
	al_draw_text(fuente, al_map_rgb(100, 0, 0), 450, 210, ALLEGRO_ALIGN_CENTRE, "Estas en la base enemiga.");
	al_draw_text(fuente, al_map_rgb(100, 0, 0), 450, 270, ALLEGRO_ALIGN_CENTRE, "Destruye sus tanques y sus instalaciones!");
	pantalla();
}

void dibujarEnemigo() {
	for (int i = 0; i < 10; i++) {
		if (tanquesEnemigos[i] != NULL) {
			al_set_target_bitmap(tanqueEnemigoBuffer);
			al_draw_bitmap_region(tanqueEnemigo, tanquesEnemigos[i]->direccion * 32, 0, 32, 32, 0, 0, 0);
			al_set_target_bitmap(buffer);
			al_draw_bitmap(tanqueEnemigoBuffer, tanquesEnemigos[i]->x, tanquesEnemigos[i]->y, 0);
			
		}
	}
	pantalla();
}

void iniciartanquesEnemigos() {
	int x;
	int y;
	int randx;
	int randy;
	int DESDEx = 1;
	int HASTAx = 30;
	int DESDEy = 1;
	int HASTAy = 19;
	for (int i = 0; i < 10; i++) {
		randx = rand() % (HASTAx - DESDEx + 1) + DESDEx;
		randy = rand() % (HASTAy - DESDEy + 1) + DESDEy;
		x = 30 * randx ;
		y = 30 * randy;
		if ((x < ejeXAzul - 60 || x > ejeXAzul + 60) || (y < ejeYAzul - 60 || y > ejeYAzul + 60)) {
			if (mapa[y / 30][(x / 30)] != 'X') {
				tanquesEnemigos[i] = new TanqueEnemigo(x, y, 1);
				cantidadEnemigos++;
			}
		}
		
	}

}

void desactivartanqueEnemigo(int posicion) {

	tanquesEnemigos[posicion] = NULL;
	cantidadEnemigos--;
	tanquesDestruidos++;
}

void iniciarBalasTanque() {
	for (int i = 0; i < 10; i++) {
		balasTanque[i] = NULL;
	}
}

void dibujarBalasTanque() {
	for (int i = 0; i < 10; i++) {

		if (balasTanque[i] != NULL) {

			al_set_target_bitmap(buffer);
			al_draw_bitmap(bala, balasTanque[i]->x, balasTanque[i]->y, NULL);
			balasDisparadas++;

			
			pantalla();

		}
	}
}

void dispararTanque(int direccion) {
	if (direccion == 0 || direccion == 1 || direccion == 2 || direccion == 3) {

		for (int i = 0; i < 10; i++) {

			if (balasTanque[i] == NULL) {

				if (direccionAzul == 0) {
					balasTanque[i] = new BalaTanque(ejeXAzul-30, ejeYAzul, direccion);
				}

				else if (direccionAzul == 1) {
					balasTanque[i] = new BalaTanque(ejeXAzul+30, ejeYAzul, direccion);
				}

				else if (direccionAzul == 2) {
					balasTanque[i] = new BalaTanque(ejeXAzul, ejeYAzul-30, direccion);
				}

				else if (direccionAzul == 3) {
					balasTanque[i] = new BalaTanque(ejeXAzul, ejeYAzul+30, direccion);
				}
				
				reproducirDisparoTanque();
				break;

			}
		}
	}
}

void desactivarBalaTanque(int posicion) {

	balasTanque[posicion] = NULL;
}

void moverBalaTanque(int movimiento) {
	for (int i = 0; i < 10; i++) {

		if (balasTanque[i] != NULL) {

			if (balasTanque[i]->orientacion == 3) {
				if (mapa[(balasTanque[i]->y / 30)][(balasTanque[i]->x / 30)] == 'X') {
					desactivarBalaTanque(i);
					break;
				}
				for (int j = 0; j < 10; j++) {
					if (tanquesEnemigos[j] != NULL) {
						if ((balasTanque[i]->y) == tanquesEnemigos[j]->y && balasTanque[i]->x == tanquesEnemigos[j]->x
							|| (balasTanque[i]->y + 30) == tanquesEnemigos[j]->y && balasTanque[i]->x == tanquesEnemigos[j]->x)
						{
							desactivarBalaTanque(i);
							desactivartanqueEnemigo(j);
							reproducirExplosionTanque();
							puntaje = puntaje + 100;
							return;
						}
					}
				}

				if (mapa[(balasTanque[i]->y / 30) + 1][(balasTanque[i]->x / 30)] == 'X') {
					mapa[(balasTanque[i]->y / 30) + 1][(balasTanque[i]->x / 30)] = ' ';
					reproducirLadrilloRoto();
					desactivarBalaTanque(i);
					bloquesDestruidos++;
					puntaje = puntaje + 10;
					

				}
				else if (balasTanque[i]->y == 570) {
					desactivarBalaTanque(i);
				}
				else balasTanque[i]->y += movimiento;
			}

			else if (balasTanque[i]->orientacion == 2) {
				if (mapa[(balasTanque[i]->y / 30)][(balasTanque[i]->x / 30)] == 'X') {
					desactivarBalaTanque(i);
					break;
				}
				for (int j = 0; j < 10; j++) {
					if (tanquesEnemigos[j] != NULL) {
						if ((balasTanque[i]->y) == tanquesEnemigos[j]->y && balasTanque[i]->x == tanquesEnemigos[j]->x
							|| (balasTanque[i]->y -30) == tanquesEnemigos[j]->y && balasTanque[i]->x == tanquesEnemigos[j]->x) {
							desactivarBalaTanque(i);
							desactivartanqueEnemigo(j);
							reproducirExplosionTanque();
							puntaje = puntaje + 100;
							return;
						}
					}
				}
				if (mapa[(balasTanque[i]->y / 30) - 1][(balasTanque[i]->x / 30)] == 'X') {
					mapa[(balasTanque[i]->y / 30) - 1][(balasTanque[i]->x / 30)] = ' ';
					reproducirLadrilloRoto();
					desactivarBalaTanque(i);
					puntaje = puntaje + 10;
					bloquesDestruidos++;
				}
				else if (balasTanque[i]->y == 0) {
					desactivarBalaTanque(i);
				}
				else balasTanque[i]->y -= movimiento;
			}

			else if (balasTanque[i]->orientacion == 1) {
				if (mapa[(balasTanque[i]->y / 30)][(balasTanque[i]->x / 30)] == 'X') {
					desactivarBalaTanque(i);
					break;
				}
				for (int j = 0; j < 10; j++) {
					if (tanquesEnemigos[j] != NULL) {
						if ((balasTanque[i]->y) == tanquesEnemigos[j]->y && (balasTanque[i]->x) == tanquesEnemigos[j]->x
							|| (balasTanque[i]->y) == tanquesEnemigos[j]->y && (balasTanque[i]->x+30) == tanquesEnemigos[j]->x) {
							desactivarBalaTanque(i);
							desactivartanqueEnemigo(j);
							reproducirExplosionTanque();
							puntaje = puntaje + 100;
							return;
						}
					}
				}
				if (mapa[balasTanque[i]->y / 30][(balasTanque[i]->x / 30) + 1] == 'X') {
					mapa[(balasTanque[i]->y / 30)][(balasTanque[i]->x / 30) + 1] = ' ';
					desactivarBalaTanque(i);
					reproducirLadrilloRoto();
					puntaje = puntaje + 10;
					bloquesDestruidos++;
				}
				else if (balasTanque[i]->x == 870) {
					desactivarBalaTanque(i);
				}
				else balasTanque[i]->x += movimiento;
			}

			else if (balasTanque[i]->orientacion == 0) {
				if (mapa[(balasTanque[i]->y / 30)][(balasTanque[i]->x / 30)] == 'X') {
					desactivarBalaTanque(i);
					break;
				}
				for (int j = 0; j < 10; j++) {
					if (tanquesEnemigos[j] != NULL) {
						if ((balasTanque[i]->y) == tanquesEnemigos[j]->y && (balasTanque[i]->x) == tanquesEnemigos[j]->x
							|| (balasTanque[i]->y) == tanquesEnemigos[j]->y && (balasTanque[i]->x - 30) == tanquesEnemigos[j]->x) {
							desactivarBalaTanque(i);
							desactivartanqueEnemigo(j);
							reproducirExplosionTanque();
							puntaje = puntaje + 100;
							return;
						}
					}
				}
				if (mapa[balasTanque[i]->y / 30][(balasTanque[i]->x / 30) - 1] == 'X') {
					mapa[(balasTanque[i]->y / 30)][(balasTanque[i]->x / 30) - 1] = ' ';
					desactivarBalaTanque(i);
					reproducirLadrilloRoto();
					puntaje = puntaje + 10;
					bloquesDestruidos++;
				}
				else if (balasTanque[i]->x == 0) {
					desactivarBalaTanque(i);
				}
				else balasTanque[i]->x -= movimiento;
			}
		}
	}
}

void masEnemigos() {
	cout << cantidadEnemigos << endl;
	if (cantidadEnemigos < 5) {
		int x;
		int y;
		int DESDEx = 1;
		int HASTAx = 30;
		int DESDEy = 1;
		int HASTAy = 19;
		for (int i = 0; i < 10; i++) {
			if (tanquesEnemigos[i] == NULL) {
				x = 30 * (rand() % (HASTAx - DESDEx + 1) + DESDEx);
				y = 30 * (rand() % (HASTAy - DESDEy + 1) + DESDEy);
				if ((x < ejeXAzul - 60 || x > ejeXAzul + 60) || (y < ejeYAzul - 60 || y > ejeYAzul + 60)) {
					if (mapa[y / 30][(x / 30)] != 'X') {
						tanquesEnemigos[i] = new TanqueEnemigo(x, y, 1);
						cantidadEnemigos++;
					}	
				}
			}	
		}
	}
}

int main(int argc, char** argv) {

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	al_init_acodec_addon();
	al_init_image_addon();

	al_init_primitives_addon();
	al_install_keyboard();

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1000);

	al_init_font_addon();
	al_init_ttf_addon();

	fuente = al_load_font("Atarian.ttf", 36, NULL);

	display = al_create_display(900, 600);
	buffer = al_create_bitmap(900, 600);

	tanqueEnemigoBuffer = al_create_bitmap(33, 33);
	tanqueAzulBuffer = al_create_bitmap(33, 33);

	ladrillo = al_load_bitmap("brick.jpg");
	tanqueAzul = al_load_bitmap("tanqueAzul1.png");
	tanqueEnemigo = al_load_bitmap("tanqueRojo1.png");
	bala = al_load_bitmap("bala.png");

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1000);

	disparoTanque = al_load_sample("disparoTanque.ogg");
	ladrilloRoto = al_load_sample("brickCrumbling.ogg");
	explosionTanque = al_load_sample("explosionTanque.ogg");
	musicaJuego = al_load_sample("musicaJuego.ogg");


	ALLEGRO_KEYBOARD_STATE estadoTeclado;

	ALLEGRO_TIMER* primertimer = al_create_timer(0.1 / 60);
	ALLEGRO_TIMER* segundotimer = al_create_timer(0.1 / 60);

	ALLEGRO_EVENT_QUEUE* evento_cola = al_create_event_queue();

	al_register_event_source(evento_cola, al_get_keyboard_event_source());
	al_register_event_source(evento_cola, al_get_timer_event_source(primertimer));
	al_register_event_source(evento_cola, al_get_timer_event_source(segundotimer));

	al_start_timer(primertimer);
	al_start_timer(segundotimer);

	dibujarMapa();
	dibujarAzul();
	dibujarPuntuacion();
	dibujarTextoInicial();
	al_rest(2);
	iniciarBalasTanque();
	iniciartanquesEnemigos();
	reproducirMusicaJuego();

	while (ciclo) {

		ALLEGRO_EVENT eventos;
		al_wait_for_event(evento_cola, &eventos);

		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			al_get_keyboard_state(&estadoTeclado);

			while (!al_key_down(&estadoTeclado, ALLEGRO_KEY_ESCAPE)) {

				al_get_keyboard_state(&estadoTeclado);

				al_set_target_bitmap(buffer); 
				al_clear_to_color(transparente); 

				ejeXAnterior = ejeXAzul;
				ejeYAnterior = ejeYAzul;

				dibujarMapa();
				pantalla();

				if (al_key_down(&estadoTeclado, ALLEGRO_KEY_LEFT)) {
					direccionAzul = 0;
					moverAzul();
				}

				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_RIGHT)) {
					direccionAzul = 1;
					moverAzul();
				}

				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_UP)) {
					direccionAzul = 2;
					moverAzul();
				}

				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_DOWN)) {
					direccionAzul = 3;
					moverAzul();
				}

				else if (al_key_down(&estadoTeclado, ALLEGRO_KEY_SPACE)) {
					dispararTanque(direccionAzul);
				}


				if (eventos.type == ALLEGRO_EVENT_TIMER) {
					if (eventos.timer.source == primertimer) {
						dibujarAzul();
						dibujarBalasTanque();
						moverBalaTanque(movimiento);
						dibujarEnemigo();
						moverEnemigo();
						dibujarPuntuacion();

						
						
					}
				}
				masEnemigos();
				pantalla();
				al_rest(0.15);
			}
		}
		ciclo = false;
		documento = fopen("Estadisticas.txt", "a");
		fprintf(documento, "%s \n", "Puntaje:");
		fprintf(documento, "%i \n", puntaje);
		fprintf(documento, "%s \n", "Tanques Destruidos:");
		fprintf(documento, "%i \n", tanquesDestruidos);
		fprintf(documento, "%s \n", "Bloques Destruidos:");
		fprintf(documento, "%i \n", bloquesDestruidos);
		fprintf(documento, "%s \n", "");
		fclose(documento);

	}

	//Se deben aplicar estos destroy para liberar la memoria que se utiliza en los bitmap, el display y el timer
	
	al_destroy_bitmap(ladrillo);
	al_destroy_bitmap(buffer);
	al_destroy_bitmap(tanqueAzul);
	al_destroy_bitmap(tanqueAzulBuffer);
	al_destroy_bitmap(tanqueEnemigo);
	al_destroy_bitmap(tanqueEnemigoBuffer);
	al_destroy_bitmap(bala);

	al_destroy_display(display);

	al_destroy_event_queue(evento_cola);

	al_destroy_sample(musicaJuego);
	al_destroy_sample(disparoTanque);
	al_destroy_sample(ladrilloRoto);
	al_destroy_sample(explosionTanque);

	al_destroy_sample_instance(instanciaSonido);

	al_destroy_font(fuente);

	al_destroy_timer(primertimer);
	al_destroy_timer(segundotimer);

	return 0;
}


//************************************************************