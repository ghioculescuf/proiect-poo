#pragma once
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>

using namespace std;

//INTERFETE
class verificabil
{
public:
	virtual bool verifica_sintaxa() = 0;
};

class executabil
{
public:
	virtual void executa_comanda() = 0;
};

class serializabil
{
public:
	virtual void serializare(ofstream&) = 0;
	virtual void deserializare(ifstream&) = 0;
};

//CLASA ABSTRACTA
class comanda : public verificabil, public executabil
{
protected:
	char* sir_verificare;
	char* sir_executie;
public:
	comanda();
	comanda(const char* c);

	comanda(const comanda&) = delete;
	comanda& operator= (const comanda&) = delete;

	virtual ~comanda();

};


class comanda_create : public comanda
{
public:
	comanda_create(const char* c) : comanda(c) {}
	bool verifica_sintaxa() override;
	void executa_comanda() override;

	comanda_create(const comanda_create&) = delete;
	comanda_create& operator= (const comanda_create&) = delete;
};

class comanda_drop : public comanda
{
public:
	comanda_drop(const char* c) : comanda(c) {}
	bool verifica_sintaxa() override;
	void executa_comanda() override;

	comanda_drop(const comanda_drop&) = delete;
	comanda_drop& operator= (const comanda_drop&) = delete;
};

class comanda_display : public comanda
{
public:
	comanda_display(const char* c) : comanda(c) {}
	bool verifica_sintaxa() override;
	void executa_comanda() override;

	comanda_display(const comanda_display&) = delete;
	comanda_display& operator= (const comanda_display&) = delete;
};

class comanda_insert : public comanda
{
public:
	comanda_insert(const char* c) : comanda(c) {}
	bool verifica_sintaxa() override;
	void executa_comanda() override;

	comanda_insert(const comanda_insert&) = delete;
	comanda_insert& operator= (const comanda_insert&) = delete;
};

class comanda_delete : public comanda
{
public:
	comanda_delete(const char* c) : comanda(c) {}
	bool verifica_sintaxa() override;
	void executa_comanda() override;

	comanda_delete(const comanda_delete&) = delete;
	comanda_delete& operator= (const comanda_delete&) = delete;
};

class comanda_update : public comanda
{
public:
	comanda_update(const char* c) : comanda(c) {}
	bool verifica_sintaxa() override;
	void executa_comanda() override;

	comanda_update(const comanda_update&) = delete;
	comanda_update& operator= (const comanda_update&) = delete;
};

class comanda_select : public comanda
{
public:
	comanda_select(const char* c) : comanda(c) {}
	bool verifica_sintaxa() override;
	void executa_comanda() override;

	comanda_select(const comanda_select&) = delete;
	comanda_select& operator= (const comanda_select&) = delete;
};

class comanda_import : public comanda
{
public:
	comanda_import(const char* c) : comanda(c) {}

	bool verifica_sintaxa() override;
	void executa_comanda() override;
	static bool verifica_fisier(char*);

	comanda_import(const comanda_import&) = delete;
	comanda_import& operator= (const comanda_import&) = delete;
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
class inregistrare : public serializabil
{
private:
	vector<string> valori_campuri;
public:
	//constructori
	inregistrare();
	inregistrare(vector<string> v);
	inregistrare(int, char[100][100]);

	//functii de citire si afisare
	friend istream& operator>>(istream&, inregistrare&);
	friend ostream& operator<<(ostream&, inregistrare);

	//functii fisiere binare
	void serializare(ofstream&) override;
	void deserializare(ifstream&) override;

	//functii accesor
	int get_nr_campuri();
	void set_valoare(string, unsigned int);
	void set_valori(vector<string>);
	string get_valoare(unsigned int);
	vector<string> get_valori();

	//alte supraincarcari de operatori
	bool operator!();
	explicit operator int();
	bool operator>=(inregistrare);
	string operator[](unsigned int);

	//alte functii
	void afiseaza_inregistrare(ofstream&, int);
	void scrie_rand_csv(ofstream&);


};

class repository : public serializabil
{
private:
	string nume_fisier;
public:
	repository();
	repository(string);


	void adauga(inregistrare);
	void sterge(int, string);
	void actualizeaza(int, int, string, string);
	void afiseaza_toate(ofstream&, int);
	void afiseaza_toate(ofstream&, int, string, int);
	void afiseaza_partial(ofstream&, int, vector<int>);
	void afiseaza_partial(ofstream&, int, vector<int>, int, string);
	void exporta_csv(ofstream&);

	void serializare(ofstream&) override;
	void deserializare(ifstream&) override;


};

//clasa pt structura unei coloane
class coloana : public serializabil
{
private:
	char* nume_coloana;
	tip_coloane tip_coloana;
	char* valoare_implicita;
	int dimensiune_coloana;
public:
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

	//functii fisiere binare
	void serializare(ofstream&) override;
	void deserializare(ifstream&) override;

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

class tabela : serializabil
{
private:
	string nume_tabela;
	list<coloana> coloane;	//lista cu structura coloanelor
	repository repo;		//repo unde se afla inregistrarile din tabela
public:
	//constructori
	tabela();
	tabela(const char*, list<coloana>);

	//operatori de citire si afisare consola
	friend ostream& operator<<(ostream&, tabela);
	friend istream& operator>>(istream&, tabela&);

	//functii de serializare
	void serializare(ofstream&) override;
	void deserializare(ifstream&) override;

	//functii accesor
	string get_nume_tabela();
	coloana get_coloana(unsigned int);
	coloana get_coloana(string);

	void set_nume_tabela(const char*);
	int get_nr_coloane();

	//alte functii
	void afiseaza_structura_tabela(); //pentru comanda display
	void adauga_inregistrare(inregistrare); //pentru comanda insert
	void sterge_inregistrare(conditie);	//pentru comanda delete
	void actualizeaza_inregistrare(conditie, conditie); //pentru comanda update
	void afiseaza_toate();
	void afiseaza_toate(conditie);
	void afiseaza_partial(vector<string>);
	void afiseaza_partial(vector<string>, conditie);
	int get_index_conditie(string);
	bool exista_coloana(string);
	void exporta_csv(ofstream&);
	//alti operatori
	bool operator!();
	explicit operator int();
	friend bool operator<(tabela, tabela);

};

//o singura baza de date cu mai multe tabele
class baza_de_date
{
private:
	static baza_de_date* instanta;
	map<string, tabela> tabele; //are numele si tabela asociata

	const char* FISIER_CONFIGURATIE = "config.txt";
	const int INDEX_SELECT = 0;		//primul rand din fisierul config
	const int INDEX_DISPLAY = 1;	//al doilea rand din fisierul config
	const string NUME_FISIER_SELECT = "SELECT_";
	const string NUME_FISIER_DISPLAY = "DISPLAY_";
	int rapoarte_select;
	int rapoarte_display;

	//constructor
	baza_de_date();
	void citeste_config();

	//operator egal si constructor de copiere
	baza_de_date(const baza_de_date&) = delete;
	baza_de_date& operator=(const baza_de_date&) = delete;

public:
	static baza_de_date* get_instanta();

	//getter
	tabela get_tabela(const char*);

	//functii pentru generat numele rapoartelor select si display
	string get_nume_fisier_select();
	string get_nume_fisier_display();

	//metode pentru comenzile DDL
	void adauga_tabela(tabela);
	void sterge_tabela(const char*);
	void afiseaza_tabela(const char*);

	//metoda pentru comenzile DML
	void insereaza_inregistrari(const char*, inregistrare);
	void sterge_inregistrari(const char*, conditie);
	void actualizeaza_inregistrari(const char*, conditie, conditie);
	void afiseaza_toate(const char*);
	void afiseaza_toate(const char*, conditie);
	void afiseaza_partial(const char*, vector<string>);
	void afiseaza_partial(const char*, vector<string>, conditie);

	//alte functii
	bool exista_tabela(string);
	void listeaza_tabele();
	void stergeti_toate_tabelele();

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

	static string to_upper(string);

	static char* copiere_sir(const char*);

	static bool este_comanda_incompleta(const char*);

	static tip_coloane identifica_tip_coloana(const char*);

	static tip_coloane identifica_tip_coloana_csv(const char*);

	static bool este_numar_pozitiv(const char*);

	static void trim(char*);

	static tip_coloane conversie(string);
};

class meniu
{
private:
	meniu() {}; //sa nu mearga instantiata
	static void afiseaza_meniu();
	static void afisati_numele_tabelelor();
	static void stergeti_toate_tabelele();
	static void afisati_inregistrarile_tabelei();
	static void stergeti_inregistrarile_tabelei();
	static void exportati_tabela();

public:
	static void executa_meniu();

};