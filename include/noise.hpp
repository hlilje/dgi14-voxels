double easeCurve(double t);

// Return value: -1 ... 1
double noise2d(int x, int y, int seed);
double noise3d(int x, int y, int z, int seed);

double noise2d_gradient(double x, double y, int seed);
double noise3d_gradient(double x, double y, double z, int seed);

double noise2d_perlin(double x, double y, int seed, int octaves,
        double persistence);

double noise2d_perlin_abs(double x, double y, int seed, int octaves,
        double persistence);

double noise3d_perlin(double x, double y, double z, int seed, int octaves,
        double persistence);

double noise3d_perlin_abs(double x, double y, double z, int seed, int octaves,
        double persistence);

// noise.cpp
#define NOISE_MAGIC_X 1619
#define NOISE_MAGIC_Y 31337
#define NOISE_MAGIC_Z 52591
#define NOISE_MAGIC_SEED 1013
