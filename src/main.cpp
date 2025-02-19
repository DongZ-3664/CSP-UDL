#include "ALNS.h"


double lower_bound = 0.;

std::string logfile;
std::string datafile;
std::string resultfile;


double Multiplier_L1 = 0.;
double Multiplier_L2 = 0.;


double alns_time = 0.;

#if _RUN_RMP
double rmp_time = 0.;
double total_time = 0.;
#endif // _RUN_RMP


#if _RUN_POPULATION

std::map<std::string, double> ObjValue = {
    {"init_popu_max", 0.}, {"init_popu_min", 0.}, {"init_popu_avg", 0.},
    {"final_popu_max",0.}, {"final_popu_min",0.}, {"final_popu_avg",0.},
    {"rmp_1",0.}, {"rmp_2",0.} };

std::map<std::string, double> LBGap = {
    {"init_popu_max", 0.}, {"init_popu_min", 0.}, {"init_popu_avg", 0.},
    {"final_popu_max",0.}, {"final_popu_min",0.}, {"final_popu_avg",0.},
    {"rmp_1",0.}, {"rmp_2",0.} };

std::map<std::string, double> CPUTime  = { 
    {"total",  0.}, {"alns",   0.}, {"rmp_1", 0.},
    {"evolve", 0.}, {"rmp_2", 0.} };

int num_crossover_Imp = 0;

#endif // _RUN_POPULATION






int main(int argc, const char* argv[])
{ 

    if (argc <= 5) {
        std::cout << "Usage: " << std::endl;
        std::cout << "argv[1] : The proportion of medium-span tasks." << std::endl;
        std::cout << "argv[2] : The proportion of short-span tasks." << std::endl;
        std::cout << "argv[3] : The punishment multiplier when the load of the crane is 1." << std::endl;
        std::cout << "argv[4] : The punishment multiplier when the load of the crane is 2." << std::endl;
        std::cout << "argv[5] : The number of slabs." << std::endl;
        std::cout << "example(for Linux)   : ./MATH_Small.a 1 3 1.4 2.0 20 ../instances" << std::endl;
        std::cout << "example(for Windows) : ./MATH_Small.exe 1 3 1.4 2.0 20 ../instances" << std::endl;


        return 1;
    }



    std::string m_ratio = argv[1];
    std::string s_ratio = argv[2];

    Multiplier_L1 = std::atof(argv[3]);
    Multiplier_L2 = std::atof(argv[4]);

    int njobs = std::atoi(argv[5]);

    datafile = argv[6];
    if (datafile.back() != '/') {
        datafile.push_back('/');
    }

    std::string append;
    std::string loghead = "ALNS";
#if _N3_Constraints_
    append += "N3_";
#endif // _N3_Constraints_

#if _Callback_
    append += "cb_";
#endif // _CallBack_

#if _RUN_RMP
    loghead = "MATH";
#endif // _RUN_RMP

#if _RUN_POPULATION
    loghead = "POPU";
#endif // _RUN_POPULATION



    std::string folder = "R_1_" + m_ratio + "_" + s_ratio + "/";
    


    std::string logdir = _LOG_FILE_PATH + folder + "F_" + argv[3] + "_" + argv[4] + "/";
    std::string lowbounddir = _LOWBOUND_FILE_PATH + folder + "F_" + argv[3] + "_" + argv[4] + "/";
    std::string resultdir = _RESULTS_FILE_PATH + folder + "F_" + argv[3] + "_" + argv[4] + "/";


    datafile = datafile + folder + itos(njobs) + ".txt";
    logfile = logdir + loghead + "_" + itos(njobs) + ".txt";
    resultfile = resultdir + loghead + "_" + itos(njobs) + ".txt";

    std::cout << datafile << std::endl;
    {
        std::string lb_file = lowbounddir + "TBM_" + itos(njobs) + ".txt";
        std::ifstream ifs(lb_file);
        double lb = 0.;
        for (int i = 0; i < 2; ++i) {
            ifs >> lb;
        }
        lower_bound = lb;
        ifs.close();
    }





    if (access(logdir.c_str(), 0) == -1) {		// logdir not found
        myself::createDirectory(logdir);
    }

    if (access(resultdir.c_str(), 0) == -1) {	// resultdir not found
        myself::createDirectory(resultdir);
    }



    double res_obj = _ALNS_RESTRICTED_MASTER(njobs);
    double res_gap = ((res_obj - lower_bound) / res_obj);
    res_gap = res_gap < __EPSILON ? 0. : res_gap;

    std::ofstream res_ofs(resultfile);
#if _RUN_RMP
    res_ofs << res_obj << std::endl;  //best-obtained
    res_ofs << res_gap << std::endl;  //gap 
    res_ofs << std::fixed << std::setprecision(2) << total_time << std::endl;
    res_ofs << std::fixed << std::setprecision(2) <<  alns_time << std::endl;
    res_ofs << std::fixed << std::setprecision(2) <<   rmp_time << std::endl;
#elif _RUN_POPULATION
    res_ofs << std::fixed << std::setprecision(2) << ObjValue["init_popu_max"]  << std::endl;
    res_ofs << std::fixed << std::setprecision(2) << ObjValue["init_popu_min"]  << std::endl;
    res_ofs << std::fixed << std::setprecision(2) << ObjValue["init_popu_avg"]  << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["init_popu_max"]  << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["init_popu_min"]  << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["init_popu_avg"]  << std::endl;
    res_ofs << std::fixed << std::setprecision(2) <<  CPUTime["alns"]           << std::endl;

    res_ofs << std::fixed << std::setprecision(2) << ObjValue["rmp_1"]          << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["rmp_1"]          << std::endl;
    res_ofs << std::fixed << std::setprecision(2) <<  CPUTime["rmp_1"]          << std::endl;

    res_ofs << std::fixed << std::setprecision(2) << ObjValue["final_popu_max"] << std::endl;
    res_ofs << std::fixed << std::setprecision(2) << ObjValue["final_popu_min"] << std::endl;
    res_ofs << std::fixed << std::setprecision(2) << ObjValue["final_popu_avg"] << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["final_popu_max"] << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["final_popu_min"] << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["final_popu_avg"] << std::endl;
    res_ofs << std::fixed << std::setprecision(2) <<  CPUTime["evolve"]         << std::endl;
    res_ofs << num_crossover_Imp << std::endl;

    res_ofs << std::fixed << std::setprecision(2) << ObjValue["rmp_2"]          << std::endl;
    res_ofs << std::fixed << std::setprecision(6) <<    LBGap["rmp_2"]          << std::endl;
    res_ofs << std::fixed << std::setprecision(2) <<  CPUTime["rmp_2"]          << std::endl;
#else
    res_ofs << res_obj << std::endl;  //best-obtained
    res_ofs << res_gap << std::endl;  //gap 
    res_ofs << std::fixed << std::setprecision(2) << alns_time / _NUM_RUNS_RMP << std::endl;
#endif
    res_ofs.close();

    //¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü¡ü

    return 0;
}
