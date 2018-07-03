#include <fstream>
#include <iostream>
#include <map>
#include <list>
#include <random>
#include <chrono>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <cstdio>

#include "classes.hpp"
#include "classes.cpp"

std::mutex m_screen;


//**********************************************************************
//************************          ************************************
//************************  main()  ************************************
//************************          ************************************
//**********************************************************************

static const int num_threads = 32;

#include "workers.cpp"

int main( int argc, char *argv[] ) {

    std::chrono::steady_clock::time_point tp1, tp2;

    std::string xofname = "xs.dat";
    std::string vofname = "vs.dat";
    std::string rofname = "randoms_used.dat";

    if(argc>1)
    {
        for(int i=1; i<argc; i++)
        {
            if((std::string)argv[i]=="-n")
            {
                if(i+2>=argc) std::cerr << "Invalid number of parameters"
                                        << std::endl;
                else
                {
                    xofname = (std::string)argv[i+1];
                    vofname = (std::string)argv[i+2];
                };
            };
        };
    };

    // delete data files before going on
    // since we'll open them for append

    std::remove(xofname.c_str());
    std::remove(vofname.c_str());
    std::remove(rofname.c_str());

    bool D_const = false;    //
    bool tau_const = false;  //

    bool log_used_randoms = true;  //

    svars vars[num_threads];
    spars pars;

    pars.x0 = 0.0;
    pars.v0 = 0.0;

//  if we use default for maxtime (-1), trajectory is recorded every dt
    pars.mintime = 1e-1;
    pars.maxtime = 1e4;
    pars.numtimes = pow(2, 4) + 1;
    pars.N_exper = 9600; // number of subsidiary trajectories
    pars.N_traj = 1e4; // number of eventual trajectories
    pars.D_max = 1e4; // maximum D limitation
    pars.logtimes = true;

    pars.D = 2.0;
    pars.tau = 0.5;

    pars.dt = 1e-2;

    pars.N_exper /= num_threads;
    pars.numtimes --; // we'll start from 0

    Levy_extr Ds(0.75);
    Levy_extr_div_tau taus(0.5);

    // defining times of 'snapshots'
    for(double j=0; j<=pars.numtimes; j++)
    {
        double t;

        if(pars.logtimes)
        {
            if(pars.maxtime > 0)
                t = pars.mintime*pow(pars.maxtime/pars.mintime, (double)j/
                                 (double)pars.numtimes);
        }
        else
        {
            if(pars.maxtime > 0)
                t = pars.mintime + (pars.maxtime-pars.mintime) * \
                               (double)j / (double)pars.numtimes;
            else
                t = pars.mintime + pars.dt * (double)j;
        };

        pars.times.push_back(t);
    }

    std::ofstream rfile(rofname);

    if (log_used_randoms)
        rfile << "#D\ttau\n";

    // start the stopwatch
    tp1 = std::chrono::steady_clock::now();

    // allocating threads
    std::thread th[num_threads];

    // for each thread define a random seed and define the generator
    for (int i = 0; i < num_threads; i++)
    {
        unsigned seed = std::chrono::system_clock \
                     ::now().time_since_epoch().count() + i;


        vars[i].gen = new std::mt19937_64(seed);

        std::stringstream msg;
        msg << "Thrd #" \
        << i \
        << " with seed " \
        << seed \
        << std::endl;

        std::cout << msg.str();
    };

    if (D_const)
        std::cout << "Using constant D = " << pars.D << std::endl;
    if (tau_const)
        std::cout << "Using constant tau = " << pars.tau << std::endl;

// main cycles
    long each_k;
    if (pars.N_traj >= 10)
    {
        each_k = pars.N_traj / 10;
    } else
        each_k = 1;

    for (unsigned long k = 0; k < pars.N_traj; k++)
    {
        if(k % each_k == 0)
            {
                if(k * 10 / each_k <= 100)
                    std::cout << k / each_k * 10 << " " << std::flush;
            };
        for (int i = 0; i < num_threads; i++)
        {
            for (auto t_max=pars.times.begin();
                 t_max != pars.times.end(); ++t_max)
            {
                vars[i].xs[*t_max] = 0;
                vars[i].vs[*t_max] = 0;
            };
        };

        for (unsigned long exper = 0; exper < pars.N_exper; exper ++)
        {
            // Launch a group of threads

            for (int i = 0; i < num_threads; i++)
            {
                if (!D_const)
                {
                    Ds >> vars[i].D;
                    while (true)
                    {
                        if (vars[i].D <= pars.D_max)
                            break;
                        else
                            Ds >> vars[i].D;
                    };
                }
                else vars[i].D = pars.D;
                if (!tau_const)
                    taus >> vars[i].tau;
                else vars[i].tau = pars.tau;

                if (log_used_randoms)
                    rfile << vars[i].D << "\t" << vars[i].tau << std::endl;

                th[i] = std::thread(calc_LE, std::ref(pars),
                                    std::ref(vars[i]), i);
            };

            // Join the threads with the main thread
            for (int i = 0; i < num_threads; ++i)
            {
                th[i].join();
            };

        };

        // stop the stopwatch and save data
        tp2 = std::chrono::steady_clock::now();
        pars.time_span = \
                std::chrono::duration_cast<std::chrono::duration<double>>(tp2 - tp1);

        save(xofname, vofname, vars, pars);
    };
    //
    std::cout << std::endl;

    std::cout << "It took me " << round(pars.time_span.count()/36.)/100.
              << " hours."<< std::endl;

    //clean up
    for (int i = 0; i < num_threads; i++)
    {
        delete vars[i].gen;
    };

    return 0;
};


