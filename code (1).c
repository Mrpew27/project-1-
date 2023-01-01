#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include<time.h> //Needed to get the current time of operation aka ctime().


//definition des structures necessaires
typedef struct produit{
    int code_produit;
    char nom_produit[64];
    char description[256];
    int quantitee;
    float prix;
    char date[10];
}produit;

//liste chainée pour stocker les données du stock
typedef struct stock{
    produit valeur;
    struct stock* suivant;
}stock;

typedef struct record {
	int operation_type; // 1 means [+]positive and 0 means [-]negative 
						// 3 means modified (always comes in two lines)
						//(convention: -1 means empty history record)
	char date[32]; //A string with this format: DDD MMM DD HH:MM:SS YYYY
	int product_id; //self explanatory
	char product_name[64]; //Product's name
	int quantity; //self explanatory
	float unit_price; //self explanatory
} record;

typedef struct history{ 
    record valeur;
	struct history* suivant;
}history;


//fonctions pour effacer l'ecran
enum ClearCodes {
    CLEAR_ALL
};
record newRecord(int opType,produit*prod,int quantitee);
static inline void deplacerVers(int ligne, int colonne) {
    printf("\x1b[%d;%df", ligne, colonne);
}
static inline void effacerEcran(void) {
    //printf("\x1b[%dJ", CLEAR_ALL);deplacerVers(0,0);
	system("cls");
}
void appendHistory(history * ht);
void addRecord(history * ht, record p);
void ecran1(stock *tete, history *tete1);
void ecran2(stock* tete, history *tete1);
//ecriture des produits d'une maniere propre
void afficherProduit(produit prod){
	printf("\n\t code du produit : %-5d \n\t nom du produit : %-20s \n\t prix :%-5.3f \n\t quantitee : %-3d \n\t description : %s \n\t date : %s",prod.code_produit,prod.nom_produit,prod.prix,prod.quantitee,prod.description, prod.date);
}
//result resultat
//fonction pour afficher le stock
void afficherStock(stock * st){
	
	stock * aux;
    aux = st;
	if (st->valeur.code_produit != -1){
		aux = st;
        // parcourir la liste chainée en affichant chaque produit
		do
		{
			afficherProduit(aux->valeur);
			aux = aux->suivant;
			printf("\t\n\t");
		}while (aux!= NULL);	
	}
}

// vérifier si un produit existe dans le stock(0 si il existe, 1 sinon)
int verif(stock * st, int id){
	stock *aux = st;
	while(aux != NULL){
		if(aux->valeur.code_produit == id){
			return 0;
		}
		aux = aux->suivant;
	}
	return 1;
}

int verifier_format_date(char* date) {
  int jour, mois, annee;
  char separateur1, separateur2;
  int nb_elements_scannes = sscanf(date, "%d%c%d%c%d", &jour, &separateur1, &mois, &separateur2, &annee);
  if (nb_elements_scannes != 5) {
    // pas assez d'éléments ont été scannés, donc la date n'est pas au bon format
    return 0;
  }
  if (separateur1 != '/' || separateur2 != '/') {
    // les séparateurs ne sont pas '/', donc la date n'est pas au bon format
    return 0;
  }
  if (jour < 1 || jour > 31 || mois < 1 || mois > 12 || annee < 1) {
    // le jour, le mois ou l'année est en dehors de la plage valide, donc la date n'est pas au bon format
    return 0;
  }
  return 1;
}

//fonction pour saisir le code du produit (peut etre utiliser pour la modification ou la supression des produits)
int saisirCP(stock * st){
	int temp_id;
	while(1){
			printf("\t Entre le code du produit: ");
			scanf("%d", &temp_id);
			if(verif(st, temp_id) == 0) break;
			else printf("\t Pas de prduits avec le code %d, veuillez réessayer\n");
		}
	return temp_id;
}

// fonction pour faciliter l'affectation d'un produit a un autre
void copierProduit(produit *prod1,produit *prod2){
	prod1->code_produit = prod2->code_produit; strcpy(prod1->nom_produit,prod2->nom_produit);
	prod1->prix = prod2->prix; prod1->quantitee = prod2->quantitee;
	strcpy(prod1->description,prod2->description);	strcpy(prod1->date,prod2->date);
}

//ajouter un produit au stock
void ajouterProduit(stock * st, produit p){
	// Adds a new produit to the stock
	stock * ptr,*aux;
	
	history * temp_history;
	temp_history = (history*)malloc(sizeof(history));
	temp_history->suivant = NULL;
	temp_history->valeur.operation_type = -1;
	
	if (st->valeur.code_produit == -1){
		// si le stock est vide alors modifier la valeur du premier noeud 
		copierProduit(&(st->valeur),&p);
		
	} else {
		// sinon on parcoure la liste et on ajoute le produit au fin
		ptr = (stock*)malloc(sizeof(stock));	
		ptr->suivant = NULL;
		aux = st;
		copierProduit(&(ptr->valeur),&p);
		while(aux->suivant != NULL){
			aux = aux->suivant;
		}
		aux->suivant = ptr;
	}
    printf("\t Produit à été ajouté avec succès\n");
    printf("\t Appuyez sur entrée pour continuer\n");
    record temp_record = newRecord(1,&st->valeur,st->valeur.quantitee); 
	addRecord(temp_history,temp_record);
}

// modifier les données d'un un produit du stock selon le code_produit
void modifier(stock *st, int code){
    int toInput; //variable muette
    stock *aux;
    aux = st;
    while(aux->valeur.code_produit != code){
        aux = aux->suivant;
    }
    if(aux == NULL){
        printf("Produit inexistant\n");
    }
    else{
        char c;
        printf("\t Produit n°%d :\n", code);
        afficherProduit(aux->valeur);
        printf("\n");
        printf("\t Nouveau prix = ");
        scanf("%d", &toInput);
        printf("\n");
        aux->valeur.prix = toInput;
        printf("\t Nouvelle quantité = \n");
        scanf("%d", &toInput);
        printf("\n");
        aux->valeur.quantitee = toInput;
        printf("\t Le produit n°%d à été modifé avec succès\n");
        printf("\t Appuyez sur entrée pour continuer\n");
    }
}

// supprimer un produit du stock selon le code_produit
void supprimerProduit(stock * st, int id){
	history * temp_history;
	temp_history = (history*)malloc(sizeof(history));
	temp_history->suivant = NULL;
	temp_history->valeur.operation_type = -1;
	stock *ptr, *aux;
	aux = st;
	ptr = (stock*)malloc(sizeof(stock));
	if (st->valeur.code_produit == -1){
		// si le stock est vide alors il n'y a rien à supprimer
		printf("\t Le stock est vide");
	}
	else{
		if (st->valeur.code_produit == id){
			// supprimer le premier noeud
			st->valeur = st->suivant->valeur;
            st->suivant = st->suivant->suivant;
			free(ptr);
		}
		else{
			// sinon on parcoure la liste pour trouver le produit à effacer
			while(aux->suivant->valeur.code_produit != id){
				aux = aux->suivant;
			}
			ptr = aux->suivant;
			aux->suivant = ptr->suivant;
			free(ptr);
		}
		printf("\t Le produit avec le code %d a été supprimé avec succès\n", id);
		record temp_record = newRecord(-1,&st->valeur,st->valeur.quantitee); 
		addRecord(temp_history,temp_record);
        printf("\t Appuyez sur entrée pour continuer\n");
	}
}

//importer le stock
void importer(stock * st){
	FILE* ptr; char ch; produit temp_prod;
	ptr = fopen("stock.csv", "r"); //ouvrir le fichier en mode lecture
	
	char temp_str1[400],temp_str2[400],temp_str5[400],temp_str3[400],temp_str4[400],temp_str6[400];
	int i=0; int scan_result;
	do {
		scan_result = fscanf(ptr, "%400[^;];%400[^;];%400[^;];%400[^;];%400[^;];%400[^\n]", 
								temp_str1, temp_str2, temp_str5,temp_str3, temp_str4, temp_str6);
		ch = fgetc(ptr);
		if (i>0 && scan_result == 6){
			temp_prod.code_produit = atoi(temp_str1);
			strcpy(temp_prod.nom_produit,temp_str2);
			strcpy(temp_prod.description,temp_str5);
			temp_prod.prix = atof(temp_str3);
			temp_prod.quantitee = atoi(temp_str4);
			strcpy(temp_prod.date,temp_str6);
			ajouterProduit(st,temp_prod);
		}
		i++;
	}while(ch != EOF);
	fclose(ptr);
}

//exporter le stock au fichier
void exporter(stock * st){
	FILE* ptr;
	ptr = fopen("stock.csv","w"); // ouvrir le fichier en mode ecriture
	stock *temp = st;
    fprintf(ptr, "code_produit;nom_produit;description;pric;quantitée;date\n");
	while(temp != NULL) {
	fprintf(ptr,"%d;%s;%s;%.3f;%d;%s\n",temp->valeur.code_produit,temp->valeur.nom_produit,temp->valeur.description,
	temp->valeur.prix,temp->valeur.quantitee,temp->valeur.date);
	temp = temp->suivant;
	}
	fclose(ptr);
}


void copyRecord(record *rec1,record *rec2){
	// Convinience function to copy product variables
	rec1->operation_type = rec2->operation_type;
	strcpy(rec1->date,rec2->date);
	rec1->product_id = rec2->product_id;
	strcpy(rec1->product_name,rec2->product_name);
	rec1->quantity = rec2->quantity;
	rec1->unit_price = rec2->unit_price;
}

//Adds a new record to history.
void addRecord(history * ht, record p){
	// Adds a new record to history.
	history * ptr,*aux;	
	if (ht->valeur.operation_type == -1){
		// If the history of records is empty then just set the value of the first element 
		copyRecord(&(ht->valeur),&p); 
	} else {
		ptr = (history*)malloc(sizeof(history));	
		ptr->suivant = NULL;
		aux = ht;
		copyRecord(&(ptr->valeur),&p);
		while(aux->suivant != NULL){
			aux = aux->suivant;
		}
		aux->suivant = ptr; 
	}	
}
//product 
char* HISTORY_FILE = "history.csv";
//Import the History of records from HISTORY_FILE
void importHistory(history * ht){
    FILE* ptr; char ch; record tempRecord;
	ptr = fopen(HISTORY_FILE, "r"); //opening file
	char t1[32],t2[32],t3[32],t4[64],t5[32],t6[32],t7[32];
	int i=0; int scan_result;
	do {
		scan_result = fscanf(ptr, "%400[^;];%400[^;];%400[^;];%400[^;];%400[^;];%400[^\n]", 
								t1,t2,t3,t4,t5,t6,t7);
		ch = fgetc(ptr);
		if (i>0 && scan_result >= 5){
			tempRecord.operation_type = atoi(t1);
			strcpy(tempRecord.date,t2);
			tempRecord.product_id = atoi(t3);
			strcpy(tempRecord.product_name,t4);
			tempRecord.quantity = atoi(t5);
			tempRecord.unit_price = atof(t6);
			addRecord(ht,tempRecord);
		}
		i++;
	}while(ch != EOF);
	fclose(ptr);
}

void afficherRecord(record prod){
	printf("\n\t operation : %d \n\t date : %-20s \n\t code produit :%i \n\t quantitee : %-3d \n\t nom : %s \n\t prix: %f",prod.operation_type,prod.date,prod.product_id,prod.quantity,prod.product_name,prod.unit_price);
}
//result resultat
//fonction pour afficher le stock
void afficherHistory(history * st){
	
	history * aux;
    aux = st;
	if (st->valeur.product_id != -1){
		aux = st;
        // parcourir la liste chainée en affichant chaque produit
		do
		{
			afficherRecord(aux->valeur);
			aux = aux->suivant;
			printf("\t\n\t");
		}while (aux!= NULL);	
	}
}




//USE: 
// Returns a record type object from relevant input.
// NewRecord() is to be used whenever a product is added/removed from the stock.
// It registers the date of the operation (all you need to do is just call it).
// optype: 0 is [-] and 1 is [+]
// Just provide the product ID, name, quantity and price.
// Example:
// record foo = newRecord(1,139,"bar",20,4.5)
record newRecord(int opType,produit*prod,int quantitee){
	record result;
	time_t current_time;
	struct tm * time_info;
	char date_string[32];

	time(&current_time);
	time_info = localtime(&current_time);

	strftime(date_string, sizeof(date_string), "%Y-%m-%d %H:%M:%S", time_info);


	result.operation_type = opType;
	strcpy(result.date, date_string);
	result.product_id = prod->code_produit;
	strcpy(result.product_name, prod->nom_produit);
	result.unit_price = prod->prix;
	result.quantity = quantitee;
	return result;
}

//Writes to HISTORY_FILE. (mode: w)
void exportHistory(history * ht){
	FILE* ptr;
	fopen(HISTORY_FILE,"w");
	history *temp = ht;
	while(temp != NULL) {
	fprintf(ptr,"%i;%c;%i;%c;%i;%f",temp->valeur.operation_type,temp->valeur.date,
	temp->valeur.product_id,temp->valeur.product_name,temp->valeur.quantity,
	temp->valeur.unit_price);
	temp = temp->suivant;
	}
	fclose(ptr);
}

//Same as exportHistory() but appends to file instead
//of rewriting it. (mode: a)
void appendHistory(history * ht){
	FILE* ptr;
	ptr = fopen(HISTORY_FILE,"a");
	history *temp = ht;
	//fprintf(ptr,"id;name;price;quantity;description\n");
	if (ptr == NULL){
		//setTextColor(RED_TXT);printf("Error");resetColor();
		//printf(": File Access denied.\n");
		return;
	}
	while(temp != NULL) {
		//if (strlen(temp->value.name)==0) strcpy(temp->value.name," ");
		temp->valeur.date[strlen(temp->valeur.date)-1] = '\0';
		fprintf(ptr,"%i;%s;%i;%s;%i;%.3f\n",
                        temp->valeur.operation_type,
                        temp->valeur.date,
                        temp->valeur.product_id,
                        temp->valeur.product_name,
                        temp->valeur.quantity,
                        temp->valeur.unit_price);
		temp = temp->suivant;
	}
	//setTextColorBright(GREEN_TXT);printf("Exported successfully");resetColor();printf(".\n");
	fclose(ptr);
}

// ecran d'accueil
void ecran1(stock *tete, history *tete1){
	    
	
    printf("\n\n\t\t\t\t ********************************************************* \n\n");
	printf("\t\t\t\t                Bienvenue dans stock-manager                     \n");
	printf("\t\t\t\t                        **********                               \n");
	printf("\t\t\t\t              Appuier sur Entrer pour continuer.                 \n");  
	printf("\n\n\t\t\t\t ********************************************************* \n\n");                              
    char character;
    character = getch();
    // bouton 'enter' de la clavier == 13
    printf("Appuyez sur entrée pour continuer\n");
    if ((int)character == 13){
		effacerEcran();
        ecran2(tete, tete1);
    }
}

//autre ecrans
void ecran2(stock* tete, history *tete1){
	char character;
	char choice;
	int c;
	
	printf("\n\t<------------------------------------------------------------------------>\n\n");
	printf("\t1 - ajouter un produit\n"
			"\t2 - supprimer un produit\n"
			"\t3 - afficher tous les produits\n"
			"\t4 - modifier un produit\n"
			"\t5 - afficher l'historique\n"
			"\t6 - exit\n");
	printf("\n\t<------------------------------------------------------------------------>\n");
	scanf("%d", &choice);
	switch (choice) {
        // ajouter un prduit
		case 1:
			effacerEcran();
			produit *p ;
			p = (produit*)malloc(sizeof(produit));
            do{
                printf("\t Donnez le code du produit: ");
                scanf("%d",&(p->code_produit));
                if(verif(tete, p->code_produit) == 0){
                    printf("\t il y a déjà un produit avec le code %d, veuillez réessayer\n", p->code_produit);
                }
            }while(verif(tete, p->code_produit) == 0);
            printf("\t Donnez le nom du produit(sans utilisation d'espaces): ");
            scanf("%s",&(p->nom_produit));
            printf("\t Donnez la description du produit(sans utilisation d'espaces): ");
            scanf("%s",&(p->description));
            printf("\t Donnez la quantitee du produit: ");
            scanf("%d",&(p->quantitee));
            printf("\t Donnez le prix unit du produit: ");
            scanf("%f",&(p->prix));
            do{
                printf("\t Donnez la date d'exp du produit en respectant ce modele -'JJ/MM/AAAA': ");
                scanf("%s",&(p->date));
                if(verifier_format_date(p->date) == 0){
                    printf("\t date erronée, veuillez réessayer\n", p->code_produit);
                }
            }while(verifier_format_date(p->date) == 0);
			ajouterProduit(tete, *p);
    		character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			ecran2(tete, tete1);
			break;
        // supprimer un produit
		case 2:
			effacerEcran();
			c= saisirCP(tete);
			verif(tete, c);
			supprimerProduit(tete, c);
			character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			ecran2(tete, tete1);
			break;
        // afficher tous les produits
		case 3:
			effacerEcran();
			afficherStock(tete);
			character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			ecran2(tete, tete1);
			break;
        // modifier un produit
		case 4:
			effacerEcran();
			c= saisirCP(tete);
			verif(tete, c);
			modifier(tete, c);
			character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			break;
		case 5:
			effacerEcran();
			afficherHistory(tete1);
			character = getch();
            // bouton 'enter' de la clavier == 13
			if ((int)character == 13){
				effacerEcran();
			}
			ecran2(tete, tete1);
			break;
        // quitter
		case 6:
            exporter(tete); // exportation du stock
            exit(0);
			break;
		default:
			printf("\t choix invalide, veuillez réessayer.\n");
			break;
    }
}


// programme principale
int main(){
	// Create an array of records with a fixed size
    const int num_records = 10;
    record records[num_records];

    // Initialize the records with default values
    
    SetConsoleOutputCP(65001); // définir l'encodage de la console sur utf8
	system("color");
	stock *tete;
	tete = (stock*)malloc(sizeof(stock));
	// creation du liste chainée pour le stock
	tete->valeur.code_produit = -1; //pas de produits
	tete->suivant = NULL;
	importer(tete); // importation du stock dans la listee chainnée
	history *tete1;
	tete1 = (history*)malloc(sizeof(history));
	// creation du liste chainée pour le stock
	tete1->valeur.operation_type = -1; //pas de produits
	tete1->suivant = NULL;
	importHistory(tete1);
	effacerEcran() ;
    // boucle infini
	while(1){
		ecran1(tete, tete1);
	}
	return 0;
}

