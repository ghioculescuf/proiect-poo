#include "Header.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cctype>
#include <iomanip>
#include <string.h>
#include <fstream>
using namespace std;

//membrii statici
baza_de_date* baza_de_date::instanta = nullptr;


//PROCESOR COMENZI
//valideaza numele pentru tabele si coloane -- nu ar avea sens sa includ metoda in una din cele 2 clase
//citeste comanda de la tastatura si o identifica
void procesor_comenzi::citeste_comanda_fisier(const char* nume_fisier)
{
	ifstream f(nume_fisier);

	char linie[1000];
	if (f.is_open()) 
	{
		while (!f.eof()) 
		{
			f >> ws;
			f.get(linie , 999);
			
			if (strcmp(linie, "") != 0)
			{
				procesor_comenzi::proceseaza_comanda(linie);
			}
		}
	}
	else
	{
		cout << "Nu se poate deschide fisierul " << nume_fisier << endl;
	}
	f.close();
}

void procesor_comenzi::citeste_comanda_consola()
{
	//CITIRE COMANDA
	char comanda[1000];
	cout << "Introduceti o comanda:" << endl;
	cin >> ws;
	cin.get(comanda, 999);
	procesor_comenzi::proceseaza_comanda(comanda);
}

void procesor_comenzi::proceseaza_comanda(char* comanda)
{
	

	//PRIMUL CUVANT DIN COMANDA IN TOKEN
	char delims[] = " ";
	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(comanda, delims, &context);

	//SCOT SPATIILE DIN FATA A ORICE NU E LITERA DIN ALFABET
	int n = strlen(context);
	for (int i = 0; i < n - 1; i++)
	{
		if (context[i] == ' ' && !isalpha(context[i + 1]))
		{
			strcpy_s(context + i, n - i, context + i + 1);
			i--;
			n--;
		}
	}
	if (context[n - 1] == ' ')
	{
		context[n - 1] = '\0';
	}
	
	//SCOT SPATIILE DE DUPA VIRGULA
	n = strlen(context);
	for (int i = 0; i < n - 1; i++)
	{
		if (context[i] == ',' && context[i + 1] == ' ')
		{
			strcpy_s(context + i + 1, n - i - 1, context + i + 2);
			i--;
			n--;
		}
	}


	//COPIEZ SIRUL CA SA NU IL DISTRUG PE CEL INITIAL
	char* copie_sir = procesor_comenzi::copiere_sir(context);

	if (token != nullptr)
	{
		if (_stricmp(token, "CREATE") == 0)
		{
			if (comanda_create::verifica_sintaxa(copie_sir))
			{
				comanda_create::executa_comanda(context);
			}
		}
		else if (_stricmp(token, "DROP") == 0)
		{
			if (comanda_drop::verifica_sintaxa(copie_sir))
			{
				comanda_drop::executa_comanda(context);
			}
		}
		else if (_stricmp(token, "DISPLAY") == 0)
		{
			if (comanda_display::verifica_sintaxa(copie_sir))
			{
				comanda_display::executa_comanda(context);
			}
		}
		else if (_stricmp(token, "INSERT") == 0)
		{
			if (comanda_insert::verifica_sintaxa(copie_sir))
			{
				comanda_insert::executa_comanda(context);
			}
		}
		else if (_stricmp(token, "DELETE") == 0)
		{
			if (comanda_delete::verifica_sintaxa(copie_sir))
			{
				comanda_delete::executa_comanda(context);
			}
		}
		else if (_stricmp(token, "SELECT") == 0)
		{
			if (comanda_select::verifica_sintaxa(copie_sir))
			{
				comanda_select::executa_comanda(context);
			}
		}
		else if (_stricmp(token, "UPDATE") == 0)
		{
			if (comanda_update::verifica_sintaxa(copie_sir))
			{

				comanda_update::executa_comanda(context);
			}
		} 
		else if (_stricmp(token, "IMPORT") == 0)
		{
			if (comanda_import::verifica_sintaxa(copie_sir))
			{
				char* copie_context = procesor_comenzi::copiere_sir(context);//probabil trebuie delete dupa ce o folosesc
				if (comanda_import::verifica_fisier(copie_context))
				{

					comanda_import::executa_comanda(context);
				}
			}
		}
		else if (_stricmp(token, "EXIT") == 0)
		{
			throw 1;
		}
		else if (_stricmp(token, "LIST") == 0)
		{
			baza_de_date::get_instanta()->listeaza_tabele();
		}
		else
		{
			cout << "Nu recunosc comanda: " << token << endl;
		}
	}

	//daca se arunca exceptie nu ajunge aici si ramane in heap -- de corectat in faza 2
	if (copie_sir != nullptr)
	{
		delete[] copie_sir;
	}
}

bool procesor_comenzi::este_nume_valid(const char* nume)
{
	if (!isalpha(nume[0]))
	{
		return false;
	}
	int n = strlen(nume);
	for (int i = 1; i < n; i++)
	{
		if (!isalnum(nume[i]) && nume[i] != '_')
		{
			return false;
		}
	}
	return true;
}

//modifica sirul pe care il primeste ca parametru
void procesor_comenzi::to_upper(char* text)
{
	int n = strlen(text);
	for (int i = 0; i < n; i++)
	{
		if (text[i] >= 'a' && text[i] <= 'z')
		{
			text[i] -= 32;
		}
	}
}

//modifica sirul pe care il primeste ca parametru
void procesor_comenzi::to_upper(char* text, int lungime)
{
	int n = strlen(text);
	for (int i = 0; i < n && i < lungime; i++)
	{
		if (text[i] >= 'a' && text[i] <= 'z')
		{
			text[i] -= 32;
		}
	}
}

//realizeaza o copie a sirului primit ca si parametru ce ulterior va trebui dezalocata din afara functiei
char* procesor_comenzi::copiere_sir(const char* sir)
{
	if (sir != nullptr)
	{
		char* copie = new char[strlen(sir) + 1];
		strcpy_s(copie, strlen(sir) + 1, sir);
		return copie;
	}
	return nullptr;
}

bool procesor_comenzi::este_comanda_incompleta(const char* sir)
{
	if (sir == nullptr)
	{
		cout << "Comanda incompleta" << endl;
		return true;
	}
	return false;
}

//ajuta la identificarea tipului de coloana in functie de valoarea introdusa
tip_coloane procesor_comenzi::identifica_tip_coloana(const char* valoare)
{
	//de luat in considerare pt faza 2 ca pot fi si numere negative care incep cu minus
	int n = strlen(valoare);
	if ((valoare[0] == '\'' && valoare[n - 1] == '\'') || (valoare[0] == '"' && valoare[n - 1] == '"'))
	{
		return tip_coloane::text;
	}

	int nr_cifre = 0;
	int nr_puncte = 0;
	bool este_negativ = false;
	if (valoare[0] == '-')
	{
		este_negativ = true;
	}
	
	for (int i = ((este_negativ == true) ? 1 : 0); i < n; i++)
	{
		if (isdigit(valoare[i]))
		{
			nr_cifre++;
		}
		//numar punctele care nu sunt pe pozitiile marginale
		else if (valoare[i] == '.' && i != 0 && i != n - 1)
		{
			nr_puncte++;
		}
	}

	if ((nr_puncte == 1 && nr_cifre == n - 1) || (nr_puncte == 1 && nr_cifre == n-2 && este_negativ))
	{
		return tip_coloane::real;
	}

	if (nr_cifre == n || (nr_cifre == n-1 && este_negativ))
	{
		return tip_coloane::integer;
	}

	return tip_coloane::tip_nedefinit;
}

tip_coloane procesor_comenzi::identifica_tip_coloana_csv(const char* valoare)
{
	int n = strlen(valoare);

	int nr_cifre = 0;
	int nr_puncte = 0;
	bool este_negativ = false;
	if (valoare[0] == '-')
	{
		este_negativ = true;
	}

	for (int i = ((este_negativ == true) ? 1 : 0); i < n; i++)
	{
		if (isdigit(valoare[i]))
		{
			nr_cifre++;
		}
		//numar punctele care nu sunt pe pozitiile marginale
		else if (valoare[i] == '.' && i != 0 && i != n - 1)
		{
			nr_puncte++;
		}
	}

	if ((nr_puncte == 1 && nr_cifre == n - 1) || (nr_puncte == 1 && nr_cifre == n - 2 && este_negativ))
	{
		return tip_coloane::real;
	}

	if (nr_cifre == n || (nr_cifre == n - 1 && este_negativ))
	{
		return tip_coloane::integer;
	}

	return tip_coloane::text;
}

//verificare dimensiune sa fie un numar
bool procesor_comenzi::este_numar_pozitiv(const char* dimensiune)
{
	int n = strlen(dimensiune);
	for (int i = 0; i < n; i++)
	{
		if (!isdigit(dimensiune[i]))
		{
			return false;
		}
	}
	return true;
}

//scot primul si ultimul caracter din valorile de tip text ca sa pot sa le stochez
void procesor_comenzi::trim(char* valoare_text)
{
	if (valoare_text != nullptr)
	{
		strcpy_s(valoare_text, strlen(valoare_text), valoare_text + 1);
		valoare_text[strlen(valoare_text) - 1] = '\0';
	}
}

tip_coloane procesor_comenzi::conversie(string s)
{
	if (s == "TEXT")
	{
		return tip_coloane::text;
	}
	else if (s == "INTEGER")
	{
		return tip_coloane::integer;
	}
	else if (s == "FLOAT")
	{
		return tip_coloane::real;
	}
	return tip_coloane::tip_nedefinit;
}

//COMANDA IMPORT
bool comanda_import::verifica_sintaxa(char* c)
{

	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(c, " ", &context);
	//verificari pentru tabela -- e in token
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	//verific daca exista tabela in baza de date
	procesor_comenzi::to_upper(token);

	if (token != nullptr && !baza_de_date::get_instanta()->exista_tabela(token))
	{
		cout << "Tabela " << token << " nu exista" << endl;
		return false;
	}
	//verificari pentru fisier -- e in context
	if (procesor_comenzi::este_comanda_incompleta(context))
	{
		return false;
	}
	char aux[5];
	if (context != nullptr)
	{
		strcpy_s(aux, 5, context + strlen(context) - 4);
	}
	procesor_comenzi::to_upper(aux);
	if (strcmp(aux, ".CSV"))
	{
		cout << "Fisierul" << context << " nu este un fisier CSV" << endl;
		return false;
	}
	//verific daca exista fisierul -- 
	ifstream f(context, ios::_Nocreate);
	if (!f)
	{
		cout << "Fisierul " << context << " nu exista" << endl;
		return false;
	}

	return true;
}
bool comanda_import::verifica_fisier(char* c)
{
	//IMPORT nume_tabela nume_fisier.CSV
	char* token = nullptr;
	char* context = nullptr;
	cout << c << endl;
	c[strlen(c) - 1] = '\0';//scot ;
	token = strtok_s(c, " ", &context);// in token e numele tabelei

	tabela t = baza_de_date::get_instanta()->get_tabela(token);
	//string nume_tabela(token);
	fstream f;
	f.open(context, ios::in);

	char linii[20][100]; //fisierul poate sa aiba maxim 20 de linii
	int i = 0;
	while (!f.eof())
	{
		string linie;
		getline(f, linie); //citesc o linie si o salvez intr-un string pe care il introduc in vectorul de linii
		strcpy_s(linii[i], linie.length() + 1, linie.c_str());
		i++;
	}

	//acum verifc ca se potriveste fiecare linie cu tipul tabelei
	for (int j = 0; j < i; j++)
	{
		char* token1 = nullptr;
		char* context1 = nullptr;
		int nr_valori = 0;
		token1 = strtok_s(linii[j], ",", &context1);
		while (token1 != nullptr)
		{
			if (nr_valori > t.get_nr_coloane())
			{
				cout << "Pe o linie din fisierul CSV exista prea multe valori pentru structura tabelei" << endl;
				return false;
			}

			tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana_csv(token1);

			//verificare potrivire tip
			if (tip_valoare != procesor_comenzi::conversie(t.get_coloana(nr_valori).get_tip_coloana()))
			{
				cout << "Nepotrivire tip pentru coloana " << t.get_coloana(nr_valori).get_nume_coloana();
				return false;
			}

			//verificare potrivire dimensiune
			int lungime = (int)strlen(token1) - (tip_valoare == tip_coloane::text ? 2 : 0);
			if (lungime > t.get_coloana(nr_valori).get_dimensiune_coloana())
			{
				cout << "Valoarea introdusa pentru coloana " << token1 << " este prea mare" << endl;
				return false;
			}


			nr_valori++;
			token1 = strtok_s(nullptr, ",()", &context1);
		}
		if (nr_valori < t.get_nr_coloane())
		{
			cout << "Ati introdus prea putine coloane pentru strucutra tabelei " << t.get_nume_tabela() << endl;
			return false;
		}

	}
	return true;
}
void comanda_import::executa_comanda(char* c)
{
	//IMPORT nume_tabela nume_fisier.CSV
	char* token = nullptr;
	char* context = nullptr;
	//cout << c << endl;
	c[strlen(c) - 1] = '\0';//scot ;
	token = strtok_s(c, " ", &context);// in token e numele tabelei
	cout << "Importam in tabela " << token << endl;

	string nume_tabela(token);
	fstream f;
	f.open(context, ios::in);


	char linii[20][100]; //fisierul poate sa aiba maxim 20 de linii
	int i = 0;
	while (!f.eof())
	{
		string linie;
		getline(f, linie); //citesc o linie si o salvez intr-un string pe care il introduc in vectorul de linii
		strcpy_s(linii[i], linie.length() + 1, linie.c_str());
		i++;
	}


	for (int j = 0; j < i; j++)
	{
		char valori[100][100];
		int nr_i = 0;
		char* token1 = nullptr;
		char* context1 = nullptr;
		int nr_valori = 0;
		token1 = strtok_s(linii[j], ",", &context1);
		while (token1 != nullptr)
		{
			tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana(token1);

			strcpy_s(valori[nr_i++], strlen(token1) + 1, token1);
			token1 = strtok_s(nullptr, ",", &context1);
		}

		inregistrare inr(nr_i, valori);
		baza_de_date::get_instanta()->insereaza_inregistrari(nume_tabela.c_str(), inr);
	}
}

//COMANDA CREATE
bool comanda_create::verifica_sintaxa(char* c)
{
	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant TABLE
	token = strtok_s(c, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "table") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru numele tabelei
	token = strtok_s(nullptr, "(", &context);

	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}
	//verific daca numele tabelei e valid <=> sa fie format doar din '_', cifre si litere
	//verific daca exista deja tabela in baza de date
	procesor_comenzi::to_upper(token);
	if (!procesor_comenzi::este_nume_valid(token))
	{
		cout << "Numele " << token << " este invalid" << endl;
		return false;
	}

	if (baza_de_date::get_instanta()->exista_tabela(token) == true)
	{
		cout << "Numele tabelei " << token << " exista deja" << endl;
		return false;
	}

	char delims[] = "(),";
	string nume_coloane[20];
	int nr_coloane = 0;

	token = strtok_s(nullptr, delims, &context);
	while (token != nullptr)
	{
		//numele coloanei
		if (procesor_comenzi::este_comanda_incompleta(token))
		{
			return false;
		}
		procesor_comenzi::to_upper(token);
		string nume_coloana(token);

		if (!procesor_comenzi::este_nume_valid(token))
		{
			cout << "Numele coloanei: " << token << " este invalid" << endl;
			return false;
		}

		//tipul coloanei
		token = strtok_s(nullptr, delims, &context);
		if (procesor_comenzi::este_comanda_incompleta(token))
		{
			return false;
		}
		tip_coloane tip_col;
		if (_stricmp("INTEGER", token) == 0)
		{
			tip_col = tip_coloane::integer;
		}
		else if (_stricmp("FLOAT", token) == 0)
		{
			tip_col = tip_coloane::real;
		}
		else if (_stricmp("TEXT", token) == 0)
		{
			tip_col = tip_coloane::text;
		}
		else
		{
			cout << "Tip coloana: " << token << " nedefinit" << endl;
			return false;
		}


		//dimensiunea coloanei
		token = strtok_s(nullptr, delims, &context);
		if (procesor_comenzi::este_comanda_incompleta(token))
		{
			return false;
		}

		if (!procesor_comenzi::este_numar_pozitiv(token))
		{
			cout << "Ati introdus gresit dimensiunea coloanei: " << nume_coloana << endl;
			return false;
		}

		int dimensiune_col = atoi(token);

		//valoarea implicita
		token = strtok_s(nullptr, delims, &context);
		if (procesor_comenzi::este_comanda_incompleta(token))
		{
			return false;
		}

		//verificari suplimentare
		for (int i = 0; i < nr_coloane; i++)
		{
			if (nume_coloana == nume_coloane[i])
			{
				cout << "Duplicat la numele coloanelor" << endl;
				return false;
			}
		}

		tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana(token);
		if (tip_col != tip_valoare)
		{
			cout << "Tipul valorii implicite difera de tipul coloanei: " << nume_coloana << endl;
			return false;
		}

		if (tip_valoare == tip_coloane::text)
		{
			procesor_comenzi::trim(token);
		}

		if ((int)strlen(token) > dimensiune_col)
		{
			cout << "Valoarea implicita este mai mare decat dimensiunea coloanei " << nume_coloana << endl;
		}

		nume_coloane[nr_coloane++] = nume_coloana;

		token = strtok_s(nullptr, delims, &context);
	}

	return true;
	//create table if not exists studenti -- inca nu am implementat pt ca nu stiu ce comportament ar trebui sa aiba
}

void comanda_create::executa_comanda(char* c)
{
	c[strlen(c) - 1] = '\0';

	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(c, " ", &context); //TABLE
	token = strtok_s(nullptr, " (", &context);
	//cout << "s-a apelat comanda create pentru tabela: " << token << " cu atributele " << endl;

	procesor_comenzi::to_upper(token);
	string nume_t(token);

	char delims[] = "(),";

	coloana coloane[20];
	int nr_coloane = 0;
	token = strtok_s(nullptr, delims, &context);

	while (token != nullptr)
	{
		coloana c;

		//cout << "--nume: " << token << endl;
		procesor_comenzi::to_upper(token);
		c.set_nume_coloana(token);

		token = strtok_s(nullptr, delims, &context);
		//cout << "--tip: " << token << endl;
		if (_stricmp("INTEGER", token) == 0)
		{
			c.set_tip_coloana(tip_coloane::integer);
		}
		else if (_stricmp("FLOAT", token) == 0)
		{
			c.set_tip_coloana(tip_coloane::real);
		}
		else if (_stricmp("TEXT", token) == 0)
		{
			c.set_tip_coloana(tip_coloane::text);
		}

		token = strtok_s(nullptr, delims, &context);
		//cout << "--dimensiune: " << token << endl;
		c.set_dimensiune_coloana(atoi(token));

		token = strtok_s(nullptr, delims, &context);
		//cout << "--valoare implicita: " << token << endl;
		if (c.get_tip_coloana() == "TEXT")
		{
			procesor_comenzi::trim(token);
		}
		c.set_valoare_implicita(token);

		coloane[nr_coloane++] = c;

		token = strtok_s(nullptr, delims, &context);
	}

	tabela t(nume_t.c_str(), nr_coloane, coloane);

	baza_de_date::get_instanta()->adauga_tabela(t);

	cout << "Tabela " << nume_t << " a fost creata." << endl;
}

//COMANDA DISPLAY
bool comanda_display::verifica_sintaxa(char* c)
{

	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant TABLE
	token = strtok_s(c, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "table") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru numele tabelei -- e in context

	if (procesor_comenzi::este_comanda_incompleta(context))
	{
		return false;
	}

	//verific daca exista tabela in baza de date
	procesor_comenzi::to_upper(context);

	if (context != nullptr && !baza_de_date::get_instanta()->exista_tabela(context))
	{
		cout << "Tabela " << context << " nu exista" << endl;
		return false;
	}

	return true;
}

void comanda_display::executa_comanda(char* c)
{
	c[strlen(c) - 1] = '\0';
	//cout << "s-a apelat comanda display pentru tabela: " << c + 6 << endl;
	baza_de_date::get_instanta()->afiseaza_tabela(c + 6, 1);
}

//COMANDA DROP
bool comanda_drop::verifica_sintaxa(char* c)
{
	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant TABLE
	token = strtok_s(c, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "table") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru numele tabelei -- e in context

	if (procesor_comenzi::este_comanda_incompleta(context))
	{
		return false;
	}

	//verific daca exista deja tabela in baza de date
	procesor_comenzi::to_upper(context);

	if (context != nullptr && !baza_de_date::get_instanta()->exista_tabela(context))
	{
		cout << "Tabela " << context << " nu exista" << endl;
		return false;
	}

	return true;
}

void comanda_drop::executa_comanda(char* c)
{
	c[strlen(c) - 1] = '\0';
	//cout << "s-a apelat comanda drop pentru tabela: " << c + 6 << endl;
	procesor_comenzi::to_upper(c + 6);
	baza_de_date::get_instanta()->sterge_tabela(c + 6);
	cout << "Tabela " << c + 6 << " a fost stearsa."<<endl;
}


//COMANDA INSERT
bool comanda_insert::verifica_sintaxa(char* c)
{
	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant INTO
	token = strtok_s(c, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "into") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru numele tabelei -- e in context
	token = strtok_s(nullptr, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	//verific daca exista deja tabela in baza de date
	procesor_comenzi::to_upper(token);

	if (!baza_de_date::get_instanta()->exista_tabela(token))
	{
		cout << "Tabela " << token << " nu exista" << endl;
		return false;
	}

	tabela t = baza_de_date::get_instanta()->get_tabela(token);

	//verificare prezenta cuvant VALUES
	token = strtok_s(nullptr, "(", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}
	if (_stricmp(token, "values") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	int nr_valori = 0;
	token = strtok_s(nullptr, ",()", &context);
	while (token != nullptr)
	{
		if (nr_valori > t.get_nr_coloane())
		{
			cout << "Ati introdus prea multe valori pentru structura tabelei" << endl;
			return false;
		}

		tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana(token);

		//verificare potrivire tip
		if (tip_valoare != procesor_comenzi::conversie(t.get_coloana(nr_valori).get_tip_coloana()))
		{
			cout << "Nepotrivire tip pentru coloana " << t.get_coloana(nr_valori).get_nume_coloana();
			return false;
		}

		//verificare potrivire dimensiune
		int lungime = (int)strlen(token) - (tip_valoare == tip_coloane::text ? 2 : 0);
		if (lungime > t.get_coloana(nr_valori).get_dimensiune_coloana())
		{
			cout << "Valoarea introdusa pentru coloana " << token << " este prea mare";
			return false;
		}


		nr_valori++;
		token = strtok_s(nullptr, ",()", &context);
	}


	if (nr_valori < t.get_nr_coloane())
	{
		cout << "Ati introdus prea putine coloane pentru strucutra tabelei " << t.get_nume_tabela() << endl;
		return false;
	}

	return true;
}

void comanda_insert::executa_comanda(char* c)
{
	//INSERT INTO nume_tabela VALUES(...); valorile sunt separate prin, și au numărul și ordinea exacta ca definiția tabelului
	char* token = nullptr;
	char* context = nullptr;
	//cout << c << endl;
	c[strlen(c) - 1] = '\0';//scot ;
	token = strtok_s(c, " ", &context);//INTO

	token = strtok_s(nullptr, " ", &context);//aici am numele tabelei
	cout << "Inseram in tabela " << token << endl;
	string nume_tabela(token);

	token = strtok_s(nullptr, " (", &context);//VALUES

	char valori[100][100];
	int nr_i = 0;

	token = strtok_s(nullptr, ",()", &context);
	while (token != nullptr)
	{
		tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana(token);
		if (tip_valoare == tip_coloane::text)
		{
			procesor_comenzi::trim(token);
		}

		strcpy_s(valori[nr_i++], strlen(token) + 1, token);
		token = strtok_s(nullptr, ",()", &context);
	}

	inregistrare inr(nr_i, valori);
	baza_de_date::get_instanta()->insereaza_inregistrari(nume_tabela.c_str(), inr);
}


//COMANDA DELETE
bool comanda_delete::verifica_sintaxa(char* c)
{
	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant FROM
	token = strtok_s(c, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "from") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru numele tabelei -- e in context
	token = strtok_s(nullptr, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	//verific daca exista tabela in baza de date
	procesor_comenzi::to_upper(token);

	if (!baza_de_date::get_instanta()->exista_tabela(token))
	{
		cout << "Tabela " << token << " nu exista" << endl;
		return false;
	}

	tabela t = baza_de_date::get_instanta()->get_tabela(token);

	//verificare existenta cuvant WHERE
	token = strtok_s(nullptr, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "where") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru conditie
	token = strtok_s(nullptr, "=", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	procesor_comenzi::to_upper(token);
	if (!t.exista_coloana(token))
	{
		cout << "Nu exista coloana " << token << endl;
		return false;
	}

	if (procesor_comenzi::este_comanda_incompleta(context))
	{
		return false;
	}

	//verificare potrivire tip valoare cu tip coloana
	tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana(context);
	if (tip_valoare != procesor_comenzi::conversie(t.get_coloana(token).get_tip_coloana()))
	{
		cout << "Tipul valoarii din clauza where nu se potriveste cu tipul coloanei" << endl;
		return false;
	}

	return true;

}

void comanda_delete::executa_comanda(char* c)
{
	c[strlen(c) - 1] = '\0';//scot ;

	//DELETE FROM nume_tabela WHERE nume_coloană = valoare (DELETE permite doar o coloană în clauza WHERE)
	char* token = nullptr;
	char* context = nullptr;

	token = strtok_s(c, " ", &context);//aici am cuvantul FROM
	token = strtok_s(nullptr, " ", &context);
	cout << "Delete pe tabela: " << token << endl;
	string nume_tabela(token);


	token = strtok_s(nullptr, " ", &context); // aici e cuvantul WHERE
	token = strtok_s(nullptr, "=", &context);
	//cout << "Unde clauza where cauta coloana: " << token << " unde e egala cu valoarea ";
	//cout << context << endl;
	tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana(context);
	if (tip_valoare == tip_coloane::text)
	{
		procesor_comenzi::trim(context);
	}

	procesor_comenzi::to_upper(token);

	conditie cond(token, context);

	baza_de_date::get_instanta()->sterge_inregistrari(nume_tabela.c_str(), cond);
}

//COMANDA UPDATE
bool comanda_update::verifica_sintaxa(char* c)
{
	//UPDATE nume_tabela SET nume_coloană = valoare WHERE nume_coloană = valoare
	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;


	//verificari pentru numele tabelei -- e in context
	token = strtok_s(c, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	//verific daca exista tabela in baza de date
	procesor_comenzi::to_upper(token);

	if (!baza_de_date::get_instanta()->exista_tabela(token))
	{
		cout << "Tabela " << token << " nu exista" << endl;
		return false;
	}

	tabela t = baza_de_date::get_instanta()->get_tabela(token);

	//verificare existenta cuvant SET
	token = strtok_s(nullptr, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "set") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru conditie
	token = strtok_s(nullptr, "=", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	procesor_comenzi::to_upper(token);
	if (!t.exista_coloana(token))
	{
		cout << "Nu exista coloana " << token << endl;
		return false;
	}

	coloana col = t.get_coloana(token);

	token = strtok_s(nullptr, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	//verificare potrivire tip valoare cu tip coloana
	tip_coloane tip_valoare = procesor_comenzi::identifica_tip_coloana(token);
	if (tip_valoare != procesor_comenzi::conversie(col.get_tip_coloana()))
	{
		cout << "Tipul valoarii din clauza set nu se potriveste cu tipul coloanei" << endl;
		return false;
	}


	//verificare existenta cuvant WHERE
	token = strtok_s(nullptr, " ", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if (_stricmp(token, "where") != 0)
	{
		cout << "Comanda introdusa este gresita" << endl;
		return false;
	}

	//verificari pentru conditie
	token = strtok_s(nullptr, "=", &context);
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	procesor_comenzi::to_upper(token);
	if (!t.exista_coloana(token))
	{
		cout << "Nu exista coloana " << token << endl;
		return false;
	}

	col = t.get_coloana(token);

	if (procesor_comenzi::este_comanda_incompleta(context))
	{
		return false;
	}

	//verificare potrivire tip valoare cu tip coloana
	tip_valoare = procesor_comenzi::identifica_tip_coloana(context);
	if (tip_valoare != procesor_comenzi::conversie(col.get_tip_coloana()))
	{
		cout << "Tipul valoarii din clauza where nu se potriveste cu tipul coloanei" << endl;
		return false;
	}
	return true;

}

void comanda_update::executa_comanda(char* c)
{
	c[strlen(c) - 1] = '\0';//scot ;
	char* token = nullptr;
	char* context = nullptr;

	token = strtok_s(c, " ", &context);
	cout << "Update pe tabela: " << token << endl;
	procesor_comenzi::to_upper(token);
	string nume_tabela = token;

	token = strtok_s(nullptr, " ", &context);//aici e cuvantul SET

	conditie c1;
	token = strtok_s(nullptr, "=", &context);
	procesor_comenzi::to_upper(token);
	c1.set_nume_coloana(token);
	//cout << "Se face set pe coloana: " << token << " cu valoarea: ";

	token = strtok_s(nullptr, " ", &context);

	if (procesor_comenzi::identifica_tip_coloana(token) == tip_coloane::text)
	{
		procesor_comenzi::trim(token);
	}
	c1.set_valoare_coloana(token);

	token = strtok_s(nullptr, " ", &context); // aici e cuvantul WHERE
	token = strtok_s(nullptr, "=", &context);
	if (procesor_comenzi::identifica_tip_coloana(context) == tip_coloane::text)
	{
		procesor_comenzi::trim(context);
	}
	procesor_comenzi::to_upper(token);
	conditie c2(token, context);

	baza_de_date::get_instanta()->actualizeaza_inregistrari(nume_tabela.c_str(), c1, c2);
}


//COMANDA SELECT
bool comanda_select::verifica_sintaxa(char* c)
{
	if (procesor_comenzi::este_comanda_incompleta(c))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (c[strlen(c) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	c[strlen(c) - 1] = '\0';


	bool coloane_specifice = false;
	procesor_comenzi::to_upper(c, 3);
	char* restul_sirului = nullptr;
	string nume_coloane[20];
	int nr_coloane = 0;
	if (c == strstr(c, "ALL"))
	{
		restul_sirului = c + 3;
	}

	if (c[0] == '(' && strstr(c, ")") != nullptr)
	{
		char* final_paranteze = strstr(c, ")");
		restul_sirului = final_paranteze + 1;
		*final_paranteze = '\0';


		char* token = nullptr;
		char* context = nullptr;
		token = strtok_s(c, " (),", &context);
		while (token != nullptr)
		{
			procesor_comenzi::to_upper(token);
			//nu merge la peste 20 de parametrii
			nume_coloane[nr_coloane++] = token;
			//cout << token << endl;
			token = strtok_s(nullptr, " (),", &context);
		}
		coloane_specifice = true;

	}

	if (restul_sirului == nullptr)
	{
		cout << "Lipseste precizarea coloanelor" << endl;
		return false;
	}

	if (coloane_specifice && nr_coloane == 0)
	{
		cout << "Trebuie sa declarati cel putin o colana" << endl;
		return false;
	}


	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(restul_sirului, " ", &context);

	//de aici continui ca si cum incep de la where

	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	if ((_stricmp(token, "FROM") != 0))
	{
		cout << "Lipseste cuvantul FROM" << endl;
		return false;
	}


	token = strtok_s(nullptr, " ", &context); //aici e numele tabelei
	if (procesor_comenzi::este_comanda_incompleta(token))
	{
		return false;
	}

	procesor_comenzi::to_upper(token);

	if (!baza_de_date::get_instanta()->exista_tabela(token))
	{
		cout << "Tabela " << token << " nu exista" << endl;
		return false;
	}

	tabela t = baza_de_date::get_instanta()->get_tabela(token);

	if (coloane_specifice == true)
	{
		for (int i = 0; i < nr_coloane; i++)
		{
			if (!t.exista_coloana(nume_coloane[i].c_str()))
			{
				cout << "Coloana " << nume_coloane[i] << " nu exista in tabela" << endl;
				return false;
			}
		}
	}


	token = strtok_s(nullptr, " ", &context); //aici ar trebui sa fie where daca a fost pus
	if (token != nullptr)
	{
		if (_stricmp(token, "WHERE") != 0)
		{
			cout << "Comanda gresita" << endl;
			return false;
		}

		token = strtok_s(nullptr, "=", &context);

		if (procesor_comenzi::este_comanda_incompleta(token))
		{
			return false;
		}

		procesor_comenzi::to_upper(token);
		if (!t.exista_coloana(token))
		{
			cout << "Nu exista coloana " << token << endl;
			return false;
		}

		tip_coloane tip_col = procesor_comenzi::conversie(t.get_coloana(token).get_tip_coloana());

		if (procesor_comenzi::este_comanda_incompleta(context))
		{
			return false;
		}

		tip_coloane tip_val = procesor_comenzi::identifica_tip_coloana(context);

		if (tip_col != tip_val)
		{
			cout << "Tipul coloanei din clauza where difera de tipul valorii din clauza where" << endl;
			return false;
		}
	}
	return true;
}

void comanda_select::executa_comanda(char* c)
{

	//cout << "se executa comanda select" << endl;
	c[strlen(c) - 1] = '\0';


	bool toate_coloanele = false;

	procesor_comenzi::to_upper(c, 3);
	char* restul_sirului = nullptr;
	string nume_coloane[20];
	int nr_coloane = 0;
	if (c == strstr(c, "ALL"))
	{
		//atunci continui cu from where
		restul_sirului = c + 3;
		toate_coloanele = true;
	}

	if (c[0] == '(' && strstr(c, ")") != nullptr)
	{

		char* final_paranteze = strstr(c, ")");
		restul_sirului = final_paranteze + 1;
		*final_paranteze = '\0';


		char* token = nullptr;
		char* context = nullptr;
		//cout << endl << "se selecteaza coloanele" << endl;
		token = strtok_s(c, " (),", &context);
		while (token != nullptr)
		{
			procesor_comenzi::to_upper(token);
			//--doar sub 20 de parametrii
			nume_coloane[nr_coloane++] = token;
			//cout << token << endl;
			token = strtok_s(nullptr, " (),", &context);
		}
		toate_coloanele = false;

	}

	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(restul_sirului, " ", &context);//from

	token = strtok_s(nullptr, " ", &context); //aici e numele tabelei
	procesor_comenzi::to_upper(token);
	string nume_tabela = token;

	token = strtok_s(nullptr, " ", &context); //aici ar trebui sa fie where daca a fost pus

	conditie cond;
	bool exista_where = false;
	if (token != nullptr)
	{
		token = strtok_s(nullptr, "=", &context);
		cond.set_nume_coloana(token);
		if (procesor_comenzi::identifica_tip_coloana(context) == tip_coloane::text)
		{
			procesor_comenzi::trim(context);
		}
		cond.set_valoare_coloana(context);
		exista_where = true;
	}

	if (toate_coloanele == true && exista_where == false)
	{
		baza_de_date::get_instanta()->afiseaza_toate(nume_tabela.c_str());
	}
	else if (toate_coloanele == true && exista_where == true)
	{
		baza_de_date::get_instanta()->afiseaza_toate(nume_tabela.c_str(), cond);
	}
	else if (toate_coloanele == false && exista_where == false)
	{
		baza_de_date::get_instanta()->afiseaza_partial(nume_tabela.c_str(), nume_coloane, nr_coloane);
	}
	else if (toate_coloanele == false && exista_where == true)
	{
		baza_de_date::get_instanta()->afiseaza_partial(nume_tabela.c_str(), nume_coloane, nr_coloane, cond);
	}
}



//CONDITIE
//constructori
conditie::conditie()
{
	this->nume_coloana = nullptr;
	this->valoare_coloana = nullptr;
}

conditie::conditie(const char* nume, const char* valoare) : conditie()
{
	if (nume != nullptr)
	{
		this->nume_coloana = new char[strlen(nume) + 1];
		strcpy_s(this->nume_coloana, strlen(nume) + 1, nume);
	}

	if (valoare != nullptr)
	{
		this->valoare_coloana = new char[strlen(valoare) + 1];
		strcpy_s(this->valoare_coloana, strlen(valoare) + 1, valoare);
	}
}

//regula celor trei
conditie::conditie(const conditie& c)
{
	if (c.nume_coloana != nullptr)
	{
		this->nume_coloana = new char[strlen(c.nume_coloana) + 1];
		strcpy_s(this->nume_coloana, strlen(c.nume_coloana) + 1, c.nume_coloana);
	}
	else
	{
		this->nume_coloana = nullptr;
	}

	if (c.valoare_coloana != nullptr)
	{
		this->valoare_coloana = new char[strlen(c.valoare_coloana) + 1];
		strcpy_s(this->valoare_coloana, strlen(c.valoare_coloana) + 1, c.valoare_coloana);
	}
	else
	{
		this->valoare_coloana = nullptr;
	}
}

conditie conditie::operator=(conditie c)
{
	if (this->nume_coloana != nullptr)
	{
		delete[] this->nume_coloana;
	}
	if (this->valoare_coloana != nullptr)
	{
		delete[] this->valoare_coloana;
	}

	if (c.nume_coloana != nullptr)
	{
		this->nume_coloana = new char[strlen(c.nume_coloana) + 1];
		strcpy_s(this->nume_coloana, strlen(c.nume_coloana) + 1, c.nume_coloana);
	}

	if (c.valoare_coloana != nullptr)
	{
		this->valoare_coloana = new char[strlen(c.valoare_coloana) + 1];
		strcpy_s(this->valoare_coloana, strlen(c.valoare_coloana) + 1, c.valoare_coloana);
	}

	return *this;
}

conditie::~conditie()
{
	if (this->nume_coloana != nullptr)
	{
		delete[] this->nume_coloana;
	}
	if (this->valoare_coloana != nullptr)
	{
		delete[] this->valoare_coloana;
	}
}

//functii de citire si afisare la consola
ostream& operator<<(ostream& out, conditie c)
{
	out << "Nume coloana: " << c.nume_coloana << endl;
	out << "Valoare coloana: " << c.valoare_coloana << endl;
	return out;
}

istream& operator>>(istream& in, conditie& c)
{
	char buffer[100];
	cout << "Dati nume coloana: ";
	in >> ws;
	in.get(buffer, 99);
	c.nume_coloana = new char[strlen(buffer) + 1];
	strcpy_s(c.nume_coloana, strlen(buffer) + 1, buffer);
	cout << endl;

	cout << "Dati valoare colana: ";
	in >> ws;
	in.get(buffer, 99);
	c.valoare_coloana = new char[strlen(buffer) + 1];
	strcpy_s(c.valoare_coloana, strlen(buffer) + 1, buffer);
	cout << endl;

	return in;
}

//functii accesor
string conditie::get_nume_coloana()
{
	if (this->nume_coloana != nullptr)
	{
		return string(this->nume_coloana);
	}
	return "";
}

void conditie::set_nume_coloana(const char* nume)
{
	if (this->nume_coloana != nullptr)
	{
		delete[] this->nume_coloana;
	}

	if (nume != nullptr)
	{
		this->nume_coloana = new char[strlen(nume) + 1];
		strcpy_s(this->nume_coloana, strlen(nume) + 1, nume);
	}
}

string conditie::get_valoare_coloana()
{
	if (this->valoare_coloana != nullptr)
	{
		return string(this->valoare_coloana);
	}
	return "";
}

void conditie::set_valoare_coloana(const char* valoare)
{
	if (this->valoare_coloana != nullptr)
	{
		delete[] this->valoare_coloana;
	}

	if (valoare != nullptr)
	{
		this->valoare_coloana = new char[strlen(valoare) + 1];
		strcpy_s(this->valoare_coloana, strlen(valoare) + 1, valoare);
	}
}

//INREGISTRARE
//constructori
inregistrare::inregistrare()
{
	nr_campuri = 0;
	valori_campuri = nullptr;
}

inregistrare::inregistrare(int nr_cmp, char valori[100][100]) :inregistrare()
{
	if (nr_cmp > 0)//daca modific parametrul trebuie sa adaug si verificare sa nu fie campul ala nullptr
	{
		nr_campuri = nr_cmp;
		valori_campuri = new char* [nr_cmp];
		for (int i = 0; i < nr_cmp; i++)
		{
			int n = strlen(valori[i]);
			valori_campuri[i] = new char[n + 1];
			strcpy_s(valori_campuri[i], n + 1, valori[i]);
		}
	}
}

//regula celor 3
inregistrare::inregistrare(const inregistrare& obj)
{
	if (obj.nr_campuri > 0 && obj.valori_campuri != nullptr)
	{
		this->nr_campuri = obj.nr_campuri;
		this->valori_campuri = new char* [obj.nr_campuri];
		for (int i = 0; i < obj.nr_campuri; i++)
		{
			int n = strlen(obj.valori_campuri[i]);
			this->valori_campuri[i] = new char[n + 1];
			strcpy_s(this->valori_campuri[i], n + 1, obj.valori_campuri[i]);
		}
	}
	else
	{
		this->nr_campuri = 0;
		this->valori_campuri = nullptr;
	}
}

inregistrare::~inregistrare()
{
	if (valori_campuri != nullptr)
	{
		for (int i = 0; i < nr_campuri; i++)
		{
			if (valori_campuri[i] != nullptr)
			{
				delete[] valori_campuri[i];
			}
		}
		delete[] valori_campuri;
	}
}

inregistrare inregistrare::operator=(inregistrare obj)
{
	if (this->valori_campuri != nullptr)
	{
		for (int i = 0; i < this->nr_campuri; i++)
		{
			if (this->valori_campuri[i] != nullptr)
			{
				delete[] this->valori_campuri[i];
			}
		}
		delete[] this->valori_campuri;
	}

	if (obj.nr_campuri > 0 && obj.valori_campuri != nullptr)
	{
		this->nr_campuri = obj.nr_campuri;
		this->valori_campuri = new char* [obj.nr_campuri];
		for (int i = 0; i < obj.nr_campuri; i++)
		{
			int n = strlen(obj.valori_campuri[i]);
			this->valori_campuri[i] = new char[n + 1];
			strcpy_s(this->valori_campuri[i], n + 1, obj.valori_campuri[i]);
		}
	}
	else
	{
		this->nr_campuri = 0;
		this->valori_campuri = nullptr;
	}


	return *this;
}

//operatori de citire si afisare consola
istream& operator>>(istream& in, inregistrare& obj)
{
	//daca utilizatorul introduce numere negative avem eroare de executie
	cout << "Dati numarul de campuri: ";
	in >> obj.nr_campuri;
	cout << endl;

	obj.valori_campuri = new char* [obj.nr_campuri];

	for (int i = 0; i < obj.nr_campuri; i++)
	{
		cout << "Camp " << i;
		char buffer[100];
		in >> ws;
		in.get(buffer, 99);
		int n = strlen(buffer);
		obj.valori_campuri[i] = new char[n + 1];
		strcpy_s(obj.valori_campuri[i], n + 1, buffer);
		cout << endl;
	}

	return in;
}

ostream& operator<<(ostream& out, inregistrare obj)
{
	out << "Numar campuri: " << obj.nr_campuri << endl;
	out << "Valori campuri:";
	for (int i = 0; i < obj.nr_campuri; i++)
	{
		out << " " << obj.valori_campuri[i];
	}
	out << endl;

	return out;
}

int inregistrare::get_nr_campuri()
{
	return nr_campuri;
}

void inregistrare::set_valori(char** c, int nr_cmp)
{
	if (valori_campuri != nullptr)
	{
		for (int i = 0; i < nr_campuri; i++)
		{
			if (valori_campuri[i] != nullptr)
			{
				delete[] valori_campuri[i];
			}
		}
		delete[] valori_campuri;
	}

	if (nr_cmp > 0 && c != nullptr)
	{
		this->nr_campuri = nr_cmp;
		this->valori_campuri = new char* [nr_cmp];
		for (int i = 0; i < nr_cmp; i++)
		{
			int n = strlen(c[i]);
			this->valori_campuri[i] = new char[n + 1];
			strcpy_s(this->valori_campuri[i], n + 1, c[i]);
		}
	}
	else
	{
		this->nr_campuri = 0;
		this->valori_campuri = nullptr;
	}
}

void inregistrare::set_valoare(const char* val, int index)
{
	if (valori_campuri[index] != nullptr)
	{
		delete[] valori_campuri[index];
	}

	valori_campuri[index] = new char[strlen(val) + 1];
	strcpy_s(valori_campuri[index], strlen(val) + 1, val);
}

char** inregistrare::get_valori()
{
	if (nr_campuri > 0)
	{
		char** c = new char* [nr_campuri];
		for (int i = 0; i < nr_campuri; i++)
		{
			if (valori_campuri[i] != nullptr)
			{
				int n = strlen(valori_campuri[i]);
				c[i] = new char[n + 1];
				strcpy_s(c[i], n + 1, valori_campuri[i]);
			}
		}
		return c;
	}
	return nullptr;
}

//alti operatori
bool inregistrare::operator!()
{
	return nr_campuri > 0;
}

inregistrare::operator int()
{
	return nr_campuri;
}

bool inregistrare::operator>=(inregistrare inr)
{
	return this->nr_campuri >= inr.nr_campuri;
}

char* inregistrare::operator[](int index)
{
	if (index >= 0 && index < nr_campuri)
	{
		return valori_campuri[index];
	}
	else
	{
		throw exception("Se incearca citire in afara limitelor");
	}
}


//alte functii
string inregistrare::get_valoare(int index)
{
	if (valori_campuri[index] != nullptr)
	{
		return string(valori_campuri[index]);
	}
	return "";
}

void inregistrare::afiseaza_inregistrare(ofstream& out,int dimensiune)
{
	for (int i = 0; i < nr_campuri; i++)
	{
		cout << left << setw(dimensiune) << setfill(' ') << valori_campuri[i];
		out << left << setw(dimensiune) << setfill(' ') << valori_campuri[i];
	}
	cout << endl;
	out << endl;
}

//REPOSITORY
repository::repository() {}
repository::repository(char* nume_fisier)
{
	if (nume_fisier != nullptr)
	{
		this->nume_fisier = string(nume_fisier) + ".bin";
	}
}
void repository::adauga(inregistrare obj)
{
	ofstream f(nume_fisier, ios::binary | ios::app);
	obj.serializare(f);
	f.close();
}

void repository::sterge(int index, conditie cond)
{
	ifstream f(nume_fisier, ios::binary);
	ofstream g("temp.bin", ios::binary | ios::trunc);

	int k = 0;
	if (f.is_open())
	{
		while (!f.eof())
		{
			inregistrare obj;
			obj.deserializare(f);
			if (obj.get_nr_campuri() != 0)
			{
				if (cond.get_valoare_coloana() != obj.get_valoare(index))
				{
					obj.serializare(g);
				}
				else
				{
					k++;
				}
			}
		}
		f.close();
		g.close();
		cout << k << " inregistrari sterse" << endl;
		remove(nume_fisier.c_str());
		int r = rename("temp.bin", nume_fisier.c_str());
	}
}

void repository::actualizeaza(int index_cu, int index_cw, conditie c1, conditie c2)
{
	ifstream f(nume_fisier, ios::binary);
	ofstream g("temp.bin", ios::binary | ios::trunc);
	int k = 0;
	if (f.is_open()) {
		while (!f.eof())
		{
			inregistrare obj;
			obj.deserializare(f);

			//aici put conditia pt care se modifica
			if (obj.get_nr_campuri() != 0 && c2.get_valoare_coloana() == obj.get_valoare(index_cw))
			{
				obj.set_valoare(c1.get_valoare_coloana().c_str(), index_cu);
				k++;
			}
			obj.serializare(g);
		}
		f.close();
		g.close();
		cout << k << " inregistrari modificate." << endl;
		remove(nume_fisier.c_str());
		int r = rename("temp.bin", nume_fisier.c_str());
	}
}

void repository::afiseaza_toate(ofstream& out, int dimensiune)
{
	ifstream f(nume_fisier, ios::binary);
	if (!f.is_open())
	{
		return;
	}
	while (!f.eof())
	{
		inregistrare obj;
		obj.deserializare(f);
		if (obj.get_nr_campuri() != 0)
		{
			obj.afiseaza_inregistrare(out,dimensiune);
		}
	}
	f.close();
}

void repository::afiseaza_toate(ofstream& out,int dimensiune, conditie cond, int index_coloana_verificare)
{
	ifstream f(nume_fisier, ios::binary);
	if (!f.is_open())
	{
		return;
	}
	while (!f.eof())
	{
		inregistrare obj;
		obj.deserializare(f);
		if (obj.get_nr_campuri() != 0 && obj.get_valoare(index_coloana_verificare) == cond.get_valoare_coloana())
		{
			obj.afiseaza_inregistrare(out,dimensiune);
		}

	}
	f.close();
}

void repository::afiseaza_partial(ofstream& out, int dimensiune, int ind[], int nr)
{
	ifstream f(nume_fisier, ios::binary);
	if (!f.is_open())
	{
		return;
	}
	while (!f.eof())
	{
		inregistrare obj;
		obj.deserializare(f);

		if (obj.get_nr_campuri() != 0)
		{
			for (int i = 0; i < nr; i++)
			{
				cout << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(ind[i]);
				out << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(ind[i]);
			}
			cout << endl;
			out << endl;
		}
	}
	f.close();
}
void repository::afiseaza_partial(ofstream& out, int dimensiune, int ind[], int nr, int index_coloana_verificare, conditie cond)
{
	ifstream f(nume_fisier, ios::binary);
	if (!f.is_open())
	{
		return;
	}
	while (!f.eof())
	{
		inregistrare obj;
		obj.deserializare(f);
		if (obj.get_nr_campuri() != 0)
		{
			if (strcmp(obj.get_valoare(index_coloana_verificare).c_str(), cond.get_valoare_coloana().c_str()) == 0)
			{
				for (int i = 0; i < nr; i++)
				{
					cout << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(ind[i]);
					out << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(ind[i]);
					
				}
				cout << endl;
				out << endl;
			}
		}
	}
	f.close();

}

//COLOANA
//constructori
coloana::coloana()
{
	this->nume_coloana = nullptr;
	this->tip_coloana = tip_coloane::text;
	this->valoare_implicita = nullptr;
	this->dimensiune_coloana = 0;
}

coloana::coloana(const char* nume, tip_coloane tip, const char* val, int dim) : coloana()
{
	if (nume != nullptr)
	{
		this->nume_coloana = new char[strlen(nume) + 1];
		strcpy_s(this->nume_coloana, strlen(nume) + 1, nume);
	}

	this->tip_coloana = tip;

	if (val != nullptr)
	{
		this->valoare_implicita = new char[strlen(val) + 1];
		strcpy_s(this->valoare_implicita, strlen(val) + 1, val);
	}

	if (dim > 0)
	{
		this->dimensiune_coloana = dim;
	}
}

//cei trei
coloana::coloana(const coloana& c)
{
	if (c.nume_coloana != nullptr)
	{
		int n = strlen(c.nume_coloana);
		this->nume_coloana = new char[n + 1];
		strcpy_s(this->nume_coloana, n + 1, c.nume_coloana);
	}
	else
	{
		this->nume_coloana = nullptr;
	}

	this->tip_coloana = c.tip_coloana;

	if (c.valoare_implicita != nullptr)
	{
		int n = strlen(c.valoare_implicita);
		this->valoare_implicita = new char[n + 1];
		strcpy_s(this->valoare_implicita, n + 1, c.valoare_implicita);
	}
	else
	{
		this->valoare_implicita = nullptr;
	}

	if (c.dimensiune_coloana > 0)
	{
		this->dimensiune_coloana = c.dimensiune_coloana;
	}
	else
	{
		this->dimensiune_coloana = 0;
	}
}

coloana& coloana::operator=(const coloana& c)
{
	if (this->nume_coloana != nullptr)
	{
		delete[] this->nume_coloana;
	}

	if (this->valoare_implicita != nullptr)
	{
		delete[] this->valoare_implicita;
	}


	if (c.nume_coloana != nullptr)
	{
		int n = strlen(c.nume_coloana);
		this->nume_coloana = new char[n + 1];
		strcpy_s(this->nume_coloana, n + 1, c.nume_coloana);
	}

	this->tip_coloana = c.tip_coloana;

	if (c.valoare_implicita != nullptr)
	{
		int n = strlen(c.valoare_implicita);
		this->valoare_implicita = new char[n + 1];
		strcpy_s(this->valoare_implicita, n + 1, c.valoare_implicita);
	}

	if (c.dimensiune_coloana > 0)
	{
		this->dimensiune_coloana = c.dimensiune_coloana;
	}
	else
	{
		this->dimensiune_coloana = 0;
	}
	return *this;
}

coloana::~coloana()
{
	if (this->nume_coloana != nullptr)
	{
		delete[] this->nume_coloana;
	}

	if (this->valoare_implicita != nullptr)
	{
		delete[] this->valoare_implicita;
	}
}

//functii de citire si afisare consola
ostream& operator<<(ostream& out, coloana c)
{
	out << "Nume coloana: " << (c.nume_coloana != nullptr ? c.nume_coloana : "") << endl;

	out << "Tip coloana: ";
	if (c.tip_coloana == tip_coloane::integer) {
		out << "INTEGER";
	}
	else if (c.tip_coloana == tip_coloane::real)
	{
		out << "FLOAT";
	}
	else if (c.tip_coloana == tip_coloane::text)
	{
		out << "TEXT";
	}
	out << endl;

	out << "Valoare implicita: " << (c.valoare_implicita != nullptr ? c.valoare_implicita : "") << endl;
	out << "Dimensiune coloana: " << c.dimensiune_coloana << endl;
	return out;
}

//ar trebui sa fac si niste validari pe ce este introdus de la tastatura
istream& operator>>(istream& in, coloana& c)
{
	cout << "Dati numele coloanei: ";
	char buffer[100];
	in >> ws;
	in.get(buffer, 99);
	cout << endl;
	if (c.nume_coloana != nullptr)
	{
		delete[] c.nume_coloana;
	}
	c.nume_coloana = new char[strlen(buffer) + 1];
	strcpy_s(c.nume_coloana, strlen(buffer) + 1, buffer);


	cout << "Dati tipul coloanei: ";
	in >> ws;
	in.get(buffer, 99);
	cout << endl;
	if (_stricmp(buffer, "text") == 0)
	{
		c.tip_coloana = tip_coloane::text;
	}
	else if (_stricmp(buffer, "integer") == 0)
	{
		c.tip_coloana = tip_coloane::integer;
	}
	else if (_stricmp(buffer, "real") == 0)
	{
		c.tip_coloana = tip_coloane::real;
	}


	cout << "Dati valoare implicita: ";
	in >> ws;
	in.get(buffer, 99);
	cout << endl;
	if (c.valoare_implicita != nullptr)
	{
		delete[] c.valoare_implicita;
	}
	c.valoare_implicita = new char[strlen(buffer) + 1];
	strcpy_s(c.valoare_implicita, strlen(buffer) + 1, buffer);

	cout << "Dati dimensiunea coloanei: ";
	in >> c.dimensiune_coloana;
	cout << endl;

	return in;
}

//functii accesor
string coloana::get_nume_coloana()
{
	if (this->nume_coloana != nullptr)
	{
		return string(this->nume_coloana);
	}
	return string("");
}

//ar trebui sa fac si o validare ca numele sa fie un nume corect dpdv al sintaxei
void coloana::set_nume_coloana(const char* nume)
{
	if (this->nume_coloana != nullptr)
	{
		delete[] this->nume_coloana;
	}

	if (nume != nullptr)
	{
		int n = strlen(nume);
		this->nume_coloana = new char[n + 1];
		strcpy_s(this->nume_coloana, n + 1, nume);
	}
}

string coloana::get_tip_coloana()
{
	if (this->tip_coloana == tip_coloane::integer) {
		return "INTEGER";
	}
	else if (this->tip_coloana == tip_coloane::real)
	{
		return "FLOAT";
	}
	else if (this->tip_coloana == tip_coloane::text)
	{
		return "TEXT";
	}
	return "";
}

void coloana::set_tip_coloana(tip_coloane tip)
{
	this->tip_coloana = tip;
}

string coloana::get_valoare_implicita()
{
	if (this->valoare_implicita != nullptr)
		return string(valoare_implicita);
	return string("");
}

void coloana::set_valoare_implicita(const char* valoare)
{
	if (this->valoare_implicita != nullptr)
	{
		delete[] this->valoare_implicita;
	}

	if (valoare != nullptr)
	{
		int n = strlen(valoare);
		this->valoare_implicita = new char[n + 1];
		strcpy_s(this->valoare_implicita, n + 1, valoare);
	}
}

int coloana::get_dimensiune_coloana()
{
	return dimensiune_coloana;
}

void coloana::set_dimensiune_coloana(int dimensiune)
{
	if (dimensiune > 0)
	{
		this->dimensiune_coloana = dimensiune;
	}
	else
	{
		this->dimensiune_coloana = 0;
	}
}



//TABELA
//constructori
tabela::tabela()
{
	nume_tabela = nullptr;
	nr_coloane = 0;
	coloane = nullptr;
	repo = repository();
}

tabela::tabela(const char* nume, int nr_c, coloana col[]) : tabela()
{
	if (nume != nullptr)
	{
		this->nume_tabela = new char[strlen(nume) + 1];
		strcpy_s(this->nume_tabela, strlen(nume) + 1, nume);
		repo = repository(nume_tabela);
	}
	if (nr_c > 0 && col != nullptr)
	{
		this->nr_coloane = nr_c;
		this->coloane = new coloana[nr_c];
		for (int i = 0; i < nr_c; i++)
		{
			this->coloane[i] = col[i];
		}
	}
}


//regula celor trei
tabela::tabela(const tabela& t)
{
	if (t.nume_tabela != nullptr)
	{
		int n = strlen(t.nume_tabela);
		this->nume_tabela = new char[n + 1];
		strcpy_s(this->nume_tabela, n + 1, t.nume_tabela);
	}
	else
	{
		this->nume_tabela = nullptr;
	}

	if (t.nr_coloane > 0 && t.coloane != nullptr)
	{
		this->nr_coloane = t.nr_coloane;
		this->coloane = new coloana[nr_coloane];
		for (int i = 0; i < t.nr_coloane; i++)
		{
			this->coloane[i] = t.coloane[i];
		}
	}
	else
	{
		this->nr_coloane = 0;
	}

	this->repo = t.repo;
}

tabela& tabela::operator=(const tabela& t)
{
	if (nume_tabela != nullptr)
	{
		delete[] nume_tabela;
	}

	if (t.nume_tabela != nullptr)
	{
		int n = strlen(t.nume_tabela);
		this->nume_tabela = new char[n + 1];
		strcpy_s(this->nume_tabela, n + 1, t.nume_tabela);
	}
	else
	{
		this->nume_tabela = nullptr;
	}

	if (this->coloane != nullptr)
	{
		delete[] this->coloane;
		this->nr_coloane = 0;
	}

	if (t.nr_coloane > 0 &&  t.coloane!=nullptr)
	{
		this->nr_coloane = t.nr_coloane;
		this->coloane = new coloana[nr_coloane];
		for (int i = 0; i < t.nr_coloane; i++)
			this->coloane[i] = t.coloane[i];
	}
	else
	{
		this->nr_coloane = 0;
	}

	this->repo = t.repo;

	return *this;
}

tabela::~tabela()
{
	if (nume_tabela != nullptr)
	{
		delete[] nume_tabela;
	}
	if (coloane != nullptr)
	{
		delete[] coloane;
	}
}

//operatori de citire si afisare consola
ostream& operator<<(ostream& out, tabela t)
{
	out << "Nume tabela: " << (t.nume_tabela != nullptr ? t.nume_tabela : "") << endl;
	out << "Nr coloane: " << t.nr_coloane << endl;
	if (t.nr_coloane > 0)
	{
		out << "Coloane:" << endl;
		for (int i = 0; i < t.nr_coloane; i++)
		{
			out << "Coloana " << i + 1 << endl;
			out << t.coloane[i];
		}
	}
	return out;
}

istream& operator>>(istream& in, tabela& t)
{
	char buffer[100];
	in >> ws;
	in.get(buffer, 99);
	t.nume_tabela = new char[strlen(buffer) + 1];
	strcpy_s(t.nume_tabela, strlen(buffer) + 1, buffer);

	cout << "Dati nr de coloane: ";
	in >> t.nr_coloane;

	if (t.coloane != nullptr)
	{
		delete[] t.coloane;
	}

	if (t.nr_coloane > 0)
	{
		t.coloane = new coloana[t.nr_coloane];
	}

	for (int i = 0; i < t.nr_coloane; i++)
	{
		in >> t.coloane[i];
	}

	return in;
}

//functii accesor
string tabela::get_nume_tabela()
{
	if (this->nume_tabela)
	{
		return string(this->nume_tabela);
	}
	return string("");
}

void tabela::set_nume_tabela(const char* nume)
{
	if (this->nume_tabela != nullptr)
	{
		delete[] this->nume_tabela;
	}

	if (nume != nullptr)
	{
		this->nume_tabela = new char[strlen(nume) + 1];
		strcpy_s(this->nume_tabela, strlen(nume) + 1, nume);
	}
}

int tabela::get_nr_coloane()
{
	return this->nr_coloane;
}

coloana tabela::get_coloana(int index)
{
	//validare pentru index sa existe; sa fie mai mic decat nr_c
	if (index >= 0 && index <= nr_coloane)
	{
		return coloane[index];
	}
	else
	{
		throw exception("Se incearca citirea din afara limitelor vectorului");
	}
}

coloana tabela::get_coloana(const char* nume_coloana)
{
	if (nume_coloana != nullptr)
	{
		for (int i = 0; i < nr_coloane; i++)
		{
			if (_stricmp(nume_coloana, coloane[i].get_nume_coloana().c_str()) == 0)
			{
				return coloane[i];
			}
		}
	}
	throw exception(("Nu exista coloana" + string(nume_coloana)).c_str());
}

void tabela::set_coloana(coloana c, int index)
{
	//validare sa fie nrc intre 1 si 10
	if (index >= 0 && index <= nr_coloane)
	{
		coloane[index] = c;
	}
	else 
	{
		throw exception("Se incearca modificarea unei valori din exteriorul vectorului");
	}
}

int tabela::get_index_coloana(const char* nume_coloana)
{
	if (nume_coloana != nullptr)
	{
		for (int i = 0; i < nr_coloane; i++)
		{
			if (_stricmp(nume_coloana, coloane[i].get_nume_coloana().c_str()) == 0)
			{
				return i;
			}
		}
	}
	return -1;
}

//alti operatori
//alte supraincarcari de operatori
bool tabela::operator!()
{
	return nr_coloane > 0;
}

//tabela fara ultima inregistrare;
//tabela tabela::operator--()
//{
//	if (nr_inregistrari > 0)
//	{
//		this->nr_inregistrari--;
//	}
//	return *this;
//}

//tabela tabela::operator--(int i)
//{
//	tabela copie = *this;
//	this->nr_inregistrari--;
//	return copie;
//}

//tabela tabela::operator-(int valoare)
//{
//	if (valoare > 0)
//	{
//		nr_inregistrari = nr_inregistrari > valoare ? nr_inregistrari - valoare : 0;
//	}
//	else
//	{
//		throw 10; // cod eroare valoare negativa
//	}
//}

tabela::operator int()
{
	return nr_coloane;
}

//bool tabela::operator<(tabela t)
//{
//	return this->nr_inregistrari < t.nr_inregistrari;
//}

//alte functii
void tabela::afiseaza_structura_tabela()
{
	const int dimensiune = 30;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_display();

	ofstream out(nume_fisier, ios::trunc);

	//la consola
	cout << left << setw(dimensiune) << setfill(separator) << "Nume";
	cout << left << setw(dimensiune) << setfill(separator) << "Tip";
	cout << left << setw(dimensiune) << setfill(separator) << "Dimensiune";
	cout << left << setw(dimensiune) << setfill(separator) << "Valoare implicita";
	cout << endl;

	//in fisiere
	out << left << setw(dimensiune) << setfill(separator) << "Nume";
	out << left << setw(dimensiune) << setfill(separator) << "Tip";
	out << left << setw(dimensiune) << setfill(separator) << "Dimensiune";
	out << left << setw(dimensiune) << setfill(separator) << "Valoare implicita";
	out << endl;

	for (int i = 0; i < 4 * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	for (int i = 0; i < nr_coloane; i++)
	{
		coloana c = this->get_coloana(i);
		//la consola
		cout << left << setw(dimensiune) << setfill(separator) << c.get_nume_coloana();
		cout << left << setw(dimensiune) << setfill(separator) << c.get_tip_coloana();
		cout << left << setw(dimensiune) << setfill(separator) << c.get_dimensiune_coloana();
		cout << left << setw(dimensiune) << setfill(separator) << c.get_valoare_implicita();
		cout << endl;
		//in fisier
		out << left << setw(dimensiune) << setfill(separator) << c.get_nume_coloana();
		out << left << setw(dimensiune) << setfill(separator) << c.get_tip_coloana();
		out << left << setw(dimensiune) << setfill(separator) << c.get_dimensiune_coloana();
		out << left << setw(dimensiune) << setfill(separator) << c.get_valoare_implicita();
		out << endl;
	}
	out.close();
	cout << "Raport generat in fisierul " << nume_fisier << endl;
}



void tabela::adauga_inregistrare(inregistrare in)
{
	repo.adauga(in);
}

void tabela::sterge_inregistrare(conditie cond)
{
	int index;

	for (index = 0; index < nr_coloane; index++)
	{
		if (cond.get_nume_coloana() == coloane[index].get_nume_coloana())
		{
			break;
		}
	}
	if (index != nr_coloane)
	{
		repo.sterge(index,cond);
	}
}

void tabela::actualizeaza_inregistrare(conditie c1, conditie c2)
{
	int index_cu;//index coloana update
	for (index_cu = 0; index_cu < nr_coloane; index_cu++)
	{
		if (c1.get_nume_coloana() == coloane[index_cu].get_nume_coloana())
		{
			break;
		}
	}

	int index_cw;//index coloana where
	for (index_cw = 0; index_cw < nr_coloane; index_cw++)
	{
		if (c2.get_nume_coloana() == coloane[index_cw].get_nume_coloana())
		{
			break;
		}
	}

	int k = 0;
	if (index_cu != nr_coloane && index_cw != nr_coloane)
	{
		repo.actualizeaza(index_cu, index_cw, c1, c2);
	}
	
}

void tabela::afiseaza_toate()
{
	//e posibil dimensiunea sa fie prea mica pentru unele inregistrari
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (int i = 0; i < nr_coloane; i++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << coloane[i].get_nume_coloana();
		out << left << setw(dimensiune) << setfill(separator) << coloane[i].get_nume_coloana();
	}

	cout << endl;
	out << endl;
	for (int i = 0; i < nr_coloane * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	repo.afiseaza_toate(out,dimensiune);
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;
}

void tabela::afiseaza_toate(conditie cond)
{
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (int i = 0; i < nr_coloane; i++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << coloane[i].get_nume_coloana();
		out << left << setw(dimensiune) << setfill(separator) << coloane[i].get_nume_coloana();
	}

	cout << endl;
	out << endl;
	for (int i = 0; i < nr_coloane * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	int index_coloana_verificare = this->get_index_coloana(cond.get_nume_coloana().c_str());
	repo.afiseaza_toate(out, dimensiune, cond, index_coloana_verificare);
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;
}

void tabela::afiseaza_partial(string* p_nume_coloane, int p_nr_coloane)
{
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (int i = 0; i < p_nr_coloane; i++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << p_nume_coloane[i];
		out << left << setw(dimensiune) << setfill(separator) << p_nume_coloane[i];
	}

	cout << endl;
	out << endl;

	for (int i = 0; i < p_nr_coloane * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	int ind_col[20];
	for (int i = 0; i < p_nr_coloane; i++)
	{
		ind_col[i] = this->get_index_coloana(p_nume_coloane[i].c_str());
		
	}

	repo.afiseaza_partial(out, dimensiune, ind_col, p_nr_coloane);
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;
	
}

void tabela::afiseaza_partial(string* p_nume_coloane, int p_nr_coloane, conditie cond)
{
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (int i = 0; i < p_nr_coloane; i++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << p_nume_coloane[i];
		out << left << setw(dimensiune) << setfill(separator) << p_nume_coloane[i];
	}

	cout << endl;
	out << endl;

	for (int i = 0; i < p_nr_coloane * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	int index_coloana_verificare = this->get_index_coloana(cond.get_nume_coloana().c_str());

	int ind_col[20];
	for (int i = 0; i < p_nr_coloane; i++)
	{
		ind_col[i] = this->get_index_coloana(p_nume_coloane[i].c_str());

	}

	repo.afiseaza_partial(out, dimensiune, ind_col, p_nr_coloane,index_coloana_verificare, cond);
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;
}


//trebuie facut to upper inainte pe parametru
bool tabela::exista_coloana(string nume_col)
{
	for (int i = 0; i < nr_coloane; i++)
	{
		if (coloane[i].get_nume_coloana() == nume_col)
		{
			return true;
		}
	}
	return false;
}

//BAZA DE DATE
//apelator pentru constructor
baza_de_date::baza_de_date()
{
	nr_tabele = 0;
	rapoarte_select = 0;
	rapoarte_display = 0;
}

baza_de_date* baza_de_date::get_instanta()
{
	if(instanta == nullptr) 
	{
		baza_de_date::instanta = new baza_de_date();
		instanta->citeste_config();
	}
	return instanta;
}

void baza_de_date::citeste_config()
{
	ifstream f(FISIER_CONFIGURATIE);
	if (f.is_open())
	{
		int count = 0;
		string temp;
		getline(f, temp);
		rapoarte_select = stoi(temp);
		getline(f, temp);
		rapoarte_display = stoi(temp);

		while (!f.eof())
		{
			string nume;
			getline(f, nume);
			if (nume != "")
			{
				ifstream g(nume + ".config", ios::binary);

				tabele[count++].deserializare(g);
				g.close();
			}
		}
		instanta->nr_tabele = count;
		f.close();
	}
	else
	{
		ofstream g(FISIER_CONFIGURATIE);
		g << "0" << endl << "0" << endl;//automat pornesc de la 0 numaratoarea pentru fisiere select si display
		g.close();
		citeste_config();
	}
}

//functii accesor
int baza_de_date::get_nr_tabele()
{
	return nr_tabele;
}

tabela baza_de_date::get_tabela(int index)
{
	if (index < 0 || index >= nr_tabele)
	{
		throw 2;
	}
	return tabele[index];
}

//daca nu exista tabela in baza de date returneaza o tabela cu valorile implicite, dar sigur exista tabela
tabela baza_de_date::get_tabela(const char* nume_tabela)
{
	if (nume_tabela != nullptr)
	{
		for (int i = 0; i < nr_tabele; i++)
		{
			if (_stricmp(nume_tabela, tabele[i].get_nume_tabela().c_str()) == 0)
			{
				return tabele[i];
			}
		}
	}
	return tabela();
}

void baza_de_date::set_tabela(tabela t, int index)
{
	//trebuie verificare pe index sa fie in nr maxim de tabele
	tabele[index] = t;
}

//functii pt generare nume pt fisierele cu rapoarte comenzi
string baza_de_date::get_nume_fisier_select()
{
	string nume = NUME_FISIER_SELECT + to_string(rapoarte_select) + ".txt";
	rapoarte_select++;

	ifstream in(FISIER_CONFIGURATIE);
	ofstream out("temp.txt", ios::trunc);

	char linie[1000];
	if (in.is_open())
	{
		int i = 0;
		while (!in.eof())
		{
			in >> ws;
			in.get(linie, 999);
			if(i==INDEX_SELECT)
			{
				out << rapoarte_select << endl;
			}
			else
			{
				out << linie << endl;
			}
			i++;
		}
	}
	in.close();
	out.close();
	remove(FISIER_CONFIGURATIE);
	int r = rename("temp.txt", FISIER_CONFIGURATIE);

	return nume;
}

string baza_de_date::get_nume_fisier_display()
{
	string nume = NUME_FISIER_DISPLAY + to_string(rapoarte_display) + ".txt";
	rapoarte_display++;

	ifstream in(FISIER_CONFIGURATIE);
	ofstream out("temp.txt", ios::trunc);

	char linie[1000];
	if (in.is_open())
	{
		int i = 0;
		while (!in.eof())
		{
			in >> ws;
			in.get(linie, 999);
			if (i == INDEX_DISPLAY)
			{
				out << rapoarte_display << endl;
			}
			else
			{
				out << linie << endl;
			}
			i++;
		}
	}
	in.close();
	out.close();
	remove(FISIER_CONFIGURATIE);
	int r = rename("temp.txt", FISIER_CONFIGURATIE);

	return nume;
}



//functii de care o sa mai am nevoie -- DDL
void baza_de_date::adauga_tabela(tabela t)
{
	//trebuie sa adaug tabela in vectorul cu tabele (tabela ce are doar nume si vector de coloane);
	this->tabele[nr_tabele] = t;
	this->nr_tabele++;
	//trebuie sa adaug tabela in fisierul cu numele tabelelor
	ofstream out(FISIER_CONFIGURATIE, ios::app);
	out << t.get_nume_tabela() << endl;
	out.close();
	//trebuie sa creez un fisier nou cu numele nume_tabela.config pentru serializarea coloanelor
	out.open(t.get_nume_tabela() + ".config", ios::binary);
	t.serializare(out);
	out.close();
	out.open(t.get_nume_tabela() + ".bin", ios::binary|ios::trunc);
	out.close();
}

void baza_de_date::sterge_tabela(const char* nume_tabela)
{
	//trebuie sa scot tabela din vectorul cu tabele
	for (int i = 0; i < this->nr_tabele; i++)
	{
		if (_stricmp(nume_tabela, this->tabele[i].get_nume_tabela().c_str()) == 0)
		{
			for (int j = i; j < this->nr_tabele - 1; j++)
			{
				tabele[j] = tabele[j + 1];
			}
			this->nr_tabele--;
			break; // pentru ca nu pot sa existe 2 tabele cu acelasi nume
		}
	}

	//trebuie sa scot tabela din fisierul cu numele tabelelor
	ifstream in(FISIER_CONFIGURATIE);
	ofstream out("temp.txt", ios::trunc);

	char linie[1000];
	if (in.is_open())
	{
		while (!in.eof())
		{
			in >> ws;
			in.get(linie, 999);

			if (_stricmp(linie, nume_tabela) != 0)
			{
				out << linie << endl;
			}
		}
	}
	in.close();
	out.close();
	remove(FISIER_CONFIGURATIE);
	int r = rename("temp.txt", FISIER_CONFIGURATIE);

	//trebuie sa sterg fisierul cu numele nume_tabela.config 
	remove((string(nume_tabela)+".config").c_str());
	remove((string(nume_tabela) + ".bin").c_str());

}

void baza_de_date::afiseaza_tabela(const char* nume, int index)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].afiseaza_structura_tabela();
		}
	}
}

//functii pentru comenzi DML
void baza_de_date::insereaza_inregistrari(const char* nume_tabela, inregistrare inr)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume_tabela, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].adauga_inregistrare(inr);
			break;
		}
	}
}

void baza_de_date::sterge_inregistrari(const char* nume_tabela, conditie cond)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume_tabela, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].sterge_inregistrare(cond);
		}
	}
}

void baza_de_date::actualizeaza_inregistrari(const char* nume_tabela, conditie c1, conditie c2)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume_tabela, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].actualizeaza_inregistrare(c1, c2);
		}
	}
}

void baza_de_date::afiseaza_toate(const char* nume)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].afiseaza_toate();
			return;
		}
	}
}

void baza_de_date::afiseaza_toate(const char* nume, conditie cond)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].afiseaza_toate(cond);
			return;
		}
	}
}

void baza_de_date::afiseaza_partial(const char* nume, string* nume_coloane, int nr)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].afiseaza_partial(nume_coloane, nr);
			return;
		}
	}
}

void baza_de_date::afiseaza_partial(const char* nume, string* nume_coloane, int nr, conditie cond)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (_stricmp(nume, tabele[i].get_nume_tabela().c_str()) == 0)
		{
			tabele[i].afiseaza_partial(nume_coloane, nr, cond);
			return;
		}
	}
}

//functii de ajutor
//!!!trebuie apelata cu un to upper de token
bool baza_de_date::exista_tabela(string nume)
{
	for (int i = 0; i < nr_tabele; i++)
	{
		if (nume == tabele[i].get_nume_tabela())
		{
			return true;
		}
	}
	return false;
}

//functii pentru testare
void baza_de_date::listeaza_tabele()
{
	for (int i = 0; i < nr_tabele; i++)
	{
		cout << "->" << tabele[i].get_nume_tabela() << endl;
	}
}

//functii serializare/deserializare
//INREGISTRARE
void inregistrare::serializare(ofstream& f)
{
	long length;
	f.write((char*)&nr_campuri, sizeof(nr_campuri));
	for (int i = 0; i < nr_campuri; i++)
	{
		length = strlen(valori_campuri[i]);
		f.write((char*)&length, sizeof(length));
		f.write(valori_campuri[i], length + 1L);
	}
}

void inregistrare::deserializare(ifstream& f)
{
	long length = 0;
	if (valori_campuri != nullptr)
	{
		for (int i = 0; i < nr_campuri; i++)
		{
			delete[] valori_campuri[i];
		}
		delete[] valori_campuri;
	}

	f.read((char*)&nr_campuri, sizeof(nr_campuri));
	valori_campuri = new char* [nr_campuri];
	for (int i = 0; i < nr_campuri; i++)
	{
		f.read((char*)&length, sizeof(length));
		valori_campuri[i] = new char[length + 1];
		f.read(valori_campuri[i], length + 1L);
	}
}

//REPOSITORY
void repository::serializeaza(ofstream& f)
{
	long length;
	length = nume_fisier.length();

	f.write((char*)&length, sizeof(length));
	f.write(nume_fisier.c_str(), length + 1L);
}

void repository::deserializeaza(ifstream& f) 
{
	long length = 0;
	f.read((char*)&length, sizeof(length));
	char* tmp = new char[length + 1];
	f.read(tmp, length + 1L);
	nume_fisier = string(tmp);
	delete[] tmp;
}

//COLOANA
void coloana::serializare(ofstream& f)
{
	long length;
	length = strlen(nume_coloana);
	f.write((char*)&length, sizeof(length));
	f.write(nume_coloana, length + 1L);
	f.write((char*)&tip_coloana, sizeof(tip_coloana));
	length = strlen(valoare_implicita);
	f.write((char*)&length, sizeof(length));
	f.write(valoare_implicita, length + 1L);
	f.write((char*)&dimensiune_coloana, sizeof(dimensiune_coloana));
}

void coloana::deserializare(ifstream& f)
{
	if (nume_coloana != nullptr)
	{
		delete[] nume_coloana;
	}

	if (valoare_implicita != nullptr)
	{
		delete[] valoare_implicita;
	}


	long length = 0;
	f.read((char*)&length, sizeof(length));
	nume_coloana = new char[length + 1];
	f.read(nume_coloana, length + 1L);

	f.read((char*)&tip_coloana, sizeof(tip_coloana));

	f.read((char*)&length, sizeof(length));
	valoare_implicita = new char[length + 1];
	f.read(valoare_implicita, length + 1L);

	f.read((char*)&dimensiune_coloana, sizeof(dimensiune_coloana));
	
}

//TABELA
void tabela::serializare(ofstream& f)
{
	long length;
	length = strlen(nume_tabela);

	f.write((char*)&length, sizeof(length));
	f.write(nume_tabela,length + 1L);

	f.write((char*)&nr_coloane, sizeof(nr_coloane));
	for (int i = 0; i < nr_coloane; i++)
	{
		coloane[i].serializare(f);
	}
	repo.serializeaza(f);

}

void tabela::deserializare(ifstream& f)
{
	if (nume_tabela != nullptr)
	{
		delete[] nume_tabela;
	}
	
	long length = 0;
	f.read((char*)&length, sizeof(length));
	nume_tabela = new char[length + 1];
	f.read(nume_tabela , length + 1L);
	f.read((char*)&nr_coloane, sizeof(nr_coloane));
	if (nr_coloane > 0)
	{
		coloane = new coloana[nr_coloane];
	}
	for (int i = 0; i < nr_coloane; i++)
	{
		coloane[i].deserializare(f);
	}
	repo.deserializeaza(f);
}
