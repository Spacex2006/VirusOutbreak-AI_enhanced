#include<iostream>
#include<vector>
#include "node.h"
#include<queue>
#include<algorithm>
#include<execution>
#include<cmath>
#include<array>
#include<random>
#include <utility>
#include <string>
#include <exception>
#include <fstream>
#include <sstream>

struct Barrio {
    int district_id;
    std::string district_name;
    std::string barrio_code;
    std::string barrio_name;
    double latitude;
    double longitude;
    int population_2020;
};

// Function to trim potential '\r' line endings on Linux/macOS
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::vector<Barrio> loadBarriosFromCSV(const std::string& filepath) {
    std::vector<Barrio> barrios;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filepath);
    }

    std::string line;
    // Skip the CSV Header row (CODDIS,NOMDIS,CODBAR,NOMBAR,latitude,longitude,population_2020)
    if (!std::getline(file, line)) {
        return barrios; 
    }

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string coddis_str, nomdis, codbar, nombar, lat_str, lon_str, pop_str;

        if (std::getline(ss, coddis_str, ',') &&
            std::getline(ss, nomdis, ',') &&
            std::getline(ss, codbar, ',') &&
            std::getline(ss, nombar, ',') &&
            std::getline(ss, lat_str, ',') &&
            std::getline(ss, lon_str, ',') &&
            std::getline(ss, pop_str, ',')) {

            Barrio b;
            b.district_id = std::stoi(trim(coddis_str));
            b.district_name = trim(nomdis);
            b.barrio_code = trim(codbar);
            b.barrio_name = trim(nombar);
            b.latitude = std::stod(trim(lat_str));
            b.longitude = std::stod(trim(lon_str));
            b.population_2020 = std::stoi(trim(pop_str));

            barrios.push_back(b);
        }
    }

    file.close();
    return barrios;
}

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



int rand(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double prob = dist(gen); // e.g., 0.3742
    return prob;
}

int main() {
    const std::string csv_path = "madrid_barrios_centroids_population_2020.csv";

    std::vector<Barrio> barrios = loadBarriosFromCSV(csv_path);

    try {
        std::cout << "Successfully loaded " << barrios.size() << " barrios.\n\n";

        // Display sample entries to verify correctness
        std::cout << "First 3 Barrio Nodes:\n";
        for (size_t i = 0; i < std::min<size_t>(3, barrios.size()); ++i) {
            const auto& b = barrios[i];
            std::cout << "[" << b.barrio_code << "] " << b.barrio_name 
                      << " (" << b.district_name << ")\n"
                      << "    Centroid: (" << b.latitude << ", " << b.longitude << ")\n"
                      << "    Population (2020): " << b.population_2020 << "\n\n";
        }
    } 
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    Node nodes;
    for(int i=0;i<barrios.size();i++){
        auto &b=barrios[i];
        for(int j;j<b.population_2020;j++){
            Node n;
            float x1,y1;
            while(1){
                x1=rand();
                y1=rand();
                x1=4*x1;
                x1=x1-2;
                int graph=std::exp(-x1*x1);
                if(y1<graph) break;
            }
            float x2,y2;
            while(1){
                x2=rand();
                y2=rand();
                x2=4*x2;
                x2=x2-2;
                int graph=std::exp(-x2*x2);
                if(y2<graph) break;
            }
            n.x=b.latitude-x1;
            n.y=b.longitude-x2;
        }
    }

    return 0;
}
