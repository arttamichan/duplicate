/* 24 Ap4 2014 ij.jang : remove times() , time(), use clock_gettime() instead */
/*
 ****************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *                                                                            
 *  Version:    C, Version 2.1
 *                                                                            
 *  File:       dhry_1.c (part 2 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *
 ****************************************************************************
 */

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "dhry.h"
//#include <ew_printf.h>

#undef DEBUG

#if defined(DEBUG)
#define PRINTF(fmt,args...) printf(fmt,##args);
#else
#define PRINTF(fmt,args...) asm volatile("");
//#define EW_PRINTF(fmt,args...) asm volatile("");
#endif

#define EW_PRINTF(fmt,args...) printf("[EW:DHRYSTONE] "fmt,##args);

/* Global Variables: */

__thread Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
__thread int             Int_Glob;
__thread Boolean         Bool_Glob;
__thread char            Ch_1_Glob,
                Ch_2_Glob;
__thread int             Arr_1_Glob [50];
__thread int             Arr_2_Glob [50] [50];

//extern char     *malloc ();
//Enumeration     Func_1 ();
  /* forward declaration necessary since Enumeration may not simply be int */

#ifndef REG
	__thread  Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        __thread Boolean Reg = true;
#endif


int Proc_1 (REG Rec_Pointer Ptr_Val_Par);
int Proc_2 (One_Fifty   *Int_Par_Ref);
int Proc_3 (Rec_Pointer *Ptr_Ref_Par);
int Proc_4 ();
int Proc_5 ();
int Proc_6 (Enumeration Enum_Val_Par, Enumeration* Enum_Ref_Par);
int Proc_7 (One_Fifty Int_1_Par_Val, One_Fifty Int_2_Par_Val, One_Fifty* Int_Par_Ref);
int Proc_8 (Arr_1_Dim Arr_1_Par_Ref, Arr_2_Dim Arr_2_Par_Ref, int Int_1_Par_Val, int Int_2_Par_Val);
Enumeration Func_1 (Capital_Letter Ch_1_Par_Val, Capital_Letter Ch_2_Par_Val);
Boolean Func_2 (Str_30 Str_1_Par_Ref, Str_30 Str_2_Par_Ref);
Boolean Func_3 (Enumeration Enum_Par_Val);


/* variables for time measurement: */
/* ij.jang remove time() and reduce Too_Small_Time */
#define Too_Small_Time 	500	/* 500msec */
#undef HZ
#define HZ 1000
static long get_cur_time(void)	/* returns in msec */
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec * 1000 +
		ts.tv_nsec / 1000000;
}

long            Begin_Time,
                End_Time,
                User_Time;
float           Microseconds,
		Dhrystones_Per_Second_old = 0,
                Dhrystones_Per_Second;


volatile int test_stopped;
void dhry_sighandler(int signal)
{
	test_stopped = 1;
}

/* end of variables for time measurement */
#if defined(TEST_MODULE)
int dhrystone(int argc, char* args[])
#else
int main(int argc, char* args[])
#endif
/*****/

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
	One_Fifty       Int_1_Loc;
	REG   One_Fifty       Int_2_Loc;
	One_Fifty       Int_3_Loc;
	REG   char            Ch_Index;
	Enumeration     Enum_Loc;
	Str_30          Str_1_Loc;
	Str_30          Str_2_Loc;
	REG   int             Run_Index;
	REG   int             Number_Of_Runs;
	unsigned int 	Number_Of_Loop = 0;	

	time_t op_time;

	/* signal handling */
	struct sigaction act;
	struct sigaction prevact_old_int, prevact_old_term;
	memset(&act, 0, sizeof(act));
	act.sa_handler = dhry_sighandler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGINT);
	sigaddset(&act.sa_mask, SIGTERM);
	sigaction(SIGINT, &act, &prevact_old_int);
	sigaction(SIGTERM, &act, &prevact_old_term);

	/* Initializations */

	Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
	Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

	Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
	Ptr_Glob->Discr                       = Ident_1;
	Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
	Ptr_Glob->variant.var_1.Int_Comp      = 40;
	strncpy (Ptr_Glob->variant.var_1.Str_Comp, 
	"DHRYSTONE PROGRAM, SOME STRING", sizeof(Ptr_Glob->variant.var_1.Str_Comp));
	strncpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING", sizeof(Str_1_Loc));

	Arr_2_Glob [8][7] = 10;
	/* Was missing in published program. Without this statement,    */
	/* Arr_2_Glob [8][7] would have an undefined value.             */
	/* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
	/* overflow may occur for this array element.                   */

	EW_PRINTF("Dhrystone start!\n");
	PRINTF ("\n");
	PRINTF ("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
	PRINTF ("\n");
	if (Reg)
	{
		PRINTF ("Program compiled with 'register' attribute\n");
		PRINTF ("\n");
	}
	else
	{
		PRINTF ("Program compiled without 'register' attribute\n");
		PRINTF ("\n");
	}
	PRINTF ("Please give the number of runs through the benchmark: ");
	{
		int n;

		switch(argc){
		case (1):
			// 		scanf ("%d", &n);
			n = 10000000;
			break;
		case (2):
		default:
			n = atol(args[1]);
			break;
		}
		if(n < 10000000) n = 10000000;
		if(n > 20000000) n = 20000000;
		Number_Of_Runs = n;
	}
	PRINTF ("\n");
	PRINTF("START (%d runs)\n", Number_Of_Runs);

	op_time = time((time_t*)0);  


	//while(1) {
	PRINTF ("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);

	/***************/
	/* Start timer */
	/***************/
	Begin_Time = get_cur_time();

	Number_Of_Loop++;

	for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
	{
		if (test_stopped) {
			EW_PRINTF("Signalled, stop the test.\n");
			break;
		}
		Proc_5();
		Proc_4();
		/* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
		Int_1_Loc = 2;
		Int_2_Loc = 3;
		strncpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING", sizeof(Str_2_Loc));
		Enum_Loc = Ident_2;
		Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
		/* Bool_Glob == 1 */
		while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
		{
			Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
			/* Int_3_Loc == 7 */
			Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
			/* Int_3_Loc == 7 */
			Int_1_Loc += 1;
		} /* while */
		/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
		/* Int_Glob == 5 */
		Proc_1 (Ptr_Glob);
		for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
		/* loop body executed twice */
		{
			if (Enum_Loc == Func_1 (Ch_Index, 'C'))
			/* then, not executed */
			{
			Proc_6 (Ident_1, &Enum_Loc);
			strncpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING", sizeof(Str_2_Loc));
			Int_2_Loc = Run_Index;
			Int_Glob = Run_Index;
			}
		}
		/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Int_2_Loc = Int_2_Loc * Int_1_Loc;
		Int_1_Loc = Int_2_Loc / Int_3_Loc;
		Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
		/* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
		Proc_2 (&Int_1_Loc);
		/* Int_1_Loc == 5 */
  	} /* loop "for Run_Index" */

	/**************/
	/* Stop timer */
	/**************/

	/* ij.jang do not use time() */  
	End_Time = get_cur_time();

	PRINTF ("Execution ends\n");
	PRINTF ("\n");
	PRINTF ("Final values of the variables used in the benchmark:\n");
	PRINTF ("\n");
	PRINTF ("Int_Glob:            %d\n", Int_Glob);
	PRINTF ("        should be:   %d\n", 5);
	PRINTF ("Bool_Glob:           %d\n", Bool_Glob);
	PRINTF ("        should be:   %d\n", 1);
	PRINTF ("Ch_1_Glob:           %c\n", Ch_1_Glob);
	PRINTF ("        should be:   %c\n", 'A');
	PRINTF ("Ch_2_Glob:           %c\n", Ch_2_Glob);
	PRINTF ("        should be:   %c\n", 'B');
	PRINTF ("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
	PRINTF ("        should be:   %d\n", 7);
	PRINTF ("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
	PRINTF ("        should be:   Number_Of_Runs + 10\n");
	PRINTF ("Ptr_Glob->\n");
	PRINTF ("  Ptr_Comp:          %d\n", (int) Ptr_Glob->Ptr_Comp);
	PRINTF ("        should be:   (implementation-dependent)\n");
	PRINTF ("  Discr:             %d\n", Ptr_Glob->Discr);
	PRINTF ("        should be:   %d\n", 0);
	PRINTF ("  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
	PRINTF ("        should be:   %d\n", 2);
	PRINTF ("  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
	PRINTF ("        should be:   %d\n", 17);
	PRINTF ("  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
	PRINTF ("Next_Ptr_Glob->\n");
	PRINTF ("  Ptr_Comp:          %d\n", (int) Next_Ptr_Glob->Ptr_Comp);
	PRINTF ("        should be:   (implementation-dependent), same as above\n");
	PRINTF ("  Discr:             %d\n", Next_Ptr_Glob->Discr);
	PRINTF ("        should be:   %d\n", 0);
	PRINTF ("  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
	PRINTF ("        should be:   %d\n", 1);
	PRINTF ("  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
	PRINTF ("        should be:   %d\n", 18);
	PRINTF ("  Str_Comp:          %s\n",
	Next_Ptr_Glob->variant.var_1.Str_Comp);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
	PRINTF ("Int_1_Loc:           %d\n", Int_1_Loc);
	PRINTF ("        should be:   %d\n", 5);
	PRINTF ("Int_2_Loc:           %d\n", Int_2_Loc);
	PRINTF ("        should be:   %d\n", 13);
	PRINTF ("Int_3_Loc:           %d\n", Int_3_Loc);
	PRINTF ("        should be:   %d\n", 7);
	PRINTF ("Enum_Loc:            %d\n", Enum_Loc);
	PRINTF ("        should be:   %d\n", 1);
	PRINTF ("Str_1_Loc:           %s\n", Str_1_Loc);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
	PRINTF ("Str_2_Loc:           %s\n", Str_2_Loc);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
	PRINTF ("\n");

	User_Time = End_Time - Begin_Time;

	if (User_Time < Too_Small_Time)
	{
		EW_PRINTF ("Measured time (%ld-%ld) too small to obtain meaningful results\n",
		Begin_Time, End_Time);
		EW_PRINTF ("Please increase number of runs\n");
		EW_PRINTF ("\n");
	}
	else
	{
#if 0
#ifdef TIME
		Microseconds = (float) User_Time * Mic_secs_Per_Second 
		/ (float) Number_Of_Runs;
		Dhrystones_Per_Second = (float) Number_Of_Runs / (float) User_Time;
#else
		Microseconds = (float) User_Time * Mic_secs_Per_Second 
		/ ((float) HZ * ((float) Number_Of_Runs));
		Dhrystones_Per_Second = ((float) HZ * (float) Number_Of_Runs)
		/ (float) User_Time;
#endif
#endif
		Microseconds = (float)User_Time * Mic_secs_Per_Second;
		Dhrystones_Per_Second = (float)
		((double)Number_Of_Runs * 1000000.0 / (double)User_Time);
		PRINTF ("Microseconds for one run through Dhrystone: ");
		PRINTF ("%6.1f \n", Microseconds);
		PRINTF ("Dhrystones per Second:                      ");
		PRINTF ("%6.1f (%d)\n", Dhrystones_Per_Second,
				(int)(Dhrystones_Per_Second / 1757));
		PRINTF ("%ld runs, %ld usec\n",
		Number_Of_Runs, User_Time);
		PRINTF ("\n");

		if(Dhrystones_Per_Second_old == 0)
			Dhrystones_Per_Second_old = Dhrystones_Per_Second;
		Dhrystones_Per_Second_old = (Dhrystones_Per_Second_old + Dhrystones_Per_Second) / 2;
#define PRINT_INTERVAL (60*20)  // 20 minute
		if( (op_time + PRINT_INTERVAL) < time((time_t *) 0)) {
			static __thread unsigned long info_loop = 0;
			info_loop++;
			EW_PRINTF("Dhrystones operation time           %d sec \n", PRINT_INTERVAL * info_loop);
			EW_PRINTF("Dhrystones MEAN:        %6.1f (Loop: %d, Run %d) \n", Dhrystones_Per_Second_old, (int)Number_Of_Loop, (int)Number_Of_Runs);
			op_time = time((time_t*)0);
		}

	}
//}  //infinite loop
	EW_PRINTF("Dhrystone end\n");

	sigaction(SIGINT, &prevact_old_int, NULL);
	sigaction(SIGTERM, &prevact_old_term, NULL);

	return (int)((Dhrystones_Per_Second + 0.5) / 1757);
}

int Proc_1 (Ptr_Val_Par)
/******************/

REG Rec_Pointer Ptr_Val_Par;
    /* executed once */
{
  REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;  
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */


  structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob); 
  Ptr_Val_Par->variant.var_1.Int_Comp = 5;
  Next_Record->variant.var_1.Int_Comp 
        = Ptr_Val_Par->variant.var_1.Int_Comp;
  Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
  Proc_3 (&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp 
                        == Ptr_Glob->Ptr_Comp */
  if (Next_Record->Discr == Ident_1)
    /* then, executed */
  {
    Next_Record->variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp, 
           &Next_Record->variant.var_1.Enum_Comp);
    Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
    Proc_7 (Next_Record->variant.var_1.Int_Comp, 10, 
           &Next_Record->variant.var_1.Int_Comp);
  }
  else /* not executed */
    structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);

  return 0;
} /* Proc_1 */


int Proc_2 (Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */

One_Fifty   *Int_Par_Ref;
{
  One_Fifty  Int_Loc;  
  Enumeration   Enum_Loc = Ident_5;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */

  return 0;
} /* Proc_2 */


int Proc_3 (Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */

Rec_Pointer *Ptr_Ref_Par;

{
  if (Ptr_Glob != Null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);

  return 0;  
} /* Proc_3 */


int Proc_4 () /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';

  return 0;
} /* Proc_4 */


int Proc_5 () /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;

  return 0;
} /* Proc_5 */


        /* Procedure for the assignment of structures,          */
        /* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
memcpy (d, s, l)
register char   *d;
register char   *s;
register int    l;
{
        while (l--) *d++ = *s++;
}
#endif
/* 24 Ap4 2014 ij.jang : remove times() , time(), use clock_gettime() instead */
/*
 ****************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *                                                                            
 *  Version:    C, Version 2.1
 *                                                                            
 *  File:       dhry_1.c (part 2 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *
 ****************************************************************************
 */

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "dhry.h"
//#include <ew_printf.h>

#undef DEBUG

#if defined(DEBUG)
#define PRINTF(fmt,args...) printf(fmt,##args);
#else
#define PRINTF(fmt,args...) asm volatile("");
//#define EW_PRINTF(fmt,args...) asm volatile("");
#endif

#define EW_PRINTF(fmt,args...) printf("[EW:DHRYSTONE] "fmt,##args);

/* Global Variables: */

__thread Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
__thread int             Int_Glob;
__thread Boolean         Bool_Glob;
__thread char            Ch_1_Glob,
                Ch_2_Glob;
__thread int             Arr_1_Glob [50];
__thread int             Arr_2_Glob [50] [50];

//extern char     *malloc ();
//Enumeration     Func_1 ();
  /* forward declaration necessary since Enumeration may not simply be int */

#ifndef REG
	__thread  Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        __thread Boolean Reg = true;
#endif


int Proc_1 (REG Rec_Pointer Ptr_Val_Par);
int Proc_2 (One_Fifty   *Int_Par_Ref);
int Proc_3 (Rec_Pointer *Ptr_Ref_Par);
int Proc_4 ();
int Proc_5 ();
int Proc_6 (Enumeration Enum_Val_Par, Enumeration* Enum_Ref_Par);
int Proc_7 (One_Fifty Int_1_Par_Val, One_Fifty Int_2_Par_Val, One_Fifty* Int_Par_Ref);
int Proc_8 (Arr_1_Dim Arr_1_Par_Ref, Arr_2_Dim Arr_2_Par_Ref, int Int_1_Par_Val, int Int_2_Par_Val);
Enumeration Func_1 (Capital_Letter Ch_1_Par_Val, Capital_Letter Ch_2_Par_Val);
Boolean Func_2 (Str_30 Str_1_Par_Ref, Str_30 Str_2_Par_Ref);
Boolean Func_3 (Enumeration Enum_Par_Val);


/* variables for time measurement: */
/* ij.jang remove time() and reduce Too_Small_Time */
#define Too_Small_Time 	500	/* 500msec */
#undef HZ
#define HZ 1000
static long get_cur_time(void)	/* returns in msec */
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec * 1000 +
		ts.tv_nsec / 1000000;
}

long            Begin_Time,
                End_Time,
                User_Time;
float           Microseconds,
		Dhrystones_Per_Second_old = 0,
                Dhrystones_Per_Second;


volatile int test_stopped;
void dhry_sighandler(int signal)
{
	test_stopped = 1;
}

/* end of variables for time measurement */
#if defined(TEST_MODULE)
int dhrystone(int argc, char* args[])
#else
int main(int argc, char* args[])
#endif
/*****/

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{
	One_Fifty       Int_1_Loc;
	REG   One_Fifty       Int_2_Loc;
	One_Fifty       Int_3_Loc;
	REG   char            Ch_Index;
	Enumeration     Enum_Loc;
	Str_30          Str_1_Loc;
	Str_30          Str_2_Loc;
	REG   int             Run_Index;
	REG   int             Number_Of_Runs;
	unsigned int 	Number_Of_Loop = 0;	

	time_t op_time;

	/* signal handling */
	struct sigaction act;
	struct sigaction prevact_old_int, prevact_old_term;
	memset(&act, 0, sizeof(act));
	act.sa_handler = dhry_sighandler;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGINT);
	sigaddset(&act.sa_mask, SIGTERM);
	sigaction(SIGINT, &act, &prevact_old_int);
	sigaction(SIGTERM, &act, &prevact_old_term);

	/* Initializations */

	Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
	Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

	Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
	Ptr_Glob->Discr                       = Ident_1;
	Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
	Ptr_Glob->variant.var_1.Int_Comp      = 40;
	strncpy (Ptr_Glob->variant.var_1.Str_Comp, 
	"DHRYSTONE PROGRAM, SOME STRING", sizeof(Ptr_Glob->variant.var_1.Str_Comp));
	strncpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING", sizeof(Str_1_Loc));

	Arr_2_Glob [8][7] = 10;
	/* Was missing in published program. Without this statement,    */
	/* Arr_2_Glob [8][7] would have an undefined value.             */
	/* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
	/* overflow may occur for this array element.                   */

	EW_PRINTF("Dhrystone start!\n");
	PRINTF ("\n");
	PRINTF ("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
	PRINTF ("\n");
	if (Reg)
	{
		PRINTF ("Program compiled with 'register' attribute\n");
		PRINTF ("\n");
	}
	else
	{
		PRINTF ("Program compiled without 'register' attribute\n");
		PRINTF ("\n");
	}
	PRINTF ("Please give the number of runs through the benchmark: ");
	{
		int n;

		switch(argc){
		case (1):
			// 		scanf ("%d", &n);
			n = 10000000;
			break;
		case (2):
		default:
			n = atol(args[1]);
			break;
		}
		if(n < 10000000) n = 10000000;
		if(n > 20000000) n = 20000000;
		Number_Of_Runs = n;
	}
	PRINTF ("\n");
	PRINTF("START (%d runs)\n", Number_Of_Runs);

	op_time = time((time_t*)0);  


	//while(1) {
	PRINTF ("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);

	/***************/
	/* Start timer */
	/***************/
	Begin_Time = get_cur_time();

	Number_Of_Loop++;

	for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
	{
		if (test_stopped) {
			EW_PRINTF("Signalled, stop the test.\n");
			break;
		}
		Proc_5();
		Proc_4();
		/* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
		Int_1_Loc = 2;
		Int_2_Loc = 3;
		strncpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING", sizeof(Str_2_Loc));
		Enum_Loc = Ident_2;
		Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
		/* Bool_Glob == 1 */
		while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
		{
			Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
			/* Int_3_Loc == 7 */
			Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
			/* Int_3_Loc == 7 */
			Int_1_Loc += 1;
		} /* while */
		/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
		/* Int_Glob == 5 */
		Proc_1 (Ptr_Glob);
		for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
		/* loop body executed twice */
		{
			if (Enum_Loc == Func_1 (Ch_Index, 'C'))
			/* then, not executed */
			{
			Proc_6 (Ident_1, &Enum_Loc);
			strncpy (Str_2_Loc, "DHRYSTONE PROGRAM, 3'RD STRING", sizeof(Str_2_Loc));
			Int_2_Loc = Run_Index;
			Int_Glob = Run_Index;
			}
		}
		/* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
		Int_2_Loc = Int_2_Loc * Int_1_Loc;
		Int_1_Loc = Int_2_Loc / Int_3_Loc;
		Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
		/* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
		Proc_2 (&Int_1_Loc);
		/* Int_1_Loc == 5 */
  	} /* loop "for Run_Index" */

	/**************/
	/* Stop timer */
	/**************/

	/* ij.jang do not use time() */  
	End_Time = get_cur_time();

	PRINTF ("Execution ends\n");
	PRINTF ("\n");
	PRINTF ("Final values of the variables used in the benchmark:\n");
	PRINTF ("\n");
	PRINTF ("Int_Glob:            %d\n", Int_Glob);
	PRINTF ("        should be:   %d\n", 5);
	PRINTF ("Bool_Glob:           %d\n", Bool_Glob);
	PRINTF ("        should be:   %d\n", 1);
	PRINTF ("Ch_1_Glob:           %c\n", Ch_1_Glob);
	PRINTF ("        should be:   %c\n", 'A');
	PRINTF ("Ch_2_Glob:           %c\n", Ch_2_Glob);
	PRINTF ("        should be:   %c\n", 'B');
	PRINTF ("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
	PRINTF ("        should be:   %d\n", 7);
	PRINTF ("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
	PRINTF ("        should be:   Number_Of_Runs + 10\n");
	PRINTF ("Ptr_Glob->\n");
	PRINTF ("  Ptr_Comp:          %d\n", (int) Ptr_Glob->Ptr_Comp);
	PRINTF ("        should be:   (implementation-dependent)\n");
	PRINTF ("  Discr:             %d\n", Ptr_Glob->Discr);
	PRINTF ("        should be:   %d\n", 0);
	PRINTF ("  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
	PRINTF ("        should be:   %d\n", 2);
	PRINTF ("  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
	PRINTF ("        should be:   %d\n", 17);
	PRINTF ("  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
	PRINTF ("Next_Ptr_Glob->\n");
	PRINTF ("  Ptr_Comp:          %d\n", (int) Next_Ptr_Glob->Ptr_Comp);
	PRINTF ("        should be:   (implementation-dependent), same as above\n");
	PRINTF ("  Discr:             %d\n", Next_Ptr_Glob->Discr);
	PRINTF ("        should be:   %d\n", 0);
	PRINTF ("  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
	PRINTF ("        should be:   %d\n", 1);
	PRINTF ("  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
	PRINTF ("        should be:   %d\n", 18);
	PRINTF ("  Str_Comp:          %s\n",
	Next_Ptr_Glob->variant.var_1.Str_Comp);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
	PRINTF ("Int_1_Loc:           %d\n", Int_1_Loc);
	PRINTF ("        should be:   %d\n", 5);
	PRINTF ("Int_2_Loc:           %d\n", Int_2_Loc);
	PRINTF ("        should be:   %d\n", 13);
	PRINTF ("Int_3_Loc:           %d\n", Int_3_Loc);
	PRINTF ("        should be:   %d\n", 7);
	PRINTF ("Enum_Loc:            %d\n", Enum_Loc);
	PRINTF ("        should be:   %d\n", 1);
	PRINTF ("Str_1_Loc:           %s\n", Str_1_Loc);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
	PRINTF ("Str_2_Loc:           %s\n", Str_2_Loc);
	PRINTF ("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
	PRINTF ("\n");

	User_Time = End_Time - Begin_Time;

	if (User_Time < Too_Small_Time)
	{
		EW_PRINTF ("Measured time (%ld-%ld) too small to obtain meaningful results\n",
		Begin_Time, End_Time);
		EW_PRINTF ("Please increase number of runs\n");
		EW_PRINTF ("\n");
	}
	else
	{
#if 0
#ifdef TIME
		Microseconds = (float) User_Time * Mic_secs_Per_Second 
		/ (float) Number_Of_Runs;
		Dhrystones_Per_Second = (float) Number_Of_Runs / (float) User_Time;
#else
		Microseconds = (float) User_Time * Mic_secs_Per_Second 
		/ ((float) HZ * ((float) Number_Of_Runs));
		Dhrystones_Per_Second = ((float) HZ * (float) Number_Of_Runs)
		/ (float) User_Time;
#endif
#endif
		Microseconds = (float)User_Time * Mic_secs_Per_Second;
		Dhrystones_Per_Second = (float)
		((double)Number_Of_Runs * 1000000.0 / (double)User_Time);
		PRINTF ("Microseconds for one run through Dhrystone: ");
		PRINTF ("%6.1f \n", Microseconds);
		PRINTF ("Dhrystones per Second:                      ");
		PRINTF ("%6.1f (%d)\n", Dhrystones_Per_Second,
				(int)(Dhrystones_Per_Second / 1757));
		PRINTF ("%ld runs, %ld usec\n",
		Number_Of_Runs, User_Time);
		PRINTF ("\n");

		if(Dhrystones_Per_Second_old == 0)
			Dhrystones_Per_Second_old = Dhrystones_Per_Second;
		Dhrystones_Per_Second_old = (Dhrystones_Per_Second_old + Dhrystones_Per_Second) / 2;
#define PRINT_INTERVAL (60*20)  // 20 minute
		if( (op_time + PRINT_INTERVAL) < time((time_t *) 0)) {
			static __thread unsigned long info_loop = 0;
			info_loop++;
			EW_PRINTF("Dhrystones operation time           %d sec \n", PRINT_INTERVAL * info_loop);
			EW_PRINTF("Dhrystones MEAN:        %6.1f (Loop: %d, Run %d) \n", Dhrystones_Per_Second_old, (int)Number_Of_Loop, (int)Number_Of_Runs);
			op_time = time((time_t*)0);
		}

	}
//}  //infinite loop
	EW_PRINTF("Dhrystone end\n");

	sigaction(SIGINT, &prevact_old_int, NULL);
	sigaction(SIGTERM, &prevact_old_term, NULL);

	return (int)((Dhrystones_Per_Second + 0.5) / 1757);
}

int Proc_1 (Ptr_Val_Par)
/******************/

REG Rec_Pointer Ptr_Val_Par;
    /* executed once */
{
  REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;  
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */


  structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob); 
  Ptr_Val_Par->variant.var_1.Int_Comp = 5;
  Next_Record->variant.var_1.Int_Comp 
        = Ptr_Val_Par->variant.var_1.Int_Comp;
  Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
  Proc_3 (&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp 
                        == Ptr_Glob->Ptr_Comp */
  if (Next_Record->Discr == Ident_1)
    /* then, executed */
  {
    Next_Record->variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp, 
           &Next_Record->variant.var_1.Enum_Comp);
    Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
    Proc_7 (Next_Record->variant.var_1.Int_Comp, 10, 
           &Next_Record->variant.var_1.Int_Comp);
  }
  else /* not executed */
    structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);

  return 0;
} /* Proc_1 */


int Proc_2 (Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */

One_Fifty   *Int_Par_Ref;
{
  One_Fifty  Int_Loc;  
  Enumeration   Enum_Loc = Ident_5;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */

  return 0;
} /* Proc_2 */


int Proc_3 (Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */

Rec_Pointer *Ptr_Ref_Par;

{
  if (Ptr_Glob != Null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);

  return 0;  
} /* Proc_3 */


int Proc_4 () /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';

  return 0;
} /* Proc_4 */


int Proc_5 () /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;

  return 0;
} /* Proc_5 */


        /* Procedure for the assignment of structures,          */
        /* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
memcpy (d, s, l)
register char   *d;
register char   *s;
register int    l;
{
        while (l--) *d++ = *s++;
}
#endif
