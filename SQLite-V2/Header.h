#pragma once
#include <string>


using namespace std;

//TO DO: clasa de baza coloana cu metode virtuala verifica si executa si sters static de la metode
//TO DO: interfata serializabil cu 2 metode virtuale serializeaza si deserializeaza

class comanda_import
{
public:
	static bool verifica_sintaxa(char*);
	static bool verifica_fisier(char*);
	static void executa_comanda(char*);
};
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

enum class tip_coloane { integer, real, text, tip_nedefinit };

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
	//serializare / deserializare
	void serializare(ofstream&);

	void deserializare(ifstream&);

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

	void afiseaza_inregistrare(ofstream&, int);


	//alte supraincarcari de operatori
	bool operator!();

	explicit operator int();

	bool operator>=(inregistrare);

	char* operator[](int);
};

class repository
{
private:
	string nume_fisier;
public:

	repository();
	repository(char*);


	void adauga(inregistrare);
	void sterge(int, conditie);


	void actualizeaza(int, int, conditie, conditie);

	void afiseaza_toate(ofstream&, int);

	void afiseaza_toate(ofstream&, int, conditie, int);

	void afiseaza_partial(ofstream&, int, int[], int);

	void afiseaza_partial(ofstream&, int, int[], int, int, conditie);

	void serializeaza(ofstream&);

	void deserializeaza(ifstream&);

};

//clasa pt structura unei coloane
class coloana
{
private:
	char* nume_coloana;
	tip_coloane tip_coloana;
	char* valoare_implicita;
	int dimensiune_coloana;
public:
	void serializare(ofstream&);
	void deserializare(ifstream&);

	//constructori
	coloana();
	coloana(const char*, tip_coloane, const char*, int);

	//constructor de copiere
	coloana(const coloana&);

	//operator=
	coloana& operator=(const coloana&);

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
	int nr_coloane;
	coloana* coloane;
	//pentru inregistrari;
	repository repo;
	
public:
	//functii de serializare
	void serializare(ofstream&);
	void deserializare(ifstream&);

	//constructori
	tabela();
	tabela(const char*, int, coloana[]);

	//constructor de copiere
	tabela(const tabela&);

	//operator=
	tabela& operator=(const tabela&);

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
	explicit operator int();
	//tabela operator--(); // tabela fara ultima inregistrare
	//tabela operator--(int);
	//tabela operator-(int);
	//bool operator<(tabela);
};

//o singura baza de date cu mai multe tabele
class baza_de_date
{
private:
	static baza_de_date* instanta;

	const char* FISIER_CONFIGURATIE = "config.txt";
	const int INDEX_SELECT = 0;//primul rand din fisierul config
	const int INDEX_DISPLAY = 1;//al doilea rand din fisierul config
	const string NUME_FISIER_SELECT = "SELECT_";
	const string NUME_FISIER_DISPLAY = "DISPLAY_";
	int rapoarte_select;
	int rapoarte_display;


	int nr_tabele;
	//poate contine 10 tabele si fiecare maxim 20 de coloane
	tabela tabele[10];

	//constructor
	baza_de_date();
	void citeste_config();

public:
	static baza_de_date* get_instanta();

	//functii accesor
	int get_nr_tabele();
	tabela get_tabela(int);
	tabela get_tabela(const char*);
	void set_tabela(tabela, int);

	string get_nume_fisier_select();
	string get_nume_fisier_display();
	
	//alte metode
	void adauga_tabela(tabela);
	void sterge_tabela(const char*);
	void afiseaza_tabela(const char*,int);

	//metoda pentru comenzile dml;
	void insereaza_inregistrari(const char*, inregistrare);
	void sterge_inregistrari(const char*, conditie);
	void actualizeaza_inregistrari(const char*, conditie, conditie);
	void afiseaza_toate(const char*);
	void afiseaza_toate(const char*, conditie);
	void afiseaza_partial(const char*, string*, int);
	void afiseaza_partial(const char*, string*, int, conditie);

	//alte functii
	bool exista_tabela(string);
	void listeaza_tabele();
};

//clasa pentru: citire, formatare, analizare comanda
class procesor_comenzi
{
private:
	static void proceseaza_comanda(char*);
public:
	static void citeste_comanda_fisier(const char*);

	static void citeste_comanda_consola();

	static bool este_nume_valid(const char*);

	static void to_upper(char*);

	static void to_upper(char*, int);

	static char* copiere_sir(const char*);

	static bool este_comanda_incompleta(const char*);

	static tip_coloane identifica_tip_coloana(const char*);

	static tip_coloane identifica_tip_coloana_csv(const char*);

	static bool este_numar_pozitiv(const char*);

	static void trim(char*);

	static tip_coloane conversie(string);
};