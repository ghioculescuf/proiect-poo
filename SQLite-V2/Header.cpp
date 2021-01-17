#include "Header.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cctype>
#include <iomanip>
#include <string.h>
#include <fstream>
#include <vector>
#include <set>
#include <list>
#include <map>
using namespace std;


//CLASA ABSTRACTA COMANDA
comanda::comanda()
{
	sir_verificare = nullptr;
	sir_executie = nullptr;
}

comanda::comanda(const char* c) : comanda()
{
	if (c != nullptr)
	{
		int n = strlen(c) + 1;
		sir_verificare = new char[n];
		sir_executie = new char[n];
		strcpy_s(sir_verificare, n, c);
		strcpy_s(sir_executie, n, c);
	}
}

comanda::~comanda()
{
	if (sir_executie != nullptr)
	{
		delete[] sir_executie;
	}
	if (sir_verificare != nullptr)
	{
		delete[] sir_verificare;
	}
}

//COMANDA CREATE
bool comanda_create::verifica_sintaxa()
{
	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant TABLE
	token = strtok_s(sir_verificare, " ", &context);
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
}
void comanda_create::executa_comanda()
{
	sir_executie[strlen(sir_executie) - 1] = '\0';

	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(sir_executie, " ", &context); //TABLE
	token = strtok_s(nullptr, " (", &context);

	procesor_comenzi::to_upper(token);
	string nume_t(token);

	char delims[] = "(),";

	list<coloana> coloane;
	token = strtok_s(nullptr, delims, &context);

	while (token != nullptr)
	{
		coloana c;


		procesor_comenzi::to_upper(token);
		c.set_nume_coloana(token);

		token = strtok_s(nullptr, delims, &context);
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
		c.set_dimensiune_coloana(atoi(token));

		token = strtok_s(nullptr, delims, &context);
		if (c.get_tip_coloana() == "TEXT")
		{
			procesor_comenzi::trim(token);
		}
		c.set_valoare_implicita(token);

		coloane.push_back(c);
		token = strtok_s(nullptr, delims, &context);
	}

	tabela t(nume_t.c_str(), coloane);

	baza_de_date::get_instanta()->adauga_tabela(t);

	cout << "Tabela " << nume_t << " a fost creata." << endl;
}

//COMANDA DISPLAY
bool comanda_display::verifica_sintaxa()
{

	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant TABLE
	token = strtok_s(sir_verificare, " ", &context);
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
void comanda_display::executa_comanda()
{
	sir_executie[strlen(sir_executie) - 1] = '\0';
	procesor_comenzi::to_upper(sir_executie + 6);
	baza_de_date::get_instanta()->afiseaza_tabela(sir_executie + 6);
}

//COMANDA DROP
bool comanda_drop::verifica_sintaxa()
{
	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant TABLE
	token = strtok_s(sir_verificare, " ", &context);
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
void comanda_drop::executa_comanda()
{
	sir_executie[strlen(sir_executie) - 1] = '\0';
	procesor_comenzi::to_upper(sir_executie + 6);
	baza_de_date::get_instanta()->sterge_tabela(sir_executie + 6);
	cout << "Tabela " << sir_executie + 6 << " a fost stearsa." << endl;
}


//COMANDA INSERT
bool comanda_insert::verifica_sintaxa()
{
	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant INTO
	token = strtok_s(sir_verificare, " ", &context);
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
void comanda_insert::executa_comanda()
{
	//INSERT INTO nume_tabela VALUES(...); valorile sunt separate prin, și au numărul și ordinea exacta ca definiția tabelului
	char* token = nullptr;
	char* context = nullptr;

	sir_executie[strlen(sir_executie) - 1] = '\0';//scot ;
	token = strtok_s(sir_executie, " ", &context);//INTO

	token = strtok_s(nullptr, " ", &context);//aici am numele tabelei
	cout << "Inseram in tabela " << token << endl;
	procesor_comenzi::to_upper(token);
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
bool comanda_delete::verifica_sintaxa()
{
	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;

	//verificare existenta cuvant FROM
	token = strtok_s(sir_verificare, " ", &context);
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
void comanda_delete::executa_comanda()
{
	sir_executie[strlen(sir_executie) - 1] = '\0';//scot ;

	//DELETE FROM nume_tabela WHERE nume_coloană = valoare (DELETE permite doar o coloană în clauza WHERE)
	char* token = nullptr;
	char* context = nullptr;

	token = strtok_s(sir_executie, " ", &context);//FROM
	token = strtok_s(nullptr, " ", &context);
	procesor_comenzi::to_upper(token);
	cout << "Delete pe tabela: " << token << endl;
	string nume_tabela(token);


	token = strtok_s(nullptr, " ", &context); //WHERE
	token = strtok_s(nullptr, "=", &context);

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
bool comanda_update::verifica_sintaxa()
{
	//UPDATE nume_tabela SET nume_coloană = valoare WHERE nume_coloană = valoare
	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;


	//verificari pentru numele tabelei -- e in context
	token = strtok_s(sir_verificare, " ", &context);
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
void comanda_update::executa_comanda()
{
	sir_executie[strlen(sir_executie) - 1] = '\0';//scot ;
	char* token = nullptr;
	char* context = nullptr;

	token = strtok_s(sir_executie, " ", &context);
	procesor_comenzi::to_upper(token);
	cout << "Update pe tabela: " << token << endl;
	procesor_comenzi::to_upper(token);
	string nume_tabela = token;

	token = strtok_s(nullptr, " ", &context);//aici e cuvantul SET

	conditie c1;
	token = strtok_s(nullptr, "=", &context);
	procesor_comenzi::to_upper(token);
	c1.set_nume_coloana(token);


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
bool comanda_select::verifica_sintaxa()
{
	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	bool coloane_specifice = false;
	procesor_comenzi::to_upper(sir_verificare, 3);
	char* restul_sirului = nullptr;
	string nume_coloane[20];
	int nr_coloane = 0;
	if (sir_verificare == strstr(sir_verificare, "ALL"))
	{
		restul_sirului = sir_verificare + 3;
	}

	if (sir_verificare[0] == '(' && strstr(sir_verificare, ")") != nullptr)
	{
		char* final_paranteze = strstr(sir_verificare, ")");
		restul_sirului = final_paranteze + 1;
		*final_paranteze = '\0';


		char* token = nullptr;
		char* context = nullptr;
		token = strtok_s(sir_verificare, " (),", &context);
		while (token != nullptr)
		{
			procesor_comenzi::to_upper(token);
			nume_coloane[nr_coloane++] = token;
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
void comanda_select::executa_comanda()
{

	sir_executie[strlen(sir_executie) - 1] = '\0';


	bool toate_coloanele = false;

	procesor_comenzi::to_upper(sir_executie, 3);
	char* restul_sirului = nullptr;
	vector<string> nume_coloane;
	if (sir_executie == strstr(sir_executie, "ALL"))
	{
		//atunci continui cu from where
		restul_sirului = sir_executie + 3;
		toate_coloanele = true;
	}

	if (sir_executie[0] == '(' && strstr(sir_executie, ")") != nullptr)
	{

		char* final_paranteze = strstr(sir_executie, ")");
		restul_sirului = final_paranteze + 1;
		*final_paranteze = '\0';


		char* token = nullptr;
		char* context = nullptr;

		token = strtok_s(sir_executie, " (),", &context);
		while (token != nullptr)
		{
			procesor_comenzi::to_upper(token);
			nume_coloane.push_back(token);
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
		procesor_comenzi::to_upper(token);
		procesor_comenzi::to_upper(context);
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
		baza_de_date::get_instanta()->afiseaza_partial(nume_tabela.c_str(), nume_coloane);
	}
	else if (toate_coloanele == false && exista_where == true)
	{
		baza_de_date::get_instanta()->afiseaza_partial(nume_tabela.c_str(), nume_coloane, cond);
	}
}


//COMANDA IMPORT
bool comanda_import::verifica_sintaxa()
{

	if (procesor_comenzi::este_comanda_incompleta(sir_verificare))
	{
		return false;
	}

	//verificare comanda se termina in ';'
	if (sir_verificare[strlen(sir_verificare) - 1] != ';')
	{
		cout << "Comanda nu se termina cu ;" << endl;
		return false;
	}
	sir_verificare[strlen(sir_verificare) - 1] = '\0';


	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(sir_verificare, " ", &context);
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
	procesor_comenzi::to_upper(token);
	tabela t = baza_de_date::get_instanta()->
		get_tabela(token);
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
void comanda_import::executa_comanda()
{
	//IMPORT nume_tabela nume_fisier.CSV
	char* token = nullptr;
	char* context = nullptr;
	//cout << c << endl;
	sir_executie[strlen(sir_executie) - 1] = '\0';//scot ;
	token = strtok_s(sir_executie, " ", &context);// in token e numele tabelei
	procesor_comenzi::to_upper(token);
	cout << "Importam in tabela " << token << endl;

	string nume_tabela(token);
	fstream f;
	f.open(context, ios::in);

	//citesc fiecare linie din csv
	multiset<string> m;
	while (!f.eof())
	{
		string linie;
		getline(f, linie);
		m.insert(linie);
	}

	//sparg fiecare linie in valori
	vector<string> v;
	for (auto it = m.begin(); it != m.end(); it++)
	{
		char linie[100];
		strcpy_s(linie, it->length() + 1, it->c_str());
		//cout << linie << endl;
		char* token1 = nullptr;
		char* context1 = nullptr;
		token1 = strtok_s(linie, ",", &context1);
		while (token1 != nullptr)
		{
			v.push_back(token1);
			token1 = strtok_s(nullptr, ",", &context1);
		}

		inregistrare inr(v);
		baza_de_date::get_instanta()->insereaza_inregistrari(nume_tabela.c_str(), inr);
		v.clear();
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

}

inregistrare::inregistrare(vector<string> v)
{
	valori_campuri = v;
}

inregistrare::inregistrare(int nr_cmp, char valori[100][100]) : inregistrare()
{
	if (nr_cmp > 0)
	{
		for (int i = 0; i < nr_cmp; i++)
		{
			valori_campuri.push_back(valori[i]);
		}
	}
}

//operatori de citire si afisare consola
istream& operator>>(istream& in, inregistrare& obj)
{
	cout << "Dati numarul de campuri: ";
	int nr_campuri;
	in >> nr_campuri;

	for (int i = 0; i < nr_campuri; i++)
	{
		cout << "Camp " << i;
		in >> ws;
		string tmp;
		getline(in, tmp);
		obj.valori_campuri.push_back(tmp);
		cout << endl;
	}
	return in;
}

ostream& operator<<(ostream& out, inregistrare obj)
{
	out << "Valori campuri:" << endl;

	for (unsigned int i = 0; i < obj.valori_campuri.size(); i++)
	{
		out << obj.valori_campuri[i] << " ";
	}
	out << endl;

	return out;
}

//functii fisiere binare
void inregistrare::serializare(ofstream& f)
{
	long length = 0;

	int nr_campuri = valori_campuri.size();
	f.write((char*)&nr_campuri, sizeof(nr_campuri));

	for (int i = 0; i < nr_campuri; i++)
	{
		length = valori_campuri[i].length();
		f.write((char*)&length, sizeof(length));
		f.write(valori_campuri[i].c_str(), length + 1L);
	}
}

void inregistrare::deserializare(ifstream& f)
{
	valori_campuri.clear();

	int nr_campuri;
	f.read((char*)&nr_campuri, sizeof(nr_campuri));

	for (int i = 0; i < nr_campuri; i++)
	{
		long length = 0;
		f.read((char*)&length, sizeof(length));
		char* tmp = new char[length + 1];
		f.read(tmp, length + 1L);
		valori_campuri.push_back(tmp);
		delete[] tmp;
	}
}

//functii accesor
int inregistrare::get_nr_campuri()
{
	return valori_campuri.size();
}

void inregistrare::set_valoare(string val, unsigned int index)
{
	if (index >= 0 && index < valori_campuri.size())
	{
		valori_campuri[index] = val;
	}
	else
	{
		throw exception("Index vector nevalid.");
	}
}

void inregistrare::set_valori(vector<string> v)
{
	valori_campuri = v;
}

string inregistrare::get_valoare(unsigned int index)
{
	if (index >= 0 && index < valori_campuri.size())
	{
		return valori_campuri[index];
	}
	else
	{
		throw exception("Index vector nevalid");
	}
}

vector<string> inregistrare::get_valori()
{
	return valori_campuri;
}

//alti operatori
bool inregistrare::operator!()
{
	return valori_campuri.size() > 0;
}

inregistrare::operator int()
{
	return valori_campuri.size();
}

bool inregistrare::operator>=(inregistrare inr)
{
	return this->valori_campuri.size() >= inr.valori_campuri.size();
}

string inregistrare::operator[](unsigned int index)
{
	if (index >= 0 && index < valori_campuri.size())
	{
		return valori_campuri[index];
	}
	else
	{
		throw exception("Index vector nevalid");
	}
}

//alte functii
void inregistrare::afiseaza_inregistrare(ofstream& out, int dimensiune)
{
	for (unsigned int i = 0; i < valori_campuri.size(); i++)
	{
		cout << left << setw(dimensiune) << setfill(' ') << valori_campuri[i];
		out << left << setw(dimensiune) << setfill(' ') << valori_campuri[i];
	}
	cout << endl;
	out << endl;
}

void inregistrare::scrie_rand_csv(ofstream& out)
{
	for (unsigned int i = 0; i < valori_campuri.size() - 1; i++)
	{
		
		out << valori_campuri[i] << ",";
	}
	out << valori_campuri[valori_campuri.size() - 1]<<endl;
	out << endl;
}


//REPOSITORY
repository::repository() {}
repository::repository(string nume_fisier)
{
	this->nume_fisier = string(nume_fisier) + ".bin";
}

void repository::adauga(inregistrare obj)
{
	ofstream f(nume_fisier, ios::binary | ios::app);
	obj.serializare(f);
	f.close();
}

void repository::sterge(int index, string valoare)
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
				if (valoare != obj.get_valoare(index))
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

void repository::actualizeaza(int index_cu, int index_cw, string v1, string v2)
{
	ifstream f(nume_fisier, ios::binary);
	ofstream g("temp.bin", ios::binary | ios::trunc);
	int k = 0;
	if (f.is_open()) {
		while (!f.eof())
		{
			inregistrare obj;
			obj.deserializare(f);

			if (obj.get_nr_campuri() != 0 && v2 == obj.get_valoare(index_cw))
			{
				obj.set_valoare(v1, index_cu);
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

void repository::exporta_csv(ofstream& out)
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
			obj.scrie_rand_csv(out);
		}
	}
	f.close();
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
			obj.afiseaza_inregistrare(out, dimensiune);
		}
	}
	f.close();
}

void repository::afiseaza_toate(ofstream& out, int dimensiune, string valoare, int index_coloana_verificare)
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
		if (obj.get_nr_campuri() != 0 && obj.get_valoare(index_coloana_verificare) == valoare)
		{
			obj.afiseaza_inregistrare(out, dimensiune);
		}

	}
	f.close();
}

void repository::afiseaza_partial(ofstream& out, int dimensiune, vector<int> indici)
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
			for (unsigned int i = 0; i < indici.size(); i++)
			{
				cout << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(indici[i]);
				out << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(indici[i]);
			}
			cout << endl;
			out << endl;
		}
	}
	f.close();
}

void repository::afiseaza_partial(ofstream& out, int dimensiune, vector<int> indici, int index_coloana_verificare, string valoare)
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
			if (obj.get_valoare(index_coloana_verificare) == valoare)
			{
				for (unsigned int i = 0; i < indici.size(); i++)
				{
					cout << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(indici[i]);
					out << left << setw(dimensiune) << setfill(' ') << obj.get_valoare(indici[i]);

				}
				cout << endl;
				out << endl;
			}
		}
	}
	f.close();
}

void repository::serializare(ofstream& f)
{
	long length;
	length = nume_fisier.length();

	f.write((char*)&length, sizeof(length));
	f.write(nume_fisier.c_str(), length + 1L);
}

void repository::deserializare(ifstream& f)
{
	long length = 0;
	f.read((char*)&length, sizeof(length));
	char* tmp = new char[length + 1];
	f.read(tmp, length + 1L);
	nume_fisier = string(tmp);
	delete[] tmp;
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

//regula celor trei
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

//functii fisiere binare
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

//functii accesor
string coloana::get_nume_coloana()
{
	if (this->nume_coloana != nullptr)
	{
		return string(this->nume_coloana);
	}
	return string("");
}

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
	nume_tabela = "";
	repo = repository();
}

tabela::tabela(const char* nume, list<coloana> col) : tabela()
{
	if (nume != nullptr)
	{
		nume_tabela = nume;
		repo = repository(nume_tabela);
	}

	coloane = col;
}

//operatori de citire si afisare consola
ostream& operator<<(ostream& out, tabela t)
{
	out << "Nume tabela: " << t.nume_tabela << endl;

	if (t.coloane.size() > 0)
	{
		out << "Coloane:" << endl;
		for (auto it = t.coloane.begin(); it != t.coloane.end(); it++)
		{
			out << *it << endl;
		}
	}
	return out;
}

istream& operator>>(istream& in, tabela& t)
{
	in >> ws;
	getline(in, t.nume_tabela);

	cout << "Dati nr de coloane: ";
	int nr_coloane;
	in >> nr_coloane;

	for (int i = 0; i < nr_coloane; i++)
	{
		coloana c;
		in >> c;
		t.coloane.push_back(c);
	}

	return in;
}

//functii fisiere binare
void tabela::serializare(ofstream& f)
{
	long length = nume_tabela.length();

	f.write((char*)&length, sizeof(length));
	f.write(nume_tabela.c_str(), length + 1L);

	int nr_coloane = coloane.size();
	f.write((char*)&nr_coloane, sizeof(nr_coloane));
	for (auto it = coloane.begin(); it != coloane.end(); it++)
	{
		it->serializare(f);
	}

	repo.serializare(f);

}

void tabela::deserializare(ifstream& f)
{

	long length = 0;
	f.read((char*)&length, sizeof(length));

	char* tmp = new char[length + 1];
	f.read(tmp, length + 1L);
	nume_tabela = string(tmp);
	delete[] tmp;


	int nr_coloane;
	f.read((char*)&nr_coloane, sizeof(nr_coloane));
	for (int i = 0; i < nr_coloane; i++)
	{
		coloana c;
		c.deserializare(f);
		coloane.push_back(c);
	}
	repo.deserializare(f);
}

//functii accesor
string tabela::get_nume_tabela()
{
	return nume_tabela;

}

coloana tabela::get_coloana(unsigned int index)
{
	if (index < 0 || index >= coloane.size())
	{
		throw exception("Index-ul nu este valid");
	}

	auto it = coloane.begin();
	advance(it, index);
	return *it;
}

coloana tabela::get_coloana(string nume_col)
{
	for (auto it = coloane.begin(); it != coloane.end(); it++)
	{
		if (it->get_nume_coloana() == nume_col)
			return *it;
	}
	throw exception("Nu exista coloana cautata");
}

void tabela::set_nume_tabela(const char* nume)
{
	if (nume != nullptr && !baza_de_date::get_instanta()->exista_tabela(nume))
	{
		nume_tabela = nume;
	}
}

int tabela::get_nr_coloane()
{
	return coloane.size();
}

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

	for (auto it = coloane.begin(); it != coloane.end(); it++)
	{
		coloana c = *it;
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
	int index = get_index_conditie(cond.get_nume_coloana());
	if (index != coloane.size())
	{
		repo.sterge(index, cond.get_valoare_coloana());
	}
}

void tabela::actualizeaza_inregistrare(conditie c1, conditie c2)
{
	int index_cu = get_index_conditie(c1.get_nume_coloana()); //index coloana update
	int index_cw = get_index_conditie(c2.get_nume_coloana()); //index coloana where
	if (index_cu != coloane.size() && index_cw != coloane.size())
	{
		repo.actualizeaza(index_cu, index_cw, c1.get_valoare_coloana(), c2.get_valoare_coloana());
	}

}

void tabela::afiseaza_toate()
{
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (auto it = coloane.begin(); it != coloane.end(); it++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << (*it).get_nume_coloana();
		out << left << setw(dimensiune) << setfill(separator) << (*it).get_nume_coloana();
	}

	cout << endl;
	out << endl;

	for (unsigned int i = 0; i < coloane.size() * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	repo.afiseaza_toate(out, dimensiune);
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;
}

void tabela::afiseaza_toate(conditie cond)
{
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (auto it = coloane.begin(); it != coloane.end(); it++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << (*it).get_nume_coloana();
		out << left << setw(dimensiune) << setfill(separator) << (*it).get_nume_coloana();
	}

	cout << endl;
	out << endl;

	for (unsigned int i = 0; i < coloane.size() * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	int index_coloana_verificare = this->get_index_conditie(cond.get_nume_coloana());
	repo.afiseaza_toate(out, dimensiune, cond.get_valoare_coloana(), index_coloana_verificare);
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;
}

void tabela::afiseaza_partial(vector<string> nume_coloane)
{
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (unsigned int i = 0; i < nume_coloane.size(); i++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << nume_coloane[i];
		out << left << setw(dimensiune) << setfill(separator) << nume_coloane[i];
	}

	cout << endl;
	out << endl;

	for (unsigned int i = 0; i < nume_coloane.size() * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	vector<int> ind_col;
	for (unsigned int i = 0; i < nume_coloane.size(); i++)
	{
		ind_col.push_back(this->get_index_conditie(nume_coloane[i]));
	}

	repo.afiseaza_partial(out, dimensiune, ind_col);
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;

}

void tabela::afiseaza_partial(vector<string> nume_coloane, conditie cond)
{
	const int dimensiune = 20;
	const char separator = ' ';

	string nume_fisier = baza_de_date::get_instanta()->get_nume_fisier_select();
	ofstream out(nume_fisier, ios::trunc);

	for (unsigned int i = 0; i < nume_coloane.size(); i++)
	{
		cout << left << setw(dimensiune) << setfill(separator) << nume_coloane[i];
		out << left << setw(dimensiune) << setfill(separator) << nume_coloane[i];
	}

	cout << endl;
	out << endl;

	for (unsigned int i = 0; i < nume_coloane.size() * dimensiune; i++)
	{
		cout << "-";
		out << "-";
	}

	cout << endl;
	out << endl;

	int index_coloana_verificare = this->get_index_conditie(cond.get_nume_coloana());

	vector<int> ind_col;
	for (unsigned int i = 0; i < nume_coloane.size(); i++)
	{
		ind_col.push_back(this->get_index_conditie(nume_coloane[i]));
	}

	repo.afiseaza_partial(out, dimensiune, ind_col, index_coloana_verificare, cond.get_valoare_coloana());
	out.close();
	cout << "Raportul se afla in fisierul " << nume_fisier << endl;
}

int tabela::get_index_conditie(string nume_col)
{
	int index = 0;
	for (auto it = coloane.begin(); it != coloane.end(); it++)
	{
		if (it->get_nume_coloana() == nume_col)
		{
			break;
		}
		index++;
	}
	return index;
}

void tabela::exporta_csv(ofstream& out)
{
	repo.exporta_csv(out);
}

//trebuie facut to upper inainte pe parametru
bool tabela::exista_coloana(string nume_col)
{
	for (auto it = coloane.begin(); it != coloane.end(); it++)
	{
		if ((*it).get_nume_coloana() == nume_col)
		{
			return true;
		}
	}
	return false;
}

//alti operatori
bool tabela::operator!()
{
	return coloane.size() > 0;
}

tabela::operator int()
{
	return coloane.size();
}

bool operator<(tabela t1, tabela t2)
{
	return  t1.coloane.size() < t2.coloane.size();
}


//BAZA DE DATE
//membrii statici
baza_de_date* baza_de_date::instanta = nullptr;

baza_de_date::baza_de_date()
{
	rapoarte_select = 0;
	rapoarte_display = 0;
}

void baza_de_date::citeste_config()
{
	ifstream f(FISIER_CONFIGURATIE);
	if (f.is_open())
	{
		string temp;
		getline(f, temp);
		rapoarte_select = stoi(temp);
		getline(f, temp);
		rapoarte_display = stoi(temp);

		//adaugare tabele din fisiere de configuratie cu structura
		while (!f.eof())
		{
			string nume;
			getline(f, nume);
			if (nume != "")
			{
				ifstream g(nume + ".config", ios::binary);
				tabela t;
				t.deserializare(g);
				tabele.insert(pair<string, tabela>(nume, t));
				g.close();
			}
		}
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

baza_de_date* baza_de_date::get_instanta()
{
	if (instanta == nullptr)
	{
		baza_de_date::instanta = new baza_de_date();
		instanta->citeste_config();
	}
	return instanta;
}

tabela baza_de_date::get_tabela(const char* nume_tabela)
{
	auto it = tabele.find(nume_tabela);
	if (it != tabele.end())
	{
		return it->second;
	}
	throw exception("Nu exista tabela cautata");
}

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
			if (i == INDEX_SELECT)
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


//functii DDL
void baza_de_date::adauga_tabela(tabela t)
{
	tabele.insert(pair<string, tabela>(t.get_nume_tabela(), t));

	//fisier cu numele tabelelor
	ofstream out(FISIER_CONFIGURATIE, ios::app);
	out << t.get_nume_tabela() << endl;
	out.close();

	//fisier cu structura coloanelor
	out.open(t.get_nume_tabela() + ".config", ios::binary);
	t.serializare(out);
	out.close();

	//fisier cu date
	out.open(t.get_nume_tabela() + ".bin", ios::binary | ios::trunc);
	out.close();
}

void baza_de_date::sterge_tabela(const char* nume_tabela)
{
	tabele.erase(nume_tabela);

	//scot tabela din fisierul cu numele tabelelor
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

	//sterg fisierele cu structura coloanelor si cu inregistrari 
	remove((string(nume_tabela) + ".config").c_str());
	remove((string(nume_tabela) + ".bin").c_str());

}

void baza_de_date::afiseaza_tabela(const char* nume)
{
	auto it = tabele.find(nume);
	if (it != tabele.end())
	{
		it->second.afiseaza_structura_tabela();
	}
}

//functii DML
void baza_de_date::insereaza_inregistrari(const char* nume_tabela, inregistrare inr)
{
	auto it = tabele.find(nume_tabela);
	if (it != tabele.end())
	{
		it->second.adauga_inregistrare(inr);
	}
}

void baza_de_date::sterge_inregistrari(const char* nume_tabela, conditie cond)
{
	auto it = tabele.find(nume_tabela);
	if (it != tabele.end())
	{
		it->second.sterge_inregistrare(cond);
	}
}

void baza_de_date::actualizeaza_inregistrari(const char* nume_tabela, conditie c1, conditie c2)
{
	auto it = tabele.find(nume_tabela);
	if (it != tabele.end())
	{
		it->second.actualizeaza_inregistrare(c1, c2);
	}
}

void baza_de_date::afiseaza_toate(const char* nume)
{
	auto it = tabele.find(nume);
	if (it != tabele.end())
	{
		it->second.afiseaza_toate();
	}
}

void baza_de_date::afiseaza_toate(const char* nume, conditie cond)
{
	auto it = tabele.find(nume);
	if (it != tabele.end())
	{
		it->second.afiseaza_toate(cond);
	}
}

void baza_de_date::afiseaza_partial(const char* nume, vector<string> nume_coloane)
{
	auto it = tabele.find(nume);
	if (it != tabele.end())
	{
		it->second.afiseaza_partial(nume_coloane);
	}
}

void baza_de_date::afiseaza_partial(const char* nume, vector<string> nume_coloane, conditie cond)
{
	auto it = tabele.find(nume);
	if (it != tabele.end())
	{
		it->second.afiseaza_partial(nume_coloane, cond);
	}
}

//alte functii
//trebuie apelata cu un to_upper(token)
bool baza_de_date::exista_tabela(string nume)
{
	auto it = tabele.find(nume);
	return it != tabele.end();
}

void baza_de_date::listeaza_tabele()
{
	for (auto it = tabele.begin(); it != tabele.end(); it++)
	{
		cout << "->" << it->first << endl;
	}
}

void baza_de_date::stergeti_toate_tabelele()
{
	for (auto it = tabele.begin(); it != tabele.end(); it++)
	{
		sterge_tabela(it->first.c_str());
	}
}


//PROCESOR COMENZI
//valideaza numele pentru tabele si coloane
//citeste comanda de la tastatura si o identifica
void procesor_comenzi::proceseaza_comanda(char* sir)
{
	//PRIMUL CUVANT DIN COMANDA IN TOKEN
	char delims[] = " ";
	char* token = nullptr;
	char* context = nullptr;
	token = strtok_s(sir, delims, &context);

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


	comanda* c = nullptr;

	if (token != nullptr)
	{
		if (_stricmp(token, "CREATE") == 0)
		{
			comanda_create com(context);
			c = &com;
			if (c->verifica_sintaxa())
			{
				c->executa_comanda();
			}
		}
		else if (_stricmp(token, "DROP") == 0)
		{
			comanda_drop com(context);
			c = &com;
			if (c->verifica_sintaxa())
			{
				c->executa_comanda();
			}
		}
		else if (_stricmp(token, "DISPLAY") == 0)
		{
			comanda_display com(context);
			c = &com;
			if (c->verifica_sintaxa())
			{
				c->executa_comanda();
			}
		}
		else if (_stricmp(token, "INSERT") == 0)
		{
			comanda_insert com(context);
			c = &com;
			if (c->verifica_sintaxa())
			{
				c->executa_comanda();
			}
		}
		else if (_stricmp(token, "DELETE") == 0)
		{
			comanda_delete com(context);
			c = &com;
			if (c->verifica_sintaxa())
			{
				c->executa_comanda();
			}
		}
		else if (_stricmp(token, "SELECT") == 0)
		{
			comanda_select com(context);
			c = &com;
			if (c->verifica_sintaxa())
			{
				c->executa_comanda();
			}
		}
		else if (_stricmp(token, "UPDATE") == 0)
		{
			comanda_update com(context);
			c = &com;
			if (c->verifica_sintaxa())
			{
				c->executa_comanda();
			}
		}
		else if (_stricmp(token, "IMPORT") == 0)
		{
			comanda_import com(context);
			c = &com;
			if (c->verifica_sintaxa() && comanda_import::verifica_fisier(context))
			{
				c->executa_comanda();
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
		else if (_stricmp(token, "MENU") == 0)
		{
			meniu::executa_meniu();
		}
		else
		{
			cout << "Nu recunosc comanda: " << token << endl;
		}
	}
	cout << endl;
}

void procesor_comenzi::citeste_comanda_fisier(const char* nume_fisier)
{
	ifstream f(nume_fisier);

	char linie[1000];
	if (f.is_open())
	{
		while (!f.eof())
		{
			f >> ws;
			f.get(linie, 999);

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
	char comanda[1000];
	cout << "Introduceti o comanda:" << endl;
	cin >> ws;
	cin.get(comanda, 999);
	procesor_comenzi::proceseaza_comanda(comanda);
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

string procesor_comenzi::to_upper(string s)
{
	for (int i = 0; i < s.length(); i++)
	{
		if(s[i]>='a' && s[i]<='z')
		{
			s[i] -= 32;
		}
	}
	return s;
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

	if ((nr_puncte == 1 && nr_cifre == n - 1) || (nr_puncte == 1 && nr_cifre == n - 2 && este_negativ))
	{
		return tip_coloane::real;
	}

	if (nr_cifre == n || (nr_cifre == n - 1 && este_negativ))
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

void meniu::afiseaza_meniu()
{
	cout << "1.Afisati numele tabelelor" << endl;
	cout << "2.Stergeti toate tabelele" << endl;
	cout << "3.Afisati inregistrarile unei tabele" << endl;
	cout << "4.Eliminati toate inregistrarile unei tabele" << endl;
	cout << "5.Exportati o tabela in format csv" << endl;
	cout << "0.Iesire meniu" << endl;
	cout << "Alegeti o comanda din cele de mai sus: ";
}

void meniu::afisati_numele_tabelelor()
{
	baza_de_date::get_instanta()->listeaza_tabele();
	cout << endl;
}

void meniu::stergeti_toate_tabelele()
{
	//baza_de_date::get_instanta()->stergeti_toate_tabelele();
	cout <<"TO DO"<< endl;
}

void meniu::afisati_inregistrarile_tabelei()
{
	string in;
	cout << "Dati denumirea tabelei: ";
	cin >> ws;
	getline(cin, in);
	in = procesor_comenzi::to_upper(in);
	if (baza_de_date::get_instanta()->exista_tabela(in))
	{
		baza_de_date::get_instanta()->afiseaza_toate(in.c_str());
	}
	else
	{
		cout << "Tabela " << in << " nu exista";
	}
}

void meniu::stergeti_inregistrarile_tabelei()
{
	/*string in;
	cout << "Dati denumirea tabelei: ";
	cin >> ws;
	getline(cin, in);
	in = procesor_comenzi::to_upper(in);
	if (baza_de_date::get_instanta()->exista_tabela(in))
	{
		ifstream f((in + ".bin"), ios::trunc);
		f.close();
	}
	else
	{
		cout << "Tabela " << in << " nu exista";
	}
	*/
	cout << "TO DO" << endl;
}

void meniu::exportati_tabela()
{
	string in;
	cout << "Dati denumirea tabelei: ";
	cin >> ws;
	getline(cin, in);
	in = procesor_comenzi::to_upper(in);
	if (baza_de_date::get_instanta()->exista_tabela(in))
	{
		ofstream f((in + "_export.csv"), ios::trunc);
		tabela t = baza_de_date::get_instanta()->get_tabela(in.c_str());
		t.exporta_csv(f);
		cout << endl << "Datele au fost exportate" << endl;
		f.close();
	}
	else
	{
		cout << "Tabela " << in << " nu exista";
	}
}

void meniu::executa_meniu()
{
	bool exit = false;
	while (!exit)
	{
		meniu::afiseaza_meniu();
		char c;
		cin >> ws;
		cin.get(c);
		bool ok = true;
		do {
			switch (c)
			{
			case '1':
				meniu::afisati_numele_tabelelor();
				break;
			case '2':
				meniu::stergeti_toate_tabelele();
				break;
			case '3':
				meniu::afisati_inregistrarile_tabelei();
				break;
			case '4':
				meniu::stergeti_inregistrarile_tabelei();
				break;
			case '5':
				meniu::exportati_tabela();
				break;
			case '0':
				exit = true;
				break;
			default:
				cout << "Optiunea aleasa nu exista" << endl;
				meniu::afiseaza_meniu();
				break;
			}
		} while (ok == false);
	}
}