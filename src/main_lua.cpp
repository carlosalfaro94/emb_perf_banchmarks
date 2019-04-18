#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <thread>

// Lua is written in C, so compiler needs to know how to link its libraries
extern "C"
{
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

using namespace std;
using namespace chrono;


inline void call_lua_function(lua_State* state, uint64_t param)
{
    lua_pushinteger(state, param);
    lua_pcall(state, 1, 0, 0);
    return;
}


int main(int argc, char* argv[])
{
    if (argc != 6) 
    {
        cout << "Usage: " << argv[0] << " <script_file> <function> <duration> <rate> <input_file.txt>\n";
        return EXIT_FAILURE;
    }

    string script_fname = (const char*) argv[1];
    string func_name    = (const char*) argv[2];
    seconds duration    = seconds(atoi(argv[3]));
    int rate            = atoi(argv[4]);
    const char* fname   = (const char*) argv[5];
    /*First messages....*/
    cout << "\n  running benchmark with:\n"
         << "\n\tFile       : " << script_fname
         << "\n\tFunction   : " << func_name
         << "\n\tRate       : " << rate << " ops/s"
         << "\n\tDuration   : " << atoi(argv[3]) << "s\n";

    if( rate <= 0 || duration.count() <= 0 )
    {
        cout << "\n\tbad input parameters...\nexit...\n";
        return -1;
    }

    auto sleep_interval = microseconds( int(1e6/rate) );
    if (sleep_interval.count() <= 0)
    {
        cout << "\tbad operations rate [" << rate << "]...\n  exit...\n";
        return -1;
    }

    /*Random generation initialzation*/

    random_device rd;  //Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<>  rate_random_gen(-sleep_interval.count()*0.5, sleep_interval.count()*0.5);   // random time for sleeping generator (to be sum to sleep_interval..)

    /*    Data loading    */
    cout << "Loading input data..." << endl;
    ifstream input_file(fname);
    if (!input_file)
    {
        cout << "Unable to open  file " << fname << "...\n   exit...\n" ;
        return EXIT_FAILURE;
    }

    vector<uint64_t> data;
    uint64_t num;
    while( !input_file.eof() )
    {
        input_file >> num;
        data.push_back(num);
    }
    auto data_size = data.size();
    cout << "Succesfully loaded input data" << endl;
    cout << "Records read: " << data_size << endl;

    cout << "Setting up Lua environment...\n";
    lua_State* lua_state = luaL_newstate();

    luaL_openlibs(lua_state);

    int status = luaL_dofile(lua_state, script_fname.c_str());

    if(status != 0)
    {
        cout << "Error loading and executing lua file " << script_fname << "...\nExiting...\n";

        return EXIT_FAILURE;
    }
    lua_getglobal(lua_state, func_name.c_str());
    cout << "Lua environment set up\n\n";
    
    microseconds function_call_duration{0};
    int nrep = 0;
    int index;

    auto start_time = system_clock::now();
    while( true )
    {
        auto process_time = system_clock::now();
        if( process_time - start_time > duration )
            break;
        
        index = nrep % data_size;
        auto inner_start_time = high_resolution_clock::now();
        call_lua_function(lua_state, data.at(index));
        auto inner_end_time = high_resolution_clock::now();

        function_call_duration += duration_cast<microseconds>(inner_end_time - inner_start_time);
        ++nrep;
        this_thread::sleep_for( sleep_interval + microseconds( rate_random_gen(gen) ) );
    }
    auto end_time = system_clock::now();
    float total_duration = ((float) duration_cast<microseconds>( end_time - start_time ).count())/1000000.;

    cout << "\n\nStatistical Report:\n";
    cout << "\n\tBenchmark duration:            " << total_duration << "s"
        << "\n\tScript file:                    " << script_fname
        << "\n\tFunction:                       " << func_name
        << "\n\tTotal number of operations:     " << nrep
        << "\n\tProduced operation rate:        " << ((float) nrep)/(total_duration) << " msg/s"
        << "\n\tAvg time per function call:     " << ((float) function_call_duration.count() )/ nrep << " us/function call"
        << endl;

    
    cout << "Execution finished gracefully" << endl;
    return EXIT_SUCCESS;
}