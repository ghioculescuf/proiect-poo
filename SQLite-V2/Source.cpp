#include <iostream>
#include "Header.h"
using namespace std;

int main(int argc, char** argv)
{
	//configuratie
	baza_de_date* bd = baza_de_date::get_instanta();
	
	//fisiere text din linia de comanda
	if (argc != 1)
	{
		
		for (int i = 1; i < argc; i++)
		{
			procesor_comenzi::citeste_comanda_fisier(argv[i]);
		}
		
	}
	
	//comenzi de la tastatura pana la comanda exit
	bool exit = false;
	while (!exit)
	{
		try
		{
			procesor_comenzi::citeste_comanda_consola();
		}
		catch (int e)
		{
			switch (e)
			{
			case 1:
				cout << "Ati iesit din aplicatie" << endl;
				exit = true;
				break;
			default:
				cout << "Cod de eroare necunoscut: " << e << endl;
				break;
			}
		}
	}
}


//IMPORT nume_tabela nume_fisier.CSV


/*
create_studenti.txt inser_studenti.txt select_studneti.txt
Comenzi testate:
create table persoane((nume,text,20,"necunoscut"),(varsta,integer,3,10),(id_functie,integer,3,1));
create table functii((id_functie,integer,3,1),(den_functie,text,20,"necunoscuta"));
insert into persoane values("Adrian",20,1);
insert into persoane values("Mihai",25,2);
insert into functii values(1,"Manager");
insert into functii values(2,"Programator");
insert into functii values(3,"Contabil");
update persoane set nume = 'Bogdan' where nume = "Mihai";
update functii set id_functie = 4 where den_functie = "Programator";
display table persoane;
display table functii;
drop table persoane;
drop table functii;
select all from functii;
select all from persoane;
select all from functii where id_functie = 1;
select (den_functie) from functii;
select (nume, varsta) from persoane;
select (nume, varsta) from persoane where nume = 'Mihai';
delete from functii where den_functie = "Contabil";
delete from persoane where varsta = 20;
delete from persoane where varsta = 31;
select all from functii;
select all from persoane;
list; -- afiseaza tabelele din baza de date cu nr de inregistrari
*/


/*
Cod de test pt clase de pus in main
	//char e[100][100] = { {"1"},{"Elev"} };
	//char f[100][100] = { {"2"},{"Student"} };
	//char g[100][100] = { {"3"},{"Profesor"} };
	//inregistrare i4(2,e);
	//inregistrare i5(2,f);
	//inregistrare i6(2,g);
	//inregistrare inreg[] = { i4,i5,i6 };
	//cout << inreg[1];
	//inreg[0] = inreg[1];
	//cout << inreg[0];
	//tabela functii
	//coloana c1("id_functie",tip_coloane::integer,"0",3);
	//
	//coloana c2;
	//c2.set_nume_coloana("den_functie");
	//c2.set_tip_coloana(tip_coloane::text);
	//c2.set_valoare_implicita("Necunoscuta");
	//c2.set_dimensiune_coloana(20);
	//cout << c1;
	//cout << c2;
	//cout << i1;
	//coloana coloane[2] = { c1,c2 };
	//
	//cout << coloane[0];
	//cout << coloane[1];
	//coloane[0] = coloane[1];
	//cout << coloane[1];
	//tabela t1("Functii", 2, coloane,3,inreg);
	//cout << t1;
	////tabela persoane
	//char a[100][100] = { {"Adrian"},{"20"},{"2"} };
	//char b[100][100] = { {"Mihai"},{"10"},{"1"} };
	//char d[100][100] = { {"Dan"},{"35"},{"3"} };
	//inregistrare i1(3, a);
	//inregistrare i2(3, b);
	//inregistrare i3(3, d);
	//inregistrare inregistrari[3] = { i1,i2,i3 };
	//coloana c3("Nume",tip_coloane::text,"Necunoscut",30);
	//coloana c4("Varsta",tip_coloane::integer,"10",3);
	//coloana c5("Id_functie", tip_coloane::integer, "0", 3);
	//
	////nu cred ca merg getteri si setteri;
	//coloana col[3] = { c3,c4,c5 };
	//tabela t2("Persoane", 3, col,3,inregistrari);
	////for (int i = 0; i < 3; i++) {
	////	t2.set_inregistrare(inregistrari[i],i);
	////}
	//cout << t2;
	//testare clasa inregistrare
	//testare clasa coloana
	//testare clasa tabela
	//testare clasa bd
*/