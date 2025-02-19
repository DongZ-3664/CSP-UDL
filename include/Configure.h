#ifndef _Configure_H_
#define _Configure_H_



//#define _DATA_FILE_PATH (std::string)"../instances/" 
#define _LOG_FILE_PATH (std::string)"../ALNSLogFile/" 
#define _LOWBOUND_FILE_PATH (std::string)"../PreResults/"
#define _RESULTS_FILE_PATH (std::string)"../Results/" 


#define _RUN_RMP 1						// 1: ALNS + RMP;						0: only ALNS
#define _RUN_POPULATION 0				// 1: evolutionary algorithm + RMP		0: only ALNS
#define _RMP_MAX_TIME 600				// The time limit for solving model [RMP]
#define _RMP_MAX_THREADS 1				// The number of threads used to solve model [RMP]


#define _ITS_PRE_SEGMENT 201			// The number of iterations executed in each searching segment
#define _NUM_SEGMENTS_ 15				// The number of searching segments


#define _DESTRUCTION_RATE 0.25			// The destruction rate
#define _WEIGHTS_SENSITIVITY 0.57		// The adaptive rate
#define _COOLING_RATE 0.997				// The cooling rate
#define _MAX_NO_IMPROVEMENT 45			// The allowable consecutive unaccepted limits
#define _NUM_RUNS_INITIAL_SOL 100		// The number of executions of the greedy construction algorithm


#define _SOLU_TABU_CAPCITGY 20			// The capacity of solution tabu list
#define _SOLU_FORGETTING_RATE 0.3		// The forgetting rate for solution tabu list
#define _OPER_TABU_CAPCITGY 0.5			// The capacity of operation tabu list (A ratio relative to the total number of tasks)
#define _OPER_FORGETTING_RATE 0.3		// The forgetting rate for operation tabu list



#define _INITIAL_SCORE 100.0

#define _SIGAMA_1 12.0
#define _SIGAMA_2 6.0
#define _SIGAMA_3 3.0
#define _SIGAMA_4 1.0

#define _CUT_OUT_RATE 0.05
#define _POOL_MIN_SIZE 1


#define __EPSILON 0.0001



#define _Callback_ 0			
#define _N3_Constraints_ 1
#define _PrintVars_ 1
#define _NUM_RUNS_RMP 1			

extern double lower_bound;
extern std::string logfile;
extern std::string datafile;
extern std::string resultfile;

extern double Multiplier_L1;
extern double Multiplier_L2;

extern double alns_time;

#if _RUN_RMP
extern double total_time;
extern double rmp_time;
#endif // _RUN_RMP


#if _RUN_POPULATION
extern std::map<std::string, double> ObjValue;

extern std::map<std::string, double> LBGap;

extern std::map<std::string, double> CPUTime;

extern int num_crossover_Imp;

#endif // _RUN_POPULATION




#endif // !_Configure_H_

