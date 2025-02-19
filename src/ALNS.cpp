#include "ALNS.h"

double _TABU_ALNS   (   int n,
                        int& best_sol_its,
                        Instance*& test_instance,
                        SolutionPool* sol_pool
                        )
{


    //==================================================================

    int JobsNumber = n;
    int DestroyNum = std::ceil(_DESTRUCTION_RATE * n);


    Solution* InitialSol = NULL;

    InitialSolutionCreator ISolCreator;
    ISolCreator.GreedyInsert(InitialSol, test_instance, _NUM_RUNS_INITIAL_SOL);
    std::cout << "Initial Solution Cost : " << InitialSol->GetTotalCosts() << std::endl;
    sol_pool->Remember(InitialSol);


    DestroyMethod Destroy("destory", DestroyNum);
    RepairMethod Repair("repair");
    RelocateMethod relocat("relocat");
    ExchangeMethod exchange("exchange");
    AcceptanceMethod Accept("accept");
    ShakeMethod Shake("shake");


    //create two tabu lsits    
    TabuSolutionList* sol_tabu_ls = new TabuSolutionList(_SOLU_TABU_CAPCITGY, 40, _SOLU_FORGETTING_RATE);

    TabuOperatorList* mv_tabu_ls = new TabuOperatorList(int(JobsNumber * _OPER_TABU_CAPCITGY), 20, _OPER_FORGETTING_RATE);

    Solution* CurSol = new Solution(InitialSol);

    double cur_cost = CurSol->GetTotalCosts();

    Solution* NeighborSol = NULL;
    Solution* BestSol = new Solution(CurSol);
    double BestSolCost = cur_cost;


    sol_tabu_ls->Remember(CurSol);

    int num_no_impro = 0;
    int max_no_impro = _MAX_NO_IMPROVEMENT;
    int segment_length = _ITS_PRE_SEGMENT;
    int segment_num = _NUM_SEGMENTS_;   //runs = segement_num * segment_length


    int cur_it = 0;


    void (RepairMethod:: * insertion_method)(Solution * &ISol, DestroyMethod * DesM) = NULL;
    void (DestroyMethod:: * removal_method)(Solution * &ISol, TabuOperatorList * tbls, int rand_degree) = NULL;

    int rm_operator_nums[4] = {};
    int in_operator_nums[4] = {};

    while (segment_num--)
    {


        double rm_score_indicator[4] = {};
        double in_score_indicator[4] = {};
        int rm_nums_indicator[4] = {};
        int in_nums_indicator[4] = {};


        int s_l = segment_length;
        while (s_l--)
        {
            int rm_type = Destroy.ChooseARemovalMethod();
            int in_type = Repair.ChooseAInsertionMethod();

            switch (rm_type)
            {
            case 1:
                removal_method = &DestroyMethod::RandomRemoval;
                break;
            case 2:
                removal_method = &DestroyMethod::NearestRemoval;
                break;
            case 3:
                removal_method = &DestroyMethod::WorstRemoval;
                break;
            default:
                break;
            }

            switch (in_type)
            {
            case 1:
                insertion_method = &RepairMethod::RandCheapestInsertion;
                break;
            case 2:
                insertion_method = &RepairMethod::RegretInsertion;
                break;
            case 3:
                insertion_method = &RepairMethod::CheapestInsertion;
                break;
            default:
                break;
            }


            ++rm_nums_indicator[rm_type];
            ++in_nums_indicator[in_type];

            if (num_no_impro == max_no_impro)    
            {
                delete CurSol;
                CurSol = new Solution(BestSol);   

                //shuffle the current solution;
                Shake.ShuffBlock(CurSol);
                cur_cost = CurSol->GetTotalCosts();
                num_no_impro = 0;
                mv_tabu_ls->ClearMemory();
                Accept.SetTemperature(0.0);
            }


            mv_tabu_ls->Updata();
            sol_tabu_ls->Updata();

            Accept.Cooling();
            NeighborSol = new Solution(CurSol);
            

            ++cur_it;
            std::cerr << "current iteration count: " << cur_it << ",  objective value: " << NeighborSol->GetTotalCosts() << std::endl;



            (Destroy.*removal_method)(NeighborSol, mv_tabu_ls, 3);
            std::vector<int> v1 = Destroy.GetMoveJobs();

            (Repair.*insertion_method)(NeighborSol, &Destroy);


            exchange.TestExChange2Jobs(NeighborSol, mv_tabu_ls);
            std::vector<int> v3 = exchange.GetMoveJobs();

            relocat.Optimize(NeighborSol, mv_tabu_ls);
            std::vector<int> v2 = relocat.GetMoveJobs();


            double neighbor_cost = NeighborSol->GetTotalCosts();

            if (sol_tabu_ls->Find(NeighborSol))
            {
                ++num_no_impro;

                if (neighbor_cost < cur_cost)   
                {
                    rm_score_indicator[rm_type] += _SIGAMA_3;
                    in_score_indicator[in_type] += _SIGAMA_3;
                }

                delete NeighborSol;
                continue;
            }


            //Neighbor solution not existed in current tabu list;
            if (neighbor_cost < BestSolCost - __EPSILON)
            {
                delete BestSol;
                BestSol = new Solution(NeighborSol);
                BestSolCost = neighbor_cost;


                
                rm_score_indicator[rm_type] += (_SIGAMA_1);
                in_score_indicator[in_type] += (_SIGAMA_1);

                best_sol_its = cur_it;


                delete CurSol;
                CurSol = NeighborSol;
                cur_cost = neighbor_cost;

                sol_pool->Remember(NeighborSol);
                num_no_impro = 0;

                continue;
            }


            if (neighbor_cost < cur_cost - __EPSILON)
            {
                delete CurSol;
                
                CurSol = NeighborSol;   
                cur_cost = neighbor_cost;

                sol_tabu_ls->Remember(CurSol);

                sol_pool->Remember(NeighborSol);
                

                v1.insert(v1.end(), v2.begin(), v2.end());

                v1.insert(v1.end(), v3.begin(), v3.end());

                for (auto s : v1)
                {
                    mv_tabu_ls->Remember(s);
                }


                //累积得分
                rm_score_indicator[rm_type] += _SIGAMA_2;
                in_score_indicator[in_type] += _SIGAMA_2;

                num_no_impro = 0;
                continue;
            }


            if (Accept.SimulatedAnnealing(CurSol, NeighborSol))     
            {

                delete  CurSol;
                CurSol = NeighborSol;   
                cur_cost = neighbor_cost;

                sol_tabu_ls->Remember(NeighborSol);                


                rm_score_indicator[rm_type] += _SIGAMA_4;
                in_score_indicator[in_type] += _SIGAMA_4;

                num_no_impro = 0;

                continue;
            }
            ++num_no_impro;
        }



        for (int i = 1; i <= 3; ++i)
        {
            double sc = 0.0;

            if (rm_nums_indicator[i] > 0)
            {
                sc = rm_score_indicator[i] / rm_nums_indicator[i];
                rm_operator_nums[i] += rm_nums_indicator[i];
            }
            Destroy.UpdataWeights(i, sc);

            if (in_nums_indicator[i] > 0)
            {
                sc = in_score_indicator[i] / in_nums_indicator[i];
                in_operator_nums[i] += in_nums_indicator[i];
            }
            Repair.UpdataWeights(i, sc);
        }

    }


    std::cerr << "the best solution: " << std::endl;
    BestSol->PrintSolutionInformation();

    delete CurSol;
    delete BestSol;

    delete sol_tabu_ls;
    delete mv_tabu_ls;

    return BestSolCost;
}




double _ALNS_RESTRICTED_MASTER    (   int n   )
{
    std::chrono::steady_clock::time_point alns_start, alns_finish;
    alns_start = std::chrono::steady_clock::now();


    std::ofstream ofs(logfile, std::ios::app);

    int JobsNumber = n;
    int best_sol_its = 0;
    Instance* instance = new Instance(JobsNumber, 2);
    SolutionPool* solution_pool = new SolutionPool(_POOL_MIN_SIZE, _CUT_OUT_RATE);
  
    double BestSolCost = _TABU_ALNS(    JobsNumber,
                                        best_sol_its, 
                                        instance,
                                        solution_pool
                                        );

    alns_finish = std::chrono::steady_clock::now();
    std::chrono::duration<double> alns_diff = alns_finish - alns_start;

    ofs << "The ALNS algorithm execution completed ! " << std::endl;
    ofs << "The ALNS algorithm execution time : "
        << std::fixed << std::setprecision(2) << alns_diff.count()
        << "s." << std::endl;

    ofs << "Best solution obtained by ALNS algorithm : " << BestSolCost << std::endl;
    ofs << best_sol_its << "iterations, find the best solution." << std::endl;
    ofs << "Best known lower bound : " << lower_bound << " , ";
    ofs << "Gap = " << 100 * (BestSolCost - lower_bound) / BestSolCost << "%." << std::endl;
    
    alns_time += alns_diff.count();


#if _RUN_RMP
    //run rmp model
    std::chrono::steady_clock::time_point model_start, model_finish;
    model_start = std::chrono::steady_clock::now();


    solution_pool->CutUseless();

    std::ostringstream os;
    solution_pool->PrintPool(os, lower_bound);

    ofs << os.str() << std::endl;


    GRBEnv env = GRBEnv();
    GRBModel rmp_model = GRBModel(env);
    MasterProblem master_problem(JobsNumber);
    master_problem.CreateInitialMP(rmp_model);
    master_problem.AddRestriction(rmp_model, solution_pool);

    double RMPSolCost = master_problem.SolveRMP(rmp_model);
    
    if (RMPSolCost < BestSolCost)
    {
        ofs << "The better solution is found by solve RMP !" << std::endl;
        BestSolCost = RMPSolCost;
    }


    model_finish = std::chrono::steady_clock::now();
    std::chrono::duration<double> model_diff = model_finish - model_start;

    ofs << "The RMP model execution completed ! " << std::endl;
    ofs << "The RMP model execution time : "
        << std::fixed << std::setprecision(2) << model_diff.count()
        << "s." << std::endl;

    ofs << "Best solution obtained by RMP model : " << BestSolCost << std::endl;
    ofs << "Best known lower bound : " << lower_bound << " , ";
    ofs << "Gap = " << 100 * (BestSolCost - lower_bound) / BestSolCost << "%." << std::endl;
    std::chrono::duration<double> total_diff = model_finish - alns_start;
    ofs << "Total time :" << std::fixed << std::setprecision(2) << total_diff.count() << "s." << std::endl;

    ofs << "========================================" << std::endl;
    ofs << std::endl;
    delete solution_pool;

    rmp_time += model_diff.count();
    total_time += total_diff.count();
#endif // _RUN_RMP



#if _RUN_POPULATION
    //evolutionary algorithm
    CPUTime["alns"] = alns_time;

    // create the initial solution pool :: size = 30
    solution_pool->InitializePopulation();

    //print solution pool
    {
        std::ostringstream os;
        solution_pool->PrintPool(os, lower_bound);
        ofs << os.str() << std::endl;
    }

    // statistic the information of initial solution pool
    {
        double avg_obj = 0., avg_gap = 0.;
        for (Solution*& s : solution_pool->GetPool()) {
            avg_obj += s->GetTotalCosts();
        }

        ObjValue["init_popu_max"] = solution_pool->GetPool().back()->GetTotalCosts();
        ObjValue["init_popu_min"] = solution_pool->GetPool().front()->GetTotalCosts();
        ObjValue["init_popu_avg"] = avg_obj / solution_pool->GetPoolSize();

        LBGap["init_popu_max"] = (ObjValue["init_popu_max"] - lower_bound) / ObjValue["init_popu_max"];
        LBGap["init_popu_min"] = (ObjValue["init_popu_min"] - lower_bound) / ObjValue["init_popu_min"];
        LBGap["init_popu_avg"] = (ObjValue["init_popu_avg"] - lower_bound) / ObjValue["init_popu_avg"];

        LBGap["init_popu_max"] = LBGap["init_popu_max"] < __EPSILON ? 0. : LBGap["init_popu_max"];
        LBGap["init_popu_min"] = LBGap["init_popu_min"] < __EPSILON ? 0. : LBGap["init_popu_min"];
        LBGap["init_popu_avg"] = LBGap["init_popu_avg"] < __EPSILON ? 0. : LBGap["init_popu_avg"];

        ofs << "The average Obj of initial solution pool is : " << ObjValue["init_popu_avg"] << std::endl;
        ofs << "The average Gap of initial solution pool is : " <<    LBGap["init_popu_avg"] << std::endl;
    }


     // run RMP1 based on the initial solution pool
    {
        std::chrono::steady_clock::time_point model_start, model_finish;
        model_start = std::chrono::steady_clock::now();

        // construct and run RMP model -- 1
        double RMPSolCost = 0.;
        {  
            GRBEnv env = GRBEnv();
            GRBModel rmp_model = GRBModel(env);
            MasterProblem master_problem(JobsNumber);
            master_problem.CreateInitialMP(rmp_model);
            master_problem.AddRestriction(rmp_model, solution_pool);

            RMPSolCost = master_problem.SolveRMP(rmp_model);

            if (RMPSolCost < BestSolCost) {
                ofs << "The better solution is found by solve RMP_1 !" << std::endl;
                BestSolCost = RMPSolCost;
            }
        }

        model_finish = std::chrono::steady_clock::now();
        std::chrono::duration<double> model_diff = model_finish - model_start;

        ofs << "The RMP_1 model execution completed ! " << std::endl;
        ofs << "The RMP_1 model execution time : "
            << std::fixed << std::setprecision(2) << model_diff.count()
            << "s." << std::endl;
        ofs << "Best solution obtained by RMP_1 model : " << BestSolCost << std::endl;
        ofs << "Best known lower bound : " << lower_bound << " , ";
        ofs << "Gap = " << 100 * (BestSolCost - lower_bound) / BestSolCost << "%." << std::endl;


        ObjValue["rmp_1"] = RMPSolCost; 
           LBGap["rmp_1"] = (BestSolCost - lower_bound) / BestSolCost;
           LBGap["rmp_1"] = LBGap["rmp_1"] < __EPSILON ? 0. : LBGap["rmp_1"];
         CPUTime["rmp_1"] = model_diff.count();
    }


    
    // run evolutionary algorithm
    {         
        std::chrono::steady_clock::time_point evolv_start, evolv_finish;
        evolv_start = std::chrono::steady_clock::now();
    
        solution_pool->Evolve(); 

        evolv_finish = std::chrono::steady_clock::now();
        std::chrono::duration<double> evolv_diff = evolv_finish - evolv_start;
        
        CPUTime["evolve"] = evolv_diff.count();
    }


    // stocastic the information of final population
    {
        if (solution_pool->GetPoolSize() != 40)
        {
            throw IllegalParameters("number of final population less than 40");
        }

        double avg_obj = 0., avg_gap = 0.;
        for (Solution*& s : solution_pool->GetPool()) {
            avg_obj += s->GetTotalCosts();
        }

        ObjValue["final_popu_max"] = solution_pool->GetPool().back()->GetTotalCosts();
        ObjValue["final_popu_min"] = solution_pool->GetPool().front()->GetTotalCosts();
        ObjValue["final_popu_avg"] = avg_obj / solution_pool->GetPoolSize();

        LBGap["final_popu_max"] = (ObjValue["final_popu_max"] - lower_bound) / ObjValue["final_popu_max"];
        LBGap["final_popu_min"] = (ObjValue["final_popu_min"] - lower_bound) / ObjValue["final_popu_min"];
        LBGap["final_popu_avg"] = (ObjValue["final_popu_avg"] - lower_bound) / ObjValue["final_popu_avg"];

        LBGap["final_popu_max"] = LBGap["final_popu_max"] < __EPSILON ? 0. : LBGap["final_popu_max"];
        LBGap["final_popu_min"] = LBGap["final_popu_min"] < __EPSILON ? 0. : LBGap["final_popu_min"];
        LBGap["final_popu_avg"] = LBGap["final_popu_avg"] < __EPSILON ? 0. : LBGap["final_popu_avg"];

        ofs << "The average Obj of final solution pool is : " << ObjValue["final_popu_avg"] << std::endl;
        ofs << "The average Gap of final solution pool is : " <<    LBGap["final_popu_avg"] << std::endl;
    }


    
    // run RMP2 after evolving 
    {
        std::chrono::steady_clock::time_point model_start, model_finish;
        model_start = std::chrono::steady_clock::now();

        // construct and run RMP model -- 2
        double RMPSolCost = 0.;
        {
            GRBEnv env = GRBEnv();
            GRBModel rmp_model = GRBModel(env);
            MasterProblem master_problem(JobsNumber);
            master_problem.CreateInitialMP(rmp_model);
            master_problem.AddRestriction(rmp_model, solution_pool);

            RMPSolCost = master_problem.SolveRMP(rmp_model);

            if (RMPSolCost < BestSolCost) {
                ofs << "The better solution is found by solve RMP_2 !" << std::endl;
                BestSolCost = RMPSolCost;
            }
        }

        model_finish = std::chrono::steady_clock::now();
        std::chrono::duration<double> model_diff = model_finish - model_start;

        ofs << "The RMP_2 model execution completed ! " << std::endl;
        ofs << "The RMP_2 model execution time : "
            << std::fixed << std::setprecision(2) << model_diff.count()
            << "s." << std::endl;
        ofs << "Best solution obtained by RMP_2 model : " << BestSolCost << std::endl;
        ofs << "Best known lower bound : " << lower_bound << " , ";
        ofs << "Gap = " << 100 * (BestSolCost - lower_bound) / BestSolCost << "%." << std::endl;

        std::chrono::duration<double> total_diff = model_finish - alns_start;
        ofs << "Total time :" << std::fixed << std::setprecision(2) << total_diff.count() << "s." << std::endl;
        ofs << "========================================" << std::endl;
        ofs << std::endl;

        ObjValue["rmp_2"] = RMPSolCost; 
           LBGap["rmp_2"] = (BestSolCost - lower_bound) / BestSolCost;
           LBGap["rmp_2"] = LBGap["rmp_2"] < __EPSILON ? 0. : LBGap["rmp_2"];
         CPUTime["rmp_2"] = model_diff.count();
         CPUTime["total"] = total_diff.count();
    }

    delete instance;  
    delete solution_pool;



#endif // _RUN_POOPULATION

    return BestSolCost;
}




