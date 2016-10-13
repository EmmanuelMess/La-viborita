#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <vector>
#include <chrono>
#include <thread>
#ifdef __cplusplus__
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

using namespace std;

int const ANCHO = 120, ALTO = 27;//de '#'
int const FINAL_DIBUJABLE = ANCHO-3;
char const MARCO = '#';
char VACIO [ANCHO], LIMITES[ANCHO];
int const TIEMPO_ENTRE_REFRESCOS = 1000;
int intentos = 0;

void ClearScreen();

struct cuerpo {
	bool cabeza;
	int x, y;
	int xpost, ypost;
	struct cuerpo *anterior = NULL;

	cuerpo(bool c, int nx, int ny, cuerpo * ant) {
        cabeza = c;
	    x = nx;
	    y = ny;
	    xpost = x;
	    ypost = y;
        anterior = ant;
	}

	char getChar() {
		return cabeza? 'S':'s';
	}

	void tick() {
        if(cabeza) {
			if(xpost > FINAL_DIBUJABLE)
				xpost = -1;
            else if(xpost < 0)
                xpost = FINAL_DIBUJABLE;
			else if(ypost > ALTO-1)
				ypost = 0;
            else if(y <= 0)
                ypost = ALTO-1;
		}

		if(anterior != NULL) {
			anterior->xpost = x;
			anterior->ypost = y;
		}

        x = xpost;
        y = ypost;
	}

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}
};

struct punto{
    int x, y;

    punto(int nx, int ny) {
        x = nx;
        y = ny;
    }
};

struct comida {
    int x, y;

    comida(vector<punto> &puntos) {
        generar(puntos);
    }

    void generar(vector<punto> &puntos) {
        bool d = false;

        do {
            d = false;
            x = rand()%ANCHO;
            y = rand()%ALTO;

            for(int i = 0; i < puntos.size(); i++) {
                if(x == puntos[i].x && y == puntos[i].y)
                    d = true;
            }

        } while(d);

        puntos.clear();
    }
};

int impr1D(char a[]) {
	cout << MARCO;

	for(int i = 0; i < ANCHO-2; i++) {
		if(a[i] == '\0')
			cout << " ";
		else cout << a[i];
	}

	cout << MARCO;
	return 0;
}

int impr2D(char c[][ANCHO]) {
	for(int y = 0; y < ALTO; y++) {
		impr1D(c[y]);
	}
	return 0;
}

long getTiempo() {
    return (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch())).count();
}

int main() {//void a() {//
	cout << "Cargando..." << endl;
	int x = ANCHO-4, y = ALTO-3;
	srand(time(NULL));

	VACIO[0] = MARCO;
	for(int i = 1; i < ANCHO-1; i++) {
		VACIO[i] = ' ';
	}

	for(int i = 0; i < ANCHO; i++) {
		LIMITES[i] = MARCO;
	}

	cuerpo m1 = *(new cuerpo(false, ANCHO/2-1, ALTO/2, NULL));
	cuerpo m = *(new cuerpo(false, ANCHO/2, ALTO/2, &m1));
	cuerpo s = *(new cuerpo(true, ANCHO/2+1, ALTO/2, &m));

    cuerpo *ult = &m1;

	vector <punto> puntos;
	comida COMIDA = *(new comida(puntos));

	char a[ALTO][ANCHO];

	int ultEstado = 3;
    long tiempo;

	while (true){
        tiempo = getTiempo();
		ClearScreen();

		if(intentos != 0)
            cout << intentos << " intento" << (intentos > 1? "s":"") << " y contando..." << endl;
        else cout << "La viborita en C++;" << endl;

		/*Debug {
			cout << "s->a(" << s.anterior->getX() << ", " << s.anterior->getY() << ");";
			cout << "s(" << s.getX() << ", " << s.getY() << ")" << endl;
			int i = 0;
			for(cuerpo *mem = &s;mem != NULL; mem = mem->anterior) {
				cout << i++;
				cout << "mem(" << mem->getX() << ", " << mem->getY() << ")" << endl;
			}
		}*/

		cout << LIMITES;
		for(auto &e : a) {
			for(char &j : e) {
				j = '\0';
			}
		}

        if((GetAsyncKeyState(VK_UP) || ultEstado == 0) && ultEstado != 1) {
            s.ypost -= 1;
            ultEstado = 0;
		}
        if((GetAsyncKeyState(VK_DOWN) || ultEstado == 1) && ultEstado != 0) {
            s.ypost += 1;
            ultEstado = 1;
        }
        if((GetAsyncKeyState(VK_LEFT) || ultEstado == 2) && ultEstado != 3) {
            s.xpost -= 1;
            if(ultEstado != 2)
                s.ypost = s.getY();
            ultEstado = 2;
        }
        if((GetAsyncKeyState(VK_RIGHT) || ultEstado == 3) && ultEstado != 2) {
            s.xpost += 1;
            if(ultEstado != 3)
                s.ypost = s.getY();
            ultEstado = 3;
        }

		for(cuerpo *mem = &s;mem != NULL; mem = mem->anterior) {
            mem->tick();
            if(mem != &s && mem->getX() == s.getX() && mem->getY() == s.getY()) {
                intentos++;
                main();
            }
			a[mem->getY()][mem->getX()] = mem->getChar();
		}

		if(s.getX() == COMIDA.x && s.getY() == COMIDA.y) {
            ult->anterior = new cuerpo(false, ult->getX(), ult->getY(), NULL);
            ult = ult->anterior;

            for(cuerpo *mem = &s;mem != NULL; mem = mem->anterior) {
                punto m = *(new punto(mem->getX(), mem->getY()));
                puntos.push_back(m);
            }

            if((ANCHO-2)*(ALTO) == puntos.size())
                break;

            COMIDA.generar(puntos);
		}

		a[COMIDA.y][COMIDA.x] = 'c';

		impr2D(a);
		cout << LIMITES;

		int deltaT = TIEMPO_ENTRE_REFRESCOS - (getTiempo()-tiempo);

		//this_thread::sleep_for(chrono::milliseconds(deltaT >= 0? deltaT:0));
		Sleep(deltaT >= 0? deltaT:0);
	}

	cout << "Ganaste!" << endl << "(Como carajo hiciste?)";
}

void ClearScreen() {
    if (system("CLS")) system("clear");
}
