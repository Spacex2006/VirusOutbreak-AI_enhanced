#include<iostream>
#include<vector>
#include "node.h"
#include<queue>
#include<algorithm>
#include<execution>
#include<cmath>
#include<array>
#include<random>

const double CONSTANT= 1.0;
const double INFECTION_RATE = 1.0;
const int OG_TIME = 5;
const double SURVIVAL_RATE = 0.7;
const int GENERATIONS = 365;


void simulate(std::vector<Node> &infected, std::vector<Node> &nodes){
    std::vector<Node> dead;
    std::vector<Node> saved;
    for(int i =0;i<GENERATIONS;i++){
        simulate_one_generation(infected, nodes, dead, saved);   
        std::cout<< i<< " "<< infected.size()<< " "<< dead.size()<< " " << saved.size()<<  std::endl;
    }

}

void simulate_one_generation(std::vector<Node> &infected, std::vector<Node> &nodes, std::vector<Node> &deads, std::vector<Node> &saved){
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<double> unif(0.0, 1.0);
    auto end = infected.end();
    std::for_each(std::execution::par, infected.begin(), end, [&](Node &node){
        for(Node &normal_node:nodes){
            if(normal_node.infected==true|| normal_node.dead==true|| normal_node.saved==true){
                continue;
            }
            else{
                double number = unif(engine);
                double threshold = std::exp(CONSTANT * std::sqrt((normal_node.x-node.x)*(normal_node.y-node.y)+(normal_node.x-node.x)*(normal_node.y-node.y)));
                if(number<threshold){
                    number = unif(engine);
                    if(number<INFECTION_RATE){
                        normal_node.infected = 1;
                        normal_node.time_left = OG_TIME;
                        infected.push_back(normal_node); 
                    }
                }

            }
        }
        node.time_left--;
        if(!node.time_left){
            infected.erase(std::find(infected.begin(), end, node));
            double survival_number = unif(engine);  
            if(survival_number<SURVIVAL_RATE){
                node.infected=false;
                node.saved=true;
                saved.push_back(node);
            }
            else{
                node.dead=true;
                deads.push_back(node);
                
            } 
        }

    });


}




int main(){






    return 0;
}
