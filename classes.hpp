// structures

struct spars;
struct svars;

// classes

class rndfile;
class le;

//************************                ******************************
//************************   parameters   ******************************
//************************                ******************************

struct spars
{
    double x0, v0, dt;
    double tau, D; // if constant, these values will be used
    double mintime, maxtime = -1;
    unsigned long numtimes, N_exper, N_traj, D_max;
    bool logtimes;

    std::list<double> times;
    std::chrono::duration<double> time_span;
};

struct svars
{
    double tau, D; // variables
    std::mt19937_64 *gen;
    std::map<double,double> xs, vs;
};

//*********************                   ******************************
//*********************     Levy_extr     ******************************
//*********************                   ******************************

class Levy_extr
{
//    Provides random numbers distributed with
//    extremal a-stable law
    protected:
        double alpha;
        std::uniform_real_distribution<double> *unif;
        std::mt19937_64 *gen;

    public:
        Levy_extr(double);
        ~Levy_extr();
        Levy_extr& operator >> (double&);
};

//*********************                           **********************
//*********************     Levy_extr_div_tau     **********************
//*********************                           **********************

class Levy_extr_div_tau
{
    // Class that has everything to generate a random variable
    // which obeys the distribution L_\alpha^{-\alpha}(\tau)/\tau

    protected:
        double alpha, A, k;
        std::vector<double> cdfx, cdfy;
        std::uniform_real_distribution<double> *unif;
        std::mt19937_64 *gen;
        std::ifstream ifile;

    public:
        Levy_extr_div_tau(double);
        ~Levy_extr_div_tau();
        double cdf_inv(double);
        Levy_extr_div_tau& operator >> (double&);
};

//************************            **********************************
//************************  langevin  **********************************
//************************            **********************************

class le{

    protected:
        double D, tau, x0, v0;

        std::normal_distribution<double> norm;
        std::mt19937_64 *gen;

    public:
        double x, v, t;

        le(spars &, std::mt19937_64 *, double, double);
        ~le();
        double step(double);
};

//************************            **********************************
//************************  mystring  **********************************
//************************            **********************************

class mystring
{
    // Python-like string handling
    private:
        std::string orig;

    public:
        mystring();
        ~mystring();
        mystring& operator = (std::string&);
        std::vector<mystring> split(std::string);
        void getlinefromfile(std::ifstream&);
        size_t find(std::string);
        operator double();
        operator std::string();
};
