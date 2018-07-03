// classes

//************************            **********************************
//************************  langevin  **********************************
//************************            **********************************

le::le(spars &pars, std::mt19937_64 *gen_,
       double tau_, double D_)
               : norm(0.0,1.0) // preinit functions go here
{
    x0 = pars.x0;
    v0 = pars.v0;

    tau = tau_;
    D = D_;

    x = x0;
    v = v0;
    t = 0.0;

    gen = gen_;
};

le::~le()
{

};

double le::step(double dt)
{
//  Performs one step in time

    v += -v * dt / tau + sqrt(2.*dt*D) * norm(*gen);
    x += v * dt;
    t += dt;

    return t;
};

//*********************                   ******************************
//*********************     Levy_extr     ******************************
//*********************                   ******************************

Levy_extr::Levy_extr(double alpha_)
{
    alpha = alpha_;

    unsigned seed = std::chrono::system_clock \
                     ::now().time_since_epoch().count() + 5;

    gen = new std::mt19937_64(seed);
    unif = new std::uniform_real_distribution<double>(0.0, 1.0);
};

Levy_extr&  Levy_extr::operator >> (double& d)
{
//  draws a random number

    double r1 = (*unif)(*gen)*M_PI - M_PI_2;
    double r2 = (*unif)(*gen);

    d = sin(alpha * (r1 + M_PI_2)) / \
        pow(cos(r1), 1. / alpha) * \
        pow(cos(r1 - alpha*(r1 + M_PI_2)) /
            (-log(r2)), (1. - alpha) / alpha);

    return *this;
};

Levy_extr::~Levy_extr()
{
    delete unif;
    delete gen;
};

//************************                           **********************
//************************     Levy_extr_div_tau     **********************
//************************                           **********************

Levy_extr_div_tau::Levy_extr_div_tau(double alpha_)
{
//  Defining variables, reading data files

    alpha = alpha_;
    std::stringstream ss;
    mystring line;
    double alpha1;

    ss << "cdfs/cdftau_a" << alpha << ".dat";

    std::string fname = ss.str();

    ifile.open(fname);

    if(ifile.is_open())
    {
        //  reading the cdf data file
        line.getlinefromfile(ifile);
        alpha1 = (double)(line.split("alpha=")[1].split(" ")[0]);
        k = (double)(line.split("k=")[1].split(" ")[0]);
        A = (double)(line.split("A=")[1].split(" ")[0]);

        if (alpha1 != alpha)
        {
            std::cerr << "Failed to load cdf file!" << std::endl;
            exit(-1);
        }
        else
        {
            std::cout << "taus: alpha = " << alpha \
                      << " k = " << k \
                      << " A = " << A << std::endl;
            while(true)
            {
                line.getlinefromfile(ifile);
                if (ifile.eof()) break;

                cdfx.push_back((double)(line.split("\t")[0]));
                cdfy.push_back((double)(line.split("\t")[1]));
            };

        };

        ifile.close();
    }
    else
    {
        std::cerr << "Warning: cannot open file " \
                  << fname << std::endl;
        std::cerr << "Ignore this message if you use constant values" \
                  << std::endl;
    };

    unsigned seed = std::chrono::system_clock \
                         ::now().time_since_epoch().count() \
                    + 255;

    gen = new std::mt19937_64(seed);
    unif = new std::uniform_real_distribution<double>(0.0, 1.0);

};

double Levy_extr_div_tau::cdf_inv(double yy)
{
//    Inverses cdf, for known cdf value gives the corresponding
//    argument x.

    for (unsigned i = 0; i < cdfy.size() - 1; i++)
    {
        if(yy == cdfy[i])
                return cdfx[i];
        else
            if(cdfy[i] < yy && yy <= cdfy[i+1])
                return(yy - (cdfy[i] * cdfx[i+1] - cdfy[i+1] * cdfx[i]) /
                       (cdfx[i+1] - cdfx[i])) / ((cdfy[i+1] - cdfy[i]) /
                                                 (cdfx[i+1] - cdfx[i]));
    };
    if(yy >= cdfy.back())
    {
        // this practically does not happen!?
        return pow(((alpha + 0) / k * (cdfy[-1] - yy) +
               pow(cdfx[-1], (-(alpha + 0)))), (-1./(alpha + 0)));
    }
    else
    {
        return 0;  // not actually the best way-out
    };
};

Levy_extr_div_tau&  Levy_extr_div_tau::operator >> (double& d)
{
//    Actually inverses the cdf and draws a random number
    d = cdf_inv((*unif)(*gen));
    return *this;
};

Levy_extr_div_tau::~Levy_extr_div_tau()
{
    delete unif;
    delete gen;
};


//************************            **********************************
//************************  mystring  **********************************
//************************            **********************************

mystring::mystring()
{
};

mystring::~mystring()
{
};

mystring& mystring::operator = (std::string &orig_)
{
    orig = orig_;

    return *this;
};

size_t mystring::find(std::string substring)
{
    size_t pos = orig.find(substring);
    return pos;
};

mystring::operator std::string()
{
    return orig;
}

mystring::operator double()
{
    return std::stod(orig);
}

std::vector<mystring> mystring::split(std::string substring)
{
    std::vector<mystring> res;
    mystring mstr1, mstr2;
    std::string str1, str2;
    size_t pos = orig.find(substring);

    str1 = orig.substr(0, pos);
    str2 = orig.substr(pos + substring.size());

    mstr1 = str1;
    mstr2 = str2;

    res.push_back(mstr1);
    res.push_back(mstr2);

    return res;
};

void mystring::getlinefromfile(std::ifstream &ifile)
{
    std::getline(ifile, orig);
};
