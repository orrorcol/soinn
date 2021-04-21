#ifndef __TEST_H__
#define __TEST_H__

#ifdef DUCH
#define DTYPE unsigned char
#else
#define DTYPE float
#endif

namespace test
{

// hnsw
static const int param_m = 16;
static const int param_ef_construction = 200;
static const int param_ef_search = 200;

// soinn
static int param_lamda = 100;
static int param_dead_age = 100;
static double param_c_noise = 0.5;
}

#endif
