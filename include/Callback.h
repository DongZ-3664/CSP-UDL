#ifndef _Callback_H_
#define _Callback_H_


#include "MasterProblem.h"


class Callback : public GRBCallback
{
public:
    GRBVar** _X_Var = NULL;
    GRBVar** _Y_Var = NULL;
    GRBVar** _U_Var = NULL;
    GRBVar** _V_Var = NULL;

    double** _X_Value = NULL;
    double** _Y_Value = NULL;
    double** _U_Value = NULL;
    double** _V_Value = NULL;

    int nJobs;

    Callback(GRBVar** xx, GRBVar** yy, GRBVar** uu, GRBVar** vv, int nn);
    ~Callback();


    void FindSubTourX(int& tourlength, int*& xtour);

    void FindMinLoopX(int*& x_succ, int& xtoursize, int& start, int& min_size);

    void FindMinLoopY(int*& y_succ, int& start, int& min_size);

    void FindTourUYV(std::vector<std::vector<int>>& mis_placed_v);



    void ReSetMembers();
    void DeleteMembers();

    void OutPutUV();

    bool IsFeasible();

    void GetPath(int*& path);

protected:

    void callback();




};


#endif // !_Callback_H_
