#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define INFINI 10000

int **poids;    // matrice des poids
int n;		// nb de sommets

typedef struct tchemin {
	int ns;
	int cout;
	int *chem;
} TypeChemin;

typedef struct tmaillon {
	TypeChemin *val;
	struct tmaillon *adr;
} Maillon;

typedef struct tpile {
	Maillon *sommet;
} TypePile;


void init_graph( int nbsommet );
void charger_graph(char *nomfich);
void dfs( int depart );
void affich();
int Existe_dans_chemin(int j, TypeChemin *ch);
void CreerPile( TypePile *p );
void Empiler( TypePile *p, TypeChemin *ch );
void Depiler( TypePile *p, TypeChemin **ch );
int PileVide( TypePile p );


int main(int argc, char *argv[])
{
   int n;
   char nomfich[40];

   do {
	printf("1) Générer un graphe complet aléatoire\n");
	printf("2) Lire le graphe à partir d'un fichier donné\n");
	printf("Choix : ");
	scanf(" %d", &n);
   }
   while (n != 1 && n != 2);

   if ( n == 1 ) {
	printf("Nombre de sommets : ");
	scanf(" %d", &n);
      	init_graph( n );
   }
   else {
	printf("Nom du fichier contenant le graphe : ");
	scanf(" %s", nomfich);
      	charger_graph( nomfich );
   }

   affich();

    long cal = 6000000L;
    clock_t start, finish;
    double duration;
    start = clock();
    while( cal-- );

   printf("PVC avec dfs \n\n");

   dfs(0);

   finish = clock();
        duration = (double)(finish - start) ; // CLOCKS_PER_SEC;
        printf( "Temps d exe : %6.6f ms\n", duration );

   return 0;
}


void init_graph( int nbsommet )
{
   int i, j;
   n = nbsommet;

   poids = malloc( n*sizeof(int *) );
   for (i=0; i<n; i++)
	poids[i] = malloc( n*sizeof(int) );

   srand((unsigned int)time((time_t *)NULL));
   for (i=0; i<n; i++) {
	for (j=i+1; j<n; j++) {
	     poids[i][j] = 1+(int) (1.0*n*rand()/(RAND_MAX+1.0));
	     poids[j][i] = poids[i][j];
	}
	poids[i][i] = 0;
   }
}


void charger_graph(char *nomfich)
{
   int i, j;
   FILE *f = fopen( nomfich, "r" );
   fscanf(f, " %d", &n);

   poids = malloc( n*sizeof(int *) );
   for (i=0; i<n; i++)
	poids[i] = malloc( n*sizeof(int) );

   for (i=0; i<n; i++)
	for (j=0; j<n; j++)
	    fscanf(f, " %d", &poids[i][j]);
   fclose(f);
}


void dfs( int depart )
{
   TypePile p;
   int i, j, k;
   long cpt = 0;
   int cout_opt = INFINI;
   TypeChemin *nouv = NULL;
   TypeChemin *cycle_opt = malloc( sizeof(TypeChemin) );
   TypeChemin *ch = malloc( sizeof(TypeChemin) );

   cycle_opt->chem = malloc( (n+1)*sizeof(int) );
   ch->chem = malloc( sizeof(int) );
   ch->ns = 1; ch->chem[0] = depart; ch->cout = 0;
   CreerPile( &p );
   Empiler(&p, ch);
   while ( !PileVide(p) ) {
	Depiler( &p, &ch );
	if ( ch->ns == n ) {
	   // chemin contenant tous les sommets
	   ch->ns = ch->ns + 1;
	   ch->chem[ ch->ns-1 ] = depart;
	   ch->cout = ch->cout + poids[ch->chem[ch->ns-2]][depart];
	   cpt++;
//printf("Solution %4d avec un cout = %d\n", cpt, ch->cout);
	   if ( ch->cout < cout_opt ) {
		// MAJ cycle optimal
		cout_opt = ch->cout;
   		for (i=0; i < n+1; i++)
   		    cycle_opt->chem[i] = ch->chem[i];
   		cycle_opt->ns = ch->ns;
   		cycle_opt->cout = ch->cout;
	   }
	}
	else {  // ifnot ( ch[0] == n )
	   i = ch->chem[ ch->ns-1 ];  // le dernier sommet du chemin
	   for (j=0; j<n; j++)
		if ( j != depart && j != i && !Existe_dans_chemin(j,ch) )  {
		   nouv = malloc( sizeof(TypeChemin) );
		   nouv->chem = malloc( (ch->ns+2)*sizeof(int) );
 		   //nouv->chem = malloc( (n+1)*sizeof(int) );
		   for (k=0; k < ch->ns; k++)
   		    	nouv->chem[k] = ch->chem[k];
		   nouv->chem[ ch->ns ] = j;
		   nouv->ns = ch->ns+1;
		   nouv->cout = ch->cout + poids[i][j];
		   Empiler( &p, nouv );
		}
     	} // endif ( ch[0] == n )
	free(ch->chem);
	free(ch);
   } // while
   // Affichage de la solution optimale
   printf("Le cycle hamiltonien de poids minimum est :\n");
   for (i=0; i <= n; i++)
	printf("%3d ", cycle_opt->chem[i]);
   printf("Son cout est = %3d\nNb total de solutions = %ld\n", cycle_opt->cout, cpt);
}


void affich()
{
   int i, j;
   FILE *f = fopen("graph.txt", "w");
   fprintf(f, "%d\n", n);
   printf("\n matrice des poids des aretes:\n");
   for (i=0; i<n; i++) {
	for (j=0; j<n; j++) {
	    printf("%3d ", poids[i][j]);
	    fprintf(f, "%d\n", poids[i][j]);
	}
	printf("\n");
   }
   printf("\n");
   fclose(f);
}


int Existe_dans_chemin( int j, TypeChemin *ch)
{
   int i;
   int trouv = 0;
   for (i=0; i < ch->ns && !trouv; i++)
	if ( j == ch->chem[i] ) trouv = 1;
   return trouv;
}


void CreerPile( TypePile *p )
{
   p->sommet = NULL;
}


void Empiler( TypePile *p, TypeChemin *ch )
{
   Maillon *q = malloc( sizeof(Maillon) );
   q->val = ch;
   q->adr = p->sommet;
   p->sommet = q;
}


void Depiler( TypePile *p, TypeChemin **ch )
{
   Maillon *q;
   if (p->sommet != NULL) {
	*ch = p->sommet->val;
	q = p->sommet;
	p->sommet = p->sommet->adr;
	free(q);
   }
}


int PileVide( TypePile p )
{
   return (p.sommet == NULL);
}

