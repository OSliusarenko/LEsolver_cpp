//************************               ************************************
//************************  worker (LE)  ************************************
//************************               ************************************

void calc_LE(spars& pars, svars& vars, int thrd_num)
{

    double dt = pars.dt;

    // simulate a trajectory

    le rw(pars, vars.gen, vars.tau, vars.D);
    //rw.step(dt);

    for (auto t_max=pars.times.begin();
         t_max != pars.times.end(); ++t_max)
    {

        while(true)
        {
            if(rw.t >= *t_max)
            {
                vars.xs[*t_max] += rw.x;
                vars.vs[*t_max] += rw.v;
                break;
            }
            else
            {
                rw.step(dt);
            };
        };
    };

};


//************************             *********************************
//************************    save()   *********************************
//************************             *********************************

void save(std::string xfname, std::string vfname,
          svars *vars, spars& pars)
{
    std::ofstream xofile(xfname, std::fstream::out | std::fstream::app);
    std::ofstream vofile(vfname, std::fstream::out | std::fstream::app);

    if(xofile.is_open() && vofile.is_open())
    {
        double x, v;
        std::stringstream ss;

        ss << "#subs_traj = " << pars.N_exper * num_threads \
              <<" time = " << round(pars.time_span.count()/36.)/100. \
              << " hours." << std::scientific << std::endl;
        xofile << ss.str() << "#t\tx" << std::scientific << std::endl;
        vofile << ss.str() << "#t\tv" << std::scientific << std::endl;

        for (auto t_max=pars.times.begin();
             t_max != pars.times.end(); ++t_max)
        {
            x = 0;
            v = 0;

            for (unsigned int proc=0; proc<num_threads; proc++)
            {
                x += vars[proc].xs[*t_max];
                v += vars[proc].vs[*t_max];
            };

            // Arithmetic averaging trajectories

            xofile << *t_max << '\t'
                   << x / sqrt((double)(pars.N_exper*num_threads))
                   << std::endl;
            vofile << *t_max << '\t'
                   << v / sqrt((double)(pars.N_exper*num_threads))
                   << std::endl;
        };

        xofile.close();
        vofile.close();
    }
    else
    {
        std::cerr<<"Cannot open file(s)!"<<std::endl;
    };
};
