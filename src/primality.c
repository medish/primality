#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <time.h>


#ifndef NULL
#define NULL (void *) 0
#endif

/*
	Algorithme Square and multiply
	\param int x : entier x 
	\param int n : modulo n
*/
void square_and_multiply(mpz_t R, mpz_t n, char * H){
	// Initialiser R
	mpz_t Ri;
	mpz_init(Ri);
	mpz_set(Ri, R);

	size_t lenH = strlen(H);						// La taille de H
	for (unsigned int i = 1; i < lenH; i++)	{		// Square and multiply
		mpz_mul(R, R, R);							// R = R*R
		mpz_mod(R, R, n);							// R = R mod n
		if(H[i] - '0'){
			mpz_mul(R, R, Ri);						// R = R * x 
			mpz_mod(R, R, n);						// R = R mod n
		}
	}
	mpz_clear(Ri);
}

void random_a(mpz_t a, mpz_t n, int seed, int min, int max){
	// Initialiser
	gmp_randstate_t state;
	gmp_randinit_default( state);

	// seed
	gmp_randseed_ui( state, (unsigned int)time(0) + seed);
	/* Pour générer un entier a tel que  min <  a  < max (N+max)
	 * Il faut génerer un nombre aléatoire entre [min, N-(min-max)]
	 * et ajouter min au résultat.
	*/
	mpz_sub_ui( n, n, min-max);
	//
	mpz_urandomm( a, state, n);
	mpz_add_ui( a, a, min);

	//
	mpz_add_ui( n, n, min-max);
	// Free memory
	gmp_randclear(state);
}

int fermat_test(mpz_t n, int k){
	// Initialisation des variables
	mpz_t a;
	mpz_t n_1;
	mpz_inits(a, n_1, (mpz_ptr)NULL);
	
	mpz_sub_ui(n_1, n, 1);					// Calculer N-1 = exposant
	char * H = mpz_get_str(NULL, 2, n_1);	// Convertir exposant en binaire

	for (int i = 0; i < k; i++)	{	
		random_a(a, n, i, 1, -1);				// Générer un entier a aléatoirement
		square_and_multiply(a, n, H);			// Calculer a = a^n-1 mod n
		if( mpz_cmp_ui(a, 1) != 0) {			// Verifier resultat
			mpz_clears(a, n_1, (mpz_ptr)NULL);
			return 0;								// retourner 0 si a == 1				
		} 
	}

	//free memory
	mpz_clears(a, n_1, (mpz_ptr)NULL);
	return 1;
}

int miller_rabin_test ( mpz_t n, int k)	{
	mpz_t s, t, y, a, mod, _mod, j, s_1, y2;
	mpz_inits ( s, t, y, a, mod, _mod, j, s_1, y2, (mpz_ptr)NULL);
	
	int bool = 1;
	mpz_set_ui ( s, 0);
	mpz_sub_ui ( t, n, 1);
	
	do	{
		mpz_add_ui ( s, s, 1);
		mpz_div_ui ( t, t, 2);
		mpz_mod_ui ( mod, t, 2);
	}while ( !mpz_cmp_ui ( mod, 0));	
	
	mpz_sub_ui ( s_1, s, 1);
	gmp_printf("n - 1 = 2^(%Zd) x %Zd\n", s, t);
	char * H = mpz_get_str(NULL, 2, t);
	for (int i = 1; i <= k; i++)	{
		printf ("Pour i = %d\n", i);
		random_a(a, n, i, 0, 0);
		gmp_printf ( "\tChoisir un nombre a = %Zd\n", a);
		mpz_set ( y, a);
		square_and_multiply ( y, n, H);

		mpz_add_ui ( _mod, y, 1);
			gmp_printf ( "\t y = %Zd mod %Zd = %Zd\n", y, n, _mod);
		if ( mpz_cmp_si ( y, 1) && mpz_cmp (_mod, n))	{
			mpz_set_ui ( j, 1);
			bool = 1;
			gmp_printf ( "\t Pour j = %Zd < %Zd\n", j, s);
			while ( mpz_cmp ( j, s) <= 0 && bool)	{
				mpz_set ( y2, y);
				square_and_multiply ( y2, n, "10\0");
				gmp_printf ( "\t\t y2 = %Zd mod %Zd\n", y2, n);
				if ( mpz_cmp_ui ( y2, 1))	{
					mpz_clears ( s, t, y, a, mod, j, s_1, y2, (mpz_ptr)NULL);
					return 0;
				}
				if ( mpz_cmp_si ( y2, -1)) bool = 0;;
				mpz_add_ui ( j, j, 1); 
			} 
			if ( bool)	{
				mpz_clears ( s, t, y, a, mod, j, s_1, y2, (mpz_ptr)NULL);
				return 0;
			}
		}
	}
	mpz_clears ( s, t, y, a, mod, j, s_1, y2, (mpz_ptr)NULL);
	return 1;
}

void fermat ( int argc, char *argv[])	{
	//a^exp mod n
	// Initialisation des variables
	mpz_t n;
	//mpz_init_set_str(a, "15", 10);
	//mpz_init_set_str(exp, "532", 10);
	mpz_init_set_str(n, argv[1], 10);

	//random_a(n, a);
	// Conversion de l'exposant en binaire
	//char * H = mpz_get_str(NULL, 2, exp);

	// Square and multiply
	//square_and_multiply(a, n, H);

	// 
	//gmp_printf("%Zd\n", a);
	if(fermat_test(n, atoi(argv[2])))
			printf("Oui\n");
	else	printf("Non\n");
	// Free memory
	//free(H);
	//mpz_clear(a);
	mpz_clear(n);
	//mpz_clear(exp);	
}

int main(int argc, char const *argv[])	{	
	mpz_t n;
	mpz_init_set_str ( n, argv[1], 10);
	printf ("%s : %s\n", 
			argv[1], 
			miller_rabin_test ( n, atoi ( argv[2])) ? "Premier\0" : "Composé\0");
	return 0;
}
