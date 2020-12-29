#pragma once
#include <string.h>
#include <string>

using namespace std;

//TO DO: clasa de baza coloana cu metode virtuala verifica si executa
//TO DO: de modificat tipul de coloana din real in float

class comanda_create
{
public:
	static bool verifica_sintaxa(char*);
	static void executa_comanda(char*);
};

class comanda_drop
{
public:
	static bool verifica_sintaxa(char*);
	static void executa_comanda(char*);
};

class comanda_display
{
public:
	static bool verifica_sintaxa(char*);
	static void executa_comanda(char*);
};

class comanda_select
{
public:
	static bool verifica_sintaxa(char*);
	static void executa_comanda(char*);
};

class comanda_delete
{
public:
	static bool verifica_sintaxa(char*);
	static void executa_comanda(char*);
};

class comanda_update
{
public:
	static bool verifica_sintaxa(char*);
	static void executa_comanda(char*);
};

class comanda_insert
{
public:
	static bool verifica_sintaxa(char*);
	static void executa_comanda(char*);
};

enum class tip_coloane {integer, real, text, tip_nedefinit};

//clasa pt o pereche nume - valoare
class conditie
{
private:
	char* nume_coloana;
	char* valoare_coloana;
public:
	//constructori
	conditie();
	conditie(const char*, const char*);

	//constructor de copire
	conditie(const conditie&);
	//operator=
	conditie operator=(conditie);
	//destructor
	~conditie();
	//functii de citire si afisare consola
	friend ostream& operator<<(ostream&, conditie);
	friend istream& operator>>(istream&, conditie&);

	//functii accesor
	string get_nume_coloana();
	void set_nume_coloana(const char*);
	string get_valoare_coloana();
	void set_valoare_coloana(const char*);

};

//clasa pt un rand din tabela
class inregistrare
{
private:
	int nr_campuri;
	char** valori_campuri;
public:
	//constructori
	inregistrare();
	inregistrare(int, char[100][100]);
	
	//constructor de copiere
	inregistrare(const inregistrare&);
	
	//operator=
	inregistrare operator=(inregistrare);
	
	//destructor
	~inregistrare();

	//functii de citire si afisare
	friend istream& operator>>(istream&, inregistrare&);
	friend ostream& operator<<(ostream&, inregistrare);

	//functii accesor -- nu are sens un setter pentru numarul de campuri
	int get_nr_campuri();

	void set_valori(char**, int);
	char** get_valori();
	void set_valoare(const char*, int);
	string get_valoare(int index);
	
	void afiseaza_inregistrare(int dimensiune);
	

	//alte supraincarcari de operatori
	bool operator!();

	explicit operator int();

	bool operator>=(inregistrare);

	char* operator[](int);
};

//clasa pt strctura unei coloane
class coloana
{
private:
	char* nume_coloana;
	tip_coloane tip_coloana;
	char* valoare_implicita;
	int dimensiune_coloana;
public:
	//constructori
	coloana();
	coloana(const char*, tip_coloane,const char*, int);

	//constructor de copiere
	coloana(const coloana&);

	//operator=
	coloana operator=(coloana);

	//destructor
	~coloana();

	//functii de citire si afisare consola
	friend ostream& operator<< (ostream&, coloana);
	friend istream& operator>>(istream&, coloana&);

	//functii accesor
	string get_nume_coloana();
	void set_nume_coloana(const char*);

	string get_tip_coloana();
	void set_tip_coloana(tip_coloane);

	string get_valoare_implicita();
	void set_valoare_implicita(const char*);

	int get_dimensiune_coloana();
	void set_dimensiune_coloana(int);
};

class tabela
{
private:
	char* nume_tabela;
	
	//de inlocuit cu vector STL
	coloana coloane[20];
	int nr_coloane;
	
	//de inlocuit cu vector STL
	inregistrare inregistrari[100];
	int nr_inregistrari;
public:
	//constructori
	tabela();
	tabela(const char*, int, coloana[]);
	tabela(const char*, int, coloana[], int, inregistrare[]);

	//constructor de copiere
	tabela(const tabela&);

	//operator=
	tabela operator=(tabela);

	//destructor
	~tabela();

	//operatori de citire si afisare consola
	friend ostream& operator<<(ostream&, tabela);
	friend istream& operator>>(istream&, tabela&);

	//functii accesor
	string get_nume_tabela();
	void set_nume_tabela(const char*);

	int get_nr_coloane();
	//void set_nr_coloane(); - nu are sens
	
	coloana get_coloana(int);
	coloana get_coloana(const char*);
	void set_coloana(coloana, int);

	int get_index_coloana(const char*);

	int get_nr_inregistrari();
	//void set_nr_inregistrari() - nu are sens

	inregistrare get_inregistrare(int);
	void set_inregistrare(inregistrare, int);


	//alte functii
	void afiseaza_structura_tabela(); //pentru comanda display
	void adauga_inregistrare(inregistrare); //pentru comanda insert
	void sterge_inregistrare(conditie);	//pentru comanda delete
	void actualizeaza_inregistrare(conditie, conditie); //pentru comanda update
	bool exista_coloana(string);
	void afiseaza_toate();
	void afiseaza_toate(conditie);
	void afiseaza_partial(string*, int);
	void afiseaza_partial(string*, int, conditie);

	//alte supraincarcari de operatori
	bool operator!();

	tabela operator--(); // tabela fara ultima inregistrare

	tabela operator--(int);

	tabela operator-(int);

	explicit operator int();

	bool operator<(tabela);
};

//o singura baza de date cu mai multe tabele
class baza_de_date
{
private:
	//int* nr_inregistrari_per_tabela; - sa existe un vector de int alocat dinamic

	static baza_de_date* instanta;
	const string nume_bd;
	int nr_tabele;
	//poate contine 10 tabele si fiecare maxim 20 de coloane si 100 de inregistrari
	tabela tabele[10];
	static int nr_instante;
	baza_de_date(string nume) : nume_bd(nume)
	{
		nr_tabele = 0;
	}
	//singleton, nu cred ca e nevoie de constructor de copiere, operator= sau destructor ?????
	//nu au sens alti operatori supraincarcati
	
public:
	static baza_de_date* getInstance(string);
	
	//functii accesor
	string get_nume_bd();
	//void set_nume_tabela nu are sens pentru ca e constant
	int get_nr_tabele();
	//void set_nr_tabele nu are sens
	tabela get_tabela(int);
	tabela get_tabela(const char*);
	void set_tabela(tabela,int);
	

	//alte metode
	void adauga_tabela(tabela);
	void sterge_tabela(const char*);
	void afiseaza_tabela(const char*);

	//metoda pentru comenzile dml;
	void insereaza_inregistrari(const char*,inregistrare);
	void sterge_inregistrari(const char*, conditie);
	void actualizeaza_inregistrari(const char*, conditie, conditie);
	void afiseaza_toate(const char*);
	void afiseaza_toate(const char*, conditie);
	void afiseaza_partial(const char*, string*,int);
	void afiseaza_partial(const char* ,string*,int,conditie);
	
															 
	//metoda de verificare cmd drop si delete
	void listeaza_tabele();
	//alte functii
	bool exista_tabela(string);
};

//clasa pentru: citire, formatare, analizare comanda
class procesor_comenzi
{
private:
	static baza_de_date* bd;
public:
	static void proceseaza_comanda();

	static void set_bd(baza_de_date* bd)
	{
		procesor_comenzi::bd = bd;
	}

	static baza_de_date* get_bd()
	{
		return bd;
	}

	static bool este_nume_valid(const char*);

	static void to_upper(char*);

	static void to_upper(char*, int);

	static char* copiere_sir(const char*);

	static bool este_comanda_incompleta(const char*);

	static tip_coloane identifica_tip_coloana(const char*);

	static bool este_numar_pozitiv(const char*);

	static void trim(char*);

	static tip_coloane conversie(string);
};
