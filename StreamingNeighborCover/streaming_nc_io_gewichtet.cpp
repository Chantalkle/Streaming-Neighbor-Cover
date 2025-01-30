#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <ctime>
#include <algorithm>
#include <random>
#include <chrono>
#include <bits/stdc++.h> 

using namespace std;

// Funktionen für den Neighbor Cover Algorithmus
void updateISvektor(std::vector<int>& IS, std::vector<int>& RN,  std::vector<std::tuple<int, int>>& ES, std::vector<int>& ISD, std::vector<int>& VC){
    // Funktion zum updaten des Independent Set Vektors für den Neighbor Cover Algorithmus
    // IS: Independent Set vektor;  RN: Vektor der Priorität der Knoten RN[v] := Position des Knoten v in Prio Liste; 
    // ES: Kantenstream , ISD: Vektor für dyn. Graph Streams, zählt Anzahl der Kanten zu Knoten mit niedrigerer Prio; 
    // VC: Vertex Cover Vektor
    for (int i = 0; i < ES.size(); i++){
        // Jede Kante des Graphstreams durchgehen und den ISD Vektor entsprechend aktualisieren
        int u = std::get<0>(ES[i]);
        int v = std::get<1>(ES[i]);
 
        if (IS[v] == 0 && IS[u] == 0 && VC[v] == 0 && VC[u] == 0) {
            // die Kante wird nur berücksichtigt, wenn noch keiner der Knoten in IS oder VC ist
            if (RN[v] < RN[u]){
                ISD[u] = 1; 
            } 
            else {
                ISD[v] = 1;
            }
        }
    }
    // Nun den IS Vektor updaten: 
    for (int i = 0; i < ISD.size(); ++i) {
        // wenn ein Knoten unter den betrachteten Kanten keine inzidente zu einem Knoten mit schlechterer Priorität hat, wird er zu IS hinzugefügt
        if (ISD[i] == 0){
            IS[i] = 1;
        }
    }
}

void updateVCvektor(std::vector<int>& IS, std::vector<int>& VC, std::vector<tuple<int,int>>& ES){
    // Funktion zum updaten des Vertex Cover Vektors für Neighbor Cover
    // IS: Independent Set Vektor, VC: Vertex Cover Vektor, ES Kantenstream
    for (int i = 0; i < ES.size(); i++){
        // Jede Kante des Graphstreams durchgehen und den VC Vektor entsprechend aktualisieren
        int u = std::get<0>(ES[i]);
        int v = std::get<1>(ES[i]);

        // Konten zu VC hinzufügen, wenn er einen Nachbarn im Independent Set hat
        if (IS[u] == 0 && IS[v] == 1) {
            VC[u] = 1;
        } else if (IS[v] == 0 && IS[u] == 1) {
            VC[v] = 1;
        }          
    }
}

std::vector<int> weightedShuffle(const std::vector<int>& weights) {
    // Funktion für gewichtetes Sampling und Sortierung der Indizes
    // Zufallszahlengenerator initialisieren
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    // Zufallszahlen generieren
    std::vector<double> vals(weights.size());
    for (size_t i = 0; i < weights.size(); ++i) {
        vals[i] = dist(gen);
    }

    // Gewichtete Sampling-Logik
    for (size_t i = 0; i < vals.size(); ++i) {
        vals[i] = std::pow(vals[i], 1.0 / static_cast<double>(weights[i]));
    }

    // Indizes und Werte paaren
    std::vector<std::pair<double, int>> valIndices;
    for (size_t i = 0; i < vals.size(); ++i) {
        valIndices.emplace_back(vals[i], i);
    }

    // Sortieren nach Werten
    std::sort(valIndices.begin(), valIndices.end(), [](const auto& x, const auto& y) {
        return x.first > y.first;
    });

    // Rückgabe der Positionen
    std::vector<int> positions(weights.size());
    for (size_t i = 0; i < valIndices.size(); ++i) {
        positions[valIndices[i].second] = i;
    }

    return positions;
}


bool allOnes(std::vector<int> IS, std::vector<int> VC){
    // Überprüfe ob alle Knoten in Is oder VC
    for (int i = 0; i < IS.size(); i++){
        if (IS[i] == 0 && VC[i] == 0){
            return false;
        }
    }
    return true;
}

std::pair<int, vector<int>> NeighborCover(int numNodes, std::vector<std::tuple<int, int>>& Edges, std::vector<int>& node_weights){
    // Funktion zum Anwenden des Neighbor Cover Algorithmus auf einen Graphen
    std::vector<int> VC(numNodes, 0);  // VC-Vector mit der Groesse der generierten Knotenmenge, alle auf 0 initialisiert
    std::vector<int> IS(numNodes, 0);  // IS-Vector mit der Groesse der generierten Knotenmenge, alle auf 0 initialisiert
    std::vector<int> ISD(numNodes, 0); // Vektor um ausgehende Kanten zu markieren
    bool allin = false; // Variable zum Überprüfen, ob alle Knoten in VC oder IS
    
    // Erstellen der zufälligen Knotenreihenfolge
    // Vektor zum Speichern der zufälligen Knotenreihenfolge
    std::vector<int> randomNodes = weightedShuffle(node_weights);

    int count = 0; // Counter für Anzahl an Stream-Durchläufen
    while (!allin){
        updateISvektor(IS, randomNodes, Edges, ISD, VC);
        updateVCvektor(IS, VC, Edges);
        count += 2; //(1 Durchlauf für IS, einer für VC)
        allin = allOnes(IS,VC);
        
        for (int i = 0; i < ISD.size(); ++i) {
             ISD[i] = 0;
        }
        

    }
    
    return std::make_pair(count, VC);

}

std::vector<int>  GreedyVC(int numNodes, std::vector<std::pair<int,int>>& Edges){
    // Greedy für Vertex Cover (für ungewichtete Graphen)
    std::vector<int> GVC(numNodes, 0); //Vektor für Vertex Cover
    for( int i = 0; i < Edges.size(); i++){
        int u  = Edges[i].first;
        int v  = Edges[i].second;
        if (GVC[u] == 0 && GVC[v] == 0){
            GVC[u] = 1;
            GVC[v] = 1;
        }
    }
    return GVC;
}


struct GraphData {
    int n = 0;
    int m = 0;
    double p = -1.0;
    int d = -1;
    std::vector<std::tuple<int, int>> edges; // Ändere hier auf tuple
};


// Funktion zum Einlesen der Graph-Daten aus einer .txt Datei
GraphData parseGraphFile(const std::string& filePath) {
    std::ifstream file(filePath);
    GraphData graphData;
    std::string line;

    // Lese die erste Zeile und extrahiere n und den spezifischen Parameter (m, p oder d)
    if (std::getline(file, line)) {
        if (line.find(",m=") != std::string::npos) {
            std::sscanf(line.c_str(), "%%n=%d,m=%d", &graphData.n, &graphData.m);
        } else if (line.find(",p=") != std::string::npos) {
            char paramStr[50];
            std::sscanf(line.c_str(), "%%n=%d,p=%s", &graphData.n, paramStr);
            graphData.p = std::stod(paramStr);
        } else if (line.find(",d=") != std::string::npos) {
            std::sscanf(line.c_str(), "%%n=%d,d=%d", &graphData.n, &graphData.d);
        }
    }

    // Lese die Kanten (u,v Paare) ab der zweiten Zeile
    int u, v;
    while (file >> u) {
        if (file.peek() == ',') {
            file.ignore();  // Ignoriere das Komma
            file >> v;
            graphData.edges.emplace_back(u, v);
        }
    }

    // Falls m nicht gesetzt ist, berechne es als die Anzahl der Kanten
    if (graphData.m == 0) {
        graphData.m = graphData.edges.size();
    }

    return graphData;
}


std::vector<int> parseNodeWeights(const std::string& weightFilePath) {
    std::ifstream nodeWeightsFile(weightFilePath);
    std::vector<int> node_weights;
    std::string line;

    if (!nodeWeightsFile.is_open()) {
        throw std::runtime_error("Fehler beim Öffnen der Gewichtedatei: " + weightFilePath);
    }
    
    while (std::getline(nodeWeightsFile, line)) {
        std::istringstream lineStream(line);
        int node, weight;
        char comma;
        if (!(lineStream >> node >> comma >> weight)) {
            std::cerr << "Fehler beim Lesen einer Zeile in " << weightFilePath << std::endl;
            return {};
        }
        node_weights.push_back(weight);
    }

    return node_weights;
}

void processGraphsWithWeights(const std::string& inputCsvFilePath, const std::string& networkxDir, const std::string& outputDir) {
    // Durchlaufen der vier Gewichtsordner in `networkx`
    for (const auto& weightDir : std::filesystem::directory_iterator(networkxDir)) {
        if (!weightDir.is_directory()) continue;
        
        std::string weightFolderName = weightDir.path().filename().string();

        // Durchlaufen der 10 Gewichtedateien in jedem Gewichtsordner
        for (const auto& weightFile : std::filesystem::directory_iterator(weightDir.path())) {
            if (weightFile.path().extension() != ".txt") continue;

            std::vector<int> node_weights = parseNodeWeights(weightFile.path().string());
            if (node_weights.empty()) continue;

            std::ifstream inputCsv(inputCsvFilePath);
            if (!inputCsv.is_open()) {
                throw std::runtime_error("Fehler beim Öffnen der Eingabedatei: " + inputCsvFilePath);
            }

            std::string line;
            bool isFirstLine = true;
            while (std::getline(inputCsv, line)) {
                if (isFirstLine) {
                    isFirstLine = false;
                    continue;
                }

                std::istringstream lineStream(line);
                std::string graphFilePath;
                std::getline(lineStream, graphFilePath, ',');

                GraphData graphData = parseGraphFile(graphFilePath);

                // Ausgabe-Dateiname auf Basis von Gewichtedatei und Graph
                std::string weightFileName = weightFile.path().stem().string();  // z.B. "node_weights_1"
                std::string outputFilePath = outputDir + "/" + weightFolderName + "_" + weightFileName + "_results_neighborcover.csv";

                std::ofstream outputCsv(outputFilePath, std::ios::app);
                if (!outputCsv.is_open()) {
                    throw std::runtime_error("Fehler beim Öffnen der Ausgabedatei: " + outputFilePath);
                }

                for (int i = 0; i < 100; ++i) {
                    auto result = NeighborCover(graphData.n, graphData.edges, node_weights);
                    const auto& vcVector = result.second;

                    int weightVC = 0;

                    for (int i = 0; i < vcVector.size(); i++){
                        if ( vcVector[i] == 1){
                            weightVC += node_weights[i];
                        }
                    }

                    outputCsv << graphFilePath << "," << graphData.n << "," << graphData.m;
                    if (graphData.p != -1.0) {
                        outputCsv << "," << graphData.p;
                    } else if (graphData.d != -1) {
                        outputCsv << "," << graphData.d;
                    }

                    outputCsv << "," << weightVC << ',';

                    for (size_t j = 0; j < vcVector.size(); ++j) {
                        if (j > 0) outputCsv << " ";
                        outputCsv << vcVector[j];
                    }
                    outputCsv << "\n";
                }
            }
        }
    }
}




int main() {
    std::string inputCsvFilePath = ""; //Pfad zur graph_info CSV Datei
    std::string weightDir = ""; //Pfad zur Textdatei mit Gewichten (gewicht für knoten i in Zeile i)
    std::string outputDir =""; //Pfad zu Ordner in dem Ergebnis-Datei gespeichert werden soll
    

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
        std::cout << "Ausgabeverzeichnis erstellt: " << outputDir << "\n";
    }

    try {
        processGraphsWithWeights(inputCsvFilePath, weightDir, outputDir);
        std::cout << "Die Verarbeitung der Graphen und das Speichern der Ergebnisse wurde abgeschlossen.\n";
    } catch (const std::exception& e) {
        std::cerr << "Fehler bei der Verarbeitung: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
//  int main(){

//     // Variablen zum Speichern Knoten- und Kantenanzahl
//     int numNodes = 0, numEdges = 0, numOperations = 0;
//     // Kantenstream
//     std::vector<std::tuple<int, int>> Edges;

//     for( int i = 0 ; i<100; i++){
//         std::pair<int, std::vector<int>> ergebnis = NeighborCover(numNodes, Edges, node_weights);
//     }

//     std::vector<int> VCG = GreedyVC(numNodes, Edges);

// }