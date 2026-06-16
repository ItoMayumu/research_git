#include <iostream>
#include <vector>
#include <cmath>

const int NX = 64;
const int NY = 64;
const double DX = 0.1;
const double DY = 0.1;
const double DT = 0.001;
const double NU = 0.01;
const double RHO = 1.0;

inline int idx(int x, int y){
    return y * NX + x;
}

int main(){
    std::vector<double> u(NX * NY, 0.0); // velocity in x direction
    std::vector<double> v(NX * NY, 0.0); // velocity in y direction
    std::vector<double> p(NX * NY, 0.0); // pressure
    std::vector<double> u_star(NX * NY, 0.0);
    std::vector<double> v_star(NX * NY, 0.0);

    int max_steps = 1000;

    for(int step=0;step < max_steps; ++step){
        for(int y=1;y<NY-1;++y){
            for(int x=1; x<NX-1; ++x){
                int c = idx(x,y);

                double diffusion_u = NU * (
                    (u[idx(x+1,y)] - 2*u[c] + u[idx(x-1,y)]) / (DX*DX) +
                    (u[idx(x,y+1)] - 2*u[c] + u[idx(x,y-1)]) / (DY*DY)
                );
                u_star[c] = u[c] + DT * diffusion_u;
            }
        }
        for(int iter = 0; iter < 100; ++iter){
            std::vector<double> p_old = p;
            for(int y=1;y<NY-1;++y){
                for(int x=1; x<NX-1; ++x){
                    int c = idx(x,y);
                    double div_u_star = (u_star[idx(x+1,y)] - u_star[idx(x-1,y)]) / (2*DX) + 
                                        (v_star[idx(x,y+1)] - v_star[idx(x,y-1)]) / (2*DY);
                                        p[c] = 0.25 * (p_old[idx(x+1,y)] + p_old[idx(x-1,y)] + p_old[idx(x,y+1)] + p_old[idx(x,y-1)] - RHO * div_u_star * DX * DY);
                }
            }
        }
        for(int y=1;y<NY-1;++y){
            for(int x=1; x<NX-1; ++x){
                int c = idx(x,y);
                u[c] = u_star[c] - (DT / RHO) * (p[idx(x+1,y)] - p[idx(x-1,y)]) / (2*DX);
                v[c] = v_star[c] - (DT / RHO) * (p[idx(x,y+1)] - p[idx(x,y-1)]) / (2*DY);
            }

        }

    }
    std::cout << "Simulation completed." << std::endl;
    return 0;
}